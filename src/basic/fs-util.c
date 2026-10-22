/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/falloc.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/mount.h>
#include <unistd.h>

#include "alloc-util.h"
#include "btrfs-util.h"
#include "chattr-util.h"
#include "dirent-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "hostname-util.h"
#include "label-util.h"
#include "lock-util.h"
#include "log.h"
#include "mkdir.h"
#include "path-util.h"
#include "process-util.h"
#include "random-util.h"
#include "ratelimit.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"
#include "tmpfile-util.h"
#include "umask-util.h"

puur_naa_nka rmdir_parents(const var_na *path, const var_na *stop) {
        var_na *p;
        puur_naa_nka r;

        assert(path);
        assert(stop);

        ᮚᮦᮔ᮪ (!path_is_safe(path))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!path_is_safe(stop))
                ᮝᮜᮤ -EINVAL;

        p = strdupa_safe(path);

        for (;;) {
                var_na *slash = NULL;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (const var_na **) &slash, NULL);
                ᮚᮦᮔ᮪ (r <= 0)
                        ᮝᮜᮤ r;
                ᮚᮦᮔ᮪ (slash == p)
                        ᮝᮜᮤ 0;

                assert(*slash == '/');
                *slash = '\0';

                ᮚᮦᮔ᮪ (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        ᮝᮜᮤ 0;

                ᮚᮦᮔ᮪ (rmdir(p) < 0 && errno != ENOENT)
                        ᮝᮜᮤ -errno;
        }
}

puur_naa_nka rename_noreplace(puur_naa_nka olddirfd, const var_na *oldpath, puur_naa_nka newdirfd, const var_na *newpath) {
        puur_naa_nka r;

        assert(olddirfd >= 0 || olddirfd == AT_FDCWD);
        assert(oldpath);
        assert(newdirfd >= 0 || newdirfd == AT_FDCWD);
        assert(newpath);

        /* Try the ideal approach first */
        ᮚᮦᮔ᮪ (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                ᮝᮜᮤ 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        ᮚᮦᮔ᮪ (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                ᮝᮜᮤ -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        ᮚᮦᮔ᮪ (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                ᮚᮦᮔ᮪ (r < 0) {
                        (void) unlinkat(newdirfd, newpath, 0);
                        ᮝᮜᮤ r;
                }

                ᮝᮜᮤ 0;
        }

        ᮚᮦᮔ᮪ (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                ᮝᮜᮤ -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        ᮚᮦᮔ᮪ (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                ᮝᮜᮤ -EEXIST;
        ᮚᮦᮔ᮪ (errno != ENOENT)
                ᮝᮜᮤ -errno;

        ᮝᮜᮤ RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

puur_naa_nka readlinkat_malloc(puur_naa_nka fd, const var_na *p, var_na **ret) {
        size_t l = PATH_MAX;

        assert(fd >= 0 || fd == AT_FDCWD);

        ᮚᮦᮔ᮪ (fd < 0 && isempty(p))
                ᮝᮜᮤ -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        for (;;) {
                _cleanup_free_ var_na *c = NULL;
                ssize_t n;

                c = new(var_na, l+1);
                ᮚᮦᮔ᮪ (!c)
                        ᮝᮜᮤ -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                ᮚᮦᮔ᮪ (n < 0)
                        ᮝᮜᮤ -errno;

                ᮚᮦᮔ᮪ ((size_t) n < l) {
                        c[n] = 0;

                        ᮚᮦᮔ᮪ (ret)
                                *ret = TAKE_PTR(c);

                        ᮝᮜᮤ 0;
                }

                ᮚᮦᮔ᮪ (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        ᮝᮜᮤ -EFBIG;

                l *= 2;
        }
}

puur_naa_nka readlink_value(const var_na *p, var_na **ret) {
        _cleanup_free_ var_na *link = NULL, *name = NULL;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        r = readlink_malloc(p, &link);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = path_extract_filename(link, &name);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (r == O_DIRECTORY)
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(name);
        ᮝᮜᮤ 0;
}

puur_naa_nka readlink_and_make_absolute(const var_na *p, var_na **ret) {
        _cleanup_free_ var_na *target = NULL;
        puur_naa_nka r;

        assert(p);
        assert(ret);

        r = readlink_malloc(p, &target);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮝᮜᮤ file_in_same_dir(p, target, ret);
}

puur_naa_nka chmod_and_chown_at(puur_naa_nka dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        ᮚᮦᮔ᮪ (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                ᮚᮦᮔ᮪ (fd < 0)
                        ᮝᮜᮤ -errno;
                dir_fd = fd;

        } ᮜᮤᮚ ᮚᮦᮔ᮪ (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                ᮚᮦᮔ᮪ (fd < 0)
                        ᮝᮜᮤ -errno;
                dir_fd = fd;
        }

        ᮝᮜᮤ fchmod_and_chown(dir_fd, mode, uid, gid);
}

puur_naa_nka fchmod_and_chown_with_fallback(puur_naa_nka fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        sa_nracanaa stat st;
        puur_naa_nka r;

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

        ᮚᮦᮔ᮪ (fstat(fd, &st) < 0)
                ᮝᮜᮤ -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        ᮚᮦᮔ᮪ (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        ᮜᮤᮚ ᮚᮦᮔ᮪ ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                ᮝᮜᮤ -EINVAL; /* insist on the right file type if it was specified */

        ᮚᮦᮔ᮪ (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                ᮚᮦᮔ᮪ (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        ᮚᮦᮔ᮪ (r < 0) {
                                ᮚᮦᮔ᮪ (!path || r != -ENOSYS)
                                        ᮝᮜᮤ r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                ᮚᮦᮔ᮪ (chmod(path, minimal & 07777) < 0)
                                        ᮝᮜᮤ -errno;
                        }
                }
        }

        ᮚᮦᮔ᮪ (do_chown)
                ᮚᮦᮔ᮪ (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                ᮚᮦᮔ᮪ (r < 0) {
                        ᮚᮦᮔ᮪ (!path || r != -ENOSYS)
                                ᮝᮜᮤ r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        ᮚᮦᮔ᮪ (chmod(path, mode & 07777) < 0)
                                ᮝᮜᮤ -errno;
                }
        }

        ᮝᮜᮤ do_chown || do_chmod;
}

puur_naa_nka fchmod_umask(puur_naa_nka fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        ᮝᮜᮤ RET_NERRNO(fchmod(fd, m & (~u)));
}

puur_naa_nka fchmod_opath(puur_naa_nka fd, mode_t m) {
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

        assert(fd >= 0);

        ᮚᮦᮔ᮪ (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                ᮝᮜᮤ 0;
        ᮚᮦᮔ᮪ (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                ᮝᮜᮤ 0;
        ᮚᮦᮔ᮪ (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                ᮚᮦᮔ᮪ (errno != ENOENT)
                        ᮝᮜᮤ -errno;

                ᮝᮜᮤ proc_fd_enoent_errno();
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka futimens_opath(puur_naa_nka fd, const sa_nracanaa timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        assert(fd >= 0);

        ᮝᮜᮤ RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

puur_naa_nka stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st) {
        assert(path);
        assert(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        ᮚᮦᮔ᮪ (!S_ISREG(st->st_mode))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        ᮚᮦᮔ᮪ (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        ᮚᮦᮔ᮪ (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        ᮝᮜᮤ 0;
}

puur_naa_nka fd_warn_permissions(const var_na *path, puur_naa_nka fd) {
        sa_nracanaa stat st;

        assert(path);
        assert(fd >= 0);

        ᮚᮦᮔ᮪ (fstat(fd, &st) < 0)
                ᮝᮜᮤ -errno;

        ᮝᮜᮤ stat_warn_permissions(path, &st);
}

puur_naa_nka access_nofollow(const var_na *path, puur_naa_nka mode) {
        ᮝᮜᮤ RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

puur_naa_nka access_fd(puur_naa_nka fd, puur_naa_nka mode) {
        /* Like access() but operates on an already open fd */

        ᮚᮦᮔ᮪ (fd == AT_FDCWD)
                ᮝᮜᮤ RET_NERRNO(access(".", mode));
        ᮚᮦᮔ᮪ (fd == XAT_FDROOT)
                ᮝᮜᮤ RET_NERRNO(access("/", mode));

        assert(fd >= 0);

        ᮝᮜᮤ RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

puur_naa_nka touch_fd(puur_naa_nka fd, usec_t stamp) {
        assert(fd >= 0);

        ᮚᮦᮔ᮪ (stamp == USEC_INFINITY)
                ᮝᮜᮤ futimens_opath(fd, /* ts= */ NULL);

        sa_nracanaa timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        ᮝᮜᮤ futimens_opath(fd, ts);
}

puur_naa_nka touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka ret;

        assert(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        ᮚᮦᮔ᮪ (parents)
                (void) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ᮚᮦᮔ᮪ (fd < 0) {
                ᮚᮦᮔ᮪ (errno != ENOENT)
                        ᮝᮜᮤ -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                ᮚᮦᮔ᮪ (fd < 0)
                        ᮝᮜᮤ -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        ᮝᮜᮤ RET_GATHER(ret, touch_fd(fd, stamp));
}

puur_naa_nka touch(const var_na *path) {
        ᮝᮜᮤ touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

puur_naa_nka symlinkat_idempotent(const var_na *target, puur_naa_nka atfd, const var_na *linkpath, bool make_relative) {
        _cleanup_free_ var_na *relpath = NULL;
        puur_naa_nka r;

        assert(target);
        assert(linkpath);

        ᮚᮦᮔ᮪ (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                target = relpath;
        }

        ᮚᮦᮔ᮪ (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ var_na *p = NULL;

                ᮚᮦᮔ᮪ (errno != EEXIST)
                        ᮝᮜᮤ -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                ᮚᮦᮔ᮪ (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        ᮝᮜᮤ -EEXIST;
                ᮚᮦᮔ᮪ (r < 0) /* Any other error? In that case propagate it as is */
                        ᮝᮜᮤ r;

                ᮚᮦᮔ᮪ (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        ᮝᮜᮤ -EEXIST;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka symlinkat_atomic_full_label(const var_na *target, puur_naa_nka atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        puur_naa_nka r;

        assert(target);
        assert(linkpath);

        _cleanup_free_ var_na *relpath = NULL;
        ᮚᮦᮔ᮪ (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                target = relpath;
        }

        _cleanup_free_ var_na *t = NULL;
        r = tempfn_random(linkpath, NULL, &t);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        bool call_label_ops_post = false;
        ᮚᮦᮔ᮪ (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        ᮚᮦᮔ᮪ (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        ᮚᮦᮔ᮪ (r < 0) {
                (void) unlinkat(atfd, t, 0);
                ᮝᮜᮤ r;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka mknodat_atomic(puur_naa_nka atfd, const var_na *path, mode_t mode, dev_t dev) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        assert(path);

        r = tempfn_random(path, NULL, &t);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (mknodat(atfd, t, mode, dev) < 0)
                ᮝᮜᮤ -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        ᮚᮦᮔ᮪ (r < 0) {
                (void) unlinkat(atfd, t, 0);
                ᮝᮜᮤ r;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka mkfifoat_atomic(puur_naa_nka dir_fd, const var_na *path, mode_t mode) {
        _cleanup_free_ var_na *t = NULL;
        puur_naa_nka r;

        assert(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, NULL, &t);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (mkfifoat(dir_fd, t, mode) < 0)
                ᮝᮜᮤ -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        ᮚᮦᮔ᮪ (r < 0) {
                (void) unlinkat(dir_fd, t, 0);
                ᮝᮜᮤ r;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka get_files_in_directory(const var_na *path, var_na ***ret_list) {
        _cleanup_strv_free_ var_na **l = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        size_t n = 0;

        assert(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ -errno;

        FOREACH_DIRENT_ALL(de, d, ᮝᮜᮤ -errno) {
                ᮚᮦᮔ᮪ (!dirent_is_file(de))
                        continue;

                ᮚᮦᮔ᮪ (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        ᮚᮦᮔ᮪ (!GREEDY_REALLOC(l, n + 2))
                                ᮝᮜᮤ -ENOMEM;

                        l[n] = strdup(de->d_name);
                        ᮚᮦᮔ᮪ (!l[n])
                                ᮝᮜᮤ -ENOMEM;

                        l[++n] = NULL;
                } ᮜᮤᮚ
                        n++;
        }

        ᮚᮦᮔ᮪ (ret_list)
                *ret_list = TAKE_PTR(l);

        ᮝᮜᮤ n;
}

static puur_naa_nka getenv_tmp_dir(const var_na **ret_path) {
        puur_naa_nka r, ret = 0;

        assert(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                const var_na *e;

                e = secure_getenv(n);
                ᮚᮦᮔ᮪ (!e)
                        continue;
                ᮚᮦᮔ᮪ (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        goto next;
                }
                ᮚᮦᮔ᮪ (!path_is_normalized(e)) {
                        r = -EPERM;
                        goto next;
                }

                r = is_dir(e, true);
                ᮚᮦᮔ᮪ (r < 0)
                        goto next;
                ᮚᮦᮔ᮪ (r == 0) {
                        r = -ENOTDIR;
                        goto next;
                }

                *ret_path = e;
                ᮝᮜᮤ 1;

        next:
                /* Remember first error, to make this more debuggable */
                ᮚᮦᮔ᮪ (ret >= 0)
                        ret = r;
        }

        ᮚᮦᮔ᮪ (ret < 0)
                ᮝᮜᮤ ret;

        *ret_path = NULL;
        ᮝᮜᮤ ret;
}

static puur_naa_nka tmp_dir_internal(const var_na *def, const var_na **ret) {
        const var_na *e;
        puur_naa_nka r, k;

        assert(def);
        assert(ret);

        r = getenv_tmp_dir(&e);
        ᮚᮦᮔ᮪ (r > 0) {
                *ret = e;
                ᮝᮜᮤ 0;
        }

        k = is_dir(def, /* follow= */ true);
        ᮚᮦᮔ᮪ (k == 0)
                k = -ENOTDIR;
        ᮚᮦᮔ᮪ (k < 0)
                ᮝᮜᮤ RET_GATHER(r, k);

        *ret = def;
        ᮝᮜᮤ 0;
}

puur_naa_nka var_tmp_dir(const var_na **ret) {
        assert(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        ᮝᮜᮤ tmp_dir_internal("/var/tmp", ret);
}

puur_naa_nka tmp_dir(const var_na **ret) {
        assert(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        ᮝᮜᮤ tmp_dir_internal("/tmp", ret);
}

puur_naa_nka unlink_or_warn(const var_na *filename) {
        assert(filename);

        ᮚᮦᮔ᮪ (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                ᮚᮦᮔ᮪ (errno != EROFS || access(filename, F_OK) >= 0)
                        ᮝᮜᮤ log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        ᮝᮜᮤ 0;
}

var_na *rmdir_and_free(var_na *p) {
        PROTECT_ERRNO;

        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        (void) rmdir(p);
        ᮝᮜᮤ mfree(p);
}

var_na* unlink_and_free(var_na *p) {
        PROTECT_ERRNO;

        ᮚᮦᮔ᮪ (!p)
                ᮝᮜᮤ NULL;

        (void) unlink(p);
        ᮝᮜᮤ mfree(p);
}

puur_naa_nka unlinkat_deallocate(puur_naa_nka fd, const var_na *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ puur_naa_nka truncate_fd = -EBADF;
        sa_nracanaa stat st;
        off_t l, bs;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert(name);
        assert((flags & ~(UNLINK_REMOVEDIR|UNLINK_ERASE)) == 0);

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

        ᮚᮦᮔ᮪ (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                ᮚᮦᮔ᮪ (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        ᮚᮦᮔ᮪ (IN_SET(errno, ENOENT, EISDIR))
                                ᮝᮜᮤ -errno;

                        ᮚᮦᮔ᮪ (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        ᮚᮦᮔ᮪ (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                ᮝᮜᮤ 0;
        }

        ᮚᮦᮔ᮪ (!S_ISREG(st.st_mode))
                ᮝᮜᮤ 0;

        ᮚᮦᮔ᮪ (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t left = st.st_size;
                var_na buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, parimaa_na(buffer));

                ᮞᮨᮓᮨᮜ (left > 0) {
                        ssize_t n;

                        n = write(truncate_fd, buffer, MIN(parimaa_na(buffer), left));
                        ᮚᮦᮔ᮪ (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                bha_nga;
                        }

                        assert(left >= (size_t) n);
                        left -= n;
                }

                /* Let's refresh metadata */
                ᮚᮦᮔ᮪ (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        ᮝᮜᮤ 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        ᮚᮦᮔ᮪ (st.st_blocks == 0 || st.st_nlink > 0)
                ᮝᮜᮤ 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        ᮚᮦᮔ᮪ (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                ᮝᮜᮤ 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        ᮚᮦᮔ᮪ (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                ᮝᮜᮤ 0;
        }

        ᮝᮜᮤ 0;
}

puur_naa_nka open_parent_at(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka flags, mode_t mode) {
        _cleanup_free_ var_na *parent = NULL;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = path_extract_directory(path, &parent);
        ᮚᮦᮔ᮪ (r == -EDESTADDRREQ) {
                parent = strdup(".");
                ᮚᮦᮔ᮪ (!parent)
                        ᮝᮜᮤ -ENOMEM;
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                ᮚᮦᮔ᮪ (!parent)
                        ᮝᮜᮤ -ENOMEM;
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        ᮚᮦᮔ᮪ (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        ᮝᮜᮤ RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

puur_naa_nka conservative_renameat(
                puur_naa_nka olddirfd, const var_na *oldpath,
                puur_naa_nka newdirfd, const var_na *newpath) {

        _cleanup_close_ puur_naa_nka old_fd = -EBADF, new_fd = -EBADF;
        sa_nracanaa stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ᮚᮦᮔ᮪ (old_fd < 0)
                goto do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        ᮚᮦᮔ᮪ (new_fd < 0)
                goto do_rename;

        ᮚᮦᮔ᮪ (fstat(old_fd, &old_stat) < 0)
                goto do_rename;

        ᮚᮦᮔ᮪ (!S_ISREG(old_stat.st_mode))
                goto do_rename;

        ᮚᮦᮔ᮪ (fstat(new_fd, &new_stat) < 0)
                goto do_rename;

        ᮚᮦᮔ᮪ (stat_inode_same(&new_stat, &old_stat))
                goto is_same;

        ᮚᮦᮔ᮪ (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                goto do_rename;

        for (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[parimaa_na(buf1)];
                ssize_t l1, l2;

                l1 = read(old_fd, buf1, parimaa_na(buf1));
                ᮚᮦᮔ᮪ (l1 < 0)
                        goto do_rename;

                ᮚᮦᮔ᮪ (l1 == parimaa_na(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = read(new_fd, buf2, l1);
                ᮜᮤᮚ {
                        assert((size_t) l1 < parimaa_na(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        assert((size_t) (l1 + 1) <= parimaa_na(buf2));
                        l2 = read(new_fd, buf2, l1 + 1);
                }
                ᮚᮦᮔ᮪ (l2 != l1)
                        goto do_rename;

                ᮚᮦᮔ᮪ (memcmp(buf1, buf2, l1) != 0)
                        goto do_rename;

                ᮚᮦᮔ᮪ ((size_t) l1 < parimaa_na(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        bha_nga;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        ᮚᮦᮔ᮪ (unlinkat(olddirfd, oldpath, 0) < 0)
                goto do_rename;

        ᮝᮜᮤ 0;

do_rename:
        ᮚᮦᮔ᮪ (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                ᮝᮜᮤ -errno;

        ᮝᮜᮤ 1;
}

puur_naa_nka posix_fallocate_loop(puur_naa_nka fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        puur_naa_nka r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        ᮚᮦᮔ᮪ (r != EINTR)
                ᮝᮜᮤ -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (const RateLimit) { 10 * USEC_PER_SEC, 16 };
        ᮞᮨᮓᮨᮜ (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                ᮚᮦᮔ᮪ (r != EINTR)
                        ᮝᮜᮤ -r;
        }

        ᮝᮜᮤ -EINTR;
}

puur_naa_nka parse_cifs_service(
                const var_na *s,
                var_na **ret_host,
                var_na **ret_service,
                var_na **ret_path) {

        _cleanup_free_ var_na *h = NULL, *ss = NULL, *x = NULL;
        const var_na *p, *e, *d;
        var_na delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -EINVAL;

        p = startswith(s, "//");
        ᮚᮦᮔ᮪ (!p) {
                p = startswith(s, "\\\\");
                ᮚᮦᮔ᮪ (!p)
                        ᮝᮜᮤ -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        ᮚᮦᮔ᮪ (!e)
                ᮝᮜᮤ -EINVAL;

        h = strndup(p, e - p);
        ᮚᮦᮔ᮪ (!h)
                ᮝᮜᮤ -ENOMEM;

        ᮚᮦᮔ᮪ (!hostname_is_valid(h, 0))
                ᮝᮜᮤ -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        ᮚᮦᮔ᮪ (!ss)
                ᮝᮜᮤ -ENOMEM;

        ᮚᮦᮔ᮪ (!filename_is_valid(ss))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                ᮚᮦᮔ᮪ (!x)
                        ᮝᮜᮤ -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                for (var_na *i = x; *i; i++)
                        ᮚᮦᮔ᮪ (*i == delimiter)
                                *i = '/';

                ᮚᮦᮔ᮪ (!path_is_valid(x))
                        ᮝᮜᮤ -EINVAL;

                path_simplify(x);
                ᮚᮦᮔ᮪ (!path_is_normalized(x))
                        ᮝᮜᮤ -EINVAL;
        }

        ᮚᮦᮔ᮪ (ret_host)
                *ret_host = TAKE_PTR(h);
        ᮚᮦᮔ᮪ (ret_service)
                *ret_service = TAKE_PTR(ss);
        ᮚᮦᮔ᮪ (ret_path)
                *ret_path = TAKE_PTR(x);

        ᮝᮜᮤ 0;
}

puur_naa_nka open_mkdir_at_full_label(puur_naa_nka dirfd, const var_na *path, puur_naa_nka flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ puur_naa_nka fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ var_na *fname = NULL, *parent = NULL;
        puur_naa_nka r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        ᮚᮦᮔ᮪ (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                ᮝᮜᮤ -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        ᮚᮦᮔ᮪ (r < 0) {
                ᮚᮦᮔ᮪ (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        ᮝᮜᮤ r;
        } ᮜᮤᮚ {
                r = path_extract_filename(path, &fname);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ᮚᮦᮔ᮪ (parent_fd < 0)
                        ᮝᮜᮤ -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        ᮚᮦᮔ᮪ (IN_SET(fd, -ELOOP, -ENOTDIR))
                ᮝᮜᮤ -EEXIST;
        ᮚᮦᮔ᮪ (fd < 0)
                ᮝᮜᮤ fd;

        ᮝᮜᮤ TAKE_FD(fd);
}

puur_naa_nka openat_report_new(puur_naa_nka dirfd, const var_na *pathname, puur_naa_nka flags, mode_t mode, bool *ret_newly_created) {
        puur_naa_nka fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        ᮚᮦᮔ᮪ (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                ᮚᮦᮔ᮪ (fd < 0)
                        ᮝᮜᮤ -errno;

                ᮚᮦᮔ᮪ (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                ᮝᮜᮤ fd;
        }

        for (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                ᮚᮦᮔ᮪ (fd >= 0) {
                        ᮚᮦᮔ᮪ (ret_newly_created)
                                *ret_newly_created = false;
                        ᮝᮜᮤ fd;
                }
                ᮚᮦᮔ᮪ (errno != ENOENT)
                        ᮝᮜᮤ -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                ᮚᮦᮔ᮪ (fd >= 0) {
                        ᮚᮦᮔ᮪ (ret_newly_created)
                                *ret_newly_created = true;
                        ᮝᮜᮤ fd;
                }
                ᮚᮦᮔ᮪ (errno != EEXIST)
                        ᮝᮜᮤ -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                ᮚᮦᮔ᮪ (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        ᮝᮜᮤ -EEXIST;
        }
}

static puur_naa_nka openat_with_automount(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, mode_t mode) {
        /* When XO_TRIGGER_AUTOMOUNT is set we want to trigger automounts on the path. open() with O_PATH
         * does not do that, so we use open_tree() without OPEN_TREE_CLONE which is equivalent to open() with
         * O_PATH except that it does trigger automounts. Some sandboxes reject open_tree() with EPERM or
         * ENOSYS, in which case we fall back to plain openat(): autofs wouldn't work inside a restricted
         * mount namespace anyway. open_tree() only ever returns O_PATH fds, so this helper is for O_PATH
         * acquisition only. */

        static bool can_open_tree = true;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);
        assert(FLAGS_SET(open_flags, O_PATH));

        ᮚᮦᮔ᮪ (can_open_tree) {
                puur_naa_nka fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                ᮚᮦᮔ᮪ (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                puur_naa_nka q = fd_verify_directory(fd);
                                ᮚᮦᮔ᮪ (q < 0) {
                                        safe_close(fd);
                                        ᮝᮜᮤ q;
                                }
                        }

                        ᮝᮜᮤ fd;
                }
                ᮚᮦᮔ᮪ (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        ᮝᮜᮤ fd;

                can_open_tree = false;
        }

        ᮝᮜᮤ RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

puur_naa_nka xopenat_full_label(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        bool made_dir = false, made_file = false;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));

        /* An inode can only be one of a directory, a regular file or a socket at the same time. */
        assert(FLAGS_SET(open_flags, O_DIRECTORY) + FLAGS_SET(xopen_flags, XO_REGULAR) + FLAGS_SET(xopen_flags, XO_SOCKET) <= 1);
        /* Sockets cannot be open()ed, only pinned via O_PATH. */
        assert(!FLAGS_SET(xopen_flags, XO_SOCKET) || FLAGS_SET(open_flags, O_PATH));
        /* XO_TRIGGER_AUTOMOUNT requires O_PATH and does not support creating inodes. XO_SUBVOLUME
         * requires O_CREAT, and XO_COW/XO_NOCOW need a writable fd for their chattr ioctl, so none are
         * compatible with XO_TRIGGER_AUTOMOUNT. */
        assert(!FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ||
               (FLAGS_SET(open_flags, O_PATH) && !FLAGS_SET(open_flags, O_CREAT)));
        assert(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && FLAGS_SET(xopen_flags, XO_SUBVOLUME)));
        assert(!(FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) && (xopen_flags & (XO_COW|XO_NOCOW))));
        assert((xopen_flags & (XO_COW|XO_NOCOW)) != (XO_COW|XO_NOCOW));

        /* Don't specify an access mode if you want auto mode. */
        assert(!FLAGS_SET(xopen_flags, XO_AUTO_RW_RO) || (open_flags & O_ACCMODE_STRICT) == 0);

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

        ᮚᮦᮔ᮪ (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                ᮚᮦᮔ᮪ (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } ᮜᮤᮚ ᮚᮦᮔ᮪ (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        ᮝᮜᮤ -EINVAL;
        }

        ᮚᮦᮔ᮪ (isempty(path)) {
                assert(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ r;
                }

                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        ᮚᮦᮔ᮪ (r < 0)
                                ᮝᮜᮤ r;
                }

                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        ᮚᮦᮔ᮪ (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                ᮝᮜᮤ TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                ᮝᮜᮤ fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ puur_naa_nka _dir_fd = -EBADF;
        ᮚᮦᮔ᮪ (dir_fd == XAT_FDROOT) {
                ᮚᮦᮔ᮪ (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                ᮜᮤᮚ {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        ᮚᮦᮔ᮪ (_dir_fd < 0)
                                ᮝᮜᮤ -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                call_label_ops_post = true;
        }

        ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                ᮜᮤᮚ
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                ᮚᮦᮔ᮪ (r == -EEXIST) {
                        ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_EXCL))
                                ᮝᮜᮤ -EEXIST;
                } ᮜᮤᮚ ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
                ᮜᮤᮚ
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        ᮚᮦᮔ᮪ (fd < 0) {
                                r = fd;
                                goto error;
                        }

                        r = fd_verify_regular(fd);
                        ᮚᮦᮔ᮪ (r < 0)
                                goto error;

                } ᮜᮤᮚ ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                ᮚᮦᮔ᮪ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                ᮜᮤᮚ ᮚᮦᮔ᮪ (fd < 0) {
                                        r = fd;
                                        goto error;
                                }
                        }

                        ᮚᮦᮔ᮪ (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                ᮚᮦᮔ᮪ (fd < 0) {
                                        r = -errno;
                                        goto error;
                                }
                        }

                        made_file = true;
                } ᮜᮤᮚ {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ puur_naa_nka inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        ᮚᮦᮔ᮪ (inode_fd < 0) {
                                ᮚᮦᮔ᮪ (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        goto error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        ᮚᮦᮔ᮪ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ᮜᮤᮚ ᮚᮦᮔ᮪ (fd < 0) {
                                                r = fd;
                                                goto error;
                                        }
                                }

                                ᮚᮦᮔ᮪ (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        ᮚᮦᮔ᮪ (fd < 0) {
                                                r = -errno;
                                                goto error;
                                        }
                                }

                                made_file = true;
                        } ᮜᮤᮚ {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                ᮚᮦᮔ᮪ (r < 0)
                                        goto error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        ᮚᮦᮔ᮪ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        ᮜᮤᮚ ᮚᮦᮔ᮪ (fd < 0) {
                                                r = fd;
                                                goto error;
                                        }
                                }

                                ᮚᮦᮔ᮪ (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        ᮚᮦᮔ᮪ (fd < 0) {
                                                r = fd;
                                                goto error;
                                        }
                                }
                        }
                }
        } ᮜᮤᮚ ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                ᮚᮦᮔ᮪ (fd < 0) {
                        r = fd;
                        goto error;
                }
        } ᮜᮤᮚ {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        ᮚᮦᮔ᮪ (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        ᮜᮤᮚ ᮚᮦᮔ᮪ (fd < 0) {
                                r = fd;
                                goto error;
                        }
                }

                ᮚᮦᮔ᮪ (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        ᮚᮦᮔ᮪ (fd < 0) {
                                r = fd;
                                goto error;
                        }
                }
        }

        ᮚᮦᮔ᮪ (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                ᮚᮦᮔ᮪ (r < 0)
                        goto error;
        }

        ᮚᮦᮔ᮪ (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ NULL, made_file || made_dir, label_context);
                ᮚᮦᮔ᮪ (r < 0)
                        goto error;
        }

        ᮚᮦᮔ᮪ (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                ᮚᮦᮔ᮪ (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        goto error;
        }

        ᮝᮜᮤ TAKE_FD(fd);

error:
        ᮚᮦᮔ᮪ (call_label_ops_post)
                (void) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : path, made_dir || made_file, label_context);

        ᮚᮦᮔ᮪ (made_dir || made_file)
                (void) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        ᮝᮜᮤ r;
}

puur_naa_nka xopenat_lock_full_label(
                puur_naa_nka dir_fd,
                const var_na *path,
                puur_naa_nka open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                puur_naa_nka operation,
                LabelContext *label_context) {

        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        ᮚᮦᮔ᮪ (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                ᮝᮜᮤ -EBADF;

        for (;;) {
                sa_nracanaa stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                ᮚᮦᮔ᮪ (fd < 0)
                        ᮝᮜᮤ fd;

                r = lock_generic(fd, locktype, operation);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                ᮚᮦᮔ᮪ (fstat(fd, &st) < 0)
                        ᮝᮜᮤ -errno;
                ᮚᮦᮔ᮪ (st.st_nlink > 0)
                        bha_nga;

                fd = safe_close(fd);
        }

        ᮝᮜᮤ TAKE_FD(fd);
}

puur_naa_nka link_fd(puur_naa_nka fd, puur_naa_nka newdirfd, const var_na *newpath) {
        puur_naa_nka r;

        assert(fd >= 0);
        assert(newdirfd >= 0 || newdirfd == AT_FDCWD);
        assert(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        ᮚᮦᮔ᮪ (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                ᮚᮦᮔ᮪ (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        ᮝᮜᮤ -ENOSYS;
        }

        ᮝᮜᮤ r;
}

puur_naa_nka linkat_replace(puur_naa_nka olddirfd, const var_na *oldpath, puur_naa_nka newdirfd, const var_na *newpath) {
        _cleanup_close_ puur_naa_nka old_fd = -EBADF;
        puur_naa_nka r;

        assert(olddirfd >= 0 || olddirfd == AT_FDCWD);
        assert(newdirfd >= 0 || newdirfd == AT_FDCWD);
        assert(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        ᮚᮦᮔ᮪ (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                ᮝᮜᮤ -EISDIR;

        ᮚᮦᮔ᮪ (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                ᮝᮜᮤ -EISDIR;

        ᮚᮦᮔ᮪ (path_implies_directory(newpath))
                ᮝᮜᮤ -EISDIR;

        /* First, try to link this directly */
        ᮚᮦᮔ᮪ (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        ᮜᮤᮚ
                r = link_fd(olddirfd, newdirfd, newpath);
        ᮚᮦᮔ᮪ (r >= 0)
                ᮝᮜᮤ 0;
        ᮚᮦᮔ᮪ (r != -EEXIST)
                ᮝᮜᮤ r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        ᮚᮦᮔ᮪ (old_fd < 0)
                ᮝᮜᮤ old_fd;

        sa_nracanaa stat old_st;
        ᮚᮦᮔ᮪ (fstat(old_fd, &old_st) < 0)
                ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                ᮝᮜᮤ -EISDIR;

        sa_nracanaa stat new_st;
        ᮚᮦᮔ᮪ (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                ᮝᮜᮤ -EEXIST;

        ᮚᮦᮔ᮪ (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                ᮝᮜᮤ 0;

        _cleanup_free_ var_na *tmp_path = NULL;
        r = tempfn_random(newpath, /* extra= */ NULL, &tmp_path);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        ᮚᮦᮔ᮪ (r < 0) {
                ᮚᮦᮔ᮪ (!ERRNO_IS_PRIVILEGE(r))
                        ᮝᮜᮤ r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        ᮚᮦᮔ᮪ (r < 0) {
                (void) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                ᮝᮜᮤ r;
        }

        ᮝᮜᮤ 0;
}
