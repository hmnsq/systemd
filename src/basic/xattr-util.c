/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/xattr.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "nulstr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sparse-endian.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
ਸਥਿਰ thread_local bool have_xattrat = true;

ਸਥਿਰ ਪੂਰਨ normalize_and_maybe_pin_inode(
                ਪੂਰਨ *fd,
                ਅਚਲ ਅੱਖਰ **ਰਸਤਾ,
                ਪੂਰਨ *at_flags,
                ਪੂਰਨ *ret_tfd,
                bool *ret_opath) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd);
        ਪ੍ਰਤਿਜ੍ਞਾ(*fd >= 0 || *fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(at_flags);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_tfd);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_opath);

        *ਰਸਤਾ = ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_null(*ਰਸਤਾ); /* Normalize "" to NULL */

        ਜੇ (*fd == AT_FDCWD) {
                ਜੇ (!*ਰਸਤਾ) /* Both unspecified? Then operate on current working directory */
                        *ਰਸਤਾ = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ਵਾਪਸ 0;
        }

        *at_flags |= AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ;

        ਜੇ (!*ਰਸਤਾ) {
                r = fd_is_opath(*fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ਵਾਪਸ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ਪੂਰਨ tfd = openat(*fd, *ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC|(FLAGS_ਸੈੱਟ(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ਜੇ (tfd < 0)
                ਵਾਪਸ -errno;

        *fd = *ret_tfd = tfd;
        *ਰਸਤਾ = ਨਿਰਾ;
        *ret_opath = true;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ssize_t getxattr_pinned_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ at_flags,
                bool by_procfs,
                ਅਚਲ ਅੱਖਰ *name,
                ਅੱਖਰ *buf,
                size_t ਮਾਪ) {

        ssize_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(!ਰਸਤਾ || !isempty(ਰਸਤਾ));
        ਪ੍ਰਤਿਜ੍ਞਾ((fd >= 0) == !ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ || FLAGS_ਸੈੱਟ(at_flags, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ));
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf || ਮਾਪ == 0);

        ਜੇ (ਰਸਤਾ)
                n = FLAGS_ਸੈੱਟ(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(ਰਸਤਾ, name, buf, ਮਾਪ)
                                                           : lgetxattr(ਰਸਤਾ, name, buf, ਮਾਪ);
        ਨਹੀਂਤਾਂ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_ਰਸਤਾ(fd), name, buf, ਮਾਪ)
                              : fgetxattr(fd, name, buf, ਮਾਪ);
        ਜੇ (n < 0)
                ਵਾਪਸ -errno;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ == 0 || (size_t) n <= ਮਾਪ);
        ਵਾਪਸ n;
}

ਪੂਰਨ getxattr_at_malloc(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *name,
                ਪੂਰਨ at_flags,
                ਅੱਖਰ **ret,
                size_t *ret_ਮਾਪ) {

        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;
        bool by_procfs;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ)) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &ਰਸਤਾ, &at_flags, &opened_fd, &by_procfs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        size_t l = 100;
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n_attempts = 7;;) {
                _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;

                ਜੇ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ਵਾਪਸ -EBUSY;
                n_attempts--;

                v = new(ਅੱਖਰ, l+1);
                ਜੇ (!v)
                        ਵਾਪਸ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_ਅੰਦਰੂਨੀ(fd, ਰਸਤਾ, at_flags, by_procfs, name, v, l);
                ਜੇ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ਜੇ (!ret_ਮਾਪ && n > 1 && memchr(v, 0, n - 1))
                                ਵਾਪਸ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ਜੇ (ret_ਮਾਪ)
                                *ret_ਮਾਪ = (size_t) n;

                        ਵਾਪਸ 0;
                }
                ਜੇ (n != -ERANGE)
                        ਵਾਪਸ (ਪੂਰਨ) n;

                n = getxattr_pinned_ਅੰਦਰੂਨੀ(fd, ਰਸਤਾ, at_flags, by_procfs, name, ਨਿਰਾ, 0);
                ਜੇ (n < 0)
                        ਵਾਪਸ (ਪੂਰਨ) n;

                l = (size_t) n;
        }
}

ਪੂਰਨ getxattr_at_bool(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ at_flags) {
        _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = getxattr_at_malloc(fd, ਰਸਤਾ, name, at_flags, &v, /* ret_size= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ parse_boolean(v);
}

ਪੂਰਨ getxattr_at_strv(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ at_flags, ਅੱਖਰ ***ret_strv) {
        _cleanup_free_ ਅੱਖਰ *nulstr = ਨਿਰਾ;
        size_t nulstr_ਮਾਪ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_strv);

        r = getxattr_at_malloc(fd, ਰਸਤਾ, name, at_flags, &nulstr, &nulstr_ਮਾਪ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_strv_free_ ਅੱਖਰ **l = strv_parse_nulstr(nulstr, nulstr_ਮਾਪ);
        ਜੇ (!l)
                ਵਾਪਸ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ listxattr_pinned_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ at_flags,
                bool by_procfs,
                ਅੱਖਰ *buf,
                size_t ਮਾਪ) {

        ssize_t n;

        ਪ੍ਰਤਿਜ੍ਞਾ(!ਰਸਤਾ || !isempty(ਰਸਤਾ));
        ਪ੍ਰਤਿਜ੍ਞਾ((fd >= 0) == !ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ || FLAGS_ਸੈੱਟ(at_flags, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ));
        ਪ੍ਰਤਿਜ੍ਞਾ(buf || ਮਾਪ == 0);

        ਜੇ (ਰਸਤਾ)
                n = FLAGS_ਸੈੱਟ(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(ਰਸਤਾ, buf, ਮਾਪ)
                                                           : llistxattr(ਰਸਤਾ, buf, ਮਾਪ);
        ਨਹੀਂਤਾਂ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_ਰਸਤਾ(fd), buf, ਮਾਪ)
                              : flistxattr(fd, buf, ਮਾਪ);
        ਜੇ (n < 0)
                ਵਾਪਸ -errno;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ == 0 || (size_t) n <= ਮਾਪ);

        ਜੇ (n > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ) /* We couldn't return this as 'int' anymore */
                ਵਾਪਸ -E2BIG;

        ਵਾਪਸ (ਪੂਰਨ) n;
}

ਪੂਰਨ listxattr_at_malloc(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, ਅੱਖਰ **ret) {
        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;
        bool by_procfs;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ)) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &ਰਸਤਾ, &at_flags, &opened_fd, &by_procfs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        size_t l = 100;
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ n_attempts = 7;;) {
                _cleanup_free_ ਅੱਖਰ *v = ਨਿਰਾ;

                ਜੇ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ਵਾਪਸ -EBUSY;
                n_attempts--;

                v = new(ਅੱਖਰ, l+1);
                ਜੇ (!v)
                        ਵਾਪਸ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_ਅੰਦਰੂਨੀ(fd, ਰਸਤਾ, at_flags, by_procfs, v, l);
                ਜੇ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ਵਾਪਸ r;
                }
                ਜੇ (r != -ERANGE)
                        ਵਾਪਸ r;

                r = listxattr_pinned_ਅੰਦਰੂਨੀ(fd, ਰਸਤਾ, at_flags, by_procfs, ਨਿਰਾ, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                l = (size_t) r;
        }
}

ਪੂਰਨ xsetxattr_ਭਰਿਆ(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ at_flags,
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                size_t ਮਾਪ,
                ਪੂਰਨ xattr_flags) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ)) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ || ਮਾਪ == 0);

        ਜੇ (ਮਾਪ == SIZE_MAX)
                ਮਾਪ = ਲੜੀ_ਲੰਬਾਈ(ਮੁੱਲ);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ ਅੱਖਰ *old_ਮੁੱਲ = ਨਿਰਾ;
        size_t old_ਮਾਪ;

        ਜੇ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, ਰਸਤਾ, name, at_flags, &old_ਮੁੱਲ, &old_ਮਾਪ) >= 0 &&
            memcmp_nn(old_ਮੁੱਲ, old_ਮਾਪ, ਮੁੱਲ, ਮਾਪ) == 0)
                ਵਾਪਸ 0;

        ਜੇ (have_xattrat && !isempty(ਰਸਤਾ)) {
                ਬਣਤਰ xattr_ਦਲੀਲਾਂ ਦਲੀਲਾਂ = {
                        .ਮੁੱਲ = PTR_ਮੰਜ਼ਿਲ_UINT64(ਮੁੱਲ),
                        .ਮਾਪ = ਮਾਪ,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, ਰਸਤਾ,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &ਦਲੀਲਾਂ, ਆਕਾਰ(ਦਲੀਲਾਂ)));
                ਜੇ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ਵਾਪਸ r;

                have_xattrat = false;
        }

        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &ਰਸਤਾ, &at_flags, &opened_fd, &by_procfs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਰਸਤਾ)
                r = FLAGS_ਸੈੱਟ(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(ਰਸਤਾ, name, ਮੁੱਲ, ਮਾਪ, xattr_flags)
                                                           : lsetxattr(ਰਸਤਾ, name, ਮੁੱਲ, ਮਾਪ, xattr_flags);
        ਨਹੀਂਤਾਂ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_ਰਸਤਾ(fd), name, ਮੁੱਲ, ਮਾਪ, xattr_flags)
                              : fsetxattr(fd, name, ਮੁੱਲ, ਮਾਪ, xattr_flags);
        ਜੇ (r < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

ਪੂਰਨ xsetxattr_strv(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ * ਅਚਲ *l) {
        _cleanup_free_ ਅੱਖਰ *nulstr = ਨਿਰਾ;
        size_t ਮਾਪ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        r = strv_make_nulstr(l, &nulstr, &ਮਾਪ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ xsetxattr_ਭਰਿਆ(fd, ਰਸਤਾ, at_flags, name, nulstr, ਮਾਪ, /* xattr_flags= */ 0);
}

ਪੂਰਨ xremovexattr(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, ਅਚਲ ਅੱਖਰ *name) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ)) == 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        ਜੇ (have_xattrat && !isempty(ਰਸਤਾ)) {
                r = RET_NERRNO(removexattrat(fd, ਰਸਤਾ,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ਜੇ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ਵਾਪਸ r;

                have_xattrat = false;
        }

        _cleanup_close_ ਪੂਰਨ tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &ਰਸਤਾ, &at_flags, &tfd, &by_procfs);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ਰਸਤਾ)
                r = FLAGS_ਸੈੱਟ(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(ਰਸਤਾ, name)
                                                           : lremovexattr(ਰਸਤਾ, name);
        ਨਹੀਂਤਾਂ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_ਰਸਤਾ(fd), name)
                              : fremovexattr(fd, name);
        ਜੇ (r < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        assert_cc(ਆਕਾਰ(usec_t) == ਆਕਾਰ(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ਜੇ (!timestamp_is_ਸੈੱਟ(u))
                ਵਾਪਸ -EIO;

        *ret = u;
        ਵਾਪਸ 0;
}

ਪੂਰਨ getcrtime_at(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ਨਿਰਾ;
        ਬਣਤਰ statx sx;
        usec_t a, b;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_ਭਰਿਆ(fd,
                        ਰਸਤਾ,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ਜੇ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_ਲੱਦੋ(&sx.stx_btime);
        ਨਹੀਂਤਾਂ
                a = USEC_INFINITY;

        size_t le_ਮਾਪ;
        r = getxattr_at_malloc(fd, ਰਸਤਾ, "user.crtime_usec", at_flags, (ਅੱਖਰ**) &le, &le_ਮਾਪ);
        ਜੇ (r >= 0) {
                ਜੇ (le_ਮਾਪ != ਆਕਾਰ(*le))
                        r = -EIO;
                ਨਹੀਂਤਾਂ
                        r = parse_crtime(*le, &b);
        }
        ਜੇ (r < 0) {
                ਜੇ (a != USEC_INFINITY) {
                        ਜੇ (ret)
                                *ret = a;
                        ਵਾਪਸ 0;
                }

                ਵਾਪਸ r;
        }

        ਜੇ (ret)
                *ret = MIN(a, b);
        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_setcrtime(ਪੂਰਨ fd, usec_t usec) {
        le64_t le;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (!timestamp_is_ਸੈੱਟ(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ਵਾਪਸ xsetxattr_ਭਰਿਆ(fd, /* path= */ ਨਿਰਾ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ,
                              "user.crtime_usec", (ਅਚਲ ਅੱਖਰ*) &le, ਆਕਾਰ(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ਅਚਲ ਅੱਖਰ *name) {
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ਅਚਲ ਅੱਖਰ *name) {
        ਵਾਪਸ streq(ASSERT_PTR(name), "security.selinux");
}
