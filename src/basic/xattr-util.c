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

স্থির পূর্ণাঙ্ক normalize_and_maybe_pin_inode(
                পূর্ণাঙ্ক *fd,
                ধ্রুব বর্ণ **path,
                পূর্ণাঙ্ক *at_flags,
                পূর্ণাঙ্ক *ret_tfd,
                bool *ret_opath) {

        পূর্ণাঙ্ক r;

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
                প্রত্যাগম 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        যদি (!*path) {
                r = fd_is_opath(*fd);
                যদি (r < 0)
                        প্রত্যাগম r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                প্রত্যাগম 0;
        }

        /* If both have been specified, then we go via O_PATH */

        পূর্ণাঙ্ক tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        যদি (tfd < 0)
                প্রত্যাগম -errno;

        *fd = *ret_tfd = tfd;
        *path = রিক্ত;
        *ret_opath = true;

        প্রত্যাগম 0;
}

স্থির ssize_t getxattr_pinned_internal(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক at_flags,
                bool by_procfs,
                ধ্রুব বর্ণ *name,
                বর্ণ *buf,
                size_t size) {

        ssize_t n;

        প্রতিজ্ঞা(!path || !isempty(path));
        প্রতিজ্ঞা((fd >= 0) == !path);
        প্রতিজ্ঞা(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(buf || size == 0);

        যদি (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        অন্যথা
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        যদি (n < 0)
                প্রত্যাগম -errno;

        প্রতিজ্ঞা(size == 0 || (size_t) n <= size);
        প্রত্যাগম n;
}

পূর্ণাঙ্ক getxattr_at_malloc(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                ধ্রুব বর্ণ *name,
                পূর্ণাঙ্ক at_flags,
                বর্ণ **ret,
                size_t *ret_size) {

        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;
        bool by_procfs;
        পূর্ণাঙ্ক r;

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
                প্রত্যাগম r;

        size_t l = 100;
        প্রতি (unsigned n_attempts = 7;;) {
                _cleanup_free_ বর্ণ *v = রিক্ত;

                যদি (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        প্রত্যাগম -EBUSY;
                n_attempts--;

                v = new(বর্ণ, l+1);
                যদি (!v)
                        প্রত্যাগম -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                যদি (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        যদি (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                প্রত্যাগম -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        যদি (ret_size)
                                *ret_size = (size_t) n;

                        প্রত্যাগম 0;
                }
                যদি (n != -ERANGE)
                        প্রত্যাগম (পূর্ণাঙ্ক) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, রিক্ত, 0);
                যদি (n < 0)
                        প্রত্যাগম (পূর্ণাঙ্ক) n;

                l = (size_t) n;
        }
}

পূর্ণাঙ্ক getxattr_at_bool(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক at_flags) {
        _cleanup_free_ বর্ণ *v = রিক্ত;
        পূর্ণাঙ্ক r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম parse_boolean(v);
}

পূর্ণাঙ্ক getxattr_at_strv(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক at_flags, বর্ণ ***ret_strv) {
        _cleanup_free_ বর্ণ *nulstr = রিক্ত;
        size_t nulstr_size;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        যদি (r < 0)
                প্রত্যাগম r;

        _cleanup_strv_free_ বর্ণ **l = strv_parse_nulstr(nulstr, nulstr_size);
        যদি (!l)
                প্রত্যাগম -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক listxattr_pinned_internal(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক at_flags,
                bool by_procfs,
                বর্ণ *buf,
                size_t size) {

        ssize_t n;

        প্রতিজ্ঞা(!path || !isempty(path));
        প্রতিজ্ঞা((fd >= 0) == !path);
        প্রতিজ্ঞা(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        প্রতিজ্ঞা(buf || size == 0);

        যদি (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        অন্যথা
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        যদি (n < 0)
                প্রত্যাগম -errno;

        প্রতিজ্ঞা(size == 0 || (size_t) n <= size);

        যদি (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                প্রত্যাগম -E2BIG;

        প্রত্যাগম (পূর্ণাঙ্ক) n;
}

পূর্ণাঙ্ক listxattr_at_malloc(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, বর্ণ **ret) {
        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;
        bool by_procfs;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        যদি (r < 0)
                প্রত্যাগম r;

        size_t l = 100;
        প্রতি (unsigned n_attempts = 7;;) {
                _cleanup_free_ বর্ণ *v = রিক্ত;

                যদি (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        প্রত্যাগম -EBUSY;
                n_attempts--;

                v = new(বর্ণ, l+1);
                যদি (!v)
                        প্রত্যাগম -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                যদি (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        প্রত্যাগম r;
                }
                যদি (r != -ERANGE)
                        প্রত্যাগম r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, রিক্ত, 0);
                যদি (r < 0)
                        প্রত্যাগম r;

                l = (size_t) r;
        }
}

পূর্ণাঙ্ক xsetxattr_full(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক at_flags,
                ধ্রুব বর্ণ *name,
                ধ্রুব বর্ণ *মান,
                size_t size,
                পূর্ণাঙ্ক xattr_flags) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(মান || size == 0);

        যদি (size == SIZE_MAX)
                size = strlen(মান);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ বর্ণ *old_মান = রিক্ত;
        size_t old_size;

        যদি (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_মান, &old_size) >= 0 &&
            memcmp_nn(old_মান, old_size, মান, size) == 0)
                প্রত্যাগম 0;

        যদি (have_xattrat && !isempty(path)) {
                সংরচনা xattr_args args = {
                        .মান = PTR_TO_UINT64(মান),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, পরিমাণ(args)));
                যদি (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        প্রত্যাগম r;

                have_xattrat = false;
        }

        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, মান, size, xattr_flags)
                                                           : lsetxattr(path, name, মান, size, xattr_flags);
        অন্যথা
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, মান, size, xattr_flags)
                              : fsetxattr(fd, name, মান, size, xattr_flags);
        যদি (r < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক xsetxattr_strv(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, ধ্রুব বর্ণ *name, বর্ণ * ধ্রুব *l) {
        _cleanup_free_ বর্ণ *nulstr = রিক্ত;
        size_t size;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

পূর্ণাঙ্ক xremovexattr(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, ধ্রুব বর্ণ *name) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        প্রতিজ্ঞা(name);

        যদি (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                যদি (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        প্রত্যাগম r;

                have_xattrat = false;
        }

        _cleanup_close_ পূর্ণাঙ্ক tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        অন্যথা
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        যদি (r < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        প্রতিজ্ঞা(ret);

        assert_cc(পরিমাণ(usec_t) == পরিমাণ(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        যদি (!timestamp_is_set(u))
                প্রত্যাগম -EIO;

        *ret = u;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক getcrtime_at(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = রিক্ত;
        সংরচনা statx sx;
        usec_t a, b;
        পূর্ণাঙ্ক r;

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

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (বর্ণ**) &le, &le_size);
        যদি (r >= 0) {
                যদি (le_size != পরিমাণ(*le))
                        r = -EIO;
                অন্যথা
                        r = parse_crtime(*le, &b);
        }
        যদি (r < 0) {
                যদি (a != USEC_INFINITY) {
                        যদি (ret)
                                *ret = a;
                        প্রত্যাগম 0;
                }

                প্রত্যাগম r;
        }

        যদি (ret)
                *ret = MIN(a, b);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক fd_setcrtime(পূর্ণাঙ্ক fd, usec_t usec) {
        le64_t le;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        প্রত্যাগম xsetxattr_full(fd, /* path= */ রিক্ত, AT_EMPTY_PATH,
                              "user.crtime_usec", (ধ্রুব বর্ণ*) &le, পরিমাণ(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ধ্রুব বর্ণ *name) {
        প্রত্যাগম STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ধ্রুব বর্ণ *name) {
        প্রত্যাগম streq(ASSERT_PTR(name), "security.selinux");
}
