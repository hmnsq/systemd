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

static thread_local 整数 have_pidfs = -1;

整数 pidfd_check_pidfs(整数 pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        もし (have_pidfs >= 0)
                戻る have_pidfs;

        _cleanup_close_ 整数 our_fd = -EBADF;
        もし (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                もし (our_fd < 0)
                        戻る -errno;

                pid_fd = our_fd;
        }

        戻る (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

整数 pidfd_get_namespace(整数 fd, unsigned long ns_type_cmd) {
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
        もし (have_pidfs == 0 || !cached_supported)
                戻る -EOPNOTSUPP;

        整数 nsfd = ioctl(fd, ns_type_cmd, 0);
        もし (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                もし (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        戻る -EOPNOTSUPP;
                }
                もし (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        戻る -ENOPKG;

                戻る -errno;
        }

        戻る nsfd;
}

整数 pidfd_get_info(整数 fd, struct pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        もし (have_pidfs == 0 || !cached_supported)
                戻る -EOPNOTSUPP;

        もし (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                もし (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        戻る -EOPNOTSUPP;
                }

                戻る -errno;
        }

        戻る 0;
}

整数 pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local 整数 cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        整数 r;

        もし (cached < 0)
                戻る cached;

        もし (cached == 0) {
                _cleanup_close_ 整数 pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                もし (r < 0)
                        戻る r;

                struct pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                もし (r == -EOPNOTSUPP)
                        戻る (cached = -EOPNOTSUPP);
                もし (r < 0)
                        戻る r;

                もし (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                否则
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        戻る FLAGS_SET(cached_mask, mask);
}

static 整数 pidfd_get_pid_fdinfo(整数 fd, pid_t *ret) {
        char path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(整数)];
        _cleanup_free_ char *p = NULL;
        整数 r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        もし (r == -ENOENT)
                戻る -EBADF;
        もし (r == -ENODATA) /* not a pidfd? */
                戻る -ENOTTY;
        もし (r < 0)
                戻る r;

        もし (streq(p, "0"))
                戻る -EREMOTE; /* PID is in foreign PID namespace? */
        もし (streq(p, "-1"))
                戻る -ESRCH;   /* refers to reaped process? */

        戻る parse_pid(p, ret);
}

static 整数 pidfd_get_pid_ioctl(整数 fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        整数 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        もし (r < 0)
                戻る r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        もし (ret)
                *ret = info.pid;
        戻る 0;
}

整数 pidfd_get_pid(整数 fd, pid_t *ret) {
        整数 r;

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
        もし (r != -EOPNOTSUPP)
                戻る r;

        戻る pidfd_get_pid_fdinfo(fd, ret);
}

整数 pidfd_verify_pid(整数 pidfd, pid_t pid) {
        pid_t current_pid;
        整数 r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        もし (r < 0)
                戻る r;

        戻る current_pid != pid ? -ESRCH : 0;
}

整数 pidfd_get_ppid(整数 fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        整数 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        もし (r < 0)
                戻る r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        もし (info.ppid == 0) /* See comments in pid_get_ppid() */
                戻る -EADDRNOTAVAIL;

        もし (ret)
                *ret = info.ppid;
        戻る 0;
}

整数 pidfd_get_uid(整数 fd, uid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        整数 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        もし (r < 0)
                戻る r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        もし (ret)
                *ret = info.ruid;
        戻る 0;
}

整数 pidfd_get_cgroupid(整数 fd, uint64_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        整数 r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        もし (r < 0)
                戻る r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        もし (ret)
                *ret = info.cgroupid;
        戻る 0;
}

整数 pidfd_get_inode_id_impl(整数 fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        整数 r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        もし (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                もし (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        戻る r;

                file_handle_supported = false;
        }

#もし SIZEOF_INO_T == 8
        struct stat st;
        もし (fstat(fd, &st) < 0)
                戻る -errno;

        もし (ret)
                *ret = (uint64_t) st.st_ino;
        戻る 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        戻る -EOPNOTSUPP;
#否则
#  error Unsupported ino_t size
#endif
}

整数 pidfd_get_inode_id(整数 fd, uint64_t *ret) {
        整数 r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EOPNOTSUPP;

        戻る pidfd_get_inode_id_impl(fd, ret);
}

整数 pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        整数 r;

        assert(ret);

        もし (initialized == getpid_cached()) {
                *ret = cached;
                戻る 0;
        }
        もし (initialized < 0)
                戻る initialized;

        _cleanup_close_ 整数 fd = pidfd_open(getpid_cached(), 0);
        もし (fd < 0)
                戻る -errno;

        r = pidfd_get_inode_id(fd, &cached);
        もし (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                戻る (initialized = -EOPNOTSUPP);
        もし (r < 0)
                戻る r;

        *ret = cached;
        initialized = getpid_cached();
        戻る 0;
}
