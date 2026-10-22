/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/xattr.h>
#include <threads.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "nulstr-util.h"
#include "parse-util.h"
#include "sparse-endian.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"
#include "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
static thread_local bool have_xattrat = true;

static 整数 normalize_and_maybe_pin_inode(
                整数 *fd,
                const char **path,
                整数 *at_flags,
                整数 *ret_tfd,
                bool *ret_opath) {

        整数 r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        如果 (*fd == AT_FDCWD) {
                如果 (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                返回 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        如果 (!*path) {
                r = fd_is_opath(*fd);
                如果 (r < 0)
                        返回 r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                返回 0;
        }

        /* If both have been specified, then we go via O_PATH */

        整数 tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        如果 (tfd < 0)
                返回 -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        返回 0;
}

static ssize_t getxattr_pinned_internal(
                整数 fd,
                const char *path,
                整数 at_flags,
                bool by_procfs,
                const char *name,
                char *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(name);
        assert(buf || size == 0);

        如果 (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        否则
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        如果 (n < 0)
                返回 -errno;

        assert(size == 0 || (size_t) n <= size);
        返回 n;
}

整数 getxattr_at_malloc(
                整数 fd,
                const char *path,
                const char *name,
                整数 at_flags,
                char **ret,
                size_t *ret_size) {

        _cleanup_close_ 整数 opened_fd = -EBADF;
        bool by_procfs;
        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert(name);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        如果 (r < 0)
                返回 r;

        size_t l = 100;
        为 (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                如果 (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        返回 -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                如果 (!v)
                        返回 -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                如果 (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        如果 (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                返回 -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        如果 (ret_size)
                                *ret_size = (size_t) n;

                        返回 0;
                }
                如果 (n != -ERANGE)
                        返回 (整数) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                如果 (n < 0)
                        返回 (整数) n;

                l = (size_t) n;
        }
}

整数 getxattr_at_bool(整数 fd, const char *path, const char *name, 整数 at_flags) {
        _cleanup_free_ char *v = NULL;
        整数 r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        如果 (r < 0)
                返回 r;

        返回 parse_boolean(v);
}

整数 getxattr_at_strv(整数 fd, const char *path, const char *name, 整数 at_flags, char ***ret_strv) {
        _cleanup_free_ char *nulstr = NULL;
        size_t nulstr_size;
        整数 r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        如果 (r < 0)
                返回 r;

        _cleanup_strv_free_ char **l = strv_parse_nulstr(nulstr, nulstr_size);
        如果 (!l)
                返回 -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        返回 0;
}

static 整数 listxattr_pinned_internal(
                整数 fd,
                const char *path,
                整数 at_flags,
                bool by_procfs,
                char *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        如果 (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        否则
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        如果 (n < 0)
                返回 -errno;

        assert(size == 0 || (size_t) n <= size);

        如果 (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                返回 -E2BIG;

        返回 (整数) n;
}

整数 listxattr_at_malloc(整数 fd, const char *path, 整数 at_flags, char **ret) {
        _cleanup_close_ 整数 opened_fd = -EBADF;
        bool by_procfs;
        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        如果 (r < 0)
                返回 r;

        size_t l = 100;
        为 (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                如果 (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        返回 -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                如果 (!v)
                        返回 -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                如果 (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        返回 r;
                }
                如果 (r != -ERANGE)
                        返回 r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                如果 (r < 0)
                        返回 r;

                l = (size_t) r;
        }
}

整数 xsetxattr_full(
                整数 fd,
                const char *path,
                整数 at_flags,
                const char *name,
                const char *value,
                size_t size,
                整数 xattr_flags) {

        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        如果 (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ char *old_value = NULL;
        size_t old_size;

        如果 (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                返回 0;

        如果 (have_xattrat && !isempty(path)) {
                struct xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, sizeof(args)));
                如果 (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        返回 r;

                have_xattrat = false;
        }

        _cleanup_close_ 整数 opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        如果 (r < 0)
                返回 r;

        如果 (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        否则
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        如果 (r < 0)
                返回 -errno;

        返回 0;
}

整数 xsetxattr_strv(整数 fd, const char *path, 整数 at_flags, const char *name, char * const *l) {
        _cleanup_free_ char *nulstr = NULL;
        size_t size;
        整数 r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        如果 (r < 0)
                返回 r;

        返回 xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

整数 xremovexattr(整数 fd, const char *path, 整数 at_flags, const char *name) {
        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        如果 (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                如果 (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        返回 r;

                have_xattrat = false;
        }

        _cleanup_close_ 整数 tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        如果 (r < 0)
                返回 r;

        如果 (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        否则
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        如果 (r < 0)
                返回 -errno;

        返回 0;
}

static 整数 parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(sizeof(usec_t) == sizeof(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        如果 (!timestamp_is_set(u))
                返回 -EIO;

        *ret = u;
        返回 0;
}

整数 getcrtime_at(
                整数 fd,
                const char *path,
                整数 at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        struct statx sx;
        usec_t a, b;
        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_full(fd,
                        path,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        如果 (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        否则
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (char**) &le, &le_size);
        如果 (r >= 0) {
                如果 (le_size != sizeof(*le))
                        r = -EIO;
                否则
                        r = parse_crtime(*le, &b);
        }
        如果 (r < 0) {
                如果 (a != USEC_INFINITY) {
                        如果 (ret)
                                *ret = a;
                        返回 0;
                }

                返回 r;
        }

        如果 (ret)
                *ret = MIN(a, b);
        返回 0;
}

整数 fd_setcrtime(整数 fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        如果 (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        返回 xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const char*) &le, sizeof(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const char *name) {
        返回 STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const char *name) {
        返回 streq(ASSERT_PTR(name), "security.selinux");
}
