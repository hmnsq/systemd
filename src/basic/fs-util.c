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

ಪೂರ್ಣಾಙ್ಕ rmdir_parents(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *stop) {
        ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(stop);

        ಯದಿ (!path_is_safe(path))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!path_is_safe(stop))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        p = strdupa_safe(path);

        ಪ್ರತಿ (;;) {
                ವರ್ಣ *slash = ರಿಕ್ತ;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (ಧ್ರುವ ವರ್ಣ **) &slash, ರಿಕ್ತ);
                ಯದಿ (r <= 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (slash == p)
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಪ್ರತಿಜ್ಞಾ(*slash == '/');
                *slash = '\0';

                ಯದಿ (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (rmdir(p) < 0 && errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }
}

ಪೂರ್ಣಾಙ್ಕ rename_noreplace(ಪೂರ್ಣಾಙ್ಕ olddirfd, ಧ್ರುವ ವರ್ಣ *oldpath, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(oldpath);
        ಪ್ರತಿಜ್ಞಾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(newpath);

        /* Try the ideal approach first */
        ಯದಿ (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        ಯದಿ (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                ಯದಿ (r < 0) {
                        (ಶೂನ್ಯ) unlinkat(newdirfd, newpath, 0);
                        ಪ್ರತ್ಯಾಗಮ r;
                }

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        ಯದಿ (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                ಪ್ರತ್ಯಾಗಮ -EEXIST;
        ಯದಿ (errno != ENOENT)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

ಪೂರ್ಣಾಙ್ಕ readlinkat_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret) {
        size_t l = PATH_MAX;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        ಯದಿ (fd < 0 && isempty(p))
                ಪ್ರತ್ಯಾಗಮ -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *c = ರಿಕ್ತ;
                ssize_t n;

                c = new(ವರ್ಣ, l+1);
                ಯದಿ (!c)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ ((size_t) n < l) {
                        c[n] = 0;

                        ಯದಿ (ret)
                                *ret = TAKE_PTR(c);

                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                ಯದಿ (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;

                l *= 2;
        }
}

ಪೂರ್ಣಾಙ್ಕ readlink_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *link = ರಿಕ್ತ, *name = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = readlink_malloc(p, &link);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = path_extract_filename(link, &name);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == O_DIRECTORY)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret = TAKE_PTR(name);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ readlink_and_make_absolute(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *target = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = readlink_malloc(p, &target);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ file_in_same_dir(p, target, ret);
}

ಪೂರ್ಣಾಙ್ಕ chmod_and_chown_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ಯದಿ (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                dir_fd = fd;

        } ಅನ್ಯಥಾ ಯದಿ (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                dir_fd = fd;
        }

        ಪ್ರತ್ಯಾಗಮ fchmod_and_chown(dir_fd, mode, uid, gid);
}

ಪೂರ್ಣಾಙ್ಕ fchmod_and_chown_with_fallback(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ಸಂರಚನಾ stat st;
        ಪೂರ್ಣಾಙ್ಕ r;

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

        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        ಯದಿ (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        ಅನ್ಯಥಾ ಯದಿ ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL; /* insist on the right file type if it was specified */

        ಯದಿ (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                ಯದಿ (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        ಯದಿ (r < 0) {
                                ಯದಿ (!path || r != -ENOSYS)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                ಯದಿ (chmod(path, minimal & 07777) < 0)
                                        ಪ್ರತ್ಯಾಗಮ -errno;
                        }
                }
        }

        ಯದಿ (do_chown)
                ಯದಿ (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                ಯದಿ (r < 0) {
                        ಯದಿ (!path || r != -ENOSYS)
                                ಪ್ರತ್ಯಾಗಮ r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        ಯದಿ (chmod(path, mode & 07777) < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                }
        }

        ಪ್ರತ್ಯಾಗಮ do_chown || do_chmod;
}

ಪೂರ್ಣಾಙ್ಕ fchmod_umask(ಪೂರ್ಣಾಙ್ಕ fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fchmod(fd, m & (~u)));
}

ಪೂರ್ಣಾಙ್ಕ fchmod_opath(ಪೂರ್ಣಾಙ್ಕ fd, mode_t m) {
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

        ಯದಿ (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ proc_fd_enoent_errno();
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ futimens_opath(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಸಂರಚನಾ timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

ಪೂರ್ಣಾಙ್ಕ stat_warn_permissions(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        ಯದಿ (!S_ISREG(st->st_mode))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        ಯದಿ (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        ಯದಿ (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_warn_permissions(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ fd) {
        ಸಂರಚನಾ stat st;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ stat_warn_permissions(path, &st);
}

ಪೂರ್ಣಾಙ್ಕ access_nofollow(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ mode) {
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

ಪೂರ್ಣಾಙ್ಕ access_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ mode) {
        /* Like access() but operates on an already open fd */

        ಯದಿ (fd == AT_FDCWD)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(access(".", mode));
        ಯದಿ (fd == XAT_FDROOT)
                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(access("/", mode));

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

ಪೂರ್ಣಾಙ್ಕ touch_fd(ಪೂರ್ಣಾಙ್ಕ fd, usec_t stamp) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (stamp == USEC_INFINITY)
                ಪ್ರತ್ಯಾಗಮ futimens_opath(fd, /* ts= */ ರಿಕ್ತ);

        ಸಂರಚನಾ timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        ಪ್ರತ್ಯಾಗಮ futimens_opath(fd, ts);
}

ಪೂರ್ಣಾಙ್ಕ touch_file(ಧ್ರುವ ವರ್ಣ *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ ret;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        ಯದಿ (parents)
                (ಶೂನ್ಯ) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ಯದಿ (fd < 0) {
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        ಪ್ರತ್ಯಾಗಮ RET_GATHER(ret, touch_fd(fd, stamp));
}

ಪೂರ್ಣಾಙ್ಕ touch(ಧ್ರುವ ವರ್ಣ *path) {
        ಪ್ರತ್ಯಾಗಮ touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

ಪೂರ್ಣಾಙ್ಕ symlinkat_idempotent(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *linkpath, bool make_relative) {
        _cleanup_free_ ವರ್ಣ *relpath = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(target);
        ಪ್ರತಿಜ್ಞಾ(linkpath);

        ಯದಿ (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                target = relpath;
        }

        ಯದಿ (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                ಯದಿ (errno != EEXIST)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                ಯದಿ (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        ಪ್ರತ್ಯಾಗಮ -EEXIST;
                ಯದಿ (r < 0) /* Any other error? In that case propagate it as is */
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        ಪ್ರತ್ಯಾಗಮ -EEXIST;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ symlinkat_atomic_full_label(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(target);
        ಪ್ರತಿಜ್ಞಾ(linkpath);

        _cleanup_free_ ವರ್ಣ *relpath = ರಿಕ್ತ;
        ಯದಿ (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                target = relpath;
        }

        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        r = tempfn_random(linkpath, ರಿಕ್ತ, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        bool call_label_ops_post = false;
        ಯದಿ (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        ಯದಿ (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        ಯದಿ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(atfd, t, 0);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ mknodat_atomic(ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, dev_t dev) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        r = tempfn_random(path, ರಿಕ್ತ, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (mknodat(atfd, t, mode, dev) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        ಯದಿ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(atfd, t, 0);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ mkfifoat_atomic(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode) {
        _cleanup_free_ ವರ್ಣ *t = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, ರಿಕ್ತ, &t);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (mkfifoat(dir_fd, t, mode) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        ಯದಿ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(dir_fd, t, 0);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_files_in_directory(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ ***ret_list) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        _cleanup_closedir_ DIR *d = ರಿಕ್ತ;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ -errno;

        FOREACH_DIRENT_ALL(de, d, ಪ್ರತ್ಯಾಗಮ -errno) {
                ಯದಿ (!dirent_is_file(de))
                        ಅನುವರ್ತ;

                ಯದಿ (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        ಯದಿ (!GREEDY_REALLOC(l, n + 2))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        l[n] = strdup(de->d_name);
                        ಯದಿ (!l[n])
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        l[++n] = ರಿಕ್ತ;
                } ಅನ್ಯಥಾ
                        n++;
        }

        ಯದಿ (ret_list)
                *ret_list = TAKE_PTR(l);

        ಪ್ರತ್ಯಾಗಮ n;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ getenv_tmp_dir(ಧ್ರುವ ವರ್ಣ **ret_path) {
        ಪೂರ್ಣಾಙ್ಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ಧ್ರುವ ವರ್ಣ *e;

                e = secure_getenv(n);
                ಯದಿ (!e)
                        ಅನುವರ್ತ;
                ಯದಿ (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        ಗಚ್ಛ next;
                }
                ಯದಿ (!path_is_normalized(e)) {
                        r = -EPERM;
                        ಗಚ್ಛ next;
                }

                r = is_dir(e, true);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ next;
                ಯದಿ (r == 0) {
                        r = -ENOTDIR;
                        ಗಚ್ಛ next;
                }

                *ret_path = e;
                ಪ್ರತ್ಯಾಗಮ 1;

        next:
                /* Remember first error, to make this more debuggable */
                ಯದಿ (ret >= 0)
                        ret = r;
        }

        ಯದಿ (ret < 0)
                ಪ್ರತ್ಯಾಗಮ ret;

        *ret_path = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tmp_dir_internal(ಧ್ರುವ ವರ್ಣ *def, ಧ್ರುವ ವರ್ಣ **ret) {
        ಧ್ರುವ ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ r, k;

        ಪ್ರತಿಜ್ಞಾ(def);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = getenv_tmp_dir(&e);
        ಯದಿ (r > 0) {
                *ret = e;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        k = is_dir(def, /* follow= */ true);
        ಯದಿ (k == 0)
                k = -ENOTDIR;
        ಯದಿ (k < 0)
                ಪ್ರತ್ಯಾಗಮ RET_GATHER(r, k);

        *ret = def;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ var_tmp_dir(ಧ್ರುವ ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        ಪ್ರತ್ಯಾಗಮ tmp_dir_internal("/var/tmp", ret);
}

ಪೂರ್ಣಾಙ್ಕ tmp_dir(ಧ್ರುವ ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        ಪ್ರತ್ಯಾಗಮ tmp_dir_internal("/tmp", ret);
}

ಪೂರ್ಣಾಙ್ಕ unlink_or_warn(ಧ್ರುವ ವರ್ಣ *filename) {
        ಪ್ರತಿಜ್ಞಾ(filename);

        ಯದಿ (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                ಯದಿ (errno != EROFS || access(filename, F_OK) >= 0)
                        ಪ್ರತ್ಯಾಗಮ log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ *rmdir_and_free(ವರ್ಣ *p) {
        PROTECT_ERRNO;

        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        (ಶೂನ್ಯ) rmdir(p);
        ಪ್ರತ್ಯಾಗಮ mfree(p);
}

ವರ್ಣ* unlink_and_free(ವರ್ಣ *p) {
        PROTECT_ERRNO;

        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        (ಶೂನ್ಯ) unlink(p);
        ಪ್ರತ್ಯಾಗಮ mfree(p);
}

ಪೂರ್ಣಾಙ್ಕ unlinkat_deallocate(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ truncate_fd = -EBADF;
        ಸಂರಚನಾ stat st;
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

        ಯದಿ (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                ಯದಿ (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        ಯದಿ (IN_SET(errno, ENOENT, EISDIR))
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        ಯದಿ (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        ಯದಿ (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (!S_ISREG(st.st_mode))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t ಎಡ = st.st_size;
                ವರ್ಣ buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, ಪರಿಮಾಣ(buffer));

                ಯಾವತ್ (ಎಡ > 0) {
                        ssize_t n;

                        n = ಲಿಖ(truncate_fd, buffer, MIN(ಪರಿಮಾಣ(buffer), ಎಡ));
                        ಯದಿ (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                ಭಙ್ಗ;
                        }

                        ಪ್ರತಿಜ್ಞಾ(ಎಡ >= (size_t) n);
                        ಎಡ -= n;
                }

                /* Let's refresh metadata */
                ಯದಿ (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        ಯದಿ (st.st_blocks == 0 || st.st_nlink > 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        ಯದಿ (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                ಪ್ರತ್ಯಾಗಮ 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        ಯದಿ (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ open_parent_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode) {
        _cleanup_free_ ವರ್ಣ *parent = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(path);

        r = path_extract_directory(path, &parent);
        ಯದಿ (r == -EDESTADDRREQ) {
                parent = strdup(".");
                ಯದಿ (!parent)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ ಯದಿ (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                ಯದಿ (!parent)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        } ಅನ್ಯಥಾ ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        ಯದಿ (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

ಪೂರ್ಣಾಙ್ಕ conservative_renameat(
                ಪೂರ್ಣಾಙ್ಕ olddirfd, ಧ್ರುವ ವರ್ಣ *oldpath,
                ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ old_fd = -EBADF, new_fd = -EBADF;
        ಸಂರಚನಾ stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ಯದಿ (old_fd < 0)
                ಗಚ್ಛ do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ಯದಿ (new_fd < 0)
                ಗಚ್ಛ do_rename;

        ಯದಿ (fstat(old_fd, &old_stat) < 0)
                ಗಚ್ಛ do_rename;

        ಯದಿ (!S_ISREG(old_stat.st_mode))
                ಗಚ್ಛ do_rename;

        ಯದಿ (fstat(new_fd, &new_stat) < 0)
                ಗಚ್ಛ do_rename;

        ಯದಿ (stat_inode_same(&new_stat, &old_stat))
                ಗಚ್ಛ is_same;

        ಯದಿ (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                ಗಚ್ಛ do_rename;

        ಪ್ರತಿ (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[ಪರಿಮಾಣ(buf1)];
                ssize_t l1, l2;

                l1 = ಪಠ(old_fd, buf1, ಪರಿಮಾಣ(buf1));
                ಯದಿ (l1 < 0)
                        ಗಚ್ಛ do_rename;

                ಯದಿ (l1 == ಪರಿಮಾಣ(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = ಪಠ(new_fd, buf2, l1);
                ಅನ್ಯಥಾ {
                        ಪ್ರತಿಜ್ಞಾ((size_t) l1 < ಪರಿಮಾಣ(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        ಪ್ರತಿಜ್ಞಾ((size_t) (l1 + 1) <= ಪರಿಮಾಣ(buf2));
                        l2 = ಪಠ(new_fd, buf2, l1 + 1);
                }
                ಯದಿ (l2 != l1)
                        ಗಚ್ಛ do_rename;

                ಯದಿ (memcmp(buf1, buf2, l1) != 0)
                        ಗಚ್ಛ do_rename;

                ಯದಿ ((size_t) l1 < ಪರಿಮಾಣ(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        ಭಙ್ಗ;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        ಯದಿ (unlinkat(olddirfd, oldpath, 0) < 0)
                ಗಚ್ಛ do_rename;

        ಪ್ರತ್ಯಾಗಮ 0;

do_rename:
        ಯದಿ (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ posix_fallocate_loop(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        ಯದಿ (r != EINTR)
                ಪ್ರತ್ಯಾಗಮ -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ಧ್ರುವ RateLimit) { 10 * USEC_PER_SEC, 16 };
        ಯಾವತ್ (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                ಯದಿ (r != EINTR)
                        ಪ್ರತ್ಯಾಗಮ -r;
        }

        ಪ್ರತ್ಯಾಗಮ -EINTR;
}

ಪೂರ್ಣಾಙ್ಕ parse_cifs_service(
                ಧ್ರುವ ವರ್ಣ *s,
                ವರ್ಣ **ret_host,
                ವರ್ಣ **ret_service,
                ವರ್ಣ **ret_path) {

        _cleanup_free_ ವರ್ಣ *h = ರಿಕ್ತ, *ss = ರಿಕ್ತ, *x = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *p, *e, *d;
        ವರ್ಣ delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        p = startswith(s, "//");
        ಯದಿ (!p) {
                p = startswith(s, "\\\\");
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        h = strndup(p, e - p);
        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (!hostname_is_valid(h, 0))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        ಯದಿ (!ss)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (!filename_is_valid(ss))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                ಯದಿ (!x)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                ಪ್ರತಿ (ವರ್ಣ *i = x; *i; i++)
                        ಯದಿ (*i == delimiter)
                                *i = '/';

                ಯದಿ (!path_is_valid(x))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                path_simplify(x);
                ಯದಿ (!path_is_normalized(x))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಯದಿ (ret_host)
                *ret_host = TAKE_PTR(h);
        ಯದಿ (ret_service)
                *ret_service = TAKE_PTR(ss);
        ಯದಿ (ret_path)
                *ret_path = TAKE_PTR(x);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ open_mkdir_at_full_label(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ ವರ್ಣ *fname = ರಿಕ್ತ, *parent = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        ಯದಿ (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಯದಿ ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        ಯದಿ (r < 0) {
                ಯದಿ (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        ಪ್ರತ್ಯಾಗಮ r;
        } ಅನ್ಯಥಾ {
                r = path_extract_filename(path, &fname);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ಯದಿ (parent_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        ಯದಿ (IN_SET(fd, -ELOOP, -ENOTDIR))
                ಪ್ರತ್ಯಾಗಮ -EEXIST;
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಪೂರ್ಣಾಙ್ಕ openat_report_new(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *pathname, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode, bool *ret_newly_created) {
        ಪೂರ್ಣಾಙ್ಕ fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        ಯದಿ (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                ಪ್ರತ್ಯಾಗಮ fd;
        }

        ಪ್ರತಿ (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                ಯದಿ (fd >= 0) {
                        ಯದಿ (ret_newly_created)
                                *ret_newly_created = false;
                        ಪ್ರತ್ಯಾಗಮ fd;
                }
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                ಯದಿ (fd >= 0) {
                        ಯದಿ (ret_newly_created)
                                *ret_newly_created = true;
                        ಪ್ರತ್ಯಾಗಮ fd;
                }
                ಯದಿ (errno != EEXIST)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                ಯದಿ (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        ಪ್ರತ್ಯಾಗಮ -EEXIST;
        }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ openat_with_automount(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ಸ್ಥಿರ bool can_open_ವೃಕ್ಷ = true;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(open_flags, O_PATH));

        ಯದಿ (can_open_ವೃಕ್ಷ) {
                ಪೂರ್ಣಾಙ್ಕ fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                ಯದಿ (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        ಯದಿ (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                ಪೂರ್ಣಾಙ್ಕ q = fd_verify_directory(fd);
                                ಯದಿ (q < 0) {
                                        safe_close(fd);
                                        ಪ್ರತ್ಯಾಗಮ q;
                                }
                        }

                        ಪ್ರತ್ಯಾಗಮ fd;
                }
                ಯದಿ (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        ಪ್ರತ್ಯಾಗಮ fd;

                can_open_ವೃಕ್ಷ = false;
        }

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

ಪೂರ್ಣಾಙ್ಕ xopenat_full_label(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        bool made_dir = false, made_file = false;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        ಪ್ರತಿಜ್ಞಾ(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        ಪ್ರತಿಜ್ಞಾ(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        ಪ್ರತಿಜ್ಞಾ((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        ಯದಿ (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        ಯದಿ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                ಯದಿ (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } ಅನ್ಯಥಾ ಯದಿ (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಯದಿ (isempty(path)) {
                ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                ಯದಿ (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        ಯದಿ (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                ಪ್ರತ್ಯಾಗಮ fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ _dir_fd = -EBADF;
        ಯದಿ (dir_fd == XAT_FDROOT) {
                ಯದಿ (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                ಅನ್ಯಥಾ {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        ಯದಿ (_dir_fd < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        ಯದಿ (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                call_label_ops_post = true;
        }

        ಯದಿ (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                ಯದಿ (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                ಅನ್ಯಥಾ
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                ಯದಿ (r == -EEXIST) {
                        ಯದಿ (FLAGS_SET(open_flags, O_EXCL))
                                ಪ್ರತ್ಯಾಗಮ -EEXIST;
                } ಅನ್ಯಥಾ ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಅನ್ಯಥಾ
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        ಯದಿ (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                ಯದಿ (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        ಯದಿ (fd < 0) {
                                r = fd;
                                ಗಚ್ಛ error;
                        }

                        r = fd_verify_regular(fd);
                        ಯದಿ (r < 0)
                                ಗಚ್ಛ error;

                } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        ಯದಿ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                ಯದಿ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                ಅನ್ಯಥಾ ಯದಿ (fd < 0) {
                                        r = fd;
                                        ಗಚ್ಛ error;
                                }
                        }

                        ಯದಿ (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                ಯದಿ (fd < 0) {
                                        r = -errno;
                                        ಗಚ್ಛ error;
                                }
                        }

                        made_file = true;
                } ಅನ್ಯಥಾ {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        ಯದಿ (inode_fd < 0) {
                                ಯದಿ (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        ಗಚ್ಛ error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                ಯದಿ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        ಯದಿ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ಅನ್ಯಥಾ ಯದಿ (fd < 0) {
                                                r = fd;
                                                ಗಚ್ಛ error;
                                        }
                                }

                                ಯದಿ (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        ಯದಿ (fd < 0) {
                                                r = -errno;
                                                ಗಚ್ಛ error;
                                        }
                                }

                                made_file = true;
                        } ಅನ್ಯಥಾ {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                ಯದಿ (r < 0)
                                        ಗಚ್ಛ error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                ಯದಿ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        ಯದಿ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ಅನ್ಯಥಾ ಯದಿ (fd < 0) {
                                                r = fd;
                                                ಗಚ್ಛ error;
                                        }
                                }

                                ಯದಿ (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        ಯದಿ (fd < 0) {
                                                r = fd;
                                                ಗಚ್ಛ error;
                                        }
                                }
                        }
                }
        } ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                ಯದಿ (fd < 0) {
                        r = fd;
                        ಗಚ್ಛ error;
                }
        } ಅನ್ಯಥಾ {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                ಯದಿ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        ಯದಿ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        ಅನ್ಯಥಾ ಯದಿ (fd < 0) {
                                r = fd;
                                ಗಚ್ಛ error;
                        }
                }

                ಯದಿ (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        ಯದಿ (fd < 0) {
                                r = fd;
                                ಗಚ್ಛ error;
                        }
                }
        }

        ಯದಿ (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ error;
        }

        ಯದಿ (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ ರಿಕ್ತ, made_file || made_dir, label_context);
                ಯದಿ (r < 0)
                        ಗಚ್ಛ error;
        }

        ಯದಿ (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                ಯದಿ (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        ಗಚ್ಛ error;
        }

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);

error:
        ಯದಿ (call_label_ops_post)
                (ಶೂನ್ಯ) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ರಿಕ್ತ : path, made_dir || made_file, label_context);

        ಯದಿ (made_dir || made_file)
                (ಶೂನ್ಯ) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ xopenat_lock_full_label(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                ಪೂರ್ಣಾಙ್ಕ operation,
                LabelContext *label_context) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        ಯದಿ (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಪ್ರತಿ (;;) {
                ಸಂರಚನಾ stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ fd;

                r = lock_generic(fd, locktype, operation);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                ಯದಿ (fstat(fd, &st) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (st.st_nlink > 0)
                        ಭಙ್ಗ;

                fd = safe_close(fd);
        }

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಪೂರ್ಣಾಙ್ಕ link_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        ಯದಿ (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                ಯದಿ (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        ಪ್ರತ್ಯಾಗಮ -ENOSYS;
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ linkat_replace(ಪೂರ್ಣಾಙ್ಕ olddirfd, ಧ್ರುವ ವರ್ಣ *oldpath, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ old_fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        ಯದಿ (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಯದಿ (path_implies_directory(newpath))
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        /* First, try to link this directly */
        ಯದಿ (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        ಅನ್ಯಥಾ
                r = link_fd(olddirfd, newdirfd, newpath);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (r != -EEXIST)
                ಪ್ರತ್ಯಾಗಮ r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        ಯದಿ (old_fd < 0)
                ಪ್ರತ್ಯಾಗಮ old_fd;

        ಸಂರಚನಾ stat old_st;
        ಯದಿ (fstat(old_fd, &old_st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಸಂರಚನಾ stat new_st;
        ಯದಿ (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                ಪ್ರತ್ಯಾಗಮ -EEXIST;

        ಯದಿ (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                ಪ್ರತ್ಯಾಗಮ 0;

        _cleanup_free_ ವರ್ಣ *tmp_path = ರಿಕ್ತ;
        r = tempfn_random(newpath, /* extra= */ ರಿಕ್ತ, &tmp_path);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        ಯದಿ (r < 0) {
                ಯದಿ (!ERRNO_IS_PRIVILEGE(r))
                        ಪ್ರತ್ಯಾಗಮ r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        ಯದಿ (r < 0) {
                (ಶೂನ್ಯ) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}
