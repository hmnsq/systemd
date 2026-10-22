/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/xattr.h>
#شامل <threads.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "nulstr-util.h"
#شامل "parse-util.h"
#شامل "sparse-endian.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "time-util.h"
#شامل "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
ساکن thread_local bool have_xattrat = true;

ساکن صحیح normalize_and_maybe_pin_inode(
                صحیح *fd,
                ثابت حرف **راستہ,
                صحیح *at_flags,
                صحیح *ret_tfd,
                bool *ret_opath) {

        صحیح r;

        أكد(fd);
        أكد(*fd >= 0 || *fd == AT_FDCWD);
        أكد(راستہ);
        أكد(at_flags);
        أكد(ret_tfd);
        أكد(ret_opath);

        *راستہ = خالی_خانے_مقصد_null(*راستہ); /* Normalize "" to NULL */

        اگر (*fd == AT_FDCWD) {
                اگر (!*راستہ) /* Both unspecified? Then operate on current working directory */
                        *راستہ = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                واپس 0;
        }

        *at_flags |= AT_خالی_خانے_راستہ;

        اگر (!*راستہ) {
                r = fd_is_opath(*fd);
                اگر (r < 0)
                        واپس r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                واپس 0;
        }

        /* If both have been specified, then we go via O_PATH */

        صحیح tfd = openat(*fd, *راستہ, O_راستہ|O_CLOEXEC|(FLAGS_سیٹ(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        اگر (tfd < 0)
                واپس -errno;

        *fd = *ret_tfd = tfd;
        *راستہ = تہی;
        *ret_opath = true;

        واپس 0;
}

ساکن ssize_t getxattr_pinned_اندرونی(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح at_flags,
                bool by_procfs,
                ثابت حرف *name,
                حرف *buf,
                size_t سائز) {

        ssize_t n;

        أكد(!راستہ || !isempty(راستہ));
        أكد((fd >= 0) == !راستہ);
        أكد(راستہ || FLAGS_سیٹ(at_flags, AT_خالی_خانے_راستہ));
        أكد(name);
        أكد(buf || سائز == 0);

        اگر (راستہ)
                n = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(راستہ, name, buf, سائز)
                                                           : lgetxattr(راستہ, name, buf, سائز);
        ورنہ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_راستہ(fd), name, buf, سائز)
                              : fgetxattr(fd, name, buf, سائز);
        اگر (n < 0)
                واپس -errno;

        أكد(سائز == 0 || (size_t) n <= سائز);
        واپس n;
}

صحیح getxattr_at_malloc(
                صحیح fd,
                ثابت حرف *راستہ,
                ثابت حرف *name,
                صحیح at_flags,
                حرف **ret,
                size_t *ret_سائز) {

        _cleanup_close_ صحیح opened_fd = -EBADF;
        bool by_procfs;
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد(name);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ)) == 0);
        أكد(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &راستہ, &at_flags, &opened_fd, &by_procfs);
        اگر (r < 0)
                واپس r;

        size_t l = 100;
        برائے (غیرعلامتی n_attempts = 7;;) {
                _cleanup_free_ حرف *v = تہی;

                اگر (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        واپس -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                اگر (!v)
                        واپس -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_اندرونی(fd, راستہ, at_flags, by_procfs, name, v, l);
                اگر (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        اگر (!ret_سائز && n > 1 && memchr(v, 0, n - 1))
                                واپس -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        اگر (ret_سائز)
                                *ret_سائز = (size_t) n;

                        واپس 0;
                }
                اگر (n != -ERANGE)
                        واپس (صحیح) n;

                n = getxattr_pinned_اندرونی(fd, راستہ, at_flags, by_procfs, name, تہی, 0);
                اگر (n < 0)
                        واپس (صحیح) n;

                l = (size_t) n;
        }
}

صحیح getxattr_at_bool(صحیح fd, ثابت حرف *راستہ, ثابت حرف *name, صحیح at_flags) {
        _cleanup_free_ حرف *v = تہی;
        صحیح r;

        r = getxattr_at_malloc(fd, راستہ, name, at_flags, &v, /* ret_size= */ تہی);
        اگر (r < 0)
                واپس r;

        واپس parse_boolean(v);
}

صحیح getxattr_at_strv(صحیح fd, ثابت حرف *راستہ, ثابت حرف *name, صحیح at_flags, حرف ***ret_strv) {
        _cleanup_free_ حرف *nulstr = تہی;
        size_t nulstr_سائز;
        صحیح r;

        أكد(ret_strv);

        r = getxattr_at_malloc(fd, راستہ, name, at_flags, &nulstr, &nulstr_سائز);
        اگر (r < 0)
                واپس r;

        _cleanup_strv_free_ حرف **l = strv_parse_nulstr(nulstr, nulstr_سائز);
        اگر (!l)
                واپس -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        واپس 0;
}

ساکن صحیح listxattr_pinned_اندرونی(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح at_flags,
                bool by_procfs,
                حرف *buf,
                size_t سائز) {

        ssize_t n;

        أكد(!راستہ || !isempty(راستہ));
        أكد((fd >= 0) == !راستہ);
        أكد(راستہ || FLAGS_سیٹ(at_flags, AT_خالی_خانے_راستہ));
        أكد(buf || سائز == 0);

        اگر (راستہ)
                n = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(راستہ, buf, سائز)
                                                           : llistxattr(راستہ, buf, سائز);
        ورنہ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_راستہ(fd), buf, سائز)
                              : flistxattr(fd, buf, سائز);
        اگر (n < 0)
                واپس -errno;

        أكد(سائز == 0 || (size_t) n <= سائز);

        اگر (n > زیادہ_سے_زیادہ_عدد) /* We couldn't return this as 'int' anymore */
                واپس -E2BIG;

        واپس (صحیح) n;
}

صحیح listxattr_at_malloc(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, حرف **ret) {
        _cleanup_close_ صحیح opened_fd = -EBADF;
        bool by_procfs;
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ)) == 0);
        أكد(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &راستہ, &at_flags, &opened_fd, &by_procfs);
        اگر (r < 0)
                واپس r;

        size_t l = 100;
        برائے (غیرعلامتی n_attempts = 7;;) {
                _cleanup_free_ حرف *v = تہی;

                اگر (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        واپس -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                اگر (!v)
                        واپس -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_اندرونی(fd, راستہ, at_flags, by_procfs, v, l);
                اگر (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        واپس r;
                }
                اگر (r != -ERANGE)
                        واپس r;

                r = listxattr_pinned_اندرونی(fd, راستہ, at_flags, by_procfs, تہی, 0);
                اگر (r < 0)
                        واپس r;

                l = (size_t) r;
        }
}

صحیح xsetxattr_بھرا(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح at_flags,
                ثابت حرف *name,
                ثابت حرف *قدر,
                size_t سائز,
                صحیح xattr_flags) {

        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ)) == 0);
        أكد(name);
        أكد(قدر || سائز == 0);

        اگر (سائز == SIZE_MAX)
                سائز = لڑی_لمبائی(قدر);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ حرف *old_قدر = تہی;
        size_t old_سائز;

        اگر (xattr_flags == 0 &&
            getxattr_at_malloc(fd, راستہ, name, at_flags, &old_قدر, &old_سائز) >= 0 &&
            memcmp_nn(old_قدر, old_سائز, قدر, سائز) == 0)
                واپس 0;

        اگر (have_xattrat && !isempty(راستہ)) {
                ساخت xattr_دلائل دلائل = {
                        .قدر = PTR_مقصد_UINT64(قدر),
                        .سائز = سائز,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, راستہ,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &دلائل, حجم(دلائل)));
                اگر (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        واپس r;

                have_xattrat = false;
        }

        _cleanup_close_ صحیح opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &راستہ, &at_flags, &opened_fd, &by_procfs);
        اگر (r < 0)
                واپس r;

        اگر (راستہ)
                r = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(راستہ, name, قدر, سائز, xattr_flags)
                                                           : lsetxattr(راستہ, name, قدر, سائز, xattr_flags);
        ورنہ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_راستہ(fd), name, قدر, سائز, xattr_flags)
                              : fsetxattr(fd, name, قدر, سائز, xattr_flags);
        اگر (r < 0)
                واپس -errno;

        واپس 0;
}

صحیح xsetxattr_strv(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, ثابت حرف *name, حرف * ثابت *l) {
        _cleanup_free_ حرف *nulstr = تہی;
        size_t سائز;
        صحیح r;

        أكد(name);

        r = strv_make_nulstr(l, &nulstr, &سائز);
        اگر (r < 0)
                واپس r;

        واپس xsetxattr_بھرا(fd, راستہ, at_flags, name, nulstr, سائز, /* xattr_flags= */ 0);
}

صحیح xremovexattr(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, ثابت حرف *name) {
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ)) == 0);
        أكد(name);

        اگر (have_xattrat && !isempty(راستہ)) {
                r = RET_NERRNO(removexattrat(fd, راستہ,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                اگر (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        واپس r;

                have_xattrat = false;
        }

        _cleanup_close_ صحیح tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &راستہ, &at_flags, &tfd, &by_procfs);
        اگر (r < 0)
                واپس r;

        اگر (راستہ)
                r = FLAGS_سیٹ(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(راستہ, name)
                                                           : lremovexattr(راستہ, name);
        ورنہ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_راستہ(fd), name)
                              : fremovexattr(fd, name);
        اگر (r < 0)
                واپس -errno;

        واپس 0;
}

ساکن صحیح parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        أكد(ret);

        assert_cc(حجم(usec_t) == حجم(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        اگر (!timestamp_is_سیٹ(u))
                واپس -EIO;

        *ret = u;
        واپس 0;
}

صحیح getcrtime_at(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = تہی;
        ساخت statx sx;
        usec_t a, b;
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_بھرا(fd,
                        راستہ,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        اگر (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_بھرو(&sx.stx_btime);
        ورنہ
                a = USEC_INFINITY;

        size_t le_سائز;
        r = getxattr_at_malloc(fd, راستہ, "user.crtime_usec", at_flags, (حرف**) &le, &le_سائز);
        اگر (r >= 0) {
                اگر (le_سائز != حجم(*le))
                        r = -EIO;
                ورنہ
                        r = parse_crtime(*le, &b);
        }
        اگر (r < 0) {
                اگر (a != USEC_INFINITY) {
                        اگر (ret)
                                *ret = a;
                        واپس 0;
                }

                واپس r;
        }

        اگر (ret)
                *ret = MIN(a, b);
        واپس 0;
}

صحیح fd_setcrtime(صحیح fd, usec_t usec) {
        le64_t le;

        أكد(fd >= 0);

        اگر (!timestamp_is_سیٹ(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        واپس xsetxattr_بھرا(fd, /* path= */ تہی, AT_خالی_خانے_راستہ,
                              "user.crtime_usec", (ثابت حرف*) &le, حجم(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ثابت حرف *name) {
        واپس STR_اندر_سیٹ(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ثابت حرف *name) {
        واپس streq(ASSERT_PTR(name), "security.selinux");
}
