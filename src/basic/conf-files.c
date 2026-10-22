/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "conf-files.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glyph-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "nulstr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "set.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ConfFile* conf_file_free(ConfFile *c) {
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        ਮੁਕਤ(c->filename);
        ਮੁਕਤ(c->ਨਤੀਜਾ);
        ਮੁਕਤ(c->original_ਰਸਤਾ);
        ਮੁਕਤ(c->resolved_ਰਸਤਾ);
        ਸੁਰੱਖਿਅਤ_close(c->fd);

        ਵਾਪਸ mfree(c);
}

DEFINE_ਸੂਚਕ_ARRAY_FREE_FUNC(ConfFile*, conf_file_free);

ਸਥਿਰ ਪੂਰਨ conf_files_log_level(ConfFilesFlags flags) {
        ਵਾਪਸ FLAGS_ਸੈੱਟ(flags, CONF_FILES_WARN) ? LOG_WARNING : LOG_DEBUG;
}

ਸਥਿਰ ਪੂਰਨ prepare_dirs(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ConfFilesFlags flags,
                ਅੱਖਰ * ਅਚਲ *dirs,
                ਅੱਖਰ **ret_ਜੜ੍ਹ,
                ਪੂਰਨ *ret_rfd,
                ਅੱਖਰ ***ret_dirs) {

        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **dirs_abs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਜੜ੍ਹ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_rfd);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_dirs || strv_isempty(dirs));

        ਪੂਰਨ log_level = conf_files_log_level(flags);

        r = ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ_harder_ਮੰਜ਼ਿਲ_null(&ਜੜ੍ਹ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to determine if '%s' points to the root directory: %m", strempty(ਜੜ੍ਹ));

        ਜੇ (ret_dirs) {
                dirs_abs = strv_copy(dirs);
                ਜੇ (!dirs_abs)
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);
        }

        _cleanup_close_ ਪੂਰਨ rfd = XAT_FDROOT;
        ਜੇ (ਜੜ੍ਹ) {
                /* When a non-trivial root is specified, we will prefix the result later. Hence, it is not
                 * necessary to modify each config directories here. but needs to normalize the root directory. */
                r = ਰਸਤਾ_make_absolute_cwd(ਜੜ੍ਹ, &ਜੜ੍ਹ_abs);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to make '%s' absolute: %m", ਜੜ੍ਹ);

                ਰਸਤਾ_simplify(ਜੜ੍ਹ_abs);

                rfd = open(ਜੜ੍ਹ, O_CLOEXEC|O_DIRECTORY|O_ਰਸਤਾ);
                ਜੇ (rfd < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, errno, "Failed to open '%s': %m", ਜੜ੍ਹ_abs);

        } ਨਹੀਂਤਾਂ ਜੇ (ret_dirs) {
                /* When an empty root or "/" is specified, we will open "/" below, hence we need to make
                 * each config directory absolute if relative. */
                r = ਰਸਤਾ_strv_make_absolute_cwd(dirs_abs);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to make directories absolute: %m");
        }

        *ret_ਜੜ੍ਹ = TAKE_PTR(ਜੜ੍ਹ_abs);
        *ret_rfd = TAKE_FD(rfd);
        ਜੇ (ret_dirs)
                *ret_dirs = TAKE_PTR(dirs_abs);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ conf_file_ਅਗੇਤਰ_ਜੜ੍ਹ(ConfFile *c, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags) {
        ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(c);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_DONT_ਅਗੇਤਰ_ਜੜ੍ਹ))
                ਵਾਪਸ 0;

        ਪੂਰਨ log_level = conf_files_log_level(flags);

        r = chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(c->ਨਤੀਜਾ, ਜੜ੍ਹ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->ਨਤੀਜਾ, ਜੜ੍ਹ);
        free_and_replace(c->ਨਤੀਜਾ, p);

        r = chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(c->resolved_ਰਸਤਾ, ਜੜ੍ਹ, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->resolved_ਰਸਤਾ, ਜੜ੍ਹ);
        free_and_replace(c->resolved_ਰਸਤਾ, p);

        /* Do not use chaseat_prefix_root(), as it is for the result of chaseat(), but the path is not chased. */
        p = ਰਸਤਾ_join(ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ), skip_leading_slash(c->original_ਰਸਤਾ));
        ਜੇ (!p)
                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

        ਰਸਤਾ_simplify(p);
        ਵਾਪਸ free_and_replace(c->original_ਰਸਤਾ, p);
}

ਸਥਿਰ bool conf_files_need_stat(ConfFilesFlags flags) {
        ਵਾਪਸ (flags & (CONF_FILES_FILTER_MASKED | CONF_FILES_REGULAR | CONF_FILES_DIRECTORY | CONF_FILES_EXECUTABLE)) != 0;
}

ਸਥਿਰ ChaseFlags conf_files_chase_flags(ConfFilesFlags flags) {
        ChaseFlags chase_flags = 0;

        ਜੇ (!conf_files_need_stat(flags) || FLAGS_ਸੈੱਟ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK))
                /* Even if no verification is requested, let's unconditionally call chaseat(),
                 * to drop unsafe symlinks. */
                chase_flags |= CHASE_NONEXISTENT;

        ਵਾਪਸ chase_flags;
}

ਸਥਿਰ ਪੂਰਨ conf_file_chase_and_verify(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,          /* for logging, can be NULL */
                ਪੂਰਨ rfd,
                ਅਚਲ ਅੱਖਰ *original_ਰਸਤਾ, /* for logging */
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *name,
                ਸੈੱਟ_2 **masked,              /* optional */
                ConfFilesFlags flags,
                ਅੱਖਰ **ret_ਰਸਤਾ,
                ਪੂਰਨ *ret_fd,
                ਬਣਤਰ stat *ret_stat) {

        _cleanup_free_ ਅੱਖਰ *resolved_ਰਸਤਾ = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਬਣਤਰ stat st = {};
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(original_ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        ਪੂਰਨ log_level = conf_files_log_level(flags);

        ਜੜ੍ਹ = ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ);

        r = chaseat(rfd, rfd, ਰਸਤਾ, conf_files_chase_flags(flags), &resolved_ਰਸਤਾ, &fd);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to chase '%s%s': %m",
                                      ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
        ਜੇ (r == 0) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK)) {
                        /* If the path points to /dev/null in a image or so, then the device node may not exist. */
                        ਜੇ (ਰਸਤਾ_equal(skip_leading_slash(resolved_ਰਸਤਾ), "dev/null")) {
                                ਜੇ (masked) {
                                        /* Mark this one as masked */
                                        r = ਸੈੱਟ_put_strdup(masked, name);
                                        ਜੇ (r < 0)
                                                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);
                                }

                                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                                       ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
                        }
                }

                ਜੇ (conf_files_need_stat(flags))
                        /* If we need to have stat, skip the entry. */
                        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(ENOENT), "Failed to chase '%s%s': %m",
                                              ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
        }

        /* Even if we do not need stat, let's take stat now. The caller may use the info later. */
        ਜੇ (fd >= 0 && fstat(fd, &st) < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, errno, "Failed to stat '%s%s': %m",
                                      ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));

        /* Is this a masking entry? */
        ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_FILTER_MASKED_BY_SYMLINK) && stat_may_be_dev_null(&st)) {
                ਜੇ (masked) {
                        /* Mark this one as masked */
                        r = ਸੈੱਟ_put_strdup(masked, name);
                        ਜੇ (r < 0)
                                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);
                }

                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (symlink to /dev/null).",
                                       ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_FILTER_MASKED_BY_ਖਾਲੀ_ਥਾਵਾਂ) && stat_is_ਖਾਲੀ_ਥਾਵਾਂ(&st)) {
                ਜੇ (masked) {
                        /* Mark this one as masked */
                        r = ਸੈੱਟ_put_strdup(masked, name);
                        ਜੇ (r < 0)
                                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);
                }

                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ERFKILL),
                                       "File '%s%s' is a mask (an empty file).",
                                       ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_REGULAR|CONF_FILES_DIRECTORY)) {
                ਜੇ (!S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode))
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EBADFD),
                                               "File '%s%s' is neither a regular file or directory.",
                                               ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
        } ਨਹੀਂਤਾਂ {
                /* Is this node a regular file? */
                ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_REGULAR)) {
                        r = stat_verify_regular(&st);
                        ਜੇ (r < 0)
                                ਵਾਪਸ log_debug_errno(r, "File '%s%s' is not a regular file: %m",
                                                       ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
                }

                /* Is this node a directory? */
                ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_DIRECTORY)) {
                        r = stat_verify_directory(&st);
                        ਜੇ (r < 0)
                                ਵਾਪਸ log_debug_errno(r, "File '%s%s' is not a directory: %m",
                                                       ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
                }
        }

        /* Does this node have the executable bit set?
         * As requested: check if the file is marked executable. Note that we don't check access(X_OK) here,
         * as we care about whether the file is marked executable at all, and not whether it is executable
         * for us, because if so, such errors are stuff we should log about. */
        ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_EXECUTABLE) && (st.st_mode & 0111) == 0)
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENOEXEC),
                                       "File '%s%s' is not marked executable.",
                                       ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));

        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(resolved_ਰਸਤਾ);
        ਜੇ (ret_fd)
                *ret_fd = TAKE_FD(fd);
        ਜੇ (ret_stat)
                *ret_stat = st;

        ਵਾਪਸ 0;
}

ਪੂਰਨ conf_file_new_at(
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਪੂਰਨ rfd,
                ConfFilesFlags flags,
                ConfFile **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਪੂਰਨ log_level = conf_files_log_level(flags);

        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_2 = ਨਿਰਾ;
        ਜੇ (DEBUG_LOGGING && !ਜੜ੍ਹ) {
                (ਖਾਲੀ) fd_get_ਰਸਤਾ(rfd, &ਜੜ੍ਹ_2);
                ਜੜ੍ਹ = ਜੜ੍ਹ_2;
        }

        _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
        ਜੇ (!c)
                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

        *c = (ConfFile) {
                .original_ਰਸਤਾ = strdup(ਰਸਤਾ),
                .fd = -EBADF,
        };

        ਜੇ (!c->original_ਰਸਤਾ)
                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

        r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, &c->filename);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to extract filename from '%s': %m", ਰਸਤਾ);

        _cleanup_free_ ਅੱਖਰ *dirpath = ਨਿਰਾ, *resolved_dirpath = ਨਿਰਾ;
        r = ਰਸਤਾ_ਕੱਢੋ_directory(ਰਸਤਾ, &dirpath);
        ਜੇ (r < 0 && r != -EDESTADDRREQ)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to extract directory from '%s': %m", ਰਸਤਾ);
        ਜੇ (r >= 0) {
                r = chaseat(rfd, rfd, dirpath,
                            CHASE_MUST_BE_DIRECTORY | conf_files_chase_flags(flags),
                            &resolved_dirpath, /* ret_fd= */ ਨਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to chase '%s%s': %m", ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ), skip_leading_slash(dirpath));
        }

        c->ਨਤੀਜਾ = ਰਸਤਾ_join(resolved_dirpath, c->filename);
        ਜੇ (!c->ਨਤੀਜਾ)
                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

        r = conf_file_chase_and_verify(
                        ਜੜ੍ਹ,
                        rfd,
                        c->original_ਰਸਤਾ,
                        c->ਨਤੀਜਾ,
                        c->filename,
                        /* masked= */ ਨਿਰਾ,
                        flags,
                        &c->resolved_ਰਸਤਾ,
                        &c->fd,
                        &c->st);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(c);
        ਵਾਪਸ 0;
}

ਪੂਰਨ conf_file_new(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ConfFile **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ rfd = -EBADF;
        r = prepare_dirs(ਜੜ੍ਹ, flags, /* dirs= */ ਨਿਰਾ, &ਜੜ੍ਹ_abs, &rfd, /* ret_dirs= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ_abs = ਨਿਰਾ;
        ਜੇ (!ਜੜ੍ਹ_abs) {
                r = ਰਸਤਾ_make_absolute_cwd(ਰਸਤਾ, &ਰਸਤਾ_abs);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(conf_files_log_level(flags), r, "Failed to make '%s' absolute: %m", ਰਸਤਾ);

                ਰਸਤਾ = ਰਸਤਾ_abs;
        }

        _cleanup_(conf_file_freep) ConfFile *c = ਨਿਰਾ;
        r = conf_file_new_at(ਰਸਤਾ, ਜੜ੍ਹ_abs, rfd, flags, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = conf_file_ਅਗੇਤਰ_ਜੜ੍ਹ(c, ਜੜ੍ਹ_abs, flags);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(c);
        ਵਾਪਸ 0;
}

DEFINE_ਨਿੱਜੀ_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR(
                conf_file_hash_ops,
                ਅੱਖਰ, string_hash_func, string_compare_func,
                ConfFile, conf_file_free);

ਸਥਿਰ ਪੂਰਨ files_ਜੋੜੋ(
                DIR *dir,
                ਅਚਲ ਅੱਖਰ *original_dirpath,
                ਅਚਲ ਅੱਖਰ *resolved_dirpath,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, /* for logging, can be NULL */
                ਪੂਰਨ rfd,
                Hashmap **files,
                ਸੈੱਟ_2 **masked,
                ਅਚਲ ਅੱਖਰ *suffix,
                ConfFilesFlags flags) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir);
        ਪ੍ਰਤਿਜ੍ਞਾ(original_dirpath);
        ਪ੍ਰਤਿਜ੍ਞਾ(resolved_dirpath);
        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(files);
        ਪ੍ਰਤਿਜ੍ਞਾ(masked);

        ਪੂਰਨ log_level = conf_files_log_level(flags);

        ਜੜ੍ਹ = ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ);

        FOREACH_DIRENT(de, dir, ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, errno, "Failed to read directory '%s%s': %m",
                                                      ਜੜ੍ਹ, skip_leading_slash(original_dirpath))) {

                _cleanup_free_ ਅੱਖਰ *original_ਰਸਤਾ = ਰਸਤਾ_join(original_dirpath, de->d_name);
                ਜੇ (!original_ਰਸਤਾ)
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

                /* Does this match the suffix? */
                ਜੇ (suffix && !endswith(de->d_name, suffix)) {
                        log_debug("Skipping file '%s%s', suffix is not '%s'.", ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ), suffix);
                        ਜਾਰੀ;
                }

                /* Has this file already been found in an earlier directory? */
                ਜੇ (hashmap_ਸ਼ਾਮਲ_ਹੈ(*files, de->d_name)) {
                        log_debug("Skipping overridden file '%s%s'.", ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
                        ਜਾਰੀ;
                }

                /* Has this been masked in an earlier directory? */
                ਜੇ ((flags & CONF_FILES_FILTER_MASKED) != 0 && ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(*masked, de->d_name)) {
                        log_debug("File '%s%s' is masked by previous entry.", ਜੜ੍ਹ, skip_leading_slash(original_ਰਸਤਾ));
                        ਜਾਰੀ;
                }

                _cleanup_free_ ਅੱਖਰ *p = ਰਸਤਾ_join(resolved_dirpath, de->d_name);
                ਜੇ (!p)
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

                _cleanup_free_ ਅੱਖਰ *resolved_ਰਸਤਾ = ਨਿਰਾ;
                _cleanup_close_ ਪੂਰਨ fd = -EBADF;
                ਬਣਤਰ stat st;
                r = conf_file_chase_and_verify(
                                ਜੜ੍ਹ,
                                rfd,
                                original_ਰਸਤਾ,
                                p,
                                de->d_name,
                                masked,
                                flags,
                                &resolved_ਰਸਤਾ,
                                &fd,
                                &st);
                ਜੇ (r == -ENOMEM)
                        ਵਾਪਸ r;
                ਜੇ (r < 0)
                        ਜਾਰੀ;

                _cleanup_(conf_file_freep) ConfFile *c = new(ConfFile, 1);
                ਜੇ (!c)
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

                *c = (ConfFile) {
                        .filename = strdup(de->d_name),
                        .ਨਤੀਜਾ = TAKE_PTR(p),
                        .original_ਰਸਤਾ = TAKE_PTR(original_ਰਸਤਾ),
                        .resolved_ਰਸਤਾ = TAKE_PTR(resolved_ਰਸਤਾ),
                        .fd = TAKE_FD(fd),
                        .st = st,
                };

                ਜੇ (!c->filename)
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

                r = hashmap_ensure_put(files, &conf_file_hash_ops, c->filename, c);
                ਜੇ (r < 0) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(r == -ENOMEM);
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);
                }
                ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

                TAKE_PTR(c);
        }

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ dump_files(Hashmap *fh, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ConfFile ***ret_files, size_t *ret_n_files) {
        ConfFile **files = ਨਿਰਾ;
        size_t n_files = 0;
        ਪੂਰਨ r;

        CLEANUP_ARRAY(files, n_files, conf_file_free_array);

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_files);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_n_files);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap. */
        r = hashmap_dump_sorted(fh, (ਖਾਲੀ***) &files, &n_files);
        ਜੇ (r < 0)
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));

        /* Hence, we need to remove them from the hashmap. */
        FOREACH_ARRAY(i, files, n_files)
                assert_se(hashmap_remove(fh, (*i)->filename) == *i);

        ਜੇ (ਜੜ੍ਹ)
                FOREACH_ARRAY(i, files, n_files) {
                        r = conf_file_ਅਗੇਤਰ_ਜੜ੍ਹ(*i, ਜੜ੍ਹ, flags);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

        *ret_files = TAKE_PTR(files);
        *ret_n_files = n_files;
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ copy_and_ਛਾਂਟੋ_files_from_hashmap(
                Hashmap *fh,
                ਅਚਲ ਅੱਖਰ *suffix,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ConfFilesFlags flags,
                ਅੱਖਰ ***ret) {

        _cleanup_strv_free_ ਅੱਖਰ **results = ਨਿਰਾ;
        _cleanup_free_ ConfFile **files = ਨਿਰਾ;
        size_t n_files = 0, n_results = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਪੂਰਨ log_level = conf_files_log_level(flags);

        /* The entries in the array given by hashmap_dump_sorted() are still owned by the hashmap.
         * Hence, do not use conf_file_free_array() for 'entries' */
        r = hashmap_dump_sorted(fh, (ਖਾਲੀ***) &files, &n_files);
        ਜੇ (r < 0)
                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

        FOREACH_ARRAY(i, files, n_files) {
                ConfFile *c = *i;
                ਅਚਲ ਅੱਖਰ *ਜੋੜੋ = ਨਿਰਾ;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_BASENAME))
                        ਜੋੜੋ = c->filename;
                ਨਹੀਂਤਾਂ ਜੇ (ਜੜ੍ਹ && !FLAGS_ਸੈੱਟ(flags, CONF_FILES_DONT_ਅਗੇਤਰ_ਜੜ੍ਹ)) {
                        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                        r = chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(c->ਨਤੀਜਾ, ਜੜ੍ਹ, &p);
                        ਜੇ (r < 0)
                                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, r, "Failed to prefix '%s' with root '%s': %m", c->ਨਤੀਜਾ, ਜੜ੍ਹ);

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_TRUNCATE_SUFFIX) && suffix) {
                                ਅੱਖਰ *e = endswith(p, suffix);
                                ਜੇ (!e)
                                        ਜਾਰੀ;

                                *e = 0;
                        }

                        ਜੇ (strv_consume_with_ਮਾਪ(&results, &n_results, TAKE_PTR(p)) < 0)
                                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

                        ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ
                        ਜੋੜੋ = c->ਨਤੀਜਾ;

                ਜੇ (FLAGS_ਸੈੱਟ(flags, CONF_FILES_TRUNCATE_SUFFIX)) {
                        ਅਚਲ ਅੱਖਰ *e = endswith(ਜੋੜੋ, suffix);
                        ਜੇ (!e)
                                ਜਾਰੀ;

                        _cleanup_free_ ਅੱਖਰ *n = strndup(ਜੋੜੋ, e - ਜੋੜੋ);
                        ਜੇ (!n)
                                ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);

                        r = strv_consume_with_ਮਾਪ(&results, &n_results, TAKE_PTR(n));
                } ਨਹੀਂਤਾਂ
                        r = strv_extend_with_ਮਾਪ(&results, &n_results, ਜੋੜੋ);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_oom_ਭਰਿਆ(log_level);
        }

        *ret = TAKE_PTR(results);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਪਾਓ_replacement(Hashmap **fh, ConfFile *replacement, ConfFilesFlags flags, ਅਚਲ ConfFile **ret) {
        _cleanup_(conf_file_freep) ConfFile *c = ASSERT_PTR(replacement);
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fh);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This consumes the input ConfFile. */

        ConfFile *existing = hashmap_get(*fh, c->filename);
        ਜੇ (existing) {
                log_debug("An entry with higher priority '%s' -> '%s' already exists, ignoring the replacement: %s",
                          existing->filename, existing->ਨਤੀਜਾ, c->original_ਰਸਤਾ);
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        r = hashmap_ensure_put(fh, &conf_file_hash_ops, c->filename, c);
        ਜੇ (r < 0) {
                ਪ੍ਰਤਿਜ੍ਞਾ(r == -ENOMEM);
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));
        }
        ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

        log_debug("Inserted replacement: '%s' -> '%s'", c->filename, c->ਨਤੀਜਾ);

        *ret = TAKE_PTR(c);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ conf_files_list_impl(
                ਅਚਲ ਅੱਖਰ *suffix,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, /* for logging, can be NULL */
                ਪੂਰਨ rfd,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs,
                ਅਚਲ ਅੱਖਰ *replacement,
                Hashmap **ret,
                ਅਚਲ ConfFile **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ਨਿਰਾ;
        cleanup_ਸੈੱਟ_free ਸੈੱਟ_2 *masked = ਨਿਰਾ;
        _cleanup_(conf_file_freep) ConfFile *c = ਨਿਰਾ;
        ਅਚਲ ConfFile *inserted = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੜ੍ਹ = ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ);

        ਜੇ (replacement) {
                r = conf_file_new_at(replacement, ਜੜ੍ਹ, rfd, flags & CONF_FILES_WARN, &c);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        STRV_FOREACH(p, dirs) {
                _cleanup_closedir_ DIR *dir = ਨਿਰਾ;
                _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;

                r = chase_and_opendirat(rfd, rfd, *p, 0, &ਰਸਤਾ, &dir);
                ਜੇ (r < 0) {
                        ਜੇ (r != -ENOENT)
                                log_ਭਰਿਆ_errno(conf_files_log_level(flags), r,
                                               "Failed to chase and open directory '%s%s', ignoring: %m",
                                               ਜੜ੍ਹ, skip_leading_slash(*p));
                        ਜਾਰੀ;
                }

                ਜੇ (c && streq_ptr(ਰਸਤਾ_startswith(c->ਨਤੀਜਾ, ਰਸਤਾ), c->filename)) {
                        r = ਪਾਓ_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                r = files_ਜੋੜੋ(dir, *p, ਰਸਤਾ, ਜੜ੍ਹ, rfd, &fh, &masked, suffix, flags);
                ਜੇ (r == -ENOMEM)
                        ਵਾਪਸ r;
        }

        ਜੇ (c) {
                r = ਪਾਓ_replacement(&fh, TAKE_PTR(c), flags, &inserted);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        *ret = TAKE_PTR(fh);
        ਜੇ (ret_inserted)
                *ret_inserted = inserted;
        ਵਾਪਸ 0;
}

ਪੂਰਨ conf_files_list_strv(
                ਅੱਖਰ ***ret,
                ਅਚਲ ਅੱਖਰ *suffix,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ rfd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **dirs_abs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = prepare_dirs(ਜੜ੍ਹ, flags, (ਅੱਖਰ**) dirs, &ਜੜ੍ਹ_abs, &rfd, &dirs_abs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = conf_files_list_impl(suffix, ਜੜ੍ਹ_abs, rfd, flags, (ਅਚਲ ਅੱਖਰ * ਅਚਲ *) dirs_abs,
                                 /* replacement= */ ਨਿਰਾ, &fh, /* ret_inserted= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ copy_and_ਛਾਂਟੋ_files_from_hashmap(fh, suffix, ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ_abs), flags, ret);
}

ਪੂਰਨ conf_files_list_strv_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *suffix,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ rfd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **dirs_abs = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_files);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_n_files);

        r = prepare_dirs(ਜੜ੍ਹ, flags, (ਅੱਖਰ**) dirs, &ਜੜ੍ਹ_abs, &rfd, &dirs_abs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = conf_files_list_impl(suffix, ਜੜ੍ਹ_abs, rfd, flags, (ਅਚਲ ਅੱਖਰ * ਅਚਲ *) dirs_abs,
                                 /* replacement= */ ਨਿਰਾ, &fh, /* ret_inserted= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ dump_files(fh, ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ_abs), flags, ret_files, ret_n_files);
}

ਪੂਰਨ conf_files_list_strv_at(
                ਅੱਖਰ ***ret,
                ਅਚਲ ਅੱਖਰ *suffix,
                ਪੂਰਨ rfd,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs) {

        _cleanup_hashmap_free_ Hashmap *fh = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (DEBUG_LOGGING)
                (ਖਾਲੀ) fd_get_ਰਸਤਾ(rfd, &ਜੜ੍ਹ); /* for logging */

        r = conf_files_list_impl(suffix, ਜੜ੍ਹ, rfd, flags, dirs, /* replacement= */ ਨਿਰਾ, &fh, /* ret_inserted= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ copy_and_ਛਾਂਟੋ_files_from_hashmap(fh, suffix, /* root= */ ਨਿਰਾ, flags, ret);
}

ਪੂਰਨ conf_files_list_strv_at_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *suffix,
                ਪੂਰਨ rfd,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs,
                ConfFile ***ret_files,
                size_t *ret_n_files) {

        _cleanup_hashmap_free_ Hashmap *fh = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(rfd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_files);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_n_files);

        ਜੇ (DEBUG_LOGGING)
                (ਖਾਲੀ) fd_get_ਰਸਤਾ(rfd, &ਜੜ੍ਹ); /* for logging */

        r = conf_files_list_impl(suffix, ਜੜ੍ਹ, rfd, flags, dirs, /* replacement= */ ਨਿਰਾ, &fh, /* ret_inserted= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ dump_files(fh, /* root= */ ਨਿਰਾ, flags, ret_files, ret_n_files);
}

ਪੂਰਨ conf_files_list(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir) {
        ਵਾਪਸ conf_files_list_strv(ret, suffix, ਜੜ੍ਹ, flags, STRV_MAKE_CONST(dir));
}

ਪੂਰਨ conf_files_list_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ਵਾਪਸ conf_files_list_strv_ਭਰਿਆ(suffix, ਜੜ੍ਹ, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ਪੂਰਨ conf_files_list_at(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir) {
        ਵਾਪਸ conf_files_list_strv_at(ret, suffix, rfd, flags, STRV_MAKE_CONST(dir));
}

ਪੂਰਨ conf_files_list_at_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir, ConfFile ***ret_files, size_t *ret_n_files) {
        ਵਾਪਸ conf_files_list_strv_at_ਭਰਿਆ(suffix, rfd, flags, STRV_MAKE_CONST(dir), ret_files, ret_n_files);
}

ਪੂਰਨ conf_files_list_nulstr(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs) {
        _cleanup_strv_free_ ਅੱਖਰ **d = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        d = strv_ਵੰਡੋ_nulstr(dirs);
        ਜੇ (!d)
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));

        ਵਾਪਸ conf_files_list_strv(ret, suffix, ਜੜ੍ਹ, flags, (ਅਚਲ ਅੱਖਰ**) d);
}

ਪੂਰਨ conf_files_list_nulstr_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ਅੱਖਰ **d = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_files);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_n_files);

        d = strv_ਵੰਡੋ_nulstr(dirs);
        ਜੇ (!d)
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));

        ਵਾਪਸ conf_files_list_strv_ਭਰਿਆ(suffix, ਜੜ੍ਹ, flags, (ਅਚਲ ਅੱਖਰ**) d, ret_files, ret_n_files);
}

ਪੂਰਨ conf_files_list_nulstr_at(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs) {
        _cleanup_strv_free_ ਅੱਖਰ **d = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        d = strv_ਵੰਡੋ_nulstr(dirs);
        ਜੇ (!d)
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));

        ਵਾਪਸ conf_files_list_strv_at(ret, suffix, rfd, flags, (ਅਚਲ ਅੱਖਰ**) d);
}

ਪੂਰਨ conf_files_list_nulstr_at_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs, ConfFile ***ret_files, size_t *ret_n_files) {
        _cleanup_strv_free_ ਅੱਖਰ **d = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_files);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_n_files);

        d = strv_ਵੰਡੋ_nulstr(dirs);
        ਜੇ (!d)
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));

        ਵਾਪਸ conf_files_list_strv_at_ਭਰਿਆ(suffix, rfd, flags, (ਅਚਲ ਅੱਖਰ**) d, ret_files, ret_n_files);
}

ਪੂਰਨ conf_files_list_with_replacement(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ **config_dirs,
                ਅਚਲ ਅੱਖਰ *replacement,
                ਅੱਖਰ ***ret_files,
                ਅੱਖਰ **ret_inserted) {

        _cleanup_hashmap_free_ Hashmap *fh = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *inserted = ਨਿਰਾ;
        ConfFilesFlags flags = CONF_FILES_REGULAR | CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_WARN;
        _cleanup_close_ ਪੂਰਨ rfd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *ਜੜ੍ਹ_abs = ਨਿਰਾ;
        _cleanup_strv_free_ ਅੱਖਰ **dirs_abs = ਨਿਰਾ;
        ਅਚਲ ConfFile *c = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_files);

        r = prepare_dirs(ਜੜ੍ਹ, flags, config_dirs, &ਜੜ੍ਹ_abs, &rfd, &dirs_abs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = conf_files_list_impl(".conf", ਜੜ੍ਹ_abs, rfd, flags, (ਅਚਲ ਅੱਖਰ * ਅਚਲ *) dirs_abs,
                                 replacement, &fh, ret_inserted ? &c : ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (c) {
                r = chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(c->ਨਤੀਜਾ, ਜੜ੍ਹ_abs, &inserted);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(conf_files_log_level(flags), r,
                                              "Failed to prefix '%s' with root '%s': %m",
                                              c->ਨਤੀਜਾ, ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ_abs));
        }

        r = copy_and_ਛਾਂਟੋ_files_from_hashmap(fh, ".conf", ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ_abs), flags, ret_files);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_inserted)
                *ret_inserted = TAKE_PTR(inserted);
        ਵਾਪਸ 0;
}

ਪੂਰਨ conf_files_list_dropins(
                ਅੱਖਰ ***ret,
                ਅਚਲ ਅੱਖਰ *dropin_dirname,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs) {

        _cleanup_strv_free_ ਅੱਖਰ **dropin_dirs = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *suffix;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(dropin_dirname);
        ਪ੍ਰਤਿਜ੍ਞਾ(dirs);

        suffix = strjoina("/", dropin_dirname);
        r = strv_extend_strv_concat(&dropin_dirs, dirs, suffix);
        ਜੇ (r < 0)
                ਵਾਪਸ log_oom_ਭਰਿਆ(conf_files_log_level(flags));

        ਵਾਪਸ conf_files_list_strv_at(ret, ".conf", ਜੜ੍ਹ_fd, flags, (ਅਚਲ ਅੱਖਰ* ਅਚਲ*) dropin_dirs);
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
ਪੂਰਨ conf_file_read(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅਚਲ ਅੱਖਰ **config_dirs,
                ਅਚਲ ਅੱਖਰ *fn,
                parse_ਸਤਰ_t parse_ਸਤਰ,
                ਖਾਲੀ *userdata,
                bool ignore_enoent,
                bool *invalid_config) {

        _cleanup_fclose_ FILE *_f = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *_fn = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ v = 0;
        FILE *f;
        ਪੂਰਨ r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fn);

        ਜੇ (streq(fn, "-")) {
                f = stdin;
                fn = "<stdin>";

                log_debug("Reading config from stdin%s", glyph(GLYPH_ELLIPSIS));

        } ਨਹੀਂਤਾਂ ਜੇ (is_ਰਸਤਾ(fn)) {
                r = ਰਸਤਾ_make_absolute_cwd(fn, &_fn);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_error_errno(r, "Failed to make path absolute: %m");
                fn = _fn;

                f = _f = fopen(fn, "re");
                ਜੇ (!_f)
                        r = -errno;
                ਨਹੀਂਤਾਂ
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));

        } ਨਹੀਂਤਾਂ {
                r = ਖੋਜ_and_fopen(fn, "re", ਜੜ੍ਹ, config_dirs, &_f, &_fn);
                ਜੇ (r >= 0) {
                        f = _f;
                        fn = _fn;
                        log_debug("Reading config file \"%s\"%s", fn, glyph(GLYPH_ELLIPSIS));
                }
        }

        ਜੇ (r == -ENOENT && ignore_enoent) {
                log_debug_errno(r, "Failed to open \"%s\", ignoring: %m", fn);
                ਵਾਪਸ 0; /* No error, but nothing happened. */
        }
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to read '%s': %m", fn);

        r = 1;  /* We entered the part where we may modify state. */

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                bool invalid_ਸਤਰ = false;
                ਪੂਰਨ k;

                k = read_stripped_ਸਤਰ(f, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (k < 0)
                        ਵਾਪਸ log_error_errno(k, "Failed to read '%s': %m", fn);
                ਜੇ (k == 0)
                        ਰੋਕੋ;

                v++;

                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(ਸਤਰ[0], 0, '#'))
                        ਜਾਰੀ;

                k = parse_ਸਤਰ(fn, v, ਸਤਰ, invalid_config ? &invalid_ਸਤਰ : ਨਿਰਾ, userdata);
                ਜੇ (k < 0 && invalid_ਸਤਰ)
                        /* Allow reporting with a special code if the caller requested this. */
                        *invalid_config = true;
                ਨਹੀਂਤਾਂ
                        /* The first error, if any, becomes our return value. */
                        RET_GATHER(r, k);
        }

        ਜੇ (ferror(f))
                RET_GATHER(r, log_error_errno(SYNTHETIC_ERRNO(EIO), "Failed to read from file %s.", fn));

        ਵਾਪਸ r;
}
