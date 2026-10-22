/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/falloc.h>
#شامل <stdlib.h>
#شامل <sys/file.h>
#شامل <sys/mount.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "btrfs-util.h"
#شامل "chattr-util.h"
#شامل "dirent-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "hostname-util.h"
#شامل "label-util.h"
#شامل "lock-util.h"
#شامل "log.h"
#شامل "mkdir.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "random-util.h"
#شامل "ratelimit.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "time-util.h"
#شامل "tmpfile-util.h"
#شامل "umask-util.h"

صحیح rmdir_parents(ثابت حرف *راستہ, ثابت حرف *stop) {
        حرف *p;
        صحیح r;

        أكد(راستہ);
        أكد(stop);

        اگر (!راستہ_is_محفوظ(راستہ))
                واپس -EINVAL;

        اگر (!راستہ_is_محفوظ(stop))
                واپس -EINVAL;

        p = strdupa_محفوظ(راستہ);

        برائے (;;) {
                حرف *slash = تہی;

                /* skip the last component. */
                r = راستہ_ڈھونڈو_آخری_component(p, /* accept_dot_dot= */ false, (ثابت حرف **) &slash, تہی);
                اگر (r <= 0)
                        واپس r;
                اگر (slash == p)
                        واپس 0;

                أكد(*slash == '/');
                *slash = '\0';

                اگر (راستہ_startswith_بھرا(stop, p, راستہ_STARTSWITH_REFUSE_DOT_DOT))
                        واپس 0;

                اگر (rmdir(p) < 0 && errno != ENOENT)
                        واپس -errno;
        }
}

صحیح rename_noreplace(صحیح olddirfd, ثابت حرف *oldpath, صحیح newdirfd, ثابت حرف *newpath) {
        صحیح r;

        أكد(olddirfd >= 0 || olddirfd == AT_FDCWD);
        أكد(oldpath);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(newpath);

        /* Try the ideal approach first */
        اگر (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                واپس 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                واپس -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        اگر (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                اگر (r < 0) {
                        (خالی) unlinkat(newdirfd, newpath, 0);
                        واپس r;
                }

                واپس 0;
        }

        اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && !اندر_سیٹ(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                واپس -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        اگر (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                واپس -EEXIST;
        اگر (errno != ENOENT)
                واپس -errno;

        واپس RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

صحیح readlinkat_malloc(صحیح fd, ثابت حرف *p, حرف **ret) {
        size_t l = PATH_MAX;

        أكد(fd >= 0 || fd == AT_FDCWD);

        اگر (fd < 0 && isempty(p))
                واپس -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        برائے (;;) {
                _cleanup_free_ حرف *c = تہی;
                ssize_t n;

                c = new(حرف, l+1);
                اگر (!c)
                        واپس -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                اگر (n < 0)
                        واپس -errno;

                اگر ((size_t) n < l) {
                        c[n] = 0;

                        اگر (ret)
                                *ret = TAKE_PTR(c);

                        واپس 0;
                }

                اگر (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        واپس -EFBIG;

                l *= 2;
        }
}

صحیح readlink_قدر(ثابت حرف *p, حرف **ret) {
        _cleanup_free_ حرف *link = تہی, *name = تہی;
        صحیح r;

        أكد(p);
        أكد(ret);

        r = readlink_malloc(p, &link);
        اگر (r < 0)
                واپس r;

        r = راستہ_نکالو_filename(link, &name);
        اگر (r < 0)
                واپس r;
        اگر (r == O_DIRECTORY)
                واپس -EINVAL;

        *ret = TAKE_PTR(name);
        واپس 0;
}

صحیح readlink_and_make_absolute(ثابت حرف *p, حرف **ret) {
        _cleanup_free_ حرف *ہدف = تہی;
        صحیح r;

        أكد(p);
        أكد(ret);

        r = readlink_malloc(p, &ہدف);
        اگر (r < 0)
                واپس r;

        واپس file_اندر_same_dir(p, ہدف, ret);
}

صحیح chmod_and_chown_at(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        اگر (راستہ) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, راستہ, O_راستہ|O_CLOEXEC|O_NOFOLLOW);
                اگر (fd < 0)
                        واپس -errno;
                dir_fd = fd;

        } ورنہ اگر (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_راستہ|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                اگر (fd < 0)
                        واپس -errno;
                dir_fd = fd;
        }

        واپس fchmod_and_chown(dir_fd, منوال, uid, gid);
}

صحیح fchmod_and_chown_with_fallback(صحیح fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ساخت stat st;
        صحیح r;

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

        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((منوال != منوال_INVALID && ((st.st_mode ^ منوال) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        اگر (منوال == منوال_INVALID)
                منوال = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        ورنہ اگر ((منوال & S_IFMT) != 0 && ((منوال ^ st.st_mode) & S_IFMT) != 0)
                واپس -EINVAL; /* insist on the right file type if it was specified */

        اگر (do_chown && do_chmod) {
                mode_t اقل = st.st_mode & منوال; /* the subset of the old and the new mask */

                اگر (((اقل ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, اقل & 07777);
                        اگر (r < 0) {
                                اگر (!راستہ || r != -ENOSYS)
                                        واپس r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                اگر (chmod(راستہ, اقل & 07777) < 0)
                                        واپس -errno;
                        }
                }
        }

        اگر (do_chown)
                اگر (fchownat(fd, "", uid, gid, AT_خالی_خانے_راستہ) < 0)
                        واپس -errno;

        اگر (do_chmod) {
                r = fchmod_opath(fd, منوال & 07777);
                اگر (r < 0) {
                        اگر (!راستہ || r != -ENOSYS)
                                واپس r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        اگر (chmod(راستہ, منوال & 07777) < 0)
                                واپس -errno;
                }
        }

        واپس do_chown || do_chmod;
}

صحیح fchmod_umask(صحیح fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        واپس RET_NERRNO(fchmod(fd, m & (~u)));
}

صحیح fchmod_opath(صحیح fd, mode_t m) {
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

        اگر (fchmodat(fd, "", m, AT_خالی_خانے_راستہ) >= 0)
                واپس 0;
        اگر (errno == EINVAL && fchmodat2(fd, "", m, AT_خالی_خانے_راستہ) >= 0) /* glibc too old? */
                واپس 0;
        اگر (!اندر_سیٹ(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                واپس -errno;

        اگر (chmod(FORMAT_PROC_FD_راستہ(fd), m) < 0) {
                اگر (errno != ENOENT)
                        واپس -errno;

                واپس proc_fd_enoent_errno();
        }

        واپس 0;
}

صحیح futimens_opath(صحیح fd, ثابت ساخت timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        أكد(fd >= 0);

        واپس RET_NERRNO(utimensat(fd, "", ts, AT_خالی_خانے_راستہ));
}

صحیح stat_warn_permissions(ثابت حرف *راستہ, ثابت ساخت stat *st) {
        أكد(راستہ);
        أكد(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        اگر (!S_ISREG(st->st_mode))
                واپس 0;

        اگر (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", راستہ);

        اگر (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", راستہ);

        اگر (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", راستہ);

        واپس 0;
}

صحیح fd_warn_permissions(ثابت حرف *راستہ, صحیح fd) {
        ساخت stat st;

        أكد(راستہ);
        أكد(fd >= 0);

        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        واپس stat_warn_permissions(راستہ, &st);
}

صحیح access_nofollow(ثابت حرف *راستہ, صحیح منوال) {
        واپس RET_NERRNO(faccessat(AT_FDCWD, راستہ, منوال, AT_SYMLINK_NOFOLLOW));
}

صحیح access_fd(صحیح fd, صحیح منوال) {
        /* Like access() but operates on an already open fd */

        اگر (fd == AT_FDCWD)
                واپس RET_NERRNO(access(".", منوال));
        اگر (fd == XAT_FDROOT)
                واپس RET_NERRNO(access("/", منوال));

        أكد(fd >= 0);

        واپس RET_NERRNO(faccessat(fd, "", منوال, AT_خالی_خانے_راستہ));
}

صحیح touch_fd(صحیح fd, usec_t stamp) {
        أكد(fd >= 0);

        اگر (stamp == USEC_INFINITY)
                واپس futimens_opath(fd, /* ts= */ تہی);

        ساخت timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        واپس futimens_opath(fd, ts);
}

صحیح touch_file(ثابت حرف *راستہ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t منوال) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح ret;

        أكد(راستہ);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        اگر (parents)
                (خالی) mkdir_parents(راستہ, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(راستہ, O_راستہ|O_CLOEXEC|O_NOFOLLOW);
        اگر (fd < 0) {
                اگر (errno != ENOENT)
                        واپس -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(راستہ, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, اندر_سیٹ(منوال, 0, منوال_INVALID) ? 0644 : منوال);
                اگر (fd < 0)
                        واپس -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, منوال, uid, gid);

        واپس RET_GATHER(ret, touch_fd(fd, stamp));
}

صحیح touch(ثابت حرف *راستہ) {
        واپس touch_file(راستہ, false, USEC_INFINITY, UID_INVALID, GID_INVALID, منوال_INVALID);
}

صحیح symlinkat_idempotent(ثابت حرف *ہدف, صحیح atfd, ثابت حرف *linkpath, bool make_relative) {
        _cleanup_free_ حرف *relpath = تہی;
        صحیح r;

        أكد(ہدف);
        أكد(linkpath);

        اگر (make_relative) {
                r = راستہ_make_relative_والد(linkpath, ہدف, &relpath);
                اگر (r < 0)
                        واپس r;

                ہدف = relpath;
        }

        اگر (symlinkat(ہدف, atfd, linkpath) < 0) {
                _cleanup_free_ حرف *p = تہی;

                اگر (errno != EEXIST)
                        واپس -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                اگر (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        واپس -EEXIST;
                اگر (r < 0) /* Any other error? In that case propagate it as is */
                        واپس r;

                اگر (!streq(p, ہدف)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        واپس -EEXIST;
        }

        واپس 0;
}

صحیح symlinkat_atomic_بھرا_label(ثابت حرف *ہدف, صحیح atfd, ثابت حرف *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        صحیح r;

        أكد(ہدف);
        أكد(linkpath);

        _cleanup_free_ حرف *relpath = تہی;
        اگر (FLAGS_سیٹ(flags, SYMLINK_MAKE_RELATIVE)) {
                r = راستہ_make_relative_والد(linkpath, ہدف, &relpath);
                اگر (r < 0)
                        واپس r;

                ہدف = relpath;
        }

        _cleanup_free_ حرف *t = تہی;
        r = tempfn_random(linkpath, تہی, &t);
        اگر (r < 0)
                واپس r;

        bool call_label_ops_post = false;
        اگر (FLAGS_سیٹ(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                اگر (r < 0)
                        واپس r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(ہدف, atfd, t));
        اگر (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        اگر (r < 0)
                واپس r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        اگر (r < 0) {
                (خالی) unlinkat(atfd, t, 0);
                واپس r;
        }

        واپس 0;
}

صحیح mknodat_atomic(صحیح atfd, ثابت حرف *راستہ, mode_t منوال, dev_t dev) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        أكد(راستہ);

        r = tempfn_random(راستہ, تہی, &t);
        اگر (r < 0)
                واپس r;

        اگر (mknodat(atfd, t, منوال, dev) < 0)
                واپس -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, راستہ));
        اگر (r < 0) {
                (خالی) unlinkat(atfd, t, 0);
                واپس r;
        }

        واپس 0;
}

صحیح mkfifoat_atomic(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال) {
        _cleanup_free_ حرف *t = تہی;
        صحیح r;

        أكد(راستہ);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(راستہ, تہی, &t);
        اگر (r < 0)
                واپس r;

        اگر (mkfifoat(dir_fd, t, منوال) < 0)
                واپس -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, راستہ));
        اگر (r < 0) {
                (خالی) unlinkat(dir_fd, t, 0);
                واپس r;
        }

        واپس 0;
}

صحیح get_files_اندر_directory(ثابت حرف *راستہ, حرف ***ret_list) {
        _cleanup_strv_free_ حرف **l = تہی;
        _cleanup_closedir_ DIR *d = تہی;
        size_t n = 0;

        أكد(راستہ);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(راستہ);
        اگر (!d)
                واپس -errno;

        FOREACH_DIRENT_ALL(de, d, واپس -errno) {
                اگر (!dirent_is_file(de))
                        جاری;

                اگر (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        اگر (!GREEDY_REALLOC(l, n + 2))
                                واپس -ENOMEM;

                        l[n] = strdup(de->d_name);
                        اگر (!l[n])
                                واپس -ENOMEM;

                        l[++n] = تہی;
                } ورنہ
                        n++;
        }

        اگر (ret_list)
                *ret_list = TAKE_PTR(l);

        واپس n;
}

ساکن صحیح getenv_tmp_dir(ثابت حرف **ret_راستہ) {
        صحیح r, ret = 0;

        أكد(ret_راستہ);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ثابت حرف *e;

                e = secure_getenv(n);
                اگر (!e)
                        جاری;
                اگر (!راستہ_is_absolute(e)) {
                        r = -ENOTDIR;
                        جاؤ اگلا_2;
                }
                اگر (!راستہ_is_normalized(e)) {
                        r = -EPERM;
                        جاؤ اگلا_2;
                }

                r = is_dir(e, true);
                اگر (r < 0)
                        جاؤ اگلا_2;
                اگر (r == 0) {
                        r = -ENOTDIR;
                        جاؤ اگلا_2;
                }

                *ret_راستہ = e;
                واپس 1;

        اگلا_2:
                /* Remember first error, to make this more debuggable */
                اگر (ret >= 0)
                        ret = r;
        }

        اگر (ret < 0)
                واپس ret;

        *ret_راستہ = تہی;
        واپس ret;
}

ساکن صحیح tmp_dir_اندرونی(ثابت حرف *def, ثابت حرف **ret) {
        ثابت حرف *e;
        صحیح r, k;

        أكد(def);
        أكد(ret);

        r = getenv_tmp_dir(&e);
        اگر (r > 0) {
                *ret = e;
                واپس 0;
        }

        k = is_dir(def, /* follow= */ true);
        اگر (k == 0)
                k = -ENOTDIR;
        اگر (k < 0)
                واپس RET_GATHER(r, k);

        *ret = def;
        واپس 0;
}

صحیح var_tmp_dir(ثابت حرف **ret) {
        أكد(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        واپس tmp_dir_اندرونی("/var/tmp", ret);
}

صحیح tmp_dir(ثابت حرف **ret) {
        أكد(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        واپس tmp_dir_اندرونی("/tmp", ret);
}

صحیح unlink_or_warn(ثابت حرف *filename) {
        أكد(filename);

        اگر (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                اگر (errno != EROFS || access(filename, F_OK) >= 0)
                        واپس log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        واپس 0;
}

حرف *rmdir_and_free(حرف *p) {
        PROTECT_ERRNO;

        اگر (!p)
                واپس تہی;

        (خالی) rmdir(p);
        واپس mfree(p);
}

حرف* unlink_and_free(حرف *p) {
        PROTECT_ERRNO;

        اگر (!p)
                واپس تہی;

        (خالی) unlink(p);
        واپس mfree(p);
}

صحیح unlinkat_deallocate(صحیح fd, ثابت حرف *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ صحیح truncate_fd = -EBADF;
        ساخت stat st;
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

        اگر (!FLAGS_سیٹ(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                اگر (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        اگر (اندر_سیٹ(errno, ENOENT, EISDIR))
                                واپس -errno;

                        اگر (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        اگر (unlinkat(fd, name, FLAGS_سیٹ(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                واپس -errno;

        اگر (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                واپس 0;

        اگر (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                واپس 0;
        }

        اگر (!S_ISREG(st.st_mode))
                واپس 0;

        اگر (FLAGS_سیٹ(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t بایاں = st.st_size;
                حرف بفر[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_بائٹیں(بفر, حجم(بفر));

                جب (بایاں > 0) {
                        ssize_t n;

                        n = اكتب(truncate_fd, بفر, MIN(حجم(بفر), بایاں));
                        اگر (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                توڑو;
                        }

                        أكد(بایاں >= (size_t) n);
                        بایاں -= n;
                }

                /* Let's refresh metadata */
                اگر (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        واپس 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        اگر (st.st_blocks == 0 || st.st_nlink > 0)
                واپس 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = باری_اوپر(st.st_size, bs); /* Round up to next block size */

        اگر (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                واپس 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        اگر (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                واپس 0;
        }

        واپس 0;
}

صحیح open_والد_at(صحیح dir_fd, ثابت حرف *راستہ, صحیح flags, mode_t منوال) {
        _cleanup_free_ حرف *والد = تہی;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(راستہ);

        r = راستہ_نکالو_directory(راستہ, &والد);
        اگر (r == -EDESTADDRREQ) {
                والد = strdup(".");
                اگر (!والد)
                        واپس -ENOMEM;
        } ورنہ اگر (r == -EADDRNOTAVAIL) {
                والد = strdup(راستہ);
                اگر (!والد)
                        واپس -ENOMEM;
        } ورنہ اگر (r < 0)
                واپس r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        اگر (!FLAGS_سیٹ(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        واپس RET_NERRNO(openat(dir_fd, والد, flags, منوال));
}

صحیح conservative_renameat(
                صحیح olddirfd, ثابت حرف *oldpath,
                صحیح newdirfd, ثابت حرف *newpath) {

        _cleanup_close_ صحیح old_fd = -EBADF, new_fd = -EBADF;
        ساخت stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        اگر (old_fd < 0)
                جاؤ do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        اگر (new_fd < 0)
                جاؤ do_rename;

        اگر (fstat(old_fd, &old_stat) < 0)
                جاؤ do_rename;

        اگر (!S_ISREG(old_stat.st_mode))
                جاؤ do_rename;

        اگر (fstat(new_fd, &new_stat) < 0)
                جاؤ do_rename;

        اگر (stat_inode_same(&new_stat, &old_stat))
                جاؤ is_same;

        اگر (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                جاؤ do_rename;

        برائے (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[حجم(buf1)];
                ssize_t l1, l2;

                l1 = اقرأ(old_fd, buf1, حجم(buf1));
                اگر (l1 < 0)
                        جاؤ do_rename;

                اگر (l1 == حجم(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = اقرأ(new_fd, buf2, l1);
                ورنہ {
                        أكد((size_t) l1 < حجم(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        أكد((size_t) (l1 + 1) <= حجم(buf2));
                        l2 = اقرأ(new_fd, buf2, l1 + 1);
                }
                اگر (l2 != l1)
                        جاؤ do_rename;

                اگر (memcmp(buf1, buf2, l1) != 0)
                        جاؤ do_rename;

                اگر ((size_t) l1 < حجم(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        توڑو;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        اگر (unlinkat(olddirfd, oldpath, 0) < 0)
                جاؤ do_rename;

        واپس 0;

do_rename:
        اگر (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                واپس -errno;

        واپس 1;
}

صحیح posix_fallocate_loop(صحیح fd, uint64_t انحراف, uint64_t سائز) {
        شرح_حد_بندی rl;
        صحیح r;

        r = posix_fallocate(fd, انحراف, سائز); /* returns positive errnos on error */
        اگر (r != EINTR)
                واپس -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ثابت شرح_حد_بندی) { 10 * USEC_فی_SEC, 16 };
        جب (ratelimit_نیچے(&rl)) {
                r = posix_fallocate(fd, انحراف, سائز);
                اگر (r != EINTR)
                        واپس -r;
        }

        واپس -EINTR;
}

صحیح parse_cifs_service(
                ثابت حرف *s,
                حرف **ret_host,
                حرف **ret_service,
                حرف **ret_راستہ) {

        _cleanup_free_ حرف *h = تہی, *ss = تہی, *x = تہی;
        ثابت حرف *p, *e, *d;
        حرف delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        اگر (!s)
                واپس -EINVAL;

        p = startswith(s, "//");
        اگر (!p) {
                p = startswith(s, "\\\\");
                اگر (!p)
                        واپس -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        اگر (!e)
                واپس -EINVAL;

        h = strndup(p, e - p);
        اگر (!h)
                واپس -ENOMEM;

        اگر (!hostname_is_valid(h, 0))
                واپس -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        اگر (!ss)
                واپس -ENOMEM;

        اگر (!filename_is_valid(ss))
                واپس -EINVAL;

        اگر (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_مقصد_STR(delimiter)));
                اگر (!x)
                        واپس -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                برائے (حرف *i = x; *i; i++)
                        اگر (*i == delimiter)
                                *i = '/';

                اگر (!راستہ_is_valid(x))
                        واپس -EINVAL;

                راستہ_simplify(x);
                اگر (!راستہ_is_normalized(x))
                        واپس -EINVAL;
        }

        اگر (ret_host)
                *ret_host = TAKE_PTR(h);
        اگر (ret_service)
                *ret_service = TAKE_PTR(ss);
        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(x);

        واپس 0;
}

صحیح open_mkdir_at_بھرا_label(صحیح dirfd, ثابت حرف *راستہ, صحیح flags, XOpenFlags xopen_flags, mode_t منوال, LabelContext *label_context) {
        _cleanup_close_ صحیح fd = -EBADF, والد_fd = -EBADF;
        _cleanup_free_ حرف *fname = تہی, *والد = تہی;
        صحیح r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        اگر (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_راستہ))
                واپس -EINVAL;
        اگر ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                واپس -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = راستہ_نکالو_directory(راستہ, &والد);
        اگر (r < 0) {
                اگر (!اندر_سیٹ(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        واپس r;
        } ورنہ {
                r = راستہ_نکالو_filename(راستہ, &fname);
                اگر (r < 0)
                        واپس r;

                والد_fd = openat(dirfd, والد, O_راستہ|O_DIRECTORY|O_CLOEXEC);
                اگر (والد_fd < 0)
                        واپس -errno;

                dirfd = والد_fd;
                راستہ = fname;
        }

        fd = xopenat_بھرا_label(dirfd, راستہ, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, منوال, label_context);
        اگر (اندر_سیٹ(fd, -ELOOP, -ENOTDIR))
                واپس -EEXIST;
        اگر (fd < 0)
                واپس fd;

        واپس TAKE_FD(fd);
}

صحیح openat_report_new(صحیح dirfd, ثابت حرف *pathname, صحیح flags, mode_t منوال, bool *ret_newly_created) {
        صحیح fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        اگر (!FLAGS_سیٹ(flags, O_CREAT) || FLAGS_سیٹ(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, منوال);
                اگر (fd < 0)
                        واپس -errno;

                اگر (ret_newly_created)
                        *ret_newly_created = FLAGS_سیٹ(flags, O_CREAT);
                واپس fd;
        }

        برائے (غیرعلامتی attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), منوال);
                اگر (fd >= 0) {
                        اگر (ret_newly_created)
                                *ret_newly_created = false;
                        واپس fd;
                }
                اگر (errno != ENOENT)
                        واپس -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, منوال);
                اگر (fd >= 0) {
                        اگر (ret_newly_created)
                                *ret_newly_created = true;
                        واپس fd;
                }
                اگر (errno != EEXIST)
                        واپس -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                اگر (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        واپس -EEXIST;
        }
}

ساکن صحیح openat_with_automount(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, mode_t منوال) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ساکن bool can_open_درخت = true;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(راستہ);
        أكد(FLAGS_سیٹ(open_flags, O_راستہ));

        اگر (can_open_درخت) {
                صحیح fd = RET_NERRNO(open_tree(dir_fd, راستہ,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_سیٹ(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                اگر (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        اگر (FLAGS_سیٹ(open_flags, O_DIRECTORY)) {
                                صحیح q = fd_verify_directory(fd);
                                اگر (q < 0) {
                                        محفوظ_close(fd);
                                        واپس q;
                                }
                        }

                        واپس fd;
                }
                اگر (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        واپس fd;

                can_open_درخت = false;
        }

        واپس RET_NERRNO(openat(dir_fd, راستہ, open_flags, منوال));
}

صحیح xopenat_بھرا_label(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, XOpenFlags xopen_flags, mode_t منوال, LabelContext *label_context) {
        _cleanup_close_ صحیح fd = -EBADF;
        bool made_dir = false, made_file = false;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        أكد(FLAGS_سیٹ(open_flags, O_DIRECTORY) + FLAGS_سیٹ(xopen_flags, XO_REGULAR) + FLAGS_سیٹ(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        أكد(!FLAGS_سیٹ(xopen_flags, XO_SOCKET) || FLAGS_سیٹ(open_flags, O_راستہ));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        أكد(!FLAGS_سیٹ(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_سیٹ(open_flags, O_راستہ) && !FLAGS_سیٹ(open_flags, O_CREAT)));
        أكد(!(FLAGS_سیٹ(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_سیٹ(xopen_flags, XO_SUBVOLUME)));
        أكد(!(FLAGS_سیٹ(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        أكد((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        أكد(!FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        اگر (منوال == منوال_INVALID)
                منوال = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        اگر (FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO)) {
                اگر (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } ورنہ اگر (open_flags & O_راستہ)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        واپس -EINVAL;
        }

        اگر (isempty(راستہ)) {
                أكد(!FLAGS_سیٹ(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                اگر (FLAGS_سیٹ(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (FLAGS_سیٹ(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        اگر (r < 0)
                                واپس r;
                }

                اگر (FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        اگر (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                واپس TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                واپس fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ صحیح _dir_fd = -EBADF;
        اگر (dir_fd == XAT_FDROOT) {
                اگر (راستہ_is_absolute(راستہ))
                        dir_fd = AT_FDCWD;
                ورنہ {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_راستہ);
                        اگر (_dir_fd < 0)
                                واپس -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        اگر (FLAGS_سیٹ(open_flags, O_CREAT) && FLAGS_سیٹ(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, راستہ, FLAGS_سیٹ(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                اگر (r < 0)
                        واپس r;

                call_label_ops_post = true;
        }

        اگر (FLAGS_سیٹ(open_flags, O_DIRECTORY|O_CREAT)) {
                اگر (FLAGS_سیٹ(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, راستہ, منوال);
                ورنہ
                        r = RET_NERRNO(mkdirat(dir_fd, راستہ, منوال));
                اگر (r == -EEXIST) {
                        اگر (FLAGS_سیٹ(open_flags, O_EXCL))
                                واپس -EEXIST;
                } ورنہ اگر (r < 0)
                        واپس r;
                ورنہ
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        اگر (FLAGS_سیٹ(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                اگر (FLAGS_سیٹ(open_flags, O_راستہ)) {
                        fd = FLAGS_سیٹ(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, راستہ, open_flags, منوال) :
                                RET_NERRNO(openat(dir_fd, راستہ, open_flags, منوال));
                        اگر (fd < 0) {
                                r = fd;
                                جاؤ error;
                        }

                        r = fd_verify_regular(fd);
                        اگر (r < 0)
                                جاؤ error;

                } ورنہ اگر (FLAGS_سیٹ(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        اگر (FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, راستہ, open_flags|O_RDWR, منوال));
                                اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                ورنہ اگر (fd < 0) {
                                        r = fd;
                                        جاؤ error;
                                }
                        }

                        اگر (fd < 0) {
                                fd = openat(dir_fd, راستہ, open_flags, منوال);
                                اگر (fd < 0) {
                                        r = -errno;
                                        جاؤ error;
                                }
                        }

                        made_file = true;
                } ورنہ {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ صحیح inode_fd = openat(dir_fd, راستہ, O_راستہ|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        اگر (inode_fd < 0) {
                                اگر (errno != ENOENT || !FLAGS_سیٹ(open_flags, O_CREAT)) {
                                        r = -errno;
                                        جاؤ error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                اگر (FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, راستہ, open_flags|O_RDWR, منوال));
                                        اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ورنہ اگر (fd < 0) {
                                                r = fd;
                                                جاؤ error;
                                        }
                                }

                                اگر (fd < 0) {
                                        fd = openat(dir_fd, راستہ, open_flags, منوال);
                                        اگر (fd < 0) {
                                                r = -errno;
                                                جاؤ error;
                                        }
                                }

                                made_file = true;
                        } ورنہ {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                اگر (r < 0)
                                        جاؤ error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                اگر (FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ورنہ اگر (fd < 0) {
                                                r = fd;
                                                جاؤ error;
                                        }
                                }

                                اگر (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        اگر (fd < 0) {
                                                r = fd;
                                                جاؤ error;
                                        }
                                }
                        }
                }
        } ورنہ اگر (FLAGS_سیٹ(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, راستہ, open_flags, منوال);
                اگر (fd < 0) {
                        r = fd;
                        جاؤ error;
                }
        } ورنہ {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                اگر (FLAGS_سیٹ(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, راستہ, O_RDWR|open_flags, منوال, &made_file);
                        اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        ورنہ اگر (fd < 0) {
                                r = fd;
                                جاؤ error;
                        }
                }

                اگر (fd < 0) {
                        fd = openat_report_new(dir_fd, راستہ, open_flags, منوال, &made_file);
                        اگر (fd < 0) {
                                r = fd;
                                جاؤ error;
                        }
                }
        }

        اگر (FLAGS_سیٹ(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                اگر (r < 0)
                        جاؤ error;
        }

        اگر (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ تہی, made_file || made_dir, label_context);
                اگر (r < 0)
                        جاؤ error;
        }

        اگر (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_سیٹ(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                اگر (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        جاؤ error;
        }

        واپس TAKE_FD(fd);

error:
        اگر (call_label_ops_post)
                (خالی) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? تہی : راستہ, made_dir || made_file, label_context);

        اگر (made_dir || made_file)
                (خالی) unlinkat(dir_fd, راستہ, made_dir ? AT_REMOVEDIR : 0);

        واپس r;
}

صحیح xopenat_تالا_بھرا_label(
                صحیح dir_fd,
                ثابت حرف *راستہ,
                صحیح open_flags,
                XOpenFlags xopen_flags,
                mode_t منوال,
                تالا_Type locktype,
                صحیح operation,
                LabelContext *label_context) {

        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(اندر_سیٹ(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        اگر (FLAGS_سیٹ(open_flags, O_DIRECTORY) && !اندر_سیٹ(locktype, تالا_BSD, تالا_NONE))
                واپس -EBADF;

        برائے (;;) {
                ساخت stat st;

                fd = xopenat_بھرا_label(dir_fd, راستہ, open_flags, xopen_flags, منوال, label_context);
                اگر (fd < 0)
                        واپس fd;

                r = تالا_generic(fd, locktype, operation);
                اگر (r < 0)
                        واپس r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                اگر (fstat(fd, &st) < 0)
                        واپس -errno;
                اگر (st.st_nlink > 0)
                        توڑو;

                fd = محفوظ_close(fd);
        }

        واپس TAKE_FD(fd);
}

صحیح link_fd(صحیح fd, صحیح newdirfd, ثابت حرف *newpath) {
        صحیح r;

        أكد(fd >= 0);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_خالی_خانے_راستہ));
        اگر (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_راستہ(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                اگر (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        واپس -ENOSYS;
        }

        واپس r;
}

صحیح linkat_replace(صحیح olddirfd, ثابت حرف *oldpath, صحیح newdirfd, ثابت حرف *newpath) {
        _cleanup_close_ صحیح old_fd = -EBADF;
        صحیح r;

        أكد(olddirfd >= 0 || olddirfd == AT_FDCWD);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        اگر (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                واپس -EISDIR;

        اگر (راستہ_مستلزم_directory(oldpath)) /* Refuse these definite directories early */
                واپس -EISDIR;

        اگر (راستہ_مستلزم_directory(newpath))
                واپس -EISDIR;

        /* First, try to link this directly */
        اگر (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        ورنہ
                r = link_fd(olddirfd, newdirfd, newpath);
        اگر (r >= 0)
                واپس 0;
        اگر (r != -EEXIST)
                واپس r;

        old_fd = xopenat(olddirfd, oldpath, O_راستہ|O_CLOEXEC);
        اگر (old_fd < 0)
                واپس old_fd;

        ساخت stat old_st;
        اگر (fstat(old_fd, &old_st) < 0)
                واپس -errno;

        اگر (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                واپس -EISDIR;

        ساخت stat new_st;
        اگر (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                واپس -errno;

        اگر (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                واپس -EEXIST;

        اگر (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                واپس 0;

        _cleanup_free_ حرف *tmp_راستہ = تہی;
        r = tempfn_random(newpath, /* extra= */ تہی, &tmp_راستہ);
        اگر (r < 0)
                واپس r;

        r = link_fd(old_fd, newdirfd, tmp_راستہ);
        اگر (r < 0) {
                اگر (!ERRNO_IS_PRIVILEGE(r))
                        واپس r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_راستہ, 0));
                اگر (r < 0)
                        واپس r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_راستہ, newdirfd, newpath));
        اگر (r < 0) {
                (خالی) unlinkat(newdirfd, tmp_راستہ, /* flags= */ 0);
                واپس r;
        }

        واپس 0;
}
