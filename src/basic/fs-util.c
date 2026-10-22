/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/falloc.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/file.h>
#अन्तर्भाव <sys/mount.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "btrfs-util.h"
#अन्तर्भाव "chattr-util.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "hostname-util.h"
#अन्तर्भाव "label-util.h"
#अन्तर्भाव "lock-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mkdir.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "random-util.h"
#अन्तर्भाव "ratelimit.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "tmpfile-util.h"
#अन्तर्भाव "umask-util.h"

पूर्णांक rmdir_parents(स्थिरांक अक्षर *path, स्थिरांक अक्षर *stop) {
        अक्षर *p;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(stop);

        जर (!path_is_safe(path))
                परत -EINVAL;

        जर (!path_is_safe(stop))
                परत -EINVAL;

        p = strdupa_safe(path);

        साठी (;;) {
                अक्षर *slash = रिक्त;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (स्थिरांक अक्षर **) &slash, रिक्त);
                जर (r <= 0)
                        परत r;
                जर (slash == p)
                        परत 0;

                प्रतिज्ञा(*slash == '/');
                *slash = '\0';

                जर (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        परत 0;

                जर (सूची_हटाओ(p) < 0 && त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;
        }
}

पूर्णांक rename_noreplace(पूर्णांक olddirfd, स्थिरांक अक्षर *oldpath, पूर्णांक newdirfd, स्थिरांक अक्षर *newpath) {
        पूर्णांक r;

        प्रतिज्ञा(olddirfd >= 0 || olddirfd == AT_FDCWD);
        प्रतिज्ञा(oldpath);
        प्रतिज्ञा(newdirfd >= 0 || newdirfd == AT_FDCWD);
        प्रतिज्ञा(newpath);

        /* Try the ideal approach first */
        जर (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                परत 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        जर (!ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) && त्रुटि_सं != EINVAL)
                परत -त्रुटि_सं;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        जर (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                जर (r < 0) {
                        (शून्य) unlinkat(newdirfd, newpath, 0);
                        परत r;
                }

                परत 0;
        }

        जर (!ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं) && !IN_SET(त्रुटि_सं, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                परत -त्रुटि_सं;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        जर (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                परत -EEXIST;
        जर (त्रुटि_सं != ENOENT)
                परत -त्रुटि_सं;

        परत RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

पूर्णांक readlinkat_malloc(पूर्णांक fd, स्थिरांक अक्षर *p, अक्षर **ret) {
        माप_प्रकार l = PATH_MAX;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);

        जर (fd < 0 && isempty(p))
                परत -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        साठी (;;) {
                _cleanup_free_ अक्षर *c = रिक्त;
                ssize_t n;

                c = new(अक्षर, l+1);
                जर (!c)
                        परत -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                जर (n < 0)
                        परत -त्रुटि_सं;

                जर ((माप_प्रकार) n < l) {
                        c[n] = 0;

                        जर (ret)
                                *ret = TAKE_PTR(c);

                        परत 0;
                }

                जर (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        परत -EFBIG;

                l *= 2;
        }
}

पूर्णांक readlink_मूल्य(स्थिरांक अक्षर *p, अक्षर **ret) {
        _cleanup_free_ अक्षर *link = रिक्त, *name = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        r = readlink_malloc(p, &link);
        जर (r < 0)
                परत r;

        r = path_extract_filename(link, &name);
        जर (r < 0)
                परत r;
        जर (r == O_DIRECTORY)
                परत -EINVAL;

        *ret = TAKE_PTR(name);
        परत 0;
}

पूर्णांक readlink_and_make_absolute(स्थिरांक अक्षर *p, अक्षर **ret) {
        _cleanup_free_ अक्षर *target = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        r = readlink_malloc(p, &target);
        जर (r < 0)
                परत r;

        परत file_in_same_dir(p, target, ret);
}

पूर्णांक chmod_and_chown_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        जर (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                जर (fd < 0)
                        परत -त्रुटि_सं;
                dir_fd = fd;

        } अन्यथा जर (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                जर (fd < 0)
                        परत -त्रुटि_सं;
                dir_fd = fd;
        }

        परत fchmod_and_chown(dir_fd, mode, uid, gid);
}

पूर्णांक fchmod_and_chown_with_fallback(पूर्णांक fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        संरचना stat st;
        पूर्णांक r;

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

        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        जर (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        अन्यथा जर ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                परत -EINVAL; /* insist on the right file type if it was specified */

        जर (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                जर (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        जर (r < 0) {
                                जर (!path || r != -ENOSYS)
                                        परत r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                जर (chmod(path, minimal & 07777) < 0)
                                        परत -त्रुटि_सं;
                        }
                }
        }

        जर (do_chown)
                जर (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        परत -त्रुटि_सं;

        जर (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                जर (r < 0) {
                        जर (!path || r != -ENOSYS)
                                परत r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        जर (chmod(path, mode & 07777) < 0)
                                परत -त्रुटि_सं;
                }
        }

        परत do_chown || do_chmod;
}

पूर्णांक fchmod_umask(पूर्णांक fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        परत RET_NERRNO(fchmod(fd, m & (~u)));
}

पूर्णांक fchmod_opath(पूर्णांक fd, mode_t m) {
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

        प्रतिज्ञा(fd >= 0);

        जर (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                परत 0;
        जर (त्रुटि_सं == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                परत 0;
        जर (!IN_SET(त्रुटि_सं, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                परत -त्रुटि_सं;

        जर (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                जर (त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;

                परत proc_fd_enoent_errno();
        }

        परत 0;
}

पूर्णांक futimens_opath(पूर्णांक fd, स्थिरांक संरचना timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        प्रतिज्ञा(fd >= 0);

        परत RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

पूर्णांक stat_warn_permissions(स्थिरांक अक्षर *path, स्थिरांक संरचना stat *st) {
        प्रतिज्ञा(path);
        प्रतिज्ञा(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        जर (!S_ISREG(st->st_mode))
                परत 0;

        जर (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        जर (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        जर (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        परत 0;
}

पूर्णांक fd_warn_permissions(स्थिरांक अक्षर *path, पूर्णांक fd) {
        संरचना stat st;

        प्रतिज्ञा(path);
        प्रतिज्ञा(fd >= 0);

        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        परत stat_warn_permissions(path, &st);
}

पूर्णांक access_nofollow(स्थिरांक अक्षर *path, पूर्णांक mode) {
        परत RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

पूर्णांक access_fd(पूर्णांक fd, पूर्णांक mode) {
        /* Like access() but operates on an already open fd */

        जर (fd == AT_FDCWD)
                परत RET_NERRNO(access(".", mode));
        जर (fd == XAT_FDROOT)
                परत RET_NERRNO(access("/", mode));

        प्रतिज्ञा(fd >= 0);

        परत RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

पूर्णांक touch_fd(पूर्णांक fd, usec_t stamp) {
        प्रतिज्ञा(fd >= 0);

        जर (stamp == USEC_INFINITY)
                परत futimens_opath(fd, /* ts= */ रिक्त);

        संरचना timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        परत futimens_opath(fd, ts);
}

पूर्णांक touch_file(स्थिरांक अक्षर *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक ret;

        प्रतिज्ञा(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        जर (parents)
                (शून्य) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = खोलो(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        जर (fd < 0) {
                जर (त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = खोलो(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                जर (fd < 0)
                        परत -त्रुटि_सं;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        परत RET_GATHER(ret, touch_fd(fd, stamp));
}

पूर्णांक touch(स्थिरांक अक्षर *path) {
        परत touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

पूर्णांक symlinkat_idempotent(स्थिरांक अक्षर *target, पूर्णांक atfd, स्थिरांक अक्षर *linkpath, bool make_relative) {
        _cleanup_free_ अक्षर *relpath = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(linkpath);

        जर (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                जर (r < 0)
                        परत r;

                target = relpath;
        }

        जर (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ अक्षर *p = रिक्त;

                जर (त्रुटि_सं != EEXIST)
                        परत -त्रुटि_सं;

                r = readlinkat_malloc(atfd, linkpath, &p);
                जर (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        परत -EEXIST;
                जर (r < 0) /* Any other error? In that case propagate it as is */
                        परत r;

                जर (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        परत -EEXIST;
        }

        परत 0;
}

पूर्णांक symlinkat_atomic_full_label(स्थिरांक अक्षर *target, पूर्णांक atfd, स्थिरांक अक्षर *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        पूर्णांक r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(linkpath);

        _cleanup_free_ अक्षर *relpath = रिक्त;
        जर (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                जर (r < 0)
                        परत r;

                target = relpath;
        }

        _cleanup_free_ अक्षर *t = रिक्त;
        r = tempfn_random(linkpath, रिक्त, &t);
        जर (r < 0)
                परत r;

        bool call_label_ops_post = false;
        जर (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                जर (r < 0)
                        परत r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        जर (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        जर (r < 0)
                परत r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        जर (r < 0) {
                (शून्य) unlinkat(atfd, t, 0);
                परत r;
        }

        परत 0;
}

पूर्णांक mknodat_atomic(पूर्णांक atfd, स्थिरांक अक्षर *path, mode_t mode, dev_t dev) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);

        r = tempfn_random(path, रिक्त, &t);
        जर (r < 0)
                परत r;

        जर (mknodat(atfd, t, mode, dev) < 0)
                परत -त्रुटि_सं;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        जर (r < 0) {
                (शून्य) unlinkat(atfd, t, 0);
                परत r;
        }

        परत 0;
}

पूर्णांक mkfifoat_atomic(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode) {
        _cleanup_free_ अक्षर *t = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, रिक्त, &t);
        जर (r < 0)
                परत r;

        जर (mkfifoat(dir_fd, t, mode) < 0)
                परत -त्रुटि_सं;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        जर (r < 0) {
                (शून्य) unlinkat(dir_fd, t, 0);
                परत r;
        }

        परत 0;
}

पूर्णांक get_files_in_directory(स्थिरांक अक्षर *path, अक्षर ***ret_list) {
        _cleanup_strv_free_ अक्षर **l = रिक्त;
        _cleanup_closedir_ सूची *d = रिक्त;
        माप_प्रकार n = 0;

        प्रतिज्ञा(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = सूची_खोलो(path);
        जर (!d)
                परत -त्रुटि_सं;

        FOREACH_DIRENT_ALL(de, d, परत -त्रुटि_सं) {
                जर (!dirent_is_file(de))
                        सुरूठेवा;

                जर (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        जर (!GREEDY_REALLOC(l, n + 2))
                                परत -ENOMEM;

                        l[n] = strdup(de->d_name);
                        जर (!l[n])
                                परत -ENOMEM;

                        l[++n] = रिक्त;
                } अन्यथा
                        n++;
        }

        जर (ret_list)
                *ret_list = TAKE_PTR(l);

        परत n;
}

स्थिर पूर्णांक getenv_tmp_dir(स्थिरांक अक्षर **ret_path) {
        पूर्णांक r, ret = 0;

        प्रतिज्ञा(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                स्थिरांक अक्षर *e;

                e = secure_getenv(n);
                जर (!e)
                        सुरूठेवा;
                जर (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        जा next;
                }
                जर (!path_is_normalized(e)) {
                        r = -EPERM;
                        जा next;
                }

                r = is_dir(e, true);
                जर (r < 0)
                        जा next;
                जर (r == 0) {
                        r = -ENOTDIR;
                        जा next;
                }

                *ret_path = e;
                परत 1;

        next:
                /* Remember first error, to make this more debuggable */
                जर (ret >= 0)
                        ret = r;
        }

        जर (ret < 0)
                परत ret;

        *ret_path = रिक्त;
        परत ret;
}

स्थिर पूर्णांक tmp_dir_internal(स्थिरांक अक्षर *def, स्थिरांक अक्षर **ret) {
        स्थिरांक अक्षर *e;
        पूर्णांक r, k;

        प्रतिज्ञा(def);
        प्रतिज्ञा(ret);

        r = getenv_tmp_dir(&e);
        जर (r > 0) {
                *ret = e;
                परत 0;
        }

        k = is_dir(def, /* follow= */ true);
        जर (k == 0)
                k = -ENOTDIR;
        जर (k < 0)
                परत RET_GATHER(r, k);

        *ret = def;
        परत 0;
}

पूर्णांक var_tmp_dir(स्थिरांक अक्षर **ret) {
        प्रतिज्ञा(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        परत tmp_dir_internal("/var/tmp", ret);
}

पूर्णांक tmp_dir(स्थिरांक अक्षर **ret) {
        प्रतिज्ञा(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        परत tmp_dir_internal("/tmp", ret);
}

पूर्णांक unlink_or_warn(स्थिरांक अक्षर *filename) {
        प्रतिज्ञा(filename);

        जर (unlink(filename) < 0 && त्रुटि_सं != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                जर (त्रुटि_सं != EROFS || access(filename, F_OK) >= 0)
                        परत log_error_errno(त्रुटि_सं, "Failed to remove \"%s\": %m", filename);

        परत 0;
}

अक्षर *rmdir_and_free(अक्षर *p) {
        PROTECT_ERRNO;

        जर (!p)
                परत रिक्त;

        (शून्य) सूची_हटाओ(p);
        परत mfree(p);
}

अक्षर* unlink_and_free(अक्षर *p) {
        PROTECT_ERRNO;

        जर (!p)
                परत रिक्त;

        (शून्य) unlink(p);
        परत mfree(p);
}

पूर्णांक unlinkat_deallocate(पूर्णांक fd, स्थिरांक अक्षर *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ पूर्णांक truncate_fd = -EBADF;
        संरचना stat st;
        off_t l, bs;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा(name);
        प्रतिज्ञा((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        जर (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                जर (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        जर (IN_SET(त्रुटि_सं, ENOENT, EISDIR))
                                परत -त्रुटि_सं;

                        जर (त्रुटि_सं != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(त्रुटि_सं, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        जर (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                परत -त्रुटि_सं;

        जर (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                परत 0;

        जर (ख_स्थिति(truncate_fd, &st) < 0) {
                log_debug_errno(त्रुटि_सं, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                परत 0;
        }

        जर (!S_ISREG(st.st_mode))
                परत 0;

        जर (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t डावा = st.st_size;
                अक्षर buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, आकार(buffer));

                पर्यंत (डावा > 0) {
                        ssize_t n;

                        n = लिख(truncate_fd, buffer, MIN(आकार(buffer), डावा));
                        जर (n < 0) {
                                log_debug_errno(त्रुटि_सं, "Failed to erase data in file '%s', ignoring.", name);
                                थांबा;
                        }

                        प्रतिज्ञा(डावा >= (माप_प्रकार) n);
                        डावा -= n;
                }

                /* Let's refresh metadata */
                जर (ख_स्थिति(truncate_fd, &st) < 0) {
                        log_debug_errno(त्रुटि_सं, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        परत 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        जर (st.st_blocks == 0 || st.st_nlink > 0)
                परत 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        जर (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                परत 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        जर (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(त्रुटि_सं, "Failed to truncate file to 0, ignoring: %m");
                परत 0;
        }

        परत 0;
}

पूर्णांक open_parent_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, पूर्णांक flags, mode_t mode) {
        _cleanup_free_ अक्षर *parent = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);

        r = path_extract_directory(path, &parent);
        जर (r == -EDESTADDRREQ) {
                parent = strdup(".");
                जर (!parent)
                        परत -ENOMEM;
        } अन्यथा जर (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                जर (!parent)
                        परत -ENOMEM;
        } अन्यथा जर (r < 0)
                परत r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        जर (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        परत RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

पूर्णांक conservative_renameat(
                पूर्णांक olddirfd, स्थिरांक अक्षर *oldpath,
                पूर्णांक newdirfd, स्थिरांक अक्षर *newpath) {

        _cleanup_close_ पूर्णांक old_fd = -EBADF, new_fd = -EBADF;
        संरचना stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        जर (old_fd < 0)
                जा do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        जर (new_fd < 0)
                जा do_rename;

        जर (ख_स्थिति(old_fd, &old_stat) < 0)
                जा do_rename;

        जर (!S_ISREG(old_stat.st_mode))
                जा do_rename;

        जर (ख_स्थिति(new_fd, &new_stat) < 0)
                जा do_rename;

        जर (stat_inode_same(&new_stat, &old_stat))
                जा is_same;

        जर (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                जा do_rename;

        साठी (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[आकार(buf1)];
                ssize_t l1, l2;

                l1 = पठ(old_fd, buf1, आकार(buf1));
                जर (l1 < 0)
                        जा do_rename;

                जर (l1 == आकार(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = पठ(new_fd, buf2, l1);
                अन्यथा {
                        प्रतिज्ञा((माप_प्रकार) l1 < आकार(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        प्रतिज्ञा((माप_प्रकार) (l1 + 1) <= आकार(buf2));
                        l2 = पठ(new_fd, buf2, l1 + 1);
                }
                जर (l2 != l1)
                        जा do_rename;

                जर (स_भेद(buf1, buf2, l1) != 0)
                        जा do_rename;

                जर ((माप_प्रकार) l1 < आकार(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        थांबा;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        जर (unlinkat(olddirfd, oldpath, 0) < 0)
                जा do_rename;

        परत 0;

do_rename:
        जर (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                परत -त्रुटि_सं;

        परत 1;
}

पूर्णांक posix_fallocate_loop(पूर्णांक fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        पूर्णांक r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        जर (r != EINTR)
                परत -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (स्थिरांक RateLimit) { 10 * USEC_PER_SEC, 16 };
        पर्यंत (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                जर (r != EINTR)
                        परत -r;
        }

        परत -EINTR;
}

पूर्णांक parse_cifs_service(
                स्थिरांक अक्षर *s,
                अक्षर **ret_host,
                अक्षर **ret_service,
                अक्षर **ret_path) {

        _cleanup_free_ अक्षर *h = रिक्त, *ss = रिक्त, *x = रिक्त;
        स्थिरांक अक्षर *p, *e, *d;
        अक्षर delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        जर (!s)
                परत -EINVAL;

        p = startswith(s, "//");
        जर (!p) {
                p = startswith(s, "\\\\");
                जर (!p)
                        परत -EINVAL;
        }

        delimiter = s[0];
        e = म_अक्षर(p, delimiter);
        जर (!e)
                परत -EINVAL;

        h = strndup(p, e - p);
        जर (!h)
                परत -ENOMEM;

        जर (!hostname_is_valid(h, 0))
                परत -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        जर (!ss)
                परत -ENOMEM;

        जर (!filename_is_valid(ss))
                परत -EINVAL;

        जर (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                जर (!x)
                        परत -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                साठी (अक्षर *i = x; *i; i++)
                        जर (*i == delimiter)
                                *i = '/';

                जर (!path_is_valid(x))
                        परत -EINVAL;

                path_simplify(x);
                जर (!path_is_normalized(x))
                        परत -EINVAL;
        }

        जर (ret_host)
                *ret_host = TAKE_PTR(h);
        जर (ret_service)
                *ret_service = TAKE_PTR(ss);
        जर (ret_path)
                *ret_path = TAKE_PTR(x);

        परत 0;
}

पूर्णांक open_mkdir_at_full_label(पूर्णांक dirfd, स्थिरांक अक्षर *path, पूर्णांक flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ पूर्णांक fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ अक्षर *fname = रिक्त, *parent = रिक्त;
        पूर्णांक r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        जर (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                परत -EINVAL;
        जर ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                परत -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        जर (r < 0) {
                जर (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        परत r;
        } अन्यथा {
                r = path_extract_filename(path, &fname);
                जर (r < 0)
                        परत r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                जर (parent_fd < 0)
                        परत -त्रुटि_सं;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        जर (IN_SET(fd, -ELOOP, -ENOTDIR))
                परत -EEXIST;
        जर (fd < 0)
                परत fd;

        परत TAKE_FD(fd);
}

पूर्णांक openat_report_new(पूर्णांक dirfd, स्थिरांक अक्षर *pathname, पूर्णांक flags, mode_t mode, bool *ret_newly_created) {
        पूर्णांक fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        जर (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                जर (fd < 0)
                        परत -त्रुटि_सं;

                जर (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                परत fd;
        }

        साठी (अचिन्हित attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                जर (fd >= 0) {
                        जर (ret_newly_created)
                                *ret_newly_created = false;
                        परत fd;
                }
                जर (त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                जर (fd >= 0) {
                        जर (ret_newly_created)
                                *ret_newly_created = true;
                        परत fd;
                }
                जर (त्रुटि_सं != EEXIST)
                        परत -त्रुटि_सं;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                जर (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        परत -EEXIST;
        }
}

स्थिर पूर्णांक openat_with_automount(पूर्णांक dir_fd, स्थिरांक अक्षर *path, पूर्णांक open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        स्थिर bool can_open_वृक्ष = true;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(path);
        प्रतिज्ञा(FLAGS_SET(open_flags, O_PATH));

        जर (can_open_वृक्ष) {
                पूर्णांक fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                जर (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        जर (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                पूर्णांक q = fd_verify_directory(fd);
                                जर (q < 0) {
                                        safe_close(fd);
                                        परत q;
                                }
                        }

                        परत fd;
                }
                जर (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        परत fd;

                can_open_वृक्ष = false;
        }

        परत RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

पूर्णांक xopenat_full_label(पूर्णांक dir_fd, स्थिरांक अक्षर *path, पूर्णांक open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        bool made_dir = false, made_file = false;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        प्रतिज्ञा(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        प्रतिज्ञा(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        प्रतिज्ञा(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        प्रतिज्ञा(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        प्रतिज्ञा(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        प्रतिज्ञा((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        प्रतिज्ञा(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        जर (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        जर (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                जर (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } अन्यथा जर (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        परत -EINVAL;
        }

        जर (isempty(path)) {
                प्रतिज्ञा(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                जर (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        जर (r < 0)
                                परत r;
                }

                जर (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        जर (r < 0)
                                परत r;
                }

                जर (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        जर (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                परत TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                परत fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ पूर्णांक _dir_fd = -EBADF;
        जर (dir_fd == XAT_FDROOT) {
                जर (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                अन्यथा {
                        _dir_fd = खोलो("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        जर (_dir_fd < 0)
                                परत -त्रुटि_सं;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        जर (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                जर (r < 0)
                        परत r;

                call_label_ops_post = true;
        }

        जर (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                जर (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                अन्यथा
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                जर (r == -EEXIST) {
                        जर (FLAGS_SET(open_flags, O_EXCL))
                                परत -EEXIST;
                } अन्यथा जर (r < 0)
                        परत r;
                अन्यथा
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        जर (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                जर (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        जर (fd < 0) {
                                r = fd;
                                जा error;
                        }

                        r = fd_verify_regular(fd);
                        जर (r < 0)
                                जा error;

                } अन्यथा जर (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        जर (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                जर (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                अन्यथा जर (fd < 0) {
                                        r = fd;
                                        जा error;
                                }
                        }

                        जर (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                जर (fd < 0) {
                                        r = -त्रुटि_सं;
                                        जा error;
                                }
                        }

                        made_file = true;
                } अन्यथा {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ पूर्णांक inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        जर (inode_fd < 0) {
                                जर (त्रुटि_सं != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -त्रुटि_सं;
                                        जा error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                जर (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        जर (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        अन्यथा जर (fd < 0) {
                                                r = fd;
                                                जा error;
                                        }
                                }

                                जर (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        जर (fd < 0) {
                                                r = -त्रुटि_सं;
                                                जा error;
                                        }
                                }

                                made_file = true;
                        } अन्यथा {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                जर (r < 0)
                                        जा error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                जर (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        जर (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        अन्यथा जर (fd < 0) {
                                                r = fd;
                                                जा error;
                                        }
                                }

                                जर (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        जर (fd < 0) {
                                                r = fd;
                                                जा error;
                                        }
                                }
                        }
                }
        } अन्यथा जर (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                जर (fd < 0) {
                        r = fd;
                        जा error;
                }
        } अन्यथा {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                जर (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        जर (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        अन्यथा जर (fd < 0) {
                                r = fd;
                                जा error;
                        }
                }

                जर (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        जर (fd < 0) {
                                r = fd;
                                जा error;
                        }
                }
        }

        जर (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                जर (r < 0)
                        जा error;
        }

        जर (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ रिक्त, made_file || made_dir, label_context);
                जर (r < 0)
                        जा error;
        }

        जर (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                जर (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        जा error;
        }

        परत TAKE_FD(fd);

error:
        जर (call_label_ops_post)
                (शून्य) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? रिक्त : path, made_dir || made_file, label_context);

        जर (made_dir || made_file)
                (शून्य) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        परत r;
}

पूर्णांक xopenat_lock_full_label(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *path,
                पूर्णांक open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                पूर्णांक operation,
                LabelContext *label_context) {

        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        जर (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                परत -EBADF;

        साठी (;;) {
                संरचना stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                जर (fd < 0)
                        परत fd;

                r = lock_generic(fd, locktype, operation);
                जर (r < 0)
                        परत r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                जर (ख_स्थिति(fd, &st) < 0)
                        परत -त्रुटि_सं;
                जर (st.st_nlink > 0)
                        थांबा;

                fd = safe_close(fd);
        }

        परत TAKE_FD(fd);
}

पूर्णांक link_fd(पूर्णांक fd, पूर्णांक newdirfd, स्थिरांक अक्षर *newpath) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(newdirfd >= 0 || newdirfd == AT_FDCWD);
        प्रतिज्ञा(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        जर (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                जर (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        परत -ENOSYS;
        }

        परत r;
}

पूर्णांक linkat_replace(पूर्णांक olddirfd, स्थिरांक अक्षर *oldpath, पूर्णांक newdirfd, स्थिरांक अक्षर *newpath) {
        _cleanup_close_ पूर्णांक old_fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(olddirfd >= 0 || olddirfd == AT_FDCWD);
        प्रतिज्ञा(newdirfd >= 0 || newdirfd == AT_FDCWD);
        प्रतिज्ञा(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        जर (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                परत -EISDIR;

        जर (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                परत -EISDIR;

        जर (path_implies_directory(newpath))
                परत -EISDIR;

        /* First, try to link this directly */
        जर (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        अन्यथा
                r = link_fd(olddirfd, newdirfd, newpath);
        जर (r >= 0)
                परत 0;
        जर (r != -EEXIST)
                परत r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        जर (old_fd < 0)
                परत old_fd;

        संरचना stat old_st;
        जर (ख_स्थिति(old_fd, &old_st) < 0)
                परत -त्रुटि_सं;

        जर (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                परत -EISDIR;

        संरचना stat new_st;
        जर (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                परत -त्रुटि_सं;

        जर (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                परत -EEXIST;

        जर (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                परत 0;

        _cleanup_free_ अक्षर *tmp_path = रिक्त;
        r = tempfn_random(newpath, /* extra= */ रिक्त, &tmp_path);
        जर (r < 0)
                परत r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        जर (r < 0) {
                जर (!ERRNO_IS_PRIVILEGE(r))
                        परत r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                जर (r < 0)
                        परत r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        जर (r < 0) {
                (शून्य) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                परत r;
        }

        परत 0;
}
