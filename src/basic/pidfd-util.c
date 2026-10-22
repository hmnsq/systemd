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

static thread_local 정수 have_pidfs = -1;

정수 pidfd_check_pidfs(정수 pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        만약 (have_pidfs >= 0)
                반환 have_pidfs;

        _cleanup_close_ 정수 our_fd = -EBADF;
        만약 (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                만약 (our_fd < 0)
                        반환 -errno;

                pid_fd = our_fd;
        }

        반환 (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

정수 pidfd_get_namespace(정수 fd, unsigned long ns_type_cmd) {
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
        만약 (have_pidfs == 0 || !cached_supported)
                반환 -EOPNOTSUPP;

        정수 nsfd = ioctl(fd, ns_type_cmd, 0);
        만약 (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                만약 (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        반환 -EOPNOTSUPP;
                }
                만약 (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        반환 -ENOPKG;

                반환 -errno;
        }

        반환 nsfd;
}

정수 pidfd_get_info(정수 fd, struct pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        만약 (have_pidfs == 0 || !cached_supported)
                반환 -EOPNOTSUPP;

        만약 (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                만약 (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        반환 -EOPNOTSUPP;
                }

                반환 -errno;
        }

        반환 0;
}

정수 pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local 정수 cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        정수 r;

        만약 (cached < 0)
                반환 cached;

        만약 (cached == 0) {
                _cleanup_close_ 정수 pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                만약 (r < 0)
                        반환 r;

                struct pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                만약 (r == -EOPNOTSUPP)
                        반환 (cached = -EOPNOTSUPP);
                만약 (r < 0)
                        반환 r;

                만약 (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                아니면
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        반환 FLAGS_SET(cached_mask, mask);
}

static 정수 pidfd_get_pid_fdinfo(정수 fd, pid_t *ret) {
        char path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(정수)];
        _cleanup_free_ char *p = NULL;
        정수 r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        만약 (r == -ENOENT)
                반환 -EBADF;
        만약 (r == -ENODATA) /* not a pidfd? */
                반환 -ENOTTY;
        만약 (r < 0)
                반환 r;

        만약 (streq(p, "0"))
                반환 -EREMOTE; /* PID is in foreign PID namespace? */
        만약 (streq(p, "-1"))
                반환 -ESRCH;   /* refers to reaped process? */

        반환 parse_pid(p, ret);
}

static 정수 pidfd_get_pid_ioctl(정수 fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        정수 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        만약 (r < 0)
                반환 r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        만약 (ret)
                *ret = info.pid;
        반환 0;
}

정수 pidfd_get_pid(정수 fd, pid_t *ret) {
        정수 r;

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
        만약 (r != -EOPNOTSUPP)
                반환 r;

        반환 pidfd_get_pid_fdinfo(fd, ret);
}

정수 pidfd_verify_pid(정수 pidfd, pid_t pid) {
        pid_t current_pid;
        정수 r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        만약 (r < 0)
                반환 r;

        반환 current_pid != pid ? -ESRCH : 0;
}

정수 pidfd_get_ppid(정수 fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        정수 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        만약 (r < 0)
                반환 r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        만약 (info.ppid == 0) /* See comments in pid_get_ppid() */
                반환 -EADDRNOTAVAIL;

        만약 (ret)
                *ret = info.ppid;
        반환 0;
}

정수 pidfd_get_uid(정수 fd, uid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        정수 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        만약 (r < 0)
                반환 r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        만약 (ret)
                *ret = info.ruid;
        반환 0;
}

정수 pidfd_get_cgroupid(정수 fd, uint64_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        정수 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        만약 (r < 0)
                반환 r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        만약 (ret)
                *ret = info.cgroupid;
        반환 0;
}

정수 pidfd_get_inode_id_impl(정수 fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        정수 r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        만약 (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                만약 (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        반환 r;

                file_handle_supported = false;
        }

#만약 SIZEOF_INO_T == 8
        struct stat st;
        만약 (fstat(fd, &st) < 0)
                반환 -errno;

        만약 (ret)
                *ret = (uint64_t) st.st_ino;
        반환 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        반환 -EOPNOTSUPP;
#아니면
#  error Unsupported ino_t size
#endif
}

정수 pidfd_get_inode_id(정수 fd, uint64_t *ret) {
        정수 r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        만약 (r < 0)
                반환 r;
        만약 (r == 0)
                반환 -EOPNOTSUPP;

        반환 pidfd_get_inode_id_impl(fd, ret);
}

정수 pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        정수 r;

        assert(ret);

        만약 (initialized == getpid_cached()) {
                *ret = cached;
                반환 0;
        }
        만약 (initialized < 0)
                반환 initialized;

        _cleanup_close_ 정수 fd = pidfd_open(getpid_cached(), 0);
        만약 (fd < 0)
                반환 -errno;

        r = pidfd_get_inode_id(fd, &cached);
        만약 (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                반환 (initialized = -EOPNOTSUPP);
        만약 (r < 0)
                반환 r;

        *ret = cached;
        initialized = getpid_cached();
        반환 0;
}
