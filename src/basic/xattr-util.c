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

static puur_naa_nka normalize_and_maybe_pin_inode(
                puur_naa_nka *fd,
                const var_na **path,
                puur_naa_nka *at_flags,
                puur_naa_nka *ret_tfd,
                bool *ret_opath) {

        puur_naa_nka r;

        assert(fd);
        assert(*fd >= 0 || *fd == AT_FDCWD);
        assert(path);
        assert(at_flags);
        assert(ret_tfd);
        assert(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        ᱡᱩᱫᱤ (*fd == AT_FDCWD) {
                ᱡᱩᱫᱤ (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        ᱡᱩᱫᱤ (!*path) {
                r = fd_is_opath(*fd);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        puur_naa_nka tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ᱡᱩᱫᱤ (tfd < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        *fd = *ret_tfd = tfd;
        *path = NULL;
        *ret_opath = true;

        ᱨᱩᱣᱟᱹᱲ 0;
}

static ssize_t getxattr_pinned_internal(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka at_flags,
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

        ᱡᱩᱫᱤ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        ᱵᱟᱝ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        ᱡᱩᱫᱤ (n < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        assert(size == 0 || (size_t) n <= size);
        ᱨᱩᱣᱟᱹᱲ n;
}

puur_naa_nka getxattr_at_malloc(
                puur_naa_nka fd,
                const var_na *path,
                const var_na *name,
                puur_naa_nka at_flags,
                var_na **ret,
                size_t *ret_size) {

        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        bool by_procfs;
        puur_naa_nka r;

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
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        size_t l = 100;
        for (unsigned n_attempts = 7;;) {
                _cleanup_free_ var_na *v = NULL;

                ᱡᱩᱫᱤ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ᱨᱩᱣᱟᱹᱲ -EBUSY;
                n_attempts--;

                v = new(var_na, l+1);
                ᱡᱩᱫᱤ (!v)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                ᱡᱩᱫᱤ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ᱡᱩᱫᱤ (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                ᱨᱩᱣᱟᱹᱲ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ᱡᱩᱫᱤ (ret_size)
                                *ret_size = (size_t) n;

                        ᱨᱩᱣᱟᱹᱲ 0;
                }
                ᱡᱩᱫᱤ (n != -ERANGE)
                        ᱨᱩᱣᱟᱹᱲ (puur_naa_nka) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, NULL, 0);
                ᱡᱩᱫᱤ (n < 0)
                        ᱨᱩᱣᱟᱹᱲ (puur_naa_nka) n;

                l = (size_t) n;
        }
}

puur_naa_nka getxattr_at_bool(puur_naa_nka fd, const var_na *path, const var_na *name, puur_naa_nka at_flags) {
        _cleanup_free_ var_na *v = NULL;
        puur_naa_nka r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ NULL);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ parse_boolean(v);
}

puur_naa_nka getxattr_at_strv(puur_naa_nka fd, const var_na *path, const var_na *name, puur_naa_nka at_flags, var_na ***ret_strv) {
        _cleanup_free_ var_na *nulstr = NULL;
        size_t nulstr_size;
        puur_naa_nka r;

        assert(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        _cleanup_strv_free_ var_na **l = strv_parse_nulstr(nulstr, nulstr_size);
        ᱡᱩᱫᱤ (!l)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ᱨᱩᱣᱟᱹᱲ 0;
}

static puur_naa_nka listxattr_pinned_internal(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka at_flags,
                bool by_procfs,
                var_na *buf,
                size_t size) {

        ssize_t n;

        assert(!path || !isempty(path));
        assert((fd >= 0) == !path);
        assert(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        assert(buf || size == 0);

        ᱡᱩᱫᱤ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        ᱵᱟᱝ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        ᱡᱩᱫᱤ (n < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        assert(size == 0 || (size_t) n <= size);

        ᱡᱩᱫᱤ (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                ᱨᱩᱣᱟᱹᱲ -E2BIG;

        ᱨᱩᱣᱟᱹᱲ (puur_naa_nka) n;
}

puur_naa_nka listxattr_at_malloc(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, var_na **ret) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        bool by_procfs;
        puur_naa_nka r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        size_t l = 100;
        for (unsigned n_attempts = 7;;) {
                _cleanup_free_ var_na *v = NULL;

                ᱡᱩᱫᱤ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ᱨᱩᱣᱟᱹᱲ -EBUSY;
                n_attempts--;

                v = new(var_na, l+1);
                ᱡᱩᱫᱤ (!v)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                ᱡᱩᱫᱤ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ᱨᱩᱣᱟᱹᱲ r;
                }
                ᱡᱩᱫᱤ (r != -ERANGE)
                        ᱨᱩᱣᱟᱹᱲ r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, NULL, 0);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                l = (size_t) r;
        }
}

puur_naa_nka xsetxattr_full(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                puur_naa_nka xattr_flags) {

        puur_naa_nka r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);
        assert(value || size == 0);

        ᱡᱩᱫᱤ (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ var_na *old_value = NULL;
        size_t old_size;

        ᱡᱩᱫᱤ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                ᱨᱩᱣᱟᱹᱲ 0;

        ᱡᱩᱫᱤ (have_xattrat && !isempty(path)) {
                sa_nracanaa xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, parimaa_na(args)));
                ᱡᱩᱫᱤ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ᱨᱩᱣᱟᱹᱲ r;

                have_xattrat = false;
        }

        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        ᱵᱟᱝ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka xsetxattr_strv(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, const var_na *name, var_na * const *l) {
        _cleanup_free_ var_na *nulstr = NULL;
        size_t size;
        puur_naa_nka r;

        assert(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

puur_naa_nka xremovexattr(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, const var_na *name) {
        puur_naa_nka r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        assert(name);

        ᱡᱩᱫᱤ (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ᱡᱩᱫᱤ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ᱨᱩᱣᱟᱹᱲ r;

                have_xattrat = false;
        }

        _cleanup_close_ puur_naa_nka tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        ᱵᱟᱝ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱨᱩᱣᱟᱹᱲ 0;
}

static puur_naa_nka parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        assert(ret);

        assert_cc(parimaa_na(usec_t) == parimaa_na(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ᱡᱩᱫᱤ (!timestamp_is_set(u))
                ᱨᱩᱣᱟᱹᱲ -EIO;

        *ret = u;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka getcrtime_at(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = NULL;
        sa_nracanaa statx sx;
        usec_t a, b;
        puur_naa_nka r;

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
        ᱡᱩᱫᱤ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        ᱵᱟᱝ
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (var_na**) &le, &le_size);
        ᱡᱩᱫᱤ (r >= 0) {
                ᱡᱩᱫᱤ (le_size != parimaa_na(*le))
                        r = -EIO;
                ᱵᱟᱝ
                        r = parse_crtime(*le, &b);
        }
        ᱡᱩᱫᱤ (r < 0) {
                ᱡᱩᱫᱤ (a != USEC_INFINITY) {
                        ᱡᱩᱫᱤ (ret)
                                *ret = a;
                        ᱨᱩᱣᱟᱹᱲ 0;
                }

                ᱨᱩᱣᱟᱹᱲ r;
        }

        ᱡᱩᱫᱤ (ret)
                *ret = MIN(a, b);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka fd_setcrtime(puur_naa_nka fd, usec_t usec) {
        le64_t le;

        assert(fd >= 0);

        ᱡᱩᱫᱤ (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ᱨᱩᱣᱟᱹᱲ xsetxattr_full(fd, /* path= */ NULL, AT_EMPTY_PATH,
                              "user.crtime_usec", (const var_na*) &le, parimaa_na(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(const var_na *name) {
        ᱨᱩᱣᱟᱹᱲ STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(const var_na *name) {
        ᱨᱩᱣᱟᱹᱲ streq(ASSERT_PTR(name), "security.selinux");
}
