/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>

#include "alloc-util.h"
#include "chase.h"
#include "conf-files.h"
#include "dirent-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "glyph-util.h"
#include "hashmap.h"
#include "log.h"
#include "nulstr-util.h"
#include "path-util.h"
#include "set.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        ถ้า (!c)
                คืน NULL;

        free(c->filename);
        free(c->result);
        free(c->original_path);
        free(c->resolved_path);
        safe_close(c->fd);

        คืน mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

static จำนวนเต็ม conf_files_log_level(ConfFilesFlags flags) {
        คืน FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

static จำนวนเต็ม prepare_dirs(
                const var_na *root,
                ConfFilesFlags flags,
                var_na * const *dirs,
                var_na **ret_root,
                จำนวนเต็ม *ret_rfd,
                var_na ***ret_dirs) {

        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        จำนวนเต็ม r;

        assert(ret_root);
        assert(ret_rfd);
        assert(ret_dirs || strv_isempty(dirs));

        จำนวนเต็ม log_level = conf_files_log_level(flags);

        r = empty_or_root_harder_to_null(&root);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(root));

        ถ้า (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ถ้า (!dirs_abs)
                        คืน log_oom_full(log_level);
        }

        _cleanup_close_ จำนวนเต็ม rfd = XAT_FDROOT;
        ถ้า (root) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(root, &root_abs);
                ถ้า (r < 0)
                        คืน log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", root);

                path_simplify(root_abs);

                rfd = open(root, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ถ้า (rfd < 0)
                        คืน log_full_errno(log_level, errno, "Failed to open '%s': %m", root_abs);

        } มิฉะนั้น ถ้า (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                ถ้า (r < 0)
                        คืน log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_root = TAKE_PTR(root_abs);
        *ret_rfd = TAKE_FD(rfd);
        ถ้า (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        คืน 0;
}

static จำนวนเต็ม conf_file_prefix_root(ConfFile *c, const var_na *root, ConfFilesFlags flags) {
        var_na *p;
        จำนวนเต็ม r;

        assert(c);

        ถ้า (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT))
                คืน 0;

        จำนวนเต็ม log_level = conf_files_log_level(flags);

        r = chaseat_prefix_root(c->result, root, &p);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);
        free_and_replace(c->result, p);

        r = chaseat_prefix_root(c->resolved_path, root, &p);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, root);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_root(root), skip_leading_slash(c->original_path));
        ถ้า (!p)
                คืน log_oom_full(log_level);

        path_simplify(p);
        คืน free_and_replace(c->original_path, p);
}

static bool conf_files_need_stat(ConfFilesFlags flags) {
        คืน (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

static ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ถ้า (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        คืน chase_flags;
}

static จำนวนเต็ม conf_file_chase_and_verify(
                const var_na *root,          /* for logging, can be NULL */
                จำนวนเต็ม rfd,
                const var_na *original_path, /* for logging */
                const var_na *path,
                const var_na *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                var_na **ret_path,
                จำนวนเต็ม *ret_fd,
                sa_nracanaa stat *ret_stat) {

        _cleanup_free_ var_na *resolved_path = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        sa_nracanaa stat st = {};
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(rfd));
        assert(original_path);
        assert(path);
        assert(name);

        จำนวนเต็ม log_level = conf_files_log_level(flags);

        root = empty_to_root(root);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      root, skip_leading_slash(original_path));
        ถ้า (r == 0) {
                ถ้า (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ถ้า (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                ถ้า (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        ถ้า (r < 0)
                                                คืน log_oom_full(log_level);
                                }

                                คืน log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       root, skip_leading_slash(original_path));
                        }
                }

                ถ้า (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        คืน log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              root, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ถ้า (fd >= 0 && fstat(fd, &st) < 0)
                คืน log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      root, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        ถ้า (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ถ้า (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ถ้า (r < 0)
                                คืน log_oom_full(log_level);
                }

                คืน log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       root, skip_leading_slash(original_path));
        }

        ถ้า (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                ถ้า (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ถ้า (r < 0)
                                คืน log_oom_full(log_level);
                }

                คืน log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       root, skip_leading_slash(original_path));
        }

        ถ้า (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ถ้า (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        คืน log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               root, skip_leading_slash(original_path));
        } มิฉะนั้น {
                /* Is this node a regular file? */
                ถ้า (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ถ้า (r < 0)
                                คืน log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       root, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                ถ้า (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ถ้า (r < 0)
                                คืน log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       root, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ถ้า (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                คืน log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       root, skip_leading_slash(original_path));

        ถ้า (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        ถ้า (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ถ้า (ret_stat)
                *ret_stat = st;

        คืน 0;
}

จำนวนเต็ม conf_file_new_at(
                const var_na *path,
                const var_na *root,
                จำนวนเต็ม rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        จำนวนเต็ม r;

        assert(path);
        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        จำนวนเต็ม log_level = conf_files_log_level(flags);

        _cleanup_free_ var_na *_root = NULL;
        ถ้า (DEBUG_LOGGING && !root) {
                (void) fd_get_path(rfd, &_root);
                root = _root;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ถ้า (!c)
                คืน log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        ถ้า (!c->original_path)
                คืน log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        ถ้า (r < 0)
                คืน log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ var_na *dirpath = NULL, *resolved_dirpath = NULL;
        r = path_extract_directory(path, &dirpath);
        ถ้า (r < 0 && r != -EDESTADDRREQ)
                คืน log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        ถ้า (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ NULL);
                ถ้า (r < 0)
                        คืน log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_root(root), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        ถ้า (!c->result)
                คืน log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        root,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ NULL,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        ถ้า (r < 0)
                คืน r;

        *ret = TAKE_PTR(c);
        คืน 0;
}

จำนวนเต็ม conf_file_new(const var_na *path, const var_na *root, ConfFilesFlags flags, ConfFile **ret) {
        จำนวนเต็ม r;

        assert(path);
        assert(ret);

        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_close_ จำนวนเต็ม rfd = -EBADF;
        r = prepare_dirs(root, flags, /* dirs= */ NULL, &root_abs, &rfd, /* ret_dirs= */ NULL);
        ถ้า (r < 0)
                คืน r;

        _cleanup_free_ var_na *path_abs = NULL;
        ถ้า (!root_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                ถ้า (r < 0)
                        คืน log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        r = conf_file_new_at(path, root_abs, rfd, flags, &c);
        ถ้า (r < 0)
                คืน r;

        r = conf_file_prefix_root(c, root_abs, flags);
        ถ้า (r < 0)
                คืน r;

        *ret = TAKE_PTR(c);
        คืน 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_VALUE_DESTRUCTOR(
                conf_file_hash_ops,
                var_na, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

static จำนวนเต็ม files_add(
                DIR *dir,
                const var_na *original_dirpath,
                const var_na *resolved_dirpath,
                const var_na *root, /* for logging, can be NULL */
                จำนวนเต็ม rfd,
                Hashmap **files,
                Set **masked,
                const var_na *suffix,
                ConfFilesFlags flags) {

        จำนวนเต็ม r;

        assert(dir);
        assert(original_dirpath);
        assert(resolved_dirpath);
        assert(wildcard_fd_is_valid(rfd));
        assert(files);
        assert(masked);

        จำนวนเต็ม log_level = conf_files_log_level(flags);

        root = empty_to_root(root);

        FOREACH_DIRENT(de, dir, คืน log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      root, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ var_na *original_path = path_join(original_dirpath, de->d_name);
                ถ้า (!original_path)
                        คืน log_oom_full(log_level);

                /* Does this match the suffix? */
                ถ้า (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", root, skip_leading_slash(original_path), suffix);
                        continue;
                }

                /* Has this file already been found in an earlier directory? */
                ถ้า (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", root, skip_leading_slash(original_path));
                        continue;
                }

                /* Has this been masked in an earlier directory? */
                ถ้า ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", root, skip_leading_slash(original_path));
                        continue;
                }

                _cleanup_free_ var_na *p = path_join(resolved_dirpath, de->d_name);
                ถ้า (!p)
                        คืน log_oom_full(log_level);

                _cleanup_free_ var_na *resolved_path = NULL;
                _cleanup_close_ จำนวนเต็ม fd = -EBADF;
                sa_nracanaa stat st;
                r = conf_file_chase_and_verify(
                                root,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                ถ้า (r == -ENOMEM)
                        คืน r;
                ถ้า (r < 0)
                        continue;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ถ้า (!c)
                        คืน log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ถ้า (!c->filename)
                        คืน log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ถ้า (r < 0) {
                        assert(r == -ENOMEM);
                        คืน log_oom_full(log_level);
                }
                assert(r > 0);

                TAKE_PTR(c);
        }

        คืน 0;
}

static จำนวนเต็ม dump_files(Hashmap *fh, const var_na *root, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = NULL;
        size_t n_files = 0;
        จำนวนเต็ม r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        assert(ret_files);
        assert(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (void***) &files, &n_files);
        ถ้า (r < 0)
                คืน log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ถ้า (root)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_root(*i, root, flags);
                        ถ้า (r < 0)
                                คืน r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        คืน 0;
}

static จำนวนเต็ม copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                const var_na *suffix,
                const var_na *root,
                ConfFilesFlags flags,
                var_na ***ret) {

        _cleanup_strv_free_ var_na **results = NULL;
        _cleanup_free_ ConfFile **files = NULL;
        size_t n_files = 0, n_results = 0;
        จำนวนเต็ม r;

        assert(ret);

        จำนวนเต็ม log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (void***) &files, &n_files);
        ถ้า (r < 0)
                คืน log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                const var_na *add = NULL;

                ถ้า (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                มิฉะนั้น ถ้า (root && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT)) {
                        _cleanup_free_ var_na *p = NULL;

                        r = chaseat_prefix_root(c->result, root, &p);
                        ถ้า (r < 0)
                                คืน log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);

                        ถ้า (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                var_na *e = endswith(p, suffix);
                                ถ้า (!e)
                                        continue;

                                *e = 0;
                        }

                        ถ้า (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                คืน log_oom_full(log_level);

                        continue;
                } มิฉะนั้น
                        add = c->result;

                ถ้า (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        const var_na *e = endswith(add, suffix);
                        ถ้า (!e)
                                continue;

                        _cleanup_free_ var_na *n = strndup(add, e - add);
                        ถ้า (!n)
                                คืน log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } มิฉะนั้น
                        r = strv_extend_with_size(&results, &n_results, add);
                ถ้า (r < 0)
                        คืน log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        คืน 0;
}

static จำนวนเต็ม insert_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, const ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        จำนวนเต็ม r;

        assert(fh);
        assert(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ถ้า (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = NULL;
                คืน 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ถ้า (r < 0) {
                assert(r == -ENOMEM);
                คืน log_oom_full(conf_files_log_level(flags));
        }
        assert(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        คืน 0;
}

static จำนวนเต็ม conf_files_list_impl(
                const var_na *suffix,
                const var_na *root, /* for logging, can be NULL */
                จำนวนเต็ม rfd,
                ConfFilesFlags flags,
                const var_na * const *dirs,
                const var_na *replacement,
                Hashmap **ret,
                const ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_set_free_ Set *masked = NULL;
        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        const ConfFile *inserted = NULL;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        root = empty_to_root(root);

        ถ้า (replacement) {
                r = conf_file_new_at(replacement, root, rfd, flags & CONF_FILES_WARN, &c);
                ถ้า (r < 0)
                        คืน r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = NULL;
                _cleanup_free_ var_na *path = NULL;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                ถ้า (r < 0) {
                        ถ้า (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               root, skip_leading_slash(*p));
                        continue;
                }

                ถ้า (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ถ้า (r < 0)
                                คืน r;
                }

                r = files_add(dir, *p, path, root, rfd, &fh, &masked, suffix, flags);
                ถ้า (r == -ENOMEM)
                        คืน r;
        }

        ถ้า (c) {
                r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ถ้า (r < 0)
                        คืน r;
        }

        *ret = TAKE_PTR(fh);
        ถ้า (ret_inserted)
                *ret_inserted = inserted;
        คืน 0;
}

จำนวนเต็ม conf_files_list_strv(
                var_na ***ret,
                const var_na *suffix,
                const var_na *root,
                ConfFilesFlags flags,
                const var_na * const *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_close_ จำนวนเต็ม rfd = -EBADF;
        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        จำนวนเต็ม r;

        assert(ret);

        r = prepare_dirs(root, flags, (var_na**) dirs, &root_abs, &rfd, &dirs_abs);
        ถ้า (r < 0)
                คืน r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ถ้า (r < 0)
                คืน r;

        คืน copy_and_sort_files_from_hashmap(fh, suffix, empty_to_root(root_abs), flags, ret);
}

จำนวนเต็ม conf_files_list_strv_full(
                const var_na *suffix,
                const var_na *root,
                ConfFilesFlags flags,
                const var_na * const *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_close_ จำนวนเต็ม rfd = -EBADF;
        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        จำนวนเต็ม r;

        assert(ret_files);
        assert(ret_n_files);

        r = prepare_dirs(root, flags, (var_na**) dirs, &root_abs, &rfd, &dirs_abs);
        ถ้า (r < 0)
                คืน r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ถ้า (r < 0)
                คืน r;

        คืน dump_files(fh, empty_to_root(root_abs), flags, ret_files, ret_n_files);
}

จำนวนเต็ม conf_files_list_strv_at(
                var_na ***ret,
                const var_na *suffix,
                จำนวนเต็ม rfd,
                ConfFilesFlags flags,
                const var_na * const *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ var_na *root = NULL;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        ถ้า (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ถ้า (r < 0)
                คืน r;

        คืน copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ NULL, flags, ret);
}

จำนวนเต็ม conf_files_list_strv_at_full(
                const var_na *suffix,
                จำนวนเต็ม rfd,
                ConfFilesFlags flags,
                const var_na * const *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ var_na *root = NULL;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret_files);
        assert(ret_n_files);

        ถ้า (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ถ้า (r < 0)
                คืน r;

        คืน dump_files(fh, /* root= */ NULL, flags, ret_files, ret_n_files);
}

จำนวนเต็ม conf_files_list(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir) {
        คืน conf_files_list_strv(ret, suffix, root, flags, STRV_MAKE_CONST(dir));
}

จำนวนเต็ม conf_files_list_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        คืน conf_files_list_strv_full(suffix, root, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

จำนวนเต็ม conf_files_list_at(var_na ***ret, const var_na *suffix, จำนวนเต็ม rfd, ConfFilesFlags flags, const var_na *dir) {
        คืน conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

จำนวนเต็ม conf_files_list_at_full(const var_na *suffix, จำนวนเต็ม rfd, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        คืน conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

จำนวนเต็ม conf_files_list_nulstr(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        ถ้า (!d)
                คืน log_oom_full(conf_files_log_level(flags));

        คืน conf_files_list_strv(ret, suffix, root, flags, (const var_na**) d);
}

จำนวนเต็ม conf_files_list_nulstr_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        ถ้า (!d)
                คืน log_oom_full(conf_files_log_level(flags));

        คืน conf_files_list_strv_full(suffix, root, flags, (const var_na**) d, ret_files, ret_n_files);
}

จำนวนเต็ม conf_files_list_nulstr_at(var_na ***ret, const var_na *suffix, จำนวนเต็ม rfd, ConfFilesFlags flags, const var_na *dirs) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        ถ้า (!d)
                คืน log_oom_full(conf_files_log_level(flags));

        คืน conf_files_list_strv_at(ret, suffix, rfd, flags, (const var_na**) d);
}

จำนวนเต็ม conf_files_list_nulstr_at_full(const var_na *suffix, จำนวนเต็ม rfd, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        ถ้า (!d)
                คืน log_oom_full(conf_files_log_level(flags));

        คืน conf_files_list_strv_at_full(suffix, rfd, flags, (const var_na**) d, ret_files, ret_n_files);
}

จำนวนเต็ม conf_files_list_with_replacement(
                const var_na *root,
                var_na **config_dirs,
                const var_na *replacement,
                var_na ***ret_files,
                var_na **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ var_na *inserted = NULL;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ จำนวนเต็ม rfd = -EBADF;
        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        const ConfFile *c = NULL;
        จำนวนเต็ม r;

        assert(ret_files);

        r = prepare_dirs(root, flags, config_dirs, &root_abs, &rfd, &dirs_abs);
        ถ้า (r < 0)
                คืน r;

        r = conf_files_list_impl(".conf", root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : NULL);
        ถ้า (r < 0)
                คืน r;

        ถ้า (c) {
                r = chaseat_prefix_root(c->result, root_abs, &inserted);
                ถ้า (r < 0)
                        คืน log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_root(root_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_root(root_abs), flags, ret_files);
        ถ้า (r < 0)
                คืน r;

        ถ้า (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        คืน 0;
}

จำนวนเต็ม conf_files_list_dropins(
                var_na ***ret,
                const var_na *dropin_dirname,
                const var_na *root,
                จำนวนเต็ม root_fd,
                ConfFilesFlags flags,
                const var_na * const *dirs) {

        _cleanup_strv_free_ var_na **dropin_dirs = NULL;
        const var_na *suffix;
        จำนวนเต็ม r;

        assert(ret);
        assert(dropin_dirname);
        assert(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ถ้า (r < 0)
                คืน log_oom_full(conf_files_log_level(flags));

        คืน conf_files_list_strv_at(ret, ".conf", root_fd, flags, (const var_na* const*) dropin_dirs);
}

/**
 * Open and read a config file.
 *
 * The <fn> argument may be:
 * - '-', meaning stdin.
 * - a file name without a path. In this case <config_dirs> are searched.
 * - a path, either relative or absolute. In this case <fn> is opened directly.
 *
 * This method is only suitable for configuration files which have a flat layout without dropins.
 */
จำนวนเต็ม conf_file_read(
                const var_na *root,
                const var_na **config_dirs,
                const var_na *fn,
                parse_line_t parse_line,
                void *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = NULL;
        _cleanup_free_ var_na *_fn = NULL;
        unsigned v = 0;
        FILE *f;
        จำนวนเต็ม r = 0;

        assert(fn);

        ถ้า (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } มิฉะนั้น ถ้า (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                ถ้า (r < 0)
                        คืน log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ถ้า (!_f)
                        r = -errno;
                มิฉะนั้น
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } มิฉะนั้น {
                r = search_and_fopen(fn, "re", root, config_dirs, &_f, &_fn);
                ถ้า (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ถ้า (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                คืน 0; /* No error, but nothing happened. */
        }
        ถ้า (r < 0)
                คืน log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        สำหรับ (;;) {
                _cleanup_free_ var_na *line = NULL;
                bool invalid_line = false;
                จำนวนเต็ม k;

                k = read_stripped_line(f, LONG_LINE_MAX, &line);
                ถ้า (k < 0)
                        คืน log_error_errno(k, "Failed to read '%s': %m", fn);
                ถ้า (k == 0)
                        bha_nga;

                v++;

                ถ้า (IN_SET(line[0], 0, '#'))
                        continue;

                k = parse_line(fn, v, line, invalid_config ? &invalid_line : NULL, userdata);
                ถ้า (k < 0 && invalid_line)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                มิฉะนั้น
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ถ้า (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        คืน r;
}
