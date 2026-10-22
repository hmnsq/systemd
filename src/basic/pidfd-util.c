/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/magic.h>
#ضمن <sys/ioctl.h>
#ضمن <threads.h>
#ضمن <unistd.h>

#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "mountpoint-util.h"
#ضمن "parse-util.h"
#ضمن "pidfd-util.h"
#ضمن "process-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"

ساكن thread_local صحيح have_pidfs = -1;

صحيح pidfd_check_pidfs(صحيح pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ܐܢ (have_pidfs >= 0)
                ܦܢܐ have_pidfs;

        _cleanup_close_ صحيح our_fd = -EBADF;
        ܐܢ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ܐܢ (our_fd < 0)
                        ܦܢܐ -errno;

                pid_fd = our_fd;
        }

        ܦܢܐ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

صحيح pidfd_get_namespace(صحيح fd, unsigned long ns_type_cmd) {
        ساكن bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        أكد(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        ܐܢ (have_pidfs == 0 || !cached_supported)
                ܦܢܐ -EOPNOTSUPP;

        صحيح nsfd = ioctl(fd, ns_type_cmd, 0);
        ܐܢ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ܐܢ (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ܦܢܐ -EOPNOTSUPP;
                }
                ܐܢ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ܦܢܐ -ENOPKG;

                ܦܢܐ -errno;
        }

        ܦܢܐ nsfd;
}

صحيح pidfd_get_info(صحيح fd, بنية pidfd_info *info) {
        ساكن bool cached_supported = true;

        أكد(fd >= 0);
        أكد(info);

        ܐܢ (have_pidfs == 0 || !cached_supported)
                ܦܢܐ -EOPNOTSUPP;

        ܐܢ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ܐܢ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ܦܢܐ -EOPNOTSUPP;
                }

                ܦܢܐ -errno;
        }

        ܦܢܐ 0;
}

صحيح pidfd_info_mask_is_supported(uint64_t mask) {
        ساكن thread_local uint64_t cached_mask = 0;
        ساكن thread_local صحيح cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        صحيح r;

        ܐܢ (cached < 0)
                ܦܢܐ cached;

        ܐܢ (cached == 0) {
                _cleanup_close_ صحيح pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ܐܢ (r < 0)
                        ܦܢܐ r;

                بنية pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ܐܢ (r == -EOPNOTSUPP)
                        ܦܢܐ (cached = -EOPNOTSUPP);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܐܢ (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                ܐܚܪܢܐ
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ܦܢܐ FLAGS_SET(cached_mask, mask);
}

ساكن صحيح pidfd_get_pid_fdinfo(صحيح fd, pid_t *ret) {
        حرف path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(صحيح)];
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        ܐܢ (r == -ENOENT)
                ܦܢܐ -EBADF;
        ܐܢ (r == -ENODATA) /* not a pidfd? */
                ܦܢܐ -ENOTTY;
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (streq(p, "0"))
                ܦܢܐ -EREMOTE; /* PID is in foreign PID namespace? */
        ܐܢ (streq(p, "-1"))
                ܦܢܐ -ESRCH;   /* refers to reaped process? */

        ܦܢܐ parse_pid(p, ret);
}

ساكن صحيح pidfd_get_pid_ioctl(صحيح fd, pid_t *ret) {
        بنية pidfd_info info = { .mask = PIDFD_INFO_PID };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ܐܢ (r < 0)
                ܦܢܐ r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ܐܢ (ret)
                *ret = info.pid;
        ܦܢܐ 0;
}

صحيح pidfd_get_pid(صحيح fd, pid_t *ret) {
        صحيح r;

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
        ܐܢ (r != -EOPNOTSUPP)
                ܦܢܐ r;

        ܦܢܐ pidfd_get_pid_fdinfo(fd, ret);
}

صحيح pidfd_verify_pid(صحيح pidfd, pid_t pid) {
        pid_t current_pid;
        صحيح r;

        أكد(pidfd >= 0);
        أكد(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ current_pid != pid ? -ESRCH : 0;
}

صحيح pidfd_get_ppid(صحيح fd, pid_t *ret) {
        بنية pidfd_info info = { .mask = PIDFD_INFO_PID };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ܐܢ (r < 0)
                ܦܢܐ r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ܐܢ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ܦܢܐ -EADDRNOTAVAIL;

        ܐܢ (ret)
                *ret = info.ppid;
        ܦܢܐ 0;
}

صحيح pidfd_get_uid(صحيح fd, uid_t *ret) {
        بنية pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ܐܢ (r < 0)
                ܦܢܐ r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        ܐܢ (ret)
                *ret = info.ruid;
        ܦܢܐ 0;
}

صحيح pidfd_get_cgroupid(صحيح fd, uint64_t *ret) {
        بنية pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ܐܢ (r < 0)
                ܦܢܐ r;

        أكد(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        ܐܢ (ret)
                *ret = info.cgroupid;
        ܦܢܐ 0;
}

صحيح pidfd_get_inode_id_impl(صحيح fd, uint64_t *ret) {
        ساكن thread_local bool file_handle_supported = true;
        صحيح r;

        أكد(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ܐܢ (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                ܐܢ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ܦܢܐ r;

                file_handle_supported = false;
        }

#ܐܢ SIZEOF_INO_T == 8
        بنية stat st;
        ܐܢ (fstat(fd, &st) < 0)
                ܦܢܐ -errno;

        ܐܢ (ret)
                *ret = (uint64_t) st.st_ino;
        ܦܢܐ 0;

#وإلاإذا SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ܦܢܐ -EOPNOTSUPP;
#ܐܚܪܢܐ
#  error Unsupported ino_t size
#endif
}

صحيح pidfd_get_inode_id(صحيح fd, uint64_t *ret) {
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r == 0)
                ܦܢܐ -EOPNOTSUPP;

        ܦܢܐ pidfd_get_inode_id_impl(fd, ret);
}

صحيح pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ساكن thread_local uint64_t cached = 0;
        ساكن thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        صحيح r;

        أكد(ret);

        ܐܢ (initialized == getpid_cached()) {
                *ret = cached;
                ܦܢܐ 0;
        }
        ܐܢ (initialized < 0)
                ܦܢܐ initialized;

        _cleanup_close_ صحيح fd = pidfd_open(getpid_cached(), 0);
        ܐܢ (fd < 0)
                ܦܢܐ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ܐܢ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ܦܢܐ (initialized = -EOPNOTSUPP);
        ܐܢ (r < 0)
                ܦܢܐ r;

        *ret = cached;
        initialized = getpid_cached();
        ܦܢܐ 0;
}
