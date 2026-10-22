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

صحیح rmdir_parents(ثابت نویسه *path, ثابت نویسه *stop) {
        نویسه *p;
        صحیح r;

        أكد(path);
        أكد(stop);

        اگر (!path_is_safe(path))
                بازگشت -EINVAL;

        اگر (!path_is_safe(stop))
                بازگشت -EINVAL;

        p = strdupa_safe(path);

        برای (;;) {
                نویسه *slash = پوچ;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (ثابت نویسه **) &slash, پوچ);
                اگر (r <= 0)
                        بازگشت r;
                اگر (slash == p)
                        بازگشت 0;

                أكد(*slash == '/');
                *slash = '\0';

                اگر (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        بازگشت 0;

                اگر (rmdir(p) < 0 && errno != ENOENT)
                        بازگشت -errno;
        }
}

صحیح rename_noreplace(صحیح olddirfd, ثابت نویسه *oldpath, صحیح newdirfd, ثابت نویسه *newpath) {
        صحیح r;

        أكد(olddirfd >= 0 || olddirfd == AT_FDCWD);
        أكد(oldpath);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(newpath);

        /* Try the ideal approach first */
        اگر (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                بازگشت 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                بازگشت -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        اگر (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                اگر (r < 0) {
                        (تهی) unlinkat(newdirfd, newpath, 0);
                        بازگشت r;
                }

                بازگشت 0;
        }

        اگر (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                بازگشت -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        اگر (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                بازگشت -EEXIST;
        اگر (errno != ENOENT)
                بازگشت -errno;

        بازگشت RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

صحیح readlinkat_malloc(صحیح fd, ثابت نویسه *p, نویسه **ret) {
        size_t l = PATH_MAX;

        أكد(fd >= 0 || fd == AT_FDCWD);

        اگر (fd < 0 && isempty(p))
                بازگشت -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        برای (;;) {
                _cleanup_free_ نویسه *c = پوچ;
                ssize_t n;

                c = new(نویسه, l+1);
                اگر (!c)
                        بازگشت -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                اگر (n < 0)
                        بازگشت -errno;

                اگر ((size_t) n < l) {
                        c[n] = 0;

                        اگر (ret)
                                *ret = TAKE_PTR(c);

                        بازگشت 0;
                }

                اگر (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        بازگشت -EFBIG;

                l *= 2;
        }
}

صحیح readlink_مقدار(ثابت نویسه *p, نویسه **ret) {
        _cleanup_free_ نویسه *link = پوچ, *name = پوچ;
        صحیح r;

        أكد(p);
        أكد(ret);

        r = readlink_malloc(p, &link);
        اگر (r < 0)
                بازگشت r;

        r = path_extract_filename(link, &name);
        اگر (r < 0)
                بازگشت r;
        اگر (r == O_DIRECTORY)
                بازگشت -EINVAL;

        *ret = TAKE_PTR(name);
        بازگشت 0;
}

صحیح readlink_and_make_absolute(ثابت نویسه *p, نویسه **ret) {
        _cleanup_free_ نویسه *target = پوچ;
        صحیح r;

        أكد(p);
        أكد(ret);

        r = readlink_malloc(p, &target);
        اگر (r < 0)
                بازگشت r;

        بازگشت file_in_same_dir(p, target, ret);
}

صحیح chmod_and_chown_at(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        اگر (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                اگر (fd < 0)
                        بازگشت -errno;
                dir_fd = fd;

        } وگرنه اگر (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                اگر (fd < 0)
                        بازگشت -errno;
                dir_fd = fd;
        }

        بازگشت fchmod_and_chown(dir_fd, mode, uid, gid);
}

صحیح fchmod_and_chown_with_fallback(صحیح fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ساختار stat st;
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
                بازگشت -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        اگر (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        وگرنه اگر ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                بازگشت -EINVAL; /* insist on the right file type if it was specified */

        اگر (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                اگر (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        اگر (r < 0) {
                                اگر (!path || r != -ENOSYS)
                                        بازگشت r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                اگر (chmod(path, minimal & 07777) < 0)
                                        بازگشت -errno;
                        }
                }
        }

        اگر (do_chown)
                اگر (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        بازگشت -errno;

        اگر (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                اگر (r < 0) {
                        اگر (!path || r != -ENOSYS)
                                بازگشت r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        اگر (chmod(path, mode & 07777) < 0)
                                بازگشت -errno;
                }
        }

        بازگشت do_chown || do_chmod;
}

صحیح fchmod_umask(صحیح fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        بازگشت RET_NERRNO(fchmod(fd, m & (~u)));
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

        اگر (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                بازگشت 0;
        اگر (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                بازگشت 0;
        اگر (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                بازگشت -errno;

        اگر (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                اگر (errno != ENOENT)
                        بازگشت -errno;

                بازگشت proc_fd_enoent_errno();
        }

        بازگشت 0;
}

صحیح futimens_opath(صحیح fd, ثابت ساختار timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        أكد(fd >= 0);

        بازگشت RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

صحیح stat_warn_permissions(ثابت نویسه *path, ثابت ساختار stat *st) {
        أكد(path);
        أكد(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        اگر (!S_ISREG(st->st_mode))
                بازگشت 0;

        اگر (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        اگر (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        اگر (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        بازگشت 0;
}

صحیح fd_warn_permissions(ثابت نویسه *path, صحیح fd) {
        ساختار stat st;

        أكد(path);
        أكد(fd >= 0);

        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        بازگشت stat_warn_permissions(path, &st);
}

صحیح access_nofollow(ثابت نویسه *path, صحیح mode) {
        بازگشت RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

صحیح access_fd(صحیح fd, صحیح mode) {
        /* Like access() but operates on an already open fd */

        اگر (fd == AT_FDCWD)
                بازگشت RET_NERRNO(access(".", mode));
        اگر (fd == XAT_FDROOT)
                بازگشت RET_NERRNO(access("/", mode));

        أكد(fd >= 0);

        بازگشت RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

صحیح touch_fd(صحیح fd, usec_t stamp) {
        أكد(fd >= 0);

        اگر (stamp == USEC_INFINITY)
                بازگشت futimens_opath(fd, /* ts= */ پوچ);

        ساختار timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        بازگشت futimens_opath(fd, ts);
}

صحیح touch_file(ثابت نویسه *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح ret;

        أكد(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        اگر (parents)
                (تهی) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        اگر (fd < 0) {
                اگر (errno != ENOENT)
                        بازگشت -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                اگر (fd < 0)
                        بازگشت -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        بازگشت RET_GATHER(ret, touch_fd(fd, stamp));
}

صحیح touch(ثابت نویسه *path) {
        بازگشت touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

صحیح symlinkat_idempotent(ثابت نویسه *target, صحیح atfd, ثابت نویسه *linkpath, bool make_relative) {
        _cleanup_free_ نویسه *relpath = پوچ;
        صحیح r;

        أكد(target);
        أكد(linkpath);

        اگر (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                اگر (r < 0)
                        بازگشت r;

                target = relpath;
        }

        اگر (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ نویسه *p = پوچ;

                اگر (errno != EEXIST)
                        بازگشت -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                اگر (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        بازگشت -EEXIST;
                اگر (r < 0) /* Any other error? In that case propagate it as is */
                        بازگشت r;

                اگر (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        بازگشت -EEXIST;
        }

        بازگشت 0;
}

صحیح symlinkat_atomic_full_label(ثابت نویسه *target, صحیح atfd, ثابت نویسه *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        صحیح r;

        أكد(target);
        أكد(linkpath);

        _cleanup_free_ نویسه *relpath = پوچ;
        اگر (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                اگر (r < 0)
                        بازگشت r;

                target = relpath;
        }

        _cleanup_free_ نویسه *t = پوچ;
        r = tempfn_random(linkpath, پوچ, &t);
        اگر (r < 0)
                بازگشت r;

        bool call_label_ops_post = false;
        اگر (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                اگر (r < 0)
                        بازگشت r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        اگر (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        اگر (r < 0)
                بازگشت r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        اگر (r < 0) {
                (تهی) unlinkat(atfd, t, 0);
                بازگشت r;
        }

        بازگشت 0;
}

صحیح mknodat_atomic(صحیح atfd, ثابت نویسه *path, mode_t mode, dev_t dev) {
        _cleanup_free_ نویسه *t = پوچ;
        صحیح r;

        أكد(path);

        r = tempfn_random(path, پوچ, &t);
        اگر (r < 0)
                بازگشت r;

        اگر (mknodat(atfd, t, mode, dev) < 0)
                بازگشت -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        اگر (r < 0) {
                (تهی) unlinkat(atfd, t, 0);
                بازگشت r;
        }

        بازگشت 0;
}

صحیح mkfifoat_atomic(صحیح dir_fd, ثابت نویسه *path, mode_t mode) {
        _cleanup_free_ نویسه *t = پوچ;
        صحیح r;

        أكد(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, پوچ, &t);
        اگر (r < 0)
                بازگشت r;

        اگر (mkfifoat(dir_fd, t, mode) < 0)
                بازگشت -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        اگر (r < 0) {
                (تهی) unlinkat(dir_fd, t, 0);
                بازگشت r;
        }

        بازگشت 0;
}

صحیح get_files_in_directory(ثابت نویسه *path, نویسه ***ret_list) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        _cleanup_closedir_ DIR *d = پوچ;
        size_t n = 0;

        أكد(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        اگر (!d)
                بازگشت -errno;

        FOREACH_DIRENT_ALL(de, d, بازگشت -errno) {
                اگر (!dirent_is_file(de))
                        ادامه;

                اگر (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        اگر (!GREEDY_REALLOC(l, n + 2))
                                بازگشت -ENOMEM;

                        l[n] = strdup(de->d_name);
                        اگر (!l[n])
                                بازگشت -ENOMEM;

                        l[++n] = پوچ;
                } وگرنه
                        n++;
        }

        اگر (ret_list)
                *ret_list = TAKE_PTR(l);

        بازگشت n;
}

ایستا صحیح getenv_tmp_dir(ثابت نویسه **ret_path) {
        صحیح r, ret = 0;

        أكد(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ثابت نویسه *e;

                e = secure_getenv(n);
                اگر (!e)
                        ادامه;
                اگر (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        برو next;
                }
                اگر (!path_is_normalized(e)) {
                        r = -EPERM;
                        برو next;
                }

                r = is_dir(e, true);
                اگر (r < 0)
                        برو next;
                اگر (r == 0) {
                        r = -ENOTDIR;
                        برو next;
                }

                *ret_path = e;
                بازگشت 1;

        next:
                /* Remember first error, to make this more debuggable */
                اگر (ret >= 0)
                        ret = r;
        }

        اگر (ret < 0)
                بازگشت ret;

        *ret_path = پوچ;
        بازگشت ret;
}

ایستا صحیح tmp_dir_internal(ثابت نویسه *def, ثابت نویسه **ret) {
        ثابت نویسه *e;
        صحیح r, k;

        أكد(def);
        أكد(ret);

        r = getenv_tmp_dir(&e);
        اگر (r > 0) {
                *ret = e;
                بازگشت 0;
        }

        k = is_dir(def, /* follow= */ true);
        اگر (k == 0)
                k = -ENOTDIR;
        اگر (k < 0)
                بازگشت RET_GATHER(r, k);

        *ret = def;
        بازگشت 0;
}

صحیح var_tmp_dir(ثابت نویسه **ret) {
        أكد(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        بازگشت tmp_dir_internal("/var/tmp", ret);
}

صحیح tmp_dir(ثابت نویسه **ret) {
        أكد(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        بازگشت tmp_dir_internal("/tmp", ret);
}

صحیح unlink_or_warn(ثابت نویسه *filename) {
        أكد(filename);

        اگر (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                اگر (errno != EROFS || access(filename, F_OK) >= 0)
                        بازگشت log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        بازگشت 0;
}

نویسه *rmdir_and_free(نویسه *p) {
        PROTECT_ERRNO;

        اگر (!p)
                بازگشت پوچ;

        (تهی) rmdir(p);
        بازگشت mfree(p);
}

نویسه* unlink_and_free(نویسه *p) {
        PROTECT_ERRNO;

        اگر (!p)
                بازگشت پوچ;

        (تهی) unlink(p);
        بازگشت mfree(p);
}

صحیح unlinkat_deallocate(صحیح fd, ثابت نویسه *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ صحیح truncate_fd = -EBADF;
        ساختار stat st;
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

        اگر (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                اگر (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        اگر (IN_SET(errno, ENOENT, EISDIR))
                                بازگشت -errno;

                        اگر (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        اگر (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                بازگشت -errno;

        اگر (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                بازگشت 0;

        اگر (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                بازگشت 0;
        }

        اگر (!S_ISREG(st.st_mode))
                بازگشت 0;

        اگر (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t چپ = st.st_size;
                نویسه buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, اندازه(buffer));

                وقتی (چپ > 0) {
                        ssize_t n;

                        n = اكتب(truncate_fd, buffer, MIN(اندازه(buffer), چپ));
                        اگر (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                بشکن;
                        }

                        أكد(چپ >= (size_t) n);
                        چپ -= n;
                }

                /* Let's refresh metadata */
                اگر (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        بازگشت 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        اگر (st.st_blocks == 0 || st.st_nlink > 0)
                بازگشت 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        اگر (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                بازگشت 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        اگر (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                بازگشت 0;
        }

        بازگشت 0;
}

صحیح open_parent_at(صحیح dir_fd, ثابت نویسه *path, صحیح flags, mode_t mode) {
        _cleanup_free_ نویسه *parent = پوچ;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);

        r = path_extract_directory(path, &parent);
        اگر (r == -EDESTADDRREQ) {
                parent = strdup(".");
                اگر (!parent)
                        بازگشت -ENOMEM;
        } وگرنه اگر (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                اگر (!parent)
                        بازگشت -ENOMEM;
        } وگرنه اگر (r < 0)
                بازگشت r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        اگر (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        بازگشت RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

صحیح conservative_renameat(
                صحیح olddirfd, ثابت نویسه *oldpath,
                صحیح newdirfd, ثابت نویسه *newpath) {

        _cleanup_close_ صحیح old_fd = -EBADF, new_fd = -EBADF;
        ساختار stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        اگر (old_fd < 0)
                برو do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        اگر (new_fd < 0)
                برو do_rename;

        اگر (fstat(old_fd, &old_stat) < 0)
                برو do_rename;

        اگر (!S_ISREG(old_stat.st_mode))
                برو do_rename;

        اگر (fstat(new_fd, &new_stat) < 0)
                برو do_rename;

        اگر (stat_inode_same(&new_stat, &old_stat))
                برو is_same;

        اگر (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                برو do_rename;

        برای (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[اندازه(buf1)];
                ssize_t l1, l2;

                l1 = اقرأ(old_fd, buf1, اندازه(buf1));
                اگر (l1 < 0)
                        برو do_rename;

                اگر (l1 == اندازه(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = اقرأ(new_fd, buf2, l1);
                وگرنه {
                        أكد((size_t) l1 < اندازه(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        أكد((size_t) (l1 + 1) <= اندازه(buf2));
                        l2 = اقرأ(new_fd, buf2, l1 + 1);
                }
                اگر (l2 != l1)
                        برو do_rename;

                اگر (memcmp(buf1, buf2, l1) != 0)
                        برو do_rename;

                اگر ((size_t) l1 < اندازه(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        بشکن;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        اگر (unlinkat(olddirfd, oldpath, 0) < 0)
                برو do_rename;

        بازگشت 0;

do_rename:
        اگر (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                بازگشت -errno;

        بازگشت 1;
}

صحیح posix_fallocate_loop(صحیح fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        صحیح r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        اگر (r != EINTR)
                بازگشت -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ثابت RateLimit) { 10 * USEC_PER_SEC, 16 };
        وقتی (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                اگر (r != EINTR)
                        بازگشت -r;
        }

        بازگشت -EINTR;
}

صحیح parse_cifs_service(
                ثابت نویسه *s,
                نویسه **ret_host,
                نویسه **ret_service,
                نویسه **ret_path) {

        _cleanup_free_ نویسه *h = پوچ, *ss = پوچ, *x = پوچ;
        ثابت نویسه *p, *e, *d;
        نویسه delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        اگر (!s)
                بازگشت -EINVAL;

        p = startswith(s, "//");
        اگر (!p) {
                p = startswith(s, "\\\\");
                اگر (!p)
                        بازگشت -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        اگر (!e)
                بازگشت -EINVAL;

        h = strndup(p, e - p);
        اگر (!h)
                بازگشت -ENOMEM;

        اگر (!hostname_is_valid(h, 0))
                بازگشت -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        اگر (!ss)
                بازگشت -ENOMEM;

        اگر (!filename_is_valid(ss))
                بازگشت -EINVAL;

        اگر (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                اگر (!x)
                        بازگشت -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                برای (نویسه *i = x; *i; i++)
                        اگر (*i == delimiter)
                                *i = '/';

                اگر (!path_is_valid(x))
                        بازگشت -EINVAL;

                path_simplify(x);
                اگر (!path_is_normalized(x))
                        بازگشت -EINVAL;
        }

        اگر (ret_host)
                *ret_host = TAKE_PTR(h);
        اگر (ret_service)
                *ret_service = TAKE_PTR(ss);
        اگر (ret_path)
                *ret_path = TAKE_PTR(x);

        بازگشت 0;
}

صحیح open_mkdir_at_full_label(صحیح dirfd, ثابت نویسه *path, صحیح flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ صحیح fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ نویسه *fname = پوچ, *parent = پوچ;
        صحیح r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        اگر (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                بازگشت -EINVAL;
        اگر ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                بازگشت -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        اگر (r < 0) {
                اگر (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        بازگشت r;
        } وگرنه {
                r = path_extract_filename(path, &fname);
                اگر (r < 0)
                        بازگشت r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                اگر (parent_fd < 0)
                        بازگشت -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        اگر (IN_SET(fd, -ELOOP, -ENOTDIR))
                بازگشت -EEXIST;
        اگر (fd < 0)
                بازگشت fd;

        بازگشت TAKE_FD(fd);
}

صحیح openat_report_new(صحیح dirfd, ثابت نویسه *pathname, صحیح flags, mode_t mode, bool *ret_newly_created) {
        صحیح fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        اگر (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                اگر (fd < 0)
                        بازگشت -errno;

                اگر (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                بازگشت fd;
        }

        برای (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                اگر (fd >= 0) {
                        اگر (ret_newly_created)
                                *ret_newly_created = false;
                        بازگشت fd;
                }
                اگر (errno != ENOENT)
                        بازگشت -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                اگر (fd >= 0) {
                        اگر (ret_newly_created)
                                *ret_newly_created = true;
                        بازگشت fd;
                }
                اگر (errno != EEXIST)
                        بازگشت -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                اگر (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        بازگشت -EEXIST;
        }
}

ایستا صحیح openat_with_automount(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        ایستا bool can_open_درخت = true;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(path);
        أكد(FLAGS_SET(open_flags, O_PATH));

        اگر (can_open_درخت) {
                صحیح fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                اگر (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        اگر (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                صحیح q = fd_verify_directory(fd);
                                اگر (q < 0) {
                                        safe_close(fd);
                                        بازگشت q;
                                }
                        }

                        بازگشت fd;
                }
                اگر (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        بازگشت fd;

                can_open_درخت = false;
        }

        بازگشت RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

صحیح xopenat_full_label(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ صحیح fd = -EBADF;
        bool made_dir = false, made_file = false;
        صحیح r;

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

        اگر (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        اگر (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                اگر (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } وگرنه اگر (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        بازگشت -EINVAL;
        }

        اگر (isempty(path)) {
                أكد(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                اگر (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        اگر (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                بازگشت TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                بازگشت fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ صحیح _dir_fd = -EBADF;
        اگر (dir_fd == XAT_FDROOT) {
                اگر (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                وگرنه {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        اگر (_dir_fd < 0)
                                بازگشت -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        اگر (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                اگر (r < 0)
                        بازگشت r;

                call_label_ops_post = true;
        }

        اگر (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                اگر (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                وگرنه
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                اگر (r == -EEXIST) {
                        اگر (FLAGS_SET(open_flags, O_EXCL))
                                بازگشت -EEXIST;
                } وگرنه اگر (r < 0)
                        بازگشت r;
                وگرنه
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        اگر (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                اگر (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        اگر (fd < 0) {
                                r = fd;
                                برو error;
                        }

                        r = fd_verify_regular(fd);
                        اگر (r < 0)
                                برو error;

                } وگرنه اگر (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        اگر (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                وگرنه اگر (fd < 0) {
                                        r = fd;
                                        برو error;
                                }
                        }

                        اگر (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                اگر (fd < 0) {
                                        r = -errno;
                                        برو error;
                                }
                        }

                        made_file = true;
                } وگرنه {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ صحیح inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        اگر (inode_fd < 0) {
                                اگر (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        برو error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                اگر (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        وگرنه اگر (fd < 0) {
                                                r = fd;
                                                برو error;
                                        }
                                }

                                اگر (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        اگر (fd < 0) {
                                                r = -errno;
                                                برو error;
                                        }
                                }

                                made_file = true;
                        } وگرنه {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                اگر (r < 0)
                                        برو error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                اگر (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        وگرنه اگر (fd < 0) {
                                                r = fd;
                                                برو error;
                                        }
                                }

                                اگر (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        اگر (fd < 0) {
                                                r = fd;
                                                برو error;
                                        }
                                }
                        }
                }
        } وگرنه اگر (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                اگر (fd < 0) {
                        r = fd;
                        برو error;
                }
        } وگرنه {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                اگر (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        اگر (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        وگرنه اگر (fd < 0) {
                                r = fd;
                                برو error;
                        }
                }

                اگر (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        اگر (fd < 0) {
                                r = fd;
                                برو error;
                        }
                }
        }

        اگر (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                اگر (r < 0)
                        برو error;
        }

        اگر (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ پوچ, made_file || made_dir, label_context);
                اگر (r < 0)
                        برو error;
        }

        اگر (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                اگر (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        برو error;
        }

        بازگشت TAKE_FD(fd);

error:
        اگر (call_label_ops_post)
                (تهی) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? پوچ : path, made_dir || made_file, label_context);

        اگر (made_dir || made_file)
                (تهی) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        بازگشت r;
}

صحیح xopenat_lock_full_label(
                صحیح dir_fd,
                ثابت نویسه *path,
                صحیح open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                صحیح operation,
                LabelContext *label_context) {

        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        اگر (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                بازگشت -EBADF;

        برای (;;) {
                ساختار stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                اگر (fd < 0)
                        بازگشت fd;

                r = lock_generic(fd, locktype, operation);
                اگر (r < 0)
                        بازگشت r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                اگر (fstat(fd, &st) < 0)
                        بازگشت -errno;
                اگر (st.st_nlink > 0)
                        بشکن;

                fd = safe_close(fd);
        }

        بازگشت TAKE_FD(fd);
}

صحیح link_fd(صحیح fd, صحیح newdirfd, ثابت نویسه *newpath) {
        صحیح r;

        أكد(fd >= 0);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        اگر (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                اگر (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        بازگشت -ENOSYS;
        }

        بازگشت r;
}

صحیح linkat_replace(صحیح olddirfd, ثابت نویسه *oldpath, صحیح newdirfd, ثابت نویسه *newpath) {
        _cleanup_close_ صحیح old_fd = -EBADF;
        صحیح r;

        أكد(olddirfd >= 0 || olddirfd == AT_FDCWD);
        أكد(newdirfd >= 0 || newdirfd == AT_FDCWD);
        أكد(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        اگر (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                بازگشت -EISDIR;

        اگر (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                بازگشت -EISDIR;

        اگر (path_implies_directory(newpath))
                بازگشت -EISDIR;

        /* First, try to link this directly */
        اگر (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        وگرنه
                r = link_fd(olddirfd, newdirfd, newpath);
        اگر (r >= 0)
                بازگشت 0;
        اگر (r != -EEXIST)
                بازگشت r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        اگر (old_fd < 0)
                بازگشت old_fd;

        ساختار stat old_st;
        اگر (fstat(old_fd, &old_st) < 0)
                بازگشت -errno;

        اگر (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                بازگشت -EISDIR;

        ساختار stat new_st;
        اگر (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                بازگشت -errno;

        اگر (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                بازگشت -EEXIST;

        اگر (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                بازگشت 0;

        _cleanup_free_ نویسه *tmp_path = پوچ;
        r = tempfn_random(newpath, /* extra= */ پوچ, &tmp_path);
        اگر (r < 0)
                بازگشت r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        اگر (r < 0) {
                اگر (!ERRNO_IS_PRIVILEGE(r))
                        بازگشت r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                اگر (r < 0)
                        بازگشت r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        اگر (r < 0) {
                (تهی) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                بازگشت r;
        }

        بازگشت 0;
}
