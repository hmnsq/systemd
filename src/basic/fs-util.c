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

整数 rmdir_parents(const char *path, const char *stop) {
        char *p;
        整数 r;

        assert(path);
        assert(stop);

        もし (!path_is_safe(path))
                戻る -EINVAL;

        もし (!path_is_safe(stop))
                戻る -EINVAL;

        p = strdupa_safe(path);

        ために (;;) {
                char *slash = NULL;

                /* skip the last component. */
                r = path_find_last_component(p, /* accept_dot_dot= */ false, (const char **) &slash, NULL);
                もし (r <= 0)
                        戻る r;
                もし (slash == p)
                        戻る 0;

                assert(*slash == '/');
                *slash = '\0';

                もし (path_startswith_full(stop, p, PATH_STARTSWITH_REFUSE_DOT_DOT))
                        戻る 0;

                もし (rmdir(p) < 0 && errno != ENOENT)
                        戻る -errno;
        }
}

整数 rename_noreplace(整数 olddirfd, const char *oldpath, 整数 newdirfd, const char *newpath) {
        整数 r;

        assert(olddirfd >= 0 || olddirfd == AT_FDCWD);
        assert(oldpath);
        assert(newdirfd >= 0 || newdirfd == AT_FDCWD);
        assert(newpath);

        /* Try the ideal approach first */
        もし (renameat2(olddirfd, oldpath, newdirfd, newpath, RENAME_NOREPLACE) >= 0)
                戻る 0;

        /* renameat2() exists since Linux 3.15, btrfs and FAT added support for it later. If it is not implemented,
         * fall back to a different method. */
        もし (!ERRNO_IS_NOT_SUPPORTED(errno) && errno != EINVAL)
                戻る -errno;

        /* Let's try to use linkat()+unlinkat() as fallback. This doesn't work on directories and on some file systems
         * that do not support hard links (such as FAT, most prominently), but for files it's pretty close to what we
         * want — though not atomic (i.e. for a short period both the new and the old filename will exist). */
        もし (linkat(olddirfd, oldpath, newdirfd, newpath, 0) >= 0) {

                r = RET_NERRNO(unlinkat(olddirfd, oldpath, 0));
                もし (r < 0) {
                        (void) unlinkat(newdirfd, newpath, 0);
                        戻る r;
                }

                戻る 0;
        }

        もし (!ERRNO_IS_NOT_SUPPORTED(errno) && !IN_SET(errno, EINVAL, EPERM)) /* FAT returns EPERM on link()… */
                戻る -errno;

        /* OK, neither RENAME_NOREPLACE nor linkat()+unlinkat() worked. Let's then fall back to the racy TOCTOU
         * vulnerable accessat(F_OK) check followed by classic, replacing renameat(), we have nothing better. */

        もし (faccessat(newdirfd, newpath, F_OK, AT_SYMLINK_NOFOLLOW) >= 0)
                戻る -EEXIST;
        もし (errno != ENOENT)
                戻る -errno;

        戻る RET_NERRNO(renameat(olddirfd, oldpath, newdirfd, newpath));
}

整数 readlinkat_malloc(整数 fd, const char *p, char **ret) {
        size_t l = PATH_MAX;

        assert(fd >= 0 || fd == AT_FDCWD);

        もし (fd < 0 && isempty(p))
                戻る -EISDIR; /* In this case, the fd points to the current working directory, and is
                                 * definitely not a symlink. Let's return earlier. */

        ために (;;) {
                _cleanup_free_ char *c = NULL;
                ssize_t n;

                c = new(char, l+1);
                もし (!c)
                        戻る -ENOMEM;

                n = readlinkat(fd, strempty(p), c, l);
                もし (n < 0)
                        戻る -errno;

                もし ((size_t) n < l) {
                        c[n] = 0;

                        もし (ret)
                                *ret = TAKE_PTR(c);

                        戻る 0;
                }

                もし (l > (SSIZE_MAX-1)/2) /* readlinkat() returns an ssize_t, and we want an extra byte for a
                                          * trailing NUL, hence do an overflow check relative to SSIZE_MAX-1
                                          * here */
                        戻る -EFBIG;

                l *= 2;
        }
}

整数 readlink_value(const char *p, char **ret) {
        _cleanup_free_ char *link = NULL, *name = NULL;
        整数 r;

        assert(p);
        assert(ret);

        r = readlink_malloc(p, &link);
        もし (r < 0)
                戻る r;

        r = path_extract_filename(link, &name);
        もし (r < 0)
                戻る r;
        もし (r == O_DIRECTORY)
                戻る -EINVAL;

        *ret = TAKE_PTR(name);
        戻る 0;
}

整数 readlink_and_make_absolute(const char *p, char **ret) {
        _cleanup_free_ char *target = NULL;
        整数 r;

        assert(p);
        assert(ret);

        r = readlink_malloc(p, &target);
        もし (r < 0)
                戻る r;

        戻る file_in_same_dir(p, target, ret);
}

整数 chmod_and_chown_at(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid) {
        _cleanup_close_ 整数 fd = -EBADF;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        もし (path) {
                /* Let's acquire an O_PATH fd, as precaution to change mode/owner on the same file */
                fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                もし (fd < 0)
                        戻る -errno;
                dir_fd = fd;

        } 否则 もし (dir_fd == AT_FDCWD) {
                /* Let's acquire an O_PATH fd of the current directory */
                fd = openat(dir_fd, ".", O_PATH|O_CLOEXEC|O_NOFOLLOW|O_DIRECTORY);
                もし (fd < 0)
                        戻る -errno;
                dir_fd = fd;
        }

        戻る fchmod_and_chown(dir_fd, mode, uid, gid);
}

整数 fchmod_and_chown_with_fallback(整数 fd, const char *path, mode_t mode, uid_t uid, gid_t gid) {
        bool do_chown, do_chmod;
        struct stat st;
        整数 r;

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

        もし (fstat(fd, &st) < 0)
                戻る -errno;

        do_chown =
                (uid != UID_INVALID && st.st_uid != uid) ||
                (gid != GID_INVALID && st.st_gid != gid);

        do_chmod =
                !S_ISLNK(st.st_mode) && /* chmod is not defined on symlinks */
                ((mode != MODE_INVALID && ((st.st_mode ^ mode) & 07777) != 0) ||
                 do_chown); /* If we change ownership, make sure we reset the mode afterwards, since chown()
                             * modifies the access mode too */

        もし (mode == MODE_INVALID)
                mode = st.st_mode; /* If we only shall do a chown(), save original mode, since chown() might break it. */
        否则 もし ((mode & S_IFMT) != 0 && ((mode ^ st.st_mode) & S_IFMT) != 0)
                戻る -EINVAL; /* insist on the right file type if it was specified */

        もし (do_chown && do_chmod) {
                mode_t minimal = st.st_mode & mode; /* the subset of the old and the new mask */

                もし (((minimal ^ st.st_mode) & 07777) != 0) {
                        r = fchmod_opath(fd, minimal & 07777);
                        もし (r < 0) {
                                もし (!path || r != -ENOSYS)
                                        戻る r;

                                /* Fallback path which doesn't use /proc/self/fd/. */
                                もし (chmod(path, minimal & 07777) < 0)
                                        戻る -errno;
                        }
                }
        }

        もし (do_chown)
                もし (fchownat(fd, "", uid, gid, AT_EMPTY_PATH) < 0)
                        戻る -errno;

        もし (do_chmod) {
                r = fchmod_opath(fd, mode & 07777);
                もし (r < 0) {
                        もし (!path || r != -ENOSYS)
                                戻る r;

                        /* Fallback path which doesn't use /proc/self/fd/. */
                        もし (chmod(path, mode & 07777) < 0)
                                戻る -errno;
                }
        }

        戻る do_chown || do_chmod;
}

整数 fchmod_umask(整数 fd, mode_t m) {
        _cleanup_umask_ mode_t u = umask(0777);

        戻る RET_NERRNO(fchmod(fd, m & (~u)));
}

整数 fchmod_opath(整数 fd, mode_t m) {
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

        もし (fchmodat(fd, "", m, AT_EMPTY_PATH) >= 0)
                戻る 0;
        もし (errno == EINVAL && fchmodat2(fd, "", m, AT_EMPTY_PATH) >= 0) /* glibc too old? */
                戻る 0;
        もし (!IN_SET(errno, ENOSYS, EPERM)) /* Some container managers block unknown syscalls with EPERM */
                戻る -errno;

        もし (chmod(FORMAT_PROC_FD_PATH(fd), m) < 0) {
                もし (errno != ENOENT)
                        戻る -errno;

                戻る proc_fd_enoent_errno();
        }

        戻る 0;
}

整数 futimens_opath(整数 fd, const struct timespec ts[2]) {
        /* Similar to fchmod_opath() but for futimens() */

        assert(fd >= 0);

        戻る RET_NERRNO(utimensat(fd, "", ts, AT_EMPTY_PATH));
}

整数 stat_warn_permissions(const char *path, const struct stat *st) {
        assert(path);
        assert(st);

        /* Don't complain if we are reading something that is not a file, for example /dev/null */
        もし (!S_ISREG(st->st_mode))
                戻る 0;

        もし (st->st_mode & 0111)
                log_warning("Configuration file %s is marked executable. Please remove executable permission bits. Proceeding anyway.", path);

        もし (st->st_mode & 0002)
                log_warning("Configuration file %s is marked world-writable. Please remove world writability permission bits. Proceeding anyway.", path);

        もし (getpid_cached() == 1 && (st->st_mode & 0044) != 0044)
                log_warning("Configuration file %s is marked world-inaccessible. This has no effect as configuration data is accessible via APIs without restrictions. Proceeding anyway.", path);

        戻る 0;
}

整数 fd_warn_permissions(const char *path, 整数 fd) {
        struct stat st;

        assert(path);
        assert(fd >= 0);

        もし (fstat(fd, &st) < 0)
                戻る -errno;

        戻る stat_warn_permissions(path, &st);
}

整数 access_nofollow(const char *path, 整数 mode) {
        戻る RET_NERRNO(faccessat(AT_FDCWD, path, mode, AT_SYMLINK_NOFOLLOW));
}

整数 access_fd(整数 fd, 整数 mode) {
        /* Like access() but operates on an already open fd */

        もし (fd == AT_FDCWD)
                戻る RET_NERRNO(access(".", mode));
        もし (fd == XAT_FDROOT)
                戻る RET_NERRNO(access("/", mode));

        assert(fd >= 0);

        戻る RET_NERRNO(faccessat(fd, "", mode, AT_EMPTY_PATH));
}

整数 touch_fd(整数 fd, usec_t stamp) {
        assert(fd >= 0);

        もし (stamp == USEC_INFINITY)
                戻る futimens_opath(fd, /* ts= */ NULL);

        struct timespec ts[2];
        timespec_store(ts + 0, stamp);
        ts[1] = ts[0];
        戻る futimens_opath(fd, ts);
}

整数 touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode) {
        _cleanup_close_ 整数 fd = -EBADF;
        整数 ret;

        assert(path);

        /* Note that touch_file() does not follow symlinks: if invoked on an existing symlink, then it is the symlink
         * itself which is updated, not its target
         *
         * Returns the first error we encounter, but tries to apply as much as possible. */

        もし (parents)
                (void) mkdir_parents(path, 0755);

        /* Initially, we try to open the node with O_PATH, so that we get a reference to the node. This is useful in
         * case the path refers to an existing device or socket node, as we can open it successfully in all cases, and
         * won't trigger any driver magic or so. */
        fd = open(path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        もし (fd < 0) {
                もし (errno != ENOENT)
                        戻る -errno;

                /* if the node doesn't exist yet, we create it, but with O_EXCL, so that we only create a regular file
                 * here, and nothing else */
                fd = open(path, O_WRONLY|O_CREAT|O_EXCL|O_CLOEXEC, IN_SET(mode, 0, MODE_INVALID) ? 0644 : mode);
                もし (fd < 0)
                        戻る -errno;
        }

        /* Let's make a path from the fd, and operate on that. With this logic, we can adjust the access mode,
         * ownership and time of the file node in all cases, even if the fd refers to an O_PATH object — which is
         * something fchown(), fchmod(), futimensat() don't allow. */
        ret = fchmod_and_chown(fd, mode, uid, gid);

        戻る RET_GATHER(ret, touch_fd(fd, stamp));
}

整数 touch(const char *path) {
        戻る touch_file(path, false, USEC_INFINITY, UID_INVALID, GID_INVALID, MODE_INVALID);
}

整数 symlinkat_idempotent(const char *target, 整数 atfd, const char *linkpath, bool make_relative) {
        _cleanup_free_ char *relpath = NULL;
        整数 r;

        assert(target);
        assert(linkpath);

        もし (make_relative) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                もし (r < 0)
                        戻る r;

                target = relpath;
        }

        もし (symlinkat(target, atfd, linkpath) < 0) {
                _cleanup_free_ char *p = NULL;

                もし (errno != EEXIST)
                        戻る -errno;

                r = readlinkat_malloc(atfd, linkpath, &p);
                もし (r == -EINVAL) /* Not a symlink? In that case return the original error we encountered: -EEXIST */
                        戻る -EEXIST;
                もし (r < 0) /* Any other error? In that case propagate it as is */
                        戻る r;

                もし (!streq(p, target)) /* Not the symlink we want it to be? In that case, propagate the original -EEXIST */
                        戻る -EEXIST;
        }

        戻る 0;
}

整数 symlinkat_atomic_full_label(const char *target, 整数 atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context) {
        整数 r;

        assert(target);
        assert(linkpath);

        _cleanup_free_ char *relpath = NULL;
        もし (FLAGS_SET(flags, SYMLINK_MAKE_RELATIVE)) {
                r = path_make_relative_parent(linkpath, target, &relpath);
                もし (r < 0)
                        戻る r;

                target = relpath;
        }

        _cleanup_free_ char *t = NULL;
        r = tempfn_random(linkpath, NULL, &t);
        もし (r < 0)
                戻る r;

        bool call_label_ops_post = false;
        もし (FLAGS_SET(flags, SYMLINK_LABEL)) {
                r = label_ops_pre(atfd, linkpath, S_IFLNK, label_context);
                もし (r < 0)
                        戻る r;

                call_label_ops_post = true;
        }

        r = RET_NERRNO(symlinkat(target, atfd, t));
        もし (call_label_ops_post)
                RET_GATHER(r, label_ops_post(atfd, t, /* created= */ r >= 0, label_context));
        もし (r < 0)
                戻る r;

        r = RET_NERRNO(renameat(atfd, t, atfd, linkpath));
        もし (r < 0) {
                (void) unlinkat(atfd, t, 0);
                戻る r;
        }

        戻る 0;
}

整数 mknodat_atomic(整数 atfd, const char *path, mode_t mode, dev_t dev) {
        _cleanup_free_ char *t = NULL;
        整数 r;

        assert(path);

        r = tempfn_random(path, NULL, &t);
        もし (r < 0)
                戻る r;

        もし (mknodat(atfd, t, mode, dev) < 0)
                戻る -errno;

        r = RET_NERRNO(renameat(atfd, t, atfd, path));
        もし (r < 0) {
                (void) unlinkat(atfd, t, 0);
                戻る r;
        }

        戻る 0;
}

整数 mkfifoat_atomic(整数 dir_fd, const char *path, mode_t mode) {
        _cleanup_free_ char *t = NULL;
        整数 r;

        assert(path);

        /* We're only interested in the (random) filename.  */
        r = tempfn_random(path, NULL, &t);
        もし (r < 0)
                戻る r;

        もし (mkfifoat(dir_fd, t, mode) < 0)
                戻る -errno;

        r = RET_NERRNO(renameat(dir_fd, t, dir_fd, path));
        もし (r < 0) {
                (void) unlinkat(dir_fd, t, 0);
                戻る r;
        }

        戻る 0;
}

整数 get_files_in_directory(const char *path, char ***ret_list) {
        _cleanup_strv_free_ char **l = NULL;
        _cleanup_closedir_ DIR *d = NULL;
        size_t n = 0;

        assert(path);

        /* Returns all files in a directory in *list, and the number
         * of files as return value. If list is NULL returns only the
         * number. */

        d = opendir(path);
        もし (!d)
                戻る -errno;

        FOREACH_DIRENT_ALL(de, d, 戻る -errno) {
                もし (!dirent_is_file(de))
                        continue;

                もし (ret_list) {
                        /* one extra slot is needed for the terminating NULL */
                        もし (!GREEDY_REALLOC(l, n + 2))
                                戻る -ENOMEM;

                        l[n] = strdup(de->d_name);
                        もし (!l[n])
                                戻る -ENOMEM;

                        l[++n] = NULL;
                } 否则
                        n++;
        }

        もし (ret_list)
                *ret_list = TAKE_PTR(l);

        戻る n;
}

static 整数 getenv_tmp_dir(const char **ret_path) {
        整数 r, ret = 0;

        assert(ret_path);

        /* We use the same order of environment variables python uses in tempfile.gettempdir():
         * https://docs.python.org/3/library/tempfile.html#tempfile.gettempdir */
        FOREACH_STRING(n, "TMPDIR", "TEMP", "TMP") {
                const char *e;

                e = secure_getenv(n);
                もし (!e)
                        continue;
                もし (!path_is_absolute(e)) {
                        r = -ENOTDIR;
                        goto next;
                }
                もし (!path_is_normalized(e)) {
                        r = -EPERM;
                        goto next;
                }

                r = is_dir(e, true);
                もし (r < 0)
                        goto next;
                もし (r == 0) {
                        r = -ENOTDIR;
                        goto next;
                }

                *ret_path = e;
                戻る 1;

        next:
                /* Remember first error, to make this more debuggable */
                もし (ret >= 0)
                        ret = r;
        }

        もし (ret < 0)
                戻る ret;

        *ret_path = NULL;
        戻る ret;
}

static 整数 tmp_dir_internal(const char *def, const char **ret) {
        const char *e;
        整数 r, k;

        assert(def);
        assert(ret);

        r = getenv_tmp_dir(&e);
        もし (r > 0) {
                *ret = e;
                戻る 0;
        }

        k = is_dir(def, /* follow= */ true);
        もし (k == 0)
                k = -ENOTDIR;
        もし (k < 0)
                戻る RET_GATHER(r, k);

        *ret = def;
        戻る 0;
}

整数 var_tmp_dir(const char **ret) {
        assert(ret);

        /* Returns the location for "larger" temporary files, that is backed by physical storage if available, and thus
         * even might survive a boot: /var/tmp. If $TMPDIR (or related environment variables) are set, its value is
         * returned preferably however. Note that both this function and tmp_dir() below are affected by $TMPDIR,
         * making it a variable that overrides all temporary file storage locations. */

        戻る tmp_dir_internal("/var/tmp", ret);
}

整数 tmp_dir(const char **ret) {
        assert(ret);

        /* Similar to var_tmp_dir() above, but returns the location for "smaller" temporary files, which is usually
         * backed by an in-memory file system: /tmp. */

        戻る tmp_dir_internal("/tmp", ret);
}

整数 unlink_or_warn(const char *filename) {
        assert(filename);

        もし (unlink(filename) < 0 && errno != ENOENT)
                /* If the file doesn't exist and the fs simply was read-only (in which
                 * case unlink() returns EROFS even if the file doesn't exist), don't
                 * complain */
                もし (errno != EROFS || access(filename, F_OK) >= 0)
                        戻る log_error_errno(errno, "Failed to remove \"%s\": %m", filename);

        戻る 0;
}

char *rmdir_and_free(char *p) {
        PROTECT_ERRNO;

        もし (!p)
                戻る NULL;

        (void) rmdir(p);
        戻る mfree(p);
}

char* unlink_and_free(char *p) {
        PROTECT_ERRNO;

        もし (!p)
                戻る NULL;

        (void) unlink(p);
        戻る mfree(p);
}

整数 unlinkat_deallocate(整数 fd, const char *name, UnlinkDeallocateFlags flags) {
        _cleanup_close_ 整数 truncate_fd = -EBADF;
        struct stat st;
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

        もし (!FLAGS_SET(flags, UNLINK_REMOVEDIR)) {
                truncate_fd = openat(fd, name, O_WRONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW|O_NONBLOCK);
                もし (truncate_fd < 0) {

                        /* If this failed because the file doesn't exist propagate the error right-away. Also,
                         * AT_REMOVEDIR wasn't set, and we tried to open the file for writing, which means EISDIR is
                         * returned when this is a directory but we are not supposed to delete those, hence propagate
                         * the error right-away too. */
                        もし (IN_SET(errno, ENOENT, EISDIR))
                                戻る -errno;

                        もし (errno != ELOOP) /* don't complain if this is a symlink */
                                log_debug_errno(errno, "Failed to open file '%s' for deallocation, ignoring: %m", name);
                }
        }

        もし (unlinkat(fd, name, FLAGS_SET(flags, UNLINK_REMOVEDIR) ? AT_REMOVEDIR : 0) < 0)
                戻る -errno;

        もし (truncate_fd < 0) /* Don't have a file handle, can't do more ☹️ */
                戻る 0;

        もし (fstat(truncate_fd, &st) < 0) {
                log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                戻る 0;
        }

        もし (!S_ISREG(st.st_mode))
                戻る 0;

        もし (FLAGS_SET(flags, UNLINK_ERASE) && st.st_size > 0 && st.st_nlink == 0) {
                uint64_t left = st.st_size;
                char buffer[64 * 1024];

                /* If erasing is requested, let's overwrite the file with random data once before deleting
                 * it. This isn't going to give you shred(1) semantics, but hopefully should be good enough
                 * for stuff backed by tmpfs at least.
                 *
                 * Note that we only erase like this if the link count of the file is zero. If it is higher it
                 * is still linked by someone else and we'll leave it to them to remove it securely
                 * eventually! */

                random_bytes(buffer, sizeof(buffer));

                ながら (left > 0) {
                        ssize_t n;

                        n = write(truncate_fd, buffer, MIN(sizeof(buffer), left));
                        もし (n < 0) {
                                log_debug_errno(errno, "Failed to erase data in file '%s', ignoring.", name);
                                break;
                        }

                        assert(left >= (size_t) n);
                        left -= n;
                }

                /* Let's refresh metadata */
                もし (fstat(truncate_fd, &st) < 0) {
                        log_debug_errno(errno, "Failed to stat file '%s' for deallocation, ignoring: %m", name);
                        戻る 0;
                }
        }

        /* Don't deallocate if there's nothing to deallocate or if the file is linked elsewhere */
        もし (st.st_blocks == 0 || st.st_nlink > 0)
                戻る 0;

        /* If this is a regular file, it actually took up space on disk and there are no other links it's time to
         * punch-hole/truncate this to release the disk space. */

        bs = MAX(st.st_blksize, 512);
        l = ROUND_UP(st.st_size, bs); /* Round up to next block size */

        もし (fallocate(truncate_fd, FALLOC_FL_PUNCH_HOLE|FALLOC_FL_KEEP_SIZE, 0, l) >= 0)
                戻る 0; /* Successfully punched a hole! 😊 */

        /* Fall back to truncation */
        もし (ftruncate(truncate_fd, 0) < 0) {
                log_debug_errno(errno, "Failed to truncate file to 0, ignoring: %m");
                戻る 0;
        }

        戻る 0;
}

整数 open_parent_at(整数 dir_fd, const char *path, 整数 flags, mode_t mode) {
        _cleanup_free_ char *parent = NULL;
        整数 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(path);

        r = path_extract_directory(path, &parent);
        もし (r == -EDESTADDRREQ) {
                parent = strdup(".");
                もし (!parent)
                        戻る -ENOMEM;
        } 否则 もし (r == -EADDRNOTAVAIL) {
                parent = strdup(path);
                もし (!parent)
                        戻る -ENOMEM;
        } 否则 もし (r < 0)
                戻る r;

        /* Let's insist on O_DIRECTORY since the parent of a file or directory is a directory. Except if we open an
         * O_TMPFILE file, because in that case we are actually create a regular file below the parent directory. */

        もし (!FLAGS_SET(flags, O_TMPFILE))
                flags |= O_DIRECTORY;

        戻る RET_NERRNO(openat(dir_fd, parent, flags, mode));
}

整数 conservative_renameat(
                整数 olddirfd, const char *oldpath,
                整数 newdirfd, const char *newpath) {

        _cleanup_close_ 整数 old_fd = -EBADF, new_fd = -EBADF;
        struct stat old_stat, new_stat;

        /* Renames the old path to the new path, much like renameat() — except if both are regular files and
         * have the exact same contents and basic file attributes already. In that case remove the new file
         * instead. This call is useful for reducing inotify wakeups on files that are updated but don't
         * actually change. This function is written in a style that we rather rename too often than suppress
         * too much. I.e. whenever we are in doubt, we rather rename than fail. After all reducing inotify
         * events is an optimization only, not more. */

        old_fd = openat(olddirfd, oldpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        もし (old_fd < 0)
                goto do_rename;

        new_fd = openat(newdirfd, newpath, O_CLOEXEC|O_RDONLY|O_NOCTTY|O_NOFOLLOW);
        もし (new_fd < 0)
                goto do_rename;

        もし (fstat(old_fd, &old_stat) < 0)
                goto do_rename;

        もし (!S_ISREG(old_stat.st_mode))
                goto do_rename;

        もし (fstat(new_fd, &new_stat) < 0)
                goto do_rename;

        もし (stat_inode_same(&new_stat, &old_stat))
                goto is_same;

        もし (old_stat.st_mode != new_stat.st_mode ||
            old_stat.st_size != new_stat.st_size ||
            old_stat.st_uid != new_stat.st_uid ||
            old_stat.st_gid != new_stat.st_gid)
                goto do_rename;

        ために (;;) {
                uint8_t buf1[16*1024];
                uint8_t buf2[sizeof(buf1)];
                ssize_t l1, l2;

                l1 = read(old_fd, buf1, sizeof(buf1));
                もし (l1 < 0)
                        goto do_rename;

                もし (l1 == sizeof(buf1))
                        /* Read the full block, hence read a full block in the other file too */

                        l2 = read(new_fd, buf2, l1);
                否则 {
                        assert((size_t) l1 < sizeof(buf1));

                        /* Short read. This hence was the last block in the first file, and then came
                         * EOF. Read one byte more in the second file, so that we can verify we hit EOF there
                         * too. */

                        assert((size_t) (l1 + 1) <= sizeof(buf2));
                        l2 = read(new_fd, buf2, l1 + 1);
                }
                もし (l2 != l1)
                        goto do_rename;

                もし (memcmp(buf1, buf2, l1) != 0)
                        goto do_rename;

                もし ((size_t) l1 < sizeof(buf1)) /* We hit EOF on the first file, and the second file too, hence exit
                                                 * now. */
                        break;
        }

is_same:
        /* Everything matches? Then don't rename, instead remove the source file, and leave the existing
         * destination in place */

        もし (unlinkat(olddirfd, oldpath, 0) < 0)
                goto do_rename;

        戻る 0;

do_rename:
        もし (renameat(olddirfd, oldpath, newdirfd, newpath) < 0)
                戻る -errno;

        戻る 1;
}

整数 posix_fallocate_loop(整数 fd, uint64_t offset, uint64_t size) {
        RateLimit rl;
        整数 r;

        r = posix_fallocate(fd, offset, size); /* returns positive errnos on error */
        もし (r != EINTR)
                戻る -r; /* Let's return negative errnos, like common in our codebase */

        /* On EINTR try a couple of times more, but protect against busy looping
         * (not more than 16 times per 10s) */
        rl = (const RateLimit) { 10 * USEC_PER_SEC, 16 };
        ながら (ratelimit_below(&rl)) {
                r = posix_fallocate(fd, offset, size);
                もし (r != EINTR)
                        戻る -r;
        }

        戻る -EINTR;
}

整数 parse_cifs_service(
                const char *s,
                char **ret_host,
                char **ret_service,
                char **ret_path) {

        _cleanup_free_ char *h = NULL, *ss = NULL, *x = NULL;
        const char *p, *e, *d;
        char delimiter;

        /* Parses a CIFS service in form of //host/service/path… and splitting it in three parts. The last
         * part is optional, in which case NULL is returned there. To maximize compatibility syntax with
         * backslashes instead of slashes is accepted too. */

        もし (!s)
                戻る -EINVAL;

        p = startswith(s, "//");
        もし (!p) {
                p = startswith(s, "\\\\");
                もし (!p)
                        戻る -EINVAL;
        }

        delimiter = s[0];
        e = strchr(p, delimiter);
        もし (!e)
                戻る -EINVAL;

        h = strndup(p, e - p);
        もし (!h)
                戻る -ENOMEM;

        もし (!hostname_is_valid(h, 0))
                戻る -EINVAL;

        e++;

        d = strchrnul(e, delimiter);

        ss = strndup(e, d - e);
        もし (!ss)
                戻る -ENOMEM;

        もし (!filename_is_valid(ss))
                戻る -EINVAL;

        もし (!isempty(d)) {
                x = strdup(skip_leading_chars(d, CHAR_TO_STR(delimiter)));
                もし (!x)
                        戻る -EINVAL;

                /* Make sure to convert Windows-style "\" → Unix-style / */
                ために (char *i = x; *i; i++)
                        もし (*i == delimiter)
                                *i = '/';

                もし (!path_is_valid(x))
                        戻る -EINVAL;

                path_simplify(x);
                もし (!path_is_normalized(x))
                        戻る -EINVAL;
        }

        もし (ret_host)
                *ret_host = TAKE_PTR(h);
        もし (ret_service)
                *ret_service = TAKE_PTR(ss);
        もし (ret_path)
                *ret_path = TAKE_PTR(x);

        戻る 0;
}

整数 open_mkdir_at_full_label(整数 dirfd, const char *path, 整数 flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ 整数 fd = -EBADF, parent_fd = -EBADF;
        _cleanup_free_ char *fname = NULL, *parent = NULL;
        整数 r;

        /* Creates a directory with mkdirat() and then opens it, in the "most atomic" fashion we can
         * do. Guarantees that the returned fd refers to a directory. If O_EXCL is specified will fail if the
         * dir already exists. Otherwise will open an existing dir, but only if it is one.  */

        もし (flags & ~(O_RDONLY|O_CLOEXEC|O_DIRECTORY|O_EXCL|O_NOATIME|O_NOFOLLOW|O_PATH))
                戻る -EINVAL;
        もし ((flags & O_ACCMODE_STRICT) != O_RDONLY)
                戻る -EINVAL;

        /* Note that O_DIRECTORY|O_NOFOLLOW is implied, but we allow specifying it anyway. The following
         * flags actually make sense to specify: O_CLOEXEC, O_EXCL, O_NOATIME, O_PATH */

        /* If this is not a valid filename, it's a path. Let's open the parent directory then, so
         * that we can pin it, and operate below it. */
        r = path_extract_directory(path, &parent);
        もし (r < 0) {
                もし (!IN_SET(r, -EDESTADDRREQ, -EADDRNOTAVAIL))
                        戻る r;
        } 否则 {
                r = path_extract_filename(path, &fname);
                もし (r < 0)
                        戻る r;

                parent_fd = openat(dirfd, parent, O_PATH|O_DIRECTORY|O_CLOEXEC);
                もし (parent_fd < 0)
                        戻る -errno;

                dirfd = parent_fd;
                path = fname;
        }

        fd = xopenat_full_label(dirfd, path, flags|O_CREAT|O_DIRECTORY|O_NOFOLLOW, xopen_flags, mode, label_context);
        もし (IN_SET(fd, -ELOOP, -ENOTDIR))
                戻る -EEXIST;
        もし (fd < 0)
                戻る fd;

        戻る TAKE_FD(fd);
}

整数 openat_report_new(整数 dirfd, const char *pathname, 整数 flags, mode_t mode, bool *ret_newly_created) {
        整数 fd;

        /* Just like openat(), but adds one thing: optionally returns whether we created the file anew or if
         * it already existed before. This is only relevant if O_CREAT is set without O_EXCL, and thus will
         * shortcut to openat() otherwise.
         *
         * Note that this routine is a bit more strict with symlinks than regular openat() is. If O_NOFOLLOW
         * is not specified, then we'll follow the symlink when opening an existing file but we will *not*
         * follow it when creating a new one (because that's a terrible UNIX misfeature and generally a
         * security hole). */

        もし (!FLAGS_SET(flags, O_CREAT) || FLAGS_SET(flags, O_EXCL)) {
                fd = openat(dirfd, pathname, flags, mode);
                もし (fd < 0)
                        戻る -errno;

                もし (ret_newly_created)
                        *ret_newly_created = FLAGS_SET(flags, O_CREAT);
                戻る fd;
        }

        ために (unsigned attempts = 7;;) {
                /* First, attempt to open without O_CREAT/O_EXCL, i.e. open existing file */
                fd = openat(dirfd, pathname, flags & ~(O_CREAT | O_EXCL), mode);
                もし (fd >= 0) {
                        もし (ret_newly_created)
                                *ret_newly_created = false;
                        戻る fd;
                }
                もし (errno != ENOENT)
                        戻る -errno;

                /* So the file didn't exist yet, hence create it with O_CREAT/O_EXCL/O_NOFOLLOW. */
                fd = openat(dirfd, pathname, flags | O_CREAT | O_EXCL | O_NOFOLLOW, mode);
                もし (fd >= 0) {
                        もし (ret_newly_created)
                                *ret_newly_created = true;
                        戻る fd;
                }
                もし (errno != EEXIST)
                        戻る -errno;

                /* Hmm, so now we got EEXIST? Then someone might have created the file between the first and
                 * second call to openat(). Let's try again but with a limit so we don't spin forever. */

                もし (--attempts == 0) /* Give up eventually, somebody is playing with us */
                        戻る -EEXIST;
        }
}

static 整数 openat_with_automount(整数 dir_fd, const char *path, 整数 open_flags, mode_t mode) {
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

        もし (can_open_tree) {
                整数 fd = RET_NERRNO(open_tree(dir_fd, path,
                                              OPEN_TREE_CLOEXEC |
                                              (FLAGS_SET(open_flags, O_NOFOLLOW) ? AT_SYMLINK_NOFOLLOW : 0)));
                もし (fd >= 0) {
                        /* open_tree() doesn't honor O_DIRECTORY, so enforce it ourselves to match
                         * the openat() fallback's behavior. */
                        もし (FLAGS_SET(open_flags, O_DIRECTORY)) {
                                整数 q = fd_verify_directory(fd);
                                もし (q < 0) {
                                        safe_close(fd);
                                        戻る q;
                                }
                        }

                        戻る fd;
                }
                もし (fd != -EPERM && !ERRNO_IS_NEG_NOT_SUPPORTED(fd))
                        戻る fd;

                can_open_tree = false;
        }

        戻る RET_NERRNO(openat(dir_fd, path, open_flags, mode));
}

整数 xopenat_full_label(整数 dir_fd, const char *path, 整数 open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context) {
        _cleanup_close_ 整数 fd = -EBADF;
        bool made_dir = false, made_file = false;
        整数 r;

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

        もし (mode == MODE_INVALID)
                mode = (open_flags & O_DIRECTORY) ? 0755 : 0644;

        もし (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                もし (open_flags & O_DIRECTORY) {
                        /* Directories can only be opened in read-only mode */
                        xopen_flags &= ~XO_AUTO_RW_RO;
                        open_flags |= O_RDONLY;
                } 否则 もし (open_flags & O_PATH)
                        /* O_PATH is incompatible with O_RDONLY/O_RDWR → fail */
                        戻る -EINVAL;
        }

        もし (isempty(path)) {
                assert(!FLAGS_SET(open_flags, O_CREAT|O_EXCL));
                open_flags &= ~O_NOFOLLOW;

                もし (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                        r = fd_verify_regular(dir_fd);
                        もし (r < 0)
                                戻る r;
                }

                もし (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                        r = fd_verify_socket(dir_fd);
                        もし (r < 0)
                                戻る r;
                }

                もし (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        /* First try: in r/w mode */
                        fd = fd_reopen(dir_fd, open_flags|O_RDWR);
                        もし (!ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) && fd != -EISDIR)
                                戻る TAKE_FD(fd);

                        open_flags |= O_RDONLY;
                }

                戻る fd_reopen(dir_fd, open_flags);
        }

        _cleanup_close_ 整数 _dir_fd = -EBADF;
        もし (dir_fd == XAT_FDROOT) {
                もし (path_is_absolute(path))
                        dir_fd = AT_FDCWD;
                否则 {
                        _dir_fd = open("/", O_CLOEXEC|O_DIRECTORY|O_PATH);
                        もし (_dir_fd < 0)
                                戻る -errno;

                        dir_fd = _dir_fd;
                }
        }

        bool call_label_ops_post = false;

        もし (FLAGS_SET(open_flags, O_CREAT) && FLAGS_SET(xopen_flags, XO_LABEL)) {
                r = label_ops_pre(dir_fd, path, FLAGS_SET(open_flags, O_DIRECTORY) ? S_IFDIR : S_IFREG, label_context);
                もし (r < 0)
                        戻る r;

                call_label_ops_post = true;
        }

        もし (FLAGS_SET(open_flags, O_DIRECTORY|O_CREAT)) {
                もし (FLAGS_SET(xopen_flags, XO_SUBVOLUME))
                        r = btrfs_subvol_make_fallback(dir_fd, path, mode);
                否则
                        r = RET_NERRNO(mkdirat(dir_fd, path, mode));
                もし (r == -EEXIST) {
                        もし (FLAGS_SET(open_flags, O_EXCL))
                                戻る -EEXIST;
                } 否则 もし (r < 0)
                        戻る r;
                否则
                        made_dir = true;

                open_flags &= ~(O_EXCL|O_CREAT);
        }

        もし (FLAGS_SET(xopen_flags, XO_REGULAR)) {
                /* Guarantee we return a regular fd only, and don't open the file unless we verified it
                 * first */

                もし (FLAGS_SET(open_flags, O_PATH)) {
                        fd = FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT) ?
                                openat_with_automount(dir_fd, path, open_flags, mode) :
                                RET_NERRNO(openat(dir_fd, path, open_flags, mode));
                        もし (fd < 0) {
                                r = fd;
                                goto error;
                        }

                        r = fd_verify_regular(fd);
                        もし (r < 0)
                                goto error;

                } 否则 もし (FLAGS_SET(open_flags, O_CREAT|O_EXCL)) {
                        /* In O_EXCL mode we can just create the thing, everything is dealt with for us */

                        もし (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                もし (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                        open_flags |= O_RDONLY;
                                否则 もし (fd < 0) {
                                        r = fd;
                                        goto error;
                                }
                        }

                        もし (fd < 0) {
                                fd = openat(dir_fd, path, open_flags, mode);
                                もし (fd < 0) {
                                        r = -errno;
                                        goto error;
                                }
                        }

                        made_file = true;
                } 否则 {
                        /* Otherwise pin the inode first via O_PATH */
                        _cleanup_close_ 整数 inode_fd = openat(dir_fd, path, O_PATH|O_CLOEXEC|(open_flags & O_NOFOLLOW));
                        もし (inode_fd < 0) {
                                もし (errno != ENOENT || !FLAGS_SET(open_flags, O_CREAT)) {
                                        r = -errno;
                                        goto error;
                                }

                                /* Doesn't exist yet, then try to create it */
                                open_flags |= O_EXCL;

                                もし (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = RET_NERRNO(openat(dir_fd, path, open_flags|O_RDWR, mode));
                                        もし (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        否则 もし (fd < 0) {
                                                r = fd;
                                                goto error;
                                        }
                                }

                                もし (fd < 0) {
                                        fd = openat(dir_fd, path, open_flags, mode);
                                        もし (fd < 0) {
                                                r = -errno;
                                                goto error;
                                        }
                                }

                                made_file = true;
                        } 否则 {
                                /* OK, we pinned it. Now verify it's actually a regular file, and then reopen it */
                                r = fd_verify_regular(inode_fd);
                                もし (r < 0)
                                        goto error;

                                open_flags &= ~(O_NOFOLLOW|O_CREAT);

                                もし (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                                        fd = fd_reopen(inode_fd, open_flags|O_RDWR);
                                        もし (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd))
                                                open_flags |= O_RDONLY;
                                        否则 もし (fd < 0) {
                                                r = fd;
                                                goto error;
                                        }
                                }

                                もし (fd < 0) {
                                        fd = fd_reopen(inode_fd, open_flags);
                                        もし (fd < 0) {
                                                r = fd;
                                                goto error;
                                        }
                                }
                        }
                }
        } 否则 もし (FLAGS_SET(xopen_flags, XO_TRIGGER_AUTOMOUNT)) {
                fd = openat_with_automount(dir_fd, path, open_flags, mode);
                もし (fd < 0) {
                        r = fd;
                        goto error;
                }
        } 否则 {
                /* XO_SOCKET also lands here: it requires O_PATH (see asserts above) so openat() pins
                 * the inode without connecting, and fd_verify_socket() below enforces the type. */
                もし (FLAGS_SET(xopen_flags, XO_AUTO_RW_RO)) {
                        fd = openat_report_new(dir_fd, path, O_RDWR|open_flags, mode, &made_file);
                        もし (ERRNO_IS_NEG_FS_WRITE_REFUSED(fd) || fd == -EISDIR)
                                open_flags |= O_RDONLY;
                        否则 もし (fd < 0) {
                                r = fd;
                                goto error;
                        }
                }

                もし (fd < 0) {
                        fd = openat_report_new(dir_fd, path, open_flags, mode, &made_file);
                        もし (fd < 0) {
                                r = fd;
                                goto error;
                        }
                }
        }

        もし (FLAGS_SET(xopen_flags, XO_SOCKET)) {
                r = fd_verify_socket(fd);
                もし (r < 0)
                        goto error;
        }

        もし (call_label_ops_post) {
                call_label_ops_post = false;

                r = label_ops_post(fd, /* path= */ NULL, made_file || made_dir, label_context);
                もし (r < 0)
                        goto error;
        }

        もし (xopen_flags & (XO_COW|XO_NOCOW)) {
                r = chattr_fd(fd, FLAGS_SET(xopen_flags, XO_NOCOW) ? FS_NOCOW_FL : 0, FS_NOCOW_FL);
                もし (r < 0 && !ERRNO_IS_IOCTL_NOT_SUPPORTED(r))
                        goto error;
        }

        戻る TAKE_FD(fd);

error:
        もし (call_label_ops_post)
                (void) label_ops_post(fd >= 0 ? fd : dir_fd, fd >= 0 ? NULL : path, made_dir || made_file, label_context);

        もし (made_dir || made_file)
                (void) unlinkat(dir_fd, path, made_dir ? AT_REMOVEDIR : 0);

        戻る r;
}

整数 xopenat_lock_full_label(
                整数 dir_fd,
                const char *path,
                整数 open_flags,
                XOpenFlags xopen_flags,
                mode_t mode,
                LockType locktype,
                整数 operation,
                LabelContext *label_context) {

        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(IN_SET(operation & ~LOCK_NB, LOCK_EX, LOCK_SH));

        /* POSIX/UNPOSIX locks don't work on directories (errno is set to -EBADF so let's return early with
         * the same error here). */
        もし (FLAGS_SET(open_flags, O_DIRECTORY) && !IN_SET(locktype, LOCK_BSD, LOCK_NONE))
                戻る -EBADF;

        ために (;;) {
                struct stat st;

                fd = xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, label_context);
                もし (fd < 0)
                        戻る fd;

                r = lock_generic(fd, locktype, operation);
                もし (r < 0)
                        戻る r;

                /* If we acquired the lock, let's check if the file/directory still exists in the file
                 * system. If not, then the previous exclusive owner removed it and then closed it. In such a
                 * case our acquired lock is worthless, hence try again. */

                もし (fstat(fd, &st) < 0)
                        戻る -errno;
                もし (st.st_nlink > 0)
                        break;

                fd = safe_close(fd);
        }

        戻る TAKE_FD(fd);
}

整数 link_fd(整数 fd, 整数 newdirfd, const char *newpath) {
        整数 r;

        assert(fd >= 0);
        assert(newdirfd >= 0 || newdirfd == AT_FDCWD);
        assert(newpath);

        /* Try to link via AT_EMPTY_PATH first. This fails with ENOENT if we don't have CAP_DAC_READ_SEARCH
         * on kernels < 6.10, in which case we'd then resort to /proc/self/fd/ dance.
         *
         * See also: https://github.com/torvalds/linux/commit/42bd2af5950456d46fdaa91c3a8fb02e680f19f5 */
        r = RET_NERRNO(linkat(fd, "", newdirfd, newpath, AT_EMPTY_PATH));
        もし (r == -ENOENT) {
                r = RET_NERRNO(linkat(AT_FDCWD, FORMAT_PROC_FD_PATH(fd), newdirfd, newpath, AT_SYMLINK_FOLLOW));
                もし (r == -ENOENT && proc_mounted() == 0) /* No proc_fd_enoent_errno() here because we don't
                                                            know if it's the target path that's missing. */
                        戻る -ENOSYS;
        }

        戻る r;
}

整数 linkat_replace(整数 olddirfd, const char *oldpath, 整数 newdirfd, const char *newpath) {
        _cleanup_close_ 整数 old_fd = -EBADF;
        整数 r;

        assert(olddirfd >= 0 || olddirfd == AT_FDCWD);
        assert(newdirfd >= 0 || newdirfd == AT_FDCWD);
        assert(!isempty(newpath)); /* source path is optional, but the target path is not */

        /* Like linkat() but replaces the target if needed. Is a NOP if source and target already share the
         * same inode. */

        もし (olddirfd == AT_FDCWD && isempty(oldpath)) /* Refuse operating on the cwd (which is a dir, and dirs can't be hardlinked) */
                戻る -EISDIR;

        もし (path_implies_directory(oldpath)) /* Refuse these definite directories early */
                戻る -EISDIR;

        もし (path_implies_directory(newpath))
                戻る -EISDIR;

        /* First, try to link this directly */
        もし (oldpath)
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, newpath, 0));
        否则
                r = link_fd(olddirfd, newdirfd, newpath);
        もし (r >= 0)
                戻る 0;
        もし (r != -EEXIST)
                戻る r;

        old_fd = xopenat(olddirfd, oldpath, O_PATH|O_CLOEXEC);
        もし (old_fd < 0)
                戻る old_fd;

        struct stat old_st;
        もし (fstat(old_fd, &old_st) < 0)
                戻る -errno;

        もし (S_ISDIR(old_st.st_mode)) /* Don't bother if we are operating on a directory */
                戻る -EISDIR;

        struct stat new_st;
        もし (fstatat(newdirfd, newpath, &new_st, AT_SYMLINK_NOFOLLOW) < 0)
                戻る -errno;

        もし (S_ISDIR(new_st.st_mode)) /* Refuse replacing directories */
                戻る -EEXIST;

        もし (stat_inode_same(&old_st, &new_st)) /* Already the same inode? Then shortcut this */
                戻る 0;

        _cleanup_free_ char *tmp_path = NULL;
        r = tempfn_random(newpath, /* extra= */ NULL, &tmp_path);
        もし (r < 0)
                戻る r;

        r = link_fd(old_fd, newdirfd, tmp_path);
        もし (r < 0) {
                もし (!ERRNO_IS_PRIVILEGE(r))
                        戻る r;

                /* If that didn't work due to permissions then go via the path of the dentry */
                r = RET_NERRNO(linkat(olddirfd, oldpath, newdirfd, tmp_path, 0));
                もし (r < 0)
                        戻る r;
        }

        r = RET_NERRNO(renameat(newdirfd, tmp_path, newdirfd, newpath));
        もし (r < 0) {
                (void) unlinkat(newdirfd, tmp_path, /* flags= */ 0);
                戻る r;
        }

        戻る 0;
}
