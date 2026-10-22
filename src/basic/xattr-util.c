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

static 정수 normalize_and_maybe_pin_inode(
                정수 *fd,
                const char **path,
                정수 *at_flags,
                정수 *ret_tfd,
                bool *ret_opath) {

        정수 r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        만약 (*fd == AT_FDCWD) {
                만약 (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                반환 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        만약 (!*path) {
                r = fd_is_opath(*fd);
                만약 (r < 0)
                        반환 r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                반환 0;
        }

        /* If both have been specified, then we go via O_PATH */

        정수 tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        만약 (tfd < 0)
                반환 -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        반환 0;
}

static ssize_t getxattr_pinned_internal(
                정수 fd,
                const char *path,
                정수 at_flags,
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

        만약 (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        아니면
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        만약 (n < 0)
                반환 -errno;

        assert(size == 0 || (size_t) n <= size);
        반환 n;
}

정수 getxattr_at_malloc(
                정수 fd,
                const char *path,
                const char *name,
                정수 at_flags,
                char **ret,
                size_t *ret_size) {

        _cleanup_close_ 정수 opened_fd = -EBADF;
        bool by_procfs;
        정수 r;

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
        만약 (r < 0)
                반환 r;

        size_t l = 100;
        위해 (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                만약 (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        반환 -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                만약 (!v)
                        반환 -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                만약 (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        만약 (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                반환 -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        만약 (ret_size)
                                *ret_size = (size_t) n;

                        반환 0;
                }
                만약 (n != -ERANGE)
                        반환 (정수) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                만약 (n < 0)
                        반환 (정수) n;

                l = (size_t) n;
        }
}

정수 getxattr_at_bool(정수 fd, const char *path, const char *name, 정수 at_flags) {
        _cleanup_free_ char *v = NULL;
        정수 r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        만약 (r < 0)
                반환 r;

        반환 parse_boolean(v);
}

정수 getxattr_at_strv(정수 fd, const char *path, const char *name, 정수 at_flags, char ***ret_strv) {
        _cleanup_free_ char *nulstr = NULL;
        size_t nulstr_size;
        정수 r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        만약 (r < 0)
                반환 r;

        _cleanup_strv_free_ char **l = strv_parse_nulstr(nulstr, nulstr_size);
        만약 (!l)
                반환 -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        반환 0;
}

static 정수 listxattr_pinned_internal(
                정수 fd,
                const char *path,
                정수 at_flags,
                bool by_procfs,
                char *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        만약 (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        아니면
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        만약 (n < 0)
                반환 -errno;

        assert(size == 0 || (size_t) n <= size);

        만약 (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                반환 -E2BIG;

        반환 (정수) n;
}

정수 listxattr_at_malloc(정수 fd, const char *path, 정수 at_flags, char **ret) {
        _cleanup_close_ 정수 opened_fd = -EBADF;
        bool by_procfs;
        정수 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        만약 (r < 0)
                반환 r;

        size_t l = 100;
        위해 (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                만약 (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        반환 -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                만약 (!v)
                        반환 -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                만약 (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        반환 r;
                }
                만약 (r != -ERANGE)
                        반환 r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                만약 (r < 0)
                        반환 r;

                l = (size_t) r;
        }
}

정수 xsetxattr_full(
                정수 fd,
                const char *path,
                정수 at_flags,
                const char *name,
                const char *value,
                size_t size,
                정수 xattr_flags) {

        정수 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        만약 (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ char *old_value = NULL;
        size_t old_size;

        만약 (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                반환 0;

        만약 (have_xattrat && !isempty(path)) {
                struct xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, sizeof(args)));
                만약 (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        반환 r;

                have_xattrat = false;
        }

        _cleanup_close_ 정수 opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        만약 (r < 0)
                반환 r;

        만약 (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        아니면
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        만약 (r < 0)
                반환 -errno;

        반환 0;
}

정수 xsetxattr_strv(정수 fd, const char *path, 정수 at_flags, const char *name, char * const *l) {
        _cleanup_free_ char *nulstr = NULL;
        size_t size;
        정수 r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        만약 (r < 0)
                반환 r;

        반환 xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

정수 xremovexattr(정수 fd, const char *path, 정수 at_flags, const char *name) {
        정수 r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        만약 (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                만약 (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        반환 r;

                have_xattrat = false;
        }

        _cleanup_close_ 정수 tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        만약 (r < 0)
                반환 r;

        만약 (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        아니면
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        만약 (r < 0)
                반환 -errno;

        반환 0;
}

static 정수 parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(sizeof(usec_t) == sizeof(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        만약 (!timestamp_is_set(u))
                반환 -EIO;

        *ret = u;
        반환 0;
}

정수 getcrtime_at(
                정수 fd,
                const char *path,
                정수 at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        struct statx sx;
        usec_t a, b;
        정수 r;

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
        만약 (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        아니면
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (char**) &le, &le_size);
        만약 (r >= 0) {
                만약 (le_size != sizeof(*le))
                        r = -EIO;
                아니면
                        r = parse_crtime(*le, &b);
        }
        만약 (r < 0) {
                만약 (a != USEC_INFINITY) {
                        만약 (ret)
                                *ret = a;
                        반환 0;
                }

                반환 r;
        }

        만약 (ret)
                *ret = MIN(a, b);
        반환 0;
}

정수 fd_setcrtime(정수 fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        만약 (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        반환 xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const char*) &le, sizeof(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const char *name) {
        반환 STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const char *name) {
        반환 streq(ASSERT_PTR(name), "security.selinux");
}
