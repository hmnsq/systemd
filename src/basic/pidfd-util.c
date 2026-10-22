/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/magic.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "mountpoint-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "pidfd-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"

நிலையான thread_local முழு have_pidfs = -1;

முழு pidfd_check_pidfs(முழு pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        எனில் (have_pidfs >= 0)
                திரும்பு have_pidfs;

        _cleanup_close_ முழு our_fd = -EBADF;
        எனில் (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                எனில் (our_fd < 0)
                        திரும்பு -errno;

                pid_fd = our_fd;
        }

        திரும்பு (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

முழு pidfd_get_namespace(முழு fd, unsigned long ns_type_cmd) {
        நிலையான bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        ப்ரதிஜ்ஞா(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        எனில் (have_pidfs == 0 || !cached_supported)
                திரும்பு -EOPNOTSUPP;

        முழு nsfd = ioctl(fd, ns_type_cmd, 0);
        எனில் (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                எனில் (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        திரும்பு -EOPNOTSUPP;
                }
                எனில் (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        திரும்பு -ENOPKG;

                திரும்பு -errno;
        }

        திரும்பு nsfd;
}

முழு pidfd_get_info(முழு fd, கட்டமைப்பு pidfd_info *info) {
        நிலையான bool cached_supported = true;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(info);

        எனில் (have_pidfs == 0 || !cached_supported)
                திரும்பு -EOPNOTSUPP;

        எனில் (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                எனில் (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        திரும்பு -EOPNOTSUPP;
                }

                திரும்பு -errno;
        }

        திரும்பு 0;
}

முழு pidfd_info_mask_is_supported(uint64_t mask) {
        நிலையான thread_local uint64_t cached_mask = 0;
        நிலையான thread_local முழு cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        முழு r;

        எனில் (cached < 0)
                திரும்பு cached;

        எனில் (cached == 0) {
                _cleanup_close_ முழு pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                எனில் (r < 0)
                        திரும்பு r;

                கட்டமைப்பு pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                எனில் (r == -EOPNOTSUPP)
                        திரும்பு (cached = -EOPNOTSUPP);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                இல்லையெனில்
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        திரும்பு FLAGS_SET(cached_mask, mask);
}

நிலையான முழு pidfd_get_pid_fdinfo(முழு fd, pid_t *ret) {
        எழுத்து path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(முழு)];
        _cleanup_free_ எழுத்து *p = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        எனில் (r == -ENOENT)
                திரும்பு -EBADF;
        எனில் (r == -ENODATA) /* not a pidfd? */
                திரும்பு -ENOTTY;
        எனில் (r < 0)
                திரும்பு r;

        எனில் (streq(p, "0"))
                திரும்பு -EREMOTE; /* PID is in foreign PID namespace? */
        எனில் (streq(p, "-1"))
                திரும்பு -ESRCH;   /* refers to reaped process? */

        திரும்பு parse_pid(p, ret);
}

நிலையான முழு pidfd_get_pid_ioctl(முழு fd, pid_t *ret) {
        கட்டமைப்பு pidfd_info info = { .mask = PIDFD_INFO_PID };
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = pidfd_get_info(fd, &info);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        எனில் (ret)
                *ret = info.pid;
        திரும்பு 0;
}

முழு pidfd_get_pid(முழு fd, pid_t *ret) {
        முழு r;

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

        ப்ரதிஜ்ஞா(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        எனில் (r != -EOPNOTSUPP)
                திரும்பு r;

        திரும்பு pidfd_get_pid_fdinfo(fd, ret);
}

முழு pidfd_verify_pid(முழு pidfd, pid_t pid) {
        pid_t current_pid;
        முழு r;

        ப்ரதிஜ்ஞா(pidfd >= 0);
        ப்ரதிஜ்ஞா(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு current_pid != pid ? -ESRCH : 0;
}

முழு pidfd_get_ppid(முழு fd, pid_t *ret) {
        கட்டமைப்பு pidfd_info info = { .mask = PIDFD_INFO_PID };
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = pidfd_get_info(fd, &info);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        எனில் (info.ppid == 0) /* See comments in pid_get_ppid() */
                திரும்பு -EADDRNOTAVAIL;

        எனில் (ret)
                *ret = info.ppid;
        திரும்பு 0;
}

முழு pidfd_get_uid(முழு fd, uid_t *ret) {
        கட்டமைப்பு pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = pidfd_get_info(fd, &info);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        எனில் (ret)
                *ret = info.ruid;
        திரும்பு 0;
}

முழு pidfd_get_cgroupid(முழு fd, uint64_t *ret) {
        கட்டமைப்பு pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = pidfd_get_info(fd, &info);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        எனில் (ret)
                *ret = info.cgroupid;
        திரும்பு 0;
}

முழு pidfd_get_inode_id_impl(முழு fd, uint64_t *ret) {
        நிலையான thread_local bool file_handle_supported = true;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        எனில் (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                எனில் (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        திரும்பு r;

                file_handle_supported = false;
        }

#எனில் SIZEOF_INO_T == 8
        கட்டமைப்பு stat st;
        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        எனில் (ret)
                *ret = (uint64_t) st.st_ino;
        திரும்பு 0;

#அல்லதெனில் SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        திரும்பு -EOPNOTSUPP;
#இல்லையெனில்
#  error Unsupported ino_t size
#endif
}

முழு pidfd_get_inode_id(முழு fd, uint64_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = pidfd_check_pidfs(fd);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EOPNOTSUPP;

        திரும்பு pidfd_get_inode_id_impl(fd, ret);
}

முழு pidfd_get_inode_id_self_cached(uint64_t *ret) {
        நிலையான thread_local uint64_t cached = 0;
        நிலையான thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        எனில் (initialized == getpid_cached()) {
                *ret = cached;
                திரும்பு 0;
        }
        எனில் (initialized < 0)
                திரும்பு initialized;

        _cleanup_close_ முழு fd = pidfd_open(getpid_cached(), 0);
        எனில் (fd < 0)
                திரும்பு -errno;

        r = pidfd_get_inode_id(fd, &cached);
        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                திரும்பு (initialized = -EOPNOTSUPP);
        எனில் (r < 0)
                திரும்பு r;

        *ret = cached;
        initialized = getpid_cached();
        திரும்பு 0;
}
