/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "conf-files.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "glyph-util.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "set.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಮುಕ್ತ(c->filename);
        ಮುಕ್ತ(c->result);
        ಮುಕ್ತ(c->original_path);
        ಮುಕ್ತ(c->resolved_path);
        safe_close(c->fd);

        ಪ್ರತ್ಯಾಗಮ mfree(c);
}

DEFINE_POINTER_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf_files_log_level(ConfFilesFlags flags) {
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ prepare_dirs(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ConfFilesFlags flags,
                ವರ್ಣ * ಧ್ರುವ *dirs,
                ವರ್ಣ **ret_ಬೇರು,
                ಪೂರ್ಣಾಙ್ಕ *ret_rfd,
                ವರ್ಣ ***ret_dirs) {

        _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;
        _cleanup_strv_free_ ವರ್ಣ **dirs_abs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_ಬೇರು);
        ಪ್ರತಿಜ್ಞಾ(ret_rfd);
        ಪ್ರತಿಜ್ಞಾ(ret_dirs || strv_isempty(dirs));

        ಪೂರ್ಣಾಙ್ಕ log_level = conf_files_log_level(flags);

        r = empty_or_ಬೇರು_harder_to_null(&ಬೇರು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(ಬೇರು));

        ಯದಿ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ಯದಿ (!dirs_abs)
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = XAT_FDROOT;
        ಯದಿ (ಬೇರು) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = path_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to make '%s' absolute: %m", ಬೇರು);

                path_simplify(ಬೇರು_abs);

                rfd = open(ಬೇರು, O_CLOEXEC|O_DIRECTORY|O_PATH);
                ಯದಿ (rfd < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, errno, "Failed to open '%s': %m", ಬೇರು_abs);

        } ಅನ್ಯಥಾ ಯದಿ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = path_strv_make_absolute_cwd(dirs_abs);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_ಬೇರು = TAKE_PTR(ಬೇರು_abs);
        *ret_rfd = TAKE_FD(rfd);
        ಯದಿ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf_file_prefix_ಬೇರು(ConfFile *c, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags) {
        ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(c);

        ಯದಿ (FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ಬೇರು))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪೂರ್ಣಾಙ್ಕ log_level = conf_files_log_level(flags);

        r = chaseat_prefix_ಬೇರು(c->result, ಬೇರು, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, ಬೇರು);
        free_and_replace(c->result, p);

        r = chaseat_prefix_ಬೇರು(c->resolved_path, ಬೇರು, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_path, ಬೇರು);
        free_and_replace(c->resolved_path, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = path_join(empty_to_ಬೇರು(ಬೇರು), skip_leading_slash(c->original_path));
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

        path_simplify(p);
        ಪ್ರತ್ಯಾಗಮ free_and_replace(c->original_path, p);
}

ಸ್ಥಿರ bool conf_files_need_stat(ConfFilesFlags flags) {
        ಪ್ರತ್ಯಾಗಮ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ಸ್ಥಿರ ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ಯದಿ (!conf_files_need_stat(flags) || FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ಪ್ರತ್ಯಾಗಮ chase_flags;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf_file_chase_and_verify(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,          /* for logging, can be NULL */
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ಧ್ರುವ ವರ್ಣ *original_path, /* for logging */
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *name,
                Set **masked,              /* optional */
                ConfFilesFlags flags,
                ವರ್ಣ **ret_path,
                ಪೂರ್ಣಾಙ್ಕ *ret_fd,
                ಸಂರಚನಾ stat *ret_stat) {

        _cleanup_free_ ವರ್ಣ *resolved_path = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಸಂರಚನಾ stat st = {};
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(original_path);
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(name);

        ಪೂರ್ಣಾಙ್ಕ log_level = conf_files_log_level(flags);

        ಬೇರು = empty_to_ಬೇರು(ಬೇರು);

        r = chaseat(rfd, rfd, path, conf_files_chase_flags(flags), &resolved_path, &fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      ಬೇರು, skip_leading_slash(original_path));
        ಯದಿ (r == 0) {
                ಯದಿ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ಯದಿ (path_equal(skip_leading_slash(resolved_path), "dev/null")) {
                                ಯದಿ (masked) {
                                        /* Mark this one as masked */
                                        r = set_put_strdup(masked, name);
                                        ಯದಿ (r < 0)
                                                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);
                                }

                                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       ಬೇರು, skip_leading_slash(original_path));
                        }
                }

                ಯದಿ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              ಬೇರು, skip_leading_slash(original_path));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ಯದಿ (fd >= 0 && fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      ಬೇರು, skip_leading_slash(original_path));

        /* Is this a masking entry? */
        ಯದಿ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ಯದಿ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);
                }

                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       ಬೇರು, skip_leading_slash(original_path));
        }

        ಯದಿ (FLAGS_SET(flags, CONF_FILES_FILTER_MASKED_BY_EMPTY) && stat_is_empty(&st)) {
                ಯದಿ (masked) {
                        /* Mark this one as masked */
                        r = set_put_strdup(masked, name);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);
                }

                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       ಬೇರು, skip_leading_slash(original_path));
        }

        ಯದಿ (FLAGS_SET(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ಯದಿ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               ಬೇರು, skip_leading_slash(original_path));
        } ಅನ್ಯಥಾ {
                /* Is this node a regular file? */
                ಯದಿ (FLAGS_SET(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       ಬೇರು, skip_leading_slash(original_path));
                }

                /* Is this node a directory? */
                ಯದಿ (FLAGS_SET(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       ಬೇರು, skip_leading_slash(original_path));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ಯದಿ (FLAGS_SET(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       ಬೇರು, skip_leading_slash(original_path));

        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(resolved_path);
        ಯದಿ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ಯದಿ (ret_stat)
                *ret_stat = st;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ conf_file_new_at(
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಙ್ಕ log_level = conf_files_log_level(flags);

        _cleanup_free_ ವರ್ಣ *ಬೇರು_2 = ರಿಕ್ತ;
        ಯದಿ (DEBUG_LOGGING && !ಬೇರು) {
                (ಶೂನ್ಯ) fd_get_path(rfd, &ಬೇರು_2);
                ಬೇರು = ಬೇರು_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

        *c = (ConfFile) {
                .original_path = strdup(path),
                .fd = -EBADF,
        };

        ಯದಿ (!c->original_path)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

        r = path_extract_filename(path, &c->filename);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to extract filename from '%s': %m", path);

        _cleanup_free_ ವರ್ಣ *dirpath = ರಿಕ್ತ, *resolved_dirpath = ರಿಕ್ತ;
        r = path_extract_directory(path, &dirpath);
        ಯದಿ (r < 0 && r != -EDESTADDRREQ)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to extract directory from '%s': %m", path);
        ಯದಿ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ರಿಕ್ತ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to chase '%s%s': %m", empty_to_ಬೇರು(ಬೇರು), skip_leading_slash(dirpath));
        }

        c->result = path_join(resolved_dirpath, c->filename);
        ಯದಿ (!c->result)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

        r = conf_file_chase_and_verify(
                        ಬೇರು,
                        rfd,
                        c->original_path,
                        c->result,
                        c->filename,
                        /* masked= */ ರಿಕ್ತ,
                        flags,
                        &c->resolved_path,
                        &c->fd,
                        &c->st);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(c);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ conf_file_new(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ConfFile **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = -EBADF;
        r = prepare_dirs(ಬೇರು, flags, /* dirs= */ ರಿಕ್ತ, &ಬೇರು_abs, &rfd, /* ret_dirs= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_free_ ವರ್ಣ *path_abs = ರಿಕ್ತ;
        ಯದಿ (!ಬೇರು_abs) {
                r = path_make_absolute_cwd(path, &path_abs);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", path);

                path = path_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ರಿಕ್ತ;
        r = conf_file_new_at(path, ಬೇರು_abs, rfd, flags, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = conf_file_prefix_ಬೇರು(c, ಬೇರು_abs, flags);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(c);
        ಪ್ರತ್ಯಾಗಮ 0;
}

DEFINE_PRIVATE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                conf_file_hash_ops,
                ವರ್ಣ, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ files_add(
                DIR *dir,
                ಧ್ರುವ ವರ್ಣ *original_dirpath,
                ಧ್ರುವ ವರ್ಣ *resolved_dirpath,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು, /* for logging, can be NULL */
                ಪೂರ್ಣಾಙ್ಕ rfd,
                Hashmap **files,
                Set **masked,
                ಧ್ರುವ ವರ್ಣ *suffix,
                ConfFilesFlags flags) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir);
        ಪ್ರತಿಜ್ಞಾ(original_dirpath);
        ಪ್ರತಿಜ್ಞಾ(resolved_dirpath);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(masked);

        ಪೂರ್ಣಾಙ್ಕ log_level = conf_files_log_level(flags);

        ಬೇರು = empty_to_ಬೇರು(ಬೇರು);

        FOREACH_DIRENT(de, dir, ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      ಬೇರು, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ ವರ್ಣ *original_path = path_join(original_dirpath, de->d_name);
                ಯದಿ (!original_path)
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

                /* Does this match the suffix? */
                ಯದಿ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", ಬೇರು, skip_leading_slash(original_path), suffix);
                        ಅನುವರ್ತ;
                }

                /* Has this file already been found in an earlier directory? */
                ಯದಿ (hashmap_contains(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", ಬೇರು, skip_leading_slash(original_path));
                        ಅನುವರ್ತ;
                }

                /* Has this been masked in an earlier directory? */
                ಯದಿ ((flags & CONF_FILES_FILTER_MASKED) != 0 && set_contains(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", ಬೇರು, skip_leading_slash(original_path));
                        ಅನುವರ್ತ;
                }

                _cleanup_free_ ವರ್ಣ *p = path_join(resolved_dirpath, de->d_name);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

                _cleanup_free_ ವರ್ಣ *resolved_path = ರಿಕ್ತ;
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
                ಸಂರಚನಾ stat st;
                r = conf_file_chase_and_verify(
                                ಬೇರು,
                                rfd,
                                original_path,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_path,
                                &fd,
                                &st);
                ಯದಿ (r == -ENOMEM)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r < 0)
                        ಅನುವರ್ತ;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ಯದಿ (!c)
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .result = TAKE_PTR(p),
                        .original_path = TAKE_PTR(original_path),
                        .resolved_path = TAKE_PTR(resolved_path),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ಯದಿ (!c->filename)
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ಯದಿ (r < 0) {
                        ಪ್ರತಿಜ್ಞಾ(r == -ENOMEM);
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);
                }
                ಪ್ರತಿಜ್ಞಾ(r > 0);

                TAKE_PTR(c);
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dump_files(Hashmap *fh, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ರಿಕ್ತ;
        size_t n_files = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (ಶೂನ್ಯ***) &files, &n_files);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ಯದಿ (ಬೇರು)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_prefix_ಬೇರು(*i, ಬೇರು, flags);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ copy_and_sort_files_from_hashmap(
                Hashmap *fh,
                ಧ್ರುವ ವರ್ಣ *suffix,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ConfFilesFlags flags,
                ವರ್ಣ ***ret) {

        _cleanup_strv_free_ ವರ್ಣ **results = ರಿಕ್ತ;
        _cleanup_free_ ConfFile **files = ರಿಕ್ತ;
        size_t n_files = 0, n_results = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಙ್ಕ log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (ಶೂನ್ಯ***) &files, &n_files);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ಧ್ರುವ ವರ್ಣ *add = ರಿಕ್ತ;

                ಯದಿ (FLAGS_SET(flags, CONF_FILES_BASENAME))
                        add = c->filename;
                ಅನ್ಯಥಾ ಯದಿ (ಬೇರು && !FLAGS_SET(flags, CONF_FILES_DONT_PREFIX_ಬೇರು)) {
                        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                        r = chaseat_prefix_ಬೇರು(c->result, ಬೇರು, &p);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->result, ಬೇರು);

                        ಯದಿ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                ವರ್ಣ *e = endswith(p, suffix);
                                ಯದಿ (!e)
                                        ಅನುವರ್ತ;

                                *e = 0;
                        }

                        ಯದಿ (strv_consume_with_size(&results, &n_results, TAKE_PTR(p)) < 0)
                                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

                        ಅನುವರ್ತ;
                } ಅನ್ಯಥಾ
                        add = c->result;

                ಯದಿ (FLAGS_SET(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ಧ್ರುವ ವರ್ಣ *e = endswith(add, suffix);
                        ಯದಿ (!e)
                                ಅನುವರ್ತ;

                        _cleanup_free_ ವರ್ಣ *n = strndup(add, e - add);
                        ಯದಿ (!n)
                                ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);

                        r = strv_consume_with_size(&results, &n_results, TAKE_PTR(n));
                } ಅನ್ಯಥಾ
                        r = strv_extend_with_size(&results, &n_results, add);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_oom_full(log_level);
        }

        *ret = TAKE_PTR(results);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ಸೇರಿಸು_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ಧ್ರುವ ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fh);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ಯದಿ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->result, c->original_path);
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ಯದಿ (r < 0) {
                ಪ್ರತಿಜ್ಞಾ(r == -ENOMEM);
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));
        }
        ಪ್ರತಿಜ್ಞಾ(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->result);

        *ret = TAKE_PTR(c);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ conf_files_list_impl(
                ಧ್ರುವ ವರ್ಣ *suffix,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು, /* for logging, can be NULL */
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs,
                ಧ್ರುವ ವರ್ಣ *replacement,
                Hashmap **ret,
                ಧ್ರುವ ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ರಿಕ್ತ;
        _cleanup_set_free_ Set *masked = ರಿಕ್ತ;
        _cleanup_(conf_file_freep) ConfFile *c = ರಿಕ್ತ;
        ಧ್ರುವ ConfFile *inserted = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಬೇರು = empty_to_ಬೇರು(ಬೇರು);

        ಯದಿ (replacement) {
                r = conf_file_new_at(replacement, ಬೇರು, rfd, flags & CONF_FILES_WARN, &c);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ರಿಕ್ತ;
                _cleanup_free_ ವರ್ಣ *path = ರಿಕ್ತ;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &path, &dir);
                ಯದಿ (r < 0) {
                        ಯದಿ (r != -ENOENT)
                                log_full_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               ಬೇರು, skip_leading_slash(*p));
                        ಅನುವರ್ತ;
                }

                ಯದಿ (c && streq_ptr(path_startswith(c->result, path), c->filename)) {
                        r = ಸೇರಿಸು_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                r = files_add(dir, *p, path, ಬೇರು, rfd, &fh, &masked, suffix, flags);
                ಯದಿ (r == -ENOMEM)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (c) {
                r = ಸೇರಿಸು_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        *ret = TAKE_PTR(fh);
        ಯದಿ (ret_inserted)
                *ret_inserted = inserted;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv(
                ವರ್ಣ ***ret,
                ಧ್ರುವ ವರ್ಣ *suffix,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = -EBADF;
        _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;
        _cleanup_strv_free_ ವರ್ಣ **dirs_abs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = prepare_dirs(ಬೇರು, flags, (ವರ್ಣ**) dirs, &ಬೇರು_abs, &rfd, &dirs_abs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = conf_files_list_impl(suffix, ಬೇರು_abs, rfd, flags, (ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *) dirs_abs,
                                 /* replacement= */ ರಿಕ್ತ, &fh, /* ret_inserted= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ copy_and_sort_files_from_hashmap(fh, suffix, empty_to_ಬೇರು(ಬೇರು_abs), flags, ret);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv_full(
                ಧ್ರುವ ವರ್ಣ *suffix,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = -EBADF;
        _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;
        _cleanup_strv_free_ ವರ್ಣ **dirs_abs = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        r = prepare_dirs(ಬೇರು, flags, (ವರ್ಣ**) dirs, &ಬೇರು_abs, &rfd, &dirs_abs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = conf_files_list_impl(suffix, ಬೇರು_abs, rfd, flags, (ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *) dirs_abs,
                                 /* replacement= */ ರಿಕ್ತ, &fh, /* ret_inserted= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ dump_files(fh, empty_to_ಬೇರು(ಬೇರು_abs), flags, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv_at(
                ವರ್ಣ ***ret,
                ಧ್ರುವ ವರ್ಣ *suffix,
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *ಬೇರು = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (DEBUG_LOGGING)
                (ಶೂನ್ಯ) fd_get_path(rfd, &ಬೇರು); /* for logging */

        r = conf_files_list_impl(suffix, ಬೇರು, rfd, flags, dirs, /* replacement= */ ರಿಕ್ತ, &fh, /* ret_inserted= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ copy_and_sort_files_from_hashmap(fh, suffix, /* root= */ ರಿಕ್ತ, flags, ret);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv_at_full(
                ಧ್ರುವ ವರ್ಣ *suffix,
                ಪೂರ್ಣಾಙ್ಕ rfd,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *ಬೇರು = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        ಯದಿ (DEBUG_LOGGING)
                (ಶೂನ್ಯ) fd_get_path(rfd, &ಬೇರು); /* for logging */

        r = conf_files_list_impl(suffix, ಬೇರು, rfd, flags, dirs, /* replacement= */ ರಿಕ್ತ, &fh, /* ret_inserted= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ dump_files(fh, /* root= */ ರಿಕ್ತ, flags, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir) {
        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv(ret, suffix, ಬೇರು, flags, STRV_MAKE_CONST(dir));
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_full(ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_full(suffix, ಬೇರು, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_at(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir) {
        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_at_full(ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_at_full(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs) {
        _cleanup_strv_free_ ವರ್ಣ **d = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        d = strv_split_nulstr(dirs);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));

        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv(ret, suffix, ಬೇರು, flags, (ಧ್ರುವ ವರ್ಣ**) d);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr_full(ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ವರ್ಣ **d = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        d = strv_split_nulstr(dirs);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));

        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_full(suffix, ಬೇರು, flags, (ಧ್ರುವ ವರ್ಣ**) d, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr_at(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs) {
        _cleanup_strv_free_ ವರ್ಣ **d = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        d = strv_split_nulstr(dirs);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));

        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_at(ret, suffix, rfd, flags, (ಧ್ರುವ ವರ್ಣ**) d);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr_at_full(ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ವರ್ಣ **d = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        d = strv_split_nulstr(dirs);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));

        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_at_full(suffix, rfd, flags, (ಧ್ರುವ ವರ್ಣ**) d, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_with_replacement(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ **config_dirs,
                ಧ್ರುವ ವರ್ಣ *replacement,
                ವರ್ಣ ***ret_files,
                ವರ್ಣ **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *inserted = ರಿಕ್ತ;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = -EBADF;
        _cleanup_free_ ವರ್ಣ *ಬೇರು_abs = ರಿಕ್ತ;
        _cleanup_strv_free_ ವರ್ಣ **dirs_abs = ರಿಕ್ತ;
        ಧ್ರುವ ConfFile *c = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_files);

        r = prepare_dirs(ಬೇರು, flags, config_dirs, &ಬೇರು_abs, &rfd, &dirs_abs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = conf_files_list_impl(".conf", ಬೇರು_abs, rfd, flags, (ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (c) {
                r = chaseat_prefix_ಬೇರು(c->result, ಬೇರು_abs, &inserted);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->result, empty_to_ಬೇರು(ಬೇರು_abs));
        }

        r = copy_and_sort_files_from_hashmap(fh, ".conf", empty_to_ಬೇರು(ಬೇರು_abs), flags, ret_files);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ conf_files_list_dropins(
                ವರ್ಣ ***ret,
                ಧ್ರುವ ವರ್ಣ *dropin_dirname,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs) {

        _cleanup_strv_free_ ವರ್ಣ **dropin_dirs = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *suffix;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(dropin_dirname);
        ಪ್ರತಿಜ್ಞಾ(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_oom_full(conf_files_log_level(flags));

        ಪ್ರತ್ಯಾಗಮ conf_files_list_strv_at(ret, ".conf", ಬೇರು_fd, flags, (ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ*) dropin_dirs);
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
ಪೂರ್ಣಾಙ್ಕ conf_file_read(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಧ್ರುವ ವರ್ಣ **config_dirs,
                ಧ್ರುವ ವರ್ಣ *fn,
                parse_ಸಾಲು_t parse_ಸಾಲು,
                ಶೂನ್ಯ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *_fn = ರಿಕ್ತ;
        unsigned v = 0;
        FILE *f;
        ಪೂರ್ಣಾಙ್ಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fn);

        ಯದಿ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ಅನ್ಯಥಾ ಯದಿ (is_path(fn)) {
                r = path_make_absolute_cwd(fn, &_fn);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ಯದಿ (!_f)
                        r = -errno;
                ಅನ್ಯಥಾ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ಅನ್ಯಥಾ {
                r = ಹುಡುಕು_and_fopen(fn, "re", ಬೇರು, config_dirs, &_f, &_fn);
                ಯದಿ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ಯದಿ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ಪ್ರತ್ಯಾಗಮ 0; /* No error, but nothing happened. */
        }
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
                bool invalid_ಸಾಲು = false;
                ಪೂರ್ಣಾಙ್ಕ k;

                k = read_stripped_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಯದಿ (k < 0)
                        ಪ್ರತ್ಯಾಗಮ log_error_errno(k, "Failed to read '%s': %m", fn);
                ಯದಿ (k == 0)
                        ಭಙ್ಗ;

                v++;

                ಯದಿ (IN_SET(ಸಾಲು[0], 0, '#'))
                        ಅನುವರ್ತ;

                k = parse_ಸಾಲು(fn, v, ಸಾಲು, invalid_config ? &invalid_ಸಾಲು : ರಿಕ್ತ, userdata);
                ಯದಿ (k < 0 && invalid_ಸಾಲು)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ಅನ್ಯಥಾ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ಯದಿ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ಪ್ರತ್ಯಾಗಮ r;
}
