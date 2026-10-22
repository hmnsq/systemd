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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ normalize_and_maybe_pin_inode(
                ಪೂರ್ಣಾಂಕ *fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ *at_flags,
                ಪೂರ್ಣಾಂಕ *ret_tfd,
                bool *ret_opath) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd);
        ಪ್ರತಿಜ್ಞಾ(*fd >= 0 || *fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(at_flags);
        ಪ್ರತಿಜ್ಞಾ(ret_tfd);
        ಪ್ರತಿಜ್ಞಾ(ret_opath);

        *ಮಾರ್ಗ = ಖಾಲಿಗಳು_ಗಮ್ಯ_null(*ಮಾರ್ಗ); /* Normalize "" to NULL */

        ಆದರೆ (*fd == AT_FDCWD) {
                ಆದರೆ (!*ಮಾರ್ಗ) /* Both unspecified? Then operate on current working directory */
                        *ಮಾರ್ಗ = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ಹಿಂತಿರುಗಿ 0;
        }

        *at_flags |= AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ;

        ಆದರೆ (!*ಮಾರ್ಗ) {
                r = fd_is_opath(*fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ಹಿಂತಿರುಗಿ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ಪೂರ್ಣಾಂಕ tfd = openat(*fd, *ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC|(FLAGS_ಗಣ(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ಆದರೆ (tfd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        *fd = *ret_tfd = tfd;
        *ಮಾರ್ಗ = ಖಾಲಿ;
        *ret_opath = true;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ssize_t getxattr_pinned_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ at_flags,
                bool by_procfs,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಅಕ್ಷರ *buf,
                size_t ಗಾತ್ರ_2) {

        ssize_t n;

        ಪ್ರತಿಜ್ಞಾ(!ಮಾರ್ಗ || !isempty(ಮಾರ್ಗ));
        ಪ್ರತಿಜ್ಞಾ((fd >= 0) == !ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ || FLAGS_ಗಣ(at_flags, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ));
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(buf || ಗಾತ್ರ_2 == 0);

        ಆದರೆ (ಮಾರ್ಗ)
                n = FLAGS_ಗಣ(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(ಮಾರ್ಗ, name, buf, ಗಾತ್ರ_2)
                                                           : lgetxattr(ಮಾರ್ಗ, name, buf, ಗಾತ್ರ_2);
        ಇಲ್ಲದಿದ್ದರೆ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), name, buf, ಗಾತ್ರ_2)
                              : fgetxattr(fd, name, buf, ಗಾತ್ರ_2);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 == 0 || (size_t) n <= ಗಾತ್ರ_2);
        ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ getxattr_at_malloc(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಪೂರ್ಣಾಂಕ at_flags,
                ಅಕ್ಷರ **ret,
                size_t *ret_ಗಾತ್ರ) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;
        bool by_procfs;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ)) == 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &ಮಾರ್ಗ, &at_flags, &opened_fd, &by_procfs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        size_t l = 100;
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n_attempts = 7;;) {
                _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;

                ಆದರೆ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ಹಿಂತಿರುಗಿ -EBUSY;
                n_attempts--;

                v = new(ಅಕ್ಷರ, l+1);
                ಆದರೆ (!v)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_ಆಂತರಿಕ(fd, ಮಾರ್ಗ, at_flags, by_procfs, name, v, l);
                ಆದರೆ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ಆದರೆ (!ret_ಗಾತ್ರ && n > 1 && memchr(v, 0, n - 1))
                                ಹಿಂತಿರುಗಿ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ಆದರೆ (ret_ಗಾತ್ರ)
                                *ret_ಗಾತ್ರ = (size_t) n;

                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (n != -ERANGE)
                        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;

                n = getxattr_pinned_ಆಂತರಿಕ(fd, ಮಾರ್ಗ, at_flags, by_procfs, name, ಖಾಲಿ, 0);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;

                l = (size_t) n;
        }
}

ಪೂರ್ಣಾಂಕ getxattr_at_bool(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ at_flags) {
        _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        r = getxattr_at_malloc(fd, ಮಾರ್ಗ, name, at_flags, &v, /* ret_size= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ parse_boolean(v);
}

ಪೂರ್ಣಾಂಕ getxattr_at_strv(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ at_flags, ಅಕ್ಷರ ***ret_strv) {
        _cleanup_free_ ಅಕ್ಷರ *nulstr = ಖಾಲಿ;
        size_t nulstr_ಗಾತ್ರ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_strv);

        r = getxattr_at_malloc(fd, ಮಾರ್ಗ, name, at_flags, &nulstr, &nulstr_ಗಾತ್ರ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_strv_free_ ಅಕ್ಷರ **l = strv_parse_nulstr(nulstr, nulstr_ಗಾತ್ರ);
        ಆದರೆ (!l)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ listxattr_pinned_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ at_flags,
                bool by_procfs,
                ಅಕ್ಷರ *buf,
                size_t ಗಾತ್ರ_2) {

        ssize_t n;

        ಪ್ರತಿಜ್ಞಾ(!ಮಾರ್ಗ || !isempty(ಮಾರ್ಗ));
        ಪ್ರತಿಜ್ಞಾ((fd >= 0) == !ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ || FLAGS_ಗಣ(at_flags, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ));
        ಪ್ರತಿಜ್ಞಾ(buf || ಗಾತ್ರ_2 == 0);

        ಆದರೆ (ಮಾರ್ಗ)
                n = FLAGS_ಗಣ(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(ಮಾರ್ಗ, buf, ಗಾತ್ರ_2)
                                                           : llistxattr(ಮಾರ್ಗ, buf, ಗಾತ್ರ_2);
        ಇಲ್ಲದಿದ್ದರೆ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), buf, ಗಾತ್ರ_2)
                              : flistxattr(fd, buf, ಗಾತ್ರ_2);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 == 0 || (size_t) n <= ಗಾತ್ರ_2);

        ಆದರೆ (n > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ) /* We couldn't return this as 'int' anymore */
                ಹಿಂತಿರುಗಿ -E2BIG;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;
}

ಪೂರ್ಣಾಂಕ listxattr_at_malloc(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, ಅಕ್ಷರ **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;
        bool by_procfs;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ)) == 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &ಮಾರ್ಗ, &at_flags, &opened_fd, &by_procfs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        size_t l = 100;
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ n_attempts = 7;;) {
                _cleanup_free_ ಅಕ್ಷರ *v = ಖಾಲಿ;

                ಆದರೆ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ಹಿಂತಿರುಗಿ -EBUSY;
                n_attempts--;

                v = new(ಅಕ್ಷರ, l+1);
                ಆದರೆ (!v)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_ಆಂತರಿಕ(fd, ಮಾರ್ಗ, at_flags, by_procfs, v, l);
                ಆದರೆ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ಹಿಂತಿರುಗಿ r;
                }
                ಆದರೆ (r != -ERANGE)
                        ಹಿಂತಿರುಗಿ r;

                r = listxattr_pinned_ಆಂತರಿಕ(fd, ಮಾರ್ಗ, at_flags, by_procfs, ಖಾಲಿ, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                l = (size_t) r;
        }
}

ಪೂರ್ಣಾಂಕ xsetxattr_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ at_flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                size_t ಗಾತ್ರ_2,
                ಪೂರ್ಣಾಂಕ xattr_flags) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ)) == 0);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ || ಗಾತ್ರ_2 == 0);

        ಆದರೆ (ಗಾತ್ರ_2 == SIZE_MAX)
                ಗಾತ್ರ_2 = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಮೌಲ್ಯ);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ ಅಕ್ಷರ *old_ಮೌಲ್ಯ = ಖಾಲಿ;
        size_t old_ಗಾತ್ರ;

        ಆದರೆ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, ಮಾರ್ಗ, name, at_flags, &old_ಮೌಲ್ಯ, &old_ಗಾತ್ರ) >= 0 &&
            memcmp_nn(old_ಮೌಲ್ಯ, old_ಗಾತ್ರ, ಮೌಲ್ಯ, ಗಾತ್ರ_2) == 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (have_xattrat && !isempty(ಮಾರ್ಗ)) {
                ರಚನೆ xattr_ವಾದಗಳು ವಾದಗಳು = {
                        .ಮೌಲ್ಯ = PTR_ಗಮ್ಯ_UINT64(ಮೌಲ್ಯ),
                        .ಗಾತ್ರ_2 = ಗಾತ್ರ_2,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, ಮಾರ್ಗ,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &ವಾದಗಳು, ಗಾತ್ರ(ವಾದಗಳು)));
                ಆದರೆ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ಹಿಂತಿರುಗಿ r;

                have_xattrat = false;
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &ಮಾರ್ಗ, &at_flags, &opened_fd, &by_procfs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಮಾರ್ಗ)
                r = FLAGS_ಗಣ(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(ಮಾರ್ಗ, name, ಮೌಲ್ಯ, ಗಾತ್ರ_2, xattr_flags)
                                                           : lsetxattr(ಮಾರ್ಗ, name, ಮೌಲ್ಯ, ಗಾತ್ರ_2, xattr_flags);
        ಇಲ್ಲದಿದ್ದರೆ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), name, ಮೌಲ್ಯ, ಗಾತ್ರ_2, xattr_flags)
                              : fsetxattr(fd, name, ಮೌಲ್ಯ, ಗಾತ್ರ_2, xattr_flags);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ xsetxattr_strv(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *l) {
        _cleanup_free_ ಅಕ್ಷರ *nulstr = ಖಾಲಿ;
        size_t ಗಾತ್ರ_2;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(name);

        r = strv_make_nulstr(l, &nulstr, &ಗಾತ್ರ_2);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ xsetxattr_ತುಂಬಿದ(fd, ಮಾರ್ಗ, at_flags, name, nulstr, ಗಾತ್ರ_2, /* xattr_flags= */ 0);
}

ಪೂರ್ಣಾಂಕ xremovexattr(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ at_flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ)) == 0);
        ಪ್ರತಿಜ್ಞಾ(name);

        ಆದರೆ (have_xattrat && !isempty(ಮಾರ್ಗ)) {
                r = RET_NERRNO(removexattrat(fd, ಮಾರ್ಗ,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ಆದರೆ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ಹಿಂತಿರುಗಿ r;

                have_xattrat = false;
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &ಮಾರ್ಗ, &at_flags, &tfd, &by_procfs);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ಮಾರ್ಗ)
                r = FLAGS_ಗಣ(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(ಮಾರ್ಗ, name)
                                                           : lremovexattr(ಮಾರ್ಗ, name);
        ಇಲ್ಲದಿದ್ದರೆ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_ಮಾರ್ಗ(fd), name)
                              : fremovexattr(fd, name);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        ಪ್ರತಿಜ್ಞಾ(ret);

        assert_cc(ಗಾತ್ರ(usec_t) == ಗಾತ್ರ(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ಆದರೆ (!timestamp_is_ಗಣ(u))
                ಹಿಂತಿರುಗಿ -EIO;

        *ret = u;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ getcrtime_at(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ಖಾಲಿ;
        ರಚನೆ statx sx;
        usec_t a, b;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_ತುಂಬಿದ(fd,
                        ಮಾರ್ಗ,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ಆದರೆ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_ತುಂಬು(&sx.stx_btime);
        ಇಲ್ಲದಿದ್ದರೆ
                a = USEC_INFINITY;

        size_t le_ಗಾತ್ರ;
        r = getxattr_at_malloc(fd, ಮಾರ್ಗ, "user.crtime_usec", at_flags, (ಅಕ್ಷರ**) &le, &le_ಗಾತ್ರ);
        ಆದರೆ (r >= 0) {
                ಆದರೆ (le_ಗಾತ್ರ != ಗಾತ್ರ(*le))
                        r = -EIO;
                ಇಲ್ಲದಿದ್ದರೆ
                        r = parse_crtime(*le, &b);
        }
        ಆದರೆ (r < 0) {
                ಆದರೆ (a != USEC_INFINITY) {
                        ಆದರೆ (ret)
                                *ret = a;
                        ಹಿಂತಿರುಗಿ 0;
                }

                ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret)
                *ret = MIN(a, b);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_setcrtime(ಪೂರ್ಣಾಂಕ fd, usec_t usec) {
        le64_t le;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (!timestamp_is_ಗಣ(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ಹಿಂತಿರುಗಿ xsetxattr_ತುಂಬಿದ(fd, /* path= */ ಖಾಲಿ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ,
                              "user.crtime_usec", (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*) &le, ಗಾತ್ರ(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಹಿಂತಿರುಗಿ streq(ASSERT_PTR(name), "security.selinux");
}
