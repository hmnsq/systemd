/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sys/xattr.h>
#अन्तर्भाव <threads.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "nulstr-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "sparse-endian.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
स्थिर thread_local bool have_xattrat = true;

स्थिर पूर्णाङ्क normalize_and_maybe_pin_inode(
                पूर्णाङ्क *fd,
                ध्रुव वर्ण **path,
                पूर्णाङ्क *at_flags,
                पूर्णाङ्क *ret_tfd,
                bool *ret_opath) {

        पूर्णाङ्क r;

        प्रतिज्ञा(fd);
        प्रतिज्ञा(*fd >= 0 || *fd == AT_FDCWD);
        प्रतिज्ञा(path);
        प्रतिज्ञा(at_flags);
        प्रतिज्ञा(ret_tfd);
        प्रतिज्ञा(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        सचे (*fd == AT_FDCWD) {
                सचे (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                पटिनिवत्त 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        सचे (!*path) {
                r = fd_is_opath(*fd);
                सचे (r < 0)
                        पटिनिवत्त r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                पटिनिवत्त 0;
        }

        /* If both have been specified, then we go via O_PATH */

        पूर्णाङ्क tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        सचे (tfd < 0)
                पटिनिवत्त -errno;

        *fd = *ret_tfd = tfd;
        *path = रिक्त;
        *ret_opath = true;

        पटिनिवत्त 0;
}

स्थिर ssize_t getxattr_pinned_internal(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                bool by_procfs,
                ध्रुव वर्ण *name,
                वर्ण *buf,
                size_t size) {

        ssize_t n;

        प्रतिज्ञा(!path || !isempty(path));
        प्रतिज्ञा((fd >= 0) == !path);
        प्रतिज्ञा(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        प्रतिज्ञा(name);
        प्रतिज्ञा(buf || size == 0);

        सचे (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        अञ्ञथा
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        सचे (n < 0)
                पटिनिवत्त -errno;

        प्रतिज्ञा(size == 0 || (size_t) n <= size);
        पटिनिवत्त n;
}

पूर्णाङ्क getxattr_at_malloc(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *name,
                पूर्णाङ्क at_flags,
                वर्ण **ret,
                size_t *ret_size) {

        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा(name);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        सचे (r < 0)
                पटिनिवत्त r;

        size_t l = 100;
        अत्थाय (unsigned n_attempts = 7;;) {
                _cleanup_free_ वर्ण *v = रिक्त;

                सचे (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        पटिनिवत्त -EBUSY;
                n_attempts--;

                v = new(वर्ण, l+1);
                सचे (!v)
                        पटिनिवत्त -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                सचे (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        सचे (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                पटिनिवत्त -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        सचे (ret_size)
                                *ret_size = (size_t) n;

                        पटिनिवत्त 0;
                }
                सचे (n != -ERANGE)
                        पटिनिवत्त (पूर्णाङ्क) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, रिक्त, 0);
                सचे (n < 0)
                        पटिनिवत्त (पूर्णाङ्क) n;

                l = (size_t) n;
        }
}

पूर्णाङ्क getxattr_at_bool(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त parse_boolean(v);
}

पूर्णाङ्क getxattr_at_strv(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags, वर्ण ***ret_strv) {
        _cleanup_free_ वर्ण *nulstr = रिक्त;
        size_t nulstr_size;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        सचे (r < 0)
                पटिनिवत्त r;

        _cleanup_strv_free_ वर्ण **l = strv_parse_nulstr(nulstr, nulstr_size);
        सचे (!l)
                पटिनिवत्त -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क listxattr_pinned_internal(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                bool by_procfs,
                वर्ण *buf,
                size_t size) {

        ssize_t n;

        प्रतिज्ञा(!path || !isempty(path));
        प्रतिज्ञा((fd >= 0) == !path);
        प्रतिज्ञा(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        प्रतिज्ञा(buf || size == 0);

        सचे (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        अञ्ञथा
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        सचे (n < 0)
                पटिनिवत्त -errno;

        प्रतिज्ञा(size == 0 || (size_t) n <= size);

        सचे (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                पटिनिवत्त -E2BIG;

        पटिनिवत्त (पूर्णाङ्क) n;
}

पूर्णाङ्क listxattr_at_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, वर्ण **ret) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        सचे (r < 0)
                पटिनिवत्त r;

        size_t l = 100;
        अत्थाय (unsigned n_attempts = 7;;) {
                _cleanup_free_ वर्ण *v = रिक्त;

                सचे (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        पटिनिवत्त -EBUSY;
                n_attempts--;

                v = new(वर्ण, l+1);
                सचे (!v)
                        पटिनिवत्त -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                सचे (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        पटिनिवत्त r;
                }
                सचे (r != -ERANGE)
                        पटिनिवत्त r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, रिक्त, 0);
                सचे (r < 0)
                        पटिनिवत्त r;

                l = (size_t) r;
        }
}

पूर्णाङ्क xsetxattr_full(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मानम्,
                size_t size,
                पूर्णाङ्क xattr_flags) {

        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(name);
        प्रतिज्ञा(मानम् || size == 0);

        सचे (size == SIZE_MAX)
                size = strlen(मानम्);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ वर्ण *old_मानम् = रिक्त;
        size_t old_size;

        सचे (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_मानम्, &old_size) >= 0 &&
            memcmp_nn(old_मानम्, old_size, मानम्, size) == 0)
                पटिनिवत्त 0;

        सचे (have_xattrat && !isempty(path)) {
                संरचना xattr_args args = {
                        .मानम् = PTR_TO_UINT64(मानम्),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, परिमाण(args)));
                सचे (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        पटिनिवत्त r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, मानम्, size, xattr_flags)
                                                           : lsetxattr(path, name, मानम्, size, xattr_flags);
        अञ्ञथा
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, मानम्, size, xattr_flags)
                              : fsetxattr(fd, name, मानम्, size, xattr_flags);
        सचे (r < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त 0;
}

पूर्णाङ्क xsetxattr_strv(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, ध्रुव वर्ण *name, वर्ण * ध्रुव *l) {
        _cleanup_free_ वर्ण *nulstr = रिक्त;
        size_t size;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

पूर्णाङ्क xremovexattr(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, ध्रुव वर्ण *name) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(name);

        सचे (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                सचे (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        पटिनिवत्त r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णाङ्क tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        अञ्ञथा
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        सचे (r < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        प्रतिज्ञा(ret);

        assert_cc(परिमाण(usec_t) == परिमाण(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        सचे (!timestamp_is_set(u))
                पटिनिवत्त -EIO;

        *ret = u;
        पटिनिवत्त 0;
}

पूर्णाङ्क getcrtime_at(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = रिक्त;
        संरचना statx sx;
        usec_t a, b;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        सचे (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        अञ्ञथा
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (वर्ण**) &le, &le_size);
        सचे (r >= 0) {
                सचे (le_size != परिमाण(*le))
                        r = -EIO;
                अञ्ञथा
                        r = parse_crtime(*le, &b);
        }
        सचे (r < 0) {
                सचे (a != USEC_INFINITY) {
                        सचे (ret)
                                *ret = a;
                        पटिनिवत्त 0;
                }

                पटिनिवत्त r;
        }

        सचे (ret)
                *ret = MIN(a, b);
        पटिनिवत्त 0;
}

पूर्णाङ्क fd_setcrtime(पूर्णाङ्क fd, usec_t usec) {
        le64_t le;

        प्रतिज्ञा(fd >= 0);

        सचे (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        पटिनिवत्त xsetxattr_full(fd, /* path= */ रिक्त, AT_EMPTY_PATH,
                              "user.crtime_usec", (ध्रुव वर्ण*) &le, परिमाण(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ध्रुव वर्ण *name) {
        पटिनिवत्त STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ध्रुव वर्ण *name) {
        पटिनिवत्त streq(ASSERT_PTR(name), "security.selinux");
}
