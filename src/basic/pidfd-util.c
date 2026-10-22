/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <linux/magic.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "mountpoint-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "pidfd-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"

സ്ഥിതം thread_local പൂർണ്ണസംഖ്യ have_pidfs = -1;

പൂർണ്ണസംഖ്യ pidfd_check_pidfs(പൂർണ്ണസംഖ്യ pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        എങ്കിൽ (have_pidfs >= 0)
                തിരികെ have_pidfs;

        _cleanup_close_ പൂർണ്ണസംഖ്യ our_fd = -EBADF;
        എങ്കിൽ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                എങ്കിൽ (our_fd < 0)
                        തിരികെ -errno;

                pid_fd = our_fd;
        }

        തിരികെ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

പൂർണ്ണസംഖ്യ pidfd_get_namespace(പൂർണ്ണസംഖ്യ fd, unsigned long ns_type_cmd) {
        സ്ഥിതം bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        പ്രതിജ്ഞാ(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        എങ്കിൽ (have_pidfs == 0 || !cached_supported)
                തിരികെ -EOPNOTSUPP;

        പൂർണ്ണസംഖ്യ nsfd = ioctl(fd, ns_type_cmd, 0);
        എങ്കിൽ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                എങ്കിൽ (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        തിരികെ -EOPNOTSUPP;
                }
                എങ്കിൽ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        തിരികെ -ENOPKG;

                തിരികെ -errno;
        }

        തിരികെ nsfd;
}

പൂർണ്ണസംഖ്യ pidfd_get_info(പൂർണ്ണസംഖ്യ fd, ഘടന pidfd_info *info) {
        സ്ഥിതം bool cached_supported = true;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(info);

        എങ്കിൽ (have_pidfs == 0 || !cached_supported)
                തിരികെ -EOPNOTSUPP;

        എങ്കിൽ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                എങ്കിൽ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        തിരികെ -EOPNOTSUPP;
                }

                തിരികെ -errno;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidfd_info_mask_is_supported(uint64_t mask) {
        സ്ഥിതം thread_local uint64_t cached_mask = 0;
        സ്ഥിതം thread_local പൂർണ്ണസംഖ്യ cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (cached < 0)
                തിരികെ cached;

        എങ്കിൽ (cached == 0) {
                _cleanup_close_ പൂർണ്ണസംഖ്യ pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ഘടന pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                എങ്കിൽ (r == -EOPNOTSUPP)
                        തിരികെ (cached = -EOPNOTSUPP);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                അല്ലെങ്കിൽ
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        തിരികെ FLAGS_SET(cached_mask, mask);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pidfd_get_pid_fdinfo(പൂർണ്ണസംഖ്യ fd, pid_t *ret) {
        അക്ഷരം path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(പൂർണ്ണസംഖ്യ)];
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ -EBADF;
        എങ്കിൽ (r == -ENODATA) /* not a pidfd? */
                തിരികെ -ENOTTY;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (streq(p, "0"))
                തിരികെ -EREMOTE; /* PID is in foreign PID namespace? */
        എങ്കിൽ (streq(p, "-1"))
                തിരികെ -ESRCH;   /* refers to reaped process? */

        തിരികെ parse_pid(p, ret);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pidfd_get_pid_ioctl(പൂർണ്ണസംഖ്യ fd, pid_t *ret) {
        ഘടന pidfd_info info = { .mask = PIDFD_INFO_PID };
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        എങ്കിൽ (ret)
                *ret = info.pid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidfd_get_pid(പൂർണ്ണസംഖ്യ fd, pid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

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

        പ്രതിജ്ഞാ(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        എങ്കിൽ (r != -EOPNOTSUPP)
                തിരികെ r;

        തിരികെ pidfd_get_pid_fdinfo(fd, ret);
}

പൂർണ്ണസംഖ്യ pidfd_verify_pid(പൂർണ്ണസംഖ്യ pidfd, pid_t pid) {
        pid_t current_pid;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidfd >= 0);
        പ്രതിജ്ഞാ(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ current_pid != pid ? -ESRCH : 0;
}

പൂർണ്ണസംഖ്യ pidfd_get_ppid(പൂർണ്ണസംഖ്യ fd, pid_t *ret) {
        ഘടന pidfd_info info = { .mask = PIDFD_INFO_PID };
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        എങ്കിൽ (info.ppid == 0) /* See comments in pid_get_ppid() */
                തിരികെ -EADDRNOTAVAIL;

        എങ്കിൽ (ret)
                *ret = info.ppid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidfd_get_uid(പൂർണ്ണസംഖ്യ fd, uid_t *ret) {
        ഘടന pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        എങ്കിൽ (ret)
                *ret = info.ruid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidfd_get_cgroupid(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        ഘടന pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = pidfd_get_info(fd, &info);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        എങ്കിൽ (ret)
                *ret = info.cgroupid;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pidfd_get_inode_id_impl(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        സ്ഥിതം thread_local bool file_handle_supported = true;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        എങ്കിൽ (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                എങ്കിൽ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        തിരികെ r;

                file_handle_supported = false;
        }

#എങ്കിൽ SIZEOF_INO_T == 8
        ഘടന stat st;
        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret)
                *ret = (uint64_t) st.st_ino;
        തിരികെ 0;

#അല്ലെങ്കിലെങ്കിൽ SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        തിരികെ -EOPNOTSUPP;
#അല്ലെങ്കിൽ
#  error Unsupported ino_t size
#endif
}

പൂർണ്ണസംഖ്യ pidfd_get_inode_id(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = pidfd_check_pidfs(fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EOPNOTSUPP;

        തിരികെ pidfd_get_inode_id_impl(fd, ret);
}

പൂർണ്ണസംഖ്യ pidfd_get_inode_id_self_cached(uint64_t *ret) {
        സ്ഥിതം thread_local uint64_t cached = 0;
        സ്ഥിതം thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (initialized == getpid_cached()) {
                *ret = cached;
                തിരികെ 0;
        }
        എങ്കിൽ (initialized < 0)
                തിരികെ initialized;

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = pidfd_open(getpid_cached(), 0);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        എങ്കിൽ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                തിരികെ (initialized = -EOPNOTSUPP);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = cached;
        initialized = getpid_cached();
        തിരികെ 0;
}
