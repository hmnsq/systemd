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
        ᮚᮦᮔ᮪ (!c)
                ᮝᮜᮤ NULL;

        free(c->filename);
        free(c->result);
        free(c->original_path);
        free(c->resolved_path);
        safe_close(c->fd);

        ᮝᮜᮤ mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

static puur_naa_nka conf_files_log_level(ConfFilesFlags flags) {
        ᮝᮜᮤ FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(root));

        ᮚᮦᮔ᮪ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ᮚᮦᮔ᮪ (!dirs_abs)
                        ᮝᮜᮤ log_oom_full(log_level);
        }

        _cleanup_close_ puur_naa_nka rfd = XAT_FDROOT;
        ᮚᮦᮔ᮪ (root) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(root, &root_abs);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", root);

                path_simplify(root_abs);

                rfd = open(root, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ᮚᮦᮔ᮪ (rfd < 0)
                        ᮝᮜᮤ log_full_errno(log_level, errno, "Failed to open '%s': %m", root_abs);

        } ᮜᮤᮚ ᮚᮦᮔ᮪ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_root = TAKE_PTR(root_abs);
        *ret_rfd = TAKE_FD(rfd);
        ᮚᮦᮔ᮪ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ᮝᮜᮤ 0;
}

static puur_naa_nka conf_file_prefix_root(ConfFile *c, const var_na *root, ConfFilesFlags flags) {
        var_na *p;
        puur_naa_nka r;

        assert(c);

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT))
                ᮝᮜᮤ 0;

        puur_naa_nka log_level = conf_files_log_level(flags);

        r = chaseat_prefix_root(c->result, root, &p);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);
        free_and_replace(c->result, p);

        r = chaseat_prefix_root(c->resolved_path, root, &p);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, root);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_root(root), skip_leading_slash(c->original_path));
        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ log_oom_full(log_level);

        path_simplify(p);
        ᮝᮜᮤ free_and_replace(c->original_path, p);
}

static bool conf_files_need_stat(ConfFilesFlags flags) {
        ᮝᮜᮤ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

static ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ᮚᮦᮔ᮪ (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ᮝᮜᮤ chase_flags;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      root, skip_leading_slash(original_path));
        ᮚᮦᮔ᮪ (r == 0) {
                ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ᮚᮦᮔ᮪ (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                ᮚᮦᮔ᮪ (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        ᮚᮦᮔ᮪ (r < 0)
                                                ᮝᮜᮤ log_oom_full(log_level);
                                }

                                ᮝᮜᮤ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       root, skip_leading_slash(original_path));
                        }
                }

                ᮚᮦᮔ᮪ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ᮝᮜᮤ log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              root, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ᮚᮦᮔ᮪ (fd >= 0 && fstat(fd, &st) < 0)
                ᮝᮜᮤ log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      root, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ᮚᮦᮔ᮪ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ log_oom_full(log_level);
                }

                ᮝᮜᮤ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       root, skip_leading_slash(original_path));
        }

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                ᮚᮦᮔ᮪ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ log_oom_full(log_level);
                }

                ᮝᮜᮤ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       root, skip_leading_slash(original_path));
        }

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ᮚᮦᮔ᮪ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ᮝᮜᮤ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               root, skip_leading_slash(original_path));
        } ᮜᮤᮚ {
                /* Is this node a regular file? */
                ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       root, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       root, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ᮝᮜᮤ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       root, skip_leading_slash(original_path));

        ᮚᮦᮔ᮪ (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        ᮚᮦᮔ᮪ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ᮚᮦᮔ᮪ (ret_stat)
                *ret_stat = st;

        ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (DEBUG_LOGGING && !root) {
                (void) fd_get_path(rfd, &_root);
                root = _root;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ᮚᮦᮔ᮪ (!c)
                ᮝᮜᮤ log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        ᮚᮦᮔ᮪ (!c->original_path)
                ᮝᮜᮤ log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ var_na *dirpath = NULL, *resolved_dirpath = NULL;
        r = path_extract_directory(path, &dirpath);
        ᮚᮦᮔ᮪ (r < 0 && r != -EDESTADDRREQ)
                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        ᮚᮦᮔ᮪ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ NULL);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_root(root), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        ᮚᮦᮔ᮪ (!c->result)
                ᮝᮜᮤ log_oom_full(log_level);

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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        *ret = TAKE_PTR(c);
        ᮝᮜᮤ 0;
}

puur_naa_nka conf_file_new(const var_na *path, const var_na *root, ConfFilesFlags flags, ConfFile **ret) {
        puur_naa_nka r;

        assert(path);
        assert(ret);

        _cleanup_free_ var_na *root_abs = NULL;
        _cleanup_close_ puur_naa_nka rfd = -EBADF;
        r = prepare_dirs(root, flags, /* dirs= */ NULL, &root_abs, &rfd, /* ret_dirs= */ NULL);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        _cleanup_free_ var_na *path_abs = NULL;
        ᮚᮦᮔ᮪ (!root_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = NULL;
        r = conf_file_new_at(path, root_abs, rfd, flags, &c);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = conf_file_prefix_root(c, root_abs, flags);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        *ret = TAKE_PTR(c);
        ᮝᮜᮤ 0;
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

        FOREACH_DIRENT(de, dir, ᮝᮜᮤ log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      root, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ var_na *original_path = path_join(original_dirpath, de->d_name);
                ᮚᮦᮔ᮪ (!original_path)
                        ᮝᮜᮤ log_oom_full(log_level);

                /* Does this match the suffix? */
                ᮚᮦᮔ᮪ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", root, skip_leading_slash(original_path), suffix);
                        continue;
                }

                /* Has this file already been found in an earlier directory? */
                ᮚᮦᮔ᮪ (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", root, skip_leading_slash(original_path));
                        continue;
                }

                /* Has this been masked in an earlier directory? */
                ᮚᮦᮔ᮪ ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", root, skip_leading_slash(original_path));
                        continue;
                }

                _cleanup_free_ var_na *p = path_join(resolved_dirpath, de->d_name);
                ᮚᮦᮔ᮪ (!p)
                        ᮝᮜᮤ log_oom_full(log_level);

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
                ᮚᮦᮔ᮪ (r == -ENOMEM)
                        ᮝᮜᮤ r;
                ᮚᮦᮔ᮪ (r < 0)
                        continue;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ᮚᮦᮔ᮪ (!c)
                        ᮝᮜᮤ log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ᮚᮦᮔ᮪ (!c->filename)
                        ᮝᮜᮤ log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ᮚᮦᮔ᮪ (r < 0) {
                        assert(r == -ENOMEM);
                        ᮝᮜᮤ log_oom_full(log_level);
                }
                assert(r > 0);

                TAKE_PTR(c);
        }

        ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ᮚᮦᮔ᮪ (root)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_root(*i, root, flags);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                const var_na *add = NULL;

                ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                ᮜᮤᮚ ᮚᮦᮔ᮪ (root && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ROOT)) {
                        _cleanup_free_ var_na *p = NULL;

                        r = chaseat_prefix_root(c->result, root, &p);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, root);

                        ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                var_na *e = endswith(p, suffix);
                                ᮚᮦᮔ᮪ (!e)
                                        continue;

                                *e = 0;
                        }

                        ᮚᮦᮔ᮪ (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                ᮝᮜᮤ log_oom_full(log_level);

                        continue;
                } ᮜᮤᮚ
                        add = c->result;

                ᮚᮦᮔ᮪ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        const var_na *e = endswith(add, suffix);
                        ᮚᮦᮔ᮪ (!e)
                                continue;

                        _cleanup_free_ var_na *n = strndup(add, e - add);
                        ᮚᮦᮔ᮪ (!n)
                                ᮝᮜᮤ log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } ᮜᮤᮚ
                        r = strv_extend_with_size(&results, &n_results, add);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        ᮝᮜᮤ 0;
}

static puur_naa_nka insert_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, const ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        puur_naa_nka r;

        assert(fh);
        assert(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ᮚᮦᮔ᮪ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = NULL;
                ᮝᮜᮤ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ᮚᮦᮔ᮪ (r < 0) {
                assert(r == -ENOMEM);
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));
        }
        assert(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        ᮝᮜᮤ 0;
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

        ᮚᮦᮔ᮪ (replacement) {
                r = conf_file_new_at(replacement, root, rfd, flags & CONF_FILES_WARN, &c);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = NULL;
                _cleanup_free_ var_na *path = NULL;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                ᮚᮦᮔ᮪ (r < 0) {
                        ᮚᮦᮔ᮪ (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               root, skip_leading_slash(*p));
                        continue;
                }

                ᮚᮦᮔ᮪ (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ r;
                }

                r = files_add(dir, *p, path, root, rfd, &fh, &masked, suffix, flags);
                ᮚᮦᮔ᮪ (r == -ENOMEM)
                        ᮝᮜᮤ r;
        }

        ᮚᮦᮔ᮪ (c) {
                r = insert_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        *ret = TAKE_PTR(fh);
        ᮚᮦᮔ᮪ (ret_inserted)
                *ret_inserted = inserted;
        ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ copy_and_sort_files_from_hashmap(fh, suffix, empty_to_root(root_abs), flags, ret);
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = conf_files_list_impl(suffix, root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ dump_files(fh, empty_to_root(root_abs), flags, ret_files, ret_n_files);
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

        ᮚᮦᮔ᮪ (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ NULL, flags, ret);
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

        ᮚᮦᮔ᮪ (DEBUG_LOGGING)
                (void) fd_get_path(rfd, &root); /* for logging */

        r = conf_files_list_impl(suffix, root, rfd, flags, dirs, /* replacement= */ NULL, &fh, /* ret_inserted= */ NULL);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ dump_files(fh, /* root= */ NULL, flags, ret_files, ret_n_files);
}

puur_naa_nka conf_files_list(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir) {
        ᮝᮜᮤ conf_files_list_strv(ret, suffix, root, flags, STRV_MAKE_CONST(dir));
}

puur_naa_nka conf_files_list_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ᮝᮜᮤ conf_files_list_strv_full(suffix, root, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_at(var_na ***ret, const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dir) {
        ᮝᮜᮤ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

puur_naa_nka conf_files_list_at_full(const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ᮝᮜᮤ conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_nulstr(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));

        ᮝᮜᮤ conf_files_list_strv(ret, suffix, root, flags, (const var_na**) d);
}

puur_naa_nka conf_files_list_nulstr_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));

        ᮝᮜᮤ conf_files_list_strv_full(suffix, root, flags, (const var_na**) d, ret_files, ret_n_files);
}

puur_naa_nka conf_files_list_nulstr_at(var_na ***ret, const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dirs) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret);

        d = strv_split_nulstr(dirs);
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));

        ᮝᮜᮤ conf_files_list_strv_at(ret, suffix, rfd, flags, (const var_na**) d);
}

puur_naa_nka conf_files_list_nulstr_at_full(const var_na *suffix, puur_naa_nka rfd, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ var_na **d = NULL;

        assert(ret_files);
        assert(ret_n_files);

        d = strv_split_nulstr(dirs);
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));

        ᮝᮜᮤ conf_files_list_strv_at_full(suffix, rfd, flags, (const var_na**) d, ret_files, ret_n_files);
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = conf_files_list_impl(".conf", root_abs, rfd, flags, (const var_na * const *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : NULL);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (c) {
                r = chaseat_prefix_root(c->result, root_abs, &inserted);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_root(root_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_root(root_abs), flags, ret_files);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ᮝᮜᮤ 0;
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
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_oom_full(conf_files_log_level(flags));

        ᮝᮜᮤ conf_files_list_strv_at(ret, ".conf", root_fd, flags, (const var_na* const*) dropin_dirs);
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

        ᮚᮦᮔ᮪ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ᮜᮤᮚ ᮚᮦᮔ᮪ (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ᮚᮦᮔ᮪ (!_f)
                        r = -errno;
                ᮜᮤᮚ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ᮜᮤᮚ {
                r = search_and_fopen(fn, "re", root, config_dirs, &_f, &_fn);
                ᮚᮦᮔ᮪ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ᮚᮦᮔ᮪ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ᮝᮜᮤ 0; /* No error, but nothing happened. */
        }
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        for (;;) {
                _cleanup_free_ var_na *line = NULL;
                bool invalid_line = false;
                puur_naa_nka k;

                k = read_stripped_line(f, LONG_LINE_MAX, &line);
                ᮚᮦᮔ᮪ (k < 0)
                        ᮝᮜᮤ log_error_errno(k, "Failed to read '%s': %m", fn);
                ᮚᮦᮔ᮪ (k == 0)
                        bha_nga;

                v++;

                ᮚᮦᮔ᮪ (IN_SET(line[0], 0, '#'))
                        continue;

                k = parse_line(fn, v, line, invalid_config ? &invalid_line : NULL, userdata);
                ᮚᮦᮔ᮪ (k < 0 && invalid_line)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ᮜᮤᮚ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ᮚᮦᮔ᮪ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ᮝᮜᮤ r;
}
