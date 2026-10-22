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
                ध्रुव वर्ण **मार्गः,
                पूर्णाङ्क *at_flags,
                पूर्णाङ्क *ret_tfd,
                bool *ret_opath) {

        पूर्णाङ्क r;

        प्रतिज्ञा(fd);
        प्रतिज्ञा(*fd >= 0 || *fd == AT_FDCWD);
        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(at_flags);
        प्रतिज्ञा(ret_tfd);
        प्रतिज्ञा(ret_opath);

        *मार्गः = रिक्तानि_गन्तव्यम्_null(*मार्गः); /* Normalize "" to NULL */

        यदि (*fd == AT_FDCWD) {
                यदि (!*मार्गः) /* Both unspecified? Then operate on current working directory */
                        *मार्गः = ".";

                *ret_tfd = -EBADF;
                *ret_opath = false;
                प्रत्यागम 0;
        }

        *at_flags |= AT_रिक्तानि_मार्गः;

        यदि (!*मार्गः) {
                r = fd_is_opath(*fd);
                यदि (r < 0)
                        प्रत्यागम r;
                *ret_opath = r;
                *ret_tfd = -EBADF;
                प्रत्यागम 0;
        }

        /* If both have been specified, then we go via O_PATH */

        पूर्णाङ्क tfd = openat(*fd, *मार्गः, O_मार्गः|O_CLOEXEC|(FLAGS_समुच्चयः(*at_flags, AT_SYMLINK_FOLLOW) ? 0 : O_NOFOLLOW));
        यदि (tfd < 0)
                प्रत्यागम -errno;

        *fd = *ret_tfd = tfd;
        *मार्गः = रिक्त;
        *ret_opath = true;

        प्रत्यागम 0;
}

स्थिर ssize_t getxattr_pinned_आन्तरिकम्(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क at_flags,
                bool by_procfs,
                ध्रुव वर्ण *name,
                वर्ण *buf,
                size_t आकार) {

        ssize_t n;

        प्रतिज्ञा(!मार्गः || !isempty(मार्गः));
        प्रतिज्ञा((fd >= 0) == !मार्गः);
        प्रतिज्ञा(मार्गः || FLAGS_समुच्चयः(at_flags, AT_रिक्तानि_मार्गः));
        प्रतिज्ञा(name);
        प्रतिज्ञा(buf || आकार == 0);

        यदि (मार्गः)
                n = FLAGS_समुच्चयः(at_flags, AT_SYMLINK_FOLLOW) ? getxattr(मार्गः, name, buf, आकार)
                                                           : lgetxattr(मार्गः, name, buf, आकार);
        अन्यथा
                n = by_procfs ? getxattr(FORMAT_PROC_FD_मार्गः(fd), name, buf, आकार)
                              : fgetxattr(fd, name, buf, आकार);
        यदि (n < 0)
                प्रत्यागम -errno;

        प्रतिज्ञा(आकार == 0 || (size_t) n <= आकार);
        प्रत्यागम n;
}

पूर्णाङ्क getxattr_at_malloc(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *name,
                पूर्णाङ्क at_flags,
                वर्ण **ret,
                size_t *ret_आकार) {

        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा(name);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_रिक्तानि_मार्गः)) == 0);
        प्रतिज्ञा(ret);

        /* So, this is single function that does what getxattr()/lgetxattr()/fgetxattr() does, but in one go,
         * and with additional bells and whistles. Specifically:
         *
         * 1. This works on O_PATH fds (via /proc/self/fd/, since getxattrat() syscall refuses them...)
         * 2. As extension to openat()-style semantics implies AT_EMPTY_PATH if path is empty
         * 3. Does a malloc() loop, automatically sizing the allocation
         * 4. NUL-terminates the returned buffer (for safety)
         */

        r = normalize_and_maybe_pin_inode(&fd, &मार्गः, &at_flags, &opened_fd, &by_procfs);
        यदि (r < 0)
                प्रत्यागम r;

        size_t l = 100;
        प्रति (अचिह्नित n_attempts = 7;;) {
                _cleanup_free_ वर्ण *v = रिक्त;

                यदि (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        प्रत्यागम -EBUSY;
                n_attempts--;

                v = new(वर्ण, l+1);
                यदि (!v)
                        प्रत्यागम -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                ssize_t n;
                n = getxattr_pinned_आन्तरिकम्(fd, मार्गः, at_flags, by_procfs, name, v, l);
                यदि (n >= 0) {
                        /* Refuse extended attributes with embedded NUL bytes if the caller isn't interested
                         * in the size. After all this must mean the caller assumes we return a NUL
                         * terminated strings, but if there's a NUL byte embedded they are definitely not
                         * regular strings */
                        यदि (!ret_आकार && n > 1 && memchr(v, 0, n - 1))
                                प्रत्यागम -EBADMSG;

                        v[n] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        यदि (ret_आकार)
                                *ret_आकार = (size_t) n;

                        प्रत्यागम 0;
                }
                यदि (n != -ERANGE)
                        प्रत्यागम (पूर्णाङ्क) n;

                n = getxattr_pinned_आन्तरिकम्(fd, मार्गः, at_flags, by_procfs, name, रिक्त, 0);
                यदि (n < 0)
                        प्रत्यागम (पूर्णाङ्क) n;

                l = (size_t) n;
        }
}

पूर्णाङ्क getxattr_at_bool(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *name, पूर्णाङ्क at_flags) {
        _cleanup_free_ वर्ण *v = रिक्त;
        पूर्णाङ्क r;

        r = getxattr_at_malloc(fd, मार्गः, name, at_flags, &v, /* ret_size= */ रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम parse_boolean(v);
}

पूर्णाङ्क getxattr_at_strv(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *name, पूर्णाङ्क at_flags, वर्ण ***ret_strv) {
        _cleanup_free_ वर्ण *nulstr = रिक्त;
        size_t nulstr_आकार;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_strv);

        r = getxattr_at_malloc(fd, मार्गः, name, at_flags, &nulstr, &nulstr_आकार);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_strv_free_ वर्ण **l = strv_parse_nulstr(nulstr, nulstr_आकार);
        यदि (!l)
                प्रत्यागम -ENOMEM;

        *ret_strv = TAKE_PTR(l);
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क listxattr_pinned_आन्तरिकम्(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क at_flags,
                bool by_procfs,
                वर्ण *buf,
                size_t आकार) {

        ssize_t n;

        प्रतिज्ञा(!मार्गः || !isempty(मार्गः));
        प्रतिज्ञा((fd >= 0) == !मार्गः);
        प्रतिज्ञा(मार्गः || FLAGS_समुच्चयः(at_flags, AT_रिक्तानि_मार्गः));
        प्रतिज्ञा(buf || आकार == 0);

        यदि (मार्गः)
                n = FLAGS_समुच्चयः(at_flags, AT_SYMLINK_FOLLOW) ? listxattr(मार्गः, buf, आकार)
                                                           : llistxattr(मार्गः, buf, आकार);
        अन्यथा
                n = by_procfs ? listxattr(FORMAT_PROC_FD_मार्गः(fd), buf, आकार)
                              : flistxattr(fd, buf, आकार);
        यदि (n < 0)
                प्रत्यागम -errno;

        प्रतिज्ञा(आकार == 0 || (size_t) n <= आकार);

        यदि (n > अधिकतम_पूर्णाङ्कः) /* We couldn't return this as 'int' anymore */
                प्रत्यागम -E2BIG;

        प्रत्यागम (पूर्णाङ्क) n;
}

पूर्णाङ्क listxattr_at_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क at_flags, वर्ण **ret) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_रिक्तानि_मार्गः)) == 0);
        प्रतिज्ञा(ret);

        /* This is to listxattr()/llistattr()/flistattr() what getxattr_at_malloc() is to getxattr()/… */

        r = normalize_and_maybe_pin_inode(&fd, &मार्गः, &at_flags, &opened_fd, &by_procfs);
        यदि (r < 0)
                प्रत्यागम r;

        size_t l = 100;
        प्रति (अचिह्नित n_attempts = 7;;) {
                _cleanup_free_ वर्ण *v = रिक्त;

                यदि (n_attempts == 0) /* If someone is racing against us, give up eventually */
                        प्रत्यागम -EBUSY;
                n_attempts--;

                v = new(वर्ण, l+1);
                यदि (!v)
                        प्रत्यागम -ENOMEM;

                l = MALLOC_ELEMENTSOF(v) - 1;

                r = listxattr_pinned_आन्तरिकम्(fd, मार्गः, at_flags, by_procfs, v, l);
                यदि (r >= 0) {
                        v[r] = 0; /* NUL terminate */
                        *ret = TAKE_PTR(v);
                        प्रत्यागम r;
                }
                यदि (r != -ERANGE)
                        प्रत्यागम r;

                r = listxattr_pinned_आन्तरिकम्(fd, मार्गः, at_flags, by_procfs, रिक्त, 0);
                यदि (r < 0)
                        प्रत्यागम r;

                l = (size_t) r;
        }
}

पूर्णाङ्क xsetxattr_पूर्णम्(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क at_flags,
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मानम्,
                size_t आकार,
                पूर्णाङ्क xattr_flags) {

        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_रिक्तानि_मार्गः)) == 0);
        प्रतिज्ञा(name);
        प्रतिज्ञा(मानम् || आकार == 0);

        यदि (आकार == SIZE_MAX)
                आकार = वाक्यदैर्घ्यम्(मानम्);

        /* Skip the write if the xattr already has the correct value, to avoid
         * unnecessary timestamp changes on the file. Only do this for plain
         * replace mode (xattr_flags == 0) — XATTR_CREATE callers expect
         * -EEXIST when the xattr already exists. */
        _cleanup_free_ वर्ण *old_मानम् = रिक्त;
        size_t old_आकार;

        यदि (xattr_flags == 0 &&
            getxattr_at_malloc(fd, मार्गः, name, at_flags, &old_मानम्, &old_आकार) >= 0 &&
            memcmp_nn(old_मानम्, old_आकार, मानम्, आकार) == 0)
                प्रत्यागम 0;

        यदि (have_xattrat && !isempty(मार्गः)) {
                संरचना xattr_तर्काः तर्काः = {
                        .मानम् = PTR_गन्तव्यम्_UINT64(मानम्),
                        .आकार = आकार,
                        .flags = xattr_flags,
                };

                r = RET_NERRNO(setxattrat(fd, मार्गः,
                                          at_flags_normalize_nofollow(at_flags),
                                          name,
                                          &तर्काः, परिमाण(तर्काः)));
                यदि (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        प्रत्यागम r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &मार्गः, &at_flags, &opened_fd, &by_procfs);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (मार्गः)
                r = FLAGS_समुच्चयः(at_flags, AT_SYMLINK_FOLLOW) ? setxattr(मार्गः, name, मानम्, आकार, xattr_flags)
                                                           : lsetxattr(मार्गः, name, मानम्, आकार, xattr_flags);
        अन्यथा
                r = by_procfs ? setxattr(FORMAT_PROC_FD_मार्गः(fd), name, मानम्, आकार, xattr_flags)
                              : fsetxattr(fd, name, मानम्, आकार, xattr_flags);
        यदि (r < 0)
                प्रत्यागम -errno;

        प्रत्यागम 0;
}

पूर्णाङ्क xsetxattr_strv(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क at_flags, ध्रुव वर्ण *name, वर्ण * ध्रुव *l) {
        _cleanup_free_ वर्ण *nulstr = रिक्त;
        size_t आकार;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        r = strv_make_nulstr(l, &nulstr, &आकार);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम xsetxattr_पूर्णम्(fd, मार्गः, at_flags, name, nulstr, आकार, /* xattr_flags= */ 0);
}

पूर्णाङ्क xremovexattr(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क at_flags, ध्रुव वर्ण *name) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_रिक्तानि_मार्गः)) == 0);
        प्रतिज्ञा(name);

        यदि (have_xattrat && !isempty(मार्गः)) {
                r = RET_NERRNO(removexattrat(fd, मार्गः,
                                             at_flags_normalize_nofollow(at_flags),
                                             name));
                यदि (r != -ENOSYS) /* No ERRNO_IS_NOT_SUPPORTED here, as EOPNOTSUPP denotes the fs doesn't
                                     support xattr */
                        प्रत्यागम r;

                have_xattrat = false;
        }

        _cleanup_close_ पूर्णाङ्क tfd = -EBADF;
        bool by_procfs;

        r = normalize_and_maybe_pin_inode(&fd, &मार्गः, &at_flags, &tfd, &by_procfs);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (मार्गः)
                r = FLAGS_समुच्चयः(at_flags, AT_SYMLINK_FOLLOW) ? removexattr(मार्गः, name)
                                                           : lremovexattr(मार्गः, name);
        अन्यथा
                r = by_procfs ? removexattr(FORMAT_PROC_FD_मार्गः(fd), name)
                              : fremovexattr(fd, name);
        यदि (r < 0)
                प्रत्यागम -errno;

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क parse_crtime(le64_t le, usec_t *ret) {
        usec_t u;

        प्रतिज्ञा(ret);

        assert_cc(परिमाण(usec_t) == परिमाण(uint64_t));
        assert_cc((usec_t) UINT64_MAX == USEC_INFINITY);

        u = (usec_t) le64toh(le);
        यदि (!timestamp_is_समुच्चयः(u))
                प्रत्यागम -EIO;

        *ret = u;
        प्रत्यागम 0;
}

पूर्णाङ्क getcrtime_at(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क at_flags,
                usec_t *ret) {

        _cleanup_free_ le64_t *le = रिक्त;
        संरचना statx sx;
        usec_t a, b;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);
        प्रतिज्ञा((at_flags & ~(AT_SYMLINK_FOLLOW|AT_रिक्तानि_मार्गः)) == 0);

        /* So here's the deal: the creation/birth time (crtime/btime) of a file is a relatively newly supported concept
         * on Linux (or more strictly speaking: a concept that only recently got supported in the API, it was
         * implemented on various file systems on the lower level since a while, but never was accessible). However, we
         * needed a concept like that for vacuuming algorithms and such, hence we emulated it via a user xattr for a
         * long time. Starting with Linux 4.11 there's statx() which exposes the timestamp to userspace for the first
         * time, where it is available. This function will read it, but it tries to keep some compatibility with older
         * systems: we try to read both the crtime/btime and the xattr, and then use whatever is older. After all the
         * concept is useful for determining how "old" a file really is, and hence using the older of the two makes
         * most sense. */

        r = xstatx_पूर्णम्(fd,
                        मार्गः,
                        at_flags_normalize_nofollow(at_flags)|AT_STATX_DONT_SYNC,
                        /* xstatx_flags= */ 0,
                        /* mandatory_mask= */ 0,
                        STATX_BTIME,
                        /* mandatory_attributes= */ 0,
                        &sx);
        यदि (r > 0 && sx.stx_btime.tv_sec != 0) /* > 0: all optional masks are supported */
                a = statx_timestamp_आरोपय(&sx.stx_btime);
        अन्यथा
                a = USEC_INFINITY;

        size_t le_आकार;
        r = getxattr_at_malloc(fd, मार्गः, "user.crtime_usec", at_flags, (वर्ण**) &le, &le_आकार);
        यदि (r >= 0) {
                यदि (le_आकार != परिमाण(*le))
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

        यदि (!timestamp_is_समुच्चयः(usec))
                usec = now(CLOCK_REALTIME);

        le = htole64((uint64_t) usec);
        प्रत्यागम xsetxattr_पूर्णम्(fd, /* path= */ रिक्त, AT_रिक्तानि_मार्गः,
                              "user.crtime_usec", (ध्रुव वर्ण*) &le, परिमाण(le),
                              /* xattr_flags= */ 0);
}

bool xattr_is_acl(ध्रुव वर्ण *name) {
        प्रत्यागम STR_अन्तर्_समुच्चयः(
                        ASSERT_PTR(name),
                        "system.posix_acl_access",
                        "system.posix_acl_default");
}

bool xattr_is_selinux(ध्रुव वर्ण *name) {
        प्रत्यागम streq(ASSERT_PTR(name), "security.selinux");
}
