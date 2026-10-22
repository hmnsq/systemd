/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/falloc.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/file.h>
#അന്തര്ഭാവ <sys/mount.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "btrfs-util.h"
#അന്തര്ഭാവ "chattr-util.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "hostname-util.h"
#അന്തര്ഭാവ "label-util.h"
#അന്തര്ഭാവ "lock-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mkdir.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "random-util.h"
#അന്തര്ഭാവ "ratelimit.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "tmpfile-util.h"
#അന്തര്ഭാവ "umask-util.h"

പൂർണ്ണസംഖ്യ rmdir_parents(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *stop) {
        അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(stop);

        എങ്കിൽ (!path_is_safe(path))
                തിരികെ -EINVAL;

        എങ്കിൽ (!path_is_safe(stop))
                തിരികെ -EINVAL;

        p = strdupa_safe(path);

        വേണ്ടി (;;) {
                അക്ഷരം *slash = ശൂന്യ;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (സ്ഥിരം അക്ഷരം **) &slash, ശൂന്യ);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;
                എങ്കിൽ (slash == p)
                        തിരികെ 0;

                പ്രതിജ്ഞാ(*slash == '/');
                *slash = '\0';

                എങ്കിൽ (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        തിരികെ 0;

                എങ്കിൽ (rmdir(p) < 0 && errno != ENOENT)
                        തിരികെ -errno;
        }
}

പൂർണ്ണസംഖ്യ rename_noreplace(പൂർണ്ണസംഖ്യ olddirfd, സ്ഥിരം അക്ഷരം *oldpath, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        പ്രതിജ്ഞാ(oldpath);
        പ്രതിജ്ഞാ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        പ്രതിജ്ഞാ(newpath);

        /* Try the ideal approach first */
        എങ്കിൽ (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                തിരികെ 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                തിരികെ -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        എങ്കിൽ (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                എങ്കിൽ (r < 0) {
                        (ശൂന്യം) unlinkat(newdirfd, newpath, 0);
                        തിരികെ r;
                }

                തിരികെ 0;
        }

        എങ്കിൽ (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                തിരികെ -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        എങ്കിൽ (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                തിരികെ -EEXIST;
        എങ്കിൽ (errno != ENOENT)
                തിരികെ -errno;

        തിരികെ RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

പൂർണ്ണസംഖ്യ readlinkat_malloc(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret) {
        size_t l = PATH_MAX;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);

        എങ്കിൽ (fd < 0 && isempty(p))
                തിരികെ -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *c = ശൂന്യ;
                ssize_t n;

                c = new(അക്ഷരം, l+1);
                എങ്കിൽ (!c)
                        തിരികെ -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;

                എങ്കിൽ ((size_t) n < l) {
                        c[n] = 0;

                        എങ്കിൽ (ret)
                                *ret = TAKE_PTR(c);

                        തിരികെ 0;
                }

                എങ്കിൽ (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        തിരികെ -EFBIG;

                l *= 2;
        }
}

പൂർണ്ണസംഖ്യ readlink_മൂല്യം(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *link = ശൂന്യ, *name = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        r = readlink_malloc(p, &link);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = path_extract_filename(link, &name);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == O_DIRECTORY)
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(name);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ readlink_and_make_absolute(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *target = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        r = readlink_malloc(p, &target);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ file_in_same_dir(p, target, ret);
}

പൂർണ്ണസംഖ്യ chmod_and_chown_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        എങ്കിൽ (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;
                dir_fd = fd;

        } അല്ലെങ്കിൽ എങ്കിൽ (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;
                dir_fd = fd;
        }

        തിരികെ fchmod_and_chown(dir_fd, mode, uid, gid);
}

പൂർണ്ണസംഖ്യ fchmod_and_chown_with_fallback(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        ഘടന stat st;
        പൂർണ്ണസംഖ്യ r;

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

        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        എങ്കിൽ (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        അല്ലെങ്കിൽ എങ്കിൽ ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                തിരികെ -EINVAL; /* insist on the right file type if it was specified */

        എങ്കിൽ (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                എങ്കിൽ (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        എങ്കിൽ (r < 0) {
                                എങ്കിൽ (!path || r != -ENOSYS)
                                        തിരികെ r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                എങ്കിൽ (chmod(path, minimal & 07777) < 0)
                                        തിരികെ -errno;
                        }
                }
        }

        എങ്കിൽ (do_chown)
                എങ്കിൽ (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        തിരികെ -errno;

        എങ്കിൽ (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                എങ്കിൽ (r < 0) {
                        എങ്കിൽ (!path || r != -ENOSYS)
                                തിരികെ r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        എങ്കിൽ (chmod(path, mode & 07777) < 0)
                                തിരികെ -errno;
                }
        }

        തിരികെ do_chown || do_chmod;
}

പൂർണ്ണസംഖ്യ fchmod_umask(പൂർണ്ണസംഖ്യ fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        തിരികെ RET_NERRNO(fchmod(fd, m & (~u)));
}

പൂർണ്ണസംഖ്യ fchmod_opath(പൂർണ്ണസംഖ്യ fd, mode_t m) {
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

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                തിരികെ 0;
        എങ്കിൽ (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                തിരികെ 0;
        എങ്കിൽ (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                തിരികെ -errno;

        എങ്കിൽ (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                എങ്കിൽ (errno != ENOENT)
                        തിരികെ -errno;

                തിരികെ proc_fd_enoent_errno();
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ futimens_opath(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ഘടന timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        പ്രതിജ്ഞാ(fd >= 0);

        തിരികെ RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

പൂർണ്ണസംഖ്യ stat_warn_permissions(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം ഘടന stat *st) {
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        എങ്കിൽ (!S_ISREG(st->st_mode))
                തിരികെ 0;

        എങ്കിൽ (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        എങ്കിൽ (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        എങ്കിൽ (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_warn_permissions(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ fd) {
        ഘടന stat st;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        തിരികെ stat_warn_permissions(path, &st);
}

പൂർണ്ണസംഖ്യ access_nofollow(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ mode) {
        തിരികെ RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

പൂർണ്ണസംഖ്യ access_fd(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ mode) {
        /* Like access() but operates on an already open fd */

        എങ്കിൽ (fd == AT_FDCWD)
                തിരികെ RET_NERRNO(access(".", mode));
        എങ്കിൽ (fd == XAT_FDROOT)
                തിരികെ RET_NERRNO(access("/", mode));

        പ്രതിജ്ഞാ(fd >= 0);

        തിരികെ RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

പൂർണ്ണസംഖ്യ touch_fd(പൂർണ്ണസംഖ്യ fd, usec_t stamp) {
        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (stamp == USEC_INFINITY)
                തിരികെ futimens_opath(fd, /* ts= */ ശൂന്യ);

        ഘടന timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        തിരികെ futimens_opath(fd, ts);
}

പൂർണ്ണസംഖ്യ touch_file(സ്ഥിരം അക്ഷരം *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ ret;

        പ്രതിജ്ഞാ(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        എങ്കിൽ (parents)
                (ശൂന്യം) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        എങ്കിൽ (fd < 0) {
                എങ്കിൽ (errno != ENOENT)
                        തിരികെ -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        തിരികെ RET_GATHER(ret, touch_fd(fd, stamp));
}

പൂർണ്ണസംഖ്യ touch(സ്ഥിരം അക്ഷരം *path) {
        തിരികെ touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

പൂർണ്ണസംഖ്യ symlinkat_idempotent(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *linkpath, bool make_relative) {
        _cleanup_free_ അക്ഷരം *relpath = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(target);
        പ്രതിജ്ഞാ(linkpath);

        എങ്കിൽ (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                target = relpath;
        }

        എങ്കിൽ (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                എങ്കിൽ (errno != EEXIST)
                        തിരികെ -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                എങ്കിൽ (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        തിരികെ -EEXIST;
                എങ്കിൽ (r < 0) /* Any other error? In that case propagate it as is */
                        തിരികെ r;

                എങ്കിൽ (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        തിരികെ -EEXIST;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ symlinkat_atomic_full_label(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(target);
        പ്രതിജ്ഞാ(linkpath);

        _cleanup_free_ അക്ഷരം *relpath = ശൂന്യ;
        എങ്കിൽ (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                target = relpath;
        }

        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        r = tempfn_random(linkpath, ശൂന്യ, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        bool call_label_ops_post = false;
        എങ്കിൽ (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        എങ്കിൽ (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        എങ്കിൽ (r < 0) {
                (ശൂന്യം) unlinkat(atfd, t, 0);
                തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ mknodat_atomic(പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *path, mode_t mode, dev_t dev) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        r = tempfn_random(path, ശൂന്യ, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (mknodat(atfd, t, mode, dev) < 0)
                തിരികെ -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        എങ്കിൽ (r < 0) {
                (ശൂന്യം) unlinkat(atfd, t, 0);
                തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ mkfifoat_atomic(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode) {
        _cleanup_free_ അക്ഷരം *t = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, ശൂന്യ, &t);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (mkfifoat(dir_fd, t, mode) < 0)
                തിരികെ -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        എങ്കിൽ (r < 0) {
                (ശൂന്യം) unlinkat(dir_fd, t, 0);
                തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_files_in_directory(സ്ഥിരം അക്ഷരം *path, അക്ഷരം ***ret_list) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        _cleanup_closedir_ DIR *d = ശൂന്യ;
        size_t n = 0;

        പ്രതിജ്ഞാ(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        എങ്കിൽ (!d)
                തിരികെ -errno;

        FOREACH_DIRENT_ALL(de, d, തിരികെ -errno) {
                എങ്കിൽ (!dirent_is_file(de))
                        തുടരുക;

                എങ്കിൽ (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        എങ്കിൽ (!GREEDY_REALLOC(l, n + 2))
                                തിരികെ -ENOMEM;

                        l[n] = strdup(de->d_name);
                        എങ്കിൽ (!l[n])
                                തിരികെ -ENOMEM;

                        l[++n] = ശൂന്യ;
                } അല്ലെങ്കിൽ
                        n++;
        }

        എങ്കിൽ (ret_list)
                *ret_list = TAKE_PTR(l);

        തിരികെ n;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ getenv_tmp_dir(സ്ഥിരം അക്ഷരം **ret_path) {
        പൂർണ്ണസംഖ്യ r, ret = 0;

        പ്രതിജ്ഞാ(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                സ്ഥിരം അക്ഷരം *e;

                e = secure_getenv(n);
                എങ്കിൽ (!e)
                        തുടരുക;
                എങ്കിൽ (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        പോകുക next;
                }
                എങ്കിൽ (!path_is_normalized(e)) {
                        r = -EPERM;
                        പോകുക next;
                }

                r = is_dir(e, true);
                എങ്കിൽ (r < 0)
                        പോകുക next;
                എങ്കിൽ (r == 0) {
                        r = -ENOTDIR;
                        പോകുക next;
                }

                *ret_path = e;
                തിരികെ 1;

        next:
                /* Remember first error, to make this more debuggable */
                എങ്കിൽ (ret >= 0)
                        ret = r;
        }

        എങ്കിൽ (ret < 0)
                തിരികെ ret;

        *ret_path = ശൂന്യ;
        തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tmp_dir_internal(സ്ഥിരം അക്ഷരം *def, സ്ഥിരം അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ r, k;

        പ്രതിജ്ഞാ(def);
        പ്രതിജ്ഞാ(ret);

        r = getenv_tmp_dir(&e);
        എങ്കിൽ (r > 0) {
                *ret = e;
                തിരികെ 0;
        }

        k = is_dir(def, /* follow= */ true);
        എങ്കിൽ (k == 0)
                k = -ENOTDIR;
        എങ്കിൽ (k < 0)
                തിരികെ RET_GATHER(r, k);

        *ret = def;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ var_tmp_dir(സ്ഥിരം അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        തിരികെ tmp_dir_internal("/var/tmp", ret);
}

പൂർണ്ണസംഖ്യ tmp_dir(സ്ഥിരം അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        തിരികെ tmp_dir_internal("/tmp", ret);
}

പൂർണ്ണസംഖ്യ unlink_or_warn(സ്ഥിരം അക്ഷരം *filename) {
        പ്രതിജ്ഞാ(filename);

        എങ്കിൽ (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                എങ്കിൽ (errno != EROFS || access(filename, F_OK) >= 0)
                        തിരികെ log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        തിരികെ 0;
}

അക്ഷരം *rmdir_and_free(അക്ഷരം *p) {
        PROTECT_ERRNO;

        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        (ശൂന്യം) rmdir(p);
        തിരികെ mfree(p);
}

അക്ഷരം* unlink_and_free(അക്ഷരം *p) {
        PROTECT_ERRNO;

        എങ്കിൽ (!p)
                തിരികെ ശൂന്യ;

        (ശൂന്യം) unlink(p);
        തിരികെ mfree(p);
}

പൂർണ്ണസംഖ്യ unlinkat_deallocate(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ truncate_fd = -EBADF;
        ഘടന stat st;
        off_t l, bs;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        എങ്കിൽ (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                എങ്കിൽ (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        എങ്കിൽ (IN_SET(errno, ENOENT, EISDIR))
                                തിരികെ -errno;

                        എങ്കിൽ (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        എങ്കിൽ (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                തിരികെ -errno;

        എങ്കിൽ (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                തിരികെ 0;

        എങ്കിൽ (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                തിരികെ 0;
        }

        എങ്കിൽ (!S_ISREG(st.st_mode))
                തിരികെ 0;

        എങ്കിൽ (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t ഇടത് = st.st_size;
                അക്ഷരം buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, വലുപ്പം(buffer));

                ആകുമ്പോൾ (ഇടത് > 0) {
                        ssize_t n;

                        n = ലിഖ(truncate_fd, buffer, MIN(വലുപ്പം(buffer), ഇടത്));
                        എങ്കിൽ (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                നിർത്തുക;
                        }

                        പ്രതിജ്ഞാ(ഇടത് >= (size_t) n);
                        ഇടത് -= n;
                }

                /* Let's refresh metadata */
                എങ്കിൽ (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        തിരികെ 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        എങ്കിൽ (st.st_blocks == 0 || st.st_nlink > 0)
                തിരികെ 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        എങ്കിൽ (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                തിരികെ 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        എങ്കിൽ (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                തിരികെ 0;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ open_parent_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, mode_t mode) {
        _cleanup_free_ അക്ഷരം *parent = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(path);

        r = path_extract_directory(path, &parent);
        എങ്കിൽ (r == -EDESTADDRREQ) {
                parent = strdup(".");
                എങ്കിൽ (!parent)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ എങ്കിൽ (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                എങ്കിൽ (!parent)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        എങ്കിൽ (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        തിരികെ RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

പൂർണ്ണസംഖ്യ conservative_renameat(
                പൂർണ്ണസംഖ്യ olddirfd, സ്ഥിരം അക്ഷരം *oldpath,
                പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ old_fd = -EBADF, new_fd = -EBADF;
        ഘടന stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        എങ്കിൽ (old_fd < 0)
                പോകുക do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        എങ്കിൽ (new_fd < 0)
                പോകുക do_rename;

        എങ്കിൽ (fstat(old_fd, &old_stat) < 0)
                പോകുക do_rename;

        എങ്കിൽ (!S_ISREG(old_stat.st_mode))
                പോകുക do_rename;

        എങ്കിൽ (fstat(new_fd, &new_stat) < 0)
                പോകുക do_rename;

        എങ്കിൽ (stat_inode_same(&new_stat, &old_stat))
                പോകുക is_same;

        എങ്കിൽ (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                പോകുക do_rename;

        വേണ്ടി (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[വലുപ്പം(buf1)];
                ssize_t l1, l2;

                l1 = പഠ(old_fd, buf1, വലുപ്പം(buf1));
                എങ്കിൽ (l1 < 0)
                        പോകുക do_rename;

                എങ്കിൽ (l1 == വലുപ്പം(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = പഠ(new_fd, buf2, l1);
                അല്ലെങ്കിൽ {
                        പ്രതിജ്ഞാ((size_t) l1 < വലുപ്പം(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        പ്രതിജ്ഞാ((size_t) (l1 + 1) <= വലുപ്പം(buf2));
                        l2 = പഠ(new_fd, buf2, l1 + 1);
                }
                എങ്കിൽ (l2 != l1)
                        പോകുക do_rename;

                എങ്കിൽ (memcmp(buf1, buf2, l1) != 0)
                        പോകുക do_rename;

                എങ്കിൽ ((size_t) l1 < വലുപ്പം(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        നിർത്തുക;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        എങ്കിൽ (unlinkat(olddirfd, oldpath, 0) < 0)
                പോകുക do_rename;

        തിരികെ 0;

do_rename:
        എങ്കിൽ (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                തിരികെ -errno;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ posix_fallocate_loop(പൂർണ്ണസംഖ്യ fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        പൂർണ്ണസംഖ്യ r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        എങ്കിൽ (r != EINTR)
                തിരികെ -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (സ്ഥിരം RateLimit) { 10 * USEC_PER_SEC, 16 };
        ആകുമ്പോൾ (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                എങ്കിൽ (r != EINTR)
                        തിരികെ -r;
        }

        തിരികെ -EINTR;
}

പൂർണ്ണസംഖ്യ parse_cifs_service(
                സ്ഥിരം അക്ഷരം *s,
                അക്ഷരം **ret_host,
                അക്ഷരം **ret_service,
                അക്ഷരം **ret_path) {

        _cleanup_free_ അക്ഷരം *h = ശൂന്യ, *ss = ശൂന്യ, *x = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *p, *e, *d;
        അക്ഷരം delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        എങ്കിൽ (!s)
                തിരികെ -EINVAL;

        p = startswith(s, "//");
        എങ്കിൽ (!p) {
                p = startswith(s, "\\\\");
                എങ്കിൽ (!p)
                        തിരികെ -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        എങ്കിൽ (!e)
                തിരികെ -EINVAL;

        h = strndup(p, e - p);
        എങ്കിൽ (!h)
                തിരികെ -ENOMEM;

        എങ്കിൽ (!hostname_is_valid(h, 0))
                തിരികെ -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        എങ്കിൽ (!ss)
                തിരികെ -ENOMEM;

        എങ്കിൽ (!filename_is_valid(ss))
                തിരികെ -EINVAL;

        എങ്കിൽ (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                എങ്കിൽ (!x)
                        തിരികെ -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                വേണ്ടി (അക്ഷരം *i = x; *i; i++)
                        എങ്കിൽ (*i == delimiter)
                                *i = '/';

                എങ്കിൽ (!path_is_valid(x))
                        തിരികെ -EINVAL;

                path_simplify(x);
                എങ്കിൽ (!path_is_normalized(x))
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (ret_host)
                *ret_host = TAKE_PTR(h);
        എങ്കിൽ (ret_service)
                *ret_service = TAKE_PTR(ss);
        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(x);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ open_mkdir_at_full_label(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ അക്ഷരം *fname = ശൂന്യ, *parent = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        എങ്കിൽ (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                തിരികെ -EINVAL;
        എങ്കിൽ ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                തിരികെ -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        തിരികെ r;
        } അല്ലെങ്കിൽ {
                r = path_extract_filename(path, &fname);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                എങ്കിൽ (parent_fd < 0)
                        തിരികെ -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        എങ്കിൽ (IN_SET(fd, -ELOOP, -ENOTDIR))
                തിരികെ -EEXIST;
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        തിരികെ TAKE_FD(fd);
}

പൂർണ്ണസംഖ്യ openat_report_new(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *pathname, പൂർണ്ണസംഖ്യ flags, mode_t mode, bool *ret_newly_created) {
        പൂർണ്ണസംഖ്യ fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        എങ്കിൽ (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;

                എങ്കിൽ (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                തിരികെ fd;
        }

        വേണ്ടി (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                എങ്കിൽ (fd >= 0) {
                        എങ്കിൽ (ret_newly_created)
                                *ret_newly_created = false;
                        തിരികെ fd;
                }
                എങ്കിൽ (errno != ENOENT)
                        തിരികെ -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                എങ്കിൽ (fd >= 0) {
                        എങ്കിൽ (ret_newly_created)
                                *ret_newly_created = true;
                        തിരികെ fd;
                }
                എങ്കിൽ (errno != EEXIST)
                        തിരികെ -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                എങ്കിൽ (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        തിരികെ -EEXIST;
        }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ openat_with_automount(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        സ്ഥിതം bool can_open_വൃക്ഷം = true;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(FLAGS_SET(open_flags, O_PATH));

        എങ്കിൽ (can_open_വൃക്ഷം) {
                പൂർണ്ണസംഖ്യ fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                എങ്കിൽ (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        എങ്കിൽ (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                പൂർണ്ണസംഖ്യ q = fd_verify_directory(fd);
                                എങ്കിൽ (q < 0) {
                                        safe_close(fd);
                                        തിരികെ q;
                                }
                        }

                        തിരികെ fd;
                }
                എങ്കിൽ (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        തിരികെ fd;

                can_open_വൃക്ഷം = false;
        }

        തിരികെ RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

പൂർണ്ണസംഖ്യ xopenat_full_label(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        bool made_dir = false, made_file = false;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        പ്രതിജ്ഞാ(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        പ്രതിജ്ഞാ(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        പ്രതിജ്ഞാ(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        പ്രതിജ്ഞാ(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        പ്രതിജ്ഞാ(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        പ്രതിജ്ഞാ((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        പ്രതിജ്ഞാ(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        എങ്കിൽ (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        എങ്കിൽ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                എങ്കിൽ (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } അല്ലെങ്കിൽ എങ്കിൽ (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (isempty(path)) {
                പ്രതിജ്ഞാ(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        എങ്കിൽ (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                തിരികെ TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                തിരികെ fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ _dir_fd = -EBADF;
        എങ്കിൽ (dir_fd == XAT_FDROOT) {
                എങ്കിൽ (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                അല്ലെങ്കിൽ {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        എങ്കിൽ (_dir_fd < 0)
                                തിരികെ -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        എങ്കിൽ (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                call_label_ops_post = true;
        }

        എങ്കിൽ (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                അല്ലെങ്കിൽ
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                എങ്കിൽ (r == -EEXIST) {
                        എങ്കിൽ (FLAGS_SET(open_flags, O_EXCL))
                                തിരികെ -EEXIST;
                } അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                        തിരികെ r;
                അല്ലെങ്കിൽ
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        എങ്കിൽ (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                എങ്കിൽ (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        എങ്കിൽ (fd < 0) {
                                r = fd;
                                പോകുക error;
                        }

                        r = fd_verify_regular(fd);
                        എങ്കിൽ (r < 0)
                                പോകുക error;

                } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        എങ്കിൽ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                എങ്കിൽ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                അല്ലെങ്കിൽ എങ്കിൽ (fd < 0) {
                                        r = fd;
                                        പോകുക error;
                                }
                        }

                        എങ്കിൽ (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                എങ്കിൽ (fd < 0) {
                                        r = -errno;
                                        പോകുക error;
                                }
                        }

                        made_file = true;
                } അല്ലെങ്കിൽ {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ പൂർണ്ണസംഖ്യ inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        എങ്കിൽ (inode_fd < 0) {
                                എങ്കിൽ (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        പോകുക error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        എങ്കിൽ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        അല്ലെങ്കിൽ എങ്കിൽ (fd < 0) {
                                                r = fd;
                                                പോകുക error;
                                        }
                                }

                                എങ്കിൽ (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        എങ്കിൽ (fd < 0) {
                                                r = -errno;
                                                പോകുക error;
                                        }
                                }

                                made_file = true;
                        } അല്ലെങ്കിൽ {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                എങ്കിൽ (r < 0)
                                        പോകുക error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        എങ്കിൽ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        അല്ലെങ്കിൽ എങ്കിൽ (fd < 0) {
                                                r = fd;
                                                പോകുക error;
                                        }
                                }

                                എങ്കിൽ (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        എങ്കിൽ (fd < 0) {
                                                r = fd;
                                                പോകുക error;
                                        }
                                }
                        }
                }
        } അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                എങ്കിൽ (fd < 0) {
                        r = fd;
                        പോകുക error;
                }
        } അല്ലെങ്കിൽ {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                എങ്കിൽ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        എങ്കിൽ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        അല്ലെങ്കിൽ എങ്കിൽ (fd < 0) {
                                r = fd;
                                പോകുക error;
                        }
                }

                എങ്കിൽ (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        എങ്കിൽ (fd < 0) {
                                r = fd;
                                പോകുക error;
                        }
                }
        }

        എങ്കിൽ (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                എങ്കിൽ (r < 0)
                        പോകുക error;
        }

        എങ്കിൽ (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ ശൂന്യ, made_file || made_dir, label_context);
                എങ്കിൽ (r < 0)
                        പോകുക error;
        }

        എങ്കിൽ (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                എങ്കിൽ (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        പോകുക error;
        }

        തിരികെ TAKE_FD(fd);

error:
        എങ്കിൽ (call_label_ops_post)
                (ശൂന്യം) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? ശൂന്യ : path, made_dir || made_file, label_context);

        എങ്കിൽ (made_dir || made_file)
                (ശൂന്യം) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ xopenat_lock_full_label(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                പൂർണ്ണസംഖ്യ operation,
                LabelContext *label_context) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        എങ്കിൽ (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                തിരികെ -EBADF;

        വേണ്ടി (;;) {
                ഘടന stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                എങ്കിൽ (fd < 0)
                        തിരികെ fd;

                r = lock_generic(fd, locktype, operation);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                എങ്കിൽ (fstat(fd, &st) < 0)
                        തിരികെ -errno;
                എങ്കിൽ (st.st_nlink > 0)
                        നിർത്തുക;

                fd = safe_close(fd);
        }

        തിരികെ TAKE_FD(fd);
}

പൂർണ്ണസംഖ്യ link_fd(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        പ്രതിജ്ഞാ(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        എങ്കിൽ (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                എങ്കിൽ (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        തിരികെ -ENOSYS;
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ linkat_replace(പൂർണ്ണസംഖ്യ olddirfd, സ്ഥിരം അക്ഷരം *oldpath, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ old_fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(olddirfd >= 0 || olddirfd == AT_FDCWD);
        പ്രതിജ്ഞാ(newdirfd >= 0 || newdirfd == AT_FDCWD);
        പ്രതിജ്ഞാ(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        എങ്കിൽ (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                തിരികെ -EISDIR;

        എങ്കിൽ (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                തിരികെ -EISDIR;

        എങ്കിൽ (path_implies_directory(newpath))
                തിരികെ -EISDIR;

        /* First, try to link this directly */
        എങ്കിൽ (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        അല്ലെങ്കിൽ
                r = link_fd(olddirfd, newdirfd, newpath);
        എങ്കിൽ (r >= 0)
                തിരികെ 0;
        എങ്കിൽ (r != -EEXIST)
                തിരികെ r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        എങ്കിൽ (old_fd < 0)
                തിരികെ old_fd;

        ഘടന stat old_st;
        എങ്കിൽ (fstat(old_fd, &old_st) < 0)
                തിരികെ -errno;

        എങ്കിൽ (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                തിരികെ -EISDIR;

        ഘടന stat new_st;
        എങ്കിൽ (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                തിരികെ -errno;

        എങ്കിൽ (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                തിരികെ -EEXIST;

        എങ്കിൽ (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                തിരികെ 0;

        _cleanup_free_ അക്ഷരം *tmp_path = ശൂന്യ;
        r = tempfn_random(newpath, /* extra= */ ശൂന്യ, &tmp_path);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (!ERRNO_IS_PRIVILEGE(r))
                        തിരികെ r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        എങ്കിൽ (r < 0) {
                (ശൂന്യം) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                തിരികെ r;
        }

        തിരികെ 0;
}
