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

static ціле normalize_and_maybe_pin_inode(
                ціле *fd,
                const char **path,
                ціле *at_flags,
                ціле *ret_tfd,
                bool *ret_opath) {

        ціле r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        якщо (*fd == AT_FDCWD) {
                якщо (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                повернути 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        якщо (!*path) {
                r = fd_is_opath(*fd);
                якщо (r < 0)
                        повернути r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                повернути 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ціле tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        якщо (tfd < 0)
                повернути -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        повернути 0;
}

static ssize_t getxattr_pinned_internal(
                ціле fd,
                const char *path,
                ціле at_flags,
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

        якщо (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        інакше
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        якщо (n < 0)
                повернути -errno;

        assert(size == 0 || (size_t) n <= size);
        повернути n;
}

ціле getxattr_at_malloc(
                ціле fd,
                const char *path,
                const char *name,
                ціле at_flags,
                char **ret,
                size_t *ret_size) {

        _cleanup_close_ ціле opened_fd = -EBADF;
        bool by_procfs;
        ціле r;

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
        якщо (r < 0)
                повернути r;

        size_t l = 100;
        для (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                якщо (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        повернути -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                якщо (!v)
                        повернути -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                якщо (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        якщо (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                повернути -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        якщо (ret_size)
                                *ret_size = (size_t) n;

                        повернути 0;
                }
                якщо (n != -ERANGE)
                        повернути (ціле) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                якщо (n < 0)
                        повернути (ціле) n;

                l = (size_t) n;
        }
}

ціле getxattr_at_bool(ціле fd, const char *path, const char *name, ціле at_flags) {
        _cleanup_free_ char *v = NULL;
        ціле r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        якщо (r < 0)
                повернути r;

        повернути parse_boolean(v);
}

ціле getxattr_at_strv(ціле fd, const char *path, const char *name, ціле at_flags, char ***ret_strv) {
        _cleanup_free_ char *nulstr = NULL;
        size_t nulstr_size;
        ціле r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        якщо (r < 0)
                повернути r;

        _cleanup_strv_free_ char **l = strv_parse_nulstr(nulstr, nulstr_size);
        якщо (!l)
                повернути -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        повернути 0;
}

static ціле listxattr_pinned_internal(
                ціле fd,
                const char *path,
                ціле at_flags,
                bool by_procfs,
                char *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        якщо (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        інакше
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        якщо (n < 0)
                повернути -errno;

        assert(size == 0 || (size_t) n <= size);

        якщо (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                повернути -E2BIG;

        повернути (ціле) n;
}

ціле listxattr_at_malloc(ціле fd, const char *path, ціле at_flags, char **ret) {
        _cleanup_close_ ціле opened_fd = -EBADF;
        bool by_procfs;
        ціле r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        якщо (r < 0)
                повернути r;

        size_t l = 100;
        для (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                якщо (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        повернути -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                якщо (!v)
                        повернути -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                якщо (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        повернути r;
                }
                якщо (r != -ERANGE)
                        повернути r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                якщо (r < 0)
                        повернути r;

                l = (size_t) r;
        }
}

ціле xsetxattr_full(
                ціле fd,
                const char *path,
                ціле at_flags,
                const char *name,
                const char *value,
                size_t size,
                ціле xattr_flags) {

        ціле r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        якщо (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ char *old_value = NULL;
        size_t old_size;

        якщо (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                повернути 0;

        якщо (have_xattrat && !isempty(path)) {
                struct xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, sizeof(args)));
                якщо (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        повернути r;

                have_xattrat = false;
        }

        _cleanup_close_ ціле opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        якщо (r < 0)
                повернути r;

        якщо (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        інакше
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        якщо (r < 0)
                повернути -errno;

        повернути 0;
}

ціле xsetxattr_strv(ціле fd, const char *path, ціле at_flags, const char *name, char * const *l) {
        _cleanup_free_ char *nulstr = NULL;
        size_t size;
        ціле r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        якщо (r < 0)
                повернути r;

        повернути xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

ціле xremovexattr(ціле fd, const char *path, ціле at_flags, const char *name) {
        ціле r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        якщо (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                якщо (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        повернути r;

                have_xattrat = false;
        }

        _cleanup_close_ ціле tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        якщо (r < 0)
                повернути r;

        якщо (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        інакше
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        якщо (r < 0)
                повернути -errno;

        повернути 0;
}

static ціле parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(sizeof(usec_t) == sizeof(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        якщо (!timestamp_is_set(u))
                повернути -EIO;

        *ret = u;
        повернути 0;
}

ціле getcrtime_at(
                ціле fd,
                const char *path,
                ціле at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        struct statx sx;
        usec_t a, b;
        ціле r;

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
        якщо (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        інакше
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (char**) &le, &le_size);
        якщо (r >= 0) {
                якщо (le_size != sizeof(*le))
                        r = -EIO;
                інакше
                        r = parse_crtime(*le, &b);
        }
        якщо (r < 0) {
                якщо (a != USEC_INFINITY) {
                        якщо (ret)
                                *ret = a;
                        повернути 0;
                }

                повернути r;
        }

        якщо (ret)
                *ret = MIN(a, b);
        повернути 0;
}

ціле fd_setcrtime(ціле fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        якщо (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        повернути xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const char*) &le, sizeof(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const char *name) {
        повернути STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const char *name) {
        повернути streq(ASSERT_PTR(name), "security.selinux");
}
