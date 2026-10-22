/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sys/xattr.h>
#அந்தர்பாவ <threads.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "nulstr-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "sparse-endian.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
நிலையான thread_local bool have_xattrat = true;

நிலையான முழு normalize_and_maybe_pin_inode(
                முழு *fd,
                மாறிலி எழுத்து **path,
                முழு *at_flags,
                முழு *ret_tfd,
                bool *ret_opath) {

        முழு r;

        ப்ரதிஜ்ஞா(fd);
        ப்ரதிஜ்ஞா(*fd >= 0 || *fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(at_flags);
        ப்ரதிஜ்ஞா(ret_tfd);
        ப்ரதிஜ்ஞா(ret_opath);

        *path = empty_to_null(*path); /* Normalize "" to NULL */

        எனில் (*fd == AT_FDCWD) {
                எனில் (!*path) /* Both unspecified? Then operate on current working directory */
                        *path = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                திரும்பு 0;
        }

        *at_flags |= AT_EMPTY_PATH;

        எனில் (!*path) {
                r = fd_is_opath(*fd);
                எனில் (r < 0)
                        திரும்பு r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                திரும்பு 0;
        }

        /* If both have been specified, then we go via O_PATH */

        முழு tfd = openat(*fd, *path, O_PATH|O_CLOEXEC|(FLAGS_SET(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        எனில் (tfd < 0)
                திரும்பு -errno;

        *fd = *ret_tfd = tfd;
        *path = வெற்று;
        *ret_opath = true;

        திரும்பு 0;
}

நிலையான ssize_t getxattr_pinned_internal(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு at_flags,
                bool by_procfs,
                மாறிலி எழுத்து *name,
                எழுத்து *buf,
                size_t size) {

        ssize_t n;

        ப்ரதிஜ்ஞா(!path || !isempty(path));
        ப்ரதிஜ்ஞா((fd >= 0) == !path);
        ப்ரதிஜ்ஞா(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(buf || size == 0);

        எனில் (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(path, name, buf, size)
                                                           : lgetxattr(path, name, buf, size);
        இல்லையெனில்
                n = by_procfs ? getxattr(FORMAT_PROC_FD_PATH(fd), name, buf, size)
                              : fgetxattr(fd, name, buf, size);
        எனில் (n < 0)
                திரும்பு -errno;

        ப்ரதிஜ்ஞா(size == 0 || (size_t) n <= size);
        திரும்பு n;
}

முழு getxattr_at_malloc(
                முழு fd,
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *name,
                முழு at_flags,
                எழுத்து **ret,
                size_t *ret_size) {

        _cleanup_close_ முழு opened_fd = -EBADF;
        bool by_procfs;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ப்ரதிஜ்ஞா(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        எனில் (r < 0)
                திரும்பு r;

        size_t l = 100;
        க்கு (unsigned n_attempts = 7;;) {
                _cleanup_free_ எழுத்து *v = வெற்று;

                எனில் (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        திரும்பு -EBUSY;
                n_attempts--;

                v = new(எழுத்து, l+1);
                எனில் (!v)
                        திரும்பு -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, v, l);
                எனில் (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        எனில் (!ret_size && n > 1 && memchr(v, 0, n - 1))
                                திரும்பு -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        எனில் (ret_size)
                                *ret_size = (size_t) n;

                        திரும்பு 0;
                }
                எனில் (n != -ERANGE)
                        திரும்பு (முழு) n;

                n = getxattr_pinned_internal(fd, path, at_flags, by_procfs, name, வெற்று, 0);
                எனில் (n < 0)
                        திரும்பு (முழு) n;

                l = (size_t) n;
        }
}

முழு getxattr_at_bool(முழு fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, முழு at_flags) {
        _cleanup_free_ எழுத்து *v = வெற்று;
        முழு r;

        r = getxattr_at_malloc(fd, path, name, at_flags, &v, /* ret_size= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு parse_boolean(v);
}

முழு getxattr_at_strv(முழு fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, முழு at_flags, எழுத்து ***ret_strv) {
        _cleanup_free_ எழுத்து *nulstr = வெற்று;
        size_t nulstr_size;
        முழு r;

        ப்ரதிஜ்ஞா(ret_strv);

        r = getxattr_at_malloc(fd, path, name, at_flags, &nulstr, &nulstr_size);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_strv_free_ எழுத்து **l = strv_parse_nulstr(nulstr, nulstr_size);
        எனில் (!l)
                திரும்பு -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        திரும்பு 0;
}

நிலையான முழு listxattr_pinned_internal(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு at_flags,
                bool by_procfs,
                எழுத்து *buf,
                size_t size) {

        ssize_t n;

        ப்ரதிஜ்ஞா(!path || !isempty(path));
        ப்ரதிஜ்ஞா((fd >= 0) == !path);
        ப்ரதிஜ்ஞா(path || FLAGS_SET(at_flags, AT_EMPTY_PATH));
        ப்ரதிஜ்ஞா(buf || size == 0);

        எனில் (path)
                n = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(path, buf, size)
                                                           : llistxattr(path, buf, size);
        இல்லையெனில்
                n = by_procfs ? listxattr(FORMAT_PROC_FD_PATH(fd), buf, size)
                              : flistxattr(fd, buf, size);
        எனில் (n < 0)
                திரும்பு -errno;

        ப்ரதிஜ்ஞா(size == 0 || (size_t) n <= size);

        எனில் (n > INT_MAX) /* We couldn't return this as 'int' anymore */
                திரும்பு -E2BIG;

        திரும்பு (முழு) n;
}

முழு listxattr_at_malloc(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, எழுத்து **ret) {
        _cleanup_close_ முழு opened_fd = -EBADF;
        bool by_procfs;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ப்ரதிஜ்ஞா(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        எனில் (r < 0)
                திரும்பு r;

        size_t l = 100;
        க்கு (unsigned n_attempts = 7;;) {
                _cleanup_free_ எழுத்து *v = வெற்று;

                எனில் (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        திரும்பு -EBUSY;
                n_attempts--;

                v = new(எழுத்து, l+1);
                எனில் (!v)
                        திரும்பு -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, v, l);
                எனில் (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        திரும்பு r;
                }
                எனில் (r != -ERANGE)
                        திரும்பு r;

                r = listxattr_pinned_internal(fd, path, at_flags, by_procfs, வெற்று, 0);
                எனில் (r < 0)
                        திரும்பு r;

                l = (size_t) r;
        }
}

முழு xsetxattr_full(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு at_flags,
                மாறிலி எழுத்து *name,
                மாறிலி எழுத்து *மதிப்பு,
                size_t size,
                முழு xattr_flags) {

        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(மதிப்பு || size == 0);

        எனில் (size == SIZE_MAX)
                size = strlen(மதிப்பு);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ எழுத்து *old_மதிப்பு = வெற்று;
        size_t old_size;

        எனில் (xattr_flags == 0 &&
            getxattr_at_malloc(fd, path, name, at_flags, &old_மதிப்பு, &old_size) >= 0 &&
            memcmp_nn(old_மதிப்பு, old_size, மதிப்பு, size) == 0)
                திரும்பு 0;

        எனில் (have_xattrat && !isempty(path)) {
                கட்டமைப்பு xattr_args args = {
                        .மதிப்பு = PTR_TO_UINT64(மதிப்பு),
                        .size = size,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, path,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &args, அளவு(args)));
                எனில் (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        திரும்பு r;

                have_xattrat = false;
        }

        _cleanup_close_ முழு opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &opened_fd, &by_procfs);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(path, name, மதிப்பு, size, xattr_flags)
                                                           : lsetxattr(path, name, மதிப்பு, size, xattr_flags);
        இல்லையெனில்
                r = by_procfs ? setxattr(FORMAT_PROC_FD_PATH(fd), name, மதிப்பு, size, xattr_flags)
                              : fsetxattr(fd, name, மதிப்பு, size, xattr_flags);
        எனில் (r < 0)
                திரும்பு -errno;

        திரும்பு 0;
}

முழு xsetxattr_strv(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, மாறிலி எழுத்து *name, எழுத்து * மாறிலி *l) {
        _cleanup_free_ எழுத்து *nulstr = வெற்று;
        size_t size;
        முழு r;

        ப்ரதிஜ்ஞா(name);

        r = strv_make_nulstr(l, &nulstr, &size);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு xsetxattr_full(fd, path, at_flags, name, nulstr, size, /* xattr_flags= */ 0);
}

முழு xremovexattr(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, மாறிலி எழுத்து *name) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);
        ப்ரதிஜ்ஞா(name);

        எனில் (have_xattrat && !isempty(path)) {
                r = RET_NERRNO(removexattrat(fd, path,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                எனில் (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        திரும்பு r;

                have_xattrat = false;
        }

        _cleanup_close_ முழு tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &path, &at_flags, &tfd, &by_procfs);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (path)
                r = FLAGS_SET(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(path, name)
                                                           : lremovexattr(path, name);
        இல்லையெனில்
                r = by_procfs ? removexattr(FORMAT_PROC_FD_PATH(fd), name)
                              : fremovexattr(fd, name);
        எனில் (r < 0)
                திரும்பு -errno;

        திரும்பு 0;
}

நிலையான முழு parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        ப்ரதிஜ்ஞா(ret);

        assert_cc(அளவு(usec_t) == அளவு(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        எனில் (!timestamp_is_set(u))
                திரும்பு -EIO;

        *ret = u;
        திரும்பு 0;
}

முழு getcrtime_at(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = வெற்று;
        கட்டமைப்பு statx sx;
        usec_t a, b;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா((at_flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH)) == 0);

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
        எனில் (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_load(&sx.stx_btime);
        இல்லையெனில்
                a = USEC_INFINITY;

        size_t le_size;
        r = getxattr_at_malloc(fd, path, "user.crtime_usec", at_flags, (எழுத்து**) &le, &le_size);
        எனில் (r >= 0) {
                எனில் (le_size != அளவு(*le))
                        r = -EIO;
                இல்லையெனில்
                        r = parse_crtime(*le, &b);
        }
        எனில் (r < 0) {
                எனில் (a != USEC_INFINITY) {
                        எனில் (ret)
                                *ret = a;
                        திரும்பு 0;
                }

                திரும்பு r;
        }

        எனில் (ret)
                *ret = MIN(a, b);
        திரும்பு 0;
}

முழு fd_setcrtime(முழு fd, usec_t usec) {
        le64_t le;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (!timestamp_is_set(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        திரும்பு xsetxattr_full(fd, /* path= */ வெற்று, AT_EMPTY_PATH,
                              "user.crtime_usec", (மாறிலி எழுத்து*) &le, அளவு(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(மாறிலி எழுத்து *name) {
        திரும்பு STR_IN_SET(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(மாறிலி எழுத்து *name) {
        திரும்பு streq(ASSERT_PTR(name), "security.selinux");
}
