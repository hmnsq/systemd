/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <sys/xattr.h>
#അന്തര്ഭാവ <threads.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "nulstr-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "sparse-endian.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
സ്ഥിതം thread_local bool have_xattrat = true;

സ്ഥിതം പൂർണ്ണസംഖ്യ normalize_and_maybe_pin_inode(
                പൂർണ്ണസംഖ്യ *fd,
                സ്ഥിരം അക്ഷരം **path,
                പൂർണ്ണസംഖ്യ *at_flags,
                പൂർണ്ണസംഖ്യ *ret_tfd,
                bool *ret_opath) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd);
        പ്രതിജ്ഞാ(*fd >= 0 || *fd == AT_FDCWD);
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(at_flags);
        പ്രതിജ്ഞാ(ret_tfd);
        പ്രതിജ്ഞാ(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        എങ്കിൽ (*fd == AT_FDCWD) {
                എങ്കിൽ (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                തിരികെ 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        എങ്കിൽ (!*path) {
                r = fd_is_opath(*fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                തിരികെ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        പൂർണ്ണസംഖ്യ tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        എങ്കിൽ (tfd < 0)
                തിരികെ -errno;

        *fd = *ret_tfd = tfd;
        *path = ശൂന്യ;
        *ret_opath = true;

        തിരികെ 0;
}

സ്ഥിതം ssize_t getxattr_pinned_internal(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ at_flags,
                bool by_procfs,
                സ്ഥിരം അക്ഷരം *name,
                അക്ഷരം *buf,
                size_t size) {

        ssize_t n;

        പ്രതിജ്ഞാ(!path || !isempty(path));
        പ്രതിജ്ഞാ((fd >= 0) == !path);
        പ്രതിജ്ഞാ(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(buf || size == 0);

        എങ്കിൽ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        അല്ലെങ്കിൽ
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        എങ്കിൽ (n < 0)
                തിരികെ -errno;

        പ്രതിജ്ഞാ(size == 0 || (size_t) n <= size);
        തിരികെ n;
}

പൂർണ്ണസംഖ്യ getxattr_at_malloc(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *name,
                പൂർണ്ണസംഖ്യ at_flags,
                അക്ഷരം **ret,
                size_t *ret_size) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;
        bool by_procfs;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        പ്രതിജ്ഞാ(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        size_t l = 100;
        വേണ്ടി (unsigned n_attempts = 7;;) {
                _cleanup_free_ അക്ഷരം *v = ശൂന്യ;

                എങ്കിൽ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        തിരികെ -EBUSY;
                n_attempts--;

                v = new(അക്ഷരം, l+1);
                എങ്കിൽ (!v)
                        തിരികെ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                എങ്കിൽ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        എങ്കിൽ (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                തിരികെ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        എങ്കിൽ (ret_size)
                                *ret_size = (size_t) n;

                        തിരികെ 0;
                }
                എങ്കിൽ (n != -ERANGE)
                        തിരികെ (പൂർണ്ണസംഖ്യ) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, ശൂന്യ, 0);
                എങ്കിൽ (n < 0)
                        തിരികെ (പൂർണ്ണസംഖ്യ) n;

                l = (size_t) n;
        }
}

പൂർണ്ണസംഖ്യ getxattr_at_bool(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ at_flags) {
        _cleanup_free_ അക്ഷരം *v = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_boolean(v);
}

പൂർണ്ണസംഖ്യ getxattr_at_strv(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ at_flags, അക്ഷരം ***ret_strv) {
        _cleanup_free_ അക്ഷരം *nulstr = ശൂന്യ;
        size_t nulstr_size;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_strv_free_ അക്ഷരം **l = strv_parse_nulstr(nulstr, nulstr_size);
        എങ്കിൽ (!l)
                തിരികെ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ listxattr_pinned_internal(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ at_flags,
                bool by_procfs,
                അക്ഷരം *buf,
                size_t size) {

        ssize_t n;

        പ്രതിജ്ഞാ(!path || !isempty(path));
        പ്രതിജ്ഞാ((fd >= 0) == !path);
        പ്രതിജ്ഞാ(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        പ്രതിജ്ഞാ(buf || size == 0);

        എങ്കിൽ (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        അല്ലെങ്കിൽ
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        എങ്കിൽ (n < 0)
                തിരികെ -errno;

        പ്രതിജ്ഞാ(size == 0 || (size_t) n <= size);

        എങ്കിൽ (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                തിരികെ -E2BIG;

        തിരികെ (പൂർണ്ണസംഖ്യ) n;
}

പൂർണ്ണസംഖ്യ listxattr_at_malloc(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, അക്ഷരം **ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;
        bool by_procfs;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);
        പ്രതിജ്ഞാ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        പ്രതിജ്ഞാ(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        size_t l = 100;
        വേണ്ടി (unsigned n_attempts = 7;;) {
                _cleanup_free_ അക്ഷരം *v = ശൂന്യ;

                എങ്കിൽ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        തിരികെ -EBUSY;
                n_attempts--;

                v = new(അക്ഷരം, l+1);
                എങ്കിൽ (!v)
                        തിരികെ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                എങ്കിൽ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        തിരികെ r;
                }
                എങ്കിൽ (r != -ERANGE)
                        തിരികെ r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, ശൂന്യ, 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                l = (size_t) r;
        }
}

പൂർണ്ണസംഖ്യ xsetxattr_full(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ at_flags,
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം അക്ഷരം *മൂല്യം,
                size_t size,
                പൂർണ്ണസംഖ്യ xattr_flags) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);
        പ്രതിജ്ഞാ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(മൂല്യം || size == 0);

        എങ്കിൽ (size == SIZE_MAX)
                size = strlen(മൂല്യം);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ അക്ഷരം *old_മൂല്യം = ശൂന്യ;
        size_t old_size;

        എങ്കിൽ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_മൂല്യം, &old_size) >= 0 &&
            memcmp_nn(old_മൂല്യം, old_size, മൂല്യം, size) == 0)
                തിരികെ 0;

        എങ്കിൽ (have_xattrat && !isempty(path)) {
                ഘടന xattr_args args = {
                        .മൂല്യം = PTR_TO_UINT64(മൂല്യം),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, വലുപ്പം(args)));
                എങ്കിൽ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        തിരികെ r;

                have_xattrat = false;
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, മൂല്യം, size, xattr_flags)
                                                           : lsetxattr(path, name, മൂല്യം, size, xattr_flags);
        അല്ലെങ്കിൽ
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, മൂല്യം, size, xattr_flags)
                              : fsetxattr(fd, name, മൂല്യം, size, xattr_flags);
        എങ്കിൽ (r < 0)
                തിരികെ -errno;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ xsetxattr_strv(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, സ്ഥിരം അക്ഷരം *name, അക്ഷരം * സ്ഥിരം *l) {
        _cleanup_free_ അക്ഷരം *nulstr = ശൂന്യ;
        size_t size;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

പൂർണ്ണസംഖ്യ xremovexattr(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, സ്ഥിരം അക്ഷരം *name) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);
        പ്രതിജ്ഞാ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        പ്രതിജ്ഞാ(name);

        എങ്കിൽ (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                എങ്കിൽ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        തിരികെ r;

                have_xattrat = false;
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        അല്ലെങ്കിൽ
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        എങ്കിൽ (r < 0)
                തിരികെ -errno;

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        പ്രതിജ്ഞാ(ret);

        assert_cc(വലുപ്പം(usec_t) == വലുപ്പം(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        എങ്കിൽ (!timestamp_is_set(u))
                തിരികെ -EIO;

        *ret = u;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ getcrtime_at(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ശൂന്യ;
        ഘടന statx sx;
        usec_t a, b;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);
        പ്രതിജ്ഞാ((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        എങ്കിൽ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        അല്ലെങ്കിൽ
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (അക്ഷരം**) &le, &le_size);
        എങ്കിൽ (r >= 0) {
                എങ്കിൽ (le_size != വലുപ്പം(*le))
                        r = -EIO;
                അല്ലെങ്കിൽ
                        r = parse_crtime(*le, &b);
        }
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (a != USEC_INFINITY) {
                        എങ്കിൽ (ret)
                                *ret = a;
                        തിരികെ 0;
                }

                തിരികെ r;
        }

        എങ്കിൽ (ret)
                *ret = MIN(a, b);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_setcrtime(പൂർണ്ണസംഖ്യ fd, usec_t usec) {
        le64_t le;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        തിരികെ xsetxattr_full(fd, /* path= */ ശൂന്യ, AT_EMPTY_PATH,
                              "user.crtime_usec", (സ്ഥിരം അക്ഷരം*) &le, വലുപ്പം(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(സ്ഥിരം അക്ഷരം *name) {
        തിരികെ STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(സ്ഥിരം അക്ഷരം *name) {
        തിരികെ streq(ASSERT_PTR(name), "security.selinux");
}
