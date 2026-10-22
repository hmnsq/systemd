/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <sys/xattr.h>
#ضمن <threads.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "nulstr-util.h"
#ضمن "parse-util.h"
#ضمن "sparse-endian.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "time-util.h"
#ضمن "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
ساكن thread_local bool have_xattrat = true;

ساكن صحيح normalize_and_maybe_pin_inode(
                صحيح *fd,
                ثابت حرف **path,
                صحيح *at_flags,
                صحيح *ret_tfd,
                bool *ret_opath) {

        صحيح r;

        أكد(fd);
        أكد(*fd >= 0 || *fd == AT_FDCWD);
        أكد(path);
        أكد(at_flags);
        أكد(ret_tfd);
        أكد(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        إذا (*fd == AT_FDCWD) {
                إذا (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                إرجاع 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        إذا (!*path) {
                r = fd_is_opath(*fd);
                إذا (r < 0)
                        إرجاع r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                إرجاع 0;
        }

        /* If both have been specified, then we go via O_PATH */

        صحيح tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        إذا (tfd < 0)
                إرجاع -errno;

        *fd = *ret_tfd = tfd;
        *path = عدم;
        *ret_opath = true;

        إرجاع 0;
}

ساكن ssize_t getxattr_pinned_internal(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                bool by_procfs,
                ثابت حرف *name,
                حرف *buf,
                size_t size) {

        ssize_t n;

        أكد(!path || !isempty(path));
        أكد((fd >= 0) == !path);
        أكد(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        أكد(name);
        أكد(buf || size == 0);

        إذا (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        وإلا
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        إذا (n < 0)
                إرجاع -errno;

        أكد(size == 0 || (size_t) n <= size);
        إرجاع n;
}

صحيح getxattr_at_malloc(
                صحيح fd,
                ثابت حرف *path,
                ثابت حرف *name,
                صحيح at_flags,
                حرف **ret,
                size_t *ret_size) {

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد(name);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                إرجاع r;

        size_t l = 100;
        لكل (unsigned n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                إذا (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        إرجاع -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                إذا (!v)
                        إرجاع -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                إذا (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        إذا (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                إرجاع -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        إذا (ret_size)
                                *ret_size = (size_t) n;

                        إرجاع 0;
                }
                إذا (n != -ERANGE)
                        إرجاع (صحيح) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, عدم, 0);
                إذا (n < 0)
                        إرجاع (صحيح) n;

                l = (size_t) n;
        }
}

صحيح getxattr_at_bool(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ عدم);
        إذا (r < 0)
                إرجاع r;

        إرجاع parse_boolean(v);
}

صحيح getxattr_at_strv(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags, حرف ***ret_strv) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t nulstr_size;
        صحيح r;

        أكد(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        إذا (r < 0)
                إرجاع r;

        _cleanup_strv_free_ حرف **l = strv_parse_nulstr(nulstr, nulstr_size);
        إذا (!l)
                إرجاع -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        إرجاع 0;
}

ساكن صحيح listxattr_pinned_internal(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                bool by_procfs,
                حرف *buf,
                size_t size) {

        ssize_t n;

        أكد(!path || !isempty(path));
        أكد((fd >= 0) == !path);
        أكد(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        أكد(buf || size == 0);

        إذا (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        وإلا
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        إذا (n < 0)
                إرجاع -errno;

        أكد(size == 0 || (size_t) n <= size);

        إذا (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                إرجاع -E2BIG;

        إرجاع (صحيح) n;
}

صحيح listxattr_at_malloc(صحيح fd, ثابت حرف *path, صحيح at_flags, حرف **ret) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                إرجاع r;

        size_t l = 100;
        لكل (unsigned n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                إذا (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        إرجاع -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                إذا (!v)
                        إرجاع -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                إذا (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        إرجاع r;
                }
                إذا (r != -ERANGE)
                        إرجاع r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, عدم, 0);
                إذا (r < 0)
                        إرجاع r;

                l = (size_t) r;
        }
}

صحيح xsetxattr_full(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *قيمة,
                size_t size,
                صحيح xattr_flags) {

        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(name);
        أكد(قيمة || size == 0);

        إذا (size == SIZE_MAX)
                size = strlen(قيمة);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ حرف *old_قيمة = عدم;
        size_t old_size;

        إذا (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_قيمة, &old_size) >= 0 &&
            memcmp_nn(old_قيمة, old_size, قيمة, size) == 0)
                إرجاع 0;

        إذا (have_xattrat && !isempty(path)) {
                بنية xattr_args args = {
                        .قيمة = PTR_TO_UINT64(قيمة),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, حجم(args)));
                إذا (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        إرجاع r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                إرجاع r;

        إذا (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, قيمة, size, xattr_flags)
                                                           : lsetxattr(path, name, قيمة, size, xattr_flags);
        وإلا
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, قيمة, size, xattr_flags)
                              : fsetxattr(fd, name, قيمة, size, xattr_flags);
        إذا (r < 0)
                إرجاع -errno;

        إرجاع 0;
}

صحيح xsetxattr_strv(صحيح fd, ثابت حرف *path, صحيح at_flags, ثابت حرف *name, حرف * ثابت *l) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t size;
        صحيح r;

        أكد(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        إذا (r < 0)
                إرجاع r;

        إرجاع xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

صحيح xremovexattr(صحيح fd, ثابت حرف *path, صحيح at_flags, ثابت حرف *name) {
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(name);

        إذا (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                إذا (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        إرجاع r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        إذا (r < 0)
                إرجاع r;

        إذا (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        وإلا
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        إذا (r < 0)
                إرجاع -errno;

        إرجاع 0;
}

ساكن صحيح parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        أكد(ret);

        assert_cc(حجم(usec_t) == حجم(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        إذا (!timestamp_is_set(u))
                إرجاع -EIO;

        *ret = u;
        إرجاع 0;
}

صحيح getcrtime_at(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = عدم;
        بنية statx sx;
        usec_t a, b;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        إذا (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        وإلا
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (حرف**) &le, &le_size);
        إذا (r >= 0) {
                إذا (le_size != حجم(*le))
                        r = -EIO;
                وإلا
                        r = parse_crtime(*le, &b);
        }
        إذا (r < 0) {
                إذا (a != USEC_INFINITY) {
                        إذا (ret)
                                *ret = a;
                        إرجاع 0;
                }

                إرجاع r;
        }

        إذا (ret)
                *ret = MIN(a, b);
        إرجاع 0;
}

صحيح fd_setcrtime(صحيح fd, usec_t usec) {
        le64_t le;

        أكد(fd >= 0);

        إذا (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        إرجاع xsetxattr_full(fd, /* path= */ عدم, AT_EMPTY_PATH,
                              "user.crtime_usec", (ثابت حرف*) &le, حجم(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ثابت حرف *name) {
        إرجاع STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ثابت حرف *name) {
        إرجاع streq(ASSERT_PTR(name), "security.selinux");
}
