/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <linux/magic.h>
#অন্তর্ভাৱ <sys/ioctl.h>
#অন্তর্ভাৱ <threads.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "mountpoint-util.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "pidfd-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "stdio-util.h"
#অন্তর্ভাৱ "string-util.h"

স্থির thread_local পূর্ণাঙ্ক have_pidfs = -1;

পূর্ণাঙ্ক pidfd_check_pidfs(পূর্ণাঙ্ক pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        যদি (have_pidfs >= 0)
                ঘূৰাই have_pidfs;

        _cleanup_close_ পূর্ণাঙ্ক our_fd = -EBADF;
        যদি (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                যদি (our_fd < 0)
                        ঘূৰাই -errno;

                pid_fd = our_fd;
        }

        ঘূৰাই (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

পূর্ণাঙ্ক pidfd_get_namespace(পূর্ণাঙ্ক fd, unsigned long ns_type_cmd) {
        স্থির bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        প্রতিজ্ঞা(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        যদি (have_pidfs == 0 || !cached_supported)
                ঘূৰাই -EOPNOTSUPP;

        পূর্ণাঙ্ক nsfd = ioctl(fd, ns_type_cmd, 0);
        যদি (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                যদি (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ঘূৰাই -EOPNOTSUPP;
                }
                যদি (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ঘূৰাই -ENOPKG;

                ঘূৰাই -errno;
        }

        ঘূৰাই nsfd;
}

পূর্ণাঙ্ক pidfd_get_info(পূর্ণাঙ্ক fd, সংরচনা pidfd_info *info) {
        স্থির bool cached_supported = true;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(info);

        যদি (have_pidfs == 0 || !cached_supported)
                ঘূৰাই -EOPNOTSUPP;

        যদি (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                যদি (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ঘূৰাই -EOPNOTSUPP;
                }

                ঘূৰাই -errno;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক pidfd_info_mask_is_supported(uint64_t mask) {
        স্থির thread_local uint64_t cached_mask = 0;
        স্থির thread_local পূর্ণাঙ্ক cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        পূর্ণাঙ্ক r;

        যদি (cached < 0)
                ঘূৰাই cached;

        যদি (cached == 0) {
                _cleanup_close_ পূর্ণাঙ্ক pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                যদি (r < 0)
                        ঘূৰাই r;

                সংরচনা pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                যদি (r == -EOPNOTSUPP)
                        ঘূৰাই (cached = -EOPNOTSUPP);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                অন্যথা
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ঘূৰাই FLAGS_SET(cached_mask, mask);
}

স্থির পূর্ণাঙ্ক pidfd_get_pid_fdinfo(পূর্ণাঙ্ক fd, pid_t *ret) {
        ৱর্ণ path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(পূর্ণাঙ্ক)];
        _cleanup_free_ ৱর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        যদি (r == -ENOENT)
                ঘূৰাই -EBADF;
        যদি (r == -ENODATA) /* not a pidfd? */
                ঘূৰাই -ENOTTY;
        যদি (r < 0)
                ঘূৰাই r;

        যদি (streq(p, "0"))
                ঘূৰাই -EREMOTE; /* PID is in foreign PID namespace? */
        যদি (streq(p, "-1"))
                ঘূৰাই -ESRCH;   /* refers to reaped process? */

        ঘূৰাই parse_pid(p, ret);
}

স্থির পূর্ণাঙ্ক pidfd_get_pid_ioctl(পূর্ণাঙ্ক fd, pid_t *ret) {
        সংরচনা pidfd_info info = { .mask = PIDFD_INFO_PID };
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = pidfd_get_info(fd, &info);
        যদি (r < 0)
                ঘূৰাই r;

        প্রতিজ্ঞা(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        যদি (ret)
                *ret = info.pid;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক pidfd_get_pid(পূর্ণাঙ্ক fd, pid_t *ret) {
        পূর্ণাঙ্ক r;

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

        প্রতিজ্ঞা(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        যদি (r != -EOPNOTSUPP)
                ঘূৰাই r;

        ঘূৰাই pidfd_get_pid_fdinfo(fd, ret);
}

পূর্ণাঙ্ক pidfd_verify_pid(পূর্ণাঙ্ক pidfd, pid_t pid) {
        pid_t current_pid;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(pidfd >= 0);
        প্রতিজ্ঞা(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই current_pid != pid ? -ESRCH : 0;
}

পূর্ণাঙ্ক pidfd_get_ppid(পূর্ণাঙ্ক fd, pid_t *ret) {
        সংরচনা pidfd_info info = { .mask = PIDFD_INFO_PID };
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = pidfd_get_info(fd, &info);
        যদি (r < 0)
                ঘূৰাই r;

        প্রতিজ্ঞা(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        যদি (info.ppid == 0) /* See comments in pid_get_ppid() */
                ঘূৰাই -EADDRNOTAVAIL;

        যদি (ret)
                *ret = info.ppid;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক pidfd_get_uid(পূর্ণাঙ্ক fd, uid_t *ret) {
        সংরচনা pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = pidfd_get_info(fd, &info);
        যদি (r < 0)
                ঘূৰাই r;

        প্রতিজ্ঞা(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        যদি (ret)
                *ret = info.ruid;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক pidfd_get_cgroupid(পূর্ণাঙ্ক fd, uint64_t *ret) {
        সংরচনা pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = pidfd_get_info(fd, &info);
        যদি (r < 0)
                ঘূৰাই r;

        প্রতিজ্ঞা(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        যদি (ret)
                *ret = info.cgroupid;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক pidfd_get_inode_id_impl(পূর্ণাঙ্ক fd, uint64_t *ret) {
        স্থির thread_local bool file_handle_supported = true;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        যদি (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                যদি (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ঘূৰাই r;

                file_handle_supported = false;
        }

#যদি SIZEOF_INO_T == 8
        সংরচনা stat st;
        যদি (fstat(fd, &st) < 0)
                ঘূৰাই -errno;

        যদি (ret)
                *ret = (uint64_t) st.st_ino;
        ঘূৰাই 0;

#নোচেদ্যদি SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ঘূৰাই -EOPNOTSUPP;
#অন্যথা
#  error Unsupported ino_t size
#endif
}

পূর্ণাঙ্ক pidfd_get_inode_id(পূর্ণাঙ্ক fd, uint64_t *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = pidfd_check_pidfs(fd);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -EOPNOTSUPP;

        ঘূৰাই pidfd_get_inode_id_impl(fd, ret);
}

পূর্ণাঙ্ক pidfd_get_inode_id_self_cached(uint64_t *ret) {
        স্থির thread_local uint64_t cached = 0;
        স্থির thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        যদি (initialized == getpid_cached()) {
                *ret = cached;
                ঘূৰাই 0;
        }
        যদি (initialized < 0)
                ঘূৰাই initialized;

        _cleanup_close_ পূর্ণাঙ্ক fd = pidfd_open(getpid_cached(), 0);
        যদি (fd < 0)
                ঘূৰাই -errno;

        r = pidfd_get_inode_id(fd, &cached);
        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ঘূৰাই (initialized = -EOPNOTSUPP);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = cached;
        initialized = getpid_cached();
        ঘূৰাই 0;
}
