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

स्थिर पूर्णांक normalize_and_maybe_pin_inode(
                पूर्णांक *fd,
                स्थिरांक अक्षर **path,
                पूर्णांक *at_flags,
                पूर्णांक *ret_tfd,
                bool *ret_opath) {

        पूर्णांक r;

        प्रतिज्ञा(fd);
        प्रतिज्ञा(*fd >= 0 || *fd == AT_FDCWD);
        प्रतिज्ञा(path);
        प्रतिज्ञा(at_flags);
        प्रतिज्ञा(ret_tfd);
        प्रतिज्ञा(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        जर (*fd == AT_FDCWD) {
                जर (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                परत 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        जर (!*path) {
                r = fd_is_opath(*fd);
                जर (r < 0)
                        परत r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                परत 0;
        }

        /* If both have been specified, then we go via O_PATH */

        पूर्णांक tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        जर (tfd < 0)
                परत -त्रुटि_सं;

        *fd = *ret_tfd = tfd;
        *path = रिक्त;
        *ret_opath = true;

        परत 0;
}

स्थिर ssize_t getxattr_pinned_internal(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक at_flags,
                bool by_procfs,
                स्थिरांक अक्षर *name,
                अक्षर *buf,
                माप_प्रकार size) {

        ssize_t n;

        प्रतिज्ञा(!path || !isempty(path));
        प्रतिज्ञा((fd >= 0) == !path);
        प्रतिज्ञा(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        प्रतिज्ञा(name);
        प्रतिज्ञा(buf || size == 0);

        जर (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        अन्यथा
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        जर (n < 0)
                परत -त्रुटि_सं;

        प्रतिज्ञा(size == 0 || (माप_प्रकार) n <= size);
        परत n;
}

पूर्णांक getxattr_at_malloc(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *name,
                पूर्णांक at_flags,
                अक्षर **ret,
                माप_प्रकार *ret_size) {

        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        bool by_procfs;
        पूर्णांक r;

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
        जर (r < 0)
                परत r;

        माप_प्रकार l = 100;
        साठी (अचिन्हित n_attempts = 7;;) {
                _cleanup_free_ अक्षर *v = रिक्त;

                जर (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        परत -EBUSY;
                n_attempts--;

                v = new(अक्षर, l+1);
                जर (!v)
                        परत -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                जर (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        जर (!ret_size && n > 1 && स_प्रथम(v, 0, n - 1))
                                परत -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        जर (ret_size)
                                *ret_size = (माप_प्रकार) n;

                        परत 0;
                }
                जर (n != -दुस्फल)
                        परत (पूर्णांक) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, रिक्त, 0);
                जर (n < 0)
                        परत (पूर्णांक) n;

                l = (माप_प्रकार) n;
        }
}

पूर्णांक getxattr_at_bool(पूर्णांक fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, पूर्णांक at_flags) {
        _cleanup_free_ अक्षर *v = रिक्त;
        पूर्णांक r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ रिक्त);
        जर (r < 0)
                परत r;

        परत parse_boolean(v);
}

पूर्णांक getxattr_at_strv(पूर्णांक fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, पूर्णांक at_flags, अक्षर ***ret_strv) {
        _cleanup_free_ अक्षर *nulstr = रिक्त;
        माप_प्रकार nulstr_size;
        पूर्णांक r;

        प्रतिज्ञा(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        जर (r < 0)
                परत r;

        _cleanup_strv_free_ अक्षर **l = strv_parse_nulstr(nulstr, nulstr_size);
        जर (!l)
                परत -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        परत 0;
}

स्थिर पूर्णांक listxattr_pinned_internal(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक at_flags,
                bool by_procfs,
                अक्षर *buf,
                माप_प्रकार size) {

        ssize_t n;

        प्रतिज्ञा(!path || !isempty(path));
        प्रतिज्ञा((fd >= 0) == !path);
        प्रतिज्ञा(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        प्रतिज्ञा(buf || size == 0);

        जर (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        अन्यथा
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        जर (n < 0)
                परत -त्रुटि_सं;

        प्रतिज्ञा(size == 0 || (माप_प्रकार) n <= size);

        जर (n > पूर्णांक_उच्च) /* We couldn't return this as 'int' anymore */
                परत -E2BIG;

        परत (पूर्णांक) n;
}

पूर्णांक listxattr_at_malloc(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, अक्षर **ret) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        bool by_procfs;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        जर (r < 0)
                परत r;

        माप_प्रकार l = 100;
        साठी (अचिन्हित n_attempts = 7;;) {
                _cleanup_free_ अक्षर *v = रिक्त;

                जर (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        परत -EBUSY;
                n_attempts--;

                v = new(अक्षर, l+1);
                जर (!v)
                        परत -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                जर (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        परत r;
                }
                जर (r != -दुस्फल)
                        परत r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, रिक्त, 0);
                जर (r < 0)
                        परत r;

                l = (माप_प्रकार) r;
        }
}

पूर्णांक xsetxattr_full(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक at_flags,
                स्थिरांक अक्षर *name,
                स्थिरांक अक्षर *मूल्य,
                माप_प्रकार size,
                पूर्णांक xattr_flags) {

        पूर्णांक r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(name);
        प्रतिज्ञा(मूल्य || size == 0);

        जर (size == SIZE_MAX)
                size = म_माप(मूल्य);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ अक्षर *old_मूल्य = रिक्त;
        माप_प्रकार old_size;

        जर (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_मूल्य, &old_size) >= 0 &&
            memcmp_nn(old_मूल्य, old_size, मूल्य, size) == 0)
                परत 0;

        जर (have_xattrat && !isempty(path)) {
                संरचना xattr_args args = {
                        .मूल्य = PTR_TO_UINT64(मूल्य),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, आकार(args)));
                जर (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        परत r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        जर (r < 0)
                परत r;

        जर (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, मूल्य, size, xattr_flags)
                                                           : lsetxattr(path, name, मूल्य, size, xattr_flags);
        अन्यथा
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, मूल्य, size, xattr_flags)
                              : fsetxattr(fd, name, मूल्य, size, xattr_flags);
        जर (r < 0)
                परत -त्रुटि_सं;

        परत 0;
}

पूर्णांक xsetxattr_strv(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, स्थिरांक अक्षर *name, अक्षर * स्थिरांक *l) {
        _cleanup_free_ अक्षर *nulstr = रिक्त;
        माप_प्रकार size;
        पूर्णांक r;

        प्रतिज्ञा(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        जर (r < 0)
                परत r;

        परत xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

पूर्णांक xremovexattr(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, स्थिरांक अक्षर *name) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        प्रतिज्ञा(name);

        जर (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                जर (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        परत r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णांक tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        जर (r < 0)
                परत r;

        जर (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        अन्यथा
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        जर (r < 0)
                परत -त्रुटि_सं;

        परत 0;
}

स्थिर पूर्णांक parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        प्रतिज्ञा(ret);

        assert_cc(आकार(usec_t) == आकार(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        जर (!timestamp_is_set(u))
                परत -EIO;

        *ret = u;
        परत 0;
}

पूर्णांक getcrtime_at(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = रिक्त;
        संरचना statx sx;
        usec_t a, b;
        पूर्णांक r;

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
        जर (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        अन्यथा
                a = USEC_INFINITY;

        माप_प्रकार le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (अक्षर**) &le, &le_size);
        जर (r >= 0) {
                जर (le_size != आकार(*le))
                        r = -EIO;
                अन्यथा
                        r = parse_crtime(*le, &b);
        }
        जर (r < 0) {
                जर (a != USEC_INFINITY) {
                        जर (ret)
                                *ret = a;
                        परत 0;
                }

                परत r;
        }

        जर (ret)
                *ret = MIN(a, b);
        परत 0;
}

पूर्णांक fd_setcrtime(पूर्णांक fd, usec_t usec) {
        le64_t le;

        प्रतिज्ञा(fd >= 0);

        जर (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        परत xsetxattr_full(fd, /* path= */ रिक्त, AT_EMPTY_PATH,
                              "user.crtime_usec", (स्थिरांक अक्षर*) &le, आकार(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(स्थिरांक अक्षर *name) {
        परत STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(स्थिरांक अक्षर *name) {
        परत streq(ASSERT_PTR(name), "security.selinux");
}
