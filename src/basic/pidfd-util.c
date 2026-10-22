/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <linux/magic.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <threads.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "mountpoint-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "pidfd-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"

સ્થિર thread_local પૂર્ણાંક have_pidfs = -1;

પૂર્ણાંક pidfd_check_pidfs(પૂર્ણાંક pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        જો (have_pidfs >= 0)
                પાછા have_pidfs;

        _cleanup_close_ પૂર્ણાંક our_fd = -EBADF;
        જો (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                જો (our_fd < 0)
                        પાછા -errno;

                pid_fd = our_fd;
        }

        પાછા (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

પૂર્ણાંક pidfd_get_namespace(પૂર્ણાંક fd, unsigned long ns_type_cmd) {
        સ્થિર bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        પ્રતિજ્ઞા(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        જો (have_pidfs == 0 || !cached_supported)
                પાછા -EOPNOTSUPP;

        પૂર્ણાંક nsfd = ioctl(fd, ns_type_cmd, 0);
        જો (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                જો (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        પાછા -EOPNOTSUPP;
                }
                જો (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        પાછા -ENOPKG;

                પાછા -errno;
        }

        પાછા nsfd;
}

પૂર્ણાંક pidfd_get_info(પૂર્ણાંક fd, સંરચના pidfd_info *info) {
        સ્થિર bool cached_supported = true;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(info);

        જો (have_pidfs == 0 || !cached_supported)
                પાછા -EOPNOTSUPP;

        જો (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                જો (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        પાછા -EOPNOTSUPP;
                }

                પાછા -errno;
        }

        પાછા 0;
}

પૂર્ણાંક pidfd_info_mask_is_supported(uint64_t mask) {
        સ્થિર thread_local uint64_t cached_mask = 0;
        સ્થિર thread_local પૂર્ણાંક cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        પૂર્ણાંક r;

        જો (cached < 0)
                પાછા cached;

        જો (cached == 0) {
                _cleanup_close_ પૂર્ણાંક pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                જો (r < 0)
                        પાછા r;

                સંરચના pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                જો (r == -EOPNOTSUPP)
                        પાછા (cached = -EOPNOTSUPP);
                જો (r < 0)
                        પાછા r;

                જો (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                નહીંતર
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        પાછા FLAGS_SET(cached_mask, mask);
}

સ્થિર પૂર્ણાંક pidfd_get_pid_fdinfo(પૂર્ણાંક fd, pid_t *ret) {
        અક્ષર path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(પૂર્ણાંક)];
        _cleanup_free_ અક્ષર *p = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        જો (r == -ENOENT)
                પાછા -EBADF;
        જો (r == -ENODATA) /* not a pidfd? */
                પાછા -ENOTTY;
        જો (r < 0)
                પાછા r;

        જો (streq(p, "0"))
                પાછા -EREMOTE; /* PID is in foreign PID namespace? */
        જો (streq(p, "-1"))
                પાછા -ESRCH;   /* refers to reaped process? */

        પાછા parse_pid(p, ret);
}

સ્થિર પૂર્ણાંક pidfd_get_pid_ioctl(પૂર્ણાંક fd, pid_t *ret) {
        સંરચના pidfd_info info = { .mask = PIDFD_INFO_PID };
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = pidfd_get_info(fd, &info);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        જો (ret)
                *ret = info.pid;
        પાછા 0;
}

પૂર્ણાંક pidfd_get_pid(પૂર્ણાંક fd, pid_t *ret) {
        પૂર્ણાંક r;

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

        પ્રતિજ્ઞા(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        જો (r != -EOPNOTSUPP)
                પાછા r;

        પાછા pidfd_get_pid_fdinfo(fd, ret);
}

પૂર્ણાંક pidfd_verify_pid(પૂર્ણાંક pidfd, pid_t pid) {
        pid_t current_pid;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidfd >= 0);
        પ્રતિજ્ઞા(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        જો (r < 0)
                પાછા r;

        પાછા current_pid != pid ? -ESRCH : 0;
}

પૂર્ણાંક pidfd_get_ppid(પૂર્ણાંક fd, pid_t *ret) {
        સંરચના pidfd_info info = { .mask = PIDFD_INFO_PID };
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = pidfd_get_info(fd, &info);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        જો (info.ppid == 0) /* See comments in pid_get_ppid() */
                પાછા -EADDRNOTAVAIL;

        જો (ret)
                *ret = info.ppid;
        પાછા 0;
}

પૂર્ણાંક pidfd_get_uid(પૂર્ણાંક fd, uid_t *ret) {
        સંરચના pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = pidfd_get_info(fd, &info);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        જો (ret)
                *ret = info.ruid;
        પાછા 0;
}

પૂર્ણાંક pidfd_get_cgroupid(પૂર્ણાંક fd, uint64_t *ret) {
        સંરચના pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = pidfd_get_info(fd, &info);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        જો (ret)
                *ret = info.cgroupid;
        પાછા 0;
}

પૂર્ણાંક pidfd_get_inode_id_impl(પૂર્ણાંક fd, uint64_t *ret) {
        સ્થિર thread_local bool file_handle_supported = true;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        જો (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                જો (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        પાછા r;

                file_handle_supported = false;
        }

#જો SIZEOF_INO_T == 8
        સંરચના stat st;
        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        જો (ret)
                *ret = (uint64_t) st.st_ino;
        પાછા 0;

#અથવાજો SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        પાછા -EOPNOTSUPP;
#નહીંતર
#  error Unsupported ino_t size
#endif
}

પૂર્ણાંક pidfd_get_inode_id(પૂર્ણાંક fd, uint64_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = pidfd_check_pidfs(fd);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EOPNOTSUPP;

        પાછા pidfd_get_inode_id_impl(fd, ret);
}

પૂર્ણાંક pidfd_get_inode_id_self_cached(uint64_t *ret) {
        સ્થિર thread_local uint64_t cached = 0;
        સ્થિર thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        જો (initialized == getpid_cached()) {
                *ret = cached;
                પાછા 0;
        }
        જો (initialized < 0)
                પાછા initialized;

        _cleanup_close_ પૂર્ણાંક fd = pidfd_open(getpid_cached(), 0);
        જો (fd < 0)
                પાછા -errno;

        r = pidfd_get_inode_id(fd, &cached);
        જો (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                પાછા (initialized = -EOPNOTSUPP);
        જો (r < 0)
                પાછા r;

        *ret = cached;
        initialized = getpid_cached();
        પાછા 0;
}
