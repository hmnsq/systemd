/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/magic.h>
#شامل <sys/ioctl.h>
#شامل <threads.h>
#شامل <unistd.h>

#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "mountpoint-util.h"
#شامل "parse-util.h"
#شامل "pidfd-util.h"
#شامل "process-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"

ایستا thread_local صحیح have_pidfs = -1;

صحیح pidfd_check_pidfs(صحیح pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        اگر (have_pidfs >= 0)
                بازگشت have_pidfs;

        _cleanup_close_ صحیح our_fd = -EBADF;
        اگر (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                اگر (our_fd < 0)
                        بازگشت -errno;

                pid_fd = our_fd;
        }

        بازگشت (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

صحیح pidfd_get_namespace(صحیح fd, unsigned long ns_type_cmd) {
        ایستا bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        أكد(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        اگر (have_pidfs == 0 || !cached_supported)
                بازگشت -EOPNOTSUPP;

        صحیح nsfd = ioctl(fd, ns_type_cmd, 0);
        اگر (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                اگر (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        بازگشت -EOPNOTSUPP;
                }
                اگر (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        بازگشت -ENOPKG;

                بازگشت -errno;
        }

        بازگشت nsfd;
}

صحیح pidfd_get_info(صحیح fd, ساختار pidfd_info *info) {
        ایستا bool cached_supported = true;

        أكد(fd >= 0);
        أكد(info);

        اگر (have_pidfs == 0 || !cached_supported)
                بازگشت -EOPNOTSUPP;

        اگر (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                اگر (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        بازگشت -EOPNOTSUPP;
                }

                بازگشت -errno;
        }

        بازگشت 0;
}

صحیح pidfd_info_mask_is_supported(uint64_t mask) {
        ایستا thread_local uint64_t cached_mask = 0;
        ایستا thread_local صحیح cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        صحیح r;

        اگر (cached < 0)
                بازگشت cached;

        اگر (cached == 0) {
                _cleanup_close_ صحیح pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                اگر (r < 0)
                        بازگشت r;

                ساختار pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                اگر (r == -EOPNOTSUPP)
                        بازگشت (cached = -EOPNOTSUPP);
                اگر (r < 0)
                        بازگشت r;

                اگر (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                وگرنه
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        بازگشت FLAGS_SET(cached_mask, mask);
}

ایستا صحیح pidfd_get_pid_fdinfo(صحیح fd, pid_t *ret) {
        نویسه path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(صحیح)];
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        اگر (r == -ENOENT)
                بازگشت -EBADF;
        اگر (r == -ENODATA) /* not a pidfd? */
                بازگشت -ENOTTY;
        اگر (r < 0)
                بازگشت r;

        اگر (streq(p, "0"))
                بازگشت -EREMOTE; /* PID is in foreign PID namespace? */
        اگر (streq(p, "-1"))
                بازگشت -ESRCH;   /* refers to reaped process? */

        بازگشت parse_pid(p, ret);
}

ایستا صحیح pidfd_get_pid_ioctl(صحیح fd, pid_t *ret) {
        ساختار pidfd_info info = { .mask = PIDFD_INFO_PID };
        صحیح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        اگر (r < 0)
                بازگشت r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        اگر (ret)
                *ret = info.pid;
        بازگشت 0;
}

صحیح pidfd_get_pid(صحیح fd, pid_t *ret) {
        صحیح r;

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

        أكد(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        اگر (r != -EOPNOTSUPP)
                بازگشت r;

        بازگشت pidfd_get_pid_fdinfo(fd, ret);
}

صحیح pidfd_verify_pid(صحیح pidfd, pid_t pid) {
        pid_t current_pid;
        صحیح r;

        أكد(pidfd >= 0);
        أكد(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        اگر (r < 0)
                بازگشت r;

        بازگشت current_pid != pid ? -ESRCH : 0;
}

صحیح pidfd_get_ppid(صحیح fd, pid_t *ret) {
        ساختار pidfd_info info = { .mask = PIDFD_INFO_PID };
        صحیح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        اگر (r < 0)
                بازگشت r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        اگر (info.ppid == 0) /* See comments in pid_get_ppid() */
                بازگشت -EADDRNOTAVAIL;

        اگر (ret)
                *ret = info.ppid;
        بازگشت 0;
}

صحیح pidfd_get_uid(صحیح fd, uid_t *ret) {
        ساختار pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        صحیح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        اگر (r < 0)
                بازگشت r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        اگر (ret)
                *ret = info.ruid;
        بازگشت 0;
}

صحیح pidfd_get_cgroupid(صحیح fd, uint64_t *ret) {
        ساختار pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        صحیح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        اگر (r < 0)
                بازگشت r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        اگر (ret)
                *ret = info.cgroupid;
        بازگشت 0;
}

صحیح pidfd_get_inode_id_impl(صحیح fd, uint64_t *ret) {
        ایستا thread_local bool file_handle_supported = true;
        صحیح r;

        أكد(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        اگر (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                اگر (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        بازگشت r;

                file_handle_supported = false;
        }

#اگر SIZEOF_INO_T == 8
        ساختار stat st;
        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        اگر (ret)
                *ret = (uint64_t) st.st_ino;
        بازگشت 0;

#وگرنهاگر SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        بازگشت -EOPNOTSUPP;
#وگرنه
#  error Unsupported ino_t size
#endif
}

صحیح pidfd_get_inode_id(صحیح fd, uint64_t *ret) {
        صحیح r;

        أكد(fd >= 0);

        r = pidfd_check_pidfs(fd);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EOPNOTSUPP;

        بازگشت pidfd_get_inode_id_impl(fd, ret);
}

صحیح pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ایستا thread_local uint64_t cached = 0;
        ایستا thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        صحیح r;

        أكد(ret);

        اگر (initialized == getpid_cached()) {
                *ret = cached;
                بازگشت 0;
        }
        اگر (initialized < 0)
                بازگشت initialized;

        _cleanup_close_ صحیح fd = pidfd_open(getpid_cached(), 0);
        اگر (fd < 0)
                بازگشت -errno;

        r = pidfd_get_inode_id(fd, &cached);
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                بازگشت (initialized = -EOPNOTSUPP);
        اگر (r < 0)
                بازگشت r;

        *ret = cached;
        initialized = getpid_cached();
        بازگشت 0;
}
