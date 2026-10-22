/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/xattr.h>
#অন্তর্ভাব <threads.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "nulstr-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "sparse-endian.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
স্থির thread_local bool have_xattrat = true;

স্থির পূর্ণসংখ্যা normalize_and_maybe_pin_inode(
                পূর্ণসংখ্যা *fd,
                ধ্রুবক অক্ষর **path,
                পূর্ণসংখ্যা *at_flags,
                পূর্ণসংখ্যা *ret_tfd,
                bool *ret_opath) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd);
        প্রতিজ্ঞা(*fd >= 0 || *fd == AT_FDCWD);
        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(at_flags);
        প্রতিজ্ঞা(ret_tfd);
        প্রতিজ্ঞা(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        যদি (*fd == AT_FDCWD) {
                যদি (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ফেরত 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        যদি (!*path) {
                r = fd_is_opath(*fd);
                যদি (r < 0)
                        ফেরত r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ফেরত 0;
        }

        /* If both have been specified, then we go via O_PATH */

        পূর্ণসংখ্যা tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        যদি (tfd < 0)
                ফেরত -errno;

        *fd = *ret_tfd = tfd;
        *path = ফাঁকা;
        *ret_opath = true;

        ফেরত 0;
}

স্থির ssize_t getxattr_pinned_internal(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা at_flags,
                bool by_procfs,
                ধ্রুবক অক্ষর *name,
                অক্ষর *buf,
                size_t আকার_2) {

        ssize_t n;

        প্রতিজ্ঞা(!path || !isempty(path));
        প্রতিজ্ঞা((fd >= 0) == !path);
        প্রতিজ্ঞা(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(buf || আকার_2 == 0);

        যদি (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, আকার_2)
                                                           : lgetxattr(path, name, buf, আকার_2);
        অন্যথা
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, আকার_2)
                              : fgetxattr(fd, name, buf, আকার_2);
        যদি (n < 0)
                ফেরত -errno;

        প্রতিজ্ঞা(আকার_2 == 0 || (size_t) n <= আকার_2);
        ফেরত n;
}

পূর্ণসংখ্যা getxattr_at_malloc(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *name,
                পূর্ণসংখ্যা at_flags,
                অক্ষর **ret,
                size_t *ret_আকার) {

        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;
        bool by_procfs;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        যদি (r < 0)
                ফেরত r;

        size_t l = 100;
        জন্য (unsigned n_attempts = 7;;) {
                _cleanup_free_ অক্ষর *v = ফাঁকা;

                যদি (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ফেরত -EBUSY;
                n_attempts--;

                v = new(অক্ষর, l+1);
                যদি (!v)
                        ফেরত -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                যদি (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        যদি (!ret_আকার && n > 1 && memchr(v, 0, n - 1))
                                ফেরত -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        যদি (ret_আকার)
                                *ret_আকার = (size_t) n;

                        ফেরত 0;
                }
                যদি (n != -ERANGE)
                        ফেরত (পূর্ণসংখ্যা) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, ফাঁকা, 0);
                যদি (n < 0)
                        ফেরত (পূর্ণসংখ্যা) n;

                l = (size_t) n;
        }
}

পূর্ণসংখ্যা getxattr_at_bool(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা at_flags) {
        _cleanup_free_ অক্ষর *v = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        ফেরত parse_boolean(v);
}

পূর্ণসংখ্যা getxattr_at_strv(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা at_flags, অক্ষর ***ret_strv) {
        _cleanup_free_ অক্ষর *nulstr = ফাঁকা;
        size_t nulstr_আকার;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_আকার);
        যদি (r < 0)
                ফেরত r;

        _cleanup_strv_free_ অক্ষর **l = strv_parse_nulstr(nulstr, nulstr_আকার);
        যদি (!l)
                ফেরত -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা listxattr_pinned_internal(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা at_flags,
                bool by_procfs,
                অক্ষর *buf,
                size_t আকার_2) {

        ssize_t n;

        প্রতিজ্ঞা(!path || !isempty(path));
        প্রতিজ্ঞা((fd >= 0) == !path);
        প্রতিজ্ঞা(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        প্রতিজ্ঞা(buf || আকার_2 == 0);

        যদি (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, আকার_2)
                                                           : llistxattr(path, buf, আকার_2);
        অন্যথা
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, আকার_2)
                              : flistxattr(fd, buf, আকার_2);
        যদি (n < 0)
                ফেরত -errno;

        প্রতিজ্ঞা(আকার_2 == 0 || (size_t) n <= আকার_2);

        যদি (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                ফেরত -E2BIG;

        ফেরত (পূর্ণসংখ্যা) n;
}

পূর্ণসংখ্যা listxattr_at_malloc(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, অক্ষর **ret) {
        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;
        bool by_procfs;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        যদি (r < 0)
                ফেরত r;

        size_t l = 100;
        জন্য (unsigned n_attempts = 7;;) {
                _cleanup_free_ অক্ষর *v = ফাঁকা;

                যদি (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ফেরত -EBUSY;
                n_attempts--;

                v = new(অক্ষর, l+1);
                যদি (!v)
                        ফেরত -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                যদি (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ফেরত r;
                }
                যদি (r != -ERANGE)
                        ফেরত r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, ফাঁকা, 0);
                যদি (r < 0)
                        ফেরত r;

                l = (size_t) r;
        }
}

পূর্ণসংখ্যা xsetxattr_full(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা at_flags,
                ধ্রুবক অক্ষর *name,
                ধ্রুবক অক্ষর *মান,
                size_t আকার_2,
                পূর্ণসংখ্যা xattr_flags) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(মান || আকার_2 == 0);

        যদি (আকার_2 == SIZE_MAX)
                আকার_2 = strlen(মান);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ অক্ষর *old_মান = ফাঁকা;
        size_t old_আকার;

        যদি (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_মান, &old_আকার) >= 0 &&
            memcmp_nn(old_মান, old_আকার, মান, আকার_2) == 0)
                ফেরত 0;

        যদি (have_xattrat && !isempty(path)) {
                কাঠামো xattr_args args = {
                        .মান = PTR_TO_UINT64(মান),
                        .আকার_2 = আকার_2,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, আকার(args)));
                যদি (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ফেরত r;

                have_xattrat = false;
        }

        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        যদি (r < 0)
                ফেরত r;

        যদি (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, মান, আকার_2, xattr_flags)
                                                           : lsetxattr(path, name, মান, আকার_2, xattr_flags);
        অন্যথা
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, মান, আকার_2, xattr_flags)
                              : fsetxattr(fd, name, মান, আকার_2, xattr_flags);
        যদি (r < 0)
                ফেরত -errno;

        ফেরত 0;
}

পূর্ণসংখ্যা xsetxattr_strv(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, ধ্রুবক অক্ষর *name, অক্ষর * ধ্রুবক *l) {
        _cleanup_free_ অক্ষর *nulstr = ফাঁকা;
        size_t আকার_2;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(name);

        r = strv_make_nulstr(l, &nulstr, &আকার_2);
        যদি (r < 0)
                ফেরত r;

        ফেরত xsetxattr_full(fd, path, at_flags, name, nulstr, আকার_2, /* xattr_flags= */ 0);
}

পূর্ণসংখ্যা xremovexattr(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, ধ্রুবক অক্ষর *name) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(name);

        যদি (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                যদি (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ফেরত r;

                have_xattrat = false;
        }

        _cleanup_close_ পূর্ণসংখ্যা tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        যদি (r < 0)
                ফেরত r;

        যদি (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        অন্যথা
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        যদি (r < 0)
                ফেরত -errno;

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        প্রতিজ্ঞা(ret);

        assert_cc(আকার(usec_t) == আকার(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        যদি (!timestamp_is_set(u))
                ফেরত -EIO;

        *ret = u;
        ফেরত 0;
}

পূর্ণসংখ্যা getcrtime_at(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ফাঁকা;
        কাঠামো statx sx;
        usec_t a, b;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        যদি (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        অন্যথা
                a = USEC_INFINITY;

        size_t le_আকার;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (অক্ষর**) &le, &le_আকার);
        যদি (r >= 0) {
                যদি (le_আকার != আকার(*le))
                        r = -EIO;
                অন্যথা
                        r = parse_crtime(*le, &b);
        }
        যদি (r < 0) {
                যদি (a != USEC_INFINITY) {
                        যদি (ret)
                                *ret = a;
                        ফেরত 0;
                }

                ফেরত r;
        }

        যদি (ret)
                *ret = MIN(a, b);
        ফেরত 0;
}

পূর্ণসংখ্যা fd_setcrtime(পূর্ণসংখ্যা fd, usec_t usec) {
        le64_t le;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ফেরত xsetxattr_full(fd, /* path= */ ফাঁকা, AT_EMPTY_PATH,
                              "user.crtime_usec", (ধ্রুবক অক্ষর*) &le, আকার(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ধ্রুবক অক্ষর *name) {
        ফেরত STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ধ্রুবক অক্ষর *name) {
        ফেরত streq(ASSERT_PTR(name), "security.selinux");
}
