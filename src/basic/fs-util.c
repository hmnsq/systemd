/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/falloc.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/file.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mount.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "btrfs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chattr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hostname-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "label-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lock-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mkdir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ratelimit.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tmpfile-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "umask-util.h"

ਪੂਰਨ rmdir_parents(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *stop) {
        ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(stop);

        ਜੇ (!ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(stop))
                ਵਾਪਸ -EINVAL;

        p = strdupa_ਸੁਰੱਖਿਅਤ(ਰਸਤਾ);

        ਲਈ (;;) {
                ਅੱਖਰ *slash = ਨਿਰਾ;

                /* skip the last component. */
                r = ਰਸਤਾ_ਲੱਭੋ_ਆਖਰੀ_component(p, /* accept_dot_dot= */ false, (ਅਚਲ ਅੱਖਰ **) &slash, ਨਿਰਾ);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;
                ਜੇ (slash == p)
                        ਵਾਪਸ 0;

                ਪ੍ਰਤਿਜ੍ਞਾ(*slash == '/');
                *slash = '\0';

                ਜੇ (ਰਸਤਾ_startswith_ਭਰਿਆ(stop, p, ਰਸਤਾ_STARTSWITH_REFUSE_DOT_DOT))
                        ਵਾਪਸ 0;

                ਜੇ (rmdir(p) < 0 && errno != ENOENT)
                        ਵਾਪਸ -errno;
        }
}

ਪੂਰਨ rename_noreplace(ਪੂਰਨ olddirfd, ਅਚਲ ਅੱਖਰ *oldpath, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(oldpath);
        ਪ੍ਰਤਿਜ੍ਞਾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(newpath);

        /* Try the ideal approach first */
        ਜੇ (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                ਵਾਪਸ 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        ਜੇ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                ਵਾਪਸ -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        ਜੇ (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                ਜੇ (r < 0) {
                        (ਖਾਲੀ) unlinkat(newdirfd, newpath, 0);
                        ਵਾਪਸ r;
                }

                ਵਾਪਸ 0;
        }

        ਜੇ (!ERRNO_IS_NOT_SUPPORTED(errno) && !ਵਿੱਚ_ਸੈੱਟ(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                ਵਾਪਸ -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        ਜੇ (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                ਵਾਪਸ -EEXIST;
        ਜੇ (errno != ENOENT)
                ਵਾਪਸ -errno;

        ਵਾਪਸ RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

ਪੂਰਨ readlinkat_malloc(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret) {
        size_t l = PATH_MAX;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);

        ਜੇ (fd < 0 && isempty(p))
                ਵਾਪਸ -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *c = ਨਿਰਾ;
                ssize_t n;

                c = new(ਅੱਖਰ, l+1);
                ਜੇ (!c)
                        ਵਾਪਸ -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;

                ਜੇ ((size_t) n < l) {
                        c[n] = 0;

                        ਜੇ (ret)
                                *ret = TAKE_PTR(c);

                        ਵਾਪਸ 0;
                }

                ਜੇ (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        ਵਾਪਸ -EFBIG;

                l *= 2;
        }
}

ਪੂਰਨ readlink_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *link = ਨਿਰਾ, *name = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = readlink_malloc(p, &link);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਰਸਤਾ_ਕੱਢੋ_filename(link, &name);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == O_DIRECTORY)
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(name);
        ਵਾਪਸ 0;
}

ਪੂਰਨ readlink_and_make_absolute(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = readlink_malloc(p, &ਨਿਸ਼ਾਨਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ file_ਵਿੱਚ_same_dir(p, ਨਿਸ਼ਾਨਾ, ret);
}

ਪੂਰਨ chmod_and_chown_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ਜੇ (ਰਸਤਾ) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC|O_NOFOLLOW);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;
                dir_fd = fd;

        } ਨਹੀਂਤਾਂ ਜੇ (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_ਰਸਤਾ|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;
                dir_fd = fd;
        }

        ਵਾਪਸ fchmod_and_chown(dir_fd, ਬਹੁਲਕ, uid, gid);
}

ਪੂਰਨ fchmod_and_chown_with_fallback(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ਬਣਤਰ stat st;
        ਪੂਰਨ r;

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

        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((ਬਹੁਲਕ != ਬਹੁਲਕ_INVALID && ((st.st_mode ^ ਬਹੁਲਕ) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        ਜੇ (ਬਹੁਲਕ == ਬਹੁਲਕ_INVALID)
                ਬਹੁਲਕ = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        ਨਹੀਂਤਾਂ ਜੇ ((ਬਹੁਲਕ & S_IFMT) != 0 && ((ਬਹੁਲਕ ^ st.st_mode) & S_IFMT) != 0)
                ਵਾਪਸ -EINVAL; /* insist on the right file type if it was specified */

        ਜੇ (do_chown && do_chmod) {
                mode_t ਨਿਊਨਤਮ = st.st_mode & ਬਹੁਲਕ; /* the subset of the old and the new mask */

                ਜੇ (((ਨਿਊਨਤਮ ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, ਨਿਊਨਤਮ & 07777);
                        ਜੇ (r < 0) {
                                ਜੇ (!ਰਸਤਾ || r != -ENOSYS)
                                        ਵਾਪਸ r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                ਜੇ (chmod(ਰਸਤਾ, ਨਿਊਨਤਮ & 07777) < 0)
                                        ਵਾਪਸ -errno;
                        }
                }
        }

        ਜੇ (do_chown)
                ਜੇ (fchownat(fd, "", uid, gid, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ) < 0)
                        ਵਾਪਸ -errno;

        ਜੇ (do_chmod) {
                r = fchmod_opath(fd, ਬਹੁਲਕ & 07777);
                ਜੇ (r < 0) {
                        ਜੇ (!ਰਸਤਾ || r != -ENOSYS)
                                ਵਾਪਸ r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        ਜੇ (chmod(ਰਸਤਾ, ਬਹੁਲਕ & 07777) < 0)
                                ਵਾਪਸ -errno;
                }
        }

        ਵਾਪਸ do_chown || do_chmod;
}

ਪੂਰਨ fchmod_umask(ਪੂਰਨ fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        ਵਾਪਸ RET_NERRNO(fchmod(fd, m & (~u)));
}

ਪੂਰਨ fchmod_opath(ਪੂਰਨ fd, mode_t m) {
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

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (fchmodat(fd, "", m, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ) >= 0)
                ਵਾਪਸ 0;
        ਜੇ (errno == EINVAL && fchmodat2(fd, "", m, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ) >= 0) /* glibc too old? */
                ਵਾਪਸ 0;
        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                ਵਾਪਸ -errno;

        ਜੇ (chmod(FORMAT_PROC_FD_ਰਸਤਾ(fd), m) < 0) {
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ proc_fd_enoent_errno();
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ futimens_opath(ਪੂਰਨ fd, ਅਚਲ ਬਣਤਰ timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਵਾਪਸ RET_NERRNO(utimensat(fd, "", ts, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ));
}

ਪੂਰਨ stat_warn_permissions(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਬਣਤਰ stat *st) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        ਜੇ (!S_ISREG(st->st_mode))
                ਵਾਪਸ 0;

        ਜੇ (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", ਰਸਤਾ);

        ਜੇ (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", ਰਸਤਾ);

        ਜੇ (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", ਰਸਤਾ);

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_warn_permissions(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ fd) {
        ਬਣਤਰ stat st;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ stat_warn_permissions(ਰਸਤਾ, &st);
}

ਪੂਰਨ access_nofollow(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ ਬਹੁਲਕ) {
        ਵਾਪਸ RET_NERRNO(faccessat(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, AT_SYMLINK_NOFOLLOW));
}

ਪੂਰਨ access_fd(ਪੂਰਨ fd, ਪੂਰਨ ਬਹੁਲਕ) {
        /* Like access() but operates on an already open fd */

        ਜੇ (fd == AT_FDCWD)
                ਵਾਪਸ RET_NERRNO(access(".", ਬਹੁਲਕ));
        ਜੇ (fd == XAT_FDROOT)
                ਵਾਪਸ RET_NERRNO(access("/", ਬਹੁਲਕ));

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਵਾਪਸ RET_NERRNO(faccessat(fd, "", ਬਹੁਲਕ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ));
}

ਪੂਰਨ touch_fd(ਪੂਰਨ fd, usec_t stamp) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (stamp == USEC_INFINITY)
                ਵਾਪਸ futimens_opath(fd, /* ts= */ ਨਿਰਾ);

        ਬਣਤਰ timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        ਵਾਪਸ futimens_opath(fd, ts);
}

ਪੂਰਨ touch_file(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t ਬਹੁਲਕ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ ret;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        ਜੇ (parents)
                (ਖਾਲੀ) mkdir_parents(ਰਸਤਾ, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC|O_NOFOLLOW);
        ਜੇ (fd < 0) {
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(ਰਸਤਾ, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, 0, ਬਹੁਲਕ_INVALID) ? 0644 : ਬਹੁਲਕ);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, ਬਹੁਲਕ, uid, gid);

        ਵਾਪਸ RET_GATHER(ret, touch_fd(fd, stamp));
}

ਪੂਰਨ touch(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਵਾਪਸ touch_file(ਰਸਤਾ, false, USEC_INFINITY, UID_INVALID, GID_INVALID, ਬਹੁਲਕ_INVALID);
}

ਪੂਰਨ symlinkat_idempotent(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *linkpath, bool make_relative) {
        _cleanup_free_ ਅੱਖਰ *relpath = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(linkpath);

        ਜੇ (make_relative) {
                r = ਰਸਤਾ_make_relative_ਮਾਪਾ(linkpath, ਨਿਸ਼ਾਨਾ, &relpath);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਨਿਸ਼ਾਨਾ = relpath;
        }

        ਜੇ (symlinkat(ਨਿਸ਼ਾਨਾ, atfd, linkpath) < 0) {
                _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                ਜੇ (errno != EEXIST)
                        ਵਾਪਸ -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                ਜੇ (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        ਵਾਪਸ -EEXIST;
                ਜੇ (r < 0) /* Any other error? In that case propagate it as is */
                        ਵਾਪਸ r;

                ਜੇ (!streq(p, ਨਿਸ਼ਾਨਾ)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        ਵਾਪਸ -EEXIST;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ symlinkat_atomic_ਭਰਿਆ_label(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(linkpath);

        _cleanup_free_ ਅੱਖਰ *relpath = ਨਿਰਾ;
        ਜੇ (FLAGS_ਸੈੱਟ(flags, SYMLINK_MAKE_RELATIVE)) {
                r = ਰਸਤਾ_make_relative_ਮਾਪਾ(linkpath, ਨਿਸ਼ਾਨਾ, &relpath);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਨਿਸ਼ਾਨਾ = relpath;
        }

        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        r = tempfn_random(linkpath, ਨਿਰਾ, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        bool call_label_ops_post = false;
        ਜੇ (FLAGS_ਸੈੱਟ(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(ਨਿਸ਼ਾਨਾ, atfd, t));
        ਜੇ (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        ਜੇ (r < 0) {
                (ਖਾਲੀ) unlinkat(atfd, t, 0);
                ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ mknodat_atomic(ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, dev_t dev) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = tempfn_random(ਰਸਤਾ, ਨਿਰਾ, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (mknodat(atfd, t, ਬਹੁਲਕ, dev) < 0)
                ਵਾਪਸ -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, ਰਸਤਾ));
        ਜੇ (r < 0) {
                (ਖਾਲੀ) unlinkat(atfd, t, 0);
                ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ mkfifoat_atomic(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ) {
        _cleanup_free_ ਅੱਖਰ *t = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(ਰਸਤਾ, ਨਿਰਾ, &t);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (mkfifoat(dir_fd, t, ਬਹੁਲਕ) < 0)
                ਵਾਪਸ -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, ਰਸਤਾ));
        ਜੇ (r < 0) {
                (ਖਾਲੀ) unlinkat(dir_fd, t, 0);
                ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ get_files_ਵਿੱਚ_directory(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ ***ret_list) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        _cleanup_closedir_ DIR *d = ਨਿਰਾ;
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(ਰਸਤਾ);
        ਜੇ (!d)
                ਵਾਪਸ -errno;

        FOREACH_DIRENT_ALL(de, d, ਵਾਪਸ -errno) {
                ਜੇ (!dirent_is_file(de))
                        ਜਾਰੀ;

                ਜੇ (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        ਜੇ (!GREEDY_REALLOC(l, n + 2))
                                ਵਾਪਸ -ENOMEM;

                        l[n] = strdup(de->d_name);
                        ਜੇ (!l[n])
                                ਵਾਪਸ -ENOMEM;

                        l[++n] = ਨਿਰਾ;
                } ਨਹੀਂਤਾਂ
                        n++;
        }

        ਜੇ (ret_list)
                *ret_list = TAKE_PTR(l);

        ਵਾਪਸ n;
}

ਸਥਿਰ ਪੂਰਨ getenv_tmp_dir(ਅਚਲ ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਪੂਰਨ r, ret = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਰਸਤਾ);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ਅਚਲ ਅੱਖਰ *e;

                e = secure_getenv(n);
                ਜੇ (!e)
                        ਜਾਰੀ;
                ਜੇ (!ਰਸਤਾ_is_absolute(e)) {
                        r = -ENOTDIR;
                        ਜਾਓ ਅਗਲਾ_2;
                }
                ਜੇ (!ਰਸਤਾ_is_normalized(e)) {
                        r = -EPERM;
                        ਜਾਓ ਅਗਲਾ_2;
                }

                r = is_dir(e, true);
                ਜੇ (r < 0)
                        ਜਾਓ ਅਗਲਾ_2;
                ਜੇ (r == 0) {
                        r = -ENOTDIR;
                        ਜਾਓ ਅਗਲਾ_2;
                }

                *ret_ਰਸਤਾ = e;
                ਵਾਪਸ 1;

        ਅਗਲਾ_2:
                /* Remember first error, to make this more debuggable */
                ਜੇ (ret >= 0)
                        ret = r;
        }

        ਜੇ (ret < 0)
                ਵਾਪਸ ret;

        *ret_ਰਸਤਾ = ਨਿਰਾ;
        ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ tmp_dir_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *def, ਅਚਲ ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *e;
        ਪੂਰਨ r, k;

        ਪ੍ਰਤਿਜ੍ਞਾ(def);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = getenv_tmp_dir(&e);
        ਜੇ (r > 0) {
                *ret = e;
                ਵਾਪਸ 0;
        }

        k = is_dir(def, /* follow= */ true);
        ਜੇ (k == 0)
                k = -ENOTDIR;
        ਜੇ (k < 0)
                ਵਾਪਸ RET_GATHER(r, k);

        *ret = def;
        ਵਾਪਸ 0;
}

ਪੂਰਨ var_tmp_dir(ਅਚਲ ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        ਵਾਪਸ tmp_dir_ਅੰਦਰੂਨੀ("/var/tmp", ret);
}

ਪੂਰਨ tmp_dir(ਅਚਲ ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        ਵਾਪਸ tmp_dir_ਅੰਦਰੂਨੀ("/tmp", ret);
}

ਪੂਰਨ unlink_or_warn(ਅਚਲ ਅੱਖਰ *filename) {
        ਪ੍ਰਤਿਜ੍ਞਾ(filename);

        ਜੇ (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                ਜੇ (errno != EROFS || access(filename, F_OK) >= 0)
                        ਵਾਪਸ log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        ਵਾਪਸ 0;
}

ਅੱਖਰ *rmdir_and_free(ਅੱਖਰ *p) {
        PROTECT_ERRNO;

        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        (ਖਾਲੀ) rmdir(p);
        ਵਾਪਸ mfree(p);
}

ਅੱਖਰ* unlink_and_free(ਅੱਖਰ *p) {
        PROTECT_ERRNO;

        ਜੇ (!p)
                ਵਾਪਸ ਨਿਰਾ;

        (ਖਾਲੀ) unlink(p);
        ਵਾਪਸ mfree(p);
}

ਪੂਰਨ unlinkat_deallocate(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ ਪੂਰਨ truncate_fd = -EBADF;
        ਬਣਤਰ stat st;
        off_t l, bs;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                ਜੇ (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(errno, ENOENT, EISDIR))
                                ਵਾਪਸ -errno;

                        ਜੇ (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        ਜੇ (unlinkat(fd, name, FLAGS_ਸੈੱਟ(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                ਵਾਪਸ 0;

        ਜੇ (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                ਵਾਪਸ 0;
        }

        ਜੇ (!S_ISREG(st.st_mode))
                ਵਾਪਸ 0;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t ਖੱਬਾ = st.st_size;
                ਅੱਖਰ ਬਫਰ[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_ਬਾਈਟਾਂ(ਬਫਰ, ਆਕਾਰ(ਬਫਰ));

                ਜਦੋਂ (ਖੱਬਾ > 0) {
                        ssize_t n;

                        n = ਲਿਖ(truncate_fd, ਬਫਰ, MIN(ਆਕਾਰ(ਬਫਰ), ਖੱਬਾ));
                        ਜੇ (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                ਰੋਕੋ;
                        }

                        ਪ੍ਰਤਿਜ੍ਞਾ(ਖੱਬਾ >= (size_t) n);
                        ਖੱਬਾ -= n;
                }

                /* Let's refresh metadata */
                ਜੇ (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        ਵਾਪਸ 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        ਜੇ (st.st_blocks == 0 || st.st_nlink > 0)
                ਵਾਪਸ 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ਗੇੜਾ_ਉੱਪਰ(st.st_size, bs); /* Round up to next block size */

        ਜੇ (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                ਵਾਪਸ 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        ਜੇ (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ open_ਮਾਪਾ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ) {
        _cleanup_free_ ਅੱਖਰ *ਮਾਪਾ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        r = ਰਸਤਾ_ਕੱਢੋ_directory(ਰਸਤਾ, &ਮਾਪਾ);
        ਜੇ (r == -EDESTADDRREQ) {
                ਮਾਪਾ = strdup(".");
                ਜੇ (!ਮਾਪਾ)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ ਜੇ (r == -EADDRNOTAVAIL) {
                ਮਾਪਾ = strdup(ਰਸਤਾ);
                ਜੇ (!ਮਾਪਾ)
                        ਵਾਪਸ -ENOMEM;
        } ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        ਵਾਪਸ RET_NERRNO(openat(dir_fd, ਮਾਪਾ, flags, ਬਹੁਲਕ));
}

ਪੂਰਨ conservative_renameat(
                ਪੂਰਨ olddirfd, ਅਚਲ ਅੱਖਰ *oldpath,
                ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath) {

        _cleanup_close_ ਪੂਰਨ old_fd = -EBADF, new_fd = -EBADF;
        ਬਣਤਰ stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ਜੇ (old_fd < 0)
                ਜਾਓ do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ਜੇ (new_fd < 0)
                ਜਾਓ do_rename;

        ਜੇ (fstat(old_fd, &old_stat) < 0)
                ਜਾਓ do_rename;

        ਜੇ (!S_ISREG(old_stat.st_mode))
                ਜਾਓ do_rename;

        ਜੇ (fstat(new_fd, &new_stat) < 0)
                ਜਾਓ do_rename;

        ਜੇ (stat_inode_same(&new_stat, &old_stat))
                ਜਾਓ is_same;

        ਜੇ (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                ਜਾਓ do_rename;

        ਲਈ (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[ਆਕਾਰ(buf1)];
                ssize_t l1, l2;

                l1 = ਪਠ(old_fd, buf1, ਆਕਾਰ(buf1));
                ਜੇ (l1 < 0)
                        ਜਾਓ do_rename;

                ਜੇ (l1 == ਆਕਾਰ(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = ਪਠ(new_fd, buf2, l1);
                ਨਹੀਂਤਾਂ {
                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l1 < ਆਕਾਰ(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) (l1 + 1) <= ਆਕਾਰ(buf2));
                        l2 = ਪਠ(new_fd, buf2, l1 + 1);
                }
                ਜੇ (l2 != l1)
                        ਜਾਓ do_rename;

                ਜੇ (memcmp(buf1, buf2, l1) != 0)
                        ਜਾਓ do_rename;

                ਜੇ ((size_t) l1 < ਆਕਾਰ(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        ਰੋਕੋ;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        ਜੇ (unlinkat(olddirfd, oldpath, 0) < 0)
                ਜਾਓ do_rename;

        ਵਾਪਸ 0;

do_rename:
        ਜੇ (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 1;
}

ਪੂਰਨ posix_fallocate_loop(ਪੂਰਨ fd, uint64_t ਵਿੱਥ, uint64_t ਮਾਪ) {
        ਦਰ_ਹੱਦਬੰਦੀ rl;
        ਪੂਰਨ r;

        r = posix_fallocate(fd, ਵਿੱਥ, ਮਾਪ); /* returns positive errnos on error */
        ਜੇ (r != EINTR)
                ਵਾਪਸ -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ਅਚਲ ਦਰ_ਹੱਦਬੰਦੀ) { 10 * USEC_ਪ੍ਰਤੀ_SEC, 16 };
        ਜਦੋਂ (ratelimit_ਹੇਠਾਂ(&rl)) {
                r = posix_fallocate(fd, ਵਿੱਥ, ਮਾਪ);
                ਜੇ (r != EINTR)
                        ਵਾਪਸ -r;
        }

        ਵਾਪਸ -EINTR;
}

ਪੂਰਨ parse_cifs_service(
                ਅਚਲ ਅੱਖਰ *s,
                ਅੱਖਰ **ret_host,
                ਅੱਖਰ **ret_service,
                ਅੱਖਰ **ret_ਰਸਤਾ) {

        _cleanup_free_ ਅੱਖਰ *h = ਨਿਰਾ, *ss = ਨਿਰਾ, *x = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *p, *e, *d;
        ਅੱਖਰ delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        ਜੇ (!s)
                ਵਾਪਸ -EINVAL;

        p = startswith(s, "//");
        ਜੇ (!p) {
                p = startswith(s, "\\\\");
                ਜੇ (!p)
                        ਵਾਪਸ -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        ਜੇ (!e)
                ਵਾਪਸ -EINVAL;

        h = strndup(p, e - p);
        ਜੇ (!h)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (!hostname_is_valid(h, 0))
                ਵਾਪਸ -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        ਜੇ (!ss)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (!filename_is_valid(ss))
                ਵਾਪਸ -EINVAL;

        ਜੇ (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_ਮੰਜ਼ਿਲ_STR(delimiter)));
                ਜੇ (!x)
                        ਵਾਪਸ -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                ਲਈ (ਅੱਖਰ *i = x; *i; i++)
                        ਜੇ (*i == delimiter)
                                *i = '/';

                ਜੇ (!ਰਸਤਾ_is_valid(x))
                        ਵਾਪਸ -EINVAL;

                ਰਸਤਾ_simplify(x);
                ਜੇ (!ਰਸਤਾ_is_normalized(x))
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (ret_host)
                *ret_host = TAKE_PTR(h);
        ਜੇ (ret_service)
                *ret_service = TAKE_PTR(ss);
        ਜੇ (ret_ਰਸਤਾ)
                *ret_ਰਸਤਾ = TAKE_PTR(x);

        ਵਾਪਸ 0;
}

ਪੂਰਨ open_mkdir_at_ਭਰਿਆ_label(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ, LabelContext *label_context) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF, ਮਾਪਾ_fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *fname = ਨਿਰਾ, *ਮਾਪਾ = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        ਜੇ (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;
        ਜੇ ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                ਵਾਪਸ -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = ਰਸਤਾ_ਕੱਢੋ_directory(ਰਸਤਾ, &ਮਾਪਾ);
        ਜੇ (r < 0) {
                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        ਵਾਪਸ r;
        } ਨਹੀਂਤਾਂ {
                r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, &fname);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਮਾਪਾ_fd = openat(dirfd, ਮਾਪਾ, O_ਰਸਤਾ|O_DIRECTORY|O_CLOEXEC);
                ਜੇ (ਮਾਪਾ_fd < 0)
                        ਵਾਪਸ -errno;

                dirfd = ਮਾਪਾ_fd;
                ਰਸਤਾ = fname;
        }

        fd = xopenat_ਭਰਿਆ_label(dirfd, ਰਸਤਾ, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, ਬਹੁਲਕ, label_context);
        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(fd, -ELOOP, -ENOTDIR))
                ਵਾਪਸ -EEXIST;
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਵਾਪਸ TAKE_FD(fd);
}

ਪੂਰਨ openat_report_new(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *pathname, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ, bool *ret_newly_created) {
        ਪੂਰਨ fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, O_CREAT) || FLAGS_ਸੈੱਟ(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, ਬਹੁਲਕ);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (ret_newly_created)
                        *ret_newly_created = FLAGS_ਸੈੱਟ(flags, O_CREAT);
                ਵਾਪਸ fd;
        }

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), ਬਹੁਲਕ);
                ਜੇ (fd >= 0) {
                        ਜੇ (ret_newly_created)
                                *ret_newly_created = false;
                        ਵਾਪਸ fd;
                }
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, ਬਹੁਲਕ);
                ਜੇ (fd >= 0) {
                        ਜੇ (ret_newly_created)
                                *ret_newly_created = true;
                        ਵਾਪਸ fd;
                }
                ਜੇ (errno != EEXIST)
                        ਵਾਪਸ -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                ਜੇ (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        ਵਾਪਸ -EEXIST;
        }
}

ਸਥਿਰ ਪੂਰਨ openat_with_automount(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, mode_t ਬਹੁਲਕ) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ਸਥਿਰ bool can_open_ਰੁੱਖ = true;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(open_flags, O_ਰਸਤਾ));

        ਜੇ (can_open_ਰੁੱਖ) {
                ਪੂਰਨ fd = RET_NERRNO(open_tree(dir_fd, ਰਸਤਾ,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_ਸੈੱਟ(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                ਜੇ (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_DIRECTORY)) {
                                ਪੂਰਨ q = fd_verify_directory(fd);
                                ਜੇ (q < 0) {
                                        ਸੁਰੱਖਿਅਤ_close(fd);
                                        ਵਾਪਸ q;
                                }
                        }

                        ਵਾਪਸ fd;
                }
                ਜੇ (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        ਵਾਪਸ fd;

                can_open_ਰੁੱਖ = false;
        }

        ਵਾਪਸ RET_NERRNO(openat(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ));
}

ਪੂਰਨ xopenat_ਭਰਿਆ_label(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ, LabelContext *label_context) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        bool made_dir = false, made_file = false;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(open_flags, O_DIRECTORY) + FLAGS_ਸੈੱਟ(xopen_flags, XO_REGULAR) + FLAGS_ਸੈੱਟ(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(xopen_flags, XO_SOCKET) || FLAGS_ਸੈੱਟ(open_flags, O_ਰਸਤਾ));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_ਸੈੱਟ(open_flags, O_ਰਸਤਾ) && !FLAGS_ਸੈੱਟ(open_flags, O_CREAT)));
        ਪ੍ਰਤਿਜ੍ਞਾ(!(FLAGS_ਸੈੱਟ(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_ਸੈੱਟ(xopen_flags, XO_SUBVOLUME)));
        ਪ੍ਰਤਿਜ੍ਞਾ(!(FLAGS_ਸੈੱਟ(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        ਪ੍ਰਤਿਜ੍ਞਾ((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        ਜੇ (ਬਹੁਲਕ == ਬਹੁਲਕ_INVALID)
                ਬਹੁਲਕ = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO)) {
                ਜੇ (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } ਨਹੀਂਤਾਂ ਜੇ (open_flags & O_ਰਸਤਾ)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        ਜੇ (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                ਵਾਪਸ TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                ਵਾਪਸ fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ ਪੂਰਨ _dir_fd = -EBADF;
        ਜੇ (dir_fd == XAT_FDROOT) {
                ਜੇ (ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                        dir_fd = AT_FDCWD;
                ਨਹੀਂਤਾਂ {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_ਰਸਤਾ);
                        ਜੇ (_dir_fd < 0)
                                ਵਾਪਸ -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_CREAT) && FLAGS_ਸੈੱਟ(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, ਰਸਤਾ, FLAGS_ਸੈੱਟ(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                call_label_ops_post = true;
        }

        ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_DIRECTORY|O_CREAT)) {
                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ);
                ਨਹੀਂਤਾਂ
                        r = RET_NERRNO(mkdirat(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ));
                ਜੇ (r == -EEXIST) {
                        ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_EXCL))
                                ਵਾਪਸ -EEXIST;
                } ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਨਹੀਂਤਾਂ
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_ਰਸਤਾ)) {
                        fd = FLAGS_ਸੈੱਟ(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ) :
                                RET_NERRNO(openat(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ));
                        ਜੇ (fd < 0) {
                                r = fd;
                                ਜਾਓ error;
                        }

                        r = fd_verify_regular(fd);
                        ਜੇ (r < 0)
                                ਜਾਓ error;

                } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, ਰਸਤਾ, open_flags|O_RDWR, ਬਹੁਲਕ));
                                ਜੇ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                ਨਹੀਂਤਾਂ ਜੇ (fd < 0) {
                                        r = fd;
                                        ਜਾਓ error;
                                }
                        }

                        ਜੇ (fd < 0) {
                                fd = openat(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ);
                                ਜੇ (fd < 0) {
                                        r = -errno;
                                        ਜਾਓ error;
                                }
                        }

                        made_file = true;
                } ਨਹੀਂਤਾਂ {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ ਪੂਰਨ inode_fd = openat(dir_fd, ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        ਜੇ (inode_fd < 0) {
                                ਜੇ (errno != ENOENT || !FLAGS_ਸੈੱਟ(open_flags, O_CREAT)) {
                                        r = -errno;
                                        ਜਾਓ error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, ਰਸਤਾ, open_flags|O_RDWR, ਬਹੁਲਕ));
                                        ਜੇ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ਨਹੀਂਤਾਂ ਜੇ (fd < 0) {
                                                r = fd;
                                                ਜਾਓ error;
                                        }
                                }

                                ਜੇ (fd < 0) {
                                        fd = openat(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ);
                                        ਜੇ (fd < 0) {
                                                r = -errno;
                                                ਜਾਓ error;
                                        }
                                }

                                made_file = true;
                        } ਨਹੀਂਤਾਂ {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                ਜੇ (r < 0)
                                        ਜਾਓ error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        ਜੇ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ਨਹੀਂਤਾਂ ਜੇ (fd < 0) {
                                                r = fd;
                                                ਜਾਓ error;
                                        }
                                }

                                ਜੇ (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        ਜੇ (fd < 0) {
                                                r = fd;
                                                ਜਾਓ error;
                                        }
                                }
                        }
                }
        } ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ);
                ਜੇ (fd < 0) {
                        r = fd;
                        ਜਾਓ error;
                }
        } ਨਹੀਂਤਾਂ {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, ਰਸਤਾ, O_RDWR|open_flags, ਬਹੁਲਕ, &made_file);
                        ਜੇ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        ਨਹੀਂਤਾਂ ਜੇ (fd < 0) {
                                r = fd;
                                ਜਾਓ error;
                        }
                }

                ਜੇ (fd < 0) {
                        fd = openat_report_new(dir_fd, ਰਸਤਾ, open_flags, ਬਹੁਲਕ, &made_file);
                        ਜੇ (fd < 0) {
                                r = fd;
                                ਜਾਓ error;
                        }
                }
        }

        ਜੇ (FLAGS_ਸੈੱਟ(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                ਜੇ (r < 0)
                        ਜਾਓ error;
        }

        ਜੇ (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ ਨਿਰਾ, made_file || made_dir, label_context);
                ਜੇ (r < 0)
                        ਜਾਓ error;
        }

        ਜੇ (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_ਸੈੱਟ(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                ਜੇ (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        ਜਾਓ error;
        }

        ਵਾਪਸ TAKE_FD(fd);

error:
        ਜੇ (call_label_ops_post)
                (ਖਾਲੀ) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ਨਿਰਾ : ਰਸਤਾ, made_dir || made_file, label_context);

        ਜੇ (made_dir || made_file)
                (ਖਾਲੀ) unlinkat(dir_fd, ਰਸਤਾ, made_dir ? AT_REMOVEDIR : 0);

        ਵਾਪਸ r;
}

ਪੂਰਨ xopenat_ਤਾਲਾ_ਭਰਿਆ_label(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ open_flags,
                XOpenFlags xopen_flags,
                mode_t ਬਹੁਲਕ,
                ਤਾਲਾ_Type locktype,
                ਪੂਰਨ operation,
                LabelContext *label_context) {

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        ਜੇ (FLAGS_ਸੈੱਟ(open_flags, O_DIRECTORY) && !ਵਿੱਚ_ਸੈੱਟ(locktype, ਤਾਲਾ_BSD, ਤਾਲਾ_NONE))
                ਵਾਪਸ -EBADF;

        ਲਈ (;;) {
                ਬਣਤਰ stat st;

                fd = xopenat_ਭਰਿਆ_label(dir_fd, ਰਸਤਾ, open_flags, xopen_flags, ਬਹੁਲਕ, label_context);
                ਜੇ (fd < 0)
                        ਵਾਪਸ fd;

                r = ਤਾਲਾ_generic(fd, locktype, operation);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                ਜੇ (fstat(fd, &st) < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (st.st_nlink > 0)
                        ਰੋਕੋ;

                fd = ਸੁਰੱਖਿਅਤ_close(fd);
        }

        ਵਾਪਸ TAKE_FD(fd);
}

ਪੂਰਨ link_fd(ਪੂਰਨ fd, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ));
        ਜੇ (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_ਰਸਤਾ(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                ਜੇ (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        ਵਾਪਸ -ENOSYS;
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ linkat_replace(ਪੂਰਨ olddirfd, ਅਚਲ ਅੱਖਰ *oldpath, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath) {
        _cleanup_close_ ਪੂਰਨ old_fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        ਜੇ (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                ਵਾਪਸ -EISDIR;

        ਜੇ (ਰਸਤਾ_ਸਿੱਟਾ_ਦਿੰਦਾ_directory(oldpath)) /* Refuse these definite directories early */
                ਵਾਪਸ -EISDIR;

        ਜੇ (ਰਸਤਾ_ਸਿੱਟਾ_ਦਿੰਦਾ_directory(newpath))
                ਵਾਪਸ -EISDIR;

        /* First, try to link this directly */
        ਜੇ (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        ਨਹੀਂਤਾਂ
                r = link_fd(olddirfd, newdirfd, newpath);
        ਜੇ (r >= 0)
                ਵਾਪਸ 0;
        ਜੇ (r != -EEXIST)
                ਵਾਪਸ r;

        old_fd = xopenat(olddirfd, oldpath, O_ਰਸਤਾ|O_CLOEXEC);
        ਜੇ (old_fd < 0)
                ਵਾਪਸ old_fd;

        ਬਣਤਰ stat old_st;
        ਜੇ (fstat(old_fd, &old_st) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                ਵਾਪਸ -EISDIR;

        ਬਣਤਰ stat new_st;
        ਜੇ (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                ਵਾਪਸ -EEXIST;

        ਜੇ (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                ਵਾਪਸ 0;

        _cleanup_free_ ਅੱਖਰ *tmp_ਰਸਤਾ = ਨਿਰਾ;
        r = tempfn_random(newpath, /* extra= */ ਨਿਰਾ, &tmp_ਰਸਤਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = link_fd(old_fd, newdirfd, tmp_ਰਸਤਾ);
        ਜੇ (r < 0) {
                ਜੇ (!ERRNO_IS_PRIVILEGE(r))
                        ਵਾਪਸ r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_ਰਸਤਾ, 0));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_ਰਸਤਾ, newdirfd, newpath));
        ਜੇ (r < 0) {
                (ਖਾਲੀ) unlinkat(newdirfd, tmp_ਰਸਤਾ, /* flags= */ 0);
                ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}
