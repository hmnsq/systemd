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
ایستا thread_local bool have_xattrat = true;

ایستا صحیح normalize_and_maybe_pin_inode(
                صحیح *fd,
                ثابت نویسه **path,
                صحیح *at_flags,
                صحیح *ret_tfd,
                bool *ret_opath) {

        صحیح r;

        أكد(fd);
        أكد(*fd >= 0 || *fd == AT_FDCWD);
        أكد(path);
        أكد(at_flags);
        أكد(ret_tfd);
        أكد(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        اگر (*fd == AT_FDCWD) {
                اگر (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                بازگشت 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        اگر (!*path) {
                r = fd_is_opath(*fd);
                اگر (r < 0)
                        بازگشت r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                بازگشت 0;
        }

        /* If both have been specified, then we go via O_PATH */

        صحیح tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        اگر (tfd < 0)
                بازگشت -errno;

        *fd = *ret_tfd = tfd;
        *path = پوچ;
        *ret_opath = true;

        بازگشت 0;
}

ایستا ssize_t getxattr_pinned_internal(
                صحیح fd,
                ثابت نویسه *path,
                صحیح at_flags,
                bool by_procfs,
                ثابت نویسه *name,
                نویسه *buf,
                size_t size) {

        ssize_t n;

        أكد(!path || !isempty(path));
        أكد((fd >= 0) == !path);
        أكد(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        أكد(name);
        أكد(buf || size == 0);

        اگر (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        وگرنه
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        اگر (n < 0)
                بازگشت -errno;

        أكد(size == 0 || (size_t) n <= size);
        بازگشت n;
}

صحیح getxattr_at_malloc(
                صحیح fd,
                ثابت نویسه *path,
                ثابت نویسه *name,
                صحیح at_flags,
                نویسه **ret,
                size_t *ret_size) {

        _cleanup_close_ صحیح opened_fd = -EBADF;
        bool by_procfs;
        صحیح r;

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
        اگر (r < 0)
                بازگشت r;

        size_t l = 100;
        برای (unsigned n_attempts = 7;;) {
                _cleanup_free_ نویسه *v = پوچ;

                اگر (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        بازگشت -EBUSY;
                n_attempts--;

                v = new(نویسه, l+1);
                اگر (!v)
                        بازگشت -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                اگر (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        اگر (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                بازگشت -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        اگر (ret_size)
                                *ret_size = (size_t) n;

                        بازگشت 0;
                }
                اگر (n != -ERANGE)
                        بازگشت (صحیح) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, پوچ, 0);
                اگر (n < 0)
                        بازگشت (صحیح) n;

                l = (size_t) n;
        }
}

صحیح getxattr_at_bool(صحیح fd, ثابت نویسه *path, ثابت نویسه *name, صحیح at_flags) {
        _cleanup_free_ نویسه *v = پوچ;
        صحیح r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ پوچ);
        اگر (r < 0)
                بازگشت r;

        بازگشت parse_boolean(v);
}

صحیح getxattr_at_strv(صحیح fd, ثابت نویسه *path, ثابت نویسه *name, صحیح at_flags, نویسه ***ret_strv) {
        _cleanup_free_ نویسه *nulstr = پوچ;
        size_t nulstr_size;
        صحیح r;

        أكد(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        اگر (r < 0)
                بازگشت r;

        _cleanup_strv_free_ نویسه **l = strv_parse_nulstr(nulstr, nulstr_size);
        اگر (!l)
                بازگشت -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        بازگشت 0;
}

ایستا صحیح listxattr_pinned_internal(
                صحیح fd,
                ثابت نویسه *path,
                صحیح at_flags,
                bool by_procfs,
                نویسه *buf,
                size_t size) {

        ssize_t n;

        أكد(!path || !isempty(path));
        أكد((fd >= 0) == !path);
        أكد(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        أكد(buf || size == 0);

        اگر (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        وگرنه
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        اگر (n < 0)
                بازگشت -errno;

        أكد(size == 0 || (size_t) n <= size);

        اگر (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                بازگشت -E2BIG;

        بازگشت (صحیح) n;
}

صحیح listxattr_at_malloc(صحیح fd, ثابت نویسه *path, صحیح at_flags, نویسه **ret) {
        _cleanup_close_ صحیح opened_fd = -EBADF;
        bool by_procfs;
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        اگر (r < 0)
                بازگشت r;

        size_t l = 100;
        برای (unsigned n_attempts = 7;;) {
                _cleanup_free_ نویسه *v = پوچ;

                اگر (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        بازگشت -EBUSY;
                n_attempts--;

                v = new(نویسه, l+1);
                اگر (!v)
                        بازگشت -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                اگر (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        بازگشت r;
                }
                اگر (r != -ERANGE)
                        بازگشت r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, پوچ, 0);
                اگر (r < 0)
                        بازگشت r;

                l = (size_t) r;
        }
}

صحیح xsetxattr_full(
                صحیح fd,
                ثابت نویسه *path,
                صحیح at_flags,
                ثابت نویسه *name,
                ثابت نویسه *مقدار,
                size_t size,
                صحیح xattr_flags) {

        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(name);
        أكد(مقدار || size == 0);

        اگر (size == SIZE_MAX)
                size = strlen(مقدار);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ نویسه *old_مقدار = پوچ;
        size_t old_size;

        اگر (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_مقدار, &old_size) >= 0 &&
            memcmp_nn(old_مقدار, old_size, مقدار, size) == 0)
                بازگشت 0;

        اگر (have_xattrat && !isempty(path)) {
                ساختار xattr_args args = {
                        .مقدار = PTR_TO_UINT64(مقدار),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, اندازه(args)));
                اگر (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        بازگشت r;

                have_xattrat = false;
        }

        _cleanup_close_ صحیح opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        اگر (r < 0)
                بازگشت r;

        اگر (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, مقدار, size, xattr_flags)
                                                           : lsetxattr(path, name, مقدار, size, xattr_flags);
        وگرنه
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, مقدار, size, xattr_flags)
                              : fsetxattr(fd, name, مقدار, size, xattr_flags);
        اگر (r < 0)
                بازگشت -errno;

        بازگشت 0;
}

صحیح xsetxattr_strv(صحیح fd, ثابت نویسه *path, صحیح at_flags, ثابت نویسه *name, نویسه * ثابت *l) {
        _cleanup_free_ نویسه *nulstr = پوچ;
        size_t size;
        صحیح r;

        أكد(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        اگر (r < 0)
                بازگشت r;

        بازگشت xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

صحیح xremovexattr(صحیح fd, ثابت نویسه *path, صحیح at_flags, ثابت نویسه *name) {
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(name);

        اگر (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                اگر (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        بازگشت r;

                have_xattrat = false;
        }

        _cleanup_close_ صحیح tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        اگر (r < 0)
                بازگشت r;

        اگر (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        وگرنه
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        اگر (r < 0)
                بازگشت -errno;

        بازگشت 0;
}

ایستا صحیح parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        أكد(ret);

        assert_cc(اندازه(usec_t) == اندازه(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        اگر (!timestamp_is_set(u))
                بازگشت -EIO;

        *ret = u;
        بازگشت 0;
}

صحیح getcrtime_at(
                صحیح fd,
                ثابت نویسه *path,
                صحیح at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = پوچ;
        ساختار statx sx;
        usec_t a, b;
        صحیح r;

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
        اگر (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        وگرنه
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (نویسه**) &le, &le_size);
        اگر (r >= 0) {
                اگر (le_size != اندازه(*le))
                        r = -EIO;
                وگرنه
                        r = parse_crtime(*le, &b);
        }
        اگر (r < 0) {
                اگر (a != USEC_INFINITY) {
                        اگر (ret)
                                *ret = a;
                        بازگشت 0;
                }

                بازگشت r;
        }

        اگر (ret)
                *ret = MIN(a, b);
        بازگشت 0;
}

صحیح fd_setcrtime(صحیح fd, usec_t usec) {
        le64_t le;

        أكد(fd >= 0);

        اگر (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        بازگشت xsetxattr_full(fd, /* path= */ پوچ, AT_EMPTY_PATH,
                              "user.crtime_usec", (ثابت نویسه*) &le, اندازه(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ثابت نویسه *name) {
        بازگشت STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ثابت نویسه *name) {
        بازگشت streq(ASSERT_PTR(name), "security.selinux");
}
