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

        אם (*fd == AT_FDCWD) {
                אם (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                החזר 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        אם (!*path) {
                r = fd_is_opath(*fd);
                אם (r < 0)
                        החזר r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                החזר 0;
        }

        /* If both have been specified, then we go via O_PATH */

        صحيح tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        אם (tfd < 0)
                החזר -errno;

        *fd = *ret_tfd = tfd;
        *path = عدم;
        *ret_opath = true;

        החזר 0;
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

        אם (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        אחרת
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        אם (n < 0)
                החזר -errno;

        أكد(size == 0 || (size_t) n <= size);
        החזר n;
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
        אם (r < 0)
                החזר r;

        size_t l = 100;
        עבור (unsigned n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                אם (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        החזר -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                אם (!v)
                        החזר -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                אם (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        אם (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                החזר -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        אם (ret_size)
                                *ret_size = (size_t) n;

                        החזר 0;
                }
                אם (n != -ERANGE)
                        החזר (صحيح) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, عدم, 0);
                אם (n < 0)
                        החזר (صحيح) n;

                l = (size_t) n;
        }
}

صحيح getxattr_at_bool(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags) {
        _cleanup_free_ حرف *v = عدم;
        صحيح r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ عدم);
        אם (r < 0)
                החזר r;

        החזר parse_boolean(v);
}

صحيح getxattr_at_strv(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags, حرف ***ret_strv) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t nulstr_size;
        صحيح r;

        أكد(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        אם (r < 0)
                החזר r;

        _cleanup_strv_free_ حرف **l = strv_parse_nulstr(nulstr, nulstr_size);
        אם (!l)
                החזר -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        החזר 0;
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

        אם (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        אחרת
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        אם (n < 0)
                החזר -errno;

        أكد(size == 0 || (size_t) n <= size);

        אם (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                החזר -E2BIG;

        החזר (صحيح) n;
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
        אם (r < 0)
                החזר r;

        size_t l = 100;
        עבור (unsigned n_attempts = 7;;) {
                _cleanup_free_ حرف *v = عدم;

                אם (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        החזר -EBUSY;
                n_attempts--;

                v = new(حرف, l+1);
                אם (!v)
                        החזר -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                אם (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        החזר r;
                }
                אם (r != -ERANGE)
                        החזר r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, عدم, 0);
                אם (r < 0)
                        החזר r;

                l = (size_t) r;
        }
}

صحيح xsetxattr_full(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *value,
                size_t size,
                صحيح xattr_flags) {

        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(name);
        أكد(value || size == 0);

        אם (size == SIZE_MAX)
                size = strlen(value);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ حرف *old_value = عدم;
        size_t old_size;

        אם (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_value, &old_size) >= 0 &&
            memcmp_nn(old_value, old_size, value, size) == 0)
                החזר 0;

        אם (have_xattrat && !isempty(path)) {
                بنية xattr_args args = {
                        .value = PTR_TO_UINT64(value),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, حجم(args)));
                אם (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        החזר r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        אם (r < 0)
                החזר r;

        אם (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, value, size, xattr_flags)
                                                           : lsetxattr(path, name, value, size, xattr_flags);
        אחרת
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, value, size, xattr_flags)
                              : fsetxattr(fd, name, value, size, xattr_flags);
        אם (r < 0)
                החזר -errno;

        החזר 0;
}

صحيح xsetxattr_strv(صحيح fd, ثابت حرف *path, صحيح at_flags, ثابت حرف *name, حرف * ثابت *l) {
        _cleanup_free_ حرف *nulstr = عدم;
        size_t size;
        صحيح r;

        أكد(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        אם (r < 0)
                החזר r;

        החזר xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

صحيح xremovexattr(صحيح fd, ثابت حرف *path, صحيح at_flags, ثابت حرف *name) {
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        أكد(name);

        אם (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                אם (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        החזר r;

                have_xattrat = false;
        }

        _cleanup_close_ صحيح tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        אם (r < 0)
                החזר r;

        אם (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        אחרת
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        אם (r < 0)
                החזר -errno;

        החזר 0;
}

ساكن صحيح parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        أكد(ret);

        assert_cc(حجم(usec_t) == حجم(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        אם (!timestamp_is_set(u))
                החזר -EIO;

        *ret = u;
        החזר 0;
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
        אם (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        אחרת
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (حرف**) &le, &le_size);
        אם (r >= 0) {
                אם (le_size != حجم(*le))
                        r = -EIO;
                אחרת
                        r = parse_crtime(*le, &b);
        }
        אם (r < 0) {
                אם (a != USEC_INFINITY) {
                        אם (ret)
                                *ret = a;
                        החזר 0;
                }

                החזר r;
        }

        אם (ret)
                *ret = MIN(a, b);
        החזר 0;
}

صحيح fd_setcrtime(صحيح fd, usec_t usec) {
        le64_t le;

        أكد(fd >= 0);

        אם (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        החזר xsetxattr_full(fd, /* path= */ عدم, AT_EMPTY_PATH,
                              "user.crtime_usec", (ثابت حرف*) &le, حجم(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ثابت حرف *name) {
        החזר STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ثابت حرف *name) {
        החזר streq(ASSERT_PTR(name), "security.selinux");
}
