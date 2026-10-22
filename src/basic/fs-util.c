/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/falloc.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/file.h>
#ଅନ୍ତର୍ଭାଵ <sys/mount.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "btrfs-util.h"
#ଅନ୍ତର୍ଭାଵ "chattr-util.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "hostname-util.h"
#ଅନ୍ତର୍ଭାଵ "label-util.h"
#ଅନ୍ତର୍ଭାଵ "lock-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mkdir.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"
#ଅନ୍ତର୍ଭାଵ "ratelimit.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "tmpfile-util.h"
#ଅନ୍ତର୍ଭାଵ "umask-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rmdir_parents(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *stop) {
        ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(stop);

        ଯଦି (!path_is_safe(path))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!path_is_safe(stop))
                ଫେରନ୍ତୁ -EINVAL;

        p = strdupa_safe(path);

        ପାଇଁ (;;) {
                ଅକ୍ଷର *slash = ଖାଲି;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (ଅଚଳ ଅକ୍ଷର **) &slash, ଖାଲି);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (slash == p)
                        ଫେରନ୍ତୁ 0;

                ପ୍ରତିଜ୍ଞା(*slash == '/');
                *slash = '\0';

                ଯଦି (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        ଫେରନ୍ତୁ 0;

                ଯଦି (rmdir(p) < 0 && errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rename_noreplace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା olddirfd, ଅଚଳ ଅକ୍ଷର *oldpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(oldpath);
        ପ୍ରତିଜ୍ଞା(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(newpath);

        /* Try the ideal approach first */
        ଯଦି (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                ଫେରନ୍ତୁ 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        ଯଦି (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                ଫେରନ୍ତୁ -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        ଯଦି (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                ଯଦି (r < 0) {
                        (ଶୂନ୍ୟ) unlinkat(newdirfd, newpath, 0);
                        ଫେରନ୍ତୁ r;
                }

                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                ଫେରନ୍ତୁ -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        ଯଦି (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                ଫେରନ୍ତୁ -EEXIST;
        ଯଦି (errno != ENOENT)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlinkat_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret) {
        size_t l = PATH_MAX;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);

        ଯଦି (fd < 0 && isempty(p))
                ଫେରନ୍ତୁ -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *c = ଖାଲି;
                ssize_t n;

                c = new(ଅକ୍ଷର, l+1);
                ଯଦି (!c)
                        ଫେରନ୍ତୁ -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି ((size_t) n < l) {
                        c[n] = 0;

                        ଯଦି (ret)
                                *ret = TAKE_PTR(c);

                        ଫେରନ୍ତୁ 0;
                }

                ଯଦି (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        ଫେରନ୍ତୁ -EFBIG;

                l *= 2;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlink_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *link = ଖାଲି, *name = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        r = readlink_malloc(p, &link);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = path_extract_filename(link, &name);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == O_DIRECTORY)
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(name);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlink_and_make_absolute(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *target = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(ret);

        r = readlink_malloc(p, &target);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ file_in_same_dir(p, target, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chmod_and_chown_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ଯଦି (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;
                dir_fd = fd;

        } ଅନ୍ୟଥା ଯଦି (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;
                dir_fd = fd;
        }

        ଫେରନ୍ତୁ fchmod_and_chown(dir_fd, mode, uid, gid);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_and_chown_with_fallback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ସଂରଚନା stat st;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        ଯଦି (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        ଅନ୍ୟଥା ଯଦି ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                ଫେରନ୍ତୁ -EINVAL; /* insist on the right file type if it was specified */

        ଯଦି (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                ଯଦି (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        ଯଦି (r < 0) {
                                ଯଦି (!path || r != -ENOSYS)
                                        ଫେରନ୍ତୁ r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                ଯଦି (chmod(path, minimal & 07777) < 0)
                                        ଫେରନ୍ତୁ -errno;
                        }
                }
        }

        ଯଦି (do_chown)
                ଯଦି (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                ଯଦି (r < 0) {
                        ଯଦି (!path || r != -ENOSYS)
                                ଫେରନ୍ତୁ r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        ଯଦି (chmod(path, mode & 07777) < 0)
                                ଫେରନ୍ତୁ -errno;
                }
        }

        ଫେରନ୍ତୁ do_chown || do_chmod;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_umask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        ଫେରନ୍ତୁ RET_NERRNO(fchmod(fd, m & (~u)));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_opath(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, mode_t m) {
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

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                ଫେରନ୍ତୁ 0;
        ଯଦି (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                ଫେରନ୍ତୁ 0;
        ଯଦି (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                ଫେରନ୍ତୁ -errno;

        ଯଦି (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ proc_fd_enoent_errno();
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା futimens_opath(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ସଂରଚନା timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଫେରନ୍ତୁ RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_warn_permissions(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ସଂରଚନା stat *st) {
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        ଯଦି (!S_ISREG(st->st_mode))
                ଫେରନ୍ତୁ 0;

        ଯଦି (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        ଯଦି (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        ଯଦି (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_warn_permissions(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ସଂରଚନା stat st;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ stat_warn_permissions(path, &st);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_nofollow(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode) {
        ଫେରନ୍ତୁ RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode) {
        /* Like access() but operates on an already open fd */

        ଯଦି (fd == AT_FDCWD)
                ଫେରନ୍ତୁ RET_NERRNO(access(".", mode));
        ଯଦି (fd == XAT_FDROOT)
                ଫେରନ୍ତୁ RET_NERRNO(access("/", mode));

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଫେରନ୍ତୁ RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା touch_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, usec_t stamp) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (stamp == USEC_INFINITY)
                ଫେରନ୍ତୁ futimens_opath(fd, /* ts= */ ଖାଲି);

        ସଂରଚନା timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        ଫେରନ୍ତୁ futimens_opath(fd, ts);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା touch_file(ଅଚଳ ଅକ୍ଷର *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

        ପ୍ରତିଜ୍ଞା(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        ଯଦି (parents)
                (ଶୂନ୍ୟ) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ଯଦି (fd < 0) {
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        ଫେରନ୍ତୁ RET_GATHER(ret, touch_fd(fd, stamp));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା touch(ଅଚଳ ଅକ୍ଷର *path) {
        ଫେରନ୍ତୁ touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlinkat_idempotent(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *linkpath, bool make_relative) {
        _cleanup_free_ ଅକ୍ଷର *relpath = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(target);
        ପ୍ରତିଜ୍ଞା(linkpath);

        ଯଦି (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                target = relpath;
        }

        ଯଦି (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                ଯଦି (errno != EEXIST)
                        ଫେରନ୍ତୁ -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                ଯଦି (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        ଫେରନ୍ତୁ -EEXIST;
                ଯଦି (r < 0) /* Any other error? In that case propagate it as is */
                        ଫେରନ୍ତୁ r;

                ଯଦି (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        ଫେରନ୍ତୁ -EEXIST;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlinkat_atomic_full_label(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(target);
        ପ୍ରତିଜ୍ଞା(linkpath);

        _cleanup_free_ ଅକ୍ଷର *relpath = ଖାଲି;
        ଯଦି (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                target = relpath;
        }

        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        r = tempfn_random(linkpath, ଖାଲି, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        bool call_label_ops_post = false;
        ଯଦି (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        ଯଦି (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        ଯଦି (r < 0) {
                (ଶୂନ୍ୟ) unlinkat(atfd, t, 0);
                ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mknodat_atomic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, dev_t dev) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        r = tempfn_random(path, ଖାଲି, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (mknodat(atfd, t, mode, dev) < 0)
                ଫେରନ୍ତୁ -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        ଯଦି (r < 0) {
                (ଶୂନ୍ୟ) unlinkat(atfd, t, 0);
                ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkfifoat_atomic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode) {
        _cleanup_free_ ଅକ୍ଷର *t = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, ଖାଲି, &t);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (mkfifoat(dir_fd, t, mode) < 0)
                ଫେରନ୍ତୁ -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        ଯଦି (r < 0) {
                (ଶୂନ୍ୟ) unlinkat(dir_fd, t, 0);
                ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_files_in_directory(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର ***ret_list) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        _cleanup_closedir_ DIR *d = ଖାଲି;
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        ଯଦି (!d)
                ଫେରନ୍ତୁ -errno;

        FOREACH_DIRENT_ALL(de, d, ଫେରନ୍ତୁ -errno) {
                ଯଦି (!dirent_is_file(de))
                        ଜାରି;

                ଯଦି (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        ଯଦି (!GREEDY_REALLOC(l, n + 2))
                                ଫେରନ୍ତୁ -ENOMEM;

                        l[n] = strdup(de->d_name);
                        ଯଦି (!l[n])
                                ଫେରନ୍ତୁ -ENOMEM;

                        l[++n] = ଖାଲି;
                } ଅନ୍ୟଥା
                        n++;
        }

        ଯଦି (ret_list)
                *ret_list = TAKE_PTR(l);

        ଫେରନ୍ତୁ n;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_tmp_dir(ଅଚଳ ଅକ୍ଷର **ret_path) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;

        ପ୍ରତିଜ୍ଞା(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ଅଚଳ ଅକ୍ଷର *e;

                e = secure_getenv(n);
                ଯଦି (!e)
                        ଜାରି;
                ଯଦି (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        ଯାଅ next;
                }
                ଯଦି (!path_is_normalized(e)) {
                        r = -EPERM;
                        ଯାଅ next;
                }

                r = is_dir(e, true);
                ଯଦି (r < 0)
                        ଯାଅ next;
                ଯଦି (r == 0) {
                        r = -ENOTDIR;
                        ଯାଅ next;
                }

                *ret_path = e;
                ଫେରନ୍ତୁ 1;

        next:
                /* Remember first error, to make this more debuggable */
                ଯଦି (ret >= 0)
                        ret = r;
        }

        ଯଦି (ret < 0)
                ଫେରନ୍ତୁ ret;

        *ret_path = ଖାଲି;
        ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp_dir_internal(ଅଚଳ ଅକ୍ଷର *def, ଅଚଳ ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, k;

        ପ୍ରତିଜ୍ଞା(def);
        ପ୍ରତିଜ୍ଞା(ret);

        r = getenv_tmp_dir(&e);
        ଯଦି (r > 0) {
                *ret = e;
                ଫେରନ୍ତୁ 0;
        }

        k = is_dir(def, /* follow= */ true);
        ଯଦି (k == 0)
                k = -ENOTDIR;
        ଯଦି (k < 0)
                ଫେରନ୍ତୁ RET_GATHER(r, k);

        *ret = def;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_tmp_dir(ଅଚଳ ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        ଫେରନ୍ତୁ tmp_dir_internal("/var/tmp", ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp_dir(ଅଚଳ ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        ଫେରନ୍ତୁ tmp_dir_internal("/tmp", ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlink_or_warn(ଅଚଳ ଅକ୍ଷର *filename) {
        ପ୍ରତିଜ୍ଞା(filename);

        ଯଦି (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                ଯଦି (errno != EROFS || access(filename, F_OK) >= 0)
                        ଫେରନ୍ତୁ log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର *rmdir_and_free(ଅକ୍ଷର *p) {
        PROTECT_ERRNO;

        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        (ଶୂନ୍ୟ) rmdir(p);
        ଫେରନ୍ତୁ mfree(p);
}

ଅକ୍ଷର* unlink_and_free(ଅକ୍ଷର *p) {
        PROTECT_ERRNO;

        ଯଦି (!p)
                ଫେରନ୍ତୁ ଖାଲି;

        (ଶୂନ୍ୟ) unlink(p);
        ଫେରନ୍ତୁ mfree(p);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlinkat_deallocate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା truncate_fd = -EBADF;
        ସଂରଚନା stat st;
        off_t l, bs;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        ଯଦି (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                ଯଦି (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        ଯଦି (IN_SET(errno, ENOENT, EISDIR))
                                ଫେରନ୍ତୁ -errno;

                        ଯଦି (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        ଯଦି (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                ଫେରନ୍ତୁ 0;

        ଯଦି (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (!S_ISREG(st.st_mode))
                ଫେରନ୍ତୁ 0;

        ଯଦି (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t ବାମ = st.st_size;
                ଅକ୍ଷର buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, ଆକାର(buffer));

                ଯେତେବେଳେ (ବାମ > 0) {
                        ssize_t n;

                        n = ଲିଖ(truncate_fd, buffer, MIN(ଆକାର(buffer), ବାମ));
                        ଯଦି (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                ଭାଙ୍ଗ;
                        }

                        ପ୍ରତିଜ୍ଞା(ବାମ >= (size_t) n);
                        ବାମ -= n;
                }

                /* Let's refresh metadata */
                ଯଦି (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        ଫେରନ୍ତୁ 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        ଯଦି (st.st_blocks == 0 || st.st_nlink > 0)
                ଫେରନ୍ତୁ 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        ଯଦି (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                ଫେରନ୍ତୁ 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        ଯଦି (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode) {
        _cleanup_free_ ଅକ୍ଷର *parent = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(path);

        r = path_extract_directory(path, &parent);
        ଯଦି (r == -EDESTADDRREQ) {
                parent = strdup(".");
                ଯଦି (!parent)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା ଯଦି (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                ଯଦି (!parent)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        ଯଦି (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        ଫେରନ୍ତୁ RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conservative_renameat(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା olddirfd, ଅଚଳ ଅକ୍ଷର *oldpath,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_fd = -EBADF, new_fd = -EBADF;
        ସଂରଚନା stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ଯଦି (old_fd < 0)
                ଯାଅ do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ଯଦି (new_fd < 0)
                ଯାଅ do_rename;

        ଯଦି (fstat(old_fd, &old_stat) < 0)
                ଯାଅ do_rename;

        ଯଦି (!S_ISREG(old_stat.st_mode))
                ଯାଅ do_rename;

        ଯଦି (fstat(new_fd, &new_stat) < 0)
                ଯାଅ do_rename;

        ଯଦି (stat_inode_same(&new_stat, &old_stat))
                ଯାଅ is_same;

        ଯଦି (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                ଯାଅ do_rename;

        ପାଇଁ (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[ଆକାର(buf1)];
                ssize_t l1, l2;

                l1 = ପଠ(old_fd, buf1, ଆକାର(buf1));
                ଯଦି (l1 < 0)
                        ଯାଅ do_rename;

                ଯଦି (l1 == ଆକାର(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = ପଠ(new_fd, buf2, l1);
                ଅନ୍ୟଥା {
                        ପ୍ରତିଜ୍ଞା((size_t) l1 < ଆକାର(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        ପ୍ରତିଜ୍ଞା((size_t) (l1 + 1) <= ଆକାର(buf2));
                        l2 = ପଠ(new_fd, buf2, l1 + 1);
                }
                ଯଦି (l2 != l1)
                        ଯାଅ do_rename;

                ଯଦି (memcmp(buf1, buf2, l1) != 0)
                        ଯାଅ do_rename;

                ଯଦି ((size_t) l1 < ଆକାର(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        ଭାଙ୍ଗ;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        ଯଦି (unlinkat(olddirfd, oldpath, 0) < 0)
                ଯାଅ do_rename;

        ଫେରନ୍ତୁ 0;

do_rename:
        ଯଦି (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_fallocate_loop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        ଯଦି (r != EINTR)
                ଫେରନ୍ତୁ -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ଅଚଳ RateLimit) { 10 * USEC_PER_SEC, 16 };
        ଯେତେବେଳେ (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                ଯଦି (r != EINTR)
                        ଫେରନ୍ତୁ -r;
        }

        ଫେରନ୍ତୁ -EINTR;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_cifs_service(
                ଅଚଳ ଅକ୍ଷର *s,
                ଅକ୍ଷର **ret_host,
                ଅକ୍ଷର **ret_service,
                ଅକ୍ଷର **ret_path) {

        _cleanup_free_ ଅକ୍ଷର *h = ଖାଲି, *ss = ଖାଲି, *x = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *p, *e, *d;
        ଅକ୍ଷର delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        ଯଦି (!s)
                ଫେରନ୍ତୁ -EINVAL;

        p = startswith(s, "//");
        ଯଦି (!p) {
                p = startswith(s, "\\\\");
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        ଯଦି (!e)
                ଫେରନ୍ତୁ -EINVAL;

        h = strndup(p, e - p);
        ଯଦି (!h)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (!hostname_is_valid(h, 0))
                ଫେରନ୍ତୁ -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        ଯଦି (!ss)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (!filename_is_valid(ss))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                ଯଦି (!x)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                ପାଇଁ (ଅକ୍ଷର *i = x; *i; i++)
                        ଯଦି (*i == delimiter)
                                *i = '/';

                ଯଦି (!path_is_valid(x))
                        ଫେରନ୍ତୁ -EINVAL;

                path_simplify(x);
                ଯଦି (!path_is_normalized(x))
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (ret_host)
                *ret_host = TAKE_PTR(h);
        ଯଦି (ret_service)
                *ret_service = TAKE_PTR(ss);
        ଯଦି (ret_path)
                *ret_path = TAKE_PTR(x);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_mkdir_at_full_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ ଅକ୍ଷର *fname = ଖାଲି, *parent = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        ଯଦି (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                ଫେରନ୍ତୁ -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        ଯଦି (r < 0) {
                ଯଦି (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        ଫେରନ୍ତୁ r;
        } ଅନ୍ୟଥା {
                r = path_extract_filename(path, &fname);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ଯଦି (parent_fd < 0)
                        ଫେରନ୍ତୁ -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        ଯଦି (IN_SET(fd, -ELOOP, -ENOTDIR))
                ଫେରନ୍ତୁ -EEXIST;
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା openat_report_new(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *pathname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode, bool *ret_newly_created) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        ଯଦି (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                ଫେରନ୍ତୁ fd;
        }

        ପାଇଁ (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                ଯଦି (fd >= 0) {
                        ଯଦି (ret_newly_created)
                                *ret_newly_created = false;
                        ଫେରନ୍ତୁ fd;
                }
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                ଯଦି (fd >= 0) {
                        ଯଦି (ret_newly_created)
                                *ret_newly_created = true;
                        ଫେରନ୍ତୁ fd;
                }
                ଯଦି (errno != EEXIST)
                        ଫେରନ୍ତୁ -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                ଯଦି (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        ଫେରନ୍ତୁ -EEXIST;
        }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା openat_with_automount(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ସ୍ଥିର bool can_open_ବୃକ୍ଷ = true;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(FLAGS_SET(open_flags, O_PATH));

        ଯଦି (can_open_ବୃକ୍ଷ) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                ଯଦି (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        ଯଦି (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା q = fd_verify_directory(fd);
                                ଯଦି (q < 0) {
                                        safe_close(fd);
                                        ଫେରନ୍ତୁ q;
                                }
                        }

                        ଫେରନ୍ତୁ fd;
                }
                ଯଦି (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        ଫେରନ୍ତୁ fd;

                can_open_ବୃକ୍ଷ = false;
        }

        ଫେରନ୍ତୁ RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_full_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        bool made_dir = false, made_file = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        ପ୍ରତିଜ୍ଞା(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        ପ୍ରତିଜ୍ଞା(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        ପ୍ରତିଜ୍ଞା(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        ପ୍ରତିଜ୍ଞା((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        ଯଦି (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        ଯଦି (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                ଯଦି (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } ଅନ୍ୟଥା ଯଦି (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (isempty(path)) {
                ପ୍ରତିଜ୍ଞା(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                ଯଦି (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        ଯଦି (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                ଫେରନ୍ତୁ TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                ଫେରନ୍ତୁ fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା _dir_fd = -EBADF;
        ଯଦି (dir_fd == XAT_FDROOT) {
                ଯଦି (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                ଅନ୍ୟଥା {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        ଯଦି (_dir_fd < 0)
                                ଫେରନ୍ତୁ -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        ଯଦି (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                call_label_ops_post = true;
        }

        ଯଦି (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                ଯଦି (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                ଅନ୍ୟଥା
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                ଯଦି (r == -EEXIST) {
                        ଯଦି (FLAGS_SET(open_flags, O_EXCL))
                                ଫେରନ୍ତୁ -EEXIST;
                } ଅନ୍ୟଥା ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଅନ୍ୟଥା
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        ଯଦି (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                ଯଦି (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        ଯଦି (fd < 0) {
                                r = fd;
                                ଯାଅ error;
                        }

                        r = fd_verify_regular(fd);
                        ଯଦି (r < 0)
                                ଯାଅ error;

                } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        ଯଦି (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                ଯଦି (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                ଅନ୍ୟଥା ଯଦି (fd < 0) {
                                        r = fd;
                                        ଯାଅ error;
                                }
                        }

                        ଯଦି (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                ଯଦି (fd < 0) {
                                        r = -errno;
                                        ଯାଅ error;
                                }
                        }

                        made_file = true;
                } ଅନ୍ୟଥା {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        ଯଦି (inode_fd < 0) {
                                ଯଦି (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        ଯାଅ error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                ଯଦି (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        ଯଦି (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ଅନ୍ୟଥା ଯଦି (fd < 0) {
                                                r = fd;
                                                ଯାଅ error;
                                        }
                                }

                                ଯଦି (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        ଯଦି (fd < 0) {
                                                r = -errno;
                                                ଯାଅ error;
                                        }
                                }

                                made_file = true;
                        } ଅନ୍ୟଥା {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                ଯଦି (r < 0)
                                        ଯାଅ error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                ଯଦି (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        ଯଦି (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ଅନ୍ୟଥା ଯଦି (fd < 0) {
                                                r = fd;
                                                ଯାଅ error;
                                        }
                                }

                                ଯଦି (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        ଯଦି (fd < 0) {
                                                r = fd;
                                                ଯାଅ error;
                                        }
                                }
                        }
                }
        } ଅନ୍ୟଥା ଯଦି (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                ଯଦି (fd < 0) {
                        r = fd;
                        ଯାଅ error;
                }
        } ଅନ୍ୟଥା {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                ଯଦି (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        ଯଦି (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        ଅନ୍ୟଥା ଯଦି (fd < 0) {
                                r = fd;
                                ଯାଅ error;
                        }
                }

                ଯଦି (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        ଯଦି (fd < 0) {
                                r = fd;
                                ଯାଅ error;
                        }
                }
        }

        ଯଦି (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                ଯଦି (r < 0)
                        ଯାଅ error;
        }

        ଯଦି (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ ଖାଲି, made_file || made_dir, label_context);
                ଯଦି (r < 0)
                        ଯାଅ error;
        }

        ଯଦି (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                ଯଦି (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        ଯାଅ error;
        }

        ଫେରନ୍ତୁ TAKE_FD(fd);

error:
        ଯଦି (call_label_ops_post)
                (ଶୂନ୍ୟ) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ଖାଲି : path, made_dir || made_file, label_context);

        ଯଦି (made_dir || made_file)
                (ଶୂନ୍ୟ) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_lock_full_label(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation,
                LabelContext *label_context) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        ଯଦି (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                ଫେରନ୍ତୁ -EBADF;

        ପାଇଁ (;;) {
                ସଂରଚନା stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ fd;

                r = lock_generic(fd, locktype, operation);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                ଯଦି (fstat(fd, &st) < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (st.st_nlink > 0)
                        ଭାଙ୍ଗ;

                fd = safe_close(fd);
        }

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା link_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        ଯଦି (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                ଯଦି (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        ଫେରନ୍ତୁ -ENOSYS;
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା linkat_replace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା olddirfd, ଅଚଳ ଅକ୍ଷର *oldpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        ଯଦି (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                ଫେରନ୍ତୁ -EISDIR;

        ଯଦି (path_implies_directory(newpath))
                ଫେରନ୍ତୁ -EISDIR;

        /* First, try to link this directly */
        ଯଦି (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        ଅନ୍ୟଥା
                r = link_fd(olddirfd, newdirfd, newpath);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ 0;
        ଯଦି (r != -EEXIST)
                ଫେରନ୍ତୁ r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        ଯଦି (old_fd < 0)
                ଫେରନ୍ତୁ old_fd;

        ସଂରଚନା stat old_st;
        ଯଦି (fstat(old_fd, &old_st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                ଫେରନ୍ତୁ -EISDIR;

        ସଂରଚନା stat new_st;
        ଯଦି (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                ଫେରନ୍ତୁ -EEXIST;

        ଯଦି (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                ଫେରନ୍ତୁ 0;

        _cleanup_free_ ଅକ୍ଷର *tmp_path = ଖାଲି;
        r = tempfn_random(newpath, /* extra= */ ଖାଲି, &tmp_path);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        ଯଦି (r < 0) {
                ଯଦି (!ERRNO_IS_PRIVILEGE(r))
                        ଫେରନ୍ତୁ r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        ଯଦି (r < 0) {
                (ଶୂନ୍ୟ) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}
