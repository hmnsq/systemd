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

        إذا (have_pidfs >= 0)
                أرجع have_pidfs;

        _cleanup_close_ صحيح our_fd = -EBADF;
        إذا (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                إذا (our_fd < 0)
                        أرجع -errno;

                pid_fd = our_fd;
        }

        أرجع (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

صحيح pidfd_get_namespace(صحيح fd, بینښه اوږد ns_type_cmd) {
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
        إذا (have_pidfs == 0 || !cached_supported)
                أرجع -EOPNOTSUPP;

        صحيح nsfd = ioctl(fd, ns_type_cmd, 0);
        إذا (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                إذا (دننه_سېټ(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        أرجع -EOPNOTSUPP;
                }
                إذا (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        أرجع -ENOPKG;

                أرجع -errno;
        }

        أرجع nsfd;
}

صحيح pidfd_get_info(صحيح fd, بنية pidfd_info *info) {
        ساكن bool cached_supported = true;

        أكد(fd >= 0);
        أكد(info);

        إذا (have_pidfs == 0 || !cached_supported)
                أرجع -EOPNOTSUPP;

        إذا (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                إذا (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        أرجع -EOPNOTSUPP;
                }

                أرجع -errno;
        }

        أرجع 0;
}

صحيح pidfd_info_ماسک_is_supported(uint64_t ماسک_3) {
        ساكن thread_local uint64_t cached_ماسک = 0;
        ساكن thread_local صحيح cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        صحيح r;

        إذا (cached < 0)
                أرجع cached;

        إذا (cached == 0) {
                _cleanup_close_ صحيح pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                إذا (r < 0)
                        أرجع r;

                بنية pidfd_info info = {
                        .ماسک_3 = PIDFD_INFO_SUPPORTED_ماسک, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                إذا (r == -EOPNOTSUPP)
                        أرجع (cached = -EOPNOTSUPP);
                إذا (r < 0)
                        أرجع r;

                إذا (FLAGS_سېټ(info.ماسک_3, PIDFD_INFO_SUPPORTED_ماسک) &&
                    FLAGS_سېټ(info.supported_ماسک, PIDFD_INFO_SUPPORTED_ماسک))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_ماسک = info.supported_ماسک;
                وإلا
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_ماسک = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        أرجع FLAGS_سېټ(cached_ماسک, ماسک_3);
}

ساكن صحيح pidfd_get_pid_fdinfo(صحيح fd, pid_t *ret) {
        حرف لار[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(صحيح)];
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(fd >= 0);

        xsprintf(لار, "/proc/self/fdinfo/%i", fd);

        r = get_proc_ډګر(لار, "Pid", &p);
        إذا (r == -ENOENT)
                أرجع -EBADF;
        إذا (r == -ENODATA) /* not a pidfd? */
                أرجع -ENOTTY;
        إذا (r < 0)
                أرجع r;

        إذا (streq(p, "0"))
                أرجع -EREMOTE; /* PID is in foreign PID namespace? */
        إذا (streq(p, "-1"))
                أرجع -ESRCH;   /* refers to reaped process? */

        أرجع parse_pid(p, ret);
}

ساكن صحيح pidfd_get_pid_ioctl(صحيح fd, pid_t *ret) {
        بنية pidfd_info info = { .ماسک_3 = PIDFD_INFO_PID };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        إذا (r < 0)
                أرجع r;

        أكد(FLAGS_سېټ(info.ماسک_3, PIDFD_INFO_PID));

        إذا (ret)
                *ret = info.pid;
        أرجع 0;
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
        إذا (r != -EOPNOTSUPP)
                أرجع r;

        أرجع pidfd_get_pid_fdinfo(fd, ret);
}

صحيح pidfd_verify_pid(صحيح pidfd, pid_t pid) {
        pid_t current_pid;
        صحيح r;

        أكد(pidfd >= 0);
        أكد(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        إذا (r < 0)
                أرجع r;

        أرجع current_pid != pid ? -ESRCH : 0;
}

صحيح pidfd_get_ppid(صحيح fd, pid_t *ret) {
        بنية pidfd_info info = { .ماسک_3 = PIDFD_INFO_PID };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        إذا (r < 0)
                أرجع r;

        أكد(FLAGS_سېټ(info.ماسک_3, PIDFD_INFO_PID));

        إذا (info.ppid == 0) /* See comments in pid_get_ppid() */
                أرجع -EADDRNOTAVAIL;

        إذا (ret)
                *ret = info.ppid;
        أرجع 0;
}

صحيح pidfd_get_uid(صحيح fd, uid_t *ret) {
        بنية pidfd_info info = { .ماسک_3 = PIDFD_INFO_CREDS };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        إذا (r < 0)
                أرجع r;

        أكد(FLAGS_سېټ(info.ماسک_3, PIDFD_INFO_CREDS));

        إذا (ret)
                *ret = info.ruid;
        أرجع 0;
}

صحيح pidfd_get_cgroupid(صحيح fd, uint64_t *ret) {
        بنية pidfd_info info = { .ماسک_3 = PIDFD_INFO_CGROUPID };
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_get_info(fd, &info);
        إذا (r < 0)
                أرجع r;

        أكد(FLAGS_سېټ(info.ماسک_3, PIDFD_INFO_CGROUPID));

        إذا (ret)
                *ret = info.cgroupid;
        أرجع 0;
}

صحيح pidfd_get_inode_id_impl(صحيح fd, uint64_t *ret) {
        ساكن thread_local bool file_handle_supported = true;
        صحيح r;

        أكد(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        إذا (file_handle_supported) {
                r = fd_منزل_handle_u64(fd, ret);
                إذا (r >= 0 || is_name_منزل_handle_at_fatal_error(r))
                        أرجع r;

                file_handle_supported = false;
        }

#إذا SIZEOF_INO_T == 8
        بنية stat st;
        إذا (fstat(fd, &st) < 0)
                أرجع -errno;

        إذا (ret)
                *ret = (uint64_t) st.st_ino;
        أرجع 0;

#وإلاإذا SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        أرجع -EOPNOTSUPP;
#وإلا
#  error Unsupported ino_t اندازه
#د_که_پای
}

صحيح pidfd_get_inode_id(صحيح fd, uint64_t *ret) {
        صحيح r;

        أكد(fd >= 0);

        r = pidfd_check_pidfs(fd);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EOPNOTSUPP;

        أرجع pidfd_get_inode_id_impl(fd, ret);
}

صحيح pidfd_get_inode_id_self_cached(uint64_t *ret) {
        ساكن thread_local uint64_t cached = 0;
        ساكن thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        صحيح r;

        أكد(ret);

        إذا (initialized == getpid_cached()) {
                *ret = cached;
                أرجع 0;
        }
        إذا (initialized < 0)
                أرجع initialized;

        _cleanup_close_ صحيح fd = pidfd_open(getpid_cached(), 0);
        إذا (fd < 0)
                أرجع -errno;

        r = pidfd_get_inode_id(fd, &cached);
        إذا (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                أرجع (initialized = -EOPNOTSUPP);
        إذا (r < 0)
                أرجع r;

        *ret = cached;
        initialized = getpid_cached();
        أرجع 0;
}
