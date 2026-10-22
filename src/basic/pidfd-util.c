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

static thread_local цео have_pidfs = -1;

цео pidfd_check_pidfs(цео pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ако (have_pidfs >= 0)
                врати have_pidfs;

        _cleanup_close_ цео our_fd = -EBADF;
        ако (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ако (our_fd < 0)
                        врати -errno;

                pid_fd = our_fd;
        }

        врати (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

цео pidfd_get_namespace(цео fd, unsigned long ns_type_cmd) {
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
        ако (have_pidfs == 0 || !cached_supported)
                врати -EOPNOTSUPP;

        цео nsfd = ioctl(fd, ns_type_cmd, 0);
        ако (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ако (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        врати -EOPNOTSUPP;
                }
                ако (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        врати -ENOPKG;

                врати -errno;
        }

        врати nsfd;
}

цео pidfd_get_info(цео fd, struct pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        ако (have_pidfs == 0 || !cached_supported)
                врати -EOPNOTSUPP;

        ако (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ако (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        врати -EOPNOTSUPP;
                }

                врати -errno;
        }

        врати 0;
}

цео pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local цео cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        цео r;

        ако (cached < 0)
                врати cached;

        ако (cached == 0) {
                _cleanup_close_ цео pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ако (r < 0)
                        врати r;

                struct pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ако (r == -EOPNOTSUPP)
                        врати (cached = -EOPNOTSUPP);
                ако (r < 0)
                        врати r;

                ако (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                инако
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        врати FLAGS_SET(cached_mask, mask);
}

static цео pidfd_get_pid_fdinfo(цео fd, pid_t *ret) {
        char path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(цео)];
        _cleanup_free_ char *p = NULL;
        цео r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        ако (r == -ENOENT)
                врати -EBADF;
        ако (r == -ENODATA) /* not a pidfd? */
                врати -ENOTTY;
        ако (r < 0)
                врати r;

        ако (streq(p, "0"))
                врати -EREMOTE; /* PID is in foreign PID namespace? */
        ако (streq(p, "-1"))
                врати -ESRCH;   /* refers to reaped process? */

        врати parse_pid(p, ret);
}

static цео pidfd_get_pid_ioctl(цео fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        цео r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ако (r < 0)
                врати r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ако (ret)
                *ret = info.pid;
        врати 0;
}

цео pidfd_get_pid(цео fd, pid_t *ret) {
        цео r;

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
        ако (r != -EOPNOTSUPP)
                врати r;

        врати pidfd_get_pid_fdinfo(fd, ret);
}

цео pidfd_verify_pid(цео pidfd, pid_t pid) {
        pid_t current_pid;
        цео r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ако (r < 0)
                врати r;

        врати current_pid != pid ? -ESRCH : 0;
}

цео pidfd_get_ppid(цео fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        цео r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ако (r < 0)
                врати r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ако (info.ppid == 0) /* See comments in pid_get_ppid() */
                врати -EADDRNOTAVAIL;

        ако (ret)
                *ret = info.ppid;
        врати 0;
}

цео pidfd_get_uid(цео fd, uid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        цео r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ако (r < 0)
                врати r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        ако (ret)
                *ret = info.ruid;
        врати 0;
}

цео pidfd_get_cgroupid(цео fd, uint64_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        цео r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ако (r < 0)
                врати r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        ако (ret)
                *ret = info.cgroupid;
        врати 0;
}

цео pidfd_get_inode_id_impl(цео fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        цео r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ако (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                ако (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        врати r;

                file_handle_supported = false;
        }

#ако SIZEOF_INO_T == 8
        struct stat st;
        ако (fstat(fd, &st) < 0)
                врати -errno;

        ако (ret)
                *ret = (uint64_t) st.st_ino;
        врати 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        врати -EOPNOTSUPP;
#инако
#  error Unsupported ino_t size
#endif
}

цео pidfd_get_inode_id(цео fd, uint64_t *ret) {
        цео r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ако (r < 0)
                врати r;
        ако (r == 0)
                врати -EOPNOTSUPP;

        врати pidfd_get_inode_id_impl(fd, ret);
}

цео pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        цео r;

        assert(ret);

        ако (initialized == getpid_cached()) {
                *ret = cached;
                врати 0;
        }
        ако (initialized < 0)
                врати initialized;

        _cleanup_close_ цео fd = pidfd_open(getpid_cached(), 0);
        ако (fd < 0)
                врати -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ако (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                врати (initialized = -EOPNOTSUPP);
        ако (r < 0)
                врати r;

        *ret = cached;
        initialized = getpid_cached();
        врати 0;
}
