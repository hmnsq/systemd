/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <linux/magic.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <threads.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "mountpoint-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "pidfd-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"

స్థిర thread_local పూర్ణాంకం have_pidfs = -1;

పూర్ణాంకం pidfd_check_pidfs(పూర్ణాంకం pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ఒకవేళ (have_pidfs >= 0)
                తిరిగి have_pidfs;

        _cleanup_close_ పూర్ణాంకం our_fd = -EBADF;
        ఒకవేళ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ఒకవేళ (our_fd < 0)
                        తిరిగి -errno;

                pid_fd = our_fd;
        }

        తిరిగి (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

పూర్ణాంకం pidfd_get_namespace(పూర్ణాంకం fd, గుర్తులేని దీర్ఘ ns_type_cmd) {
        స్థిర bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        ప్రతిజ్ఞా(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        ఒకవేళ (have_pidfs == 0 || !cached_supported)
                తిరిగి -EOPNOTSUPP;

        పూర్ణాంకం nsfd = ioctl(fd, ns_type_cmd, 0);
        ఒకవేళ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ఒకవేళ (లోపల_సమితి(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        తిరిగి -EOPNOTSUPP;
                }
                ఒకవేళ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        తిరిగి -ENOPKG;

                తిరిగి -errno;
        }

        తిరిగి nsfd;
}

పూర్ణాంకం pidfd_get_info(పూర్ణాంకం fd, నిర్మాణం pidfd_info *info) {
        స్థిర bool cached_supported = true;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(info);

        ఒకవేళ (have_pidfs == 0 || !cached_supported)
                తిరిగి -EOPNOTSUPP;

        ఒకవేళ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ఒకవేళ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        తిరిగి -EOPNOTSUPP;
                }

                తిరిగి -errno;
        }

        తిరిగి 0;
}

పూర్ణాంకం pidfd_info_ముసుగు_is_supported(uint64_t ముసుగు_3) {
        స్థిర thread_local uint64_t cached_ముసుగు = 0;
        స్థిర thread_local పూర్ణాంకం cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        పూర్ణాంకం r;

        ఒకవేళ (cached < 0)
                తిరిగి cached;

        ఒకవేళ (cached == 0) {
                _cleanup_close_ పూర్ణాంకం pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ఒకవేళ (r < 0)
                        తిరిగి r;

                నిర్మాణం pidfd_info info = {
                        .ముసుగు_3 = PIDFD_INFO_SUPPORTED_ముసుగు, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ఒకవేళ (r == -EOPNOTSUPP)
                        తిరిగి (cached = -EOPNOTSUPP);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (FLAGS_సమితి(info.ముసుగు_3, PIDFD_INFO_SUPPORTED_ముసుగు) &&
                    FLAGS_సమితి(info.supported_ముసుగు, PIDFD_INFO_SUPPORTED_ముసుగు))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_ముసుగు = info.supported_ముసుగు;
                లేకపోతే
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_ముసుగు = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        తిరిగి FLAGS_సమితి(cached_ముసుగు, ముసుగు_3);
}

స్థిర పూర్ణాంకం pidfd_get_pid_fdinfo(పూర్ణాంకం fd, pid_t *ret) {
        అక్షరం మార్గం[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(పూర్ణాంకం)];
        _cleanup_free_ అక్షరం *p = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        xsprintf(మార్గం, "/proc/self/fdinfo/%i", fd);

        r = get_proc_క్షేత్రం(మార్గం, "Pid", &p);
        ఒకవేళ (r == -ENOENT)
                తిరిగి -EBADF;
        ఒకవేళ (r == -ENODATA) /* not a pidfd? */
                తిరిగి -ENOTTY;
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (streq(p, "0"))
                తిరిగి -EREMOTE; /* PID is in foreign PID namespace? */
        ఒకవేళ (streq(p, "-1"))
                తిరిగి -ESRCH;   /* refers to reaped process? */

        తిరిగి parse_pid(p, ret);
}

స్థిర పూర్ణాంకం pidfd_get_pid_ioctl(పూర్ణాంకం fd, pid_t *ret) {
        నిర్మాణం pidfd_info info = { .ముసుగు_3 = PIDFD_INFO_PID };
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(FLAGS_సమితి(info.ముసుగు_3, PIDFD_INFO_PID));

        ఒకవేళ (ret)
                *ret = info.pid;
        తిరిగి 0;
}

పూర్ణాంకం pidfd_get_pid(పూర్ణాంకం fd, pid_t *ret) {
        పూర్ణాంకం r;

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

        ప్రతిజ్ఞా(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        ఒకవేళ (r != -EOPNOTSUPP)
                తిరిగి r;

        తిరిగి pidfd_get_pid_fdinfo(fd, ret);
}

పూర్ణాంకం pidfd_verify_pid(పూర్ణాంకం pidfd, pid_t pid) {
        pid_t current_pid;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidfd >= 0);
        ప్రతిజ్ఞా(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి current_pid != pid ? -ESRCH : 0;
}

పూర్ణాంకం pidfd_get_ppid(పూర్ణాంకం fd, pid_t *ret) {
        నిర్మాణం pidfd_info info = { .ముసుగు_3 = PIDFD_INFO_PID };
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(FLAGS_సమితి(info.ముసుగు_3, PIDFD_INFO_PID));

        ఒకవేళ (info.ppid == 0) /* See comments in pid_get_ppid() */
                తిరిగి -EADDRNOTAVAIL;

        ఒకవేళ (ret)
                *ret = info.ppid;
        తిరిగి 0;
}

పూర్ణాంకం pidfd_get_uid(పూర్ణాంకం fd, uid_t *ret) {
        నిర్మాణం pidfd_info info = { .ముసుగు_3 = PIDFD_INFO_CREDS };
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(FLAGS_సమితి(info.ముసుగు_3, PIDFD_INFO_CREDS));

        ఒకవేళ (ret)
                *ret = info.ruid;
        తిరిగి 0;
}

పూర్ణాంకం pidfd_get_cgroupid(పూర్ణాంకం fd, uint64_t *ret) {
        నిర్మాణం pidfd_info info = { .ముసుగు_3 = PIDFD_INFO_CGROUPID };
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(FLAGS_సమితి(info.ముసుగు_3, PIDFD_INFO_CGROUPID));

        ఒకవేళ (ret)
                *ret = info.cgroupid;
        తిరిగి 0;
}

పూర్ణాంకం pidfd_get_inode_id_impl(పూర్ణాంకం fd, uint64_t *ret) {
        స్థిర thread_local bool file_handle_supported = true;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ఒకవేళ (file_handle_supported) {
                r = fd_గమ్యం_handle_u64(fd, ret);
                ఒకవేళ (r >= 0 || is_name_గమ్యం_handle_at_fatal_error(r))
                        తిరిగి r;

                file_handle_supported = false;
        }

#ఒకవేళ SIZEOF_INO_T == 8
        నిర్మాణం stat st;
        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret)
                *ret = (uint64_t) st.st_ino;
        తిరిగి 0;

#లేదావేళ SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        తిరిగి -EOPNOTSUPP;
#లేకపోతే
#  error Unsupported ino_t కొలత
#అయితే_ముగింపు
}

పూర్ణాంకం pidfd_get_inode_id(పూర్ణాంకం fd, uint64_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EOPNOTSUPP;

        తిరిగి pidfd_get_inode_id_impl(fd, ret);
}

పూర్ణాంకం pidfd_get_inode_id_self_cached(uint64_t *ret) {
        స్థిర thread_local uint64_t cached = 0;
        స్థిర thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (initialized == getpid_cached()) {
                *ret = cached;
                తిరిగి 0;
        }
        ఒకవేళ (initialized < 0)
                తిరిగి initialized;

        _cleanup_close_ పూర్ణాంకం fd = pidfd_open(getpid_cached(), 0);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                తిరిగి (initialized = -EOPNOTSUPP);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = cached;
        initialized = getpid_cached();
        తిరిగి 0;
}
