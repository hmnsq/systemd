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

static ཧྲིལ normalize_and_maybe_pin_inode(
                ཧྲིལ *fd,
                const var_na **path,
                ཧྲིལ *at_flags,
                ཧྲིལ *ret_tfd,
                bool *ret_opath) {

        ཧྲིལ r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        གལ་སྲིད (*fd == AT_FDCWD) {
                གལ་སྲིད (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ལོག 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        གལ་སྲིད (!*path) {
                r = fd_is_opath(*fd);
                གལ་སྲིད (r < 0)
                        ལོག r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ལོག 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ཧྲིལ tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        གལ་སྲིད (tfd < 0)
                ལོག -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        ལོག 0;
}

static ssize_t getxattr_pinned_internal(
                ཧྲིལ fd,
                const var_na *path,
                ཧྲིལ at_flags,
                bool by_procfs,
                const var_na *name,
                var_na *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(name);
        assert(buf || size == 0);

        གལ་སྲིད (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        དེ་མིན
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        གལ་སྲིད (n < 0)
                ལོག -errno;

        assert(size == 0 || (size_t) n <= size);
        ལོག n;
}

ཧྲིལ getxattr_at_malloc(
                ཧྲིལ fd,
                const var_na *path,
                const var_na *name,
                ཧྲིལ at_flags,
                var_na **ret,
                size_t *ret_size) {

        _cleanup_close_ ཧྲིལ opened_fd = -EBADF;
        bool by_procfs;
        ཧྲིལ r;

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
        གལ་སྲིད (r < 0)
                ལོག r;

        size_t l = 100;
        དོན་ལུ (unsigned n_attempts = 7;;) {
                _cleanup_free_ var_na *v = NULL;

                གལ་སྲིད (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ལོག -EBUSY;
                n_attempts--;

                v = new(var_na, l+1);
                གལ་སྲིད (!v)
                        ལོག -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                གལ་སྲིད (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        གལ་སྲིད (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                ལོག -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        གལ་སྲིད (ret_size)
                                *ret_size = (size_t) n;

                        ལོག 0;
                }
                གལ་སྲིད (n != -ERANGE)
                        ལོག (ཧྲིལ) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                གལ་སྲིད (n < 0)
                        ལོག (ཧྲིལ) n;

                l = (size_t) n;
        }
}

ཧྲིལ getxattr_at_bool(ཧྲིལ fd, const var_na *path, const var_na *name, ཧྲིལ at_flags) {
        _cleanup_free_ var_na *v = NULL;
        ཧྲིལ r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག parse_boolean(v);
}

ཧྲིལ getxattr_at_strv(ཧྲིལ fd, const var_na *path, const var_na *name, ཧྲིལ at_flags, var_na ***ret_strv) {
        _cleanup_free_ var_na *nulstr = NULL;
        size_t nulstr_size;
        ཧྲིལ r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        གལ་སྲིད (r < 0)
                ལོག r;

        _cleanup_strv_free_ var_na **l = strv_parse_nulstr(nulstr, nulstr_size);
        གལ་སྲིད (!l)
                ལོག -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ལོག 0;
}

static ཧྲིལ listxattr_pinned_internal(
                ཧྲིལ fd,
                const var_na *path,
                ཧྲིལ at_flags,
                bool by_procfs,
                var_na *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        གལ་སྲིད (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        དེ་མིན
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        གལ་སྲིད (n < 0)
                ལོག -errno;

        assert(size == 0 || (size_t) n <= size);

        གལ་སྲིད (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                ལོག -E2BIG;

        ལོག (ཧྲིལ) n;
}

ཧྲིལ listxattr_at_malloc(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, var_na **ret) {
        _cleanup_close_ ཧྲིལ opened_fd = -EBADF;
        bool by_procfs;
        ཧྲིལ r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        གལ་སྲིད (r < 0)
                ལོག r;

        size_t l = 100;
        དོན་ལུ (unsigned n_attempts = 7;;) {
                _cleanup_free_ var_na *v = NULL;

                གལ་སྲིད (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ལོག -EBUSY;
                n_attempts--;

                v = new(var_na, l+1);
                གལ་སྲིད (!v)
                        ལོག -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                གལ་སྲིད (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ལོག r;
                }
                གལ་སྲིད (r != -ERANGE)
                        ལོག r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                གལ་སྲིད (r < 0)
                        ལོག r;

                l = (size_t) r;
        }
}

ཧྲིལ xsetxattr_full(
                ཧྲིལ fd,
                const var_na *path,
                ཧྲིལ at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                ཧྲིལ xattr_flags) {

        ཧྲིལ r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        གལ་སྲིད (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ var_na *old_value = NULL;
        size_t old_size;

        གལ་སྲིད (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                ལོག 0;

        གལ་སྲིད (have_xattrat && !isempty(path)) {
                sa_nracanaa xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, parimaa_na(args)));
                གལ་སྲིད (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ལོག r;

                have_xattrat = false;
        }

        _cleanup_close_ ཧྲིལ opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        དེ་མིན
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        གལ་སྲིད (r < 0)
                ལོག -errno;

        ལོག 0;
}

ཧྲིལ xsetxattr_strv(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, const var_na *name, var_na * const *l) {
        _cleanup_free_ var_na *nulstr = NULL;
        size_t size;
        ཧྲིལ r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        གལ་སྲིད (r < 0)
                ལོག r;

        ལོག xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

ཧྲིལ xremovexattr(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, const var_na *name) {
        ཧྲིལ r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        གལ་སྲིད (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                གལ་སྲིད (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ལོག r;

                have_xattrat = false;
        }

        _cleanup_close_ ཧྲིལ tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        དེ་མིན
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        གལ་སྲིད (r < 0)
                ལོག -errno;

        ལོག 0;
}

static ཧྲིལ parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(parimaa_na(usec_t) == parimaa_na(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        གལ་སྲིད (!timestamp_is_set(u))
                ལོག -EIO;

        *ret = u;
        ལོག 0;
}

ཧྲིལ getcrtime_at(
                ཧྲིལ fd,
                const var_na *path,
                ཧྲིལ at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        sa_nracanaa statx sx;
        usec_t a, b;
        ཧྲིལ r;

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
        གལ་སྲིད (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        དེ་མིན
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (var_na**) &le, &le_size);
        གལ་སྲིད (r >= 0) {
                གལ་སྲིད (le_size != parimaa_na(*le))
                        r = -EIO;
                དེ་མིན
                        r = parse_crtime(*le, &b);
        }
        གལ་སྲིད (r < 0) {
                གལ་སྲིད (a != USEC_INFINITY) {
                        གལ་སྲིད (ret)
                                *ret = a;
                        ལོག 0;
                }

                ལོག r;
        }

        གལ་སྲིད (ret)
                *ret = MIN(a, b);
        ལོག 0;
}

ཧྲིལ fd_setcrtime(ཧྲིལ fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        གལ་སྲིད (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ལོག xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const var_na*) &le, parimaa_na(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const var_na *name) {
        ལོག STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const var_na *name) {
        ལོག streq(ASSERT_PTR(name), "security.selinux");
}
