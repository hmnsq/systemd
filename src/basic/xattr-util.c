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

static ຈຳນວນເຕັມ normalize_and_maybe_pin_inode(
                ຈຳນວນເຕັມ *fd,
                const var_na **path,
                ຈຳນວນເຕັມ *at_flags,
                ຈຳນວນເຕັມ *ret_tfd,
                bool *ret_opath) {

        ຈຳນວນເຕັມ r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        ຖ້າ (*fd == AT_FDCWD) {
                ຖ້າ (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ສົ່ງຄືນ 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        ຖ້າ (!*path) {
                r = fd_is_opath(*fd);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ສົ່ງຄືນ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ຈຳນວນເຕັມ tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ຖ້າ (tfd < 0)
                ສົ່ງຄືນ -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        ສົ່ງຄືນ 0;
}

static ssize_t getxattr_pinned_internal(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                ຈຳນວນເຕັມ at_flags,
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

        ຖ້າ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        ບໍ່ດັ່ງນັ້ນ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        ຖ້າ (n < 0)
                ສົ່ງຄືນ -errno;

        assert(size == 0 || (size_t) n <= size);
        ສົ່ງຄືນ n;
}

ຈຳນວນເຕັມ getxattr_at_malloc(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                const var_na *name,
                ຈຳນວນເຕັມ at_flags,
                var_na **ret,
                size_t *ret_size) {

        _cleanup_close_ ຈຳນວນເຕັມ opened_fd = -EBADF;
        bool by_procfs;
        ຈຳນວນເຕັມ r;

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
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        size_t l = 100;
        ສຳລັບ (unsigned n_attempts = 7;;) {
                _cleanup_free_ var_na *v = NULL;

                ຖ້າ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ສົ່ງຄືນ -EBUSY;
                n_attempts--;

                v = new(var_na, l+1);
                ຖ້າ (!v)
                        ສົ່ງຄືນ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                ຖ້າ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ຖ້າ (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                ສົ່ງຄືນ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ຖ້າ (ret_size)
                                *ret_size = (size_t) n;

                        ສົ່ງຄືນ 0;
                }
                ຖ້າ (n != -ERANGE)
                        ສົ່ງຄືນ (ຈຳນວນເຕັມ) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                ຖ້າ (n < 0)
                        ສົ່ງຄືນ (ຈຳນວນເຕັມ) n;

                l = (size_t) n;
        }
}

ຈຳນວນເຕັມ getxattr_at_bool(ຈຳນວນເຕັມ fd, const var_na *path, const var_na *name, ຈຳນວນເຕັມ at_flags) {
        _cleanup_free_ var_na *v = NULL;
        ຈຳນວນເຕັມ r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ parse_boolean(v);
}

ຈຳນວນເຕັມ getxattr_at_strv(ຈຳນວນເຕັມ fd, const var_na *path, const var_na *name, ຈຳນວນເຕັມ at_flags, var_na ***ret_strv) {
        _cleanup_free_ var_na *nulstr = NULL;
        size_t nulstr_size;
        ຈຳນວນເຕັມ r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        _cleanup_strv_free_ var_na **l = strv_parse_nulstr(nulstr, nulstr_size);
        ຖ້າ (!l)
                ສົ່ງຄືນ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ listxattr_pinned_internal(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                ຈຳນວນເຕັມ at_flags,
                bool by_procfs,
                var_na *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        ຖ້າ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        ບໍ່ດັ່ງນັ້ນ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        ຖ້າ (n < 0)
                ສົ່ງຄືນ -errno;

        assert(size == 0 || (size_t) n <= size);

        ຖ້າ (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                ສົ່ງຄືນ -E2BIG;

        ສົ່ງຄືນ (ຈຳນວນເຕັມ) n;
}

ຈຳນວນເຕັມ listxattr_at_malloc(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, var_na **ret) {
        _cleanup_close_ ຈຳນວນເຕັມ opened_fd = -EBADF;
        bool by_procfs;
        ຈຳນວນເຕັມ r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        size_t l = 100;
        ສຳລັບ (unsigned n_attempts = 7;;) {
                _cleanup_free_ var_na *v = NULL;

                ຖ້າ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ສົ່ງຄືນ -EBUSY;
                n_attempts--;

                v = new(var_na, l+1);
                ຖ້າ (!v)
                        ສົ່ງຄືນ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                ຖ້າ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ສົ່ງຄືນ r;
                }
                ຖ້າ (r != -ERANGE)
                        ສົ່ງຄືນ r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                l = (size_t) r;
        }
}

ຈຳນວນເຕັມ xsetxattr_full(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                ຈຳນວນເຕັມ at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                ຈຳນວນເຕັມ xattr_flags) {

        ຈຳນວນເຕັມ r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        ຖ້າ (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ var_na *old_value = NULL;
        size_t old_size;

        ຖ້າ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                ສົ່ງຄືນ 0;

        ຖ້າ (have_xattrat && !isempty(path)) {
                sa_nracanaa xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, parimaa_na(args)));
                ຖ້າ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ສົ່ງຄືນ r;

                have_xattrat = false;
        }

        _cleanup_close_ ຈຳນວນເຕັມ opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        ບໍ່ດັ່ງນັ້ນ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ xsetxattr_strv(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, const var_na *name, var_na * const *l) {
        _cleanup_free_ var_na *nulstr = NULL;
        size_t size;
        ຈຳນວນເຕັມ r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

ຈຳນວນເຕັມ xremovexattr(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, const var_na *name) {
        ຈຳນວນເຕັມ r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        ຖ້າ (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ຖ້າ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ສົ່ງຄືນ r;

                have_xattrat = false;
        }

        _cleanup_close_ ຈຳນວນເຕັມ tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        ບໍ່ດັ່ງນັ້ນ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ 0;
}

static ຈຳນວນເຕັມ parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(parimaa_na(usec_t) == parimaa_na(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ຖ້າ (!timestamp_is_set(u))
                ສົ່ງຄືນ -EIO;

        *ret = u;
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ getcrtime_at(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                ຈຳນວນເຕັມ at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        sa_nracanaa statx sx;
        usec_t a, b;
        ຈຳນວນເຕັມ r;

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
        ຖ້າ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        ບໍ່ດັ່ງນັ້ນ
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (var_na**) &le, &le_size);
        ຖ້າ (r >= 0) {
                ຖ້າ (le_size != parimaa_na(*le))
                        r = -EIO;
                ບໍ່ດັ່ງນັ້ນ
                        r = parse_crtime(*le, &b);
        }
        ຖ້າ (r < 0) {
                ຖ້າ (a != USEC_INFINITY) {
                        ຖ້າ (ret)
                                *ret = a;
                        ສົ່ງຄືນ 0;
                }

                ສົ່ງຄືນ r;
        }

        ຖ້າ (ret)
                *ret = MIN(a, b);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fd_setcrtime(ຈຳນວນເຕັມ fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        ຖ້າ (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ສົ່ງຄືນ xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const var_na*) &le, parimaa_na(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const var_na *name) {
        ສົ່ງຄືນ STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const var_na *name) {
        ສົ່ງຄືນ streq(ASSERT_PTR(name), "security.selinux");
}
