/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <linux/falloc.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/file.h>
#অন্তর্ভাৱ <sys/mount.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "btrfs-util.h"
#অন্তর্ভাৱ "chattr-util.h"
#অন্তর্ভাৱ "dirent-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "hostname-util.h"
#অন্তর্ভাৱ "label-util.h"
#অন্তর্ভাৱ "lock-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "mkdir.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "random-util.h"
#অন্তর্ভাৱ "ratelimit.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "time-util.h"
#অন্তর্ভাৱ "tmpfile-util.h"
#অন্তর্ভাৱ "umask-util.h"

পূর্ণাঙ্ক rmdir_parents(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *stop) {
        ৱর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(stop);

        যদি (!path_is_safe(path))
                ঘূৰাই -EINVAL;

        যদি (!path_is_safe(stop))
                ঘূৰাই -EINVAL;

        p = strdupa_safe(path);

        বাবে (;;) {
                ৱর্ণ *slash = রিক্ত;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (ধ্রুৱ ৱর্ণ **) &slash, রিক্ত);
                যদি (r <= 0)
                        ঘূৰাই r;
                যদি (slash == p)
                        ঘূৰাই 0;

                প্রতিজ্ঞা(*slash == '/');
                *slash = '\0';

                যদি (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        ঘূৰাই 0;

                যদি (rmdir(p) < 0 && errno != ENOENT)
                        ঘূৰাই -errno;
        }
}

পূর্ণাঙ্ক rename_noreplace(পূর্ণাঙ্ক olddirfd, ধ্রুৱ ৱর্ণ *oldpath, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(olddirfd >= 0 || olddirfd == AT_FDCWD);
        প্রতিজ্ঞা(oldpath);
        প্রতিজ্ঞা(newdirfd >= 0 || newdirfd == AT_FDCWD);
        প্রতিজ্ঞা(newpath);

        /* Try the ideal approach first */
        যদি (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                ঘূৰাই 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        যদি (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                ঘূৰাই -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        যদি (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                যদি (r < 0) {
                        (শূন্য) unlinkat(newdirfd, newpath, 0);
                        ঘূৰাই r;
                }

                ঘূৰাই 0;
        }

        যদি (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                ঘূৰাই -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        যদি (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                ঘূৰাই -EEXIST;
        যদি (errno != ENOENT)
                ঘূৰাই -errno;

        ঘূৰাই RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

পূর্ণাঙ্ক readlinkat_malloc(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret) {
        size_t l = PATH_MAX;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);

        যদি (fd < 0 && isempty(p))
                ঘূৰাই -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        বাবে (;;) {
                _cleanup_free_ ৱর্ণ *c = রিক্ত;
                ssize_t n;

                c = new(ৱর্ণ, l+1);
                যদি (!c)
                        ঘূৰাই -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                যদি (n < 0)
                        ঘূৰাই -errno;

                যদি ((size_t) n < l) {
                        c[n] = 0;

                        যদি (ret)
                                *ret = TAKE_PTR(c);

                        ঘূৰাই 0;
                }

                যদি (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        ঘূৰাই -EFBIG;

                l *= 2;
        }
}

পূর্ণাঙ্ক readlink_মান(ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *link = রিক্ত, *name = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        r = readlink_malloc(p, &link);
        যদি (r < 0)
                ঘূৰাই r;

        r = path_extract_filename(link, &name);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == O_DIRECTORY)
                ঘূৰাই -EINVAL;

        *ret = TAKE_PTR(name);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক readlink_and_make_absolute(ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *target = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(ret);

        r = readlink_malloc(p, &target);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই file_in_same_dir(p, target, ret);
}

পূর্ণাঙ্ক chmod_and_chown_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);

        যদি (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                যদি (fd < 0)
                        ঘূৰাই -errno;
                dir_fd = fd;

        } অন্যথা যদি (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                যদি (fd < 0)
                        ঘূৰাই -errno;
                dir_fd = fd;
        }

        ঘূৰাই fchmod_and_chown(dir_fd, mode, uid, gid);
}

পূর্ণাঙ্ক fchmod_and_chown_with_fallback(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        সংরচনা stat st;
        পূর্ণাঙ্ক r;

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

        যদি (fstat(fd, &st) < 0)
                ঘূৰাই -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        যদি (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        অন্যথা যদি ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                ঘূৰাই -EINVAL; /* insist on the right file type if it was specified */

        যদি (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                যদি (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        যদি (r < 0) {
                                যদি (!path || r != -ENOSYS)
                                        ঘূৰাই r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                যদি (chmod(path, minimal & 07777) < 0)
                                        ঘূৰাই -errno;
                        }
                }
        }

        যদি (do_chown)
                যদি (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        ঘূৰাই -errno;

        যদি (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                যদি (r < 0) {
                        যদি (!path || r != -ENOSYS)
                                ঘূৰাই r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        যদি (chmod(path, mode & 07777) < 0)
                                ঘূৰাই -errno;
                }
        }

        ঘূৰাই do_chown || do_chmod;
}

পূর্ণাঙ্ক fchmod_umask(পূর্ণাঙ্ক fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        ঘূৰাই RET_NERRNO(fchmod(fd, m & (~u)));
}

পূর্ণাঙ্ক fchmod_opath(পূর্ণাঙ্ক fd, mode_t m) {
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

        প্রতিজ্ঞা(fd >= 0);

        যদি (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                ঘূৰাই 0;
        যদি (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                ঘূৰাই 0;
        যদি (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                ঘূৰাই -errno;

        যদি (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                যদি (errno != ENOENT)
                        ঘূৰাই -errno;

                ঘূৰাই proc_fd_enoent_errno();
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক futimens_opath(পূর্ণাঙ্ক fd, ধ্রুৱ সংরচনা timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        প্রতিজ্ঞা(fd >= 0);

        ঘূৰাই RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

পূর্ণাঙ্ক stat_warn_permissions(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ সংরচনা stat *st) {
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        যদি (!S_ISREG(st->st_mode))
                ঘূৰাই 0;

        যদি (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        যদি (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        যদি (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক fd_warn_permissions(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক fd) {
        সংরচনা stat st;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(fd >= 0);

        যদি (fstat(fd, &st) < 0)
                ঘূৰাই -errno;

        ঘূৰাই stat_warn_permissions(path, &st);
}

পূর্ণাঙ্ক access_nofollow(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক mode) {
        ঘূৰাই RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

পূর্ণাঙ্ক access_fd(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক mode) {
        /* Like access() but operates on an already open fd */

        যদি (fd == AT_FDCWD)
                ঘূৰাই RET_NERRNO(access(".", mode));
        যদি (fd == XAT_FDROOT)
                ঘূৰাই RET_NERRNO(access("/", mode));

        প্রতিজ্ঞা(fd >= 0);

        ঘূৰাই RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

পূর্ণাঙ্ক touch_fd(পূর্ণাঙ্ক fd, usec_t stamp) {
        প্রতিজ্ঞা(fd >= 0);

        যদি (stamp == USEC_INFINITY)
                ঘূৰাই futimens_opath(fd, /* ts= */ রিক্ত);

        সংরচনা timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        ঘূৰাই futimens_opath(fd, ts);
}

পূর্ণাঙ্ক touch_file(ধ্রুৱ ৱর্ণ *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক ret;

        প্রতিজ্ঞা(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        যদি (parents)
                (শূন্য) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        যদি (fd < 0) {
                যদি (errno != ENOENT)
                        ঘূৰাই -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                যদি (fd < 0)
                        ঘূৰাই -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        ঘূৰাই RET_GATHER(ret, touch_fd(fd, stamp));
}

পূর্ণাঙ্ক touch(ধ্রুৱ ৱর্ণ *path) {
        ঘূৰাই touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

পূর্ণাঙ্ক symlinkat_idempotent(ধ্রুৱ ৱর্ণ *target, পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *linkpath, bool make_relative) {
        _cleanup_free_ ৱর্ণ *relpath = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(linkpath);

        যদি (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                যদি (r < 0)
                        ঘূৰাই r;

                target = relpath;
        }

        যদি (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ ৱর্ণ *p = রিক্ত;

                যদি (errno != EEXIST)
                        ঘূৰাই -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                যদি (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        ঘূৰাই -EEXIST;
                যদি (r < 0) /* Any other error? In that case propagate it as is */
                        ঘূৰাই r;

                যদি (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        ঘূৰাই -EEXIST;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক symlinkat_atomic_full_label(ধ্রুৱ ৱর্ণ *target, পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(target);
        প্রতিজ্ঞা(linkpath);

        _cleanup_free_ ৱর্ণ *relpath = রিক্ত;
        যদি (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                যদি (r < 0)
                        ঘূৰাই r;

                target = relpath;
        }

        _cleanup_free_ ৱর্ণ *t = রিক্ত;
        r = tempfn_random(linkpath, রিক্ত, &t);
        যদি (r < 0)
                ঘূৰাই r;

        bool call_label_ops_post = false;
        যদি (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                যদি (r < 0)
                        ঘূৰাই r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        যদি (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        যদি (r < 0)
                ঘূৰাই r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        যদি (r < 0) {
                (শূন্য) unlinkat(atfd, t, 0);
                ঘূৰাই r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক mknodat_atomic(পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *path, mode_t mode, dev_t dev) {
        _cleanup_free_ ৱর্ণ *t = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        r = tempfn_random(path, রিক্ত, &t);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (mknodat(atfd, t, mode, dev) < 0)
                ঘূৰাই -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        যদি (r < 0) {
                (শূন্য) unlinkat(atfd, t, 0);
                ঘূৰাই r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক mkfifoat_atomic(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode) {
        _cleanup_free_ ৱর্ণ *t = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, রিক্ত, &t);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (mkfifoat(dir_fd, t, mode) < 0)
                ঘূৰাই -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        যদি (r < 0) {
                (শূন্য) unlinkat(dir_fd, t, 0);
                ঘূৰাই r;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক get_files_in_directory(ধ্রুৱ ৱর্ণ *path, ৱর্ণ ***ret_list) {
        _cleanup_strv_free_ ৱর্ণ **l = রিক্ত;
        _cleanup_closedir_ DIR *d = রিক্ত;
        size_t n = 0;

        প্রতিজ্ঞা(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        যদি (!d)
                ঘূৰাই -errno;

        FOREACH_DIRENT_ALL(de, d, ঘূৰাই -errno) {
                যদি (!dirent_is_file(de))
                        অনুৱর্ত;

                যদি (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        যদি (!GREEDY_REALLOC(l, n + 2))
                                ঘূৰাই -ENOMEM;

                        l[n] = strdup(de->d_name);
                        যদি (!l[n])
                                ঘূৰাই -ENOMEM;

                        l[++n] = রিক্ত;
                } অন্যথা
                        n++;
        }

        যদি (ret_list)
                *ret_list = TAKE_PTR(l);

        ঘূৰাই n;
}

স্থির পূর্ণাঙ্ক getenv_tmp_dir(ধ্রুৱ ৱর্ণ **ret_path) {
        পূর্ণাঙ্ক r, ret = 0;

        প্রতিজ্ঞা(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ধ্রুৱ ৱর্ণ *e;

                e = secure_getenv(n);
                যদি (!e)
                        অনুৱর্ত;
                যদি (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        গচ্ছ next;
                }
                যদি (!path_is_normalized(e)) {
                        r = -EPERM;
                        গচ্ছ next;
                }

                r = is_dir(e, true);
                যদি (r < 0)
                        গচ্ছ next;
                যদি (r == 0) {
                        r = -ENOTDIR;
                        গচ্ছ next;
                }

                *ret_path = e;
                ঘূৰাই 1;

        next:
                /* Remember first error, to make this more debuggable */
                যদি (ret >= 0)
                        ret = r;
        }

        যদি (ret < 0)
                ঘূৰাই ret;

        *ret_path = রিক্ত;
        ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক tmp_dir_internal(ধ্রুৱ ৱর্ণ *def, ধ্রুৱ ৱর্ণ **ret) {
        ধ্রুৱ ৱর্ণ *e;
        পূর্ণাঙ্ক r, k;

        প্রতিজ্ঞা(def);
        প্রতিজ্ঞা(ret);

        r = getenv_tmp_dir(&e);
        যদি (r > 0) {
                *ret = e;
                ঘূৰাই 0;
        }

        k = is_dir(def, /* follow= */ true);
        যদি (k == 0)
                k = -ENOTDIR;
        যদি (k < 0)
                ঘূৰাই RET_GATHER(r, k);

        *ret = def;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক var_tmp_dir(ধ্রুৱ ৱর্ণ **ret) {
        প্রতিজ্ঞা(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        ঘূৰাই tmp_dir_internal("/var/tmp", ret);
}

পূর্ণাঙ্ক tmp_dir(ধ্রুৱ ৱর্ণ **ret) {
        প্রতিজ্ঞা(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        ঘূৰাই tmp_dir_internal("/tmp", ret);
}

পূর্ণাঙ্ক unlink_or_warn(ধ্রুৱ ৱর্ণ *filename) {
        প্রতিজ্ঞা(filename);

        যদি (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                যদি (errno != EROFS || access(filename, F_OK) >= 0)
                        ঘূৰাই log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        ঘূৰাই 0;
}

ৱর্ণ *rmdir_and_free(ৱর্ণ *p) {
        PROTECT_ERRNO;

        যদি (!p)
                ঘূৰাই রিক্ত;

        (শূন্য) rmdir(p);
        ঘূৰাই mfree(p);
}

ৱর্ণ* unlink_and_free(ৱর্ণ *p) {
        PROTECT_ERRNO;

        যদি (!p)
                ঘূৰাই রিক্ত;

        (শূন্য) unlink(p);
        ঘূৰাই mfree(p);
}

পূর্ণাঙ্ক unlinkat_deallocate(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ পূর্ণাঙ্ক truncate_fd = -EBADF;
        সংরচনা stat st;
        off_t l, bs;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        যদি (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                যদি (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        যদি (IN_SET(errno, ENOENT, EISDIR))
                                ঘূৰাই -errno;

                        যদি (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        যদি (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                ঘূৰাই -errno;

        যদি (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                ঘূৰাই 0;

        যদি (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                ঘূৰাই 0;
        }

        যদি (!S_ISREG(st.st_mode))
                ঘূৰাই 0;

        যদি (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t বাম = st.st_size;
                ৱর্ণ buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, পরিমাণ(buffer));

                যেতিয়ালৈ (বাম > 0) {
                        ssize_t n;

                        n = লিখ(truncate_fd, buffer, MIN(পরিমাণ(buffer), বাম));
                        যদি (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                ভঙ্গ;
                        }

                        প্রতিজ্ঞা(বাম >= (size_t) n);
                        বাম -= n;
                }

                /* Let's refresh metadata */
                যদি (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        ঘূৰাই 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        যদি (st.st_blocks == 0 || st.st_nlink > 0)
                ঘূৰাই 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        যদি (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                ঘূৰাই 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        যদি (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                ঘূৰাই 0;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক open_parent_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, mode_t mode) {
        _cleanup_free_ ৱর্ণ *parent = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(path);

        r = path_extract_directory(path, &parent);
        যদি (r == -EDESTADDRREQ) {
                parent = strdup(".");
                যদি (!parent)
                        ঘূৰাই -ENOMEM;
        } অন্যথা যদি (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                যদি (!parent)
                        ঘূৰাই -ENOMEM;
        } অন্যথা যদি (r < 0)
                ঘূৰাই r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        যদি (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        ঘূৰাই RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

পূর্ণাঙ্ক conservative_renameat(
                পূর্ণাঙ্ক olddirfd, ধ্রুৱ ৱর্ণ *oldpath,
                পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath) {

        _cleanup_close_ পূর্ণাঙ্ক old_fd = -EBADF, new_fd = -EBADF;
        সংরচনা stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        যদি (old_fd < 0)
                গচ্ছ do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        যদি (new_fd < 0)
                গচ্ছ do_rename;

        যদি (fstat(old_fd, &old_stat) < 0)
                গচ্ছ do_rename;

        যদি (!S_ISREG(old_stat.st_mode))
                গচ্ছ do_rename;

        যদি (fstat(new_fd, &new_stat) < 0)
                গচ্ছ do_rename;

        যদি (stat_inode_same(&new_stat, &old_stat))
                গচ্ছ is_same;

        যদি (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                গচ্ছ do_rename;

        বাবে (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[পরিমাণ(buf1)];
                ssize_t l1, l2;

                l1 = পঠ(old_fd, buf1, পরিমাণ(buf1));
                যদি (l1 < 0)
                        গচ্ছ do_rename;

                যদি (l1 == পরিমাণ(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = পঠ(new_fd, buf2, l1);
                অন্যথা {
                        প্রতিজ্ঞা((size_t) l1 < পরিমাণ(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        প্রতিজ্ঞা((size_t) (l1 + 1) <= পরিমাণ(buf2));
                        l2 = পঠ(new_fd, buf2, l1 + 1);
                }
                যদি (l2 != l1)
                        গচ্ছ do_rename;

                যদি (memcmp(buf1, buf2, l1) != 0)
                        গচ্ছ do_rename;

                যদি ((size_t) l1 < পরিমাণ(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        ভঙ্গ;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        যদি (unlinkat(olddirfd, oldpath, 0) < 0)
                গচ্ছ do_rename;

        ঘূৰাই 0;

do_rename:
        যদি (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                ঘূৰাই -errno;

        ঘূৰাই 1;
}

পূর্ণাঙ্ক posix_fallocate_loop(পূর্ণাঙ্ক fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        পূর্ণাঙ্ক r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        যদি (r != EINTR)
                ঘূৰাই -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ধ্রুৱ RateLimit) { 10 * USEC_PER_SEC, 16 };
        যেতিয়ালৈ (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                যদি (r != EINTR)
                        ঘূৰাই -r;
        }

        ঘূৰাই -EINTR;
}

পূর্ণাঙ্ক parse_cifs_service(
                ধ্রুৱ ৱর্ণ *s,
                ৱর্ণ **ret_host,
                ৱর্ণ **ret_service,
                ৱর্ণ **ret_path) {

        _cleanup_free_ ৱর্ণ *h = রিক্ত, *ss = রিক্ত, *x = রিক্ত;
        ধ্রুৱ ৱর্ণ *p, *e, *d;
        ৱর্ণ delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        যদি (!s)
                ঘূৰাই -EINVAL;

        p = startswith(s, "//");
        যদি (!p) {
                p = startswith(s, "\\\\");
                যদি (!p)
                        ঘূৰাই -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        যদি (!e)
                ঘূৰাই -EINVAL;

        h = strndup(p, e - p);
        যদি (!h)
                ঘূৰাই -ENOMEM;

        যদি (!hostname_is_valid(h, 0))
                ঘূৰাই -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        যদি (!ss)
                ঘূৰাই -ENOMEM;

        যদি (!filename_is_valid(ss))
                ঘূৰাই -EINVAL;

        যদি (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                যদি (!x)
                        ঘূৰাই -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                বাবে (ৱর্ণ *i = x; *i; i++)
                        যদি (*i == delimiter)
                                *i = '/';

                যদি (!path_is_valid(x))
                        ঘূৰাই -EINVAL;

                path_simplify(x);
                যদি (!path_is_normalized(x))
                        ঘূৰাই -EINVAL;
        }

        যদি (ret_host)
                *ret_host = TAKE_PTR(h);
        যদি (ret_service)
                *ret_service = TAKE_PTR(ss);
        যদি (ret_path)
                *ret_path = TAKE_PTR(x);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক open_mkdir_at_full_label(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ ৱর্ণ *fname = রিক্ত, *parent = রিক্ত;
        পূর্ণাঙ্ক r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        যদি (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                ঘূৰাই -EINVAL;
        যদি ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                ঘূৰাই -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        যদি (r < 0) {
                যদি (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        ঘূৰাই r;
        } অন্যথা {
                r = path_extract_filename(path, &fname);
                যদি (r < 0)
                        ঘূৰাই r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                যদি (parent_fd < 0)
                        ঘূৰাই -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        যদি (IN_SET(fd, -ELOOP, -ENOTDIR))
                ঘূৰাই -EEXIST;
        যদি (fd < 0)
                ঘূৰাই fd;

        ঘূৰাই TAKE_FD(fd);
}

পূর্ণাঙ্ক openat_report_new(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *pathname, পূর্ণাঙ্ক flags, mode_t mode, bool *ret_newly_created) {
        পূর্ণাঙ্ক fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        যদি (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                যদি (fd < 0)
                        ঘূৰাই -errno;

                যদি (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                ঘূৰাই fd;
        }

        বাবে (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                যদি (fd >= 0) {
                        যদি (ret_newly_created)
                                *ret_newly_created = false;
                        ঘূৰাই fd;
                }
                যদি (errno != ENOENT)
                        ঘূৰাই -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                যদি (fd >= 0) {
                        যদি (ret_newly_created)
                                *ret_newly_created = true;
                        ঘূৰাই fd;
                }
                যদি (errno != EEXIST)
                        ঘূৰাই -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                যদি (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        ঘূৰাই -EEXIST;
        }
}

স্থির পূর্ণাঙ্ক openat_with_automount(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        স্থির bool can_open_বৃক্ষ = true;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(FLAGS_SET(open_flags, O_PATH));

        যদি (can_open_বৃক্ষ) {
                পূর্ণাঙ্ক fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                যদি (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        যদি (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                পূর্ণাঙ্ক q = fd_verify_directory(fd);
                                যদি (q < 0) {
                                        safe_close(fd);
                                        ঘূৰাই q;
                                }
                        }

                        ঘূৰাই fd;
                }
                যদি (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        ঘূৰাই fd;

                can_open_বৃক্ষ = false;
        }

        ঘূৰাই RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

পূর্ণাঙ্ক xopenat_full_label(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        bool made_dir = false, made_file = false;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        প্রতিজ্ঞা(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        প্রতিজ্ঞা(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        প্রতিজ্ঞা(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        প্রতিজ্ঞা(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        প্রতিজ্ঞা(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        প্রতিজ্ঞা((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        প্রতিজ্ঞা(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        যদি (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        যদি (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                যদি (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } অন্যথা যদি (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        ঘূৰাই -EINVAL;
        }

        যদি (isempty(path)) {
                প্রতিজ্ঞা(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                যদি (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        যদি (r < 0)
                                ঘূৰাই r;
                }

                যদি (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        যদি (r < 0)
                                ঘূৰাই r;
                }

                যদি (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        যদি (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                ঘূৰাই TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                ঘূৰাই fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ পূর্ণাঙ্ক _dir_fd = -EBADF;
        যদি (dir_fd == XAT_FDROOT) {
                যদি (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                অন্যথা {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        যদি (_dir_fd < 0)
                                ঘূৰাই -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        যদি (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                যদি (r < 0)
                        ঘূৰাই r;

                call_label_ops_post = true;
        }

        যদি (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                যদি (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                অন্যথা
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                যদি (r == -EEXIST) {
                        যদি (FLAGS_SET(open_flags, O_EXCL))
                                ঘূৰাই -EEXIST;
                } অন্যথা যদি (r < 0)
                        ঘূৰাই r;
                অন্যথা
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        যদি (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                যদি (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        যদি (fd < 0) {
                                r = fd;
                                গচ্ছ error;
                        }

                        r = fd_verify_regular(fd);
                        যদি (r < 0)
                                গচ্ছ error;

                } অন্যথা যদি (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        যদি (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                যদি (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                অন্যথা যদি (fd < 0) {
                                        r = fd;
                                        গচ্ছ error;
                                }
                        }

                        যদি (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                যদি (fd < 0) {
                                        r = -errno;
                                        গচ্ছ error;
                                }
                        }

                        made_file = true;
                } অন্যথা {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ পূর্ণাঙ্ক inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        যদি (inode_fd < 0) {
                                যদি (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        গচ্ছ error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                যদি (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        যদি (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        অন্যথা যদি (fd < 0) {
                                                r = fd;
                                                গচ্ছ error;
                                        }
                                }

                                যদি (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        যদি (fd < 0) {
                                                r = -errno;
                                                গচ্ছ error;
                                        }
                                }

                                made_file = true;
                        } অন্যথা {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                যদি (r < 0)
                                        গচ্ছ error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                যদি (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        যদি (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        অন্যথা যদি (fd < 0) {
                                                r = fd;
                                                গচ্ছ error;
                                        }
                                }

                                যদি (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        যদি (fd < 0) {
                                                r = fd;
                                                গচ্ছ error;
                                        }
                                }
                        }
                }
        } অন্যথা যদি (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                যদি (fd < 0) {
                        r = fd;
                        গচ্ছ error;
                }
        } অন্যথা {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                যদি (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        যদি (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        অন্যথা যদি (fd < 0) {
                                r = fd;
                                গচ্ছ error;
                        }
                }

                যদি (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        যদি (fd < 0) {
                                r = fd;
                                গচ্ছ error;
                        }
                }
        }

        যদি (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                যদি (r < 0)
                        গচ্ছ error;
        }

        যদি (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ রিক্ত, made_file || made_dir, label_context);
                যদি (r < 0)
                        গচ্ছ error;
        }

        যদি (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                যদি (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        গচ্ছ error;
        }

        ঘূৰাই TAKE_FD(fd);

error:
        যদি (call_label_ops_post)
                (শূন্য) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? রিক্ত : path, made_dir || made_file, label_context);

        যদি (made_dir || made_file)
                (শূন্য) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        ঘূৰাই r;
}

পূর্ণাঙ্ক xopenat_lock_full_label(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুৱ ৱর্ণ *path,
                পূর্ণাঙ্ক open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                পূর্ণাঙ্ক operation,
                LabelContext *label_context) {

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        যদি (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                ঘূৰাই -EBADF;

        বাবে (;;) {
                সংরচনা stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                যদি (fd < 0)
                        ঘূৰাই fd;

                r = lock_generic(fd, locktype, operation);
                যদি (r < 0)
                        ঘূৰাই r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                যদি (fstat(fd, &st) < 0)
                        ঘূৰাই -errno;
                যদি (st.st_nlink > 0)
                        ভঙ্গ;

                fd = safe_close(fd);
        }

        ঘূৰাই TAKE_FD(fd);
}

পূর্ণাঙ্ক link_fd(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(newdirfd >= 0 || newdirfd == AT_FDCWD);
        প্রতিজ্ঞা(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        যদি (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                যদি (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        ঘূৰাই -ENOSYS;
        }

        ঘূৰাই r;
}

পূর্ণাঙ্ক linkat_replace(পূর্ণাঙ্ক olddirfd, ধ্রুৱ ৱর্ণ *oldpath, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath) {
        _cleanup_close_ পূর্ণাঙ্ক old_fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(olddirfd >= 0 || olddirfd == AT_FDCWD);
        প্রতিজ্ঞা(newdirfd >= 0 || newdirfd == AT_FDCWD);
        প্রতিজ্ঞা(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        যদি (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                ঘূৰাই -EISDIR;

        যদি (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                ঘূৰাই -EISDIR;

        যদি (path_implies_directory(newpath))
                ঘূৰাই -EISDIR;

        /* First, try to link this directly */
        যদি (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        অন্যথা
                r = link_fd(olddirfd, newdirfd, newpath);
        যদি (r >= 0)
                ঘূৰাই 0;
        যদি (r != -EEXIST)
                ঘূৰাই r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        যদি (old_fd < 0)
                ঘূৰাই old_fd;

        সংরচনা stat old_st;
        যদি (fstat(old_fd, &old_st) < 0)
                ঘূৰাই -errno;

        যদি (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                ঘূৰাই -EISDIR;

        সংরচনা stat new_st;
        যদি (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                ঘূৰাই -errno;

        যদি (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                ঘূৰাই -EEXIST;

        যদি (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                ঘূৰাই 0;

        _cleanup_free_ ৱর্ণ *tmp_path = রিক্ত;
        r = tempfn_random(newpath, /* extra= */ রিক্ত, &tmp_path);
        যদি (r < 0)
                ঘূৰাই r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        যদি (r < 0) {
                যদি (!ERRNO_IS_PRIVILEGE(r))
                        ঘূৰাই r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                যদি (r < 0)
                        ঘূৰাই r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        যদি (r < 0) {
                (শূন্য) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                ঘূৰাই r;
        }

        ঘূৰাই 0;
}
