/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sys/xattr.h>
#અન્તર્ભાવ <threads.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "nulstr-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "sparse-endian.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
સ્થિર thread_local bool have_xattrat = true;

સ્થિર પૂર્ણાંક normalize_and_maybe_pin_inode(
                પૂર્ણાંક *fd,
                અચલ અક્ષર **path,
                પૂર્ણાંક *at_flags,
                પૂર્ણાંક *ret_tfd,
                bool *ret_opath) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd);
        પ્રતિજ્ઞા(*fd >= 0 || *fd == AT_FDCWD);
        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(at_flags);
        પ્રતિજ્ઞા(ret_tfd);
        પ્રતિજ્ઞા(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        જો (*fd == AT_FDCWD) {
                જો (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                પાછા 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        જો (!*path) {
                r = fd_is_opath(*fd);
                જો (r < 0)
                        પાછા r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                પાછા 0;
        }

        /* If both have been specified, then we go via O_PATH */

        પૂર્ણાંક tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        જો (tfd < 0)
                પાછા -errno;

        *fd = *ret_tfd = tfd;
        *path = ખાલી;
        *ret_opath = true;

        પાછા 0;
}

સ્થિર ssize_t getxattr_pinned_internal(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક at_flags,
                bool by_procfs,
                અચલ અક્ષર *name,
                અક્ષર *buf,
                size_t size) {

        ssize_t n;

        પ્રતિજ્ઞા(!path || !isempty(path));
        પ્રતિજ્ઞા((fd >= 0) == !path);
        પ્રતિજ્ઞા(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(buf || size == 0);

        જો (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        નહીંતર
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        જો (n < 0)
                પાછા -errno;

        પ્રતિજ્ઞા(size == 0 || (size_t) n <= size);
        પાછા n;
}

પૂર્ણાંક getxattr_at_malloc(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                અચલ અક્ષર *name,
                પૂર્ણાંક at_flags,
                અક્ષર **ret,
                size_t *ret_size) {

        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;
        bool by_procfs;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);
        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        પ્રતિજ્ઞા(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        જો (r < 0)
                પાછા r;

        size_t l = 100;
        માટે (unsigned n_attempts = 7;;) {
                _cleanup_free_ અક્ષર *v = ખાલી;

                જો (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        પાછા -EBUSY;
                n_attempts--;

                v = new(અક્ષર, l+1);
                જો (!v)
                        પાછા -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                જો (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        જો (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                પાછા -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        જો (ret_size)
                                *ret_size = (size_t) n;

                        પાછા 0;
                }
                જો (n != -ERANGE)
                        પાછા (પૂર્ણાંક) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, ખાલી, 0);
                જો (n < 0)
                        પાછા (પૂર્ણાંક) n;

                l = (size_t) n;
        }
}

પૂર્ણાંક getxattr_at_bool(પૂર્ણાંક fd, અચલ અક્ષર *path, અચલ અક્ષર *name, પૂર્ણાંક at_flags) {
        _cleanup_free_ અક્ષર *v = ખાલી;
        પૂર્ણાંક r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ ખાલી);
        જો (r < 0)
                પાછા r;

        પાછા parse_boolean(v);
}

પૂર્ણાંક getxattr_at_strv(પૂર્ણાંક fd, અચલ અક્ષર *path, અચલ અક્ષર *name, પૂર્ણાંક at_flags, અક્ષર ***ret_strv) {
        _cleanup_free_ અક્ષર *nulstr = ખાલી;
        size_t nulstr_size;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        જો (r < 0)
                પાછા r;

        _cleanup_strv_free_ અક્ષર **l = strv_parse_nulstr(nulstr, nulstr_size);
        જો (!l)
                પાછા -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        પાછા 0;
}

સ્થિર પૂર્ણાંક listxattr_pinned_internal(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક at_flags,
                bool by_procfs,
                અક્ષર *buf,
                size_t size) {

        ssize_t n;

        પ્રતિજ્ઞા(!path || !isempty(path));
        પ્રતિજ્ઞા((fd >= 0) == !path);
        પ્રતિજ્ઞા(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        પ્રતિજ્ઞા(buf || size == 0);

        જો (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        નહીંતર
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        જો (n < 0)
                પાછા -errno;

        પ્રતિજ્ઞા(size == 0 || (size_t) n <= size);

        જો (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                પાછા -E2BIG;

        પાછા (પૂર્ણાંક) n;
}

પૂર્ણાંક listxattr_at_malloc(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, અક્ષર **ret) {
        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;
        bool by_procfs;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);
        પ્રતિજ્ઞા((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        પ્રતિજ્ઞા(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        જો (r < 0)
                પાછા r;

        size_t l = 100;
        માટે (unsigned n_attempts = 7;;) {
                _cleanup_free_ અક્ષર *v = ખાલી;

                જો (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        પાછા -EBUSY;
                n_attempts--;

                v = new(અક્ષર, l+1);
                જો (!v)
                        પાછા -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                જો (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        પાછા r;
                }
                જો (r != -ERANGE)
                        પાછા r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, ખાલી, 0);
                જો (r < 0)
                        પાછા r;

                l = (size_t) r;
        }
}

પૂર્ણાંક xsetxattr_full(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક at_flags,
                અચલ અક્ષર *name,
                અચલ અક્ષર *કિંમત,
                size_t size,
                પૂર્ણાંક xattr_flags) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);
        પ્રતિજ્ઞા((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(કિંમત || size == 0);

        જો (size == SIZE_MAX)
                size = strlen(કિંમત);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ અક્ષર *old_કિંમત = ખાલી;
        size_t old_size;

        જો (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_કિંમત, &old_size) >= 0 &&
            memcmp_nn(old_કિંમત, old_size, કિંમત, size) == 0)
                પાછા 0;

        જો (have_xattrat && !isempty(path)) {
                સંરચના xattr_args args = {
                        .કિંમત = PTR_TO_UINT64(કિંમત),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, કદ(args)));
                જો (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        પાછા r;

                have_xattrat = false;
        }

        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        જો (r < 0)
                પાછા r;

        જો (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, કિંમત, size, xattr_flags)
                                                           : lsetxattr(path, name, કિંમત, size, xattr_flags);
        નહીંતર
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, કિંમત, size, xattr_flags)
                              : fsetxattr(fd, name, કિંમત, size, xattr_flags);
        જો (r < 0)
                પાછા -errno;

        પાછા 0;
}

પૂર્ણાંક xsetxattr_strv(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, અચલ અક્ષર *name, અક્ષર * અચલ *l) {
        _cleanup_free_ અક્ષર *nulstr = ખાલી;
        size_t size;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        જો (r < 0)
                પાછા r;

        પાછા xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

પૂર્ણાંક xremovexattr(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, અચલ અક્ષર *name) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);
        પ્રતિજ્ઞા((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        પ્રતિજ્ઞા(name);

        જો (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                જો (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        પાછા r;

                have_xattrat = false;
        }

        _cleanup_close_ પૂર્ણાંક tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        જો (r < 0)
                પાછા r;

        જો (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        નહીંતર
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        જો (r < 0)
                પાછા -errno;

        પાછા 0;
}

સ્થિર પૂર્ણાંક parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        પ્રતિજ્ઞા(ret);

        assert_cc(કદ(usec_t) == કદ(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        જો (!timestamp_is_set(u))
                પાછા -EIO;

        *ret = u;
        પાછા 0;
}

પૂર્ણાંક getcrtime_at(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ખાલી;
        સંરચના statx sx;
        usec_t a, b;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);
        પ્રતિજ્ઞા((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        જો (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        નહીંતર
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (અક્ષર**) &le, &le_size);
        જો (r >= 0) {
                જો (le_size != કદ(*le))
                        r = -EIO;
                નહીંતર
                        r = parse_crtime(*le, &b);
        }
        જો (r < 0) {
                જો (a != USEC_INFINITY) {
                        જો (ret)
                                *ret = a;
                        પાછા 0;
                }

                પાછા r;
        }

        જો (ret)
                *ret = MIN(a, b);
        પાછા 0;
}

પૂર્ણાંક fd_setcrtime(પૂર્ણાંક fd, usec_t usec) {
        le64_t le;

        પ્રતિજ્ઞા(fd >= 0);

        જો (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        પાછા xsetxattr_full(fd, /* path= */ ખાલી, AT_EMPTY_PATH,
                              "user.crtime_usec", (અચલ અક્ષર*) &le, કદ(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(અચલ અક્ષર *name) {
        પાછા STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(અચલ અક્ષર *name) {
        પાછા streq(ASSERT_PTR(name), "security.selinux");
}
