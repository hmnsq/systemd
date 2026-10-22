/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/falloc.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/file.h>
#ಅನ್ತರ್ಭಾವ <sys/mount.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "btrfs-util.h"
#ಅನ್ತರ್ಭಾವ "chattr-util.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "hostname-util.h"
#ಅನ್ತರ್ಭಾವ "label-util.h"
#ಅನ್ತರ್ಭಾವ "lock-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mkdir.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "ratelimit.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "tmpfile-util.h"
#ಅನ್ತರ್ಭಾವ "umask-util.h"

ಪೂರ್ಣಾಂಕ rmdir_parents(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *stop) {
        ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(stop);

        ಆದರೆ (!ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(stop))
                ಹಿಂತಿರುಗಿ -EINVAL;

        p = strdupa_ಸುರಕ್ಷಿತ(ಮಾರ್ಗ);

        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ *slash = ಖಾಲಿ;

                /* skip the last component. */
                r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_component(p, /* accept_dot_dot= */ false, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **) &slash, ಖಾಲಿ);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (slash == p)
                        ಹಿಂತಿರುಗಿ 0;

                ಪ್ರತಿಜ್ಞಾ(*slash == '/');
                *slash = '\0';

                ಆದರೆ (ಮಾರ್ಗ_startswith_ತುಂಬಿದ(stop, p, ಮಾರ್ಗ_STARTSWITH_REFUSE_DOT_DOT))
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (rmdir(p) < 0 && errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;
        }
}

ಪೂರ್ಣಾಂಕ rename_noreplace(ಪೂರ್ಣಾಂಕ olddirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(oldpath);
        ಪ್ರತಿಜ್ಞಾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(newpath);

        /* Try the ideal approach first */
        ಆದರೆ (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                ಹಿಂತಿರುಗಿ 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                ಹಿಂತಿರುಗಿ -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        ಆದರೆ (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                ಆದರೆ (r < 0) {
                        (ಶೂನ್ಯ) unlinkat(newdirfd, newpath, 0);
                        ಹಿಂತಿರುಗಿ r;
                }

                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ಒಳ_ಗಣ(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                ಹಿಂತಿರುಗಿ -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        ಆದರೆ (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                ಹಿಂತಿರುಗಿ -EEXIST;
        ಆದರೆ (errno != ENOENT)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

ಪೂರ್ಣಾಂಕ readlinkat_malloc(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret) {
        size_t l = PATH_MAX;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        ಆದರೆ (fd < 0 && isempty(p))
                ಹಿಂತಿರುಗಿ -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *c = ಖಾಲಿ;
                ssize_t n;

                c = new(ಅಕ್ಷರ, l+1);
                ಆದರೆ (!c)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ ((size_t) n < l) {
                        c[n] = 0;

                        ಆದರೆ (ret)
                                *ret = TAKE_PTR(c);

                        ಹಿಂತಿರುಗಿ 0;
                }

                ಆದರೆ (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        ಹಿಂತಿರುಗಿ -EFBIG;

                l *= 2;
        }
}

ಪೂರ್ಣಾಂಕ readlink_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *link = ಖಾಲಿ, *name = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = readlink_malloc(p, &link);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(link, &name);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == O_DIRECTORY)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret = TAKE_PTR(name);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ readlink_and_make_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಗುರಿ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = readlink_malloc(p, &ಗುರಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ file_ಒಳ_same_dir(p, ಗುರಿ, ret);
}

ಪೂರ್ಣಾಂಕ chmod_and_chown_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ಆದರೆ (ಮಾರ್ಗ) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC|O_NOFOLLOW);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                dir_fd = fd;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_ಮಾರ್ಗ|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                dir_fd = fd;
        }

        ಹಿಂತಿರುಗಿ fchmod_and_chown(dir_fd, ಬಹುಲಕ, uid, gid);
}

ಪೂರ್ಣಾಂಕ fchmod_and_chown_with_fallback(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ರಚನೆ stat st;
        ಪೂರ್ಣಾಂಕ r;

        /* Change ownership and access mode of the specified fd. Tries to do so safely, ensuring that at no
         * point in time the access mode is above the old access mode under the old ownership or the new
         * access mode under the new ownership. Note: this call tries hard to leave the access mode
         * unaffected if the uid/gid is changed, i.e. it undoes implicit suid/sgid dropping the kernel does
         * on chown().
         *
         * This call is happy with O_PATH fds.
         *
         * If path is given, allow a fallback path which does not use /proc/self/fd/. On any normal system
         * /proc will be mounted, but in certain improperly assembled environments it might not be. This is
         * less secure (potential TOCTOU), so should only be used after consideration. */

        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((ಬಹುಲಕ != ಬಹುಲಕ_INVALID && ((st.st_mode ^ ಬಹುಲಕ) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        ಆದರೆ (ಬಹುಲಕ == ಬಹುಲಕ_INVALID)
                ಬಹುಲಕ = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((ಬಹುಲಕ & S_IFMT) != 0 && ((ಬಹುಲಕ ^ st.st_mode) & S_IFMT) != 0)
                ಹಿಂತಿರುಗಿ -EINVAL; /* insist on the right file type if it was specified */

        ಆದರೆ (do_chown && do_chmod) {
                mode_t ಕನಿಷ್ಠತಮ = st.st_mode & ಬಹುಲಕ; /* the subset of the old and the new mask */

                ಆದರೆ (((ಕನಿಷ್ಠತಮ ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, ಕನಿಷ್ಠತಮ & 07777);
                        ಆದರೆ (r < 0) {
                                ಆದರೆ (!ಮಾರ್ಗ || r != -ENOSYS)
                                        ಹಿಂತಿರುಗಿ r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                ಆದರೆ (chmod(ಮಾರ್ಗ, ಕನಿಷ್ಠತಮ & 07777) < 0)
                                        ಹಿಂತಿರುಗಿ -errno;
                        }
                }
        }

        ಆದರೆ (do_chown)
                ಆದರೆ (fchownat(fd, "", uid, gid, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (do_chmod) {
                r = fchmod_opath(fd, ಬಹುಲಕ & 07777);
                ಆದರೆ (r < 0) {
                        ಆದರೆ (!ಮಾರ್ಗ || r != -ENOSYS)
                                ಹಿಂತಿರುಗಿ r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        ಆದರೆ (chmod(ಮಾರ್ಗ, ಬಹುಲಕ & 07777) < 0)
                                ಹಿಂತಿರುಗಿ -errno;
                }
        }

        ಹಿಂತಿರುಗಿ do_chown || do_chmod;
}

ಪೂರ್ಣಾಂಕ fchmod_umask(ಪೂರ್ಣಾಂಕ fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        ಹಿಂತಿರುಗಿ RET_NERRNO(fchmod(fd, m & (~u)));
}

ಪೂರ್ಣಾಂಕ fchmod_opath(ಪೂರ್ಣಾಂಕ fd, mode_t m) {
        /* This function operates also on fd that might have been opened with
         * O_PATH. The tool set we have is non-intuitive:
         * - fchmod(2) only operates on open files (i. e., fds with an open file description);
         * - fchmodat(2) does not have a flag arg like fchownat(2) does, so no way to pass AT_EMPTY_PATH;
         *   + it should not be confused with the libc fchmodat(3) interface, which adds 4th flag argument,
         *     and supports AT_EMPTY_PATH since v2.39 (previously only supported AT_SYMLINK_NOFOLLOW). So if
         *     the kernel has fchmodat2(2), since v2.39 glibc will call into it directly. If the kernel
         *     doesn't, or glibc is older than v2.39, glibc's internal fallback will return EINVAL if
         *     AT_EMPTY_PATH is passed.
         * - fchmodat2(2) supports all the AT_* flags, but is still very recent.
         *
         * We try to use fchmodat(3) first, and on EINVAL fall back to fchmodat2(), and, if that is also not
         * supported, resort to the /proc/self/fd dance. */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (fchmodat(fd, "", m, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ) >= 0)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (errno == EINVAL && fchmodat2(fd, "", m, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ) >= 0) /* glibc too old? */
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (!ಒಳ_ಗಣ(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (chmod(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), m) < 0) {
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ proc_fd_enoent_errno();
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ futimens_opath(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಹಿಂತಿರುಗಿ RET_NERRNO(utimensat(fd, "", ts, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ));
}

ಪೂರ್ಣಾಂಕ stat_warn_permissions(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        ಆದರೆ (!S_ISREG(st->st_mode))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", ಮಾರ್ಗ);

        ಆದರೆ (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", ಮಾರ್ಗ);

        ಆದರೆ (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", ಮಾರ್ಗ);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_warn_permissions(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ fd) {
        ರಚನೆ stat st;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ stat_warn_permissions(ಮಾರ್ಗ, &st);
}

ಪೂರ್ಣಾಂಕ access_nofollow(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ RET_NERRNO(faccessat(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, AT_SYMLINK_NOFOLLOW));
}

ಪೂರ್ಣಾಂಕ access_fd(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ) {
        /* Like access() but operates on an already open fd */

        ಆದರೆ (fd == AT_FDCWD)
                ಹಿಂತಿರುಗಿ RET_NERRNO(access(".", ಬಹುಲಕ));
        ಆದರೆ (fd == XAT_FDROOT)
                ಹಿಂತಿರುಗಿ RET_NERRNO(access("/", ಬಹುಲಕ));

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಹಿಂತಿರುಗಿ RET_NERRNO(faccessat(fd, "", ಬಹುಲಕ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ));
}

ಪೂರ್ಣಾಂಕ touch_fd(ಪೂರ್ಣಾಂಕ fd, usec_t stamp) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (stamp == USEC_INFINITY)
                ಹಿಂತಿರುಗಿ futimens_opath(fd, /* ts= */ ಖಾಲಿ);

        ರಚನೆ timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        ಹಿಂತಿರುಗಿ futimens_opath(fd, ts);
}

ಪೂರ್ಣಾಂಕ touch_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t ಬಹುಲಕ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ ret;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        ಆದರೆ (parents)
                (ಶೂನ್ಯ) mkdir_parents(ಮಾರ್ಗ, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC|O_NOFOLLOW);
        ಆದರೆ (fd < 0) {
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(ಮಾರ್ಗ, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, ಒಳ_ಗಣ(ಬಹುಲಕ, 0, ಬಹುಲಕ_INVALID) ? 0644 : ಬಹುಲಕ);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, ಬಹುಲಕ, uid, gid);

        ಹಿಂತಿರುಗಿ RET_GATHER(ret, touch_fd(fd, stamp));
}

ಪೂರ್ಣಾಂಕ touch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ touch_file(ಮಾರ್ಗ, false, USEC_INFINITY, UID_INVALID, GID_INVALID, ಬಹುಲಕ_INVALID);
}

ಪೂರ್ಣಾಂಕ symlinkat_idempotent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath, bool make_relative) {
        _cleanup_free_ ಅಕ್ಷರ *relpath = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);
        ಪ್ರತಿಜ್ಞಾ(linkpath);

        ಆದರೆ (make_relative) {
                r = ಮಾರ್ಗ_make_relative_ಪೋಷಕ(linkpath, ಗುರಿ, &relpath);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಗುರಿ = relpath;
        }

        ಆದರೆ (symlinkat(ಗುರಿ, atfd, linkpath) < 0) {
                _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                ಆದರೆ (errno != EEXIST)
                        ಹಿಂತಿರುಗಿ -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                ಆದರೆ (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        ಹಿಂತಿರುಗಿ -EEXIST;
                ಆದರೆ (r < 0) /* Any other error? In that case propagate it as is */
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (!streq(p, ಗುರಿ)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        ಹಿಂತಿರುಗಿ -EEXIST;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ symlinkat_atomic_ತುಂಬಿದ_label(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);
        ಪ್ರತಿಜ್ಞಾ(linkpath);

        _cleanup_free_ ಅಕ್ಷರ *relpath = ಖಾಲಿ;
        ಆದರೆ (FLAGS_ಗಣ(flags, SYMLINK_MAKE_RELATIVE)) {
                r = ಮಾರ್ಗ_make_relative_ಪೋಷಕ(linkpath, ಗುರಿ, &relpath);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಗುರಿ = relpath;
        }

        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        r = tempfn_random(linkpath, ಖಾಲಿ, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        bool call_label_ops_post = false;
        ಆದರೆ (FLAGS_ಗಣ(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(ಗುರಿ, atfd, t));
        ಆದರೆ (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        ಆದರೆ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(atfd, t, 0);
                ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ mknodat_atomic(ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, dev_t dev) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = tempfn_random(ಮಾರ್ಗ, ಖಾಲಿ, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (mknodat(atfd, t, ಬಹುಲಕ, dev) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, ಮಾರ್ಗ));
        ಆದರೆ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(atfd, t, 0);
                ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ mkfifoat_atomic(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ) {
        _cleanup_free_ ಅಕ್ಷರ *t = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(ಮಾರ್ಗ, ಖಾಲಿ, &t);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (mkfifoat(dir_fd, t, ಬಹುಲಕ) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, ಮಾರ್ಗ));
        ಆದರೆ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(dir_fd, t, 0);
                ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_files_ಒಳ_directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ ***ret_list) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        _cleanup_closedir_ DIR *d = ಖಾಲಿ;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(ಮಾರ್ಗ);
        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ -errno;

        FOREACH_DIRENT_ALL(de, d, ಹಿಂತಿರುಗಿ -errno) {
                ಆದರೆ (!dirent_is_file(de))
                        ಮುಂದುವರಿ;

                ಆದರೆ (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        ಆದರೆ (!GREEDY_REALLOC(l, n + 2))
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        l[n] = strdup(de->d_name);
                        ಆದರೆ (!l[n])
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        l[++n] = ಖಾಲಿ;
                } ಇಲ್ಲದಿದ್ದರೆ
                        n++;
        }

        ಆದರೆ (ret_list)
                *ret_list = TAKE_PTR(l);

        ಹಿಂತಿರುಗಿ n;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ getenv_tmp_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಪೂರ್ಣಾಂಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(ret_ಮಾರ್ಗ);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

                e = secure_getenv(n);
                ಆದರೆ (!e)
                        ಮುಂದುವರಿ;
                ಆದರೆ (!ಮಾರ್ಗ_is_absolute(e)) {
                        r = -ENOTDIR;
                        ಹೋಗು ಮುಂದಿನ_2;
                }
                ಆದರೆ (!ಮಾರ್ಗ_is_normalized(e)) {
                        r = -EPERM;
                        ಹೋಗು ಮುಂದಿನ_2;
                }

                r = is_dir(e, true);
                ಆದರೆ (r < 0)
                        ಹೋಗು ಮುಂದಿನ_2;
                ಆದರೆ (r == 0) {
                        r = -ENOTDIR;
                        ಹೋಗು ಮುಂದಿನ_2;
                }

                *ret_ಮಾರ್ಗ = e;
                ಹಿಂತಿರುಗಿ 1;

        ಮುಂದಿನ_2:
                /* Remember first error, to make this more debuggable */
                ಆದರೆ (ret >= 0)
                        ret = r;
        }

        ಆದರೆ (ret < 0)
                ಹಿಂತಿರುಗಿ ret;

        *ret_ಮಾರ್ಗ = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tmp_dir_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *def, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ r, k;

        ಪ್ರತಿಜ್ಞಾ(def);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = getenv_tmp_dir(&e);
        ಆದರೆ (r > 0) {
                *ret = e;
                ಹಿಂತಿರುಗಿ 0;
        }

        k = is_dir(def, /* follow= */ true);
        ಆದರೆ (k == 0)
                k = -ENOTDIR;
        ಆದರೆ (k < 0)
                ಹಿಂತಿರುಗಿ RET_GATHER(r, k);

        *ret = def;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ var_tmp_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        ಹಿಂತಿರುಗಿ tmp_dir_ಆಂತರಿಕ("/var/tmp", ret);
}

ಪೂರ್ಣಾಂಕ tmp_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        ಹಿಂತಿರುಗಿ tmp_dir_ಆಂತರಿಕ("/tmp", ret);
}

ಪೂರ್ಣಾಂಕ unlink_or_warn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) {
        ಪ್ರತಿಜ್ಞಾ(filename);

        ಆದರೆ (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                ಆದರೆ (errno != EROFS || access(filename, F_OK) >= 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ *rmdir_and_free(ಅಕ್ಷರ *p) {
        PROTECT_ERRNO;

        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        (ಶೂನ್ಯ) rmdir(p);
        ಹಿಂತಿರುಗಿ mfree(p);
}

ಅಕ್ಷರ* unlink_and_free(ಅಕ್ಷರ *p) {
        PROTECT_ERRNO;

        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        (ಶೂನ್ಯ) unlink(p);
        ಹಿಂತಿರುಗಿ mfree(p);
}

ಪೂರ್ಣಾಂಕ unlinkat_deallocate(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ truncate_fd = -EBADF;
        ರಚನೆ stat st;
        off_t l, bs;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

        /* Operates like unlinkat() but also deallocates the file contents if it is a regular file and there's no other
         * link to it. This is useful to ensure that other processes that might have the file open for reading won't be
         * able to keep the data pinned on disk forever. This call is particular useful whenever we execute clean-up
         * jobs ("vacuuming"), where we want to make sure the data is really gone and the disk space released and
         * returned to the free pool.
         *
         * Deallocation is preferably done by FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE (👊) if supported, which means
         * the file won't change size. That's a good thing since we shouldn't needlessly trigger SIGBUS in other
         * programs that have mmap()ed the file. (The assumption here is that changing file contents to all zeroes
         * underneath those programs is the better choice than simply triggering SIGBUS in them which truncation does.)
         * However if hole punching is not implemented in the kernel or file system we'll fall back to normal file
         * truncation (🔪), as our goal of deallocating the data space trumps our goal of being nice to readers (💐).
         *
         * Note that we attempt deallocation, but failure to succeed with that is not considered fatal, as long as the
         * primary job – to delete the file – is accomplished. */

        ಆದರೆ (!FLAGS_ಗಣ(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                ಆದರೆ (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        ಆದರೆ (ಒಳ_ಗಣ(errno, ENOENT, EISDIR))
                                ಹಿಂತಿರುಗಿ -errno;

                        ಆದರೆ (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        ಆದರೆ (unlinkat(fd, name, FLAGS_ಗಣ(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (!S_ISREG(st.st_mode))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (FLAGS_ಗಣ(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t ಎಡ = st.st_size;
                ಅಕ್ಷರ ಬಫರ್[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_ಬೈಟುಗಳು(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));

                ಆಗ (ಎಡ > 0) {
                        ssize_t n;

                        n = ಲಿಖ(truncate_fd, ಬಫರ್, MIN(ಗಾತ್ರ(ಬಫರ್), ಎಡ));
                        ಆದರೆ (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                ನಿಲ್ಲಿಸು;
                        }

                        ಪ್ರತಿಜ್ಞಾ(ಎಡ >= (size_t) n);
                        ಎಡ -= n;
                }

                /* Let's refresh metadata */
                ಆದರೆ (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        ಹಿಂತಿರುಗಿ 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        ಆದರೆ (st.st_blocks == 0 || st.st_nlink > 0)
                ಹಿಂತಿರುಗಿ 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ಸುತ್ತು_ಮೇಲೆ(st.st_size, bs); /* Round up to next block size */

        ಆದರೆ (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                ಹಿಂತಿರುಗಿ 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        ಆದರೆ (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ open_ಪೋಷಕ_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ) {
        _cleanup_free_ ಅಕ್ಷರ *ಪೋಷಕ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಮಾರ್ಗ, &ಪೋಷಕ);
        ಆದರೆ (r == -EDESTADDRREQ) {
                ಪೋಷಕ = strdup(".");
                ಆದರೆ (!ಪೋಷಕ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == -EADDRNOTAVAIL) {
                ಪೋಷಕ = strdup(ಮಾರ್ಗ);
                ಆದರೆ (!ಪೋಷಕ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        ಆದರೆ (!FLAGS_ಗಣ(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        ಹಿಂತಿರುಗಿ RET_NERRNO(openat(dir_fd, ಪೋಷಕ, flags, ಬಹುಲಕ));
}

ಪೂರ್ಣಾಂಕ conservative_renameat(
                ಪೂರ್ಣಾಂಕ olddirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath,
                ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ old_fd = -EBADF, new_fd = -EBADF;
        ರಚನೆ stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ಆದರೆ (old_fd < 0)
                ಹೋಗು do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ಆದರೆ (new_fd < 0)
                ಹೋಗು do_rename;

        ಆದರೆ (fstat(old_fd, &old_stat) < 0)
                ಹೋಗು do_rename;

        ಆದರೆ (!S_ISREG(old_stat.st_mode))
                ಹೋಗು do_rename;

        ಆದರೆ (fstat(new_fd, &new_stat) < 0)
                ಹೋಗು do_rename;

        ಆದರೆ (stat_inode_same(&new_stat, &old_stat))
                ಹೋಗು is_same;

        ಆದರೆ (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                ಹೋಗು do_rename;

        ಗಾಗಿ (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[ಗಾತ್ರ(buf1)];
                ssize_t l1, l2;

                l1 = ಪಠ(old_fd, buf1, ಗಾತ್ರ(buf1));
                ಆದರೆ (l1 < 0)
                        ಹೋಗು do_rename;

                ಆದರೆ (l1 == ಗಾತ್ರ(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = ಪಠ(new_fd, buf2, l1);
                ಇಲ್ಲದಿದ್ದರೆ {
                        ಪ್ರತಿಜ್ಞಾ((size_t) l1 < ಗಾತ್ರ(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        ಪ್ರತಿಜ್ಞಾ((size_t) (l1 + 1) <= ಗಾತ್ರ(buf2));
                        l2 = ಪಠ(new_fd, buf2, l1 + 1);
                }
                ಆದರೆ (l2 != l1)
                        ಹೋಗು do_rename;

                ಆದರೆ (memcmp(buf1, buf2, l1) != 0)
                        ಹೋಗು do_rename;

                ಆದರೆ ((size_t) l1 < ಗಾತ್ರ(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        ನಿಲ್ಲಿಸು;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        ಆದರೆ (unlinkat(olddirfd, oldpath, 0) < 0)
                ಹೋಗು do_rename;

        ಹಿಂತಿರುಗಿ 0;

do_rename:
        ಆದರೆ (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ posix_fallocate_loop(ಪೂರ್ಣಾಂಕ fd, uint64_t ಸ್ಥಾನಾಂತರ, uint64_t ಗಾತ್ರ_2) {
        ದರ_ಎಲ್ಲೆ rl;
        ಪೂರ್ಣಾಂಕ r;

        r = posix_fallocate(fd, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2); /* returns positive errnos on error */
        ಆದರೆ (r != EINTR)
                ಹಿಂತಿರುಗಿ -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ಸ್ಥಿರಾಂಕ ದರ_ಎಲ್ಲೆ) { 10 * USEC_ಪ್ರತಿ_SEC, 16 };
        ಆಗ (ratelimit_ಕೆಳಗೆ(&rl)) {
                r = posix_fallocate(fd, ಸ್ಥಾನಾಂತರ, ಗಾತ್ರ_2);
                ಆದರೆ (r != EINTR)
                        ಹಿಂತಿರುಗಿ -r;
        }

        ಹಿಂತಿರುಗಿ -EINTR;
}

ಪೂರ್ಣಾಂಕ parse_cifs_service(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s,
                ಅಕ್ಷರ **ret_host,
                ಅಕ್ಷರ **ret_service,
                ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {

        _cleanup_free_ ಅಕ್ಷರ *h = ಖಾಲಿ, *ss = ಖಾಲಿ, *x = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *e, *d;
        ಅಕ್ಷರ delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -EINVAL;

        p = startswith(s, "//");
        ಆದರೆ (!p) {
                p = startswith(s, "\\\\");
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -EINVAL;

        h = strndup(p, e - p);
        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (!hostname_is_valid(h, 0))
                ಹಿಂತಿರುಗಿ -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        ಆದರೆ (!ss)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (!filename_is_valid(ss))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_ಗಮ್ಯ_STR(delimiter)));
                ಆದರೆ (!x)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                ಗಾಗಿ (ಅಕ್ಷರ *i = x; *i; i++)
                        ಆದರೆ (*i == delimiter)
                                *i = '/';

                ಆದರೆ (!ಮಾರ್ಗ_is_valid(x))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                ಮಾರ್ಗ_simplify(x);
                ಆದರೆ (!ಮಾರ್ಗ_is_normalized(x))
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (ret_host)
                *ret_host = TAKE_PTR(h);
        ಆದರೆ (ret_service)
                *ret_service = TAKE_PTR(ss);
        ಆದರೆ (ret_ಮಾರ್ಗ)
                *ret_ಮಾರ್ಗ = TAKE_PTR(x);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ open_mkdir_at_ತುಂಬಿದ_label(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ, LabelContext *label_context) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF, ಪೋಷಕ_fd = -EBADF;
        _cleanup_free_ ಅಕ್ಷರ *fname = ಖಾಲಿ, *ಪೋಷಕ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        ಆದರೆ (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಮಾರ್ಗ, &ಪೋಷಕ);
        ಆದರೆ (r < 0) {
                ಆದರೆ (!ಒಳ_ಗಣ(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        ಹಿಂತಿರುಗಿ r;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, &fname);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಪೋಷಕ_fd = openat(dirfd, ಪೋಷಕ, O_ಮಾರ್ಗ|O_DIRECTORY|O_CLOEXEC);
                ಆದರೆ (ಪೋಷಕ_fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                dirfd = ಪೋಷಕ_fd;
                ಮಾರ್ಗ = fname;
        }

        fd = xopenat_ತುಂಬಿದ_label(dirfd, ಮಾರ್ಗ, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, ಬಹುಲಕ, label_context);
        ಆದರೆ (ಒಳ_ಗಣ(fd, -ELOOP, -ENOTDIR))
                ಹಿಂತಿರುಗಿ -EEXIST;
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಪೂರ್ಣಾಂಕ openat_report_new(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ, bool *ret_newly_created) {
        ಪೂರ್ಣಾಂಕ fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        ಆದರೆ (!FLAGS_ಗಣ(flags, O_CREAT) || FLAGS_ಗಣ(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, ಬಹುಲಕ);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (ret_newly_created)
                        *ret_newly_created = FLAGS_ಗಣ(flags, O_CREAT);
                ಹಿಂತಿರುಗಿ fd;
        }

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), ಬಹುಲಕ);
                ಆದರೆ (fd >= 0) {
                        ಆದರೆ (ret_newly_created)
                                *ret_newly_created = false;
                        ಹಿಂತಿರುಗಿ fd;
                }
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, ಬಹುಲಕ);
                ಆದರೆ (fd >= 0) {
                        ಆದರೆ (ret_newly_created)
                                *ret_newly_created = true;
                        ಹಿಂತಿರುಗಿ fd;
                }
                ಆದರೆ (errno != EEXIST)
                        ಹಿಂತಿರುಗಿ -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                ಆದರೆ (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        ಹಿಂತಿರುಗಿ -EEXIST;
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ openat_with_automount(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, mode_t ಬಹುಲಕ) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ಸ್ಥಿರ bool can_open_ವೃಕ್ಷ = true;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(open_flags, O_ಮಾರ್ಗ));

        ಆದರೆ (can_open_ವೃಕ್ಷ) {
                ಪೂರ್ಣಾಂಕ fd = RET_NERRNO(open_tree(dir_fd, ಮಾರ್ಗ,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_ಗಣ(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                ಆದರೆ (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        ಆದರೆ (FLAGS_ಗಣ(open_flags, O_DIRECTORY)) {
                                ಪೂರ್ಣಾಂಕ q = fd_verify_directory(fd);
                                ಆದರೆ (q < 0) {
                                        ಸುರಕ್ಷಿತ_close(fd);
                                        ಹಿಂತಿರುಗಿ q;
                                }
                        }

                        ಹಿಂತಿರುಗಿ fd;
                }
                ಆದರೆ (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        ಹಿಂತಿರುಗಿ fd;

                can_open_ವೃಕ್ಷ = false;
        }

        ಹಿಂತಿರುಗಿ RET_NERRNO(openat(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ));
}

ಪೂರ್ಣಾಂಕ xopenat_ತುಂಬಿದ_label(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ, LabelContext *label_context) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        bool made_dir = false, made_file = false;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(open_flags, O_DIRECTORY) + FLAGS_ಗಣ(xopen_flags, XO_REGULAR) + FLAGS_ಗಣ(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(xopen_flags, XO_SOCKET) || FLAGS_ಗಣ(open_flags, O_ಮಾರ್ಗ));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_ಗಣ(open_flags, O_ಮಾರ್ಗ) && !FLAGS_ಗಣ(open_flags, O_CREAT)));
        ಪ್ರತಿಜ್ಞಾ(!(FLAGS_ಗಣ(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_ಗಣ(xopen_flags, XO_SUBVOLUME)));
        ಪ್ರತಿಜ್ಞಾ(!(FLAGS_ಗಣ(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        ಪ್ರತಿಜ್ಞಾ((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

        /* This is like openat(), but has a few tricks up its sleeves, extending behaviour:
         *
         *   • O_DIRECTORY|O_CREAT is supported, which causes a directory to be created, and immediately
         *     opened. When used with the XO_SUBVOLUME flag this will even create a btrfs subvolume.
         *
         *   • If O_CREAT is used with XO_LABEL, any created file will be immediately relabelled.
         *
         *   • If the path is specified NULL or empty, behaves like fd_reopen().
         *
         *   • If XO_COW or XO_NOCOW is specified will turn off or on the NOCOW btrfs flag on the file, if
         *     available.
         *
         *   • if XO_REGULAR is specified will return an error if inode is not a regular file.
         *
         *   • if XO_SOCKET is specified will return an error if inode is not a socket.
         *
         *   • if XO_TRIGGER_AUTOMOUNT is specified O_PATH fds will trigger automounts.
         *
         *   • If mode is specified as MODE_INVALID, we'll use 0755 for dirs, and 0644 for regular files.
         *
         *   • The dir fd can be passed as XAT_FDROOT, in which case any relative paths will be taken relative to the root fs.
         *
         *   • If XO_AUTO_RW_RO is specified and the file cannot be opened in O_RDWR mode due to EACCES/EROFS or similar, retry in O_RDONLY mode.
         */

        ಆದರೆ (ಬಹುಲಕ == ಬಹುಲಕ_INVALID)
                ಬಹುಲಕ = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO)) {
                ಆದರೆ (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (open_flags & O_ಮಾರ್ಗ)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        ಆದರೆ (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                ಹಿಂತಿರುಗಿ TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                ಹಿಂತಿರುಗಿ fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ _dir_fd = -EBADF;
        ಆದರೆ (dir_fd == XAT_FDROOT) {
                ಆದರೆ (ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                        dir_fd = AT_FDCWD;
                ಇಲ್ಲದಿದ್ದರೆ {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_ಮಾರ್ಗ);
                        ಆದರೆ (_dir_fd < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        ಆದರೆ (FLAGS_ಗಣ(open_flags, O_CREAT) && FLAGS_ಗಣ(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, ಮಾರ್ಗ, FLAGS_ಗಣ(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                call_label_ops_post = true;
        }

        ಆದರೆ (FLAGS_ಗಣ(open_flags, O_DIRECTORY|O_CREAT)) {
                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ);
                ಇಲ್ಲದಿದ್ದರೆ
                        r = RET_NERRNO(mkdirat(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ));
                ಆದರೆ (r == -EEXIST) {
                        ಆದರೆ (FLAGS_ಗಣ(open_flags, O_EXCL))
                                ಹಿಂತಿರುಗಿ -EEXIST;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಇಲ್ಲದಿದ್ದರೆ
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                ಆದರೆ (FLAGS_ಗಣ(open_flags, O_ಮಾರ್ಗ)) {
                        fd = FLAGS_ಗಣ(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ) :
                                RET_NERRNO(openat(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ));
                        ಆದರೆ (fd < 0) {
                                r = fd;
                                ಹೋಗು error;
                        }

                        r = fd_verify_regular(fd);
                        ಆದರೆ (r < 0)
                                ಹೋಗು error;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, ಮಾರ್ಗ, open_flags|O_RDWR, ಬಹುಲಕ));
                                ಆದರೆ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd < 0) {
                                        r = fd;
                                        ಹೋಗು error;
                                }
                        }

                        ಆದರೆ (fd < 0) {
                                fd = openat(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ);
                                ಆದರೆ (fd < 0) {
                                        r = -errno;
                                        ಹೋಗು error;
                                }
                        }

                        made_file = true;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ ಪೂರ್ಣಾಂಕ inode_fd = openat(dir_fd, ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        ಆದರೆ (inode_fd < 0) {
                                ಆದರೆ (errno != ENOENT || !FLAGS_ಗಣ(open_flags, O_CREAT)) {
                                        r = -errno;
                                        ಹೋಗು error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, ಮಾರ್ಗ, open_flags|O_RDWR, ಬಹುಲಕ));
                                        ಆದರೆ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd < 0) {
                                                r = fd;
                                                ಹೋಗು error;
                                        }
                                }

                                ಆದರೆ (fd < 0) {
                                        fd = openat(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ);
                                        ಆದರೆ (fd < 0) {
                                                r = -errno;
                                                ಹೋಗು error;
                                        }
                                }

                                made_file = true;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                ಆದರೆ (r < 0)
                                        ಹೋಗು error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        ಆದರೆ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd < 0) {
                                                r = fd;
                                                ಹೋಗು error;
                                        }
                                }

                                ಆದರೆ (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        ಆದರೆ (fd < 0) {
                                                r = fd;
                                                ಹೋಗು error;
                                        }
                                }
                        }
                }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ);
                ಆದರೆ (fd < 0) {
                        r = fd;
                        ಹೋಗು error;
                }
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, ಮಾರ್ಗ, O_RDWR|open_flags, ಬಹುಲಕ, &made_file);
                        ಆದರೆ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd < 0) {
                                r = fd;
                                ಹೋಗು error;
                        }
                }

                ಆದರೆ (fd < 0) {
                        fd = openat_report_new(dir_fd, ಮಾರ್ಗ, open_flags, ಬಹುಲಕ, &made_file);
                        ಆದರೆ (fd < 0) {
                                r = fd;
                                ಹೋಗು error;
                        }
                }
        }

        ಆದರೆ (FLAGS_ಗಣ(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                ಆದರೆ (r < 0)
                        ಹೋಗು error;
        }

        ಆದರೆ (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ ಖಾಲಿ, made_file || made_dir, label_context);
                ಆದರೆ (r < 0)
                        ಹೋಗು error;
        }

        ಆದರೆ (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_ಗಣ(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                ಆದರೆ (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        ಹೋಗು error;
        }

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);

error:
        ಆದರೆ (call_label_ops_post)
                (ಶೂನ್ಯ) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ಖಾಲಿ : ಮಾರ್ಗ, made_dir || made_file, label_context);

        ಆದರೆ (made_dir || made_file)
                (ಶೂನ್ಯ) unlinkat(dir_fd, ಮಾರ್ಗ, made_dir ? AT_REMOVEDIR : 0);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ xopenat_ಬೀಗ_ತುಂಬಿದ_label(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ open_flags,
                XOpenFlags xopen_flags,
                mode_t ಬಹುಲಕ,
                ಬೀಗ_Type locktype,
                ಪೂರ್ಣಾಂಕ operation,
                LabelContext *label_context) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        ಆದರೆ (FLAGS_ಗಣ(open_flags, O_DIRECTORY) && !ಒಳ_ಗಣ(locktype, ಬೀಗ_BSD, ಬೀಗ_NONE))
                ಹಿಂತಿರುಗಿ -EBADF;

        ಗಾಗಿ (;;) {
                ರಚನೆ stat st;

                fd = xopenat_ತುಂಬಿದ_label(dir_fd, ಮಾರ್ಗ, open_flags, xopen_flags, ಬಹುಲಕ, label_context);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ fd;

                r = ಬೀಗ_generic(fd, locktype, operation);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                ಆದರೆ (fstat(fd, &st) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (st.st_nlink > 0)
                        ನಿಲ್ಲಿಸು;

                fd = ಸುರಕ್ಷಿತ_close(fd);
        }

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಪೂರ್ಣಾಂಕ link_fd(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ));
        ಆದರೆ (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_ಮಾರ್ಗ(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                ಆದರೆ (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        ಹಿಂತಿರುಗಿ -ENOSYS;
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ linkat_replace(ಪೂರ್ಣಾಂಕ olddirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ old_fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        ಆದರೆ (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (ಮಾರ್ಗ_ಸೂಚಿಸುತ್ತದೆ_directory(oldpath)) /* Refuse these definite directories early */
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಆದರೆ (ಮಾರ್ಗ_ಸೂಚಿಸುತ್ತದೆ_directory(newpath))
                ಹಿಂತಿರುಗಿ -EISDIR;

        /* First, try to link this directly */
        ಆದರೆ (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        ಇಲ್ಲದಿದ್ದರೆ
                r = link_fd(olddirfd, newdirfd, newpath);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (r != -EEXIST)
                ಹಿಂತಿರುಗಿ r;

        old_fd = xopenat(olddirfd, oldpath, O_ಮಾರ್ಗ|O_CLOEXEC);
        ಆದರೆ (old_fd < 0)
                ಹಿಂತಿರುಗಿ old_fd;

        ರಚನೆ stat old_st;
        ಆದರೆ (fstat(old_fd, &old_st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                ಹಿಂತಿರುಗಿ -EISDIR;

        ರಚನೆ stat new_st;
        ಆದರೆ (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                ಹಿಂತಿರುಗಿ -EEXIST;

        ಆದರೆ (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                ಹಿಂತಿರುಗಿ 0;

        _cleanup_free_ ಅಕ್ಷರ *tmp_ಮಾರ್ಗ = ಖಾಲಿ;
        r = tempfn_random(newpath, /* extra= */ ಖಾಲಿ, &tmp_ಮಾರ್ಗ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = link_fd(old_fd, newdirfd, tmp_ಮಾರ್ಗ);
        ಆದರೆ (r < 0) {
                ಆದರೆ (!ERRNO_IS_PRIVILEGE(r))
                        ಹಿಂತಿರುಗಿ r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_ಮಾರ್ಗ, 0));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_ಮಾರ್ಗ, newdirfd, newpath));
        ಆದರೆ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(newdirfd, tmp_ಮಾರ್ಗ, /* flags= */ 0);
                ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}
