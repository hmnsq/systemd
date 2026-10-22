/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/falloc.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/file.h>
#అన్తర్భావ <sys/mount.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "btrfs-util.h"
#అన్తర్భావ "chattr-util.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "hostname-util.h"
#అన్తర్భావ "label-util.h"
#అన్తర్భావ "lock-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mkdir.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "random-util.h"
#అన్తర్భావ "ratelimit.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "tmpfile-util.h"
#అన్తర్భావ "umask-util.h"

పూర్ణాంకం rmdir_parents(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *stop) {
        అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(stop);

        ఒకవేళ (!మార్గం_is_సురక్షిత(మార్గం))
                తిరిగి -EINVAL;

        ఒకవేళ (!మార్గం_is_సురక్షిత(stop))
                తిరిగి -EINVAL;

        p = strdupa_సురక్షిత(మార్గం);

        కొరకు (;;) {
                అక్షరం *slash = ఖాళీ;

                /* skip the last component. */
                r = మార్గం_కనుగొను_చివరి_component(p, /* accept_dot_dot= */ false, (స్థిరరాశి అక్షరం **) &slash, ఖాళీ);
                ఒకవేళ (r <= 0)
                        తిరిగి r;
                ఒకవేళ (slash == p)
                        తిరిగి 0;

                ప్రతిజ్ఞా(*slash == '/');
                *slash = '\0';

                ఒకవేళ (మార్గం_startswith_నిండిన(stop, p, మార్గం_STARTSWITH_REFUSE_DOT_DOT))
                        తిరిగి 0;

                ఒకవేళ (rmdir(p) < 0 && errno != ENOENT)
                        తిరిగి -errno;
        }
}

పూర్ణాంకం rename_noreplace(పూర్ణాంకం olddirfd, స్థిరరాశి అక్షరం *oldpath, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ప్రతిజ్ఞా(oldpath);
        ప్రతిజ్ఞా(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ప్రతిజ్ఞా(newpath);

        /* Try the ideal approach first */
        ఒకవేళ (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                తిరిగి 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                తిరిగి -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        ఒకవేళ (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                ఒకవేళ (r < 0) {
                        (శూన్య) unlinkat(newdirfd, newpath, 0);
                        తిరిగి r;
                }

                తిరిగి 0;
        }

        ఒకవేళ (!ERRNO_IS_NOT_SUPPORTED(errno) && !లోపల_సమితి(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                తిరిగి -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        ఒకవేళ (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                తిరిగి -EEXIST;
        ఒకవేళ (errno != ENOENT)
                తిరిగి -errno;

        తిరిగి RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

పూర్ణాంకం readlinkat_malloc(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *p, అక్షరం **ret) {
        size_t l = PATH_MAX;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);

        ఒకవేళ (fd < 0 && isempty(p))
                తిరిగి -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *c = ఖాళీ;
                ssize_t n;

                c = new(అక్షరం, l+1);
                ఒకవేళ (!c)
                        తిరిగి -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;

                ఒకవేళ ((size_t) n < l) {
                        c[n] = 0;

                        ఒకవేళ (ret)
                                *ret = TAKE_PTR(c);

                        తిరిగి 0;
                }

                ఒకవేళ (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        తిరిగి -EFBIG;

                l *= 2;
        }
}

పూర్ణాంకం readlink_విలువ(స్థిరరాశి అక్షరం *p, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *link = ఖాళీ, *name = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        r = readlink_malloc(p, &link);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = మార్గం_వెలికితీయి_filename(link, &name);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == O_DIRECTORY)
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(name);
        తిరిగి 0;
}

పూర్ణాంకం readlink_and_make_absolute(స్థిరరాశి అక్షరం *p, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *లక్ష్యం = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        r = readlink_malloc(p, &లక్ష్యం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి file_లోపల_same_dir(p, లక్ష్యం, ret);
}

పూర్ణాంకం chmod_and_chown_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ఒకవేళ (మార్గం) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, మార్గం, O_మార్గం|O_CLOEXEC|O_NOFOLLOW);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;
                dir_fd = fd;

        } లేకపోతే ఒకవేళ (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_మార్గం|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;
                dir_fd = fd;
        }

        తిరిగి fchmod_and_chown(dir_fd, బహుళకం, uid, gid);
}

పూర్ణాంకం fchmod_and_chown_with_fallback(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        నిర్మాణం stat st;
        పూర్ణాంకం r;

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

        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((బహుళకం != బహుళకం_INVALID && ((st.st_mode ^ బహుళకం) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        ఒకవేళ (బహుళకం == బహుళకం_INVALID)
                బహుళకం = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        లేకపోతే ఒకవేళ ((బహుళకం & S_IFMT) != 0 && ((బహుళకం ^ st.st_mode) & S_IFMT) != 0)
                తిరిగి -EINVAL; /* insist on the right file type if it was specified */

        ఒకవేళ (do_chown && do_chmod) {
                mode_t కనీస = st.st_mode & బహుళకం; /* the subset of the old and the new mask */

                ఒకవేళ (((కనీస ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, కనీస & 07777);
                        ఒకవేళ (r < 0) {
                                ఒకవేళ (!మార్గం || r != -ENOSYS)
                                        తిరిగి r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                ఒకవేళ (chmod(మార్గం, కనీస & 07777) < 0)
                                        తిరిగి -errno;
                        }
                }
        }

        ఒకవేళ (do_chown)
                ఒకవేళ (fchownat(fd, "", uid, gid, AT_ఖాళీలు_మార్గం) < 0)
                        తిరిగి -errno;

        ఒకవేళ (do_chmod) {
                r = fchmod_opath(fd, బహుళకం & 07777);
                ఒకవేళ (r < 0) {
                        ఒకవేళ (!మార్గం || r != -ENOSYS)
                                తిరిగి r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        ఒకవేళ (chmod(మార్గం, బహుళకం & 07777) < 0)
                                తిరిగి -errno;
                }
        }

        తిరిగి do_chown || do_chmod;
}

పూర్ణాంకం fchmod_umask(పూర్ణాంకం fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        తిరిగి RET_NERRNO(fchmod(fd, m & (~u)));
}

పూర్ణాంకం fchmod_opath(పూర్ణాంకం fd, mode_t m) {
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

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (fchmodat(fd, "", m, AT_ఖాళీలు_మార్గం) >= 0)
                తిరిగి 0;
        ఒకవేళ (errno == EINVAL && fchmodat2(fd, "", m, AT_ఖాళీలు_మార్గం) >= 0) /* glibc too old? */
                తిరిగి 0;
        ఒకవేళ (!లోపల_సమితి(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                తిరిగి -errno;

        ఒకవేళ (chmod(FORMAT_PROC_FD_మార్గం(fd), m) < 0) {
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                తిరిగి proc_fd_enoent_errno();
        }

        తిరిగి 0;
}

పూర్ణాంకం futimens_opath(పూర్ణాంకం fd, స్థిరరాశి నిర్మాణం timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        ప్రతిజ్ఞా(fd >= 0);

        తిరిగి RET_NERRNO(utimensat(fd, "", ts, AT_ఖాళీలు_మార్గం));
}

పూర్ణాంకం stat_warn_permissions(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి నిర్మాణం stat *st) {
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        ఒకవేళ (!S_ISREG(st->st_mode))
                తిరిగి 0;

        ఒకవేళ (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", మార్గం);

        ఒకవేళ (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", మార్గం);

        ఒకవేళ (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", మార్గం);

        తిరిగి 0;
}

పూర్ణాంకం fd_warn_permissions(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం fd) {
        నిర్మాణం stat st;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        తిరిగి stat_warn_permissions(మార్గం, &st);
}

పూర్ణాంకం access_nofollow(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం బహుళకం) {
        తిరిగి RET_NERRNO(faccessat(AT_FDCWD, మార్గం, బహుళకం, AT_SYMLINK_NOFOLLOW));
}

పూర్ణాంకం access_fd(పూర్ణాంకం fd, పూర్ణాంకం బహుళకం) {
        /* Like access() but operates on an already open fd */

        ఒకవేళ (fd == AT_FDCWD)
                తిరిగి RET_NERRNO(access(".", బహుళకం));
        ఒకవేళ (fd == XAT_FDROOT)
                తిరిగి RET_NERRNO(access("/", బహుళకం));

        ప్రతిజ్ఞా(fd >= 0);

        తిరిగి RET_NERRNO(faccessat(fd, "", బహుళకం, AT_ఖాళీలు_మార్గం));
}

పూర్ణాంకం touch_fd(పూర్ణాంకం fd, usec_t stamp) {
        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (stamp == USEC_INFINITY)
                తిరిగి futimens_opath(fd, /* ts= */ ఖాళీ);

        నిర్మాణం timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        తిరిగి futimens_opath(fd, ts);
}

పూర్ణాంకం touch_file(స్థిరరాశి అక్షరం *మార్గం, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t బహుళకం) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం ret;

        ప్రతిజ్ఞా(మార్గం);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        ఒకవేళ (parents)
                (శూన్య) mkdir_parents(మార్గం, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(మార్గం, O_మార్గం|O_CLOEXEC|O_NOFOLLOW);
        ఒకవేళ (fd < 0) {
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(మార్గం, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, లోపల_సమితి(బహుళకం, 0, బహుళకం_INVALID) ? 0644 : బహుళకం);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, బహుళకం, uid, gid);

        తిరిగి RET_GATHER(ret, touch_fd(fd, stamp));
}

పూర్ణాంకం touch(స్థిరరాశి అక్షరం *మార్గం) {
        తిరిగి touch_file(మార్గం, false, USEC_INFINITY, UID_INVALID, GID_INVALID, బహుళకం_INVALID);
}

పూర్ణాంకం symlinkat_idempotent(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *linkpath, bool make_relative) {
        _cleanup_free_ అక్షరం *relpath = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(లక్ష్యం);
        ప్రతిజ్ఞా(linkpath);

        ఒకవేళ (make_relative) {
                r = మార్గం_make_relative_మాతృ(linkpath, లక్ష్యం, &relpath);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                లక్ష్యం = relpath;
        }

        ఒకవేళ (symlinkat(లక్ష్యం, atfd, linkpath) < 0) {
                _cleanup_free_ అక్షరం *p = ఖాళీ;

                ఒకవేళ (errno != EEXIST)
                        తిరిగి -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                ఒకవేళ (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        తిరిగి -EEXIST;
                ఒకవేళ (r < 0) /* Any other error? In that case propagate it as is */
                        తిరిగి r;

                ఒకవేళ (!streq(p, లక్ష్యం)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        తిరిగి -EEXIST;
        }

        తిరిగి 0;
}

పూర్ణాంకం symlinkat_atomic_నిండిన_label(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(లక్ష్యం);
        ప్రతిజ్ఞా(linkpath);

        _cleanup_free_ అక్షరం *relpath = ఖాళీ;
        ఒకవేళ (FLAGS_సమితి(flags, SYMLINK_MAKE_RELATIVE)) {
                r = మార్గం_make_relative_మాతృ(linkpath, లక్ష్యం, &relpath);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                లక్ష్యం = relpath;
        }

        _cleanup_free_ అక్షరం *t = ఖాళీ;
        r = tempfn_random(linkpath, ఖాళీ, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        bool call_label_ops_post = false;
        ఒకవేళ (FLAGS_సమితి(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(లక్ష్యం, atfd, t));
        ఒకవేళ (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        ఒకవేళ (r < 0) {
                (శూన్య) unlinkat(atfd, t, 0);
                తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం mknodat_atomic(పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, dev_t dev) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        r = tempfn_random(మార్గం, ఖాళీ, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (mknodat(atfd, t, బహుళకం, dev) < 0)
                తిరిగి -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, మార్గం));
        ఒకవేళ (r < 0) {
                (శూన్య) unlinkat(atfd, t, 0);
                తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం mkfifoat_atomic(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం) {
        _cleanup_free_ అక్షరం *t = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(మార్గం, ఖాళీ, &t);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (mkfifoat(dir_fd, t, బహుళకం) < 0)
                తిరిగి -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, మార్గం));
        ఒకవేళ (r < 0) {
                (శూన్య) unlinkat(dir_fd, t, 0);
                తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం get_files_లోపల_directory(స్థిరరాశి అక్షరం *మార్గం, అక్షరం ***ret_list) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        _cleanup_closedir_ DIR *d = ఖాళీ;
        size_t n = 0;

        ప్రతిజ్ఞా(మార్గం);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(మార్గం);
        ఒకవేళ (!d)
                తిరిగి -errno;

        FOREACH_DIRENT_ALL(de, d, తిరిగి -errno) {
                ఒకవేళ (!dirent_is_file(de))
                        కొనసాగు;

                ఒకవేళ (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        ఒకవేళ (!GREEDY_REALLOC(l, n + 2))
                                తిరిగి -ENOMEM;

                        l[n] = strdup(de->d_name);
                        ఒకవేళ (!l[n])
                                తిరిగి -ENOMEM;

                        l[++n] = ఖాళీ;
                } లేకపోతే
                        n++;
        }

        ఒకవేళ (ret_list)
                *ret_list = TAKE_PTR(l);

        తిరిగి n;
}

స్థిర పూర్ణాంకం getenv_tmp_dir(స్థిరరాశి అక్షరం **ret_మార్గం) {
        పూర్ణాంకం r, ret = 0;

        ప్రతిజ్ఞా(ret_మార్గం);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                స్థిరరాశి అక్షరం *e;

                e = secure_getenv(n);
                ఒకవేళ (!e)
                        కొనసాగు;
                ఒకవేళ (!మార్గం_is_absolute(e)) {
                        r = -ENOTDIR;
                        వెళ్ళు తదుపరి_2;
                }
                ఒకవేళ (!మార్గం_is_normalized(e)) {
                        r = -EPERM;
                        వెళ్ళు తదుపరి_2;
                }

                r = is_dir(e, true);
                ఒకవేళ (r < 0)
                        వెళ్ళు తదుపరి_2;
                ఒకవేళ (r == 0) {
                        r = -ENOTDIR;
                        వెళ్ళు తదుపరి_2;
                }

                *ret_మార్గం = e;
                తిరిగి 1;

        తదుపరి_2:
                /* Remember first error, to make this more debuggable */
                ఒకవేళ (ret >= 0)
                        ret = r;
        }

        ఒకవేళ (ret < 0)
                తిరిగి ret;

        *ret_మార్గం = ఖాళీ;
        తిరిగి ret;
}

స్థిర పూర్ణాంకం tmp_dir_అంతర్గత(స్థిరరాశి అక్షరం *def, స్థిరరాశి అక్షరం **ret) {
        స్థిరరాశి అక్షరం *e;
        పూర్ణాంకం r, k;

        ప్రతిజ్ఞా(def);
        ప్రతిజ్ఞా(ret);

        r = getenv_tmp_dir(&e);
        ఒకవేళ (r > 0) {
                *ret = e;
                తిరిగి 0;
        }

        k = is_dir(def, /* follow= */ true);
        ఒకవేళ (k == 0)
                k = -ENOTDIR;
        ఒకవేళ (k < 0)
                తిరిగి RET_GATHER(r, k);

        *ret = def;
        తిరిగి 0;
}

పూర్ణాంకం var_tmp_dir(స్థిరరాశి అక్షరం **ret) {
        ప్రతిజ్ఞా(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        తిరిగి tmp_dir_అంతర్గత("/var/tmp", ret);
}

పూర్ణాంకం tmp_dir(స్థిరరాశి అక్షరం **ret) {
        ప్రతిజ్ఞా(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        తిరిగి tmp_dir_అంతర్గత("/tmp", ret);
}

పూర్ణాంకం unlink_or_warn(స్థిరరాశి అక్షరం *filename) {
        ప్రతిజ్ఞా(filename);

        ఒకవేళ (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                ఒకవేళ (errno != EROFS || access(filename, F_OK) >= 0)
                        తిరిగి log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        తిరిగి 0;
}

అక్షరం *rmdir_and_free(అక్షరం *p) {
        PROTECT_ERRNO;

        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        (శూన్య) rmdir(p);
        తిరిగి mfree(p);
}

అక్షరం* unlink_and_free(అక్షరం *p) {
        PROTECT_ERRNO;

        ఒకవేళ (!p)
                తిరిగి ఖాళీ;

        (శూన్య) unlink(p);
        తిరిగి mfree(p);
}

పూర్ణాంకం unlinkat_deallocate(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ పూర్ణాంకం truncate_fd = -EBADF;
        నిర్మాణం stat st;
        off_t l, bs;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        ఒకవేళ (!FLAGS_సమితి(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                ఒకవేళ (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        ఒకవేళ (లోపల_సమితి(errno, ENOENT, EISDIR))
                                తిరిగి -errno;

                        ఒకవేళ (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        ఒకవేళ (unlinkat(fd, name, FLAGS_సమితి(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                తిరిగి -errno;

        ఒకవేళ (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                తిరిగి 0;

        ఒకవేళ (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                తిరిగి 0;
        }

        ఒకవేళ (!S_ISREG(st.st_mode))
                తిరిగి 0;

        ఒకవేళ (FLAGS_సమితి(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t ఎడమ = st.st_size;
                అక్షరం బఫరు[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_బైట్లు(బఫరు, పరిమాణం(బఫరు));

                అయితే (ఎడమ > 0) {
                        ssize_t n;

                        n = లిఖ(truncate_fd, బఫరు, MIN(పరిమాణం(బఫరు), ఎడమ));
                        ఒకవేళ (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                ఆపు;
                        }

                        ప్రతిజ్ఞా(ఎడమ >= (size_t) n);
                        ఎడమ -= n;
                }

                /* Let's refresh metadata */
                ఒకవేళ (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        తిరిగి 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        ఒకవేళ (st.st_blocks == 0 || st.st_nlink > 0)
                తిరిగి 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = విడత_పైకి(st.st_size, bs); /* Round up to next block size */

        ఒకవేళ (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                తిరిగి 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        ఒకవేళ (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                తిరిగి 0;
        }

        తిరిగి 0;
}

పూర్ణాంకం open_మాతృ_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, mode_t బహుళకం) {
        _cleanup_free_ అక్షరం *మాతృ = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(మార్గం);

        r = మార్గం_వెలికితీయి_directory(మార్గం, &మాతృ);
        ఒకవేళ (r == -EDESTADDRREQ) {
                మాతృ = strdup(".");
                ఒకవేళ (!మాతృ)
                        తిరిగి -ENOMEM;
        } లేకపోతే ఒకవేళ (r == -EADDRNOTAVAIL) {
                మాతృ = strdup(మార్గం);
                ఒకవేళ (!మాతృ)
                        తిరిగి -ENOMEM;
        } లేకపోతే ఒకవేళ (r < 0)
                తిరిగి r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        ఒకవేళ (!FLAGS_సమితి(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        తిరిగి RET_NERRNO(openat(dir_fd, మాతృ, flags, బహుళకం));
}

పూర్ణాంకం conservative_renameat(
                పూర్ణాంకం olddirfd, స్థిరరాశి అక్షరం *oldpath,
                పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath) {

        _cleanup_close_ పూర్ణాంకం old_fd = -EBADF, new_fd = -EBADF;
        నిర్మాణం stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ఒకవేళ (old_fd < 0)
                వెళ్ళు do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ఒకవేళ (new_fd < 0)
                వెళ్ళు do_rename;

        ఒకవేళ (fstat(old_fd, &old_stat) < 0)
                వెళ్ళు do_rename;

        ఒకవేళ (!S_ISREG(old_stat.st_mode))
                వెళ్ళు do_rename;

        ఒకవేళ (fstat(new_fd, &new_stat) < 0)
                వెళ్ళు do_rename;

        ఒకవేళ (stat_inode_same(&new_stat, &old_stat))
                వెళ్ళు is_same;

        ఒకవేళ (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                వెళ్ళు do_rename;

        కొరకు (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[పరిమాణం(buf1)];
                ssize_t l1, l2;

                l1 = పఠ(old_fd, buf1, పరిమాణం(buf1));
                ఒకవేళ (l1 < 0)
                        వెళ్ళు do_rename;

                ఒకవేళ (l1 == పరిమాణం(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = పఠ(new_fd, buf2, l1);
                లేకపోతే {
                        ప్రతిజ్ఞా((size_t) l1 < పరిమాణం(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        ప్రతిజ్ఞా((size_t) (l1 + 1) <= పరిమాణం(buf2));
                        l2 = పఠ(new_fd, buf2, l1 + 1);
                }
                ఒకవేళ (l2 != l1)
                        వెళ్ళు do_rename;

                ఒకవేళ (memcmp(buf1, buf2, l1) != 0)
                        వెళ్ళు do_rename;

                ఒకవేళ ((size_t) l1 < పరిమాణం(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        ఆపు;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        ఒకవేళ (unlinkat(olddirfd, oldpath, 0) < 0)
                వెళ్ళు do_rename;

        తిరిగి 0;

do_rename:
        ఒకవేళ (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                తిరిగి -errno;

        తిరిగి 1;
}

పూర్ణాంకం posix_fallocate_loop(పూర్ణాంకం fd, uint64_t స్థానభ్రంశం, uint64_t కొలత) {
        రేటు_హద్దు rl;
        పూర్ణాంకం r;

        r = posix_fallocate(fd, స్థానభ్రంశం, కొలత); /* returns positive errnos on error */
        ఒకవేళ (r != EINTR)
                తిరిగి -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (స్థిరరాశి రేటు_హద్దు) { 10 * USEC_చొప్పున_SEC, 16 };
        అయితే (ratelimit_క్రింద(&rl)) {
                r = posix_fallocate(fd, స్థానభ్రంశం, కొలత);
                ఒకవేళ (r != EINTR)
                        తిరిగి -r;
        }

        తిరిగి -EINTR;
}

పూర్ణాంకం parse_cifs_service(
                స్థిరరాశి అక్షరం *s,
                అక్షరం **ret_host,
                అక్షరం **ret_service,
                అక్షరం **ret_మార్గం) {

        _cleanup_free_ అక్షరం *h = ఖాళీ, *ss = ఖాళీ, *x = ఖాళీ;
        స్థిరరాశి అక్షరం *p, *e, *d;
        అక్షరం delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        ఒకవేళ (!s)
                తిరిగి -EINVAL;

        p = startswith(s, "//");
        ఒకవేళ (!p) {
                p = startswith(s, "\\\\");
                ఒకవేళ (!p)
                        తిరిగి -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        ఒకవేళ (!e)
                తిరిగి -EINVAL;

        h = strndup(p, e - p);
        ఒకవేళ (!h)
                తిరిగి -ENOMEM;

        ఒకవేళ (!hostname_is_valid(h, 0))
                తిరిగి -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        ఒకవేళ (!ss)
                తిరిగి -ENOMEM;

        ఒకవేళ (!filename_is_valid(ss))
                తిరిగి -EINVAL;

        ఒకవేళ (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_గమ్యం_STR(delimiter)));
                ఒకవేళ (!x)
                        తిరిగి -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                కొరకు (అక్షరం *i = x; *i; i++)
                        ఒకవేళ (*i == delimiter)
                                *i = '/';

                ఒకవేళ (!మార్గం_is_valid(x))
                        తిరిగి -EINVAL;

                మార్గం_simplify(x);
                ఒకవేళ (!మార్గం_is_normalized(x))
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (ret_host)
                *ret_host = TAKE_PTR(h);
        ఒకవేళ (ret_service)
                *ret_service = TAKE_PTR(ss);
        ఒకవేళ (ret_మార్గం)
                *ret_మార్గం = TAKE_PTR(x);

        తిరిగి 0;
}

పూర్ణాంకం open_mkdir_at_నిండిన_label(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, XOpenFlags xopen_flags, mode_t బహుళకం, LabelContext *label_context) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF, మాతృ_fd = -EBADF;
        _cleanup_free_ అక్షరం *fname = ఖాళీ, *మాతృ = ఖాళీ;
        పూర్ణాంకం r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        ఒకవేళ (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_మార్గం))
                తిరిగి -EINVAL;
        ఒకవేళ ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                తిరిగి -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = మార్గం_వెలికితీయి_directory(మార్గం, &మాతృ);
        ఒకవేళ (r < 0) {
                ఒకవేళ (!లోపల_సమితి(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        తిరిగి r;
        } లేకపోతే {
                r = మార్గం_వెలికితీయి_filename(మార్గం, &fname);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                మాతృ_fd = openat(dirfd, మాతృ, O_మార్గం|O_DIRECTORY|O_CLOEXEC);
                ఒకవేళ (మాతృ_fd < 0)
                        తిరిగి -errno;

                dirfd = మాతృ_fd;
                మార్గం = fname;
        }

        fd = xopenat_నిండిన_label(dirfd, మార్గం, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, బహుళకం, label_context);
        ఒకవేళ (లోపల_సమితి(fd, -ELOOP, -ENOTDIR))
                తిరిగి -EEXIST;
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        తిరిగి TAKE_FD(fd);
}

పూర్ణాంకం openat_report_new(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *pathname, పూర్ణాంకం flags, mode_t బహుళకం, bool *ret_newly_created) {
        పూర్ణాంకం fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        ఒకవేళ (!FLAGS_సమితి(flags, O_CREAT) || FLAGS_సమితి(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, బహుళకం);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;

                ఒకవేళ (ret_newly_created)
                        *ret_newly_created = FLAGS_సమితి(flags, O_CREAT);
                తిరిగి fd;
        }

        కొరకు (గుర్తులేని attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), బహుళకం);
                ఒకవేళ (fd >= 0) {
                        ఒకవేళ (ret_newly_created)
                                *ret_newly_created = false;
                        తిరిగి fd;
                }
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, బహుళకం);
                ఒకవేళ (fd >= 0) {
                        ఒకవేళ (ret_newly_created)
                                *ret_newly_created = true;
                        తిరిగి fd;
                }
                ఒకవేళ (errno != EEXIST)
                        తిరిగి -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                ఒకవేళ (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        తిరిగి -EEXIST;
        }
}

స్థిర పూర్ణాంకం openat_with_automount(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, mode_t బహుళకం) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        స్థిర bool can_open_వృక్షం = true;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(FLAGS_సమితి(open_flags, O_మార్గం));

        ఒకవేళ (can_open_వృక్షం) {
                పూర్ణాంకం fd = RET_NERRNO(open_tree(dir_fd, మార్గం,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_సమితి(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                ఒకవేళ (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        ఒకవేళ (FLAGS_సమితి(open_flags, O_DIRECTORY)) {
                                పూర్ణాంకం q = fd_verify_directory(fd);
                                ఒకవేళ (q < 0) {
                                        సురక్షిత_close(fd);
                                        తిరిగి q;
                                }
                        }

                        తిరిగి fd;
                }
                ఒకవేళ (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        తిరిగి fd;

                can_open_వృక్షం = false;
        }

        తిరిగి RET_NERRNO(openat(dir_fd, మార్గం, open_flags, బహుళకం));
}

పూర్ణాంకం xopenat_నిండిన_label(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, XOpenFlags xopen_flags, mode_t బహుళకం, LabelContext *label_context) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        bool made_dir = false, made_file = false;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        ప్రతిజ్ఞా(FLAGS_సమితి(open_flags, O_DIRECTORY) + FLAGS_సమితి(xopen_flags, XO_REGULAR) + FLAGS_సమితి(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        ప్రతిజ్ఞా(!FLAGS_సమితి(xopen_flags, XO_SOCKET) || FLAGS_సమితి(open_flags, O_మార్గం));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        ప్రతిజ్ఞా(!FLAGS_సమితి(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_సమితి(open_flags, O_మార్గం) && !FLAGS_సమితి(open_flags, O_CREAT)));
        ప్రతిజ్ఞా(!(FLAGS_సమితి(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_సమితి(xopen_flags, XO_SUBVOLUME)));
        ప్రతిజ్ఞా(!(FLAGS_సమితి(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        ప్రతిజ్ఞా((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        ప్రతిజ్ఞా(!FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        ఒకవేళ (బహుళకం == బహుళకం_INVALID)
                బహుళకం = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO)) {
                ఒకవేళ (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } లేకపోతే ఒకవేళ (open_flags & O_మార్గం)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (isempty(మార్గం)) {
                ప్రతిజ్ఞా(!FLAGS_సమితి(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        ఒకవేళ (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                తిరిగి TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                తిరిగి fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ పూర్ణాంకం _dir_fd = -EBADF;
        ఒకవేళ (dir_fd == XAT_FDROOT) {
                ఒకవేళ (మార్గం_is_absolute(మార్గం))
                        dir_fd = AT_FDCWD;
                లేకపోతే {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_మార్గం);
                        ఒకవేళ (_dir_fd < 0)
                                తిరిగి -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        ఒకవేళ (FLAGS_సమితి(open_flags, O_CREAT) && FLAGS_సమితి(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, మార్గం, FLAGS_సమితి(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                call_label_ops_post = true;
        }

        ఒకవేళ (FLAGS_సమితి(open_flags, O_DIRECTORY|O_CREAT)) {
                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, మార్గం, బహుళకం);
                లేకపోతే
                        r = RET_NERRNO(mkdirat(dir_fd, మార్గం, బహుళకం));
                ఒకవేళ (r == -EEXIST) {
                        ఒకవేళ (FLAGS_సమితి(open_flags, O_EXCL))
                                తిరిగి -EEXIST;
                } లేకపోతే ఒకవేళ (r < 0)
                        తిరిగి r;
                లేకపోతే
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                ఒకవేళ (FLAGS_సమితి(open_flags, O_మార్గం)) {
                        fd = FLAGS_సమితి(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, మార్గం, open_flags, బహుళకం) :
                                RET_NERRNO(openat(dir_fd, మార్గం, open_flags, బహుళకం));
                        ఒకవేళ (fd < 0) {
                                r = fd;
                                వెళ్ళు error;
                        }

                        r = fd_verify_regular(fd);
                        ఒకవేళ (r < 0)
                                వెళ్ళు error;

                } లేకపోతే ఒకవేళ (FLAGS_సమితి(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, మార్గం, open_flags|O_RDWR, బహుళకం));
                                ఒకవేళ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                లేకపోతే ఒకవేళ (fd < 0) {
                                        r = fd;
                                        వెళ్ళు error;
                                }
                        }

                        ఒకవేళ (fd < 0) {
                                fd = openat(dir_fd, మార్గం, open_flags, బహుళకం);
                                ఒకవేళ (fd < 0) {
                                        r = -errno;
                                        వెళ్ళు error;
                                }
                        }

                        made_file = true;
                } లేకపోతే {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ పూర్ణాంకం inode_fd = openat(dir_fd, మార్గం, O_మార్గం|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        ఒకవేళ (inode_fd < 0) {
                                ఒకవేళ (errno != ENOENT || !FLAGS_సమితి(open_flags, O_CREAT)) {
                                        r = -errno;
                                        వెళ్ళు error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, మార్గం, open_flags|O_RDWR, బహుళకం));
                                        ఒకవేళ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        లేకపోతే ఒకవేళ (fd < 0) {
                                                r = fd;
                                                వెళ్ళు error;
                                        }
                                }

                                ఒకవేళ (fd < 0) {
                                        fd = openat(dir_fd, మార్గం, open_flags, బహుళకం);
                                        ఒకవేళ (fd < 0) {
                                                r = -errno;
                                                వెళ్ళు error;
                                        }
                                }

                                made_file = true;
                        } లేకపోతే {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                ఒకవేళ (r < 0)
                                        వెళ్ళు error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        ఒకవేళ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        లేకపోతే ఒకవేళ (fd < 0) {
                                                r = fd;
                                                వెళ్ళు error;
                                        }
                                }

                                ఒకవేళ (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        ఒకవేళ (fd < 0) {
                                                r = fd;
                                                వెళ్ళు error;
                                        }
                                }
                        }
                }
        } లేకపోతే ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, మార్గం, open_flags, బహుళకం);
                ఒకవేళ (fd < 0) {
                        r = fd;
                        వెళ్ళు error;
                }
        } లేకపోతే {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, మార్గం, O_RDWR|open_flags, బహుళకం, &made_file);
                        ఒకవేళ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        లేకపోతే ఒకవేళ (fd < 0) {
                                r = fd;
                                వెళ్ళు error;
                        }
                }

                ఒకవేళ (fd < 0) {
                        fd = openat_report_new(dir_fd, మార్గం, open_flags, బహుళకం, &made_file);
                        ఒకవేళ (fd < 0) {
                                r = fd;
                                వెళ్ళు error;
                        }
                }
        }

        ఒకవేళ (FLAGS_సమితి(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                ఒకవేళ (r < 0)
                        వెళ్ళు error;
        }

        ఒకవేళ (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ ఖాళీ, made_file || made_dir, label_context);
                ఒకవేళ (r < 0)
                        వెళ్ళు error;
        }

        ఒకవేళ (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_సమితి(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                ఒకవేళ (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        వెళ్ళు error;
        }

        తిరిగి TAKE_FD(fd);

error:
        ఒకవేళ (call_label_ops_post)
                (శూన్య) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ఖాళీ : మార్గం, made_dir || made_file, label_context);

        ఒకవేళ (made_dir || made_file)
                (శూన్య) unlinkat(dir_fd, మార్గం, made_dir ? AT_REMOVEDIR : 0);

        తిరిగి r;
}

పూర్ణాంకం xopenat_తాళం_నిండిన_label(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం open_flags,
                XOpenFlags xopen_flags,
                mode_t బహుళకం,
                తాళం_Type locktype,
                పూర్ణాంకం operation,
                LabelContext *label_context) {

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(లోపల_సమితి(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        ఒకవేళ (FLAGS_సమితి(open_flags, O_DIRECTORY) && !లోపల_సమితి(locktype, తాళం_BSD, తాళం_NONE))
                తిరిగి -EBADF;

        కొరకు (;;) {
                నిర్మాణం stat st;

                fd = xopenat_నిండిన_label(dir_fd, మార్గం, open_flags, xopen_flags, బహుళకం, label_context);
                ఒకవేళ (fd < 0)
                        తిరిగి fd;

                r = తాళం_generic(fd, locktype, operation);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                ఒకవేళ (fstat(fd, &st) < 0)
                        తిరిగి -errno;
                ఒకవేళ (st.st_nlink > 0)
                        ఆపు;

                fd = సురక్షిత_close(fd);
        }

        తిరిగి TAKE_FD(fd);
}

పూర్ణాంకం link_fd(పూర్ణాంకం fd, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ప్రతిజ్ఞా(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_ఖాళీలు_మార్గం));
        ఒకవేళ (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_మార్గం(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                ఒకవేళ (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        తిరిగి -ENOSYS;
        }

        తిరిగి r;
}

పూర్ణాంకం linkat_replace(పూర్ణాంకం olddirfd, స్థిరరాశి అక్షరం *oldpath, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath) {
        _cleanup_close_ పూర్ణాంకం old_fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ప్రతిజ్ఞా(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ప్రతిజ్ఞా(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        ఒకవేళ (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                తిరిగి -EISDIR;

        ఒకవేళ (మార్గం_సూచిస్తుంది_directory(oldpath)) /* Refuse these definite directories early */
                తిరిగి -EISDIR;

        ఒకవేళ (మార్గం_సూచిస్తుంది_directory(newpath))
                తిరిగి -EISDIR;

        /* First, try to link this directly */
        ఒకవేళ (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        లేకపోతే
                r = link_fd(olddirfd, newdirfd, newpath);
        ఒకవేళ (r >= 0)
                తిరిగి 0;
        ఒకవేళ (r != -EEXIST)
                తిరిగి r;

        old_fd = xopenat(olddirfd, oldpath, O_మార్గం|O_CLOEXEC);
        ఒకవేళ (old_fd < 0)
                తిరిగి old_fd;

        నిర్మాణం stat old_st;
        ఒకవేళ (fstat(old_fd, &old_st) < 0)
                తిరిగి -errno;

        ఒకవేళ (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                తిరిగి -EISDIR;

        నిర్మాణం stat new_st;
        ఒకవేళ (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                తిరిగి -errno;

        ఒకవేళ (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                తిరిగి -EEXIST;

        ఒకవేళ (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                తిరిగి 0;

        _cleanup_free_ అక్షరం *tmp_మార్గం = ఖాళీ;
        r = tempfn_random(newpath, /* extra= */ ఖాళీ, &tmp_మార్గం);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = link_fd(old_fd, newdirfd, tmp_మార్గం);
        ఒకవేళ (r < 0) {
                ఒకవేళ (!ERRNO_IS_PRIVILEGE(r))
                        తిరిగి r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_మార్గం, 0));
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_మార్గం, newdirfd, newpath));
        ఒకవేళ (r < 0) {
                (శూన్య) unlinkat(newdirfd, tmp_మార్గం, /* flags= */ 0);
                తిరిగి r;
        }

        తిరిగి 0;
}
