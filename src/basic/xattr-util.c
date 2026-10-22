/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sys/xattr.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "nulstr-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "sparse-endian.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
ସ୍ଥିର thread_local bool have_xattrat = true;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା normalize_and_maybe_pin_inode(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd,
                ଅଚଳ ଅକ୍ଷର **path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *at_flags,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_tfd,
                bool *ret_opath) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd);
        ପ୍ରତିଜ୍ଞା(*fd >= 0 || *fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(at_flags);
        ପ୍ରତିଜ୍ଞା(ret_tfd);
        ପ୍ରତିଜ୍ଞା(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        ଯଦି (*fd == AT_FDCWD) {
                ଯଦି (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                ଫେରନ୍ତୁ 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        ଯଦି (!*path) {
                r = fd_is_opath(*fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                ଫେରନ୍ତୁ 0;
        }

        /* If both have been specified, then we go via O_PATH */

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ଯଦି (tfd < 0)
                ଫେରନ୍ତୁ -errno;

        *fd = *ret_tfd = tfd;
        *path = ଖାଲି;
        *ret_opath = true;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ssize_t getxattr_pinned_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                bool by_procfs,
                ଅଚଳ ଅକ୍ଷର *name,
                ଅକ୍ଷର *buf,
                size_t size) {

        ssize_t n;

        ପ୍ରତିଜ୍ଞା(!path || !isempty(path));
        ପ୍ରତିଜ୍ଞା((fd >= 0) == !path);
        ପ୍ରତିଜ୍ଞା(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(buf || size == 0);

        ଯଦି (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        ଅନ୍ୟଥା
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ -errno;

        ପ୍ରତିଜ୍ଞା(size == 0 || (size_t) n <= size);
        ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_at_malloc(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ଅଚଳ ଅକ୍ଷର *name,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                ଅକ୍ଷର **ret,
                size_t *ret_size) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;
        bool by_procfs;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ପ୍ରତିଜ୍ଞା(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        size_t l = 100;
        ପାଇଁ (unsigned n_attempts = 7;;) {
                _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;

                ଯଦି (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ଫେରନ୍ତୁ -EBUSY;
                n_attempts--;

                v = new(ଅକ୍ଷର, l+1);
                ଯଦି (!v)
                        ଫେରନ୍ତୁ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                ଯଦି (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ଯଦି (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                ଫେରନ୍ତୁ -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ଯଦି (ret_size)
                                *ret_size = (size_t) n;

                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (n != -ERANGE)
                        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, ଖାଲି, 0);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;

                l = (size_t) n;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_at_bool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags) {
        _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ parse_boolean(v);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_at_strv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅକ୍ଷର ***ret_strv) {
        _cleanup_free_ ଅକ୍ଷର *nulstr = ଖାଲି;
        size_t nulstr_size;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_strv_free_ ଅକ୍ଷର **l = strv_parse_nulstr(nulstr, nulstr_size);
        ଯଦି (!l)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା listxattr_pinned_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                bool by_procfs,
                ଅକ୍ଷର *buf,
                size_t size) {

        ssize_t n;

        ପ୍ରତିଜ୍ଞା(!path || !isempty(path));
        ପ୍ରତିଜ୍ଞା((fd >= 0) == !path);
        ପ୍ରତିଜ୍ଞା(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        ପ୍ରତିଜ୍ଞା(buf || size == 0);

        ଯଦି (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        ଅନ୍ୟଥା
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ -errno;

        ପ୍ରତିଜ୍ଞା(size == 0 || (size_t) n <= size);

        ଯଦି (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                ଫେରନ୍ତୁ -E2BIG;

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା listxattr_at_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅକ୍ଷର **ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;
        bool by_procfs;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ପ୍ରତିଜ୍ଞା(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        size_t l = 100;
        ପାଇଁ (unsigned n_attempts = 7;;) {
                _cleanup_free_ ଅକ୍ଷର *v = ଖାଲି;

                ଯଦି (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        ଫେରନ୍ତୁ -EBUSY;
                n_attempts--;

                v = new(ଅକ୍ଷର, l+1);
                ଯଦି (!v)
                        ଫେରନ୍ତୁ -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                ଯଦି (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ଫେରନ୍ତୁ r;
                }
                ଯଦି (r != -ERANGE)
                        ଫେରନ୍ତୁ r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, ଖାଲି, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                l = (size_t) r;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xsetxattr_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ,
                size_t size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା xattr_flags) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ || size == 0);

        ଯଦି (size == SIZE_MAX)
                size = strlen(ମୂଲ୍ୟ);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ ଅକ୍ଷର *old_ମୂଲ୍ୟ = ଖାଲି;
        size_t old_size;

        ଯଦି (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_ମୂଲ୍ୟ, &old_size) >= 0 &&
            memcmp_nn(old_ମୂଲ୍ୟ, old_size, ମୂଲ୍ୟ, size) == 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (have_xattrat && !isempty(path)) {
                ସଂରଚନା xattr_args args = {
                        .ମୂଲ୍ୟ = PTR_TO_UINT64(ମୂଲ୍ୟ),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, ଆକାର(args)));
                ଯଦି (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ଫେରନ୍ତୁ r;

                have_xattrat = false;
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, ମୂଲ୍ୟ, size, xattr_flags)
                                                           : lsetxattr(path, name, ମୂଲ୍ୟ, size, xattr_flags);
        ଅନ୍ୟଥା
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, ମୂଲ୍ୟ, size, xattr_flags)
                              : fsetxattr(fd, name, ମୂଲ୍ୟ, size, xattr_flags);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xsetxattr_strv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର * ଅଚଳ *l) {
        _cleanup_free_ ଅକ୍ଷର *nulstr = ଖାଲି;
        size_t size;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xremovexattr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅଚଳ ଅକ୍ଷର *name) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ପ୍ରତିଜ୍ଞା(name);

        ଯଦି (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ଯଦି (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        ଫେରନ୍ତୁ r;

                have_xattrat = false;
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        ଅନ୍ୟଥା
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        ପ୍ରତିଜ୍ଞା(ret);

        assert_cc(ଆକାର(usec_t) == ଆକାର(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ଯଦି (!timestamp_is_set(u))
                ଫେରନ୍ତୁ -EIO;

        *ret = u;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getcrtime_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ଖାଲି;
        ସଂରଚନା statx sx;
        usec_t a, b;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        ଯଦି (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        ଅନ୍ୟଥା
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (ଅକ୍ଷର**) &le, &le_size);
        ଯଦି (r >= 0) {
                ଯଦି (le_size != ଆକାର(*le))
                        r = -EIO;
                ଅନ୍ୟଥା
                        r = parse_crtime(*le, &b);
        }
        ଯଦି (r < 0) {
                ଯଦି (a != USEC_INFINITY) {
                        ଯଦି (ret)
                                *ret = a;
                        ଫେରନ୍ତୁ 0;
                }

                ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret)
                *ret = MIN(a, b);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_setcrtime(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, usec_t usec) {
        le64_t le;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        ଫେରନ୍ତୁ xsetxattr_full(fd, /* path= */ ଖାଲି, AT_EMPTY_PATH,
                              "user.crtime_usec", (ଅଚଳ ଅକ୍ଷର*) &le, ଆକାର(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ଅଚଳ ଅକ୍ଷର *name) {
        ଫେରନ୍ତୁ STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ଅଚଳ ଅକ୍ଷର *name) {
        ଫେରନ୍ତୁ streq(ASSERT_PTR(name), "security.selinux");
}
