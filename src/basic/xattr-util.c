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
                ثابت حرف **رستہ,
                صحيح *at_flags,
                صحيح *ret_tfd,
                bool *ret_opath) {

        صحيح r;

        أكد(fd);
        أكد(*fd >= 0 || *fd == AT_FDCWD);
        أكد(رستہ);
        أكد(at_flags);
        أكد(ret_tfd);
        أكد(ret_opath);

        *رستہ = خالی_تھاواں_منزل_null(*رستہ); /* Normalize "" to NULL */

        إذا (*fd == AT_FDCWD) {
                إذا (!*رستہ) /* Both unspecified? Then operate on current working directory */
                        *رستہ = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                أرجع 0;
        }

        *at_flags |= AT_خالی_تھاواں_رستہ;

        إذا (!*رستہ) {
                r = fd_is_opath(*fd);
                إذا (r < 0)
                        أرجع r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                أرجع 0;
        }

        /* If both have been specified, then we go via O_PATH */

        صحيح tfd = openat(*fd, *رستہ, O_رستہ|O_CLOEXEC|(FLAGS_سیٹ(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        إذا (tfd < 0)
                أرجع -errno;

        *fd = *ret_tfd = tfd;
        *رستہ = عدم;
        *ret_opath = true;

        أرجع 0;
}

ساكن ssize_t getxattr_pinned_اندرونی(
                صحيح fd,
                ثابت حرف *رستہ,
                صحيح at_flags,
                bool by_procfs,
                ثابت حرف *name,
                حرف *buf,
                size_t ناپ) {

        ssize_t n;

        أكد(!رستہ || !isempty(رستہ));
        أكد((fd >= 0) == !رستہ);
        أكد(رستہ || FLAGS_سیٹ(at_flags, AT_خالی_تھاواں_رستہ));
        أكد(name);
        أكد(buf || ناپ == 0);

        إذا (رستہ)
                n = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(رستہ, name, buf, ناپ)
                                                           : lgetxattr(رستہ, name, buf, ناپ);
        وإلا
                n = by_procfs ? getxattr(FORMAT_PROC_FD_رستہ(fd), name, buf, ناپ)
                              : fgetxattr(fd, name, buf, ناپ);
        إذا (n < 0)
                أرجع -errno;

        أكد(ناپ == 0 || (size_t) n <= ناپ);
        أرجع n;
}

صحيح getxattr_at_malloc(
                صحيح fd,
                ثابت حرف *رستہ,
                ثابت حرف *name,
                صحيح at_flags,
                حرف **ret,
                size_t *ret_ناپ) {

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد(name);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_تھاواں_رستہ)) == 0);
        أكد(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &رستہ, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        size_t l = 100;
        لكل (بنانشانی n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                إذا (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        أرجع -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                إذا (!v)
                        أرجع -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_اندرونی(fd, رستہ, at_flags, by_procfs, name, v, l);
                إذا (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        إذا (!ret_ناپ && n > 1 && memchr(v, 0, n - 1))
                                أرجع -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        إذا (ret_ناپ)
                                *ret_ناپ = (size_t) n;

                        أرجع 0;
                }
                إذا (n != -ERANGE)
                        أرجع (صحيح) n;

                n = getxattr_pinned_اندرونی(fd, رستہ, at_flags, by_procfs, name, عدم, 0);
                إذا (n < 0)
                        أرجع (صحيح) n;

                l = (size_t) n;
        }
}

صحيح getxattr_at_bool(صحيح fd, ثابت حرف *رستہ, ثابت حرف *name, صحيح at_flags) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        r = getxattr_at_malloc(fd, رستہ, name, at_flags, &v, /* ret_size= */ عدم);
        إذا (r < 0)
                أرجع r;

        أرجع parse_boolean(v);
}

صحيح getxattr_at_strv(صحيح fd, ثابت حرف *رستہ, ثابت حرف *name, صحيح at_flags, حرف ***ret_strv) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t nulstr_ناپ;
        صحيح r;

        أكد(ret_strv);

        r = getxattr_at_malloc(fd, رستہ, name, at_flags, &nulstr, &nulstr_ناپ);
        إذا (r < 0)
                أرجع r;

        _cleanup_strv_free_ حرف **l = strv_parse_nulstr(nulstr, nulstr_ناپ);
        إذا (!l)
                أرجع -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        أرجع 0;
}

ساكن صحيح listxattr_pinned_اندرونی(
                صحيح fd,
                ثابت حرف *رستہ,
                صحيح at_flags,
                bool by_procfs,
                حرف *buf,
                size_t ناپ) {

        ssize_t n;

        أكد(!رستہ || !isempty(رستہ));
        أكد((fd >= 0) == !رستہ);
        أكد(رستہ || FLAGS_سیٹ(at_flags, AT_خالی_تھاواں_رستہ));
        أكد(buf || ناپ == 0);

        إذا (رستہ)
                n = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(رستہ, buf, ناپ)
                                                           : llistxattr(رستہ, buf, ناپ);
        وإلا
                n = by_procfs ? listxattr(FORMAT_PROC_FD_رستہ(fd), buf, ناپ)
                              : flistxattr(fd, buf, ناپ);
        إذا (n < 0)
                أرجع -errno;

        أكد(ناپ == 0 || (size_t) n <= ناپ);

        إذا (n > ودھ_توں_ودھ_عدد) /* We couldn't return this as 'int' anymore */
                أرجع -E2BIG;

        أرجع (صحيح) n;
}

صحيح listxattr_at_malloc(صحيح fd, ثابت حرف *رستہ, صحيح at_flags, حرف **ret) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_تھاواں_رستہ)) == 0);
        أكد(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &رستہ, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        size_t l = 100;
        لكل (بنانشانی n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                إذا (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        أرجع -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                إذا (!v)
                        أرجع -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_اندرونی(fd, رستہ, at_flags, by_procfs, v, l);
                إذا (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        أرجع r;
                }
                إذا (r != -ERANGE)
                        أرجع r;

                r = listxattr_pinned_اندرونی(fd, رستہ, at_flags, by_procfs, عدم, 0);
                إذا (r < 0)
                        أرجع r;

                l = (size_t) r;
        }
}

صحيح xsetxattr_بھریا(
                صحيح fd,
                ثابت حرف *رستہ,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *قدر,
                size_t ناپ,
                صحيح xattr_flags) {

        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_تھاواں_رستہ)) == 0);
        أكد(name);
        أكد(قدر || ناپ == 0);

        إذا (ناپ == SIZE_MAX)
                ناپ = لڑی_لمبائی(قدر);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ حرف *old_قدر = عدم;
        size_t old_ناپ;

        إذا (xattr_flags == 0 &&
            getxattr_at_malloc(fd, رستہ, name, at_flags, &old_قدر, &old_ناپ) >= 0 &&
            memcmp_nn(old_قدر, old_ناپ, قدر, ناپ) == 0)
                أرجع 0;

        إذا (have_xattrat && !isempty(رستہ)) {
                بنية xattr_دلیلاں دلیلاں = {
                        .قدر = PTR_منزل_UINT64(قدر),
                        .ناپ = ناپ,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, رستہ,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &دلیلاں, حجم(دلیلاں)));
                إذا (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        أرجع r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &رستہ, &at_flags, &opened_fd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        إذا (رستہ)
                r = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(رستہ, name, قدر, ناپ, xattr_flags)
                                                           : lsetxattr(رستہ, name, قدر, ناپ, xattr_flags);
        وإلا
                r = by_procfs ? setxattr(FORMAT_PROC_FD_رستہ(fd), name, قدر, ناپ, xattr_flags)
                              : fsetxattr(fd, name, قدر, ناپ, xattr_flags);
        إذا (r < 0)
                أرجع -errno;

        أرجع 0;
}

صحيح xsetxattr_strv(صحيح fd, ثابت حرف *رستہ, صحيح at_flags, ثابت حرف *name, حرف * ثابت *l) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t ناپ;
        صحيح r;

        أكد(name);

        r = strv_make_nulstr(l, &nulstr, &ناپ);
        إذا (r < 0)
                أرجع r;

        أرجع xsetxattr_بھریا(fd, رستہ, at_flags, name, nulstr, ناپ, /* xattr_flags= */ 0);
}

صحيح xremovexattr(صحيح fd, ثابت حرف *رستہ, صحيح at_flags, ثابت حرف *name) {
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_تھاواں_رستہ)) == 0);
        أكد(name);

        إذا (have_xattrat && !isempty(رستہ)) {
                r = RET_NERRNO(removexattrat(fd, رستہ,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                إذا (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        أرجع r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &رستہ, &at_flags, &tfd, &by_procfs);
        إذا (r < 0)
                أرجع r;

        إذا (رستہ)
                r = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(رستہ, name)
                                                           : lremovexattr(رستہ, name);
        وإلا
                r = by_procfs ? removexattr(FORMAT_PROC_FD_رستہ(fd), name)
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
        إذا (!timestamp_is_سیٹ(u))
                أرجع -EIO;

        *ret = u;
        أرجع 0;
}

صحيح getcrtime_at(
                صحيح fd,
                ثابت حرف *رستہ,
                صحيح at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = عدم;
        بنية statx sx;
        usec_t a, b;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_تھاواں_رستہ)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_بھریا(fd,
                        رستہ,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        إذا (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_لدو(&sx.stx_btime);
        وإلا
                a = USEC_INFINITY;

        size_t le_ناپ;
        r = getxattr_at_malloc(fd, رستہ, "user.crtime_usec", at_flags, (حرف**) &le, &le_ناپ);
        إذا (r >= 0) {
                إذا (le_ناپ != حجم(*le))
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

        إذا (!timestamp_is_سیٹ(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        أرجع xsetxattr_بھریا(fd, /* path= */ عدم, AT_خالی_تھاواں_رستہ,
                              "user.crtime_usec", (ثابت حرف*) &le, حجم(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ثابت حرف *name) {
        أرجع STR_وچ_سیٹ(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ثابت حرف *name) {
        أرجع streq(ASSERT_PTR(name), "security.selinux");
}
