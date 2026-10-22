/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <threads.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "mountpoint-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "pidfd-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಙ್ಕ have_pidfs = -1;

ಪೂರ್ಣಾಙ್ಕ pidfd_check_pidfs(ಪೂರ್ಣಾಙ್ಕ pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ಯದಿ (have_pidfs >= 0)
                ಪ್ರತ್ಯಾಗಮ have_pidfs;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ our_fd = -EBADF;
        ಯದಿ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ಯದಿ (our_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                pid_fd = our_fd;
        }

        ಪ್ರತ್ಯಾಗಮ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_namespace(ಪೂರ್ಣಾಙ್ಕ fd, unsigned long ns_type_cmd) {
        ಸ್ಥಿರ bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        ಯದಿ (have_pidfs == 0 || !cached_supported)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        ಪೂರ್ಣಾಙ್ಕ nsfd = ioctl(fd, ns_type_cmd, 0);
        ಯದಿ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ಯದಿ (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
                }
                ಯದಿ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ಪ್ರತ್ಯಾಗಮ -ENOPKG;

                ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ nsfd;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_info(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ pidfd_info *info) {
        ಸ್ಥಿರ bool cached_supported = true;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(info);

        ಯದಿ (have_pidfs == 0 || !cached_supported)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        ಯದಿ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ಯದಿ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
                }

                ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_info_mask_is_supported(uint64_t mask) {
        ಸ್ಥಿರ thread_local uint64_t cached_mask = 0;
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಙ್ಕ cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (cached < 0)
                ಪ್ರತ್ಯಾಗಮ cached;

        ಯದಿ (cached == 0) {
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಸಂರಚನಾ pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ಯದಿ (r == -EOPNOTSUPP)
                        ಪ್ರತ್ಯಾಗಮ (cached = -EOPNOTSUPP);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                ಅನ್ಯಥಾ
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(cached_mask, mask);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pidfd_get_pid_fdinfo(ಪೂರ್ಣಾಙ್ಕ fd, pid_t *ret) {
        ವರ್ಣ path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಙ್ಕ)];
        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ -EBADF;
        ಯದಿ (r == -ENODATA) /* not a pidfd? */
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (streq(p, "0"))
                ಪ್ರತ್ಯಾಗಮ -EREMOTE; /* PID is in foreign PID namespace? */
        ಯದಿ (streq(p, "-1"))
                ಪ್ರತ್ಯಾಗಮ -ESRCH;   /* refers to reaped process? */

        ಪ್ರತ್ಯಾಗಮ parse_pid(p, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ pidfd_get_pid_ioctl(ಪೂರ್ಣಾಙ್ಕ fd, pid_t *ret) {
        ಸಂರಚನಾ pidfd_info info = { .mask = PIDFD_INFO_PID };
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ಯದಿ (ret)
                *ret = info.pid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_pid(ಪೂರ್ಣಾಙ್ಕ fd, pid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

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

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        ಯದಿ (r != -EOPNOTSUPP)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ pidfd_get_pid_fdinfo(fd, ret);
}

ಪೂರ್ಣಾಙ್ಕ pidfd_verify_pid(ಪೂರ್ಣಾಙ್ಕ pidfd, pid_t pid) {
        pid_t current_pid;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidfd >= 0);
        ಪ್ರತಿಜ್ಞಾ(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ current_pid != pid ? -ESRCH : 0;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_ppid(ಪೂರ್ಣಾಙ್ಕ fd, pid_t *ret) {
        ಸಂರಚನಾ pidfd_info info = { .mask = PIDFD_INFO_PID };
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ಯದಿ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ಪ್ರತ್ಯಾಗಮ -EADDRNOTAVAIL;

        ಯದಿ (ret)
                *ret = info.ppid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_uid(ಪೂರ್ಣಾಙ್ಕ fd, uid_t *ret) {
        ಸಂರಚನಾ pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        ಯದಿ (ret)
                *ret = info.ruid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_cgroupid(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        ಸಂರಚನಾ pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        ಯದಿ (ret)
                *ret = info.cgroupid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_inode_id_impl(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        ಸ್ಥಿರ thread_local bool file_handle_supported = true;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ಯದಿ (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                ಯದಿ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ಪ್ರತ್ಯಾಗಮ r;

                file_handle_supported = false;
        }

#ಯದಿ SIZEOF_INO_T == 8
        ಸಂರಚನಾ stat st;
        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret)
                *ret = (uint64_t) st.st_ino;
        ಪ್ರತ್ಯಾಗಮ 0;

#ನೋಚೇದ್ಯದಿ SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;
#ಅನ್ಯಥಾ
#  error Unsupported ino_t size
#endif
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_inode_id(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        ಪ್ರತ್ಯಾಗಮ pidfd_get_inode_id_impl(fd, ret);
}

ಪೂರ್ಣಾಙ್ಕ pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ಸ್ಥಿರ thread_local uint64_t cached = 0;
        ಸ್ಥಿರ thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (initialized == getpid_cached()) {
                *ret = cached;
                ಪ್ರತ್ಯಾಗಮ 0;
        }
        ಯದಿ (initialized < 0)
                ಪ್ರತ್ಯಾಗಮ initialized;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = pidfd_open(getpid_cached(), 0);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ಯದಿ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ಪ್ರತ್ಯಾಗಮ (initialized = -EOPNOTSUPP);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = cached;
        initialized = getpid_cached();
        ಪ್ರತ್ಯಾಗಮ 0;
}
