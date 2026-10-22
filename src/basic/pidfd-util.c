/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/magic.h>
#include <sys/ioctl.h>
#include <threads.h>
#include <unistd.h>

#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "mountpoint-util.h"
#include "parse-util.h"
#include "pidfd-util.h"
#include "process-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"

static thread_local จำนวนเต็ม have_pidfs = -1;

จำนวนเต็ม pidfd_check_pidfs(จำนวนเต็ม pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ถ้า (have_pidfs >= 0)
                คืน have_pidfs;

        _cleanup_close_ จำนวนเต็ม our_fd = -EBADF;
        ถ้า (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ถ้า (our_fd < 0)
                        คืน -errno;

                pid_fd = our_fd;
        }

        คืน (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

จำนวนเต็ม pidfd_get_namespace(จำนวนเต็ม fd, unsigned long ns_type_cmd) {
        static bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        assert(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        ถ้า (have_pidfs == 0 || !cached_supported)
                คืน -EOPNOTSUPP;

        จำนวนเต็ม nsfd = ioctl(fd, ns_type_cmd, 0);
        ถ้า (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ถ้า (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        คืน -EOPNOTSUPP;
                }
                ถ้า (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        คืน -ENOPKG;

                คืน -errno;
        }

        คืน nsfd;
}

จำนวนเต็ม pidfd_get_info(จำนวนเต็ม fd, sa_nracanaa pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        ถ้า (have_pidfs == 0 || !cached_supported)
                คืน -EOPNOTSUPP;

        ถ้า (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ถ้า (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        คืน -EOPNOTSUPP;
                }

                คืน -errno;
        }

        คืน 0;
}

จำนวนเต็ม pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local จำนวนเต็ม cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        จำนวนเต็ม r;

        ถ้า (cached < 0)
                คืน cached;

        ถ้า (cached == 0) {
                _cleanup_close_ จำนวนเต็ม pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ถ้า (r < 0)
                        คืน r;

                sa_nracanaa pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ถ้า (r == -EOPNOTSUPP)
                        คืน (cached = -EOPNOTSUPP);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                มิฉะนั้น
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        คืน FLAGS_SET(cached_mask, mask);
}

static จำนวนเต็ม pidfd_get_pid_fdinfo(จำนวนเต็ม fd, pid_t *ret) {
        var_na path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(จำนวนเต็ม)];
        _cleanup_free_ var_na *p = NULL;
        จำนวนเต็ม r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        ถ้า (r == -ENOENT)
                คืน -EBADF;
        ถ้า (r == -ENODATA) /* not a pidfd? */
                คืน -ENOTTY;
        ถ้า (r < 0)
                คืน r;

        ถ้า (streq(p, "0"))
                คืน -EREMOTE; /* PID is in foreign PID namespace? */
        ถ้า (streq(p, "-1"))
                คืน -ESRCH;   /* refers to reaped process? */

        คืน parse_pid(p, ret);
}

static จำนวนเต็ม pidfd_get_pid_ioctl(จำนวนเต็ม fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ถ้า (r < 0)
                คืน r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ถ้า (ret)
                *ret = info.pid;
        คืน 0;
}

จำนวนเต็ม pidfd_get_pid(จำนวนเต็ม fd, pid_t *ret) {
        จำนวนเต็ม r;

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

        assert(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        ถ้า (r != -EOPNOTSUPP)
                คืน r;

        คืน pidfd_get_pid_fdinfo(fd, ret);
}

จำนวนเต็ม pidfd_verify_pid(จำนวนเต็ม pidfd, pid_t pid) {
        pid_t current_pid;
        จำนวนเต็ม r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ถ้า (r < 0)
                คืน r;

        คืน current_pid != pid ? -ESRCH : 0;
}

จำนวนเต็ม pidfd_get_ppid(จำนวนเต็ม fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ถ้า (r < 0)
                คืน r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ถ้า (info.ppid == 0) /* See comments in pid_get_ppid() */
                คืน -EADDRNOTAVAIL;

        ถ้า (ret)
                *ret = info.ppid;
        คืน 0;
}

จำนวนเต็ม pidfd_get_uid(จำนวนเต็ม fd, uid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ถ้า (r < 0)
                คืน r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        ถ้า (ret)
                *ret = info.ruid;
        คืน 0;
}

จำนวนเต็ม pidfd_get_cgroupid(จำนวนเต็ม fd, uint64_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ถ้า (r < 0)
                คืน r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        ถ้า (ret)
                *ret = info.cgroupid;
        คืน 0;
}

จำนวนเต็ม pidfd_get_inode_id_impl(จำนวนเต็ม fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        จำนวนเต็ม r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ถ้า (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                ถ้า (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        คืน r;

                file_handle_supported = false;
        }

#ถ้า SIZEOF_INO_T == 8
        sa_nracanaa stat st;
        ถ้า (fstat(fd, &st) < 0)
                คืน -errno;

        ถ้า (ret)
                *ret = (uint64_t) st.st_ino;
        คืน 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        คืน -EOPNOTSUPP;
#มิฉะนั้น
#  error Unsupported ino_t size
#endif
}

จำนวนเต็ม pidfd_get_inode_id(จำนวนเต็ม fd, uint64_t *ret) {
        จำนวนเต็ม r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r == 0)
                คืน -EOPNOTSUPP;

        คืน pidfd_get_inode_id_impl(fd, ret);
}

จำนวนเต็ม pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        จำนวนเต็ม r;

        assert(ret);

        ถ้า (initialized == getpid_cached()) {
                *ret = cached;
                คืน 0;
        }
        ถ้า (initialized < 0)
                คืน initialized;

        _cleanup_close_ จำนวนเต็ม fd = pidfd_open(getpid_cached(), 0);
        ถ้า (fd < 0)
                คืน -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ถ้า (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                คืน (initialized = -EOPNOTSUPP);
        ถ้า (r < 0)
                คืน r;

        *ret = cached;
        initialized = getpid_cached();
        คืน 0;
}
