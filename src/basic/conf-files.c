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
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಬಿಡುಗಡೆ(c->filename);
        ಬಿಡುಗಡೆ(c->ಫಲಿತಾಂಶ);
        ಬಿಡುಗಡೆ(c->original_ಮಾರ್ಗ);
        ಬಿಡುಗಡೆ(c->resolved_ಮಾರ್ಗ);
        ಸುರಕ್ಷಿತ_close(c->fd);

        ಹಿಂತಿರುಗಿ mfree(c);
}

DEFINE_ಸೂಚಕ_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf_files_log_level(ConfFilesFlags flags) {
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ prepare_dirs(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ConfFilesFlags flags,
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs,
                ಅಕ್ಷರ **ret_ಬೇರು,
                ಪೂರ್ಣಾಂಕ *ret_rfd,
                ಅಕ್ಷರ ***ret_dirs) {

        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **dirs_abs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_ಬೇರು);
        ಪ್ರತಿಜ್ಞಾ(ret_rfd);
        ಪ್ರತಿಜ್ಞಾ(ret_dirs || strv_isempty(dirs));

        ಪೂರ್ಣಾಂಕ log_level = conf_files_log_level(flags);

        r = ಖಾಲಿಗಳು_or_ಬೇರು_harder_ಗಮ್ಯ_null(&ಬೇರು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(ಬೇರು));

        ಆದರೆ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ಆದರೆ (!dirs_abs)
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = XAT_FDROOT;
        ಆದರೆ (ಬೇರು) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = ಮಾರ್ಗ_make_absolute_cwd(ಬೇರು, &ಬೇರು_abs);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to make '%s' absolute: %m", ಬೇರು);

                ಮಾರ್ಗ_simplify(ಬೇರು_abs);

                rfd = open(ಬೇರು, O_CLOEXEC|O_DIRECTORY|O_ಮಾರ್ಗ);
                ಆದರೆ (rfd < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, errno, "Failed to open '%s': %m", ಬೇರು_abs);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = ಮಾರ್ಗ_strv_make_absolute_cwd(dirs_abs);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_ಬೇರು = TAKE_PTR(ಬೇರು_abs);
        *ret_rfd = TAKE_FD(rfd);
        ಆದರೆ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf_file_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(ConfFile *c, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags) {
        ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(c);

        ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_DONT_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು))
                ಹಿಂತಿರುಗಿ 0;

        ಪೂರ್ಣಾಂಕ log_level = conf_files_log_level(flags);

        r = chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(c->ಫಲಿತಾಂಶ, ಬೇರು, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->ಫಲಿತಾಂಶ, ಬೇರು);
        free_and_replace(c->ಫಲಿತಾಂಶ, p);

        r = chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(c->resolved_ಮಾರ್ಗ, ಬೇರು, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_ಮಾರ್ಗ, ಬೇರು);
        free_and_replace(c->resolved_ಮಾರ್ಗ, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = ಮಾರ್ಗ_join(ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು), skip_leading_slash(c->original_ಮಾರ್ಗ));
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

        ಮಾರ್ಗ_simplify(p);
        ಹಿಂತಿರುಗಿ free_and_replace(c->original_ಮಾರ್ಗ, p);
}

ಸ್ಥಿರ bool conf_files_need_stat(ConfFilesFlags flags) {
        ಹಿಂತಿರುಗಿ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ಸ್ಥಿರ ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ಆದರೆ (!conf_files_need_stat(flags) || FLAGS_ಗಣ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ಹಿಂತಿರುಗಿ chase_flags;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf_file_chase_and_verify(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,          /* for logging, can be NULL */
                ಪೂರ್ಣಾಂಕ rfd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *original_ಮಾರ್ಗ, /* for logging */
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಗಣ_2 **masked,              /* optional */
                ConfFilesFlags flags,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ *ret_fd,
                ರಚನೆ stat *ret_stat) {

        _cleanup_free_ ಅಕ್ಷರ *resolved_ಮಾರ್ಗ = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ರಚನೆ stat st = {};
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(original_ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(name);

        ಪೂರ್ಣಾಂಕ log_level = conf_files_log_level(flags);

        ಬೇರು = ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು);

        r = chaseat(rfd, rfd, ಮಾರ್ಗ, conf_files_chase_flags(flags), &resolved_ಮಾರ್ಗ, &fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
        ಆದರೆ (r == 0) {
                ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ಆದರೆ (ಮಾರ್ಗ_equal(skip_leading_slash(resolved_ಮಾರ್ಗ), "dev/null")) {
                                ಆದರೆ (masked) {
                                        /* Mark this one as masked */
                                        r = ಗಣ_put_strdup(masked, name);
                                        ಆದರೆ (r < 0)
                                                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);
                                }

                                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
                        }
                }

                ಆದರೆ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ಆದರೆ (fd >= 0 && fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));

        /* Is this a masking entry? */
        ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ಆದರೆ (masked) {
                        /* Mark this one as masked */
                        r = ಗಣ_put_strdup(masked, name);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);
                }

                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_FILTER_MASKED_BY_ಖಾಲಿಗಳು) && stat_is_ಖಾಲಿಗಳು(&st)) {
                ಆದರೆ (masked) {
                        /* Mark this one as masked */
                        r = ಗಣ_put_strdup(masked, name);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);
                }

                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ಆದರೆ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* Is this node a regular file? */
                ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
                }

                /* Is this node a directory? */
                ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));

        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(resolved_ಮಾರ್ಗ);
        ಆದರೆ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ಆದರೆ (ret_stat)
                *ret_stat = st;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ conf_file_new_at(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಪೂರ್ಣಾಂಕ rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಂಕ log_level = conf_files_log_level(flags);

        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_2 = ಖಾಲಿ;
        ಆದರೆ (DEBUG_LOGGING && !ಬೇರು) {
                (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(rfd, &ಬೇರು_2);
                ಬೇರು = ಬೇರು_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

        *c = (ConfFile) {
                .original_ಮಾರ್ಗ = strdup(ಮಾರ್ಗ),
                .fd = -EBADF,
        };

        ಆದರೆ (!c->original_ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, &c->filename);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to extract filename from '%s': %m", ಮಾರ್ಗ);

        _cleanup_free_ ಅಕ್ಷರ *dirpath = ಖಾಲಿ, *resolved_dirpath = ಖಾಲಿ;
        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಮಾರ್ಗ, &dirpath);
        ಆದರೆ (r < 0 && r != -EDESTADDRREQ)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to extract directory from '%s': %m", ಮಾರ್ಗ);
        ಆದರೆ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ಖಾಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to chase '%s%s': %m", ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು), skip_leading_slash(dirpath));
        }

        c->ಫಲಿತಾಂಶ = ಮಾರ್ಗ_join(resolved_dirpath, c->filename);
        ಆದರೆ (!c->ಫಲಿತಾಂಶ)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

        r = conf_file_chase_and_verify(
                        ಬೇರು,
                        rfd,
                        c->original_ಮಾರ್ಗ,
                        c->ಫಲಿತಾಂಶ,
                        c->filename,
                        /* masked= */ ಖಾಲಿ,
                        flags,
                        &c->resolved_ಮಾರ್ಗ,
                        &c->fd,
                        &c->st);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(c);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ conf_file_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ConfFile **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = -EBADF;
        r = prepare_dirs(ಬೇರು, flags, /* dirs= */ ಖಾಲಿ, &ಬೇರು_abs, &rfd, /* ret_dirs= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ_abs = ಖಾಲಿ;
        ಆದರೆ (!ಬೇರು_abs) {
                r = ಮಾರ್ಗ_make_absolute_cwd(ಮಾರ್ಗ, &ಮಾರ್ಗ_abs);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", ಮಾರ್ಗ);

                ಮಾರ್ಗ = ಮಾರ್ಗ_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ಖಾಲಿ;
        r = conf_file_new_at(ಮಾರ್ಗ, ಬೇರು_abs, rfd, flags, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = conf_file_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(c, ಬೇರು_abs, flags);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(c);
        ಹಿಂತಿರುಗಿ 0;
}

DEFINE_ಖಾಸಗಿ_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(
                conf_file_hash_ops,
                ಅಕ್ಷರ, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ files_ಕೂಡಿಸು(
                DIR *dir,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *original_dirpath,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *resolved_dirpath,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, /* for logging, can be NULL */
                ಪೂರ್ಣಾಂಕ rfd,
                Hashmap **files,
                ಗಣ_2 **masked,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ConfFilesFlags flags) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir);
        ಪ್ರತಿಜ್ಞಾ(original_dirpath);
        ಪ್ರತಿಜ್ಞಾ(resolved_dirpath);
        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(masked);

        ಪೂರ್ಣಾಂಕ log_level = conf_files_log_level(flags);

        ಬೇರು = ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು);

        FOREACH_DIRENT(de, dir, ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      ಬೇರು, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ ಅಕ್ಷರ *original_ಮಾರ್ಗ = ಮಾರ್ಗ_join(original_dirpath, de->d_name);
                ಆದರೆ (!original_ಮಾರ್ಗ)
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

                /* Does this match the suffix? */
                ಆದರೆ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ), suffix);
                        ಮುಂದುವರಿ;
                }

                /* Has this file already been found in an earlier directory? */
                ಆದರೆ (hashmap_ಒಳಗೊಂಡಿದೆ(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
                        ಮುಂದುವರಿ;
                }

                /* Has this been masked in an earlier directory? */
                ಆದರೆ ((flags & CONF_FILES_FILTER_MASKED) != 0 && ಗಣ_ಒಳಗೊಂಡಿದೆ(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", ಬೇರು, skip_leading_slash(original_ಮಾರ್ಗ));
                        ಮುಂದುವರಿ;
                }

                _cleanup_free_ ಅಕ್ಷರ *p = ಮಾರ್ಗ_join(resolved_dirpath, de->d_name);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

                _cleanup_free_ ಅಕ್ಷರ *resolved_ಮಾರ್ಗ = ಖಾಲಿ;
                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
                ರಚನೆ stat st;
                r = conf_file_chase_and_verify(
                                ಬೇರು,
                                rfd,
                                original_ಮಾರ್ಗ,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_ಮಾರ್ಗ,
                                &fd,
                                &st);
                ಆದರೆ (r == -ENOMEM)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r < 0)
                        ಮುಂದುವರಿ;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ಆದರೆ (!c)
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .ಫಲಿತಾಂಶ = TAKE_PTR(p),
                        .original_ಮಾರ್ಗ = TAKE_PTR(original_ಮಾರ್ಗ),
                        .resolved_ಮಾರ್ಗ = TAKE_PTR(resolved_ಮಾರ್ಗ),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ಆದರೆ (!c->filename)
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ಆದರೆ (r < 0) {
                        ಪ್ರತಿಜ್ಞಾ(r == -ENOMEM);
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);
                }
                ಪ್ರತಿಜ್ಞಾ(r > 0);

                TAKE_PTR(c);
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dump_files(Hashmap *fh, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ಖಾಲಿ;
        size_t n_files = 0;
        ಪೂರ್ಣಾಂಕ r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (ಶೂನ್ಯ***) &files, &n_files);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ಆದರೆ (ಬೇರು)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(*i, ಬೇರು, flags);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ copy_and_ವಿಂಗಡಿಸು_files_from_hashmap(
                Hashmap *fh,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ConfFilesFlags flags,
                ಅಕ್ಷರ ***ret) {

        _cleanup_strv_free_ ಅಕ್ಷರ **results = ಖಾಲಿ;
        _cleanup_free_ ConfFile **files = ಖಾಲಿ;
        size_t n_files = 0, n_results = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪೂರ್ಣಾಂಕ log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (ಶೂನ್ಯ***) &files, &n_files);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೂಡಿಸು = ಖಾಲಿ;

                ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_BASENAME))
                        ಕೂಡಿಸು = c->filename;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬೇರು && !FLAGS_ಗಣ(flags, CONF_FILES_DONT_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು)) {
                        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                        r = chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(c->ಫಲಿತಾಂಶ, ಬೇರು, &p);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->ಫಲಿತಾಂಶ, ಬೇರು);

                        ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                ಅಕ್ಷರ *e = endswith(p, suffix);
                                ಆದರೆ (!e)
                                        ಮುಂದುವರಿ;

                                *e = 0;
                        }

                        ಆದರೆ (strv_consume_with_ಗಾತ್ರ(&results, &n_results, TAKE_PTR(p)) < 0)
                                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

                        ಮುಂದುವರಿ;
                } ಇಲ್ಲದಿದ್ದರೆ
                        ಕೂಡಿಸು = c->ಫಲಿತಾಂಶ;

                ಆದರೆ (FLAGS_ಗಣ(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = endswith(ಕೂಡಿಸು, suffix);
                        ಆದರೆ (!e)
                                ಮುಂದುವರಿ;

                        _cleanup_free_ ಅಕ್ಷರ *n = strndup(ಕೂಡಿಸು, e - ಕೂಡಿಸು);
                        ಆದರೆ (!n)
                                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);

                        r = strv_consume_with_ಗಾತ್ರ(&results, &n_results, TAKE_PTR(n));
                } ಇಲ್ಲದಿದ್ದರೆ
                        r = strv_extend_with_ಗಾತ್ರ(&results, &n_results, ಕೂಡಿಸು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(log_level);
        }

        *ret = TAKE_PTR(results);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸೇರಿಸು_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fh);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ಆದರೆ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->ಫಲಿತಾಂಶ, c->original_ಮಾರ್ಗ);
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ಆದರೆ (r < 0) {
                ಪ್ರತಿಜ್ಞಾ(r == -ENOMEM);
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));
        }
        ಪ್ರತಿಜ್ಞಾ(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->ಫಲಿತಾಂಶ);

        *ret = TAKE_PTR(c);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ conf_files_list_impl(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, /* for logging, can be NULL */
                ಪೂರ್ಣಾಂಕ rfd,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement,
                Hashmap **ret,
                ಸ್ಥಿರಾಂಕ ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ಖಾಲಿ;
        cleanup_ಗಣ_free ಗಣ_2 *masked = ಖಾಲಿ;
        _cleanup_(conf_file_freep) ConfFile *c = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ConfFile *inserted = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಬೇರು = ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು);

        ಆದರೆ (replacement) {
                r = conf_file_new_at(replacement, ಬೇರು, rfd, flags & CONF_FILES_WARN, &c);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ಖಾಲಿ;
                _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &ಮಾರ್ಗ, &dir);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (r != -ENOENT)
                                log_ತುಂಬಿದ_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               ಬೇರು, skip_leading_slash(*p));
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (c && streq_ptr(ಮಾರ್ಗ_startswith(c->ಫಲಿತಾಂಶ, ಮಾರ್ಗ), c->filename)) {
                        r = ಸೇರಿಸು_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                r = files_ಕೂಡಿಸು(dir, *p, ಮಾರ್ಗ, ಬೇರು, rfd, &fh, &masked, suffix, flags);
                ಆದರೆ (r == -ENOMEM)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (c) {
                r = ಸೇರಿಸು_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        *ret = TAKE_PTR(fh);
        ಆದರೆ (ret_inserted)
                *ret_inserted = inserted;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ conf_files_list_strv(
                ಅಕ್ಷರ ***ret,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **dirs_abs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = prepare_dirs(ಬೇರು, flags, (ಅಕ್ಷರ**) dirs, &ಬೇರು_abs, &rfd, &dirs_abs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = conf_files_list_impl(suffix, ಬೇರು_abs, rfd, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *) dirs_abs,
                                 /* replacement= */ ಖಾಲಿ, &fh, /* ret_inserted= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ copy_and_ವಿಂಗಡಿಸು_files_from_hashmap(fh, suffix, ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು_abs), flags, ret);
}

ಪೂರ್ಣಾಂಕ conf_files_list_strv_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **dirs_abs = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        r = prepare_dirs(ಬೇರು, flags, (ಅಕ್ಷರ**) dirs, &ಬೇರು_abs, &rfd, &dirs_abs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = conf_files_list_impl(suffix, ಬೇರು_abs, rfd, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *) dirs_abs,
                                 /* replacement= */ ಖಾಲಿ, &fh, /* ret_inserted= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ dump_files(fh, ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು_abs), flags, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಂಕ conf_files_list_strv_at(
                ಅಕ್ಷರ ***ret,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಪೂರ್ಣಾಂಕ rfd,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (DEBUG_LOGGING)
                (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(rfd, &ಬೇರು); /* for logging */

        r = conf_files_list_impl(suffix, ಬೇರು, rfd, flags, dirs, /* replacement= */ ಖಾಲಿ, &fh, /* ret_inserted= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ copy_and_ವಿಂಗಡಿಸು_files_from_hashmap(fh, suffix, /* root= */ ಖಾಲಿ, flags, ret);
}

ಪೂರ್ಣಾಂಕ conf_files_list_strv_at_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix,
                ಪೂರ್ಣಾಂಕ rfd,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(rfd));
        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        ಆದರೆ (DEBUG_LOGGING)
                (ಶೂನ್ಯ) fd_get_ಮಾರ್ಗ(rfd, &ಬೇರು); /* for logging */

        r = conf_files_list_impl(suffix, ಬೇರು, rfd, flags, dirs, /* replacement= */ ಖಾಲಿ, &fh, /* ret_inserted= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ dump_files(fh, /* root= */ ಖಾಲಿ, flags, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಂಕ conf_files_list(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir) {
        ಹಿಂತಿರುಗಿ conf_files_list_strv(ret, suffix, ಬೇರು, flags, STRV_MAKE_CONST(dir));
}

ಪೂರ್ಣಾಂಕ conf_files_list_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ಹಿಂತಿರುಗಿ conf_files_list_strv_ತುಂಬಿದ(suffix, ಬೇರು, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ಪೂರ್ಣಾಂಕ conf_files_list_at(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir) {
        ಹಿಂತಿರುಗಿ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

ಪೂರ್ಣಾಂಕ conf_files_list_at_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ಹಿಂತಿರುಗಿ conf_files_list_strv_at_ತುಂಬಿದ(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ಪೂರ್ಣಾಂಕ conf_files_list_nulstr(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs) {
        _cleanup_strv_free_ ಅಕ್ಷರ **d = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        d = strv_ವಿಭಜಿಸು_nulstr(dirs);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));

        ಹಿಂತಿರುಗಿ conf_files_list_strv(ret, suffix, ಬೇರು, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**) d);
}

ಪೂರ್ಣಾಂಕ conf_files_list_nulstr_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ಅಕ್ಷರ **d = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        d = strv_ವಿಭಜಿಸು_nulstr(dirs);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));

        ಹಿಂತಿರುಗಿ conf_files_list_strv_ತುಂಬಿದ(suffix, ಬೇರು, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**) d, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಂಕ conf_files_list_nulstr_at(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs) {
        _cleanup_strv_free_ ಅಕ್ಷರ **d = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        d = strv_ವಿಭಜಿಸು_nulstr(dirs);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));

        ಹಿಂತಿರುಗಿ conf_files_list_strv_at(ret, suffix, rfd, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**) d);
}

ಪೂರ್ಣಾಂಕ conf_files_list_nulstr_at_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ಅಕ್ಷರ **d = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(ret_files);
        ಪ್ರತಿಜ್ಞಾ(ret_n_files);

        d = strv_ವಿಭಜಿಸು_nulstr(dirs);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));

        ಹಿಂತಿರುಗಿ conf_files_list_strv_at_ತುಂಬಿದ(suffix, rfd, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ**) d, ret_files, ret_n_files);
}

ಪೂರ್ಣಾಂಕ conf_files_list_with_replacement(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ **config_dirs,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement,
                ಅಕ್ಷರ ***ret_files,
                ಅಕ್ಷರ **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *inserted = ಖಾಲಿ;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *ಬೇರು_abs = ಖಾಲಿ;
        _cleanup_strv_free_ ಅಕ್ಷರ **dirs_abs = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ConfFile *c = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_files);

        r = prepare_dirs(ಬೇರು, flags, config_dirs, &ಬೇರು_abs, &rfd, &dirs_abs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = conf_files_list_impl(".conf", ಬೇರು_abs, rfd, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (c) {
                r = chaseat_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು(c->ಫಲಿತಾಂಶ, ಬೇರು_abs, &inserted);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->ಫಲಿತಾಂಶ, ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು_abs));
        }

        r = copy_and_ವಿಂಗಡಿಸು_files_from_hashmap(fh, ".conf", ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು_abs), flags, ret_files);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ conf_files_list_dropins(
                ಅಕ್ಷರ ***ret,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dropin_dirname,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs) {

        _cleanup_strv_free_ ಅಕ್ಷರ **dropin_dirs = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(dropin_dirname);
        ಪ್ರತಿಜ್ಞಾ(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_oom_ತುಂಬಿದ(conf_files_log_level(flags));

        ಹಿಂತಿರುಗಿ conf_files_list_strv_at(ret, ".conf", ಬೇರು_fd, flags, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*) dropin_dirs);
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
ಪೂರ್ಣಾಂಕ conf_file_read(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **config_dirs,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                parse_ಸಾಲು_t parse_ಸಾಲು,
                ಶೂನ್ಯ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *_fn = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ v = 0;
        FILE *f;
        ಪೂರ್ಣಾಂಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fn);

        ಆದರೆ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_ಮಾರ್ಗ(fn)) {
                r = ಮಾರ್ಗ_make_absolute_cwd(fn, &_fn);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ಆದರೆ (!_f)
                        r = -errno;
                ಇಲ್ಲದಿದ್ದರೆ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ಇಲ್ಲದಿದ್ದರೆ {
                r = ಹುಡುಕು_and_fopen(fn, "re", ಬೇರು, config_dirs, &_f, &_fn);
                ಆದರೆ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ಆದರೆ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ಹಿಂತಿರುಗಿ 0; /* No error, but nothing happened. */
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
                bool invalid_ಸಾಲು = false;
                ಪೂರ್ಣಾಂಕ k;

                k = read_stripped_ಸಾಲು(f, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (k < 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(k, "Failed to read '%s': %m", fn);
                ಆದರೆ (k == 0)
                        ನಿಲ್ಲಿಸು;

                v++;

                ಆದರೆ (ಒಳ_ಗಣ(ಸಾಲು[0], 0, '#'))
                        ಮುಂದುವರಿ;

                k = parse_ಸಾಲು(fn, v, ಸಾಲು, invalid_config ? &invalid_ಸಾಲು : ಖಾಲಿ, userdata);
                ಆದರೆ (k < 0 && invalid_ಸಾಲು)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ಇಲ್ಲದಿದ್ದರೆ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ಆದರೆ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ಹಿಂತಿರುಗಿ r;
}
