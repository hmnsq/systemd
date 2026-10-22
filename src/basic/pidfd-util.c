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

static thread_local ཧྲིལ have_pidfs = -1;

ཧྲིལ pidfd_check_pidfs(ཧྲིལ pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        གལ་ཏེ (have_pidfs >= 0)
                ཕྱིར have_pidfs;

        _cleanup_close_ ཧྲིལ our_fd = -EBADF;
        གལ་ཏེ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                གལ་ཏེ (our_fd < 0)
                        ཕྱིར -errno;

                pid_fd = our_fd;
        }

        ཕྱིར (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ཧྲིལ pidfd_get_namespace(ཧྲིལ fd, unsigned long ns_type_cmd) {
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
        གལ་ཏེ (have_pidfs == 0 || !cached_supported)
                ཕྱིར -EOPNOTSUPP;

        ཧྲིལ nsfd = ioctl(fd, ns_type_cmd, 0);
        གལ་ཏེ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                གལ་ཏེ (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ཕྱིར -EOPNOTSUPP;
                }
                གལ་ཏེ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ཕྱིར -ENOPKG;

                ཕྱིར -errno;
        }

        ཕྱིར nsfd;
}

ཧྲིལ pidfd_get_info(ཧྲིལ fd, sa_nracanaa pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        གལ་ཏེ (have_pidfs == 0 || !cached_supported)
                ཕྱིར -EOPNOTSUPP;

        གལ་ཏེ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                གལ་ཏེ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ཕྱིར -EOPNOTSUPP;
                }

                ཕྱིར -errno;
        }

        ཕྱིར 0;
}

ཧྲིལ pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local ཧྲིལ cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ཧྲིལ r;

        གལ་ཏེ (cached < 0)
                ཕྱིར cached;

        གལ་ཏེ (cached == 0) {
                _cleanup_close_ ཧྲིལ pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                sa_nracanaa pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                གལ་ཏེ (r == -EOPNOTSUPP)
                        ཕྱིར (cached = -EOPNOTSUPP);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                གལ་ཏེ (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                དེ་མིན
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ཕྱིར FLAGS_SET(cached_mask, mask);
}

static ཧྲིལ pidfd_get_pid_fdinfo(ཧྲིལ fd, pid_t *ret) {
        var_na path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ཧྲིལ)];
        _cleanup_free_ var_na *p = NULL;
        ཧྲིལ r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        གལ་ཏེ (r == -ENOENT)
                ཕྱིར -EBADF;
        གལ་ཏེ (r == -ENODATA) /* not a pidfd? */
                ཕྱིར -ENOTTY;
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (streq(p, "0"))
                ཕྱིར -EREMOTE; /* PID is in foreign PID namespace? */
        གལ་ཏེ (streq(p, "-1"))
                ཕྱིར -ESRCH;   /* refers to reaped process? */

        ཕྱིར parse_pid(p, ret);
}

static ཧྲིལ pidfd_get_pid_ioctl(ཧྲིལ fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        ཧྲིལ r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        གལ་ཏེ (ret)
                *ret = info.pid;
        ཕྱིར 0;
}

ཧྲིལ pidfd_get_pid(ཧྲིལ fd, pid_t *ret) {
        ཧྲིལ r;

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
        གལ་ཏེ (r != -EOPNOTSUPP)
                ཕྱིར r;

        ཕྱིར pidfd_get_pid_fdinfo(fd, ret);
}

ཧྲིལ pidfd_verify_pid(ཧྲིལ pidfd, pid_t pid) {
        pid_t current_pid;
        ཧྲིལ r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        ཕྱིར current_pid != pid ? -ESRCH : 0;
}

ཧྲིལ pidfd_get_ppid(ཧྲིལ fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        ཧྲིལ r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        གལ་ཏེ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ཕྱིར -EADDRNOTAVAIL;

        གལ་ཏེ (ret)
                *ret = info.ppid;
        ཕྱིར 0;
}

ཧྲིལ pidfd_get_uid(ཧྲིལ fd, uid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        ཧྲིལ r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        གལ་ཏེ (ret)
                *ret = info.ruid;
        ཕྱིར 0;
}

ཧྲིལ pidfd_get_cgroupid(ཧྲིལ fd, uint64_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        ཧྲིལ r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        གལ་ཏེ (ret)
                *ret = info.cgroupid;
        ཕྱིར 0;
}

ཧྲིལ pidfd_get_inode_id_impl(ཧྲིལ fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        ཧྲིལ r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        གལ་ཏེ (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                གལ་ཏེ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ཕྱིར r;

                file_handle_supported = false;
        }

#གལ་ཏེ SIZEOF_INO_T == 8
        sa_nracanaa stat st;
        གལ་ཏེ (fstat(fd, &st) < 0)
                ཕྱིར -errno;

        གལ་ཏེ (ret)
                *ret = (uint64_t) st.st_ino;
        ཕྱིར 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ཕྱིར -EOPNOTSUPP;
#དེ་མིན
#  error Unsupported ino_t size
#endif
}

ཧྲིལ pidfd_get_inode_id(ཧྲིལ fd, uint64_t *ret) {
        ཧྲིལ r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        གལ་ཏེ (r < 0)
                ཕྱིར r;
        གལ་ཏེ (r == 0)
                ཕྱིར -EOPNOTSUPP;

        ཕྱིར pidfd_get_inode_id_impl(fd, ret);
}

ཧྲིལ pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ཧྲིལ r;

        assert(ret);

        གལ་ཏེ (initialized == getpid_cached()) {
                *ret = cached;
                ཕྱིར 0;
        }
        གལ་ཏེ (initialized < 0)
                ཕྱིར initialized;

        _cleanup_close_ ཧྲིལ fd = pidfd_open(getpid_cached(), 0);
        གལ་ཏེ (fd < 0)
                ཕྱིར -errno;

        r = pidfd_get_inode_id(fd, &cached);
        གལ་ཏེ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ཕྱིར (initialized = -EOPNOTSUPP);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret = cached;
        initialized = getpid_cached();
        ཕྱིར 0;
}
