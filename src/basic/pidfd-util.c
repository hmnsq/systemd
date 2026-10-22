/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/magic.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <threads.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "mountpoint-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "pidfd-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ସ୍ଥିର thread_local ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_pidfs = -1;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_check_pidfs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ଯଦି (have_pidfs >= 0)
                ଫେରନ୍ତୁ have_pidfs;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା our_fd = -EBADF;
        ଯଦି (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ଯଦି (our_fd < 0)
                        ଫେରନ୍ତୁ -errno;

                pid_fd = our_fd;
        }

        ଫେରନ୍ତୁ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned long ns_type_cmd) {
        ସ୍ଥିର bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        ଯଦି (have_pidfs == 0 || !cached_supported)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା nsfd = ioctl(fd, ns_type_cmd, 0);
        ଯଦି (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ଯଦି (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ଫେରନ୍ତୁ -EOPNOTSUPP;
                }
                ଯଦି (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ଫେରନ୍ତୁ -ENOPKG;

                ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ nsfd;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା pidfd_info *info) {
        ସ୍ଥିର bool cached_supported = true;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(info);

        ଯଦି (have_pidfs == 0 || !cached_supported)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        ଯଦି (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ଯଦି (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ଫେରନ୍ତୁ -EOPNOTSUPP;
                }

                ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_info_mask_is_supported(uint64_t mask) {
        ସ୍ଥିର thread_local uint64_t cached_mask = 0;
        ସ୍ଥିର thread_local ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (cached < 0)
                ଫେରନ୍ତୁ cached;

        ଯଦି (cached == 0) {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ସଂରଚନା pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ଯଦି (r == -EOPNOTSUPP)
                        ଫେରନ୍ତୁ (cached = -EOPNOTSUPP);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                ଅନ୍ୟଥା
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ଫେରନ୍ତୁ FLAGS_SET(cached_mask, mask);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_pid_fdinfo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid_t *ret) {
        ଅକ୍ଷର path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)];
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ -EBADF;
        ଯଦି (r == -ENODATA) /* not a pidfd? */
                ଫେରନ୍ତୁ -ENOTTY;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (streq(p, "0"))
                ଫେରନ୍ତୁ -EREMOTE; /* PID is in foreign PID namespace? */
        ଯଦି (streq(p, "-1"))
                ଫେରନ୍ତୁ -ESRCH;   /* refers to reaped process? */

        ଫେରନ୍ତୁ parse_pid(p, ret);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_pid_ioctl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid_t *ret) {
        ସଂରଚନା pidfd_info info = { .mask = PIDFD_INFO_PID };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ଯଦି (ret)
                *ret = info.pid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_pid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        ଯଦି (r != -EOPNOTSUPP)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pidfd_get_pid_fdinfo(fd, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_verify_pid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd, pid_t pid) {
        pid_t current_pid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidfd >= 0);
        ପ୍ରତିଜ୍ଞା(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ current_pid != pid ? -ESRCH : 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_ppid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid_t *ret) {
        ସଂରଚନା pidfd_info info = { .mask = PIDFD_INFO_PID };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ଯଦି (info.ppid == 0) /* See comments in pid_get_ppid() */
                ଫେରନ୍ତୁ -EADDRNOTAVAIL;

        ଯଦି (ret)
                *ret = info.ppid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_uid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uid_t *ret) {
        ସଂରଚନା pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        ଯଦି (ret)
                *ret = info.ruid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_cgroupid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ସଂରଚନା pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        ଯଦି (ret)
                *ret = info.cgroupid;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_inode_id_impl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ସ୍ଥିର thread_local bool file_handle_supported = true;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ଯଦି (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                ଯଦି (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ଫେରନ୍ତୁ r;

                file_handle_supported = false;
        }

#ଯଦି SIZEOF_INO_T == 8
        ସଂରଚନା stat st;
        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret)
                *ret = (uint64_t) st.st_ino;
        ଫେରନ୍ତୁ 0;

#ନଚେତଯଦି SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ଫେରନ୍ତୁ -EOPNOTSUPP;
#ଅନ୍ୟଥା
#  error Unsupported ino_t size
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_inode_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        ଫେରନ୍ତୁ pidfd_get_inode_id_impl(fd, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ସ୍ଥିର thread_local uint64_t cached = 0;
        ସ୍ଥିର thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (initialized == getpid_cached()) {
                *ret = cached;
                ଫେରନ୍ତୁ 0;
        }
        ଯଦି (initialized < 0)
                ଫେରନ୍ତୁ initialized;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = pidfd_open(getpid_cached(), 0);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ଫେରନ୍ତୁ (initialized = -EOPNOTSUPP);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = cached;
        initialized = getpid_cached();
        ଫେରନ୍ତୁ 0;
}
