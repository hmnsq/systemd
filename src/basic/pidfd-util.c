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

static thread_local целое have_pidfs = -1;

целое pidfd_check_pidfs(целое pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        если (have_pidfs >= 0)
                вернуть have_pidfs;

        _cleanup_close_ целое our_fd = -EBADF;
        если (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                если (our_fd < 0)
                        вернуть -errno;

                pid_fd = our_fd;
        }

        вернуть (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

целое pidfd_get_namespace(целое fd, unsigned long ns_type_cmd) {
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
        если (have_pidfs == 0 || !cached_supported)
                вернуть -EOPNOTSUPP;

        целое nsfd = ioctl(fd, ns_type_cmd, 0);
        если (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                если (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        вернуть -EOPNOTSUPP;
                }
                если (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        вернуть -ENOPKG;

                вернуть -errno;
        }

        вернуть nsfd;
}

целое pidfd_get_info(целое fd, struct pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        если (have_pidfs == 0 || !cached_supported)
                вернуть -EOPNOTSUPP;

        если (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                если (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        вернуть -EOPNOTSUPP;
                }

                вернуть -errno;
        }

        вернуть 0;
}

целое pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local целое cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        целое r;

        если (cached < 0)
                вернуть cached;

        если (cached == 0) {
                _cleanup_close_ целое pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                если (r < 0)
                        вернуть r;

                struct pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                если (r == -EOPNOTSUPP)
                        вернуть (cached = -EOPNOTSUPP);
                если (r < 0)
                        вернуть r;

                если (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                иначе
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        вернуть FLAGS_SET(cached_mask, mask);
}

static целое pidfd_get_pid_fdinfo(целое fd, pid_t *ret) {
        char path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(целое)];
        _cleanup_free_ char *p = NULL;
        целое r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        если (r == -ENOENT)
                вернуть -EBADF;
        если (r == -ENODATA) /* not a pidfd? */
                вернуть -ENOTTY;
        если (r < 0)
                вернуть r;

        если (streq(p, "0"))
                вернуть -EREMOTE; /* PID is in foreign PID namespace? */
        если (streq(p, "-1"))
                вернуть -ESRCH;   /* refers to reaped process? */

        вернуть parse_pid(p, ret);
}

static целое pidfd_get_pid_ioctl(целое fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        целое r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        если (r < 0)
                вернуть r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        если (ret)
                *ret = info.pid;
        вернуть 0;
}

целое pidfd_get_pid(целое fd, pid_t *ret) {
        целое r;

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
        если (r != -EOPNOTSUPP)
                вернуть r;

        вернуть pidfd_get_pid_fdinfo(fd, ret);
}

целое pidfd_verify_pid(целое pidfd, pid_t pid) {
        pid_t current_pid;
        целое r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        если (r < 0)
                вернуть r;

        вернуть current_pid != pid ? -ESRCH : 0;
}

целое pidfd_get_ppid(целое fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        целое r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        если (r < 0)
                вернуть r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        если (info.ppid == 0) /* See comments in pid_get_ppid() */
                вернуть -EADDRNOTAVAIL;

        если (ret)
                *ret = info.ppid;
        вернуть 0;
}

целое pidfd_get_uid(целое fd, uid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        целое r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        если (r < 0)
                вернуть r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        если (ret)
                *ret = info.ruid;
        вернуть 0;
}

целое pidfd_get_cgroupid(целое fd, uint64_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        целое r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        если (r < 0)
                вернуть r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        если (ret)
                *ret = info.cgroupid;
        вернуть 0;
}

целое pidfd_get_inode_id_impl(целое fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        целое r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        если (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                если (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        вернуть r;

                file_handle_supported = false;
        }

#если SIZEOF_INO_T == 8
        struct stat st;
        если (fstat(fd, &st) < 0)
                вернуть -errno;

        если (ret)
                *ret = (uint64_t) st.st_ino;
        вернуть 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        вернуть -EOPNOTSUPP;
#иначе
#  error Unsupported ino_t size
#endif
}

целое pidfd_get_inode_id(целое fd, uint64_t *ret) {
        целое r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        если (r < 0)
                вернуть r;
        если (r == 0)
                вернуть -EOPNOTSUPP;

        вернуть pidfd_get_inode_id_impl(fd, ret);
}

целое pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        целое r;

        assert(ret);

        если (initialized == getpid_cached()) {
                *ret = cached;
                вернуть 0;
        }
        если (initialized < 0)
                вернуть initialized;

        _cleanup_close_ целое fd = pidfd_open(getpid_cached(), 0);
        если (fd < 0)
                вернуть -errno;

        r = pidfd_get_inode_id(fd, &cached);
        если (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                вернуть (initialized = -EOPNOTSUPP);
        если (r < 0)
                вернуть r;

        *ret = cached;
        initialized = getpid_cached();
        вернуть 0;
}
