/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/magic.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <threads.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "mountpoint-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "pidfd-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"

स्थिर thread_local पूर्णाङ्क have_pidfs = -1;

पूर्णाङ्क pidfd_check_pidfs(पूर्णाङ्क pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        यदि (have_pidfs >= 0)
                प्रत्यागम have_pidfs;

        _cleanup_close_ पूर्णाङ्क our_fd = -EBADF;
        यदि (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                यदि (our_fd < 0)
                        प्रत्यागम -त्रुटि_सं;

                pid_fd = our_fd;
        }

        प्रत्यागम (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

पूर्णाङ्क pidfd_get_namespace(पूर्णाङ्क fd, अचिन्हित दीर्घ ns_type_cmd) {
        स्थिर bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        प्रतिज्ञा(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        यदि (have_pidfs == 0 || !cached_supported)
                प्रत्यागम -EOPNOTSUPP;

        पूर्णाङ्क nsfd = ioctl(fd, ns_type_cmd, 0);
        यदि (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                यदि (IN_SET(त्रुटि_सं, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        प्रत्यागम -EOPNOTSUPP;
                }
                यदि (त्रुटि_सं == EOPNOTSUPP) /* Translate to something more recognizable */
                        प्रत्यागम -ENOPKG;

                प्रत्यागम -त्रुटि_सं;
        }

        प्रत्यागम nsfd;
}

पूर्णाङ्क pidfd_get_info(पूर्णाङ्क fd, संरचना pidfd_info *info) {
        स्थिर bool cached_supported = true;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(info);

        यदि (have_pidfs == 0 || !cached_supported)
                प्रत्यागम -EOPNOTSUPP;

        यदि (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                यदि (ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं)) {
                        cached_supported = false;
                        प्रत्यागम -EOPNOTSUPP;
                }

                प्रत्यागम -त्रुटि_सं;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क pidfd_info_mask_is_supported(uint64_t mask) {
        स्थिर thread_local uint64_t cached_mask = 0;
        स्थिर thread_local पूर्णाङ्क cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        पूर्णाङ्क r;

        यदि (cached < 0)
                प्रत्यागम cached;

        यदि (cached == 0) {
                _cleanup_close_ पूर्णाङ्क pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                यदि (r < 0)
                        प्रत्यागम r;

                संरचना pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                यदि (r == -EOPNOTSUPP)
                        प्रत्यागम (cached = -EOPNOTSUPP);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                अन्यथा
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        प्रत्यागम FLAGS_SET(cached_mask, mask);
}

स्थिर पूर्णाङ्क pidfd_get_pid_fdinfo(पूर्णाङ्क fd, pid_t *ret) {
        वर्ण path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(पूर्णाङ्क)];
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        यदि (r == -ENOENT)
                प्रत्यागम -EBADF;
        यदि (r == -ENODATA) /* not a pidfd? */
                प्रत्यागम -ENOTTY;
        यदि (r < 0)
                प्रत्यागम r;

        यदि (streq(p, "0"))
                प्रत्यागम -EREMOTE; /* PID is in foreign PID namespace? */
        यदि (streq(p, "-1"))
                प्रत्यागम -ESRCH;   /* refers to reaped process? */

        प्रत्यागम parse_pid(p, ret);
}

स्थिर पूर्णाङ्क pidfd_get_pid_ioctl(पूर्णाङ्क fd, pid_t *ret) {
        संरचना pidfd_info info = { .mask = PIDFD_INFO_PID };
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = pidfd_get_info(fd, &info);
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        यदि (ret)
                *ret = info.pid;
        प्रत्यागम 0;
}

पूर्णाङ्क pidfd_get_pid(पूर्णाङ्क fd, pid_t *ret) {
        पूर्णाङ्क r;

        /* Converts a pidfd into a pid. We try ioctl(PIDFD_GET_INFO) (kernel 6.13+) first,
         * /proc/self/fdinfo/ as fallback. Well known errors:
         *
         *    -EBADF   → fd invalid
         *    -ESRCH   → fd valid, but process is already reaped
         *
         * pidfd_get_pid_fdinfo() might additionally fail for other reasons:
         *
         *    -ENOSYS  → /proc/ not mounted
         *    -ENOTTY  → fd valid, but not a pidfd
         *    -EREMOTE → fd valid, but pid is in another namespace we cannot translate to the local one
         *               (when using PIDFD_GET_INFO this is indistinguishable from -ESRCH)
         */

        प्रतिज्ञा(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        यदि (r != -EOPNOTSUPP)
                प्रत्यागम r;

        प्रत्यागम pidfd_get_pid_fdinfo(fd, ret);
}

पूर्णाङ्क pidfd_verify_pid(पूर्णाङ्क pidfd, pid_t pid) {
        pid_t current_pid;
        पूर्णाङ्क r;

        प्रतिज्ञा(pidfd >= 0);
        प्रतिज्ञा(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम current_pid != pid ? -ESRCH : 0;
}

पूर्णाङ्क pidfd_get_ppid(पूर्णाङ्क fd, pid_t *ret) {
        संरचना pidfd_info info = { .mask = PIDFD_INFO_PID };
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = pidfd_get_info(fd, &info);
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        यदि (info.ppid == 0) /* See comments in pid_get_ppid() */
                प्रत्यागम -EADDRNOTAVAIL;

        यदि (ret)
                *ret = info.ppid;
        प्रत्यागम 0;
}

पूर्णाङ्क pidfd_get_uid(पूर्णाङ्क fd, uid_t *ret) {
        संरचना pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = pidfd_get_info(fd, &info);
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        यदि (ret)
                *ret = info.ruid;
        प्रत्यागम 0;
}

पूर्णाङ्क pidfd_get_cgroupid(पूर्णाङ्क fd, uint64_t *ret) {
        संरचना pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = pidfd_get_info(fd, &info);
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        यदि (ret)
                *ret = info.cgroupid;
        प्रत्यागम 0;
}

पूर्णाङ्क pidfd_get_inode_id_impl(पूर्णाङ्क fd, uint64_t *ret) {
        स्थिर thread_local bool file_handle_supported = true;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        यदि (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                यदि (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        प्रत्यागम r;

                file_handle_supported = false;
        }

#यदि SIZEOF_INO_T == 8
        संरचना stat st;
        यदि (ख_स्थिति(fd, &st) < 0)
                प्रत्यागम -त्रुटि_सं;

        यदि (ret)
                *ret = (uint64_t) st.st_ino;
        प्रत्यागम 0;

#नोचेद्यदि SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        प्रत्यागम -EOPNOTSUPP;
#अन्यथा
#  error Unsupported ino_t size
#endif
}

पूर्णाङ्क pidfd_get_inode_id(पूर्णाङ्क fd, uint64_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = pidfd_check_pidfs(fd);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EOPNOTSUPP;

        प्रत्यागम pidfd_get_inode_id_impl(fd, ret);
}

पूर्णाङ्क pidfd_get_inode_id_self_cached(uint64_t *ret) {
        स्थिर thread_local uint64_t cached = 0;
        स्थिर thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        यदि (initialized == getpid_cached()) {
                *ret = cached;
                प्रत्यागम 0;
        }
        यदि (initialized < 0)
                प्रत्यागम initialized;

        _cleanup_close_ पूर्णाङ्क fd = pidfd_open(getpid_cached(), 0);
        यदि (fd < 0)
                प्रत्यागम -त्रुटि_सं;

        r = pidfd_get_inode_id(fd, &cached);
        यदि (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                प्रत्यागम (initialized = -EOPNOTSUPP);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = cached;
        initialized = getpid_cached();
        प्रत्यागम 0;
}
