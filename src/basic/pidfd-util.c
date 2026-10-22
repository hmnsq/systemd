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

ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಂಕ have_pidfs = -1;

ಪೂರ್ಣಾಂಕ pidfd_check_pidfs(ಪೂರ್ಣಾಂಕ pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ಆದರೆ (have_pidfs >= 0)
                ಹಿಂತಿರುಗಿ have_pidfs;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ our_fd = -EBADF;
        ಆದರೆ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ಆದರೆ (our_fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                pid_fd = our_fd;
        }

        ಹಿಂತಿರುಗಿ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ಪೂರ್ಣಾಂಕ pidfd_get_namespace(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ns_type_cmd) {
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
        ಆದರೆ (have_pidfs == 0 || !cached_supported)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        ಪೂರ್ಣಾಂಕ nsfd = ioctl(fd, ns_type_cmd, 0);
        ಆದರೆ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ಆದರೆ (ಒಳ_ಗಣ(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
                }
                ಆದರೆ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ಹಿಂತಿರುಗಿ -ENOPKG;

                ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ nsfd;
}

ಪೂರ್ಣಾಂಕ pidfd_get_info(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ pidfd_info *info) {
        ಸ್ಥಿರ bool cached_supported = true;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(info);

        ಆದರೆ (have_pidfs == 0 || !cached_supported)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        ಆದರೆ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ಆದರೆ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
                }

                ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidfd_info_ಮುಖವಾಡ_is_supported(uint64_t ಮುಖವಾಡ_3) {
        ಸ್ಥಿರ thread_local uint64_t cached_ಮುಖವಾಡ = 0;
        ಸ್ಥಿರ thread_local ಪೂರ್ಣಾಂಕ cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (cached < 0)
                ಹಿಂತಿರುಗಿ cached;

        ಆದರೆ (cached == 0) {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ರಚನೆ pidfd_info info = {
                        .ಮುಖವಾಡ_3 = PIDFD_INFO_SUPPORTED_ಮುಖವಾಡ, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ಆದರೆ (r == -EOPNOTSUPP)
                        ಹಿಂತಿರುಗಿ (cached = -EOPNOTSUPP);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (FLAGS_ಗಣ(info.ಮುಖವಾಡ_3, PIDFD_INFO_SUPPORTED_ಮುಖವಾಡ) &&
                    FLAGS_ಗಣ(info.supported_ಮುಖವಾಡ, PIDFD_INFO_SUPPORTED_ಮುಖವಾಡ))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_ಮುಖವಾಡ = info.supported_ಮುಖವಾಡ;
                ಇಲ್ಲದಿದ್ದರೆ
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_ಮುಖವಾಡ = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(cached_ಮುಖವಾಡ, ಮುಖವಾಡ_3);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pidfd_get_pid_fdinfo(ಪೂರ್ಣಾಂಕ fd, pid_t *ret) {
        ಅಕ್ಷರ ಮಾರ್ಗ[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ಪೂರ್ಣಾಂಕ)];
        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        xsprintf(ಮಾರ್ಗ, "/proc/self/fdinfo/%i", fd);

        r = get_proc_ಕ್ಷೇತ್ರ(ಮಾರ್ಗ, "Pid", &p);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ -EBADF;
        ಆದರೆ (r == -ENODATA) /* not a pidfd? */
                ಹಿಂತಿರುಗಿ -ENOTTY;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (streq(p, "0"))
                ಹಿಂತಿರುಗಿ -EREMOTE; /* PID is in foreign PID namespace? */
        ಆದರೆ (streq(p, "-1"))
                ಹಿಂತಿರುಗಿ -ESRCH;   /* refers to reaped process? */

        ಹಿಂತಿರುಗಿ parse_pid(p, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pidfd_get_pid_ioctl(ಪೂರ್ಣಾಂಕ fd, pid_t *ret) {
        ರಚನೆ pidfd_info info = { .ಮುಖವಾಡ_3 = PIDFD_INFO_PID };
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(info.ಮುಖವಾಡ_3, PIDFD_INFO_PID));

        ಆದರೆ (ret)
                *ret = info.pid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidfd_get_pid(ಪೂರ್ಣಾಂಕ fd, pid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

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
        ಆದರೆ (r != -EOPNOTSUPP)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pidfd_get_pid_fdinfo(fd, ret);
}

ಪೂರ್ಣಾಂಕ pidfd_verify_pid(ಪೂರ್ಣಾಂಕ pidfd, pid_t pid) {
        pid_t current_pid;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidfd >= 0);
        ಪ್ರತಿಜ್ಞಾ(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ current_pid != pid ? -ESRCH : 0;
}

ಪೂರ್ಣಾಂಕ pidfd_get_ppid(ಪೂರ್ಣಾಂಕ fd, pid_t *ret) {
        ರಚನೆ pidfd_info info = { .ಮುಖವಾಡ_3 = PIDFD_INFO_PID };
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(info.ಮುಖವಾಡ_3, PIDFD_INFO_PID));

        ಆದರೆ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ಹಿಂತಿರುಗಿ -EADDRNOTAVAIL;

        ಆದರೆ (ret)
                *ret = info.ppid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidfd_get_uid(ಪೂರ್ಣಾಂಕ fd, uid_t *ret) {
        ರಚನೆ pidfd_info info = { .ಮುಖವಾಡ_3 = PIDFD_INFO_CREDS };
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(info.ಮುಖವಾಡ_3, PIDFD_INFO_CREDS));

        ಆದರೆ (ret)
                *ret = info.ruid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidfd_get_cgroupid(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ರಚನೆ pidfd_info info = { .ಮುಖವಾಡ_3 = PIDFD_INFO_CGROUPID };
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(FLAGS_ಗಣ(info.ಮುಖವಾಡ_3, PIDFD_INFO_CGROUPID));

        ಆದರೆ (ret)
                *ret = info.cgroupid;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pidfd_get_inode_id_impl(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ಸ್ಥಿರ thread_local bool file_handle_supported = true;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ಆದರೆ (file_handle_supported) {
                r = fd_ಗಮ್ಯ_handle_u64(fd, ret);
                ಆದರೆ (r >= 0 || is_name_ಗಮ್ಯ_handle_at_fatal_error(r))
                        ಹಿಂತಿರುಗಿ r;

                file_handle_supported = false;
        }

#ಆದರೆ SIZEOF_INO_T == 8
        ರಚನೆ stat st;
        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret)
                *ret = (uint64_t) st.st_ino;
        ಹಿಂತಿರುಗಿ 0;

#ಅಥವಾಆದರೆ SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;
#ಇಲ್ಲದಿದ್ದರೆ
#  error Unsupported ino_t ಗಾತ್ರ_2
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ pidfd_get_inode_id(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        ಹಿಂತಿರುಗಿ pidfd_get_inode_id_impl(fd, ret);
}

ಪೂರ್ಣಾಂಕ pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ಸ್ಥಿರ thread_local uint64_t cached = 0;
        ಸ್ಥಿರ thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (initialized == getpid_cached()) {
                *ret = cached;
                ಹಿಂತಿರುಗಿ 0;
        }
        ಆದರೆ (initialized < 0)
                ಹಿಂತಿರುಗಿ initialized;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = pidfd_open(getpid_cached(), 0);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ಹಿಂತಿರುಗಿ (initialized = -EOPNOTSUPP);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = cached;
        initialized = getpid_cached();
        ಹಿಂತಿರುಗಿ 0;
}
