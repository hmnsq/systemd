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

        यदि (*fd == AT_FDCWD) {
                यदि (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                प्रत्यागम 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        यदि (!*path) {
                r = fd_is_opath(*fd);
                यदि (r < 0)
                        प्रत्यागम r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                प्रत्यागम 0;
        }

        /* If both have been specified, then we go via O_PATH */

        पूर्णाङ्क tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        यदि (tfd < 0)
                प्रत्यागम -त्रुटि_सं;

        *fd = *ret_tfd = tfd;
        *path = रिक्त;
        *ret_opath = true;

        प्रत्यागम 0;
}

स्थिर ssize_t getxattr_pinned_internal(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                bool by_procfs,
                ध्रुव वर्ण *name,
                वर्ण *buf,
                माप_प्रकार size) {

        ssize_t n;

        प्रतिज्ञा(!path || !isempty(path));
        प्रतिज्ञा((fd >= 0) == !path);
        प्रतिज्ञा(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        प्रतिज्ञा(name);
        प्रतिज्ञा(buf || size == 0);

        यदि (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        अन्यथा
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        यदि (n < 0)
                प्रत्यागम -त्रुटि_सं;

        प्रतिज्ञा(size == 0 || (माप_प्रकार) n <= size);
        प्रत्यागम n;
}

पूर्णाङ्क getxattr_at_malloc(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *name,
                पूर्णाङ्क at_flags,
                वर्ण **ret,
                माप_प्रकार *ret_size) {

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
        यदि (r < 0)
                प्रत्यागम r;

        माप_प्रकार l = 100;
        प्रति (अचिन्हित n_attempts = 7;;) {
                _cleanup_free_ वर्ण *v = रिक्त;

                यदि (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        प्रत्यागम -EBUSY;
                n_attempts--;

                v = new(वर्ण, l+1);
                यदि (!v)
                        प्रत्यागम -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                यदि (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        यदि (!ret_size && n > 1 && स_प्रथम(v, 0, n - 1))
                                प्रत्यागम -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        यदि (ret_size)
                                *ret_size = (माप_प्रकार) n;

                        प्रत्यागम 0;
                }
                यदि (n != -दुस्फल)
                        प्रत्यागम (पूर्णाङ्क) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, रिक्त, 0);
                यदि (n < 0)
                        प्रत्यागम (पूर्णाङ्क) n;

                l = (माप_प्रकार) n;
        }
}

पूर्णाङ्क getxattr_at_bool(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम parse_boolean(v);
}

पूर्णाङ्क getxattr_at_strv(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags, वर्ण ***ret_strv) {
        _cleanup_free_ वर्ण *nulstr = रिक्त;
        माप_प्रकार nulstr_size;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_strv_free_ वर्ण **l = strv_parse_nulstr(nulstr, nulstr_size);
        यदि (!l)
                प्रत्यागम -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क listxattr_pinned_internal(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                bool by_procfs,
                वर्ण *buf,
                माप_प्रकार size) {

        ssize_t n;

        प्रतिज्ञा(!path || !isempty(path));
        प्रतिज्ञा((fd >= 0) == !path);
        प्रतिज्ञा(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        प्रतिज्ञा(buf || size == 0);

        यदि (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        अन्यथा
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        यदि (n < 0)
                प्रत्यागम -त्रुटि_सं;

        प्रतिज्ञा(size == 0 || (माप_प्रकार) n <= size);

        यदि (n > पूर्णांक_उच्च) /* We couldn't return this as 'int' anymore */
                प्रत्यागम -E2BIG;

        प्रत्यागम (पूर्णाङ्क) n;
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
        यदि (r < 0)
                प्रत्यागम r;

        माप_प्रकार l = 100;
        प्रति (अचिन्हित n_attempts = 7;;) {
                _cleanup_free_ वर्ण *v = रिक्त;

                यदि (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        प्रत्यागम -EBUSY;
                n_attempts--;

                v = new(वर्ण, l+1);
                यदि (!v)
                        प्रत्यागम -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                यदि (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        प्रत्यागम r;
                }
                यदि (r != -दुस्फल)
                        प्रत्यागम r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, रिक्त, 0);
                यदि (r < 0)
                        प्रत्यागम r;

                l = (माप_प्रकार) r;
        }
}

पूर्णाङ्क xsetxattr_full(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मूल्य,
                माप_प्रकार size,
                पूर्णाङ्क xattr_flags) {

        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(name);
        प्रतिज्ञा(मूल्य || size == 0);

        यदि (size == SIZE_MAX)
                size = म_माप(मूल्य);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ वर्ण *old_मूल्य = रिक्त;
        माप_प्रकार old_size;

        यदि (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_मूल्य, &old_size) >= 0 &&
            memcmp_nn(old_मूल्य, old_size, मूल्य, size) == 0)
                प्रत्यागम 0;

        यदि (have_xattrat && !isempty(path)) {
                संरचना xattr_args args = {
                        .मूल्य = PTR_TO_UINT64(मूल्य),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, परिमाण(args)));
                यदि (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        प्रत्यागम r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, मूल्य, size, xattr_flags)
                                                           : lsetxattr(path, name, मूल्य, size, xattr_flags);
        अन्यथा
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, मूल्य, size, xattr_flags)
                              : fsetxattr(fd, name, मूल्य, size, xattr_flags);
        यदि (r < 0)
                प्रत्यागम -त्रुटि_सं;

        प्रत्यागम 0;
}

पूर्णाङ्क xsetxattr_strv(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, ध्रुव वर्ण *name, वर्ण * ध्रुव *l) {
        _cleanup_free_ वर्ण *nulstr = रिक्त;
        माप_प्रकार size;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

पूर्णाङ्क xremovexattr(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, ध्रुव वर्ण *name) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(name);

        यदि (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                यदि (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        प्रत्यागम r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णाङ्क tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        अन्यथा
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        यदि (r < 0)
                प्रत्यागम -त्रुटि_सं;

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        प्रतिज्ञा(ret);

        assert_cc(परिमाण(usec_t) == परिमाण(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        यदि (!timestamp_is_set(u))
                प्रत्यागम -EIO;

        *ret = u;
        प्रत्यागम 0;
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
        यदि (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        अन्यथा
                a = USEC_INFINITY;

        माप_प्रकार le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (वर्ण**) &le, &le_size);
        यदि (r >= 0) {
                यदि (le_size != परिमाण(*le))
                        r = -EIO;
                अन्यथा
                        r = parse_crtime(*le, &b);
        }
        यदि (r < 0) {
                यदि (a != USEC_INFINITY) {
                        यदि (ret)
                                *ret = a;
                        प्रत्यागम 0;
                }

                प्रत्यागम r;
        }

        यदि (ret)
                *ret = MIN(a, b);
        प्रत्यागम 0;
}

पूर्णाङ्क fd_setcrtime(पूर्णाङ्क fd, usec_t usec) {
        le64_t le;

        प्रतिज्ञा(fd >= 0);

        यदि (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        प्रत्यागम xsetxattr_full(fd, /* path= */ रिक्त, AT_EMPTY_PATH,
                              "user.crtime_usec", (ध्रुव वर्ण*) &le, परिमाण(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ध्रुव वर्ण *name) {
        प्रत्यागम STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ध्रुव वर्ण *name) {
        प्रत्यागम streq(ASSERT_PTR(name), "security.selinux");
}
