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
        якщо (!c)
                повернути NULL;

        free(c->filename);
        free(c->result);
        free(c->original_path);
        free(c->resolved_path);
        safe_close(c->fd);

        повернути mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

static ціле conf_files_log_level(ConfFilesFlags flags) {
        повернути FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

static ціле prepare_dirs(
                const char *root,
                ConfFilesFlags flags,
                char * const *dirs,
                char **ret_root,
                ціле *ret_rfd,
                char ***ret_dirs) {

        _cleanup_free_ char *root_abs = NULL;
        _cleanup_strv_free_ char **dirs_abs = NULL;
        ціле r;

        assert(ret_root);
        assert(ret_rfd);
        assert(ret_dirs || strv_isempty(dirs));

        ціле log_level = conf_files_log_level(flags);

        r = empty_or_root_harder_to_null(&root);
        якщо (r < 0)
                повернути log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(root));

        якщо (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                якщо (!dirs_abs)
                        повернути log_oom_full(log_level);
        }

        _cleanup_close_ ціле rfd = XAT_FDROOT;
        якщо (root) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(root, &root_abs);
                якщо (r < 0)
                        повернути log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", root);

                path_simplify(root_abs);

                rfd = open(root, O_CLOEXEC|O_DIRECTORY|O_PATH);
                якщо (rfd < 0)
                        повернути log_full_errno(log_level, errno, "Failed to open '%s': %m", root_abs);

        } інакше якщо (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                якщо (r < 0)
                        повернути log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_root = TAKE_PTR(root_abs);
        *ret_rfd = TAKE_FD(rfd);
        якщо (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        повернути 0;
}

static ціле conf_file_prefix_root(ConfFile *c, const char *root, ConfFilesFlags flags) {
        char *p;
        ціле r;

        assert(c);

        якщо (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT))
                повернути 0;

        ціле log_level = conf_files_log_level(flags);

        r = chaseat_prefix_root(c->result, root, &p);
        якщо (r < 0)
                повернути log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);
        free_and_replace(c->result, p);

        r = chaseat_prefix_root(c->resolved_path, root, &p);
        якщо (r < 0)
                повернути log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, root);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_root(root), skip_leading_slash(c->original_path));
        якщо (!p)
                повернути log_oom_full(log_level);

        path_simplify(p);
        повернути free_and_replace(c->original_path, p);
}

static bool conf_files_need_stat(ConfFilesFlags flags) {
        повернути (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

static ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        якщо (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        повернути chase_flags;
}

static ціле conf_file_chase_and_verify(
                const char *root,          /* for logging, can be NULL */
                ціле rfd,
                const char *original_path, /* for logging */
                const char *path,
                const char *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                char **ret_path,
                ціле *ret_fd,
                struct stat *ret_stat) {

        _cleanup_free_ char *resolved_path = NULL;
        _cleanup_close_ ціле fd = -EBADF;
        struct stat st = {};
        ціле r;

        assert(wildcard_fd_is_valid(rfd));
        assert(original_path);
        assert(path);
        assert(name);

        ціле log_level = conf_files_log_level(flags);

        root = empty_to_root(root);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        якщо (r < 0)
                повернути log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      root, skip_leading_slash(original_path));
        якщо (r == 0) {
                якщо (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        якщо (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                якщо (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        якщо (r < 0)
                                                повернути log_oom_full(log_level);
                                }

                                повернути log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       root, skip_leading_slash(original_path));
                        }
                }

                якщо (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        повернути log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              root, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        якщо (fd >= 0 && fstat(fd, &st) < 0)
                повернути log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      root, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        якщо (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                якщо (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        якщо (r < 0)
                                повернути log_oom_full(log_level);
                }

                повернути log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       root, skip_leading_slash(original_path));
        }

        якщо (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                якщо (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        якщо (r < 0)
                                повернути log_oom_full(log_level);
                }

                повернути log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       root, skip_leading_slash(original_path));
        }

        якщо (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                якщо (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        повернути log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               root, skip_leading_slash(original_path));
        } інакше {
                /* Is this node a regular file? */
                якщо (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        якщо (r < 0)
                                повернути log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       root, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                якщо (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        якщо (r < 0)
                                повернути log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       root, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        якщо (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                повернути log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       root, skip_leading_slash(original_path));

        якщо (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        якщо (ret_fd)
                *ret_fd = TAKE_FD(fd);
        якщо (ret_stat)
                *ret_stat = st;

        повернути 0;
}

ціле conf_file_new_at(
                const char *path,
                const char *root,
                ціле rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        ціле r;

        assert(path);
        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        ціле log_level = conf_files_log_level(flags);

        _cleanup_free_ char *_root = NULL;
        якщо (DEBUG_LOGGING && !root) {
                (void) fd_get_path(rfd, &_root);
                root = _root;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        якщо (!c)
                повернути log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        якщо (!c->original_path)
                повернути log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        якщо (r < 0)
                повернути log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ char *dirpath = NULL, *resolved_dirpath = NULL;
        r = path_extract_directory(path, &dirpath);
        якщо (r < 0 && r != -EDESTADDRREQ)
                повернути log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        якщо (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ NULL);
                якщо (r < 0)
                        повернути log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_root(root), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        якщо (!c->result)
                повернути log_oom_full(log_level);

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
        якщо (r < 0)
                повернути r;

        *ret = TAKE_PTR(c);
        повернути 0;
}

ціле conf_file_new(const char *path, const char *root, ConfFilesFlags flags, ConfFile **ret) {
        ціле r;

        assert(path);
        assert(ret);

        _cleanup_free_ char *root_abs = NULL;
        _cleanup_close_ ціле rfd = -EBADF;
        r = prepare_dirs(root, flags, /* dirs= */ NULL, &root_abs, &rfd, /* ret_dirs= */ NULL);
        якщо (r < 0)
                повернути r;

        _cleanup_free_ char *path_abs = NULL;
        якщо (!root_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                якщо (r < 0)
                        повернути log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        r = conf_file_new_at(path, root_abs, rfd, flags, &c);
        якщо (r < 0)
                повернути r;

        r = conf_file_prefix_root(c, root_abs, flags);
        якщо (r < 0)
                повернути r;

        *ret = TAKE_PTR(c);
        повернути 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_VALUE_DESTRUCTOR(
                conf_file_hash_ops,
                char, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

static ціле files_add(
                DIR *dir,
                const char *original_dirpath,
                const char *resolved_dirpath,
                const char *root, /* for logging, can be NULL */
                ціле rfd,
                Hashmap **files,
                Set **masked,
                const char *suffix,
                ConfFilesFlags flags) {

        ціле r;

        assert(dir);
        assert(original_dirpath);
        assert(resolved_dirpath);
        assert(wildcard_fd_is_valid(rfd));
        assert(files);
        assert(masked);

        ціле log_level = conf_files_log_level(flags);

        root = empty_to_root(root);

        FOREACH_DIRENT(de, dir, повернути log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      root, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ char *original_path = path_join(original_dirpath, de->d_name);
                якщо (!original_path)
                        повернути log_oom_full(log_level);

                /* Does this match the suffix? */
                якщо (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", root, skip_leading_slash(original_path), suffix);
                        continue;
                }

                /* Has this file already been found in an earlier directory? */
                якщо (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", root, skip_leading_slash(original_path));
                        continue;
                }

                /* Has this been masked in an earlier directory? */
                якщо ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", root, skip_leading_slash(original_path));
                        continue;
                }

                _cleanup_free_ char *p = path_join(resolved_dirpath, de->d_name);
                якщо (!p)
                        повернути log_oom_full(log_level);

                _cleanup_free_ char *resolved_path = NULL;
                _cleanup_close_ ціле fd = -EBADF;
                struct stat st;
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
                якщо (r == -ENOMEM)
                        повернути r;
                якщо (r < 0)
                        continue;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                якщо (!c)
                        повернути log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                якщо (!c->filename)
                        повернути log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                якщо (r < 0) {
                        assert(r == -ENOMEM);
                        повернути log_oom_full(log_level);
                }
                assert(r > 0);

                TAKE_PTR(c);
        }

        повернути 0;
}

static ціле dump_files(Hashmap *fh, const char *root, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = NULL;
        size_t n_files = 0;
        ціле r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        assert(ret_files);
        assert(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (void***) &files, &n_files);
        якщо (r < 0)
                повернути log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        якщо (root)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_root(*i, root, flags);
                        якщо (r < 0)
                                повернути r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        повернути 0;
}

static ціле copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                const char *suffix,
                const char *root,
                ConfFilesFlags flags,
                char ***ret) {

        _cleanup_strv_free_ char **results = NULL;
        _cleanup_free_ ConfFile **files = NULL;
        size_t n_files = 0, n_results = 0;
        ціле r;

        assert(ret);

        ціле log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (void***) &files, &n_files);
        якщо (r < 0)
                повернути log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                const char *add = NULL;

                якщо (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                інакше якщо (root && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT)) {
                        _cleanup_free_ char *p = NULL;

                        r = chaseat_prefix_root(c->result, root, &p);
                        якщо (r < 0)
                                повернути log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);

                        якщо (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                char *e = endswith(p, suffix);
                                якщо (!e)
                                        continue;

                                *e = 0;
                        }

                        якщо (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                повернути log_oom_full(log_level);

                        continue;
                } інакше
                        add = c->result;

                якщо (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        const char *e = endswith(add, suffix);
                        якщо (!e)
                                continue;

                        _cleanup_free_ char *n = strndup(add, e - add);
                        якщо (!n)
                                повернути log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } інакше
                        r = strv_extend_with_size(&results, &n_results, add);
                якщо (r < 0)
                        повернути log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        повернути 0;
}

static ціле insert_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, const ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        ціле r;

        assert(fh);
        assert(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        якщо (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = NULL;
                повернути 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        якщо (r < 0) {
                assert(r == -ENOMEM);
                повернути log_oom_full(conf_files_log_level(flags));
        }
        assert(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        повернути 0;
}

static ціле conf_files_list_impl(
                const char *suffix,
                const char *root, /* for logging, can be NULL */
                ціле rfd,
                ConfFilesFlags flags,
                const char * const *dirs,
                const char *replacement,
                Hashmap **ret,
                const ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_set_free_ Set *masked = NULL;
        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        const ConfFile *inserted = NULL;
        ціле r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        root = empty_to_root(root);

        якщо (replacement) {
                r = conf_file_new_at(replacement, root, rfd, flags & CONF_FILES_WARN, &c);
                якщо (r < 0)
                        повернути r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = NULL;
                _cleanup_free_ char *path = NULL;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                якщо (r < 0) {
                        якщо (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               root, skip_leading_slash(*p));
                        continue;
                }

                якщо (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        якщо (r < 0)
                                повернути r;
                }

                r = files_add(dir, *p, path, root, rfd, &fh, &masked, suffix, flags);
                якщо (r == -ENOMEM)
                        повернути r;
        }

        якщо (c) {
                r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                якщо (r < 0)
                        повернути r;
        }

        *ret = TAKE_PTR(fh);
        якщо (ret_inserted)
                *ret_inserted = inserted;
        повернути 0;
}

ціле conf_files_list_strv(
                char ***ret,
                const char *suffix,
                const char *root,
                ConfFilesFlags flags,
                const char * const *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_close_ ціле rfd = -EBADF;
        _cleanup_free_ char *root_abs = NULL;
        _cleanup_strv_free_ char **dirs_abs = NULL;
        ціле r;

        assert(ret);

        r = prepare_dirs(root, flags, (char**) dirs, &root_abs, &rfd, &dirs_abs);
        якщо (r < 0)
                повернути r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const char * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        якщо (r < 0)
                повернути r;

        повернути copy_and_sort_files_from_hashmap(fh, suffix, empty_to_root(root_abs), flags, ret);
}

ціле conf_files_list_strv_full(
                const char *suffix,
                const char *root,
                ConfFilesFlags flags,
                const char * const *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_close_ ціле rfd = -EBADF;
        _cleanup_free_ char *root_abs = NULL;
        _cleanup_strv_free_ char **dirs_abs = NULL;
        ціле r;

        assert(ret_files);
        assert(ret_n_files);

        r = prepare_dirs(root, flags, (char**) dirs, &root_abs, &rfd, &dirs_abs);
        якщо (r < 0)
                повернути r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const char * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        якщо (r < 0)
                повернути r;

        повернути dump_files(fh, empty_to_root(root_abs), flags, ret_files, ret_n_files);
}

ціле conf_files_list_strv_at(
                char ***ret,
                const char *suffix,
                ціле rfd,
                ConfFilesFlags flags,
                const char * const *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ char *root = NULL;
        ціле r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        якщо (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        якщо (r < 0)
                повернути r;

        повернути copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ NULL, flags, ret);
}

ціле conf_files_list_strv_at_full(
                const char *suffix,
                ціле rfd,
                ConfFilesFlags flags,
                const char * const *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ char *root = NULL;
        ціле r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret_files);
        assert(ret_n_files);

        якщо (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        якщо (r < 0)
                повернути r;

        повернути dump_files(fh, /* root= */ NULL, flags, ret_files, ret_n_files);
}

ціле conf_files_list(char ***ret, const char *suffix, const char *root, ConfFilesFlags flags, const char *dir) {
        повернути conf_files_list_strv(ret, suffix, root, flags, STRV_MAKE_CONST(dir));
}

ціле conf_files_list_full(const char *suffix, const char *root, ConfFilesFlags flags, const char *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        повернути conf_files_list_strv_full(suffix, root, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ціле conf_files_list_at(char ***ret, const char *suffix, ціле rfd, ConfFilesFlags flags, const char *dir) {
        повернути conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

ціле conf_files_list_at_full(const char *suffix, ціле rfd, ConfFilesFlags flags, const char *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        повернути conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ціле conf_files_list_nulstr(char ***ret, const char *suffix, const char *root, ConfFilesFlags flags, const char *dirs) {
        _cleanup_strv_free_ char **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        якщо (!d)
                повернути log_oom_full(conf_files_log_level(flags));

        повернути conf_files_list_strv(ret, suffix, root, flags, (const char**) d);
}

ціле conf_files_list_nulstr_full(const char *suffix, const char *root, ConfFilesFlags flags, const char *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ char **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        якщо (!d)
                повернути log_oom_full(conf_files_log_level(flags));

        повернути conf_files_list_strv_full(suffix, root, flags, (const char**) d, ret_files, ret_n_files);
}

ціле conf_files_list_nulstr_at(char ***ret, const char *suffix, ціле rfd, ConfFilesFlags flags, const char *dirs) {
        _cleanup_strv_free_ char **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        якщо (!d)
                повернути log_oom_full(conf_files_log_level(flags));

        повернути conf_files_list_strv_at(ret, suffix, rfd, flags, (const char**) d);
}

ціле conf_files_list_nulstr_at_full(const char *suffix, ціле rfd, ConfFilesFlags flags, const char *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ char **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        якщо (!d)
                повернути log_oom_full(conf_files_log_level(flags));

        повернути conf_files_list_strv_at_full(suffix, rfd, flags, (const char**) d, ret_files, ret_n_files);
}

ціле conf_files_list_with_replacement(
                const char *root,
                char **config_dirs,
                const char *replacement,
                char ***ret_files,
                char **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ char *inserted = NULL;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ ціле rfd = -EBADF;
        _cleanup_free_ char *root_abs = NULL;
        _cleanup_strv_free_ char **dirs_abs = NULL;
        const ConfFile *c = NULL;
        ціле r;

        assert(ret_files);

        r = prepare_dirs(root, flags, config_dirs, &root_abs, &rfd, &dirs_abs);
        якщо (r < 0)
                повернути r;

        r = conf_files_list_impl(".conf", root_abs, rfd, flags, (const char * const *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : NULL);
        якщо (r < 0)
                повернути r;

        якщо (c) {
                r = chaseat_prefix_root(c->result, root_abs, &inserted);
                якщо (r < 0)
                        повернути log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_root(root_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_root(root_abs), flags, ret_files);
        якщо (r < 0)
                повернути r;

        якщо (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        повернути 0;
}

ціле conf_files_list_dropins(
                char ***ret,
                const char *dropin_dirname,
                const char *root,
                ціле root_fd,
                ConfFilesFlags flags,
                const char * const *dirs) {

        _cleanup_strv_free_ char **dropin_dirs = NULL;
        const char *suffix;
        ціле r;

        assert(ret);
        assert(dropin_dirname);
        assert(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        якщо (r < 0)
                повернути log_oom_full(conf_files_log_level(flags));

        повернути conf_files_list_strv_at(ret, ".conf", root_fd, flags, (const char* const*) dropin_dirs);
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
ціле conf_file_read(
                const char *root,
                const char **config_dirs,
                const char *fn,
                parse_line_t parse_line,
                void *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = NULL;
        _cleanup_free_ char *_fn = NULL;
        unsigned v = 0;
        FILE *f;
        ціле r = 0;

        assert(fn);

        якщо (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } інакше якщо (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                якщо (r < 0)
                        повернути log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                якщо (!_f)
                        r = -errno;
                інакше
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } інакше {
                r = search_and_fopen(fn, "re", root, config_dirs, &_f, &_fn);
                якщо (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        якщо (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                повернути 0; /* No error, but nothing happened. */
        }
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        для (;;) {
                _cleanup_free_ char *line = NULL;
                bool invalid_line = false;
                ціле k;

                k = read_stripped_line(f, LONG_LINE_MAX, &line);
                якщо (k < 0)
                        повернути log_error_errno(k, "Failed to read '%s': %m", fn);
                якщо (k == 0)
                        break;

                v++;

                якщо (IN_SET(line[0], 0, '#'))
                        continue;

                k = parse_line(fn, v, line, invalid_config ? &invalid_line : NULL, userdata);
                якщо (k < 0 && invalid_line)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                інакше
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        якщо (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        повернути r;
}
