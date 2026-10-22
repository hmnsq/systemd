/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/falloc.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/file.h>
#அந்தர்பாவ <sys/mount.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "btrfs-util.h"
#அந்தர்பாவ "chattr-util.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "hostname-util.h"
#அந்தர்பாவ "label-util.h"
#அந்தர்பாவ "lock-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mkdir.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "random-util.h"
#அந்தர்பாவ "ratelimit.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "tmpfile-util.h"
#அந்தர்பாவ "umask-util.h"

முழு rmdir_parents(மாறிலி எழுத்து *path, மாறிலி எழுத்து *stop) {
        எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(stop);

        எனில் (!path_is_safe(path))
                திரும்பு -EINVAL;

        எனில் (!path_is_safe(stop))
                திரும்பு -EINVAL;

        p = strdupa_safe(path);

        க்கு (;;) {
                எழுத்து *slash = வெற்று;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (மாறிலி எழுத்து **) &slash, வெற்று);
                எனில் (r <= 0)
                        திரும்பு r;
                எனில் (slash == p)
                        திரும்பு 0;

                ப்ரதிஜ்ஞா(*slash == '/');
                *slash = '\0';

                எனில் (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        திரும்பு 0;

                எனில் (rmdir(p) < 0 && errno != ENOENT)
                        திரும்பு -errno;
        }
}

முழு rename_noreplace(முழு olddirfd, மாறிலி எழுத்து *oldpath, முழு newdirfd, மாறிலி எழுத்து *newpath) {
        முழு r;

        ப்ரதிஜ்ஞா(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ப்ரதிஜ்ஞா(oldpath);
        ப்ரதிஜ்ஞா(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ப்ரதிஜ்ஞா(newpath);

        /* Try the ideal approach first */
        எனில் (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                திரும்பு 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        எனில் (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                திரும்பு -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        எனில் (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                எனில் (r < 0) {
                        (வெறுமை) unlinkat(newdirfd, newpath, 0);
                        திரும்பு r;
                }

                திரும்பு 0;
        }

        எனில் (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                திரும்பு -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        எனில் (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                திரும்பு -EEXIST;
        எனில் (errno != ENOENT)
                திரும்பு -errno;

        திரும்பு RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

முழு readlinkat_malloc(முழு fd, மாறிலி எழுத்து *p, எழுத்து **ret) {
        size_t l = PATH_MAX;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);

        எனில் (fd < 0 && isempty(p))
                திரும்பு -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        க்கு (;;) {
                _cleanup_free_ எழுத்து *c = வெற்று;
                ssize_t n;

                c = new(எழுத்து, l+1);
                எனில் (!c)
                        திரும்பு -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                எனில் (n < 0)
                        திரும்பு -errno;

                எனில் ((size_t) n < l) {
                        c[n] = 0;

                        எனில் (ret)
                                *ret = TAKE_PTR(c);

                        திரும்பு 0;
                }

                எனில் (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        திரும்பு -EFBIG;

                l *= 2;
        }
}

முழு readlink_மதிப்பு(மாறிலி எழுத்து *p, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *link = வெற்று, *name = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        r = readlink_malloc(p, &link);
        எனில் (r < 0)
                திரும்பு r;

        r = path_extract_filename(link, &name);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == O_DIRECTORY)
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(name);
        திரும்பு 0;
}

முழு readlink_and_make_absolute(மாறிலி எழுத்து *p, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *target = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        r = readlink_malloc(p, &target);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு file_in_same_dir(p, target, ret);
}

முழு chmod_and_chown_at(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);

        எனில் (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                எனில் (fd < 0)
                        திரும்பு -errno;
                dir_fd = fd;

        } இல்லையெனில் எனில் (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                எனில் (fd < 0)
                        திரும்பு -errno;
                dir_fd = fd;
        }

        திரும்பு fchmod_and_chown(dir_fd, mode, uid, gid);
}

முழு fchmod_and_chown_with_fallback(முழு fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        கட்டமைப்பு stat st;
        முழு r;

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

        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        எனில் (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        இல்லையெனில் எனில் ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                திரும்பு -EINVAL; /* insist on the right file type if it was specified */

        எனில் (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                எனில் (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        எனில் (r < 0) {
                                எனில் (!path || r != -ENOSYS)
                                        திரும்பு r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                எனில் (chmod(path, minimal & 07777) < 0)
                                        திரும்பு -errno;
                        }
                }
        }

        எனில் (do_chown)
                எனில் (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        திரும்பு -errno;

        எனில் (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                எனில் (r < 0) {
                        எனில் (!path || r != -ENOSYS)
                                திரும்பு r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        எனில் (chmod(path, mode & 07777) < 0)
                                திரும்பு -errno;
                }
        }

        திரும்பு do_chown || do_chmod;
}

முழு fchmod_umask(முழு fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        திரும்பு RET_NERRNO(fchmod(fd, m & (~u)));
}

முழு fchmod_opath(முழு fd, mode_t m) {
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

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                திரும்பு 0;
        எனில் (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                திரும்பு 0;
        எனில் (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                திரும்பு -errno;

        எனில் (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                திரும்பு proc_fd_enoent_errno();
        }

        திரும்பு 0;
}

முழு futimens_opath(முழு fd, மாறிலி கட்டமைப்பு timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        ப்ரதிஜ்ஞா(fd >= 0);

        திரும்பு RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

முழு stat_warn_permissions(மாறிலி எழுத்து *path, மாறிலி கட்டமைப்பு stat *st) {
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        எனில் (!S_ISREG(st->st_mode))
                திரும்பு 0;

        எனில் (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        எனில் (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        எனில் (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        திரும்பு 0;
}

முழு fd_warn_permissions(மாறிலி எழுத்து *path, முழு fd) {
        கட்டமைப்பு stat st;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        திரும்பு stat_warn_permissions(path, &st);
}

முழு access_nofollow(மாறிலி எழுத்து *path, முழு mode) {
        திரும்பு RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

முழு access_fd(முழு fd, முழு mode) {
        /* Like access() but operates on an already open fd */

        எனில் (fd == AT_FDCWD)
                திரும்பு RET_NERRNO(access(".", mode));
        எனில் (fd == XAT_FDROOT)
                திரும்பு RET_NERRNO(access("/", mode));

        ப்ரதிஜ்ஞா(fd >= 0);

        திரும்பு RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

முழு touch_fd(முழு fd, usec_t stamp) {
        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (stamp == USEC_INFINITY)
                திரும்பு futimens_opath(fd, /* ts= */ வெற்று);

        கட்டமைப்பு timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        திரும்பு futimens_opath(fd, ts);
}

முழு touch_file(மாறிலி எழுத்து *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ முழு fd = -EBADF;
        முழு ret;

        ப்ரதிஜ்ஞா(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        எனில் (parents)
                (வெறுமை) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        எனில் (fd < 0) {
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                எனில் (fd < 0)
                        திரும்பு -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        திரும்பு RET_GATHER(ret, touch_fd(fd, stamp));
}

முழு touch(மாறிலி எழுத்து *path) {
        திரும்பு touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

முழு symlinkat_idempotent(மாறிலி எழுத்து *target, முழு atfd, மாறிலி எழுத்து *linkpath, bool make_relative) {
        _cleanup_free_ எழுத்து *relpath = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(target);
        ப்ரதிஜ்ஞா(linkpath);

        எனில் (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                எனில் (r < 0)
                        திரும்பு r;

                target = relpath;
        }

        எனில் (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ எழுத்து *p = வெற்று;

                எனில் (errno != EEXIST)
                        திரும்பு -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                எனில் (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        திரும்பு -EEXIST;
                எனில் (r < 0) /* Any other error? In that case propagate it as is */
                        திரும்பு r;

                எனில் (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        திரும்பு -EEXIST;
        }

        திரும்பு 0;
}

முழு symlinkat_atomic_full_label(மாறிலி எழுத்து *target, முழு atfd, மாறிலி எழுத்து *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        முழு r;

        ப்ரதிஜ்ஞா(target);
        ப்ரதிஜ்ஞா(linkpath);

        _cleanup_free_ எழுத்து *relpath = வெற்று;
        எனில் (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                எனில் (r < 0)
                        திரும்பு r;

                target = relpath;
        }

        _cleanup_free_ எழுத்து *t = வெற்று;
        r = tempfn_random(linkpath, வெற்று, &t);
        எனில் (r < 0)
                திரும்பு r;

        bool call_label_ops_post = false;
        எனில் (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                எனில் (r < 0)
                        திரும்பு r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        எனில் (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        எனில் (r < 0)
                திரும்பு r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        எனில் (r < 0) {
                (வெறுமை) unlinkat(atfd, t, 0);
                திரும்பு r;
        }

        திரும்பு 0;
}

முழு mknodat_atomic(முழு atfd, மாறிலி எழுத்து *path, mode_t mode, dev_t dev) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        r = tempfn_random(path, வெற்று, &t);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (mknodat(atfd, t, mode, dev) < 0)
                திரும்பு -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        எனில் (r < 0) {
                (வெறுமை) unlinkat(atfd, t, 0);
                திரும்பு r;
        }

        திரும்பு 0;
}

முழு mkfifoat_atomic(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode) {
        _cleanup_free_ எழுத்து *t = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, வெற்று, &t);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (mkfifoat(dir_fd, t, mode) < 0)
                திரும்பு -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        எனில் (r < 0) {
                (வெறுமை) unlinkat(dir_fd, t, 0);
                திரும்பு r;
        }

        திரும்பு 0;
}

முழு get_files_in_directory(மாறிலி எழுத்து *path, எழுத்து ***ret_list) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        _cleanup_closedir_ DIR *d = வெற்று;
        size_t n = 0;

        ப்ரதிஜ்ஞா(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        எனில் (!d)
                திரும்பு -errno;

        FOREACH_DIRENT_ALL(de, d, திரும்பு -errno) {
                எனில் (!dirent_is_file(de))
                        தொடர்;

                எனில் (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        எனில் (!GREEDY_REALLOC(l, n + 2))
                                திரும்பு -ENOMEM;

                        l[n] = strdup(de->d_name);
                        எனில் (!l[n])
                                திரும்பு -ENOMEM;

                        l[++n] = வெற்று;
                } இல்லையெனில்
                        n++;
        }

        எனில் (ret_list)
                *ret_list = TAKE_PTR(l);

        திரும்பு n;
}

நிலையான முழு getenv_tmp_dir(மாறிலி எழுத்து **ret_path) {
        முழு r, ret = 0;

        ப்ரதிஜ்ஞா(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                மாறிலி எழுத்து *e;

                e = secure_getenv(n);
                எனில் (!e)
                        தொடர்;
                எனில் (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        செல் next;
                }
                எனில் (!path_is_normalized(e)) {
                        r = -EPERM;
                        செல் next;
                }

                r = is_dir(e, true);
                எனில் (r < 0)
                        செல் next;
                எனில் (r == 0) {
                        r = -ENOTDIR;
                        செல் next;
                }

                *ret_path = e;
                திரும்பு 1;

        next:
                /* Remember first error, to make this more debuggable */
                எனில் (ret >= 0)
                        ret = r;
        }

        எனில் (ret < 0)
                திரும்பு ret;

        *ret_path = வெற்று;
        திரும்பு ret;
}

நிலையான முழு tmp_dir_internal(மாறிலி எழுத்து *def, மாறிலி எழுத்து **ret) {
        மாறிலி எழுத்து *e;
        முழு r, k;

        ப்ரதிஜ்ஞா(def);
        ப்ரதிஜ்ஞா(ret);

        r = getenv_tmp_dir(&e);
        எனில் (r > 0) {
                *ret = e;
                திரும்பு 0;
        }

        k = is_dir(def, /* follow= */ true);
        எனில் (k == 0)
                k = -ENOTDIR;
        எனில் (k < 0)
                திரும்பு RET_GATHER(r, k);

        *ret = def;
        திரும்பு 0;
}

முழு var_tmp_dir(மாறிலி எழுத்து **ret) {
        ப்ரதிஜ்ஞா(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        திரும்பு tmp_dir_internal("/var/tmp", ret);
}

முழு tmp_dir(மாறிலி எழுத்து **ret) {
        ப்ரதிஜ்ஞா(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        திரும்பு tmp_dir_internal("/tmp", ret);
}

முழு unlink_or_warn(மாறிலி எழுத்து *filename) {
        ப்ரதிஜ்ஞா(filename);

        எனில் (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                எனில் (errno != EROFS || access(filename, F_OK) >= 0)
                        திரும்பு log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        திரும்பு 0;
}

எழுத்து *rmdir_and_free(எழுத்து *p) {
        PROTECT_ERRNO;

        எனில் (!p)
                திரும்பு வெற்று;

        (வெறுமை) rmdir(p);
        திரும்பு mfree(p);
}

எழுத்து* unlink_and_free(எழுத்து *p) {
        PROTECT_ERRNO;

        எனில் (!p)
                திரும்பு வெற்று;

        (வெறுமை) unlink(p);
        திரும்பு mfree(p);
}

முழு unlinkat_deallocate(முழு fd, மாறிலி எழுத்து *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ முழு truncate_fd = -EBADF;
        கட்டமைப்பு stat st;
        off_t l, bs;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        எனில் (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                எனில் (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        எனில் (IN_SET(errno, ENOENT, EISDIR))
                                திரும்பு -errno;

                        எனில் (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        எனில் (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                திரும்பு -errno;

        எனில் (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                திரும்பு 0;

        எனில் (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                திரும்பு 0;
        }

        எனில் (!S_ISREG(st.st_mode))
                திரும்பு 0;

        எனில் (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t இடது = st.st_size;
                எழுத்து buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, அளவு(buffer));

                போது (இடது > 0) {
                        ssize_t n;

                        n = லிக(truncate_fd, buffer, MIN(அளவு(buffer), இடது));
                        எனில் (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                நிறுத்து;
                        }

                        ப்ரதிஜ்ஞா(இடது >= (size_t) n);
                        இடது -= n;
                }

                /* Let's refresh metadata */
                எனில் (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        திரும்பு 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        எனில் (st.st_blocks == 0 || st.st_nlink > 0)
                திரும்பு 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        எனில் (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                திரும்பு 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        எனில் (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                திரும்பு 0;
        }

        திரும்பு 0;
}

முழு open_parent_at(முழு dir_fd, மாறிலி எழுத்து *path, முழு flags, mode_t mode) {
        _cleanup_free_ எழுத்து *parent = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(path);

        r = path_extract_directory(path, &parent);
        எனில் (r == -EDESTADDRREQ) {
                parent = strdup(".");
                எனில் (!parent)
                        திரும்பு -ENOMEM;
        } இல்லையெனில் எனில் (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                எனில் (!parent)
                        திரும்பு -ENOMEM;
        } இல்லையெனில் எனில் (r < 0)
                திரும்பு r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        எனில் (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        திரும்பு RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

முழு conservative_renameat(
                முழு olddirfd, மாறிலி எழுத்து *oldpath,
                முழு newdirfd, மாறிலி எழுத்து *newpath) {

        _cleanup_close_ முழு old_fd = -EBADF, new_fd = -EBADF;
        கட்டமைப்பு stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        எனில் (old_fd < 0)
                செல் do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        எனில் (new_fd < 0)
                செல் do_rename;

        எனில் (fstat(old_fd, &old_stat) < 0)
                செல் do_rename;

        எனில் (!S_ISREG(old_stat.st_mode))
                செல் do_rename;

        எனில் (fstat(new_fd, &new_stat) < 0)
                செல் do_rename;

        எனில் (stat_inode_same(&new_stat, &old_stat))
                செல் is_same;

        எனில் (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                செல் do_rename;

        க்கு (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[அளவு(buf1)];
                ssize_t l1, l2;

                l1 = பட(old_fd, buf1, அளவு(buf1));
                எனில் (l1 < 0)
                        செல் do_rename;

                எனில் (l1 == அளவு(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = பட(new_fd, buf2, l1);
                இல்லையெனில் {
                        ப்ரதிஜ்ஞா((size_t) l1 < அளவு(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        ப்ரதிஜ்ஞா((size_t) (l1 + 1) <= அளவு(buf2));
                        l2 = பட(new_fd, buf2, l1 + 1);
                }
                எனில் (l2 != l1)
                        செல் do_rename;

                எனில் (memcmp(buf1, buf2, l1) != 0)
                        செல் do_rename;

                எனில் ((size_t) l1 < அளவு(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        நிறுத்து;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        எனில் (unlinkat(olddirfd, oldpath, 0) < 0)
                செல் do_rename;

        திரும்பு 0;

do_rename:
        எனில் (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                திரும்பு -errno;

        திரும்பு 1;
}

முழு posix_fallocate_loop(முழு fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        முழு r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        எனில் (r != EINTR)
                திரும்பு -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (மாறிலி RateLimit) { 10 * USEC_PER_SEC, 16 };
        போது (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                எனில் (r != EINTR)
                        திரும்பு -r;
        }

        திரும்பு -EINTR;
}

முழு parse_cifs_service(
                மாறிலி எழுத்து *s,
                எழுத்து **ret_host,
                எழுத்து **ret_service,
                எழுத்து **ret_path) {

        _cleanup_free_ எழுத்து *h = வெற்று, *ss = வெற்று, *x = வெற்று;
        மாறிலி எழுத்து *p, *e, *d;
        எழுத்து delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        எனில் (!s)
                திரும்பு -EINVAL;

        p = startswith(s, "//");
        எனில் (!p) {
                p = startswith(s, "\\\\");
                எனில் (!p)
                        திரும்பு -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        எனில் (!e)
                திரும்பு -EINVAL;

        h = strndup(p, e - p);
        எனில் (!h)
                திரும்பு -ENOMEM;

        எனில் (!hostname_is_valid(h, 0))
                திரும்பு -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        எனில் (!ss)
                திரும்பு -ENOMEM;

        எனில் (!filename_is_valid(ss))
                திரும்பு -EINVAL;

        எனில் (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                எனில் (!x)
                        திரும்பு -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                க்கு (எழுத்து *i = x; *i; i++)
                        எனில் (*i == delimiter)
                                *i = '/';

                எனில் (!path_is_valid(x))
                        திரும்பு -EINVAL;

                path_simplify(x);
                எனில் (!path_is_normalized(x))
                        திரும்பு -EINVAL;
        }

        எனில் (ret_host)
                *ret_host = TAKE_PTR(h);
        எனில் (ret_service)
                *ret_service = TAKE_PTR(ss);
        எனில் (ret_path)
                *ret_path = TAKE_PTR(x);

        திரும்பு 0;
}

முழு open_mkdir_at_full_label(முழு dirfd, மாறிலி எழுத்து *path, முழு flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ முழு fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ எழுத்து *fname = வெற்று, *parent = வெற்று;
        முழு r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        எனில் (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                திரும்பு -EINVAL;
        எனில் ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                திரும்பு -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        எனில் (r < 0) {
                எனில் (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        திரும்பு r;
        } இல்லையெனில் {
                r = path_extract_filename(path, &fname);
                எனில் (r < 0)
                        திரும்பு r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                எனில் (parent_fd < 0)
                        திரும்பு -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        எனில் (IN_SET(fd, -ELOOP, -ENOTDIR))
                திரும்பு -EEXIST;
        எனில் (fd < 0)
                திரும்பு fd;

        திரும்பு TAKE_FD(fd);
}

முழு openat_report_new(முழு dirfd, மாறிலி எழுத்து *pathname, முழு flags, mode_t mode, bool *ret_newly_created) {
        முழு fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        எனில் (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                எனில் (fd < 0)
                        திரும்பு -errno;

                எனில் (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                திரும்பு fd;
        }

        க்கு (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                எனில் (fd >= 0) {
                        எனில் (ret_newly_created)
                                *ret_newly_created = false;
                        திரும்பு fd;
                }
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                எனில் (fd >= 0) {
                        எனில் (ret_newly_created)
                                *ret_newly_created = true;
                        திரும்பு fd;
                }
                எனில் (errno != EEXIST)
                        திரும்பு -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                எனில் (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        திரும்பு -EEXIST;
        }
}

நிலையான முழு openat_with_automount(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        நிலையான bool can_open_மரம் = true;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(FLAGS_SET(open_flags, O_PATH));

        எனில் (can_open_மரம்) {
                முழு fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                எனில் (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        எனில் (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                முழு q = fd_verify_directory(fd);
                                எனில் (q < 0) {
                                        safe_close(fd);
                                        திரும்பு q;
                                }
                        }

                        திரும்பு fd;
                }
                எனில் (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        திரும்பு fd;

                can_open_மரம் = false;
        }

        திரும்பு RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

முழு xopenat_full_label(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ முழு fd = -EBADF;
        bool made_dir = false, made_file = false;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        ப்ரதிஜ்ஞா(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        ப்ரதிஜ்ஞா(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        ப்ரதிஜ்ஞா(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        ப்ரதிஜ்ஞா(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        ப்ரதிஜ்ஞா(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        ப்ரதிஜ்ஞா((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        ப்ரதிஜ்ஞா(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        எனில் (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        எனில் (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                எனில் (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } இல்லையெனில் எனில் (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        திரும்பு -EINVAL;
        }

        எனில் (isempty(path)) {
                ப்ரதிஜ்ஞா(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                எனில் (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        எனில் (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                திரும்பு TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                திரும்பு fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ முழு _dir_fd = -EBADF;
        எனில் (dir_fd == XAT_FDROOT) {
                எனில் (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                இல்லையெனில் {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        எனில் (_dir_fd < 0)
                                திரும்பு -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        எனில் (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                எனில் (r < 0)
                        திரும்பு r;

                call_label_ops_post = true;
        }

        எனில் (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                எனில் (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                இல்லையெனில்
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                எனில் (r == -EEXIST) {
                        எனில் (FLAGS_SET(open_flags, O_EXCL))
                                திரும்பு -EEXIST;
                } இல்லையெனில் எனில் (r < 0)
                        திரும்பு r;
                இல்லையெனில்
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        எனில் (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                எனில் (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        எனில் (fd < 0) {
                                r = fd;
                                செல் error;
                        }

                        r = fd_verify_regular(fd);
                        எனில் (r < 0)
                                செல் error;

                } இல்லையெனில் எனில் (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        எனில் (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                எனில் (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                இல்லையெனில் எனில் (fd < 0) {
                                        r = fd;
                                        செல் error;
                                }
                        }

                        எனில் (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                எனில் (fd < 0) {
                                        r = -errno;
                                        செல் error;
                                }
                        }

                        made_file = true;
                } இல்லையெனில் {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ முழு inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        எனில் (inode_fd < 0) {
                                எனில் (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        செல் error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                எனில் (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        எனில் (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        இல்லையெனில் எனில் (fd < 0) {
                                                r = fd;
                                                செல் error;
                                        }
                                }

                                எனில் (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        எனில் (fd < 0) {
                                                r = -errno;
                                                செல் error;
                                        }
                                }

                                made_file = true;
                        } இல்லையெனில் {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                எனில் (r < 0)
                                        செல் error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                எனில் (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        எனில் (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        இல்லையெனில் எனில் (fd < 0) {
                                                r = fd;
                                                செல் error;
                                        }
                                }

                                எனில் (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        எனில் (fd < 0) {
                                                r = fd;
                                                செல் error;
                                        }
                                }
                        }
                }
        } இல்லையெனில் எனில் (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                எனில் (fd < 0) {
                        r = fd;
                        செல் error;
                }
        } இல்லையெனில் {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                எனில் (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        எனில் (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        இல்லையெனில் எனில் (fd < 0) {
                                r = fd;
                                செல் error;
                        }
                }

                எனில் (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        எனில் (fd < 0) {
                                r = fd;
                                செல் error;
                        }
                }
        }

        எனில் (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                எனில் (r < 0)
                        செல் error;
        }

        எனில் (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ வெற்று, made_file || made_dir, label_context);
                எனில் (r < 0)
                        செல் error;
        }

        எனில் (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                எனில் (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        செல் error;
        }

        திரும்பு TAKE_FD(fd);

error:
        எனில் (call_label_ops_post)
                (வெறுமை) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? வெற்று : path, made_dir || made_file, label_context);

        எனில் (made_dir || made_file)
                (வெறுமை) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        திரும்பு r;
}

முழு xopenat_lock_full_label(
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                முழு open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                முழு operation,
                LabelContext *label_context) {

        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        எனில் (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                திரும்பு -EBADF;

        க்கு (;;) {
                கட்டமைப்பு stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                எனில் (fd < 0)
                        திரும்பு fd;

                r = lock_generic(fd, locktype, operation);
                எனில் (r < 0)
                        திரும்பு r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                எனில் (fstat(fd, &st) < 0)
                        திரும்பு -errno;
                எனில் (st.st_nlink > 0)
                        நிறுத்து;

                fd = safe_close(fd);
        }

        திரும்பு TAKE_FD(fd);
}

முழு link_fd(முழு fd, முழு newdirfd, மாறிலி எழுத்து *newpath) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ப்ரதிஜ்ஞா(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        எனில் (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                எனில் (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        திரும்பு -ENOSYS;
        }

        திரும்பு r;
}

முழு linkat_replace(முழு olddirfd, மாறிலி எழுத்து *oldpath, முழு newdirfd, மாறிலி எழுத்து *newpath) {
        _cleanup_close_ முழு old_fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(olddirfd >= 0 || olddirfd == AT_FDCWD);
        ப்ரதிஜ்ஞா(newdirfd >= 0 || newdirfd == AT_FDCWD);
        ப்ரதிஜ்ஞா(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        எனில் (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                திரும்பு -EISDIR;

        எனில் (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                திரும்பு -EISDIR;

        எனில் (path_implies_directory(newpath))
                திரும்பு -EISDIR;

        /* First, try to link this directly */
        எனில் (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        இல்லையெனில்
                r = link_fd(olddirfd, newdirfd, newpath);
        எனில் (r >= 0)
                திரும்பு 0;
        எனில் (r != -EEXIST)
                திரும்பு r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        எனில் (old_fd < 0)
                திரும்பு old_fd;

        கட்டமைப்பு stat old_st;
        எனில் (fstat(old_fd, &old_st) < 0)
                திரும்பு -errno;

        எனில் (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                திரும்பு -EISDIR;

        கட்டமைப்பு stat new_st;
        எனில் (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                திரும்பு -errno;

        எனில் (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                திரும்பு -EEXIST;

        எனில் (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                திரும்பு 0;

        _cleanup_free_ எழுத்து *tmp_path = வெற்று;
        r = tempfn_random(newpath, /* extra= */ வெற்று, &tmp_path);
        எனில் (r < 0)
                திரும்பு r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        எனில் (r < 0) {
                எனில் (!ERRNO_IS_PRIVILEGE(r))
                        திரும்பு r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                எனில் (r < 0)
                        திரும்பு r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        எனில் (r < 0) {
                (வெறுமை) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                திரும்பு r;
        }

        திரும்பு 0;
}
