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

static thread_local số_nguyên have_pidfs = -1;

số_nguyên pidfd_check_pidfs(số_nguyên pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        nếu (have_pidfs >= 0)
                trả_về have_pidfs;

        _cleanup_close_ số_nguyên our_fd = -EBADF;
        nếu (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                nếu (our_fd < 0)
                        trả_về -errno;

                pid_fd = our_fd;
        }

        trả_về (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

số_nguyên pidfd_get_namespace(số_nguyên fd, unsigned long ns_type_cmd) {
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
        nếu (have_pidfs == 0 || !cached_supported)
                trả_về -EOPNOTSUPP;

        số_nguyên nsfd = ioctl(fd, ns_type_cmd, 0);
        nếu (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                nếu (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        trả_về -EOPNOTSUPP;
                }
                nếu (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        trả_về -ENOPKG;

                trả_về -errno;
        }

        trả_về nsfd;
}

số_nguyên pidfd_get_info(số_nguyên fd, struct pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        nếu (have_pidfs == 0 || !cached_supported)
                trả_về -EOPNOTSUPP;

        nếu (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                nếu (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        trả_về -EOPNOTSUPP;
                }

                trả_về -errno;
        }

        trả_về 0;
}

số_nguyên pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local số_nguyên cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        số_nguyên r;

        nếu (cached < 0)
                trả_về cached;

        nếu (cached == 0) {
                _cleanup_close_ số_nguyên pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                nếu (r < 0)
                        trả_về r;

                struct pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                nếu (r == -EOPNOTSUPP)
                        trả_về (cached = -EOPNOTSUPP);
                nếu (r < 0)
                        trả_về r;

                nếu (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                không_thì
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        trả_về FLAGS_SET(cached_mask, mask);
}

static số_nguyên pidfd_get_pid_fdinfo(số_nguyên fd, pid_t *ret) {
        char path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(số_nguyên)];
        _cleanup_free_ char *p = NULL;
        số_nguyên r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        nếu (r == -ENOENT)
                trả_về -EBADF;
        nếu (r == -ENODATA) /* not a pidfd? */
                trả_về -ENOTTY;
        nếu (r < 0)
                trả_về r;

        nếu (streq(p, "0"))
                trả_về -EREMOTE; /* PID is in foreign PID namespace? */
        nếu (streq(p, "-1"))
                trả_về -ESRCH;   /* refers to reaped process? */

        trả_về parse_pid(p, ret);
}

static số_nguyên pidfd_get_pid_ioctl(số_nguyên fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        số_nguyên r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        nếu (r < 0)
                trả_về r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        nếu (ret)
                *ret = info.pid;
        trả_về 0;
}

số_nguyên pidfd_get_pid(số_nguyên fd, pid_t *ret) {
        số_nguyên r;

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
        nếu (r != -EOPNOTSUPP)
                trả_về r;

        trả_về pidfd_get_pid_fdinfo(fd, ret);
}

số_nguyên pidfd_verify_pid(số_nguyên pidfd, pid_t pid) {
        pid_t current_pid;
        số_nguyên r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        nếu (r < 0)
                trả_về r;

        trả_về current_pid != pid ? -ESRCH : 0;
}

số_nguyên pidfd_get_ppid(số_nguyên fd, pid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_PID };
        số_nguyên r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        nếu (r < 0)
                trả_về r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        nếu (info.ppid == 0) /* See comments in pid_get_ppid() */
                trả_về -EADDRNOTAVAIL;

        nếu (ret)
                *ret = info.ppid;
        trả_về 0;
}

số_nguyên pidfd_get_uid(số_nguyên fd, uid_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        số_nguyên r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        nếu (r < 0)
                trả_về r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        nếu (ret)
                *ret = info.ruid;
        trả_về 0;
}

số_nguyên pidfd_get_cgroupid(số_nguyên fd, uint64_t *ret) {
        struct pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        số_nguyên r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        nếu (r < 0)
                trả_về r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        nếu (ret)
                *ret = info.cgroupid;
        trả_về 0;
}

số_nguyên pidfd_get_inode_id_impl(số_nguyên fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        số_nguyên r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        nếu (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                nếu (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        trả_về r;

                file_handle_supported = false;
        }

#nếu SIZEOF_INO_T == 8
        struct stat st;
        nếu (fstat(fd, &st) < 0)
                trả_về -errno;

        nếu (ret)
                *ret = (uint64_t) st.st_ino;
        trả_về 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        trả_về -EOPNOTSUPP;
#không_thì
#  error Unsupported ino_t size
#endif
}

số_nguyên pidfd_get_inode_id(số_nguyên fd, uint64_t *ret) {
        số_nguyên r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EOPNOTSUPP;

        trả_về pidfd_get_inode_id_impl(fd, ret);
}

số_nguyên pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        số_nguyên r;

        assert(ret);

        nếu (initialized == getpid_cached()) {
                *ret = cached;
                trả_về 0;
        }
        nếu (initialized < 0)
                trả_về initialized;

        _cleanup_close_ số_nguyên fd = pidfd_open(getpid_cached(), 0);
        nếu (fd < 0)
                trả_về -errno;

        r = pidfd_get_inode_id(fd, &cached);
        nếu (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                trả_về (initialized = -EOPNOTSUPP);
        nếu (r < 0)
                trả_về r;

        *ret = cached;
        initialized = getpid_cached();
        trả_về 0;
}
