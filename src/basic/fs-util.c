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

पूर्णाङ्क rmdir_parents(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *stop) {
        वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(stop);

        यदि (!मार्गः_is_safe(मार्गः))
                प्रत्यागम -EINVAL;

        यदि (!मार्गः_is_safe(stop))
                प्रत्यागम -EINVAL;

        p = strdupa_safe(मार्गः);

        प्रति (;;) {
                वर्ण *slash = रिक्त;

                /* skip the last component. */
                r = मार्गः_find_चरमः_component(p, /* accept_dot_dot= */ false, (ध्रुव वर्ण **) &slash, रिक्त);
                यदि (r <= 0)
                        प्रत्यागम r;
                यदि (slash == p)
                        प्रत्यागम 0;

                प्रतिज्ञा(*slash == '/');
                *slash = '\0';

                यदि (मार्गः_startswith_पूर्णम्(stop, p, मार्गः_STARTSWITH_REFUSE_DOT_DOT))
                        प्रत्यागम 0;

                यदि (rmdir(p) < 0 && errno != ENOENT)
                        प्रत्यागम -errno;
        }
}

पूर्णाङ्क rename_noreplace(पूर्णाङ्क olddirfd, ध्रुव वर्ण *oldpath, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath) {
        पूर्णाङ्क r;

        प्रतिज्ञा(olddirfd >= 0 || olddirfd == AT_FDCWD);
        प्रतिज्ञा(oldpath);
        प्रतिज्ञा(newdirfd >= 0 || newdirfd == AT_FDCWD);
        प्रतिज्ञा(newpath);

        /* Try the ideal approach first */
        यदि (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                प्रत्यागम 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        यदि (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                प्रत्यागम -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        यदि (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                यदि (r < 0) {
                        (शून्य) unlinkat(newdirfd, newpath, 0);
                        प्रत्यागम r;
                }

                प्रत्यागम 0;
        }

        यदि (!ERRNO_IS_NOT_SUPPORTED(errno) && !अन्तर्_समुच्चयः(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                प्रत्यागम -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        यदि (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                प्रत्यागम -EEXIST;
        यदि (errno != ENOENT)
                प्रत्यागम -errno;

        प्रत्यागम RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

पूर्णाङ्क readlinkat_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *p, वर्ण **ret) {
        size_t l = PATH_MAX;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);

        यदि (fd < 0 && isempty(p))
                प्रत्यागम -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        प्रति (;;) {
                _cleanup_free_ वर्ण *c = रिक्त;
                ssize_t n;

                c = new(वर्ण, l+1);
                यदि (!c)
                        प्रत्यागम -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                यदि (n < 0)
                        प्रत्यागम -errno;

                यदि ((size_t) n < l) {
                        c[n] = 0;

                        यदि (ret)
                                *ret = TAKE_PTR(c);

                        प्रत्यागम 0;
                }

                यदि (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        प्रत्यागम -EFBIG;

                l *= 2;
        }
}

पूर्णाङ्क readlink_मानम्(ध्रुव वर्ण *p, वर्ण **ret) {
        _cleanup_free_ वर्ण *link = रिक्त, *name = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        r = readlink_malloc(p, &link);
        यदि (r < 0)
                प्रत्यागम r;

        r = मार्गः_extract_filename(link, &name);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == O_DIRECTORY)
                प्रत्यागम -EINVAL;

        *ret = TAKE_PTR(name);
        प्रत्यागम 0;
}

पूर्णाङ्क readlink_and_make_absolute(ध्रुव वर्ण *p, वर्ण **ret) {
        _cleanup_free_ वर्ण *target = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        r = readlink_malloc(p, &target);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम file_अन्तर्_same_dir(p, target, ret);
}

पूर्णाङ्क chmod_and_chown_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्, uid_t uid, gid_t gid) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        यदि (मार्गः) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, मार्गः, O_मार्गः|O_CLOEXEC|O_NOFOLLOW);
                यदि (fd < 0)
                        प्रत्यागम -errno;
                dir_fd = fd;

        } अन्यथा यदि (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_मार्गः|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                यदि (fd < 0)
                        प्रत्यागम -errno;
                dir_fd = fd;
        }

        प्रत्यागम fchmod_and_chown(dir_fd, बहुलकम्, uid, gid);
}

पूर्णाङ्क fchmod_and_chown_with_fallback(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        संरचना stat st;
        पूर्णाङ्क r;

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

        यदि (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((बहुलकम् != बहुलकम्_INVALID && ((st.st_mode ^ बहुलकम्) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        यदि (बहुलकम् == बहुलकम्_INVALID)
                बहुलकम् = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        अन्यथा यदि ((बहुलकम् & S_IFMT) != 0 && ((बहुलकम् ^ st.st_mode) & S_IFMT) != 0)
                प्रत्यागम -EINVAL; /* insist on the right file type if it was specified */

        यदि (do_chown && do_chmod) {
                mode_t अल्पिष्ठम् = st.st_mode & बहुलकम्; /* the subset of the old and the new mask */

                यदि (((अल्पिष्ठम् ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, अल्पिष्ठम् & 07777);
                        यदि (r < 0) {
                                यदि (!मार्गः || r != -ENOSYS)
                                        प्रत्यागम r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                यदि (chmod(मार्गः, अल्पिष्ठम् & 07777) < 0)
                                        प्रत्यागम -errno;
                        }
                }
        }

        यदि (do_chown)
                यदि (fchownat(fd, "", uid, gid, AT_रिक्तानि_मार्गः) < 0)
                        प्रत्यागम -errno;

        यदि (do_chmod) {
                r = fchmod_opath(fd, बहुलकम् & 07777);
                यदि (r < 0) {
                        यदि (!मार्गः || r != -ENOSYS)
                                प्रत्यागम r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        यदि (chmod(मार्गः, बहुलकम् & 07777) < 0)
                                प्रत्यागम -errno;
                }
        }

        प्रत्यागम do_chown || do_chmod;
}

पूर्णाङ्क fchmod_umask(पूर्णाङ्क fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        प्रत्यागम RET_NERRNO(fchmod(fd, m & (~u)));
}

पूर्णाङ्क fchmod_opath(पूर्णाङ्क fd, mode_t m) {
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

        यदि (fchmodat(fd, "", m, AT_रिक्तानि_मार्गः) >= 0)
                प्रत्यागम 0;
        यदि (errno == EINVAL && fchmodat2(fd, "", m, AT_रिक्तानि_मार्गः) >= 0) /* glibc too old? */
                प्रत्यागम 0;
        यदि (!अन्तर्_समुच्चयः(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                प्रत्यागम -errno;

        यदि (chmod(FORMAT_PROC_FD_मार्गः(fd), m) < 0) {
                यदि (errno != ENOENT)
                        प्रत्यागम -errno;

                प्रत्यागम proc_fd_enoent_errno();
        }

        प्रत्यागम 0;
}

पूर्णाङ्क futimens_opath(पूर्णाङ्क fd, ध्रुव संरचना timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        प्रतिज्ञा(fd >= 0);

        प्रत्यागम RET_NERRNO(utimensat(fd, "", ts, AT_रिक्तानि_मार्गः));
}

पूर्णाङ्क stat_warn_permissions(ध्रुव वर्ण *मार्गः, ध्रुव संरचना stat *st) {
        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        यदि (!S_ISREG(st->st_mode))
                प्रत्यागम 0;

        यदि (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", मार्गः);

        यदि (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", मार्गः);

        यदि (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", मार्गः);

        प्रत्यागम 0;
}

पूर्णाङ्क fd_warn_permissions(ध्रुव वर्ण *मार्गः, पूर्णाङ्क fd) {
        संरचना stat st;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(fd >= 0);

        यदि (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        प्रत्यागम stat_warn_permissions(मार्गः, &st);
}

पूर्णाङ्क access_nofollow(ध्रुव वर्ण *मार्गः, पूर्णाङ्क बहुलकम्) {
        प्रत्यागम RET_NERRNO(faccessat(AT_FDCWD, मार्गः, बहुलकम्, AT_SYMLINK_NOFOLLOW));
}

पूर्णाङ्क access_fd(पूर्णाङ्क fd, पूर्णाङ्क बहुलकम्) {
        /* Like access() but operates on an already open fd */

        यदि (fd == AT_FDCWD)
                प्रत्यागम RET_NERRNO(access(".", बहुलकम्));
        यदि (fd == XAT_FDROOT)
                प्रत्यागम RET_NERRNO(access("/", बहुलकम्));

        प्रतिज्ञा(fd >= 0);

        प्रत्यागम RET_NERRNO(faccessat(fd, "", बहुलकम्, AT_रिक्तानि_मार्गः));
}

पूर्णाङ्क touch_fd(पूर्णाङ्क fd, usec_t stamp) {
        प्रतिज्ञा(fd >= 0);

        यदि (stamp == USEC_INFINITY)
                प्रत्यागम futimens_opath(fd, /* ts= */ रिक्त);

        संरचना timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        प्रत्यागम futimens_opath(fd, ts);
}

पूर्णाङ्क touch_file(ध्रुव वर्ण *मार्गः, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t बहुलकम्) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क ret;

        प्रतिज्ञा(मार्गः);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        यदि (parents)
                (शून्य) mkdir_parents(मार्गः, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = उद्घाटय(मार्गः, O_मार्गः|O_CLOEXEC|O_NOFOLLOW);
        यदि (fd < 0) {
                यदि (errno != ENOENT)
                        प्रत्यागम -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = उद्घाटय(मार्गः, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, अन्तर्_समुच्चयः(बहुलकम्, 0, बहुलकम्_INVALID) ? 0644 : बहुलकम्);
                यदि (fd < 0)
                        प्रत्यागम -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, बहुलकम्, uid, gid);

        प्रत्यागम RET_GATHER(ret, touch_fd(fd, stamp));
}

पूर्णाङ्क touch(ध्रुव वर्ण *मार्गः) {
        प्रत्यागम touch_file(मार्गः, false, USEC_INFINITY, UID_INVALID, GID_INVALID, बहुलकम्_INVALID);
}

पूर्णाङ्क symlinkat_idempotent(ध्रुव वर्ण *target, पूर्णाङ्क atfd, ध्रुव वर्ण *linkpath, bool make_relative) {
        _cleanup_free_ वर्ण *relpath = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(linkpath);

        यदि (make_relative) {
                r = मार्गः_make_relative_parent(linkpath, target, &relpath);
                यदि (r < 0)
                        प्रत्यागम r;

                target = relpath;
        }

        यदि (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ वर्ण *p = रिक्त;

                यदि (errno != EEXIST)
                        प्रत्यागम -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                यदि (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        प्रत्यागम -EEXIST;
                यदि (r < 0) /* Any other error? In that case propagate it as is */
                        प्रत्यागम r;

                यदि (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        प्रत्यागम -EEXIST;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क symlinkat_atomic_पूर्णम्_label(ध्रुव वर्ण *target, पूर्णाङ्क atfd, ध्रुव वर्ण *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        पूर्णाङ्क r;

        प्रतिज्ञा(target);
        प्रतिज्ञा(linkpath);

        _cleanup_free_ वर्ण *relpath = रिक्त;
        यदि (FLAGS_समुच्चयः(flags, SYMLINK_MAKE_RELATIVE)) {
                r = मार्गः_make_relative_parent(linkpath, target, &relpath);
                यदि (r < 0)
                        प्रत्यागम r;

                target = relpath;
        }

        _cleanup_free_ वर्ण *t = रिक्त;
        r = tempfn_random(linkpath, रिक्त, &t);
        यदि (r < 0)
                प्रत्यागम r;

        bool call_label_ops_post = false;
        यदि (FLAGS_समुच्चयः(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                यदि (r < 0)
                        प्रत्यागम r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        यदि (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        यदि (r < 0)
                प्रत्यागम r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        यदि (r < 0) {
                (शून्य) unlinkat(atfd, t, 0);
                प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क mknodat_atomic(पूर्णाङ्क atfd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्, dev_t dev) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        r = tempfn_random(मार्गः, रिक्त, &t);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (mknodat(atfd, t, बहुलकम्, dev) < 0)
                प्रत्यागम -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, मार्गः));
        यदि (r < 0) {
                (शून्य) unlinkat(atfd, t, 0);
                प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क mkfifoat_atomic(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्) {
        _cleanup_free_ वर्ण *t = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(मार्गः, रिक्त, &t);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (mkfifoat(dir_fd, t, बहुलकम्) < 0)
                प्रत्यागम -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, मार्गः));
        यदि (r < 0) {
                (शून्य) unlinkat(dir_fd, t, 0);
                प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क get_files_अन्तर्_directory(ध्रुव वर्ण *मार्गः, वर्ण ***ret_list) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        _cleanup_closedir_ DIR *d = रिक्त;
        size_t n = 0;

        प्रतिज्ञा(मार्गः);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(मार्गः);
        यदि (!d)
                प्रत्यागम -errno;

        FOREACH_DIRENT_ALL(de, d, प्रत्यागम -errno) {
                यदि (!dirent_is_file(de))
                        अनुवर्त;

                यदि (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        यदि (!GREEDY_REALLOC(l, n + 2))
                                प्रत्यागम -ENOMEM;

                        l[n] = strdup(de->d_name);
                        यदि (!l[n])
                                प्रत्यागम -ENOMEM;

                        l[++n] = रिक्त;
                } अन्यथा
                        n++;
        }

        यदि (ret_list)
                *ret_list = TAKE_PTR(l);

        प्रत्यागम n;
}

स्थिर पूर्णाङ्क getenv_tmp_dir(ध्रुव वर्ण **ret_मार्गः) {
        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(ret_मार्गः);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                ध्रुव वर्ण *e;

                e = secure_getenv(n);
                यदि (!e)
                        अनुवर्त;
                यदि (!मार्गः_is_absolute(e)) {
                        r = -ENOTDIR;
                        गच्छ अग्र_2;
                }
                यदि (!मार्गः_is_normalized(e)) {
                        r = -EPERM;
                        गच्छ अग्र_2;
                }

                r = is_dir(e, true);
                यदि (r < 0)
                        गच्छ अग्र_2;
                यदि (r == 0) {
                        r = -ENOTDIR;
                        गच्छ अग्र_2;
                }

                *ret_मार्गः = e;
                प्रत्यागम 1;

        अग्र_2:
                /* Remember first error, to make this more debuggable */
                यदि (ret >= 0)
                        ret = r;
        }

        यदि (ret < 0)
                प्रत्यागम ret;

        *ret_मार्गः = रिक्त;
        प्रत्यागम ret;
}

स्थिर पूर्णाङ्क tmp_dir_आन्तरिकम्(ध्रुव वर्ण *def, ध्रुव वर्ण **ret) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क r, k;

        प्रतिज्ञा(def);
        प्रतिज्ञा(ret);

        r = getenv_tmp_dir(&e);
        यदि (r > 0) {
                *ret = e;
                प्रत्यागम 0;
        }

        k = is_dir(def, /* follow= */ true);
        यदि (k == 0)
                k = -ENOTDIR;
        यदि (k < 0)
                प्रत्यागम RET_GATHER(r, k);

        *ret = def;
        प्रत्यागम 0;
}

पूर्णाङ्क var_tmp_dir(ध्रुव वर्ण **ret) {
        प्रतिज्ञा(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        प्रत्यागम tmp_dir_आन्तरिकम्("/var/tmp", ret);
}

पूर्णाङ्क tmp_dir(ध्रुव वर्ण **ret) {
        प्रतिज्ञा(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        प्रत्यागम tmp_dir_आन्तरिकम्("/tmp", ret);
}

पूर्णाङ्क unlink_or_warn(ध्रुव वर्ण *filename) {
        प्रतिज्ञा(filename);

        यदि (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                यदि (errno != EROFS || access(filename, F_OK) >= 0)
                        प्रत्यागम log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        प्रत्यागम 0;
}

वर्ण *rmdir_and_free(वर्ण *p) {
        PROTECT_ERRNO;

        यदि (!p)
                प्रत्यागम रिक्त;

        (शून्य) rmdir(p);
        प्रत्यागम mfree(p);
}

वर्ण* unlink_and_free(वर्ण *p) {
        PROTECT_ERRNO;

        यदि (!p)
                प्रत्यागम रिक्त;

        (शून्य) unlink(p);
        प्रत्यागम mfree(p);
}

पूर्णाङ्क unlinkat_deallocate(पूर्णाङ्क fd, ध्रुव वर्ण *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ पूर्णाङ्क truncate_fd = -EBADF;
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

        यदि (!FLAGS_समुच्चयः(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                यदि (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        यदि (अन्तर्_समुच्चयः(errno, ENOENT, EISDIR))
                                प्रत्यागम -errno;

                        यदि (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        यदि (unlinkat(fd, name, FLAGS_समुच्चयः(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                प्रत्यागम -errno;

        यदि (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                प्रत्यागम 0;

        यदि (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                प्रत्यागम 0;
        }

        यदि (!S_ISREG(st.st_mode))
                प्रत्यागम 0;

        यदि (FLAGS_समुच्चयः(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t वामः = st.st_size;
                वर्ण बफर[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_अष्टकानि(बफर, परिमाण(बफर));

                यावत् (वामः > 0) {
                        ssize_t n;

                        n = लिख(truncate_fd, बफर, MIN(परिमाण(बफर), वामः));
                        यदि (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                भङ्ग;
                        }

                        प्रतिज्ञा(वामः >= (size_t) n);
                        वामः -= n;
                }

                /* Let's refresh metadata */
                यदि (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        प्रत्यागम 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        यदि (st.st_blocks == 0 || st.st_nlink > 0)
                प्रत्यागम 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = आवर्तनम्_ऊर्ध्वम्(st.st_size, bs); /* Round up to next block size */

        यदि (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                प्रत्यागम 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        यदि (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                प्रत्यागम 0;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क open_parent_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags, mode_t बहुलकम्) {
        _cleanup_free_ वर्ण *parent = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(मार्गः);

        r = मार्गः_extract_directory(मार्गः, &parent);
        यदि (r == -EDESTADDRREQ) {
                parent = strdup(".");
                यदि (!parent)
                        प्रत्यागम -ENOMEM;
        } अन्यथा यदि (r == -EADDRNOTAVAIL) {
                parent = strdup(मार्गः);
                यदि (!parent)
                        प्रत्यागम -ENOMEM;
        } अन्यथा यदि (r < 0)
                प्रत्यागम r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        यदि (!FLAGS_समुच्चयः(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        प्रत्यागम RET_NERRNO(openat(dir_fd, parent, flags, बहुलकम्));
}

पूर्णाङ्क conservative_renameat(
                पूर्णाङ्क olddirfd, ध्रुव वर्ण *oldpath,
                पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath) {

        _cleanup_close_ पूर्णाङ्क old_fd = -EBADF, new_fd = -EBADF;
        संरचना stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        यदि (old_fd < 0)
                गच्छ do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        यदि (new_fd < 0)
                गच्छ do_rename;

        यदि (fstat(old_fd, &old_stat) < 0)
                गच्छ do_rename;

        यदि (!S_ISREG(old_stat.st_mode))
                गच्छ do_rename;

        यदि (fstat(new_fd, &new_stat) < 0)
                गच्छ do_rename;

        यदि (stat_inode_same(&new_stat, &old_stat))
                गच्छ is_same;

        यदि (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                गच्छ do_rename;

        प्रति (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[परिमाण(buf1)];
                ssize_t l1, l2;

                l1 = पठ(old_fd, buf1, परिमाण(buf1));
                यदि (l1 < 0)
                        गच्छ do_rename;

                यदि (l1 == परिमाण(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = पठ(new_fd, buf2, l1);
                अन्यथा {
                        प्रतिज्ञा((size_t) l1 < परिमाण(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        प्रतिज्ञा((size_t) (l1 + 1) <= परिमाण(buf2));
                        l2 = पठ(new_fd, buf2, l1 + 1);
                }
                यदि (l2 != l1)
                        गच्छ do_rename;

                यदि (memcmp(buf1, buf2, l1) != 0)
                        गच्छ do_rename;

                यदि ((size_t) l1 < परिमाण(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        भङ्ग;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        यदि (unlinkat(olddirfd, oldpath, 0) < 0)
                गच्छ do_rename;

        प्रत्यागम 0;

do_rename:
        यदि (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                प्रत्यागम -errno;

        प्रत्यागम 1;
}

पूर्णाङ्क posix_fallocate_loop(पूर्णाङ्क fd, uint64_t विस्थापनम्, uint64_t आकार) {
        दरः_सीमा rl;
        पूर्णाङ्क r;

        r = posix_fallocate(fd, विस्थापनम्, आकार); /* returns positive errnos on error */
        यदि (r != EINTR)
                प्रत्यागम -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (ध्रुव दरः_सीमा) { 10 * USEC_प्रत्येकम्_SEC, 16 };
        यावत् (ratelimit_अधः(&rl)) {
                r = posix_fallocate(fd, विस्थापनम्, आकार);
                यदि (r != EINTR)
                        प्रत्यागम -r;
        }

        प्रत्यागम -EINTR;
}

पूर्णाङ्क parse_cifs_service(
                ध्रुव वर्ण *s,
                वर्ण **ret_host,
                वर्ण **ret_service,
                वर्ण **ret_मार्गः) {

        _cleanup_free_ वर्ण *h = रिक्त, *ss = रिक्त, *x = रिक्त;
        ध्रुव वर्ण *p, *e, *d;
        वर्ण delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        यदि (!s)
                प्रत्यागम -EINVAL;

        p = startswith(s, "//");
        यदि (!p) {
                p = startswith(s, "\\\\");
                यदि (!p)
                        प्रत्यागम -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        यदि (!e)
                प्रत्यागम -EINVAL;

        h = strndup(p, e - p);
        यदि (!h)
                प्रत्यागम -ENOMEM;

        यदि (!hostname_is_valid(h, 0))
                प्रत्यागम -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        यदि (!ss)
                प्रत्यागम -ENOMEM;

        यदि (!filename_is_valid(ss))
                प्रत्यागम -EINVAL;

        यदि (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_गन्तव्यम्_STR(delimiter)));
                यदि (!x)
                        प्रत्यागम -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                प्रति (वर्ण *i = x; *i; i++)
                        यदि (*i == delimiter)
                                *i = '/';

                यदि (!मार्गः_is_valid(x))
                        प्रत्यागम -EINVAL;

                मार्गः_simplify(x);
                यदि (!मार्गः_is_normalized(x))
                        प्रत्यागम -EINVAL;
        }

        यदि (ret_host)
                *ret_host = TAKE_PTR(h);
        यदि (ret_service)
                *ret_service = TAKE_PTR(ss);
        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(x);

        प्रत्यागम 0;
}

पूर्णाङ्क open_mkdir_at_पूर्णम्_label(पूर्णाङ्क dirfd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags, XOpenFlags xopen_flags, mode_t बहुलकम्, LabelContext *label_context) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ वर्ण *fname = रिक्त, *parent = रिक्त;
        पूर्णाङ्क r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        यदि (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_मार्गः))
                प्रत्यागम -EINVAL;
        यदि ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                प्रत्यागम -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = मार्गः_extract_directory(मार्गः, &parent);
        यदि (r < 0) {
                यदि (!अन्तर्_समुच्चयः(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        प्रत्यागम r;
        } अन्यथा {
                r = मार्गः_extract_filename(मार्गः, &fname);
                यदि (r < 0)
                        प्रत्यागम r;

                parent_fd = openat(dirfd, parent, O_मार्गः|O_DIRECTORY|O_CLOEXEC);
                यदि (parent_fd < 0)
                        प्रत्यागम -errno;

                dirfd = parent_fd;
                मार्गः = fname;
        }

        fd = xopenat_पूर्णम्_label(dirfd, मार्गः, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, बहुलकम्, label_context);
        यदि (अन्तर्_समुच्चयः(fd, -ELOOP, -ENOTDIR))
                प्रत्यागम -EEXIST;
        यदि (fd < 0)
                प्रत्यागम fd;

        प्रत्यागम TAKE_FD(fd);
}

पूर्णाङ्क openat_report_new(पूर्णाङ्क dirfd, ध्रुव वर्ण *pathname, पूर्णाङ्क flags, mode_t बहुलकम्, bool *ret_newly_created) {
        पूर्णाङ्क fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        यदि (!FLAGS_समुच्चयः(flags, O_CREAT) || FLAGS_समुच्चयः(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, बहुलकम्);
                यदि (fd < 0)
                        प्रत्यागम -errno;

                यदि (ret_newly_created)
                        *ret_newly_created = FLAGS_समुच्चयः(flags, O_CREAT);
                प्रत्यागम fd;
        }

        प्रति (अचिह्नित attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), बहुलकम्);
                यदि (fd >= 0) {
                        यदि (ret_newly_created)
                                *ret_newly_created = false;
                        प्रत्यागम fd;
                }
                यदि (errno != ENOENT)
                        प्रत्यागम -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, बहुलकम्);
                यदि (fd >= 0) {
                        यदि (ret_newly_created)
                                *ret_newly_created = true;
                        प्रत्यागम fd;
                }
                यदि (errno != EEXIST)
                        प्रत्यागम -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                यदि (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        प्रत्यागम -EEXIST;
        }
}

स्थिर पूर्णाङ्क openat_with_automount(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क open_flags, mode_t बहुलकम्) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        स्थिर bool can_open_वृक्षः = true;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(FLAGS_समुच्चयः(open_flags, O_मार्गः));

        यदि (can_open_वृक्षः) {
                पूर्णाङ्क fd = RET_NERRNO(open_tree(dir_fd, मार्गः,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_समुच्चयः(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                यदि (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        यदि (FLAGS_समुच्चयः(open_flags, O_DIRECTORY)) {
                                पूर्णाङ्क q = fd_verify_directory(fd);
                                यदि (q < 0) {
                                        safe_close(fd);
                                        प्रत्यागम q;
                                }
                        }

                        प्रत्यागम fd;
                }
                यदि (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        प्रत्यागम fd;

                can_open_वृक्षः = false;
        }

        प्रत्यागम RET_NERRNO(openat(dir_fd, मार्गः, open_flags, बहुलकम्));
}

पूर्णाङ्क xopenat_पूर्णम्_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क open_flags, XOpenFlags xopen_flags, mode_t बहुलकम्, LabelContext *label_context) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        bool made_dir = false, made_file = false;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        प्रतिज्ञा(FLAGS_समुच्चयः(open_flags, O_DIRECTORY) + FLAGS_समुच्चयः(xopen_flags, XO_REGULAR) + FLAGS_समुच्चयः(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        प्रतिज्ञा(!FLAGS_समुच्चयः(xopen_flags, XO_SOCKET) || FLAGS_समुच्चयः(open_flags, O_मार्गः));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        प्रतिज्ञा(!FLAGS_समुच्चयः(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_समुच्चयः(open_flags, O_मार्गः) && !FLAGS_समुच्चयः(open_flags, O_CREAT)));
        प्रतिज्ञा(!(FLAGS_समुच्चयः(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_समुच्चयः(xopen_flags, XO_SUBVOLUME)));
        प्रतिज्ञा(!(FLAGS_समुच्चयः(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        प्रतिज्ञा((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        प्रतिज्ञा(!FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        यदि (बहुलकम् == बहुलकम्_INVALID)
                बहुलकम् = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        यदि (FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO)) {
                यदि (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } अन्यथा यदि (open_flags & O_मार्गः)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        प्रत्यागम -EINVAL;
        }

        यदि (isempty(मार्गः)) {
                प्रतिज्ञा(!FLAGS_समुच्चयः(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                यदि (FLAGS_समुच्चयः(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (FLAGS_समुच्चयः(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        यदि (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                प्रत्यागम TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                प्रत्यागम fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ पूर्णाङ्क _dir_fd = -EBADF;
        यदि (dir_fd == XAT_FDROOT) {
                यदि (मार्गः_is_absolute(मार्गः))
                        dir_fd = AT_FDCWD;
                अन्यथा {
                        _dir_fd = उद्घाटय("/", O_CLOEXEC|O_DIRECTORY|O_मार्गः);
                        यदि (_dir_fd < 0)
                                प्रत्यागम -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        यदि (FLAGS_समुच्चयः(open_flags, O_CREAT) && FLAGS_समुच्चयः(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, मार्गः, FLAGS_समुच्चयः(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                यदि (r < 0)
                        प्रत्यागम r;

                call_label_ops_post = true;
        }

        यदि (FLAGS_समुच्चयः(open_flags, O_DIRECTORY|O_CREAT)) {
                यदि (FLAGS_समुच्चयः(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, मार्गः, बहुलकम्);
                अन्यथा
                        r = RET_NERRNO(mkdirat(dir_fd, मार्गः, बहुलकम्));
                यदि (r == -EEXIST) {
                        यदि (FLAGS_समुच्चयः(open_flags, O_EXCL))
                                प्रत्यागम -EEXIST;
                } अन्यथा यदि (r < 0)
                        प्रत्यागम r;
                अन्यथा
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        यदि (FLAGS_समुच्चयः(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                यदि (FLAGS_समुच्चयः(open_flags, O_मार्गः)) {
                        fd = FLAGS_समुच्चयः(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, मार्गः, open_flags, बहुलकम्) :
                                RET_NERRNO(openat(dir_fd, मार्गः, open_flags, बहुलकम्));
                        यदि (fd < 0) {
                                r = fd;
                                गच्छ error;
                        }

                        r = fd_verify_regular(fd);
                        यदि (r < 0)
                                गच्छ error;

                } अन्यथा यदि (FLAGS_समुच्चयः(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        यदि (FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, मार्गः, open_flags|O_RDWR, बहुलकम्));
                                यदि (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                अन्यथा यदि (fd < 0) {
                                        r = fd;
                                        गच्छ error;
                                }
                        }

                        यदि (fd < 0) {
                                fd = openat(dir_fd, मार्गः, open_flags, बहुलकम्);
                                यदि (fd < 0) {
                                        r = -errno;
                                        गच्छ error;
                                }
                        }

                        made_file = true;
                } अन्यथा {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ पूर्णाङ्क inode_fd = openat(dir_fd, मार्गः, O_मार्गः|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        यदि (inode_fd < 0) {
                                यदि (errno != ENOENT || !FLAGS_समुच्चयः(open_flags, O_CREAT)) {
                                        r = -errno;
                                        गच्छ error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                यदि (FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, मार्गः, open_flags|O_RDWR, बहुलकम्));
                                        यदि (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        अन्यथा यदि (fd < 0) {
                                                r = fd;
                                                गच्छ error;
                                        }
                                }

                                यदि (fd < 0) {
                                        fd = openat(dir_fd, मार्गः, open_flags, बहुलकम्);
                                        यदि (fd < 0) {
                                                r = -errno;
                                                गच्छ error;
                                        }
                                }

                                made_file = true;
                        } अन्यथा {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                यदि (r < 0)
                                        गच्छ error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                यदि (FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        यदि (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        अन्यथा यदि (fd < 0) {
                                                r = fd;
                                                गच्छ error;
                                        }
                                }

                                यदि (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        यदि (fd < 0) {
                                                r = fd;
                                                गच्छ error;
                                        }
                                }
                        }
                }
        } अन्यथा यदि (FLAGS_समुच्चयः(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, मार्गः, open_flags, बहुलकम्);
                यदि (fd < 0) {
                        r = fd;
                        गच्छ error;
                }
        } अन्यथा {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                यदि (FLAGS_समुच्चयः(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, मार्गः, O_RDWR|open_flags, बहुलकम्, &made_file);
                        यदि (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        अन्यथा यदि (fd < 0) {
                                r = fd;
                                गच्छ error;
                        }
                }

                यदि (fd < 0) {
                        fd = openat_report_new(dir_fd, मार्गः, open_flags, बहुलकम्, &made_file);
                        यदि (fd < 0) {
                                r = fd;
                                गच्छ error;
                        }
                }
        }

        यदि (FLAGS_समुच्चयः(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                यदि (r < 0)
                        गच्छ error;
        }

        यदि (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ रिक्त, made_file || made_dir, label_context);
                यदि (r < 0)
                        गच्छ error;
        }

        यदि (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_समुच्चयः(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                यदि (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        गच्छ error;
        }

        प्रत्यागम TAKE_FD(fd);

error:
        यदि (call_label_ops_post)
                (शून्य) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? रिक्त : मार्गः, made_dir || made_file, label_context);

        यदि (made_dir || made_file)
                (शून्य) unlinkat(dir_fd, मार्गः, made_dir ? AT_REMOVEDIR : 0);

        प्रत्यागम r;
}

पूर्णाङ्क xopenat_तालकम्_पूर्णम्_label(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क open_flags,
                XOpenFlags xopen_flags,
                mode_t बहुलकम्,
                तालकम्_Type locktype,
                पूर्णाङ्क operation,
                LabelContext *label_context) {

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(अन्तर्_समुच्चयः(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        यदि (FLAGS_समुच्चयः(open_flags, O_DIRECTORY) && !अन्तर्_समुच्चयः(locktype, तालकम्_BSD, तालकम्_NONE))
                प्रत्यागम -EBADF;

        प्रति (;;) {
                संरचना stat st;

                fd = xopenat_पूर्णम्_label(dir_fd, मार्गः, open_flags, xopen_flags, बहुलकम्, label_context);
                यदि (fd < 0)
                        प्रत्यागम fd;

                r = तालकम्_generic(fd, locktype, operation);
                यदि (r < 0)
                        प्रत्यागम r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                यदि (fstat(fd, &st) < 0)
                        प्रत्यागम -errno;
                यदि (st.st_nlink > 0)
                        भङ्ग;

                fd = safe_close(fd);
        }

        प्रत्यागम TAKE_FD(fd);
}

पूर्णाङ्क link_fd(पूर्णाङ्क fd, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(newdirfd >= 0 || newdirfd == AT_FDCWD);
        प्रतिज्ञा(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_रिक्तानि_मार्गः));
        यदि (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_मार्गः(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                यदि (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        प्रत्यागम -ENOSYS;
        }

        प्रत्यागम r;
}

पूर्णाङ्क linkat_replace(पूर्णाङ्क olddirfd, ध्रुव वर्ण *oldpath, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath) {
        _cleanup_close_ पूर्णाङ्क old_fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(olddirfd >= 0 || olddirfd == AT_FDCWD);
        प्रतिज्ञा(newdirfd >= 0 || newdirfd == AT_FDCWD);
        प्रतिज्ञा(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        यदि (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                प्रत्यागम -EISDIR;

        यदि (मार्गः_उपपादयति_directory(oldpath)) /* Refuse these definite directories early */
                प्रत्यागम -EISDIR;

        यदि (मार्गः_उपपादयति_directory(newpath))
                प्रत्यागम -EISDIR;

        /* First, try to link this directly */
        यदि (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        अन्यथा
                r = link_fd(olddirfd, newdirfd, newpath);
        यदि (r >= 0)
                प्रत्यागम 0;
        यदि (r != -EEXIST)
                प्रत्यागम r;

        old_fd = xopenat(olddirfd, oldpath, O_मार्गः|O_CLOEXEC);
        यदि (old_fd < 0)
                प्रत्यागम old_fd;

        संरचना stat old_st;
        यदि (fstat(old_fd, &old_st) < 0)
                प्रत्यागम -errno;

        यदि (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                प्रत्यागम -EISDIR;

        संरचना stat new_st;
        यदि (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                प्रत्यागम -errno;

        यदि (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                प्रत्यागम -EEXIST;

        यदि (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                प्रत्यागम 0;

        _cleanup_free_ वर्ण *tmp_मार्गः = रिक्त;
        r = tempfn_random(newpath, /* extra= */ रिक्त, &tmp_मार्गः);
        यदि (r < 0)
                प्रत्यागम r;

        r = link_fd(old_fd, newdirfd, tmp_मार्गः);
        यदि (r < 0) {
                यदि (!ERRNO_IS_PRIVILEGE(r))
                        प्रत्यागम r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_मार्गः, 0));
                यदि (r < 0)
                        प्रत्यागम r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_मार्गः, newdirfd, newpath));
        यदि (r < 0) {
                (शून्य) unlinkat(newdirfd, tmp_मार्गः, /* flags= */ 0);
                प्रत्यागम r;
        }

        प्रत्यागम 0;
}
