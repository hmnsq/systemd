/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <sys/xattr.h>
#समावेश <threads.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "nulstr-util.h"
#समावेश "parse-util.h"
#समावेश "sparse-endian.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "time-util.h"
#समावेश "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
अटल thread_local bool have_xattrat = true;

अटल पूर्णांक normalize_and_maybe_pin_inode(
                पूर्णांक *fd,
                स्थिर अक्षर **path,
                पूर्णांक *at_flags,
                पूर्णांक *ret_tfd,
                bool *ret_opath) {

        पूर्णांक r;

        निश्चित(fd);
        निश्चित(*fd >= 0 || *fd == AT_FDCWD);
        निश्चित(path);
        निश्चित(at_flags);
        निश्चित(ret_tfd);
        निश्चित(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        अगर (*fd == AT_FDCWD) {
                अगर (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                वापस 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        अगर (!*path) {
                r = fd_is_opath(*fd);
                अगर (r < 0)
                        वापस r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                वापस 0;
        }

        /* If both have been specified, then we go via O_PATH */

        पूर्णांक tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        अगर (tfd < 0)
                वापस -त्रुटि_सं;

        *fd = *ret_tfd = tfd;
        *path = शून्य;
        *ret_opath = true;

        वापस 0;
}

अटल ssize_t getxattr_pinned_internal(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक at_flags,
                bool by_procfs,
                स्थिर अक्षर *name,
                अक्षर *buf,
                माप_प्रकार आकार) {

        ssize_t n;

        निश्चित(!path || !isempty(path));
        निश्चित((fd >= 0) == !path);
        निश्चित(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        निश्चित(name);
        निश्चित(buf || आकार == 0);

        अगर (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, आकार)
                                                           : lgetxattr(path, name, buf, आकार);
        अन्यथा
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, आकार)
                              : fgetxattr(fd, name, buf, आकार);
        अगर (n < 0)
                वापस -त्रुटि_सं;

        निश्चित(आकार == 0 || (माप_प्रकार) n <= आकार);
        वापस n;
}

पूर्णांक getxattr_at_malloc(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                स्थिर अक्षर *name,
                पूर्णांक at_flags,
                अक्षर **ret,
                माप_प्रकार *ret_आकार) {

        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        bool by_procfs;
        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);
        निश्चित(name);
        निश्चित((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        निश्चित(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        अगर (r < 0)
                वापस r;

        माप_प्रकार l = 100;
        क्रम (अचिन्हित n_attempts = 7;;) {
                _cleanup_free_ अक्षर *v = शून्य;

                अगर (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        वापस -EBUSY;
                n_attempts--;

                v = new(अक्षर, l+1);
                अगर (!v)
                        वापस -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                अगर (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        अगर (!ret_आकार && n > 1 && स_प्रथम(v, 0, n - 1))
                                वापस -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        अगर (ret_आकार)
                                *ret_आकार = (माप_प्रकार) n;

                        वापस 0;
                }
                अगर (n != -दुस्फल)
                        वापस (पूर्णांक) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, शून्य, 0);
                अगर (n < 0)
                        वापस (पूर्णांक) n;

                l = (माप_प्रकार) n;
        }
}

पूर्णांक getxattr_at_bool(पूर्णांक fd, स्थिर अक्षर *path, स्थिर अक्षर *name, पूर्णांक at_flags) {
        _cleanup_free_ अक्षर *v = शून्य;
        पूर्णांक r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ शून्य);
        अगर (r < 0)
                वापस r;

        वापस parse_boolean(v);
}

पूर्णांक getxattr_at_strv(पूर्णांक fd, स्थिर अक्षर *path, स्थिर अक्षर *name, पूर्णांक at_flags, अक्षर ***ret_strv) {
        _cleanup_free_ अक्षर *nulstr = शून्य;
        माप_प्रकार nulstr_आकार;
        पूर्णांक r;

        निश्चित(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_आकार);
        अगर (r < 0)
                वापस r;

        _cleanup_strv_free_ अक्षर **l = strv_parse_nulstr(nulstr, nulstr_आकार);
        अगर (!l)
                वापस -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        वापस 0;
}

अटल पूर्णांक listxattr_pinned_internal(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक at_flags,
                bool by_procfs,
                अक्षर *buf,
                माप_प्रकार आकार) {

        ssize_t n;

        निश्चित(!path || !isempty(path));
        निश्चित((fd >= 0) == !path);
        निश्चित(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        निश्चित(buf || आकार == 0);

        अगर (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, आकार)
                                                           : llistxattr(path, buf, आकार);
        अन्यथा
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, आकार)
                              : flistxattr(fd, buf, आकार);
        अगर (n < 0)
                वापस -त्रुटि_सं;

        निश्चित(आकार == 0 || (माप_प्रकार) n <= आकार);

        अगर (n > पूर्णांक_उच्च) /* We couldn't return this as 'int' anymore */
                वापस -E2BIG;

        वापस (पूर्णांक) n;
}

पूर्णांक listxattr_at_malloc(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, अक्षर **ret) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        bool by_procfs;
        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);
        निश्चित((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        निश्चित(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        अगर (r < 0)
                वापस r;

        माप_प्रकार l = 100;
        क्रम (अचिन्हित n_attempts = 7;;) {
                _cleanup_free_ अक्षर *v = शून्य;

                अगर (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        वापस -EBUSY;
                n_attempts--;

                v = new(अक्षर, l+1);
                अगर (!v)
                        वापस -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                अगर (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        वापस r;
                }
                अगर (r != -दुस्फल)
                        वापस r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, शून्य, 0);
                अगर (r < 0)
                        वापस r;

                l = (माप_प्रकार) r;
        }
}

पूर्णांक xsetxattr_full(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक at_flags,
                स्थिर अक्षर *name,
                स्थिर अक्षर *मूल्य,
                माप_प्रकार आकार,
                पूर्णांक xattr_flags) {

        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);
        निश्चित((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        निश्चित(name);
        निश्चित(मूल्य || आकार == 0);

        अगर (आकार == SIZE_MAX)
                आकार = म_माप(मूल्य);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ अक्षर *old_मूल्य = शून्य;
        माप_प्रकार old_आकार;

        अगर (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_मूल्य, &old_आकार) >= 0 &&
            memcmp_nn(old_मूल्य, old_आकार, मूल्य, आकार) == 0)
                वापस 0;

        अगर (have_xattrat && !isempty(path)) {
                काष्ठा xattr_args args = {
                        .मूल्य = PTR_TO_UINT64(मूल्य),
                        .आकार = आकार,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, माप(args)));
                अगर (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        वापस r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        अगर (r < 0)
                वापस r;

        अगर (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, मूल्य, आकार, xattr_flags)
                                                           : lsetxattr(path, name, मूल्य, आकार, xattr_flags);
        अन्यथा
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, मूल्य, आकार, xattr_flags)
                              : fsetxattr(fd, name, मूल्य, आकार, xattr_flags);
        अगर (r < 0)
                वापस -त्रुटि_सं;

        वापस 0;
}

पूर्णांक xsetxattr_strv(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, स्थिर अक्षर *name, अक्षर * स्थिर *l) {
        _cleanup_free_ अक्षर *nulstr = शून्य;
        माप_प्रकार आकार;
        पूर्णांक r;

        निश्चित(name);

        r = strv_make_nulstr(l, &nulstr, &आकार);
        अगर (r < 0)
                वापस r;

        वापस xsetxattr_full(fd, path, at_flags, name, nulstr, आकार, /* xattr_flags= */ 0);
}

पूर्णांक xremovexattr(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, स्थिर अक्षर *name) {
        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);
        निश्चित((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        निश्चित(name);

        अगर (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                अगर (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        वापस r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णांक tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        अगर (r < 0)
                वापस r;

        अगर (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        अन्यथा
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        अगर (r < 0)
                वापस -त्रुटि_सं;

        वापस 0;
}

अटल पूर्णांक parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        निश्चित(ret);

        assert_cc(माप(usec_t) == माप(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        अगर (!timestamp_is_set(u))
                वापस -EIO;

        *ret = u;
        वापस 0;
}

पूर्णांक getcrtime_at(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = शून्य;
        काष्ठा statx sx;
        usec_t a, b;
        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);
        निश्चित((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        अगर (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        अन्यथा
                a = USEC_INFINITY;

        माप_प्रकार le_आकार;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (अक्षर**) &le, &le_आकार);
        अगर (r >= 0) {
                अगर (le_आकार != माप(*le))
                        r = -EIO;
                अन्यथा
                        r = parse_crtime(*le, &b);
        }
        अगर (r < 0) {
                अगर (a != USEC_INFINITY) {
                        अगर (ret)
                                *ret = a;
                        वापस 0;
                }

                वापस r;
        }

        अगर (ret)
                *ret = MIN(a, b);
        वापस 0;
}

पूर्णांक fd_setcrtime(पूर्णांक fd, usec_t usec) {
        le64_t le;

        निश्चित(fd >= 0);

        अगर (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        वापस xsetxattr_full(fd, /* path= */ शून्य, AT_EMPTY_PATH,
                              "user.crtime_usec", (स्थिर अक्षर*) &le, माप(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(स्थिर अक्षर *name) {
        वापस STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(स्थिर अक्षर *name) {
        वापस streq(ASSERT_PTR(name), "security.selinux");
}
