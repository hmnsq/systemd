/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/magic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mountpoint-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidfd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਸਥਿਰ thread_local ਪੂਰਨ have_pidfs = -1;

ਪੂਰਨ pidfd_check_pidfs(ਪੂਰਨ pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ਜੇ (have_pidfs >= 0)
                ਵਾਪਸ have_pidfs;

        _cleanup_close_ ਪੂਰਨ our_fd = -EBADF;
        ਜੇ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ਜੇ (our_fd < 0)
                        ਵਾਪਸ -errno;

                pid_fd = our_fd;
        }

        ਵਾਪਸ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ਪੂਰਨ pidfd_get_namespace(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ns_type_cmd) {
        ਸਥਿਰ bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        ਜੇ (have_pidfs == 0 || !cached_supported)
                ਵਾਪਸ -EOPNOTSUPP;

        ਪੂਰਨ nsfd = ioctl(fd, ns_type_cmd, 0);
        ਜੇ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ਵਾਪਸ -EOPNOTSUPP;
                }
                ਜੇ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ਵਾਪਸ -ENOPKG;

                ਵਾਪਸ -errno;
        }

        ਵਾਪਸ nsfd;
}

ਪੂਰਨ pidfd_get_info(ਪੂਰਨ fd, ਬਣਤਰ pidfd_info *info) {
        ਸਥਿਰ bool cached_supported = true;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(info);

        ਜੇ (have_pidfs == 0 || !cached_supported)
                ਵਾਪਸ -EOPNOTSUPP;

        ਜੇ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ਜੇ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ਵਾਪਸ -EOPNOTSUPP;
                }

                ਵਾਪਸ -errno;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ pidfd_info_ਨਕਾਬ_is_supported(uint64_t ਨਕਾਬ_3) {
        ਸਥਿਰ thread_local uint64_t cached_ਨਕਾਬ = 0;
        ਸਥਿਰ thread_local ਪੂਰਨ cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ਪੂਰਨ r;

        ਜੇ (cached < 0)
                ਵਾਪਸ cached;

        ਜੇ (cached == 0) {
                _cleanup_close_ ਪੂਰਨ pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਬਣਤਰ pidfd_info info = {
                        .ਨਕਾਬ_3 = PIDFD_INFO_SUPPORTED_ਨਕਾਬ, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ਜੇ (r == -EOPNOTSUPP)
                        ਵਾਪਸ (cached = -EOPNOTSUPP);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (FLAGS_ਸੈੱਟ(info.ਨਕਾਬ_3, PIDFD_INFO_SUPPORTED_ਨਕਾਬ) &&
                    FLAGS_ਸੈੱਟ(info.supported_ਨਕਾਬ, PIDFD_INFO_SUPPORTED_ਨਕਾਬ))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_ਨਕਾਬ = info.supported_ਨਕਾਬ;
                ਨਹੀਂਤਾਂ
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_ਨਕਾਬ = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ਵਾਪਸ FLAGS_ਸੈੱਟ(cached_ਨਕਾਬ, ਨਕਾਬ_3);
}

ਸਥਿਰ ਪੂਰਨ pidfd_get_pid_fdinfo(ਪੂਰਨ fd, pid_t *ret) {
        ਅੱਖਰ ਰਸਤਾ[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ਪੂਰਨ)];
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        xsprintf(ਰਸਤਾ, "/proc/self/fdinfo/%i", fd);

        r = get_proc_ਖੇਤਰ(ਰਸਤਾ, "Pid", &p);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ -EBADF;
        ਜੇ (r == -ENODATA) /* not a pidfd? */
                ਵਾਪਸ -ENOTTY;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (streq(p, "0"))
                ਵਾਪਸ -EREMOTE; /* PID is in foreign PID namespace? */
        ਜੇ (streq(p, "-1"))
                ਵਾਪਸ -ESRCH;   /* refers to reaped process? */

        ਵਾਪਸ parse_pid(p, ret);
}

ਸਥਿਰ ਪੂਰਨ pidfd_get_pid_ioctl(ਪੂਰਨ fd, pid_t *ret) {
        ਬਣਤਰ pidfd_info info = { .ਨਕਾਬ_3 = PIDFD_INFO_PID };
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(info.ਨਕਾਬ_3, PIDFD_INFO_PID));

        ਜੇ (ret)
                *ret = info.pid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidfd_get_pid(ਪੂਰਨ fd, pid_t *ret) {
        ਪੂਰਨ r;

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

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        ਜੇ (r != -EOPNOTSUPP)
                ਵਾਪਸ r;

        ਵਾਪਸ pidfd_get_pid_fdinfo(fd, ret);
}

ਪੂਰਨ pidfd_verify_pid(ਪੂਰਨ pidfd, pid_t pid) {
        pid_t current_pid;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidfd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ current_pid != pid ? -ESRCH : 0;
}

ਪੂਰਨ pidfd_get_ppid(ਪੂਰਨ fd, pid_t *ret) {
        ਬਣਤਰ pidfd_info info = { .ਨਕਾਬ_3 = PIDFD_INFO_PID };
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(info.ਨਕਾਬ_3, PIDFD_INFO_PID));

        ਜੇ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ਵਾਪਸ -EADDRNOTAVAIL;

        ਜੇ (ret)
                *ret = info.ppid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidfd_get_uid(ਪੂਰਨ fd, uid_t *ret) {
        ਬਣਤਰ pidfd_info info = { .ਨਕਾਬ_3 = PIDFD_INFO_CREDS };
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(info.ਨਕਾਬ_3, PIDFD_INFO_CREDS));

        ਜੇ (ret)
                *ret = info.ruid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidfd_get_cgroupid(ਪੂਰਨ fd, uint64_t *ret) {
        ਬਣਤਰ pidfd_info info = { .ਨਕਾਬ_3 = PIDFD_INFO_CGROUPID };
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(FLAGS_ਸੈੱਟ(info.ਨਕਾਬ_3, PIDFD_INFO_CGROUPID));

        ਜੇ (ret)
                *ret = info.cgroupid;
        ਵਾਪਸ 0;
}

ਪੂਰਨ pidfd_get_inode_id_impl(ਪੂਰਨ fd, uint64_t *ret) {
        ਸਥਿਰ thread_local bool file_handle_supported = true;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ਜੇ (file_handle_supported) {
                r = fd_ਮੰਜ਼ਿਲ_handle_u64(fd, ret);
                ਜੇ (r >= 0 || is_name_ਮੰਜ਼ਿਲ_handle_at_fatal_error(r))
                        ਵਾਪਸ r;

                file_handle_supported = false;
        }

#ਜੇ SIZEOF_INO_T == 8
        ਬਣਤਰ stat st;
        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret)
                *ret = (uint64_t) st.st_ino;
        ਵਾਪਸ 0;

#ਜਾਂਜੇ SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ਵਾਪਸ -EOPNOTSUPP;
#ਨਹੀਂਤਾਂ
#  error Unsupported ino_t ਮਾਪ
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ pidfd_get_inode_id(ਪੂਰਨ fd, uint64_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EOPNOTSUPP;

        ਵਾਪਸ pidfd_get_inode_id_impl(fd, ret);
}

ਪੂਰਨ pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ਸਥਿਰ thread_local uint64_t cached = 0;
        ਸਥਿਰ thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (initialized == getpid_cached()) {
                *ret = cached;
                ਵਾਪਸ 0;
        }
        ਜੇ (initialized < 0)
                ਵਾਪਸ initialized;

        _cleanup_close_ ਪੂਰਨ fd = pidfd_open(getpid_cached(), 0);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ਵਾਪਸ (initialized = -EOPNOTSUPP);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = cached;
        initialized = getpid_cached();
        ਵਾਪਸ 0;
}
