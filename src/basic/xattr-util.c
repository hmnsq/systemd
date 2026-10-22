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
                ثابت حرف **مسیر,
                صحيح *at_flags,
                صحيح *ret_tfd,
                bool *ret_opath) {

        صحيح r;

        أكد(fd);
        أكد(*fd >= 0 || *fd == AT_FDCWD);
        أكد(مسیر);
        أكد(at_flags);
        أكد(ret_tfd);
        أكد(ret_opath);

        *مسیر = خالی_مقصد_null(*مسیر); /* Normalize "" to NULL */

        إذا (*fd == AT_FDCWD) {
                إذا (!*مسیر) /* Both unspecified? Then operate on current working directory */
                        *مسیر = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                أرجع 0;
        }

        *at_flags |= AT_خالی_مسیر;

        إذا (!*مسیر) {
                r = fd_is_opath(*fd);
                إذا (r < 0)
                        أرجع r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                أرجع 0;
        }

        /* If both have been specified, then we go via O_PATH */

        صحيح tfd = openat(*fd, *مسیر, O_مسیر|O_CLOEXEC|(FLAGS_مجموعه(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        إذا (tfd < 0)
                أرجع -errno;

        *fd = *ret_tfd = tfd;
        *مسیر = عدم;
        *ret_opath = true;

        أرجع 0;
}

ساكن ssize_t getxattr_pinned_داخلی(
                صحيح fd,
                ثابت حرف *مسیر,
                صحيح at_flags,
                bool by_procfs,
                ثابت حرف *name,
                حرف *buf,
                size_t اندازه) {

        ssize_t n;

        أكد(!مسیر || !isempty(مسیر));
        أكد((fd >= 0) == !مسیر);
        أكد(مسیر || FLAGS_مجموعه(at_flags, AT_خالی_مسیر));
        أكد(name);
        أكد(buf || اندازه == 0);

        إذا (مسیر)
                n = FLAGS_مجموعه(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(مسیر, name, buf, اندازه)
                                                           : lgetxattr(مسیر, name, buf, اندازه);
        وإلا
                n = by_procfs ? getxattr(FORMAT_PROC_FD_مسیر(fd), name, buf, اندازه)
                              : fgetxattr(fd, name, buf, اندازه);
        إذا (n < 0)
                أرجع -errno;

        أكد(اندازه == 0 || (size_t) n <= اندازه);
        أرجع n;
}

صحيح getxattr_at_malloc(
                صحيح fd,
                ثابت حرف *مسیر,
                ثابت حرف *name,
                صحيح at_flags,
                حرف **ret,
                size_t *ret_اندازه) {

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد(name);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_مسیر)) == 0);
        أكد(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &مسیر, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        size_t l = 100;
        لكل (بیعلامت n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                إذا (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        أرجع -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                إذا (!v)
                        أرجع -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_داخلی(fd, مسیر, at_flags, by_procfs, name, v, l);
                إذا (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        إذا (!ret_اندازه && n > 1 && memchr(v, 0, n - 1))
                                أرجع -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        إذا (ret_اندازه)
                                *ret_اندازه = (size_t) n;

                        أرجع 0;
                }
                إذا (n != -ERANGE)
                        أرجع (صحيح) n;

                n = getxattr_pinned_داخلی(fd, مسیر, at_flags, by_procfs, name, عدم, 0);
                إذا (n < 0)
                        أرجع (صحيح) n;

                l = (size_t) n;
        }
}

صحيح getxattr_at_bool(صحيح fd, ثابت حرف *مسیر, ثابت حرف *name, صحيح at_flags) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        r = getxattr_at_malloc(fd, مسیر, name, at_flags, &v, /* ret_size= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع parse_boolean(v);
}

صحيح getxattr_at_strv(صحيح fd, ثابت حرف *مسیر, ثابت حرف *name, صحيح at_flags, حرف ***ret_strv) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t nulstr_اندازه;
        صحيح r;

        أكد(ret_strv);

        r = getxattr_at_malloc(fd, مسیر, name, at_flags, &nulstr, &nulstr_اندازه);
        إذا (r < 0)
                أرجع r;

        _cleanup_strv_free_ حرف **l = strv_parse_nulstr(nulstr, nulstr_اندازه);
        إذا (!l)
                أرجع -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        أرجع 0;
}

ساكن صحيح listxattr_pinned_داخلی(
                صحيح fd,
                ثابت حرف *مسیر,
                صحيح at_flags,
                bool by_procfs,
                حرف *buf,
                size_t اندازه) {

        ssize_t n;

        أكد(!مسیر || !isempty(مسیر));
        أكد((fd >= 0) == !مسیر);
        أكد(مسیر || FLAGS_مجموعه(at_flags, AT_خالی_مسیر));
        أكد(buf || اندازه == 0);

        إذا (مسیر)
                n = FLAGS_مجموعه(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(مسیر, buf, اندازه)
                                                           : llistxattr(مسیر, buf, اندازه);
        وإلا
                n = by_procfs ? listxattr(FORMAT_PROC_FD_مسیر(fd), buf, اندازه)
                              : flistxattr(fd, buf, اندازه);
        إذا (n < 0)
                أرجع -errno;

        أكد(اندازه == 0 || (size_t) n <= اندازه);

        إذا (n > بیشترین_صحیح) /* We couldn't return this as 'int' anymore */
                أرجع -E2BIG;

        أرجع (صحيح) n;
}

صحيح listxattr_at_malloc(صحيح fd, ثابت حرف *مسیر, صحيح at_flags, حرف **ret) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_مسیر)) == 0);
        أكد(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &مسیر, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        size_t l = 100;
        لكل (بیعلامت n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                إذا (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        أرجع -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                إذا (!v)
                        أرجع -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_داخلی(fd, مسیر, at_flags, by_procfs, v, l);
                إذا (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        أرجع r;
                }
                إذا (r != -ERANGE)
                        أرجع r;

                r = listxattr_pinned_داخلی(fd, مسیر, at_flags, by_procfs, عدم, 0);
                إذا (r < 0)
                        أرجع r;

                l = (size_t) r;
        }
}

صحيح xsetxattr_پر(
                صحيح fd,
                ثابت حرف *مسیر,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *مقدار,
                size_t اندازه,
                صحيح xattr_flags) {

        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_مسیر)) == 0);
        أكد(name);
        أكد(مقدار || اندازه == 0);

        إذا (اندازه == SIZE_MAX)
                اندازه = طول_رشته(مقدار);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ حرف *old_مقدار = عدم;
        size_t old_اندازه;

        إذا (xattr_flags == 0 &&
            getxattr_at_malloc(fd, مسیر, name, at_flags, &old_مقدار, &old_اندازه) >= 0 &&
            memcmp_nn(old_مقدار, old_اندازه, مقدار, اندازه) == 0)
                أرجع 0;

        إذا (have_xattrat && !isempty(مسیر)) {
                بنية xattr_آرگومانها آرگومانها = {
                        .مقدار = PTR_مقصد_UINT64(مقدار),
                        .اندازه = اندازه,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, مسیر,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &آرگومانها, حجم(آرگومانها)));
                إذا (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        أرجع r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &مسیر, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        إذا (مسیر)
                r = FLAGS_مجموعه(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(مسیر, name, مقدار, اندازه, xattr_flags)
                                                           : lsetxattr(مسیر, name, مقدار, اندازه, xattr_flags);
        وإلا
                r = by_procfs ? setxattr(FORMAT_PROC_FD_مسیر(fd), name, مقدار, اندازه, xattr_flags)
                              : fsetxattr(fd, name, مقدار, اندازه, xattr_flags);
        إذا (r < 0)
                أرجع -errno;

        أرجع 0;
}

صحيح xsetxattr_strv(صحيح fd, ثابت حرف *مسیر, صحيح at_flags, ثابت حرف *name, حرف * ثابت *l) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t اندازه;
        صحيح r;

        أكد(name);

        r = strv_make_nulstr(l, &nulstr, &اندازه);
        إذا (r < 0)
                أرجع r;

        أرجع xsetxattr_پر(fd, مسیر, at_flags, name, nulstr, اندازه, /* xattr_flags= */ 0);
}

صحيح xremovexattr(صحيح fd, ثابت حرف *مسیر, صحيح at_flags, ثابت حرف *name) {
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_مسیر)) == 0);
        أكد(name);

        إذا (have_xattrat && !isempty(مسیر)) {
                r = RET_NERRNO(removexattrat(fd, مسیر,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                إذا (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        أرجع r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &مسیر, &at_flags, &tfd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        إذا (مسیر)
                r = FLAGS_مجموعه(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(مسیر, name)
                                                           : lremovexattr(مسیر, name);
        وإلا
                r = by_procfs ? removexattr(FORMAT_PROC_FD_مسیر(fd), name)
                              : fremovexattr(fd, name);
        إذا (r < 0)
                أرجع -errno;

        أرجع 0;
}

ساكن صحيح parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        أكد(ret);

        assert_cc(حجم(usec_t) == حجم(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        إذا (!timestamp_is_مجموعه(u))
                أرجع -EIO;

        *ret = u;
        أرجع 0;
}

صحيح getcrtime_at(
                صحيح fd,
                ثابت حرف *مسیر,
                صحيح at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = عدم;
        بنية statx sx;
        usec_t a, b;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_مسیر)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_پر(fd,
                        مسیر,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        إذا (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_بارگیری(&sx.stx_btime);
        وإلا
                a = USEC_INFINITY;

        size_t le_اندازه;
        r = getxattr_at_malloc(fd, مسیر, "user.crtime_usec", at_flags, (حرف**) &le, &le_اندازه);
        إذا (r >= 0) {
                إذا (le_اندازه != حجم(*le))
                        r = -EIO;
                وإلا
                        r = parse_crtime(*le, &b);
        }
        إذا (r < 0) {
                إذا (a != USEC_INFINITY) {
                        إذا (ret)
                                *ret = a;
                        أرجع 0;
                }

                أرجع r;
        }

        إذا (ret)
                *ret = MIN(a, b);
        أرجع 0;
}

صحيح fd_setcrtime(صحيح fd, usec_t usec) {
        le64_t le;

        أكد(fd >= 0);

        إذا (!timestamp_is_مجموعه(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        أرجع xsetxattr_پر(fd, /* path= */ عدم, AT_خالی_مسیر,
                              "user.crtime_usec", (ثابت حرف*) &le, حجم(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ثابت حرف *name) {
        أرجع STR_درون_مجموعه(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ثابت حرف *name) {
        أرجع streq(ASSERT_PTR(name), "security.selinux");
}
