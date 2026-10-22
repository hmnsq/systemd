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
        ᯀᯮᯝ (!c)
                ᯀᯮᯞ NULL;

        free(c->filename);
        free(c->result);
        free(c->original_path);
        free(c->resolved_path);
        safe_close(c->fd);

        ᯀᯮᯞ mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

static puur_naa_nka conf_files_log_level(ConfFilesFlags flags) {
        ᯀᯮᯞ FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

static puur_naa_nka prepare_dirs(
                const var_na *root,
                ConfFilesFlags flags,
                var_na * const *dirs,
                var_na **ret_root,
                puur_naa_nka *ret_rfd,
                var_na ***ret_dirs) {

        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        puur_naa_nka r;

        assert(ret_root);
        assert(ret_rfd);
        assert(ret_dirs || strv_isempty(dirs));

        puur_naa_nka log_level = conf_files_log_level(flags);

        r = empty_or_root_harder_to_null(&root);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(root));

        ᯀᯮᯝ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ᯀᯮᯝ (!dirs_abs)
                        ᯀᯮᯞ log_oom_full(log_level);
        }

        _cleanup_close_ puur_naa_nka rfd = XAT_FDROOT;
        ᯀᯮᯝ (root) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(root, &root_abs);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", root);

                path_simplify(root_abs);

                rfd = open(root, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ᯀᯮᯝ (rfd < 0)
                        ᯀᯮᯞ log_full_errno(log_level, errno, "Failed to open '%s': %m", root_abs);

        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_root = TAKE_PTR(root_abs);
        *ret_rfd = TAKE_FD(rfd);
        ᯀᯮᯝ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ᯀᯮᯞ 0;
}

static puur_naa_nka conf_file_prefix_root(ConfFile *c, const var_na *root, ConfFilesFlags flags) {
        var_na *p;
        puur_naa_nka r;

        assert(c);

        ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT))
                ᯀᯮᯞ 0;

        puur_naa_nka log_level = conf_files_log_level(flags);

        r = chaseat_prefix_root(c->result, root, &p);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);
        free_and_replace(c->result, p);

        r = chaseat_prefix_root(c->resolved_path, root, &p);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, root);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_root(root), skip_leading_slash(c->original_path));
        ᯀᯮᯝ (!p)
                ᯀᯮᯞ log_oom_full(log_level);

        path_simplify(p);
        ᯀᯮᯞ free_and_replace(c->original_path, p);
}

static bool conf_files_need_stat(ConfFilesFlags flags) {
        ᯀᯮᯞ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

static ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ᯀᯮᯝ (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ᯀᯮᯞ chase_flags;
}

static puur_naa_nka conf_file_chase_and_verify(
                const var_na *root,          /* for logging, can be NULL */
                puur_naa_nka rfd,
                const var_na *original_path, /* for logging */
                const var_na *path,
                const var_na *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                var_na **ret_path,
                puur_naa_nka *ret_fd,
                sa_nracanaa stat *ret_stat) {

        _cleanup_free_ var_na *resolved_path = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        sa_nracanaa stat st = {};
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));
        assert(original_path);
        assert(path);
        assert(name);

        puur_naa_nka log_level = conf_files_log_level(flags);

        root = empty_to_root(root);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      root, skip_leading_slash(original_path));
        ᯀᯮᯝ (r == 0) {
                ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ᯀᯮᯝ (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                ᯀᯮᯝ (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        ᯀᯮᯝ (r < 0)
                                                ᯀᯮᯞ log_oom_full(log_level);
                                }

                                ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       root, skip_leading_slash(original_path));
                        }
                }

                ᯀᯮᯝ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ᯀᯮᯞ log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              root, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ᯀᯮᯝ (fd >= 0 && fstat(fd, &st) < 0)
                ᯀᯮᯞ log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      root, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ᯀᯮᯝ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ log_oom_full(log_level);
                }

                ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       root, skip_leading_slash(original_path));
        }

        ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                ᯀᯮᯝ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ log_oom_full(log_level);
                }

                ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       root, skip_leading_slash(original_path));
        }

        ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ᯀᯮᯝ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               root, skip_leading_slash(original_path));
        } ᯀᯮᯝᯂᯪ {
                /* Is this node a regular file? */
                ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       root, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       root, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ᯀᯮᯞ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       root, skip_leading_slash(original_path));

        ᯀᯮᯝ (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        ᯀᯮᯝ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ᯀᯮᯝ (ret_stat)
                *ret_stat = st;

        ᯀᯮᯞ 0;
}

puur_naa_nka conf_file_new_at(
                const var_na *path,
                const var_na *root,
                puur_naa_nka rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        puur_naa_nka r;

        assert(path);
        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        puur_naa_nka log_level = conf_files_log_level(flags);

        _cleanup_free_ var_na *_root = NULL;
        ᯀᯮᯝ (DEBUG_LOGGING && !root) {
                (void) fd_get_path(rfd, &_root);
                root = _root;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ᯀᯮᯝ (!c)
                ᯀᯮᯞ log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        ᯀᯮᯝ (!c->original_path)
                ᯀᯮᯞ log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ var_na *dirpath = NULL, *resolved_dirpath = NULL;
        r = path_extract_directory(path, &dirpath);
        ᯀᯮᯝ (r < 0 && r != -EDESTADDRREQ)
                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        ᯀᯮᯝ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ NULL);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_root(root), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        ᯀᯮᯝ (!c->result)
                ᯀᯮᯞ log_oom_full(log_level);

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
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        *ret = TAKE_PTR(c);
        ᯀᯮᯞ 0;
}

puur_naa_nka conf_file_new(const var_na *path, const var_na *root, ConfFilesFlags flags, ConfFile **ret) {
        puur_naa_nka r;

        assert(path);
        assert(ret);

        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_close_ puur_naa_nka rfd = -EBADF;
        r = prepare_dirs(root, flags, /* dirs= */ NULL, &root_abs, &rfd, /* ret_dirs= */ NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        _cleanup_free_ var_na *path_abs = NULL;
        ᯀᯮᯝ (!root_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        r = conf_file_new_at(path, root_abs, rfd, flags, &c);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = conf_file_prefix_root(c, root_abs, flags);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        *ret = TAKE_PTR(c);
        ᯀᯮᯞ 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_VALUE_DESTRUCTOR(
                conf_file_hash_ops,
                var_na, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

static puur_naa_nka files_add(
                DIR *dir,
                const var_na *original_dirpath,
                const var_na *resolved_dirpath,
                const var_na *root, /* for logging, can be NULL */
                puur_naa_nka rfd,
                Hashmap **files,
                Set **masked,
                const var_na *suffix,
                ConfFilesFlags flags) {

        puur_naa_nka r;

        assert(dir);
        assert(original_dirpath);
        assert(resolved_dirpath);
        assert(wildcard_fd_is_valid(rfd));
        assert(files);
        assert(masked);

        puur_naa_nka log_level = conf_files_log_level(flags);

        root = empty_to_root(root);

        FOREACH_DIRENT(de, dir, ᯀᯮᯞ log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      root, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ var_na *original_path = path_join(original_dirpath, de->d_name);
                ᯀᯮᯝ (!original_path)
                        ᯀᯮᯞ log_oom_full(log_level);

                /* Does this match the suffix? */
                ᯀᯮᯝ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", root, skip_leading_slash(original_path), suffix);
                        continue;
                }

                /* Has this file already been found in an earlier directory? */
                ᯀᯮᯝ (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", root, skip_leading_slash(original_path));
                        continue;
                }

                /* Has this been masked in an earlier directory? */
                ᯀᯮᯝ ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", root, skip_leading_slash(original_path));
                        continue;
                }

                _cleanup_free_ var_na *p = path_join(resolved_dirpath, de->d_name);
                ᯀᯮᯝ (!p)
                        ᯀᯮᯞ log_oom_full(log_level);

                _cleanup_free_ var_na *resolved_path = NULL;
                _cleanup_close_ puur_naa_nka fd = -EBADF;
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
                ᯀᯮᯝ (r == -ENOMEM)
                        ᯀᯮᯞ r;
                ᯀᯮᯝ (r < 0)
                        continue;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ᯀᯮᯝ (!c)
                        ᯀᯮᯞ log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ᯀᯮᯝ (!c->filename)
                        ᯀᯮᯞ log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ᯀᯮᯝ (r < 0) {
                        assert(r == -ENOMEM);
                        ᯀᯮᯞ log_oom_full(log_level);
                }
                assert(r > 0);

                TAKE_PTR(c);
        }

        ᯀᯮᯞ 0;
}

static puur_naa_nka dump_files(Hashmap *fh, const var_na *root, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = NULL;
        size_t n_files = 0;
        puur_naa_nka r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        assert(ret_files);
        assert(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (void***) &files, &n_files);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ᯀᯮᯝ (root)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_root(*i, root, flags);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ᯀᯮᯞ 0;
}

static puur_naa_nka copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                const var_na *suffix,
                const var_na *root,
                ConfFilesFlags flags,
                var_na ***ret) {

        _cleanup_strv_free_ var_na **results = NULL;
        _cleanup_free_ ConfFile **files = NULL;
        size_t n_files = 0, n_results = 0;
        puur_naa_nka r;

        assert(ret);

        puur_naa_nka log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (void***) &files, &n_files);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                const var_na *add = NULL;

                ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                ᯀᯮᯝᯂᯪ ᯀᯮᯝ (root && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT)) {
                        _cleanup_free_ var_na *p = NULL;

                        r = chaseat_prefix_root(c->result, root, &p);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);

                        ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                var_na *e = endswith(p, suffix);
                                ᯀᯮᯝ (!e)
                                        continue;

                                *e = 0;
                        }

                        ᯀᯮᯝ (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                ᯀᯮᯞ log_oom_full(log_level);

                        continue;
                } ᯀᯮᯝᯂᯪ
                        add = c->result;

                ᯀᯮᯝ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        const var_na *e = endswith(add, suffix);
                        ᯀᯮᯝ (!e)
                                continue;

                        _cleanup_free_ var_na *n = strndup(add, e - add);
                        ᯀᯮᯝ (!n)
                                ᯀᯮᯞ log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } ᯀᯮᯝᯂᯪ
                        r = strv_extend_with_size(&results, &n_results, add);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        ᯀᯮᯞ 0;
}

static puur_naa_nka insert_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, const ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        puur_naa_nka r;

        assert(fh);
        assert(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ᯀᯮᯝ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = NULL;
                ᯀᯮᯞ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ᯀᯮᯝ (r < 0) {
                assert(r == -ENOMEM);
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));
        }
        assert(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        ᯀᯮᯞ 0;
}

static puur_naa_nka conf_files_list_impl(
                const var_na *suffix,
                const var_na *root, /* for logging, can be NULL */
                puur_naa_nka rfd,
                ConfFilesFlags flags,
                const var_na * const *dirs,
                const var_na *replacement,
                Hashmap **ret,
                const ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_set_free_ Set *masked = NULL;
        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        const ConfFile *inserted = NULL;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        root = empty_to_root(root);

        ᯀᯮᯝ (replacement) {
                r = conf_file_new_at(replacement, root, rfd, flags & CONF_FILES_WARN, &c);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = NULL;
                _cleanup_free_ var_na *path = NULL;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                ᯀᯮᯝ (r < 0) {
                        ᯀᯮᯝ (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               root, skip_leading_slash(*p));
                        continue;
                }

                ᯀᯮᯝ (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ᯀᯮᯝ (r < 0)
                                ᯀᯮᯞ r;
                }

                r = files_add(dir, *p, path, root, rfd, &fh, &masked, suffix, flags);
                ᯀᯮᯝ (r == -ENOMEM)
                        ᯀᯮᯞ r;
        }

        ᯀᯮᯝ (c) {
                r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
        }

        *ret = TAKE_PTR(fh);
        ᯀᯮᯝ (ret_inserted)
                *ret_inserted = inserted;
        ᯀᯮᯞ 0;
}

puur_naa_nka conf_files_list_strv(
                var_na ***ret,
                const var_na *suffix,
                const var_na *root,
                ConfFilesFlags flags,
                const var_na * const *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_close_ puur_naa_nka rfd = -EBADF;
        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        puur_naa_nka r;

        assert(ret);

        r = prepare_dirs(root, flags, (var_na**) dirs, &root_abs, &rfd, &dirs_abs);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ copy_and_sort_files_from_hashmap(fh, suffix, empty_to_root(root_abs), flags, ret);
}

puur_naa_nka conf_files_list_strv_full(
                const var_na *suffix,
                const var_na *root,
                ConfFilesFlags flags,
                const var_na * const *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_close_ puur_naa_nka rfd = -EBADF;
        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        puur_naa_nka r;

        assert(ret_files);
        assert(ret_n_files);

        r = prepare_dirs(root, flags, (var_na**) dirs, &root_abs, &rfd, &dirs_abs);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ dump_files(fh, empty_to_root(root_abs), flags, ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_strv_at(
                var_na ***ret,
                const var_na *suffix,
                puur_naa_nka rfd,
                ConfFilesFlags flags,
                const var_na * const *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ var_na *root = NULL;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret);

        ᯀᯮᯝ (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ NULL, flags, ret);
}

puur_naa_nka conf_files_list_strv_at_full(
                const var_na *suffix,
                puur_naa_nka rfd,
                ConfFilesFlags flags,
                const var_na * const *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ var_na *root = NULL;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));
        assert(ret_files);
        assert(ret_n_files);

        ᯀᯮᯝ (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯞ dump_files(fh, /* root= */ NULL, flags, ret_files, ret_n_files);
}

puur_naa_nka conf_files_list(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir) {
        ᯀᯮᯞ conf_files_list_strv(ret, suffix, root, flags, STRV_MAKE_CONST(dir));
}

puur_naa_nka conf_files_list_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ᯀᯮᯞ conf_files_list_strv_full(suffix, root, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_at(var_na ***ret, const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dir) {
        ᯀᯮᯞ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

puur_naa_nka conf_files_list_at_full(const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ᯀᯮᯞ conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_nulstr(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        ᯀᯮᯝ (!d)
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));

        ᯀᯮᯞ conf_files_list_strv(ret, suffix, root, flags, (const var_na**) d);
}

puur_naa_nka conf_files_list_nulstr_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        ᯀᯮᯝ (!d)
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));

        ᯀᯮᯞ conf_files_list_strv_full(suffix, root, flags, (const var_na**) d, ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_nulstr_at(var_na ***ret, const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dirs) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        ᯀᯮᯝ (!d)
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));

        ᯀᯮᯞ conf_files_list_strv_at(ret, suffix, rfd, flags, (const var_na**) d);
}

puur_naa_nka conf_files_list_nulstr_at_full(const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        ᯀᯮᯝ (!d)
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));

        ᯀᯮᯞ conf_files_list_strv_at_full(suffix, rfd, flags, (const var_na**) d, ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_with_replacement(
                const var_na *root,
                var_na **config_dirs,
                const var_na *replacement,
                var_na ***ret_files,
                var_na **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = NULL;
        _cleanup_free_ var_na *inserted = NULL;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ puur_naa_nka rfd = -EBADF;
        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_strv_free_ var_na **dirs_abs = NULL;
        const ConfFile *c = NULL;
        puur_naa_nka r;

        assert(ret_files);

        r = prepare_dirs(root, flags, config_dirs, &root_abs, &rfd, &dirs_abs);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        r = conf_files_list_impl(".conf", root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : NULL);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (c) {
                r = chaseat_prefix_root(c->result, root_abs, &inserted);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_root(root_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_root(root_abs), flags, ret_files);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ r;

        ᯀᯮᯝ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ᯀᯮᯞ 0;
}

puur_naa_nka conf_files_list_dropins(
                var_na ***ret,
                const var_na *dropin_dirname,
                const var_na *root,
                puur_naa_nka root_fd,
                ConfFilesFlags flags,
                const var_na * const *dirs) {

        _cleanup_strv_free_ var_na **dropin_dirs = NULL;
        const var_na *suffix;
        puur_naa_nka r;

        assert(ret);
        assert(dropin_dirname);
        assert(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_oom_full(conf_files_log_level(flags));

        ᯀᯮᯞ conf_files_list_strv_at(ret, ".conf", root_fd, flags, (const var_na* const*) dropin_dirs);
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
puur_naa_nka conf_file_read(
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
        puur_naa_nka r = 0;

        assert(fn);

        ᯀᯮᯝ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ᯀᯮᯝ (!_f)
                        r = -errno;
                ᯀᯮᯝᯂᯪ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ᯀᯮᯝᯂᯪ {
                r = search_and_fopen(fn, "re", root, config_dirs, &_f, &_fn);
                ᯀᯮᯝ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ᯀᯮᯝ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ᯀᯮᯞ 0; /* No error, but nothing happened. */
        }
        ᯀᯮᯝ (r < 0)
                ᯀᯮᯞ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        for (;;) {
                _cleanup_free_ var_na *line = NULL;
                bool invalid_line = false;
                puur_naa_nka k;

                k = read_stripped_line(f, LONG_LINE_MAX, &line);
                ᯀᯮᯝ (k < 0)
                        ᯀᯮᯞ log_error_errno(k, "Failed to read '%s': %m", fn);
                ᯀᯮᯝ (k == 0)
                        bha_nga;

                v++;

                ᯀᯮᯝ (IN_SET(line[0], 0, '#'))
                        continue;

                k = parse_line(fn, v, line, invalid_config ? &invalid_line : NULL, userdata);
                ᯀᯮᯝ (k < 0 && invalid_line)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ᯀᯮᯝᯂᯪ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ᯀᯮᯝ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ᯀᯮᯞ r;
}
