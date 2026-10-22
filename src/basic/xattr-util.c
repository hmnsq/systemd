/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/xattr.h>
#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "sparse-endian.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
ಸ್ಥಿರ thread_local bool have_xattrat = true;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ normalize_and_maybe_pin_inode(
                ಪೂರ್ಣಾಙ್ಕ *fd,
                ಧ್ರುವ ವರ್ಣ **path,
                ಪೂರ್ಣಾಙ್ಕ *at_flags,
                ಪೂರ್ಣಾಙ್ಕ *ret_tfd,
                bool *ret_opath) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd);
        ಪ್ರತಿಜ್ಞಾ(*fd >= 0 || *fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(at_flags);
        ಪ್ರತಿಜ್ಞಾ(ret_tfd);
        ಪ್ರತಿಜ್ಞಾ(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        ಯದಿ (*fd == AT_FDCWD) {
                ಯದಿ (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        ಯದಿ (!*path) {
                r = fd_is_opath(*fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ಪೂರ್ಣಾಙ್ಕ tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ಯದಿ (tfd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *fd = *ret_tfd = tfd;
        *path = ರಿಕ್ತ;
        *ret_opath = true;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ssize_t getxattr_pinned_internal(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                bool by_procfs,
                ಧ್ರುವ ವರ್ಣ *name,
                ವರ್ಣ *buf,
                size_t size) {

        ssize_t n;

        ಪ್ರತಿಜ್ಞಾ(!path || !isempty(path));
        ಪ್ರತಿಜ್ಞಾ((fd >= 0) == !path);
        ಪ್ರತಿಜ್ಞಾ(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(buf || size == 0);

        ಯದಿ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        ಅನ್ಯಥಾ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿಜ್ಞಾ(size == 0 || (size_t) n <= size);
        ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ getxattr_at_malloc(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *name,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                ವರ್ಣ **ret,
                size_t *ret_size) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;
        bool by_procfs;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        size_t l = 100;
        ಪ್ರತಿ (unsigned n_attempts = 7;;) {
                _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;

                ಯದಿ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ಪ್ರತ್ಯಾಗಮ -EBUSY;
                n_attempts--;

                v = new(ವರ್ಣ, l+1);
                ಯದಿ (!v)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                ಯದಿ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ಯದಿ (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ಯದಿ (ret_size)
                                *ret_size = (size_t) n;

                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (n != -ERANGE)
                        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, ರಿಕ್ತ, 0);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;

                l = (size_t) n;
        }
}

ಪೂರ್ಣಾಙ್ಕ getxattr_at_bool(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ at_flags) {
        _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ parse_boolean(v);
}

ಪೂರ್ಣಾಙ್ಕ getxattr_at_strv(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ at_flags, ವರ್ಣ ***ret_strv) {
        _cleanup_free_ ವರ್ಣ *nulstr = ರಿಕ್ತ;
        size_t nulstr_size;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_strv_free_ ವರ್ಣ **l = strv_parse_nulstr(nulstr, nulstr_size);
        ಯದಿ (!l)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ listxattr_pinned_internal(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                bool by_procfs,
                ವರ್ಣ *buf,
                size_t size) {

        ssize_t n;

        ಪ್ರತಿಜ್ಞಾ(!path || !isempty(path));
        ಪ್ರತಿಜ್ಞಾ((fd >= 0) == !path);
        ಪ್ರತಿಜ್ಞಾ(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        ಪ್ರತಿಜ್ಞಾ(buf || size == 0);

        ಯದಿ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        ಅನ್ಯಥಾ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿಜ್ಞಾ(size == 0 || (size_t) n <= size);

        ಯದಿ (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;
}

ಪೂರ್ಣಾಙ್ಕ listxattr_at_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, ವರ್ಣ **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;
        bool by_procfs;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        size_t l = 100;
        ಪ್ರತಿ (unsigned n_attempts = 7;;) {
                _cleanup_free_ ವರ್ಣ *v = ರಿಕ್ತ;

                ಯದಿ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ಪ್ರತ್ಯಾಗಮ -EBUSY;
                n_attempts--;

                v = new(ವರ್ಣ, l+1);
                ಯದಿ (!v)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                ಯದಿ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ಪ್ರತ್ಯಾಗಮ r;
                }
                ಯದಿ (r != -ERANGE)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, ರಿಕ್ತ, 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                l = (size_t) r;
        }
}

ಪೂರ್ಣಾಙ್ಕ xsetxattr_full(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                ಧ್ರುವ ವರ್ಣ *name,
                ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ,
                size_t size,
                ಪೂರ್ಣಾಙ್ಕ xattr_flags) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || size == 0);

        ಯದಿ (size == SIZE_MAX)
                size = strlen(ಮೌಲ್ಯ);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ ವರ್ಣ *old_ಮೌಲ್ಯ = ರಿಕ್ತ;
        size_t old_size;

        ಯದಿ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_ಮೌಲ್ಯ, &old_size) >= 0 &&
            memcmp_nn(old_ಮೌಲ್ಯ, old_size, ಮೌಲ್ಯ, size) == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (have_xattrat && !isempty(path)) {
                ಸಂರಚನಾ xattr_args args = {
                        .ಮೌಲ್ಯ = PTR_TO_UINT64(ಮೌಲ್ಯ),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, ಪರಿಮಾಣ(args)));
                ಯದಿ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ಪ್ರತ್ಯಾಗಮ r;

                have_xattrat = false;
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, ಮೌಲ್ಯ, size, xattr_flags)
                                                           : lsetxattr(path, name, ಮೌಲ್ಯ, size, xattr_flags);
        ಅನ್ಯಥಾ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, ಮೌಲ್ಯ, size, xattr_flags)
                              : fsetxattr(fd, name, ಮೌಲ್ಯ, size, xattr_flags);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ xsetxattr_strv(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ * ಧ್ರುವ *l) {
        _cleanup_free_ ವರ್ಣ *nulstr = ರಿಕ್ತ;
        size_t size;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

ಪೂರ್ಣಾಙ್ಕ xremovexattr(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ at_flags, ಧ್ರುವ ವರ್ಣ *name) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ಪ್ರತಿಜ್ಞಾ(name);

        ಯದಿ (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ಯದಿ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ಪ್ರತ್ಯಾಗಮ r;

                have_xattrat = false;
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        ಅನ್ಯಥಾ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        ಪ್ರತಿಜ್ಞಾ(ret);

        assert_cc(ಪರಿಮಾಣ(usec_t) == ಪರಿಮಾಣ(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ಯದಿ (!timestamp_is_set(u))
                ಪ್ರತ್ಯಾಗಮ -EIO;

        *ret = u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ getcrtime_at(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ರಿಕ್ತ;
        ಸಂರಚನಾ statx sx;
        usec_t a, b;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        ಯದಿ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        ಅನ್ಯಥಾ
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (ವರ್ಣ**) &le, &le_size);
        ಯದಿ (r >= 0) {
                ಯದಿ (le_size != ಪರಿಮಾಣ(*le))
                        r = -EIO;
                ಅನ್ಯಥಾ
                        r = parse_crtime(*le, &b);
        }
        ಯದಿ (r < 0) {
                ಯದಿ (a != USEC_INFINITY) {
                        ಯದಿ (ret)
                                *ret = a;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret)
                *ret = MIN(a, b);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ fd_setcrtime(ಪೂರ್ಣಾಙ್ಕ fd, usec_t usec) {
        le64_t le;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ಪ್ರತ್ಯಾಗಮ xsetxattr_full(fd, /* path= */ ರಿಕ್ತ, AT_EMPTY_PATH,
                              "user.crtime_usec", (ಧ್ರುವ ವರ್ಣ*) &le, ಪರಿಮಾಣ(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ಧ್ರುವ ವರ್ಣ *name) {
        ಪ್ರತ್ಯಾಗಮ streq(ASSERT_PTR(name), "security.selinux");
}
