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

static thread_local ціле have_pidfs = -1;

ціле pidfd_check_pidfs(ціле pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        якщо (have_pidfs >= 0)
                повернути have_pidfs;

        _cleanup_close_ ціле our_fd = -EBADF;
        якщо (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                якщо (our_fd < 0)
                        повернути -errno;

                pid_fd = our_fd;
        }

        повернути (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ціле pidfd_get_namespace(ціле fd, unsigned long ns_type_cmd) {
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
        якщо (have_pidfs == 0 || !cached_supported)
                повернути -EOPNOTSUPP;

        ціле nsfd = ioctl(fd, ns_type_cmd, 0);
        якщо (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                якщо (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        повернути -EOPNOTSUPP;
                }
                якщо (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        повернути -ENOPKG;

                повернути -errno;
        }

        повернути nsfd;
}

ціле pidfd_get_info(ціле fd, struct pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        якщо (have_pidfs == 0 || !cached_supported)
                повернути -EOPNOTSUPP;

        якщо (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                якщо (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        повернути -EOPNOTSUPP;
                }

                повернути -errno;
        }

        повернути 0;
}

ціле pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local ціле cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ціле r;

        якщо (cached < 0)
                повернути cached;

        якщо (cached == 0) {
                _cleanup_close_ ціле pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                якщо (r < 0)
                        повернути r;

                struct pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                якщо (r == -EOPNOTSUPP)
                        повернути (cached = -EOPNOTSUPP);
                якщо (r < 0)
                        повернути r;

                якщо (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                інакше
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        повернути FLAGS_SET(cached_mask, mask);
}

static ціле pidfd_get_pid_fdinfo(ціле fd, pid_t *ret) {
        char path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ціле)];
        _cleanup_free_ char *p = NULL;
        ціле r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        якщо (r == -ENOENT)
                повернути -EBADF;
        якщо (r == -ENODATA) /* not a pidfd? */
                повернути -ENOTTY;
        якщо (r < 0)
                повернути r;

        якщо (streq(p, "0"))
                повернути -EREMOTE; /* PID is in foreign PID namespace? */
        якщо (streq(p, "-1"))
                повернути -ESRCH;   /* refers to reaped process? */

        повернути parse_pid(p, ret);
}

static ціле pidfd_get_pid_ioctl(ціле fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        ціле r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        якщо (r < 0)
                повернути r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        якщо (ret)
                *ret = info.pid;
        повернути 0;
}

ціле pidfd_get_pid(ціле fd, pid_t *ret) {
        ціле r;

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
        якщо (r != -EOPNOTSUPP)
                повернути r;

        повернути pidfd_get_pid_fdinfo(fd, ret);
}

ціле pidfd_verify_pid(ціле pidfd, pid_t pid) {
        pid_t current_pid;
        ціле r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        якщо (r < 0)
                повернути r;

        повернути current_pid != pid ? -ESRCH : 0;
}

ціле pidfd_get_ppid(ціле fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        ціле r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        якщо (r < 0)
                повернути r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        якщо (info.ppid == 0) /* See comments in pid_get_ppid() */
                повернути -EADDRNOTAVAIL;

        якщо (ret)
                *ret = info.ppid;
        повернути 0;
}

ціле pidfd_get_uid(ціле fd, uid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        ціле r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        якщо (r < 0)
                повернути r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        якщо (ret)
                *ret = info.ruid;
        повернути 0;
}

ціле pidfd_get_cgroupid(ціле fd, uint64_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        ціле r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        якщо (r < 0)
                повернути r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        якщо (ret)
                *ret = info.cgroupid;
        повернути 0;
}

ціле pidfd_get_inode_id_impl(ціле fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        ціле r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        якщо (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                якщо (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        повернути r;

                file_handle_supported = false;
        }

#якщо SIZEOF_INO_T == 8
        struct stat st;
        якщо (fstat(fd, &st) < 0)
                повернути -errno;

        якщо (ret)
                *ret = (uint64_t) st.st_ino;
        повернути 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        повернути -EOPNOTSUPP;
#інакше
#  error Unsupported ino_t size
#endif
}

ціле pidfd_get_inode_id(ціле fd, uint64_t *ret) {
        ціле r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EOPNOTSUPP;

        повернути pidfd_get_inode_id_impl(fd, ret);
}

ціле pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ціле r;

        assert(ret);

        якщо (initialized == getpid_cached()) {
                *ret = cached;
                повернути 0;
        }
        якщо (initialized < 0)
                повернути initialized;

        _cleanup_close_ ціле fd = pidfd_open(getpid_cached(), 0);
        якщо (fd < 0)
                повернути -errno;

        r = pidfd_get_inode_id(fd, &cached);
        якщо (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                повернути (initialized = -EOPNOTSUPP);
        якщо (r < 0)
                повернути r;

        *ret = cached;
        initialized = getpid_cached();
        повернути 0;
}
