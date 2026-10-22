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

static цео normalize_and_maybe_pin_inode(
                цео *fd,
                const char **path,
                цео *at_flags,
                цео *ret_tfd,
                bool *ret_opath) {

        цео r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        ако (*fd == AT_FDCWD) {
                ако (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                врати 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        ако (!*path) {
                r = fd_is_opath(*fd);
                ако (r < 0)
                        врати r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                врати 0;
        }

        /* If both have been specified, then we go via O_PATH */

        цео tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ако (tfd < 0)
                врати -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        врати 0;
}

static ssize_t getxattr_pinned_internal(
                цео fd,
                const char *path,
                цео at_flags,
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

        ако (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        инако
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        ако (n < 0)
                врати -errno;

        assert(size == 0 || (size_t) n <= size);
        врати n;
}

цео getxattr_at_malloc(
                цео fd,
                const char *path,
                const char *name,
                цео at_flags,
                char **ret,
                size_t *ret_size) {

        _cleanup_close_ цео opened_fd = -EBADF;
        bool by_procfs;
        цео r;

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
        ако (r < 0)
                врати r;

        size_t l = 100;
        за (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                ако (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        врати -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                ако (!v)
                        врати -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                ако (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ако (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                врати -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ако (ret_size)
                                *ret_size = (size_t) n;

                        врати 0;
                }
                ако (n != -ERANGE)
                        врати (цео) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                ако (n < 0)
                        врати (цео) n;

                l = (size_t) n;
        }
}

цео getxattr_at_bool(цео fd, const char *path, const char *name, цео at_flags) {
        _cleanup_free_ char *v = NULL;
        цео r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        ако (r < 0)
                врати r;

        врати parse_boolean(v);
}

цео getxattr_at_strv(цео fd, const char *path, const char *name, цео at_flags, char ***ret_strv) {
        _cleanup_free_ char *nulstr = NULL;
        size_t nulstr_size;
        цео r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        ако (r < 0)
                врати r;

        _cleanup_strv_free_ char **l = strv_parse_nulstr(nulstr, nulstr_size);
        ако (!l)
                врати -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        врати 0;
}

static цео listxattr_pinned_internal(
                цео fd,
                const char *path,
                цео at_flags,
                bool by_procfs,
                char *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        ако (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        инако
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        ако (n < 0)
                врати -errno;

        assert(size == 0 || (size_t) n <= size);

        ако (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                врати -E2BIG;

        врати (цео) n;
}

цео listxattr_at_malloc(цео fd, const char *path, цео at_flags, char **ret) {
        _cleanup_close_ цео opened_fd = -EBADF;
        bool by_procfs;
        цео r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ако (r < 0)
                врати r;

        size_t l = 100;
        за (unsigned n_attempts = 7;;) {
                _cleanup_free_ char *v = NULL;

                ако (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        врати -EBUSY;
                n_attempts--;

                v = new(char, l+1);
                ако (!v)
                        врати -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                ако (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        врати r;
                }
                ако (r != -ERANGE)
                        врати r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                ако (r < 0)
                        врати r;

                l = (size_t) r;
        }
}

цео xsetxattr_full(
                цео fd,
                const char *path,
                цео at_flags,
                const char *name,
                const char *value,
                size_t size,
                цео xattr_flags) {

        цео r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        ако (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ char *old_value = NULL;
        size_t old_size;

        ако (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                врати 0;

        ако (have_xattrat && !isempty(path)) {
                struct xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, sizeof(args)));
                ако (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        врати r;

                have_xattrat = false;
        }

        _cleanup_close_ цео opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ако (r < 0)
                врати r;

        ако (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        инако
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        ако (r < 0)
                врати -errno;

        врати 0;
}

цео xsetxattr_strv(цео fd, const char *path, цео at_flags, const char *name, char * const *l) {
        _cleanup_free_ char *nulstr = NULL;
        size_t size;
        цео r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        ако (r < 0)
                врати r;

        врати xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

цео xremovexattr(цео fd, const char *path, цео at_flags, const char *name) {
        цео r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        ако (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ако (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        врати r;

                have_xattrat = false;
        }

        _cleanup_close_ цео tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        ако (r < 0)
                врати r;

        ако (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        инако
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        ако (r < 0)
                врати -errno;

        врати 0;
}

static цео parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(sizeof(usec_t) == sizeof(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ако (!timestamp_is_set(u))
                врати -EIO;

        *ret = u;
        врати 0;
}

цео getcrtime_at(
                цео fd,
                const char *path,
                цео at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        struct statx sx;
        usec_t a, b;
        цео r;

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
        ако (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        инако
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (char**) &le, &le_size);
        ако (r >= 0) {
                ако (le_size != sizeof(*le))
                        r = -EIO;
                инако
                        r = parse_crtime(*le, &b);
        }
        ако (r < 0) {
                ако (a != USEC_INFINITY) {
                        ако (ret)
                                *ret = a;
                        врати 0;
                }

                врати r;
        }

        ако (ret)
                *ret = MIN(a, b);
        врати 0;
}

цео fd_setcrtime(цео fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        ако (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        врати xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const char*) &le, sizeof(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const char *name) {
        врати STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const char *name) {
        врати streq(ASSERT_PTR(name), "security.selinux");
}
