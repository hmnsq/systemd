/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/falloc.h>
#ضمن <stdlib.h>
#ضمن <sys/file.h>
#ضمن <sys/mount.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "btrfs-util.h"
#ضمن "chattr-util.h"
#ضمن "dirent-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "hostname-util.h"
#ضمن "label-util.h"
#ضمن "lock-util.h"
#ضمن "log.h"
#ضمن "mkdir.h"
#ضمن "path-util.h"
#ضمن "process-util.h"
#ضمن "random-util.h"
#ضمن "ratelimit.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "time-util.h"
#ضمن "tmpfile-util.h"
#ضمن "umask-util.h"

صحيح rmdir_parents(ثابت حرف *path, ثابت حرف *stop) {
        حرف *p;
        صحيح r;

        أكد(path);
        أكد(stop);

        𐡄𐡍 (!path_is_safe(path))
                أرجع -EINVAL;

        𐡄𐡍 (!path_is_safe(stop))
                أرجع -EINVAL;

        p = strdupa_safe(path);

        لكل (;;) {
                حرف *slash = عدم;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (ثابت حرف **) &slash, عدم);
                𐡄𐡍 (r <= 0)
                        أرجع r;
                𐡄𐡍 (slash == p)
                        أرجع 0;

                أكد(*slash == '/');
                *slash = '\0';

                𐡄𐡍 (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        أرجع 0;

                𐡄𐡍 (rmdir(p) < 0 && errno != ENOENT)
                        أرجع -errno;
        }
}

صحيح rename_noreplace(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath) {
        صحيح r;

        أكد(olddirfd >= 0 || olddirfd == AT_FDCWD);
        أكد(oldpath);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(newpath);

        /* Try the ideal approach first */
        𐡄𐡍 (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                أرجع 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        𐡄𐡍 (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                أرجع -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        𐡄𐡍 (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                𐡄𐡍 (r < 0) {
                        (فراغ) unlinkat(newdirfd, newpath, 0);
                        أرجع r;
                }

                أرجع 0;
        }

        𐡄𐡍 (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                أرجع -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        𐡄𐡍 (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                أرجع -EEXIST;
        𐡄𐡍 (errno != ENOENT)
                أرجع -errno;

        أرجع RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

صحيح readlinkat_malloc(صحيح fd, ثابت حرف *p, حرف **ret) {
        size_t l = PATH_MAX;

        أكد(fd >= 0 || fd == AT_FDCWD);

        𐡄𐡍 (fd < 0 && isempty(p))
                أرجع -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        لكل (;;) {
                _cleanup_free_ حرف *c = عدم;
                ssize_t n;

                c = new(حرف, l+1);
                𐡄𐡍 (!c)
                        أرجع -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                𐡄𐡍 (n < 0)
                        أرجع -errno;

                𐡄𐡍 ((size_t) n < l) {
                        c[n] = 0;

                        𐡄𐡍 (ret)
                                *ret = TAKE_PTR(c);

                        أرجع 0;
                }

                𐡄𐡍 (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        أرجع -EFBIG;

                l *= 2;
        }
}

صحيح readlink_value(ثابت حرف *p, حرف **ret) {
        _cleanup_free_ حرف *link = عدم, *name = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        r = readlink_malloc(p, &link);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = path_extract_filename(link, &name);
        𐡄𐡍 (r < 0)
                أرجع r;
        𐡄𐡍 (r == O_DIRECTORY)
                أرجع -EINVAL;

        *ret = TAKE_PTR(name);
        أرجع 0;
}

صحيح readlink_and_make_absolute(ثابت حرف *p, حرف **ret) {
        _cleanup_free_ حرف *target = عدم;
        صحيح r;

        أكد(p);
        أكد(ret);

        r = readlink_malloc(p, &target);
        𐡄𐡍 (r < 0)
                أرجع r;

        أرجع file_in_same_dir(p, target, ret);
}

صحيح chmod_and_chown_at(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        𐡄𐡍 (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                𐡄𐡍 (fd < 0)
                        أرجع -errno;
                dir_fd = fd;

        } وإلا 𐡄𐡍 (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                𐡄𐡍 (fd < 0)
                        أرجع -errno;
                dir_fd = fd;
        }

        أرجع fchmod_and_chown(dir_fd, mode, uid, gid);
}

صحيح fchmod_and_chown_with_fallback(صحيح fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        بنية stat st;
        صحيح r;

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

        𐡄𐡍 (fstat(fd, &st) < 0)
                أرجع -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        𐡄𐡍 (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        وإلا 𐡄𐡍 ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                أرجع -EINVAL; /* insist on the right file type if it was specified */

        𐡄𐡍 (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                𐡄𐡍 (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        𐡄𐡍 (r < 0) {
                                𐡄𐡍 (!path || r != -ENOSYS)
                                        أرجع r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                𐡄𐡍 (chmod(path, minimal & 07777) < 0)
                                        أرجع -errno;
                        }
                }
        }

        𐡄𐡍 (do_chown)
                𐡄𐡍 (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        أرجع -errno;

        𐡄𐡍 (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                𐡄𐡍 (r < 0) {
                        𐡄𐡍 (!path || r != -ENOSYS)
                                أرجع r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        𐡄𐡍 (chmod(path, mode & 07777) < 0)
                                أرجع -errno;
                }
        }

        أرجع do_chown || do_chmod;
}

صحيح fchmod_umask(صحيح fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        أرجع RET_NERRNO(fchmod(fd, m & (~u)));
}

صحيح fchmod_opath(صحيح fd, mode_t m) {
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

        أكد(fd >= 0);

        𐡄𐡍 (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                أرجع 0;
        𐡄𐡍 (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                أرجع 0;
        𐡄𐡍 (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                أرجع -errno;

        𐡄𐡍 (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                𐡄𐡍 (errno != ENOENT)
                        أرجع -errno;

                أرجع proc_fd_enoent_errno();
        }

        أرجع 0;
}

صحيح futimens_opath(صحيح fd, ثابت بنية timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        أكد(fd >= 0);

        أرجع RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

صحيح stat_warn_permissions(ثابت حرف *path, ثابت بنية stat *st) {
        أكد(path);
        أكد(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        𐡄𐡍 (!S_ISREG(st->st_mode))
                أرجع 0;

        𐡄𐡍 (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        𐡄𐡍 (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        𐡄𐡍 (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        أرجع 0;
}

صحيح fd_warn_permissions(ثابت حرف *path, صحيح fd) {
        بنية stat st;

        أكد(path);
        أكد(fd >= 0);

        𐡄𐡍 (fstat(fd, &st) < 0)
                أرجع -errno;

        أرجع stat_warn_permissions(path, &st);
}

صحيح access_nofollow(ثابت حرف *path, صحيح mode) {
        أرجع RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

صحيح access_fd(صحيح fd, صحيح mode) {
        /* Like access() but operates on an already open fd */

        𐡄𐡍 (fd == AT_FDCWD)
                أرجع RET_NERRNO(access(".", mode));
        𐡄𐡍 (fd == XAT_FDROOT)
                أرجع RET_NERRNO(access("/", mode));

        أكد(fd >= 0);

        أرجع RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

صحيح touch_fd(صحيح fd, usec_t stamp) {
        أكد(fd >= 0);

        𐡄𐡍 (stamp == USEC_INFINITY)
                أرجع futimens_opath(fd, /* ts= */ عدم);

        بنية timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        أرجع futimens_opath(fd, ts);
}

صحيح touch_file(ثابت حرف *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح ret;

        أكد(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        𐡄𐡍 (parents)
                (فراغ) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        𐡄𐡍 (fd < 0) {
                𐡄𐡍 (errno != ENOENT)
                        أرجع -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                𐡄𐡍 (fd < 0)
                        أرجع -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        أرجع RET_GATHER(ret, touch_fd(fd, stamp));
}

صحيح touch(ثابت حرف *path) {
        أرجع touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

صحيح symlinkat_idempotent(ثابت حرف *target, صحيح atfd, ثابت حرف *linkpath, bool make_relative) {
        _cleanup_free_ حرف *relpath = عدم;
        صحيح r;

        أكد(target);
        أكد(linkpath);

        𐡄𐡍 (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                𐡄𐡍 (r < 0)
                        أرجع r;

                target = relpath;
        }

        𐡄𐡍 (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ حرف *p = عدم;

                𐡄𐡍 (errno != EEXIST)
                        أرجع -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                𐡄𐡍 (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        أرجع -EEXIST;
                𐡄𐡍 (r < 0) /* Any other error? In that case propagate it as is */
                        أرجع r;

                𐡄𐡍 (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        أرجع -EEXIST;
        }

        أرجع 0;
}

صحيح symlinkat_atomic_full_label(ثابت حرف *target, صحيح atfd, ثابت حرف *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        صحيح r;

        أكد(target);
        أكد(linkpath);

        _cleanup_free_ حرف *relpath = عدم;
        𐡄𐡍 (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                𐡄𐡍 (r < 0)
                        أرجع r;

                target = relpath;
        }

        _cleanup_free_ حرف *t = عدم;
        r = tempfn_random(linkpath, عدم, &t);
        𐡄𐡍 (r < 0)
                أرجع r;

        bool call_label_ops_post = false;
        𐡄𐡍 (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                𐡄𐡍 (r < 0)
                        أرجع r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        𐡄𐡍 (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        𐡄𐡍 (r < 0)
                أرجع r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        𐡄𐡍 (r < 0) {
                (فراغ) unlinkat(atfd, t, 0);
                أرجع r;
        }

        أرجع 0;
}

صحيح mknodat_atomic(صحيح atfd, ثابت حرف *path, mode_t mode, dev_t dev) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(path);

        r = tempfn_random(path, عدم, &t);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (mknodat(atfd, t, mode, dev) < 0)
                أرجع -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        𐡄𐡍 (r < 0) {
                (فراغ) unlinkat(atfd, t, 0);
                أرجع r;
        }

        أرجع 0;
}

صحيح mkfifoat_atomic(صحيح dir_fd, ثابت حرف *path, mode_t mode) {
        _cleanup_free_ حرف *t = عدم;
        صحيح r;

        أكد(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, عدم, &t);
        𐡄𐡍 (r < 0)
                أرجع r;

        𐡄𐡍 (mkfifoat(dir_fd, t, mode) < 0)
                أرجع -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        𐡄𐡍 (r < 0) {
                (فراغ) unlinkat(dir_fd, t, 0);
                أرجع r;
        }

        أرجع 0;
}

صحيح get_files_in_directory(ثابت حرف *path, حرف ***ret_list) {
        _cleanup_strv_free_ حرف **l = عدم;
        _cleanup_closedir_ DIR *d = عدم;
        size_t n = 0;

        أكد(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        𐡄𐡍 (!d)
                أرجع -errno;

        FOREACH_DIRENT_ALL(de, d, أرجع -errno) {
                𐡄𐡍 (!dirent_is_file(de))
                        استمر;

                𐡄𐡍 (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        𐡄𐡍 (!GREEDY_REALLOC(l, n + 2))
                                أرجع -ENOMEM;

                        l[n] = strdup(de->d_name);
                        𐡄𐡍 (!l[n])
                                أرجع -ENOMEM;

                        l[++n] = عدم;
                } وإلا
                        n++;
        }

        𐡄𐡍 (ret_list)
                *ret_list = TAKE_PTR(l);

        أرجع n;
}

ساكن صحيح getenv_tmp_dir(ثابت حرف **ret_path) {
        صحيح r, ret = 0;

        أكد(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ثابت حرف *e;

                e = secure_getenv(n);
                𐡄𐡍 (!e)
                        استمر;
                𐡄𐡍 (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        اذهب next;
                }
                𐡄𐡍 (!path_is_normalized(e)) {
                        r = -EPERM;
                        اذهب next;
                }

                r = is_dir(e, true);
                𐡄𐡍 (r < 0)
                        اذهب next;
                𐡄𐡍 (r == 0) {
                        r = -ENOTDIR;
                        اذهب next;
                }

                *ret_path = e;
                أرجع 1;

        next:
                /* Remember first error, to make this more debuggable */
                𐡄𐡍 (ret >= 0)
                        ret = r;
        }

        𐡄𐡍 (ret < 0)
                أرجع ret;

        *ret_path = عدم;
        أرجع ret;
}

ساكن صحيح tmp_dir_internal(ثابت حرف *def, ثابت حرف **ret) {
        ثابت حرف *e;
        صحيح r, k;

        أكد(def);
        أكد(ret);

        r = getenv_tmp_dir(&e);
        𐡄𐡍 (r > 0) {
                *ret = e;
                أرجع 0;
        }

        k = is_dir(def, /* follow= */ true);
        𐡄𐡍 (k == 0)
                k = -ENOTDIR;
        𐡄𐡍 (k < 0)
                أرجع RET_GATHER(r, k);

        *ret = def;
        أرجع 0;
}

صحيح var_tmp_dir(ثابت حرف **ret) {
        أكد(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        أرجع tmp_dir_internal("/var/tmp", ret);
}

صحيح tmp_dir(ثابت حرف **ret) {
        أكد(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        أرجع tmp_dir_internal("/tmp", ret);
}

صحيح unlink_or_warn(ثابت حرف *filename) {
        أكد(filename);

        𐡄𐡍 (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                𐡄𐡍 (errno != EROFS || access(filename, F_OK) >= 0)
                        أرجع log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        أرجع 0;
}

حرف *rmdir_and_free(حرف *p) {
        PROTECT_ERRNO;

        𐡄𐡍 (!p)
                أرجع عدم;

        (فراغ) rmdir(p);
        أرجع mfree(p);
}

حرف* unlink_and_free(حرف *p) {
        PROTECT_ERRNO;

        𐡄𐡍 (!p)
                أرجع عدم;

        (فراغ) unlink(p);
        أرجع mfree(p);
}

صحيح unlinkat_deallocate(صحيح fd, ثابت حرف *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ صحيح truncate_fd = -EBADF;
        بنية stat st;
        off_t l, bs;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد(name);
        أكد((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        𐡄𐡍 (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                𐡄𐡍 (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        𐡄𐡍 (IN_SET(errno, ENOENT, EISDIR))
                                أرجع -errno;

                        𐡄𐡍 (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        𐡄𐡍 (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                أرجع -errno;

        𐡄𐡍 (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                أرجع 0;

        𐡄𐡍 (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                أرجع 0;
        }

        𐡄𐡍 (!S_ISREG(st.st_mode))
                أرجع 0;

        𐡄𐡍 (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t left = st.st_size;
                حرف buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, حجم(buffer));

                طالما (left > 0) {
                        ssize_t n;

                        n = اكتب(truncate_fd, buffer, MIN(حجم(buffer), left));
                        𐡄𐡍 (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                اكسر;
                        }

                        أكد(left >= (size_t) n);
                        left -= n;
                }

                /* Let's refresh metadata */
                𐡄𐡍 (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        أرجع 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        𐡄𐡍 (st.st_blocks == 0 || st.st_nlink > 0)
                أرجع 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        𐡄𐡍 (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                أرجع 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        𐡄𐡍 (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                أرجع 0;
        }

        أرجع 0;
}

صحيح open_parent_at(صحيح dir_fd, ثابت حرف *path, صحيح flags, mode_t mode) {
        _cleanup_free_ حرف *parent = عدم;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        r = path_extract_directory(path, &parent);
        𐡄𐡍 (r == -EDESTADDRREQ) {
                parent = strdup(".");
                𐡄𐡍 (!parent)
                        أرجع -ENOMEM;
        } وإلا 𐡄𐡍 (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                𐡄𐡍 (!parent)
                        أرجع -ENOMEM;
        } وإلا 𐡄𐡍 (r < 0)
                أرجع r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        𐡄𐡍 (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        أرجع RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

صحيح conservative_renameat(
                صحيح olddirfd, ثابت حرف *oldpath,
                صحيح newdirfd, ثابت حرف *newpath) {

        _cleanup_close_ صحيح old_fd = -EBADF, new_fd = -EBADF;
        بنية stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        𐡄𐡍 (old_fd < 0)
                اذهب do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        𐡄𐡍 (new_fd < 0)
                اذهب do_rename;

        𐡄𐡍 (fstat(old_fd, &old_stat) < 0)
                اذهب do_rename;

        𐡄𐡍 (!S_ISREG(old_stat.st_mode))
                اذهب do_rename;

        𐡄𐡍 (fstat(new_fd, &new_stat) < 0)
                اذهب do_rename;

        𐡄𐡍 (stat_inode_same(&new_stat, &old_stat))
                اذهب is_same;

        𐡄𐡍 (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                اذهب do_rename;

        لكل (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[حجم(buf1)];
                ssize_t l1, l2;

                l1 = اقرأ(old_fd, buf1, حجم(buf1));
                𐡄𐡍 (l1 < 0)
                        اذهب do_rename;

                𐡄𐡍 (l1 == حجم(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = اقرأ(new_fd, buf2, l1);
                وإلا {
                        أكد((size_t) l1 < حجم(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        أكد((size_t) (l1 + 1) <= حجم(buf2));
                        l2 = اقرأ(new_fd, buf2, l1 + 1);
                }
                𐡄𐡍 (l2 != l1)
                        اذهب do_rename;

                𐡄𐡍 (memcmp(buf1, buf2, l1) != 0)
                        اذهب do_rename;

                𐡄𐡍 ((size_t) l1 < حجم(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        اكسر;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        𐡄𐡍 (unlinkat(olddirfd, oldpath, 0) < 0)
                اذهب do_rename;

        أرجع 0;

do_rename:
        𐡄𐡍 (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                أرجع -errno;

        أرجع 1;
}

صحيح posix_fallocate_loop(صحيح fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        صحيح r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        𐡄𐡍 (r != EINTR)
                أرجع -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ثابت RateLimit) { 10 * USEC_PER_SEC, 16 };
        طالما (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                𐡄𐡍 (r != EINTR)
                        أرجع -r;
        }

        أرجع -EINTR;
}

صحيح parse_cifs_service(
                ثابت حرف *s,
                حرف **ret_host,
                حرف **ret_service,
                حرف **ret_path) {

        _cleanup_free_ حرف *h = عدم, *ss = عدم, *x = عدم;
        ثابت حرف *p, *e, *d;
        حرف delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        𐡄𐡍 (!s)
                أرجع -EINVAL;

        p = startswith(s, "//");
        𐡄𐡍 (!p) {
                p = startswith(s, "\\\\");
                𐡄𐡍 (!p)
                        أرجع -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        𐡄𐡍 (!e)
                أرجع -EINVAL;

        h = strndup(p, e - p);
        𐡄𐡍 (!h)
                أرجع -ENOMEM;

        𐡄𐡍 (!hostname_is_valid(h, 0))
                أرجع -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        𐡄𐡍 (!ss)
                أرجع -ENOMEM;

        𐡄𐡍 (!filename_is_valid(ss))
                أرجع -EINVAL;

        𐡄𐡍 (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                𐡄𐡍 (!x)
                        أرجع -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                لكل (حرف *i = x; *i; i++)
                        𐡄𐡍 (*i == delimiter)
                                *i = '/';

                𐡄𐡍 (!path_is_valid(x))
                        أرجع -EINVAL;

                path_simplify(x);
                𐡄𐡍 (!path_is_normalized(x))
                        أرجع -EINVAL;
        }

        𐡄𐡍 (ret_host)
                *ret_host = TAKE_PTR(h);
        𐡄𐡍 (ret_service)
                *ret_service = TAKE_PTR(ss);
        𐡄𐡍 (ret_path)
                *ret_path = TAKE_PTR(x);

        أرجع 0;
}

صحيح open_mkdir_at_full_label(صحيح dirfd, ثابت حرف *path, صحيح flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ صحيح fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ حرف *fname = عدم, *parent = عدم;
        صحيح r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        𐡄𐡍 (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                أرجع -EINVAL;
        𐡄𐡍 ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                أرجع -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        𐡄𐡍 (r < 0) {
                𐡄𐡍 (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        أرجع r;
        } وإلا {
                r = path_extract_filename(path, &fname);
                𐡄𐡍 (r < 0)
                        أرجع r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                𐡄𐡍 (parent_fd < 0)
                        أرجع -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        𐡄𐡍 (IN_SET(fd, -ELOOP, -ENOTDIR))
                أرجع -EEXIST;
        𐡄𐡍 (fd < 0)
                أرجع fd;

        أرجع TAKE_FD(fd);
}

صحيح openat_report_new(صحيح dirfd, ثابت حرف *pathname, صحيح flags, mode_t mode, bool *ret_newly_created) {
        صحيح fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        𐡄𐡍 (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                𐡄𐡍 (fd < 0)
                        أرجع -errno;

                𐡄𐡍 (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                أرجع fd;
        }

        لكل (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                𐡄𐡍 (fd >= 0) {
                        𐡄𐡍 (ret_newly_created)
                                *ret_newly_created = false;
                        أرجع fd;
                }
                𐡄𐡍 (errno != ENOENT)
                        أرجع -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                𐡄𐡍 (fd >= 0) {
                        𐡄𐡍 (ret_newly_created)
                                *ret_newly_created = true;
                        أرجع fd;
                }
                𐡄𐡍 (errno != EEXIST)
                        أرجع -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                𐡄𐡍 (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        أرجع -EEXIST;
        }
}

ساكن صحيح openat_with_automount(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ساكن bool can_open_tree = true;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);
        أكد(FLAGS_SET(open_flags, O_PATH));

        𐡄𐡍 (can_open_tree) {
                صحيح fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                𐡄𐡍 (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        𐡄𐡍 (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                صحيح q = fd_verify_directory(fd);
                                𐡄𐡍 (q < 0) {
                                        safe_close(fd);
                                        أرجع q;
                                }
                        }

                        أرجع fd;
                }
                𐡄𐡍 (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        أرجع fd;

                can_open_tree = false;
        }

        أرجع RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

صحيح xopenat_full_label(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ صحيح fd = -EBADF;
        bool made_dir = false, made_file = false;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        أكد(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        أكد(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        أكد(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        أكد(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        أكد(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        أكد((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        أكد(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        𐡄𐡍 (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        𐡄𐡍 (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                𐡄𐡍 (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } وإلا 𐡄𐡍 (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        أرجع -EINVAL;
        }

        𐡄𐡍 (isempty(path)) {
                أكد(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }

                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        𐡄𐡍 (r < 0)
                                أرجع r;
                }

                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        𐡄𐡍 (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                أرجع TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                أرجع fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ صحيح _dir_fd = -EBADF;
        𐡄𐡍 (dir_fd == XAT_FDROOT) {
                𐡄𐡍 (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                وإلا {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        𐡄𐡍 (_dir_fd < 0)
                                أرجع -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        𐡄𐡍 (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                𐡄𐡍 (r < 0)
                        أرجع r;

                call_label_ops_post = true;
        }

        𐡄𐡍 (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                وإلا
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                𐡄𐡍 (r == -EEXIST) {
                        𐡄𐡍 (FLAGS_SET(open_flags, O_EXCL))
                                أرجع -EEXIST;
                } وإلا 𐡄𐡍 (r < 0)
                        أرجع r;
                وإلا
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        𐡄𐡍 (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                𐡄𐡍 (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        𐡄𐡍 (fd < 0) {
                                r = fd;
                                اذهب error;
                        }

                        r = fd_verify_regular(fd);
                        𐡄𐡍 (r < 0)
                                اذهب error;

                } وإلا 𐡄𐡍 (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        𐡄𐡍 (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                𐡄𐡍 (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                وإلا 𐡄𐡍 (fd < 0) {
                                        r = fd;
                                        اذهب error;
                                }
                        }

                        𐡄𐡍 (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                𐡄𐡍 (fd < 0) {
                                        r = -errno;
                                        اذهب error;
                                }
                        }

                        made_file = true;
                } وإلا {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ صحيح inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        𐡄𐡍 (inode_fd < 0) {
                                𐡄𐡍 (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        اذهب error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        𐡄𐡍 (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        وإلا 𐡄𐡍 (fd < 0) {
                                                r = fd;
                                                اذهب error;
                                        }
                                }

                                𐡄𐡍 (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        𐡄𐡍 (fd < 0) {
                                                r = -errno;
                                                اذهب error;
                                        }
                                }

                                made_file = true;
                        } وإلا {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                𐡄𐡍 (r < 0)
                                        اذهب error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        𐡄𐡍 (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        وإلا 𐡄𐡍 (fd < 0) {
                                                r = fd;
                                                اذهب error;
                                        }
                                }

                                𐡄𐡍 (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        𐡄𐡍 (fd < 0) {
                                                r = fd;
                                                اذهب error;
                                        }
                                }
                        }
                }
        } وإلا 𐡄𐡍 (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                𐡄𐡍 (fd < 0) {
                        r = fd;
                        اذهب error;
                }
        } وإلا {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                𐡄𐡍 (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        𐡄𐡍 (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        وإلا 𐡄𐡍 (fd < 0) {
                                r = fd;
                                اذهب error;
                        }
                }

                𐡄𐡍 (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        𐡄𐡍 (fd < 0) {
                                r = fd;
                                اذهب error;
                        }
                }
        }

        𐡄𐡍 (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                𐡄𐡍 (r < 0)
                        اذهب error;
        }

        𐡄𐡍 (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ عدم, made_file || made_dir, label_context);
                𐡄𐡍 (r < 0)
                        اذهب error;
        }

        𐡄𐡍 (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                𐡄𐡍 (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        اذهب error;
        }

        أرجع TAKE_FD(fd);

error:
        𐡄𐡍 (call_label_ops_post)
                (فراغ) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? عدم : path, made_dir || made_file, label_context);

        𐡄𐡍 (made_dir || made_file)
                (فراغ) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        أرجع r;
}

صحيح xopenat_lock_full_label(
                صحيح dir_fd,
                ثابت حرف *path,
                صحيح open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                صحيح operation,
                LabelContext *label_context) {

        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        𐡄𐡍 (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                أرجع -EBADF;

        لكل (;;) {
                بنية stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                𐡄𐡍 (fd < 0)
                        أرجع fd;

                r = lock_generic(fd, locktype, operation);
                𐡄𐡍 (r < 0)
                        أرجع r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                𐡄𐡍 (fstat(fd, &st) < 0)
                        أرجع -errno;
                𐡄𐡍 (st.st_nlink > 0)
                        اكسر;

                fd = safe_close(fd);
        }

        أرجع TAKE_FD(fd);
}

صحيح link_fd(صحيح fd, صحيح newdirfd, ثابت حرف *newpath) {
        صحيح r;

        أكد(fd >= 0);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        𐡄𐡍 (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                𐡄𐡍 (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        أرجع -ENOSYS;
        }

        أرجع r;
}

صحيح linkat_replace(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath) {
        _cleanup_close_ صحيح old_fd = -EBADF;
        صحيح r;

        أكد(olddirfd >= 0 || olddirfd == AT_FDCWD);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        𐡄𐡍 (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                أرجع -EISDIR;

        𐡄𐡍 (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                أرجع -EISDIR;

        𐡄𐡍 (path_implies_directory(newpath))
                أرجع -EISDIR;

        /* First, try to link this directly */
        𐡄𐡍 (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        وإلا
                r = link_fd(olddirfd, newdirfd, newpath);
        𐡄𐡍 (r >= 0)
                أرجع 0;
        𐡄𐡍 (r != -EEXIST)
                أرجع r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        𐡄𐡍 (old_fd < 0)
                أرجع old_fd;

        بنية stat old_st;
        𐡄𐡍 (fstat(old_fd, &old_st) < 0)
                أرجع -errno;

        𐡄𐡍 (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                أرجع -EISDIR;

        بنية stat new_st;
        𐡄𐡍 (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                أرجع -errno;

        𐡄𐡍 (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                أرجع -EEXIST;

        𐡄𐡍 (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                أرجع 0;

        _cleanup_free_ حرف *tmp_path = عدم;
        r = tempfn_random(newpath, /* extra= */ عدم, &tmp_path);
        𐡄𐡍 (r < 0)
                أرجع r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        𐡄𐡍 (r < 0) {
                𐡄𐡍 (!ERRNO_IS_PRIVILEGE(r))
                        أرجع r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                𐡄𐡍 (r < 0)
                        أرجع r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        𐡄𐡍 (r < 0) {
                (فراغ) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                أرجع r;
        }

        أرجع 0;
}
