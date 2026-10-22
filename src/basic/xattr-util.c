/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sys/xattr.h>
#అన్తర్భావ <threads.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "nulstr-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "sparse-endian.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "xattr-util.h"

/* Use a single cache for all of *xattrat syscalls (added in kernel 6.13) */
స్థిర thread_local bool have_xattrat = true;

స్థిర పూర్ణాంకం normalize_and_maybe_pin_inode(
                పూర్ణాంకం *fd,
                స్థిరరాశి అక్షరం **మార్గం,
                పూర్ణాంకం *at_flags,
                పూర్ణాంకం *ret_tfd,
                bool *ret_opath) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd);
        ప్రతిజ్ఞా(*fd >= 0 || *fd == AT_FDCWD);
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(at_flags);
        ప్రతిజ్ఞా(ret_tfd);
        ప్రతిజ్ఞా(ret_opath);

        *మార్గం = ఖాళీలు_గమ్యం_null(*మార్గం); /* Normalize "" to NULL */

        ఒకవేళ (*fd == AT_FDCWD) {
                ఒకవేళ (!*మార్గం) /* Both unspecified? Then operate on current working directory */
                        *మార్గం = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                తిరిగి 0;
        }

        *at_flags |= AT_ఖాళీలు_మార్గం;

        ఒకవేళ (!*మార్గం) {
                r = fd_is_opath(*fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                తిరిగి 0;
        }

        /* If both have been specified, then we go via O_PATH */

        పూర్ణాంకం tfd = openat(*fd, *మార్గం, O_మార్గం|O_CLOEXEC|(FLAGS_సమితి(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        ఒకవేళ (tfd < 0)
                తిరిగి -errno;

        *fd = *ret_tfd = tfd;
        *మార్గం = ఖాళీ;
        *ret_opath = true;

        తిరిగి 0;
}

స్థిర ssize_t getxattr_pinned_అంతర్గత(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం at_flags,
                bool by_procfs,
                స్థిరరాశి అక్షరం *name,
                అక్షరం *buf,
                size_t కొలత) {

        ssize_t n;

        ప్రతిజ్ఞా(!మార్గం || !isempty(మార్గం));
        ప్రతిజ్ఞా((fd >= 0) == !మార్గం);
        ప్రతిజ్ఞా(మార్గం || FLAGS_సమితి(at_flags, AT_ఖాళీలు_మార్గం));
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(buf || కొలత == 0);

        ఒకవేళ (మార్గం)
                n = FLAGS_సమితి(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(మార్గం, name, buf, కొలత)
                                                           : lgetxattr(మార్గం, name, buf, కొలత);
        లేకపోతే
                n = by_procfs ? getxattr(FORMAT_PROC_FD_మార్గం(fd), name, buf, కొలత)
                              : fgetxattr(fd, name, buf, కొలత);
        ఒకవేళ (n < 0)
                తిరిగి -errno;

        ప్రతిజ్ఞా(కొలత == 0 || (size_t) n <= కొలత);
        తిరిగి n;
}

పూర్ణాంకం getxattr_at_malloc(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *name,
                పూర్ణాంకం at_flags,
                అక్షరం **ret,
                size_t *ret_కొలత) {

        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;
        bool by_procfs;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం)) == 0);
        ప్రతిజ్ఞా(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &మార్గం, &at_flags, &opened_fd, &by_procfs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        size_t l = 100;
        కొరకు (గుర్తులేని n_attempts = 7;;) {
                _cleanup_free_ అక్షరం *v = ఖాళీ;

                ఒకవేళ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        తిరిగి -EBUSY;
                n_attempts--;

                v = new(అక్షరం, l+1);
                ఒకవేళ (!v)
                        తిరిగి -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_అంతర్గత(fd, మార్గం, at_flags, by_procfs, name, v, l);
                ఒకవేళ (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        ఒకవేళ (!ret_కొలత && n > 1 && memchr(v, 0, n - 1))
                                తిరిగి -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        ఒకవేళ (ret_కొలత)
                                *ret_కొలత = (size_t) n;

                        తిరిగి 0;
                }
                ఒకవేళ (n != -ERANGE)
                        తిరిగి (పూర్ణాంకం) n;

                n = getxattr_pinned_అంతర్గత(fd, మార్గం, at_flags, by_procfs, name, ఖాళీ, 0);
                ఒకవేళ (n < 0)
                        తిరిగి (పూర్ణాంకం) n;

                l = (size_t) n;
        }
}

పూర్ణాంకం getxattr_at_bool(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, పూర్ణాంకం at_flags) {
        _cleanup_free_ అక్షరం *v = ఖాళీ;
        పూర్ణాంకం r;

        r = getxattr_at_malloc(fd, మార్గం, name, at_flags, &v, /* ret_size= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి parse_boolean(v);
}

పూర్ణాంకం getxattr_at_strv(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, పూర్ణాంకం at_flags, అక్షరం ***ret_strv) {
        _cleanup_free_ అక్షరం *nulstr = ఖాళీ;
        size_t nulstr_కొలత;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_strv);

        r = getxattr_at_malloc(fd, మార్గం, name, at_flags, &nulstr, &nulstr_కొలత);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_strv_free_ అక్షరం **l = strv_parse_nulstr(nulstr, nulstr_కొలత);
        ఒకవేళ (!l)
                తిరిగి -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం listxattr_pinned_అంతర్గత(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం at_flags,
                bool by_procfs,
                అక్షరం *buf,
                size_t కొలత) {

        ssize_t n;

        ప్రతిజ్ఞా(!మార్గం || !isempty(మార్గం));
        ప్రతిజ్ఞా((fd >= 0) == !మార్గం);
        ప్రతిజ్ఞా(మార్గం || FLAGS_సమితి(at_flags, AT_ఖాళీలు_మార్గం));
        ప్రతిజ్ఞా(buf || కొలత == 0);

        ఒకవేళ (మార్గం)
                n = FLAGS_సమితి(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(మార్గం, buf, కొలత)
                                                           : llistxattr(మార్గం, buf, కొలత);
        లేకపోతే
                n = by_procfs ? listxattr(FORMAT_PROC_FD_మార్గం(fd), buf, కొలత)
                              : flistxattr(fd, buf, కొలత);
        ఒకవేళ (n < 0)
                తిరిగి -errno;

        ప్రతిజ్ఞా(కొలత == 0 || (size_t) n <= కొలత);

        ఒకవేళ (n > గరిష్ఠ_పూర్ణాంకం) /* We couldn't return this as 'int' anymore */
                తిరిగి -E2BIG;

        తిరిగి (పూర్ణాంకం) n;
}

పూర్ణాంకం listxattr_at_malloc(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, అక్షరం **ret) {
        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;
        bool by_procfs;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం)) == 0);
        ప్రతిజ్ఞా(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &మార్గం, &at_flags, &opened_fd, &by_procfs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        size_t l = 100;
        కొరకు (గుర్తులేని n_attempts = 7;;) {
                _cleanup_free_ అక్షరం *v = ఖాళీ;

                ఒకవేళ (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        తిరిగి -EBUSY;
                n_attempts--;

                v = new(అక్షరం, l+1);
                ఒకవేళ (!v)
                        తిరిగి -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_అంతర్గత(fd, మార్గం, at_flags, by_procfs, v, l);
                ఒకవేళ (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        తిరిగి r;
                }
                ఒకవేళ (r != -ERANGE)
                        తిరిగి r;

                r = listxattr_pinned_అంతర్గత(fd, మార్గం, at_flags, by_procfs, ఖాళీ, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                l = (size_t) r;
        }
}

పూర్ణాంకం xsetxattr_నిండిన(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం at_flags,
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి అక్షరం *విలువ,
                size_t కొలత,
                పూర్ణాంకం xattr_flags) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం)) == 0);
        ప్రతిజ్ఞా(name);
        ప్రతిజ్ఞా(విలువ || కొలత == 0);

        ఒకవేళ (కొలత == SIZE_MAX)
                కొలత = అక్షరశ్రేణి_పొడవు(విలువ);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ అక్షరం *old_విలువ = ఖాళీ;
        size_t old_కొలత;

        ఒకవేళ (xattr_flags == 0 &&
            getxattr_at_malloc(fd, మార్గం, name, at_flags, &old_విలువ, &old_కొలత) >= 0 &&
            memcmp_nn(old_విలువ, old_కొలత, విలువ, కొలత) == 0)
                తిరిగి 0;

        ఒకవేళ (have_xattrat && !isempty(మార్గం)) {
                నిర్మాణం xattr_వాదనలు వాదనలు = {
                        .విలువ = PTR_గమ్యం_UINT64(విలువ),
                        .కొలత = కొలత,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, మార్గం,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &వాదనలు, పరిమాణం(వాదనలు)));
                ఒకవేళ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        తిరిగి r;

                have_xattrat = false;
        }

        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &మార్గం, &at_flags, &opened_fd, &by_procfs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (మార్గం)
                r = FLAGS_సమితి(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(మార్గం, name, విలువ, కొలత, xattr_flags)
                                                           : lsetxattr(మార్గం, name, విలువ, కొలత, xattr_flags);
        లేకపోతే
                r = by_procfs ? setxattr(FORMAT_PROC_FD_మార్గం(fd), name, విలువ, కొలత, xattr_flags)
                              : fsetxattr(fd, name, విలువ, కొలత, xattr_flags);
        ఒకవేళ (r < 0)
                తిరిగి -errno;

        తిరిగి 0;
}

పూర్ణాంకం xsetxattr_strv(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, స్థిరరాశి అక్షరం *name, అక్షరం * స్థిరరాశి *l) {
        _cleanup_free_ అక్షరం *nulstr = ఖాళీ;
        size_t కొలత;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);

        r = strv_make_nulstr(l, &nulstr, &కొలత);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి xsetxattr_నిండిన(fd, మార్గం, at_flags, name, nulstr, కొలత, /* xattr_flags= */ 0);
}

పూర్ణాంకం xremovexattr(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, స్థిరరాశి అక్షరం *name) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం)) == 0);
        ప్రతిజ్ఞా(name);

        ఒకవేళ (have_xattrat && !isempty(మార్గం)) {
                r = RET_NERRNO(removexattrat(fd, మార్గం,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                ఒకవేళ (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        తిరిగి r;

                have_xattrat = false;
        }

        _cleanup_close_ పూర్ణాంకం tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &మార్గం, &at_flags, &tfd, &by_procfs);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (మార్గం)
                r = FLAGS_సమితి(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(మార్గం, name)
                                                           : lremovexattr(మార్గం, name);
        లేకపోతే
                r = by_procfs ? removexattr(FORMAT_PROC_FD_మార్గం(fd), name)
                              : fremovexattr(fd, name);
        ఒకవేళ (r < 0)
                తిరిగి -errno;

        తిరిగి 0;
}

స్థిర పూర్ణాంకం parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        ప్రతిజ్ఞా(ret);

        assert_cc(పరిమాణం(usec_t) == పరిమాణం(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        ఒకవేళ (!timestamp_is_సమితి(u))
                తిరిగి -EIO;

        *ret = u;
        తిరిగి 0;
}

పూర్ణాంకం getcrtime_at(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = ఖాళీ;
        నిర్మాణం statx sx;
        usec_t a, b;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా((at_flags & ~(AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_నిండిన(fd,
                        మార్గం,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        ఒకవేళ (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_ఎక్కించు(&sx.stx_btime);
        లేకపోతే
                a = USEC_INFINITY;

        size_t le_కొలత;
        r = getxattr_at_malloc(fd, మార్గం, "user.crtime_usec", at_flags, (అక్షరం**) &le, &le_కొలత);
        ఒకవేళ (r >= 0) {
                ఒకవేళ (le_కొలత != పరిమాణం(*le))
                        r = -EIO;
                లేకపోతే
                        r = parse_crtime(*le, &b);
        }
        ఒకవేళ (r < 0) {
                ఒకవేళ (a != USEC_INFINITY) {
                        ఒకవేళ (ret)
                                *ret = a;
                        తిరిగి 0;
                }

                తిరిగి r;
        }

        ఒకవేళ (ret)
                *ret = MIN(a, b);
        తిరిగి 0;
}

పూర్ణాంకం fd_setcrtime(పూర్ణాంకం fd, usec_t usec) {
        le64_t le;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (!timestamp_is_సమితి(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        తిరిగి xsetxattr_నిండిన(fd, /* path= */ ఖాళీ, AT_ఖాళీలు_మార్గం,
                              "user.crtime_usec", (స్థిరరాశి అక్షరం*) &le, పరిమాణం(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(స్థిరరాశి అక్షరం *name) {
        తిరిగి STR_లోపల_సమితి(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(స్థిరరాశి అక్షరం *name) {
        తిరిగి streq(ASSERT_PTR(name), "security.selinux");
}
