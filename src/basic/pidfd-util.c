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

static thread_local පූර්ණ_සංඛ්‍යා have_pidfs = -1;

පූර්ණ_සංඛ්‍යා pidfd_check_pidfs(පූර්ණ_සංඛ්‍යා pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        නම් (have_pidfs >= 0)
                ආපසු have_pidfs;

        _cleanup_close_ පූර්ණ_සංඛ්‍යා our_fd = -EBADF;
        නම් (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                නම් (our_fd < 0)
                        ආපසු -errno;

                pid_fd = our_fd;
        }

        ආපසු (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

පූර්ණ_සංඛ්‍යා pidfd_get_namespace(පූර්ණ_සංඛ්‍යා fd, unsigned long ns_type_cmd) {
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
        නම් (have_pidfs == 0 || !cached_supported)
                ආපසු -EOPNOTSUPP;

        පූර්ණ_සංඛ්‍යා nsfd = ioctl(fd, ns_type_cmd, 0);
        නම් (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                නම් (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ආපසු -EOPNOTSUPP;
                }
                නම් (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ආපසු -ENOPKG;

                ආපසු -errno;
        }

        ආපසු nsfd;
}

පූර්ණ_සංඛ්‍යා pidfd_get_info(පූර්ණ_සංඛ්‍යා fd, sa_nracanaa pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        නම් (have_pidfs == 0 || !cached_supported)
                ආපසු -EOPNOTSUPP;

        නම් (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                නම් (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ආපසු -EOPNOTSUPP;
                }

                ආපසු -errno;
        }

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local පූර්ණ_සංඛ්‍යා cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        පූර්ණ_සංඛ්‍යා r;

        නම් (cached < 0)
                ආපසු cached;

        නම් (cached == 0) {
                _cleanup_close_ පූර්ණ_සංඛ්‍යා pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                නම් (r < 0)
                        ආපසු r;

                sa_nracanaa pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                නම් (r == -EOPNOTSUPP)
                        ආපසු (cached = -EOPNOTSUPP);
                නම් (r < 0)
                        ආපසු r;

                නම් (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                නැතහොත්
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ආපසු FLAGS_SET(cached_mask, mask);
}

static පූර්ණ_සංඛ්‍යා pidfd_get_pid_fdinfo(පූර්ණ_සංඛ්‍යා fd, pid_t *ret) {
        var_na path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(පූර්ණ_සංඛ්‍යා)];
        _cleanup_free_ var_na *p = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        නම් (r == -ENOENT)
                ආපසු -EBADF;
        නම් (r == -ENODATA) /* not a pidfd? */
                ආපසු -ENOTTY;
        නම් (r < 0)
                ආපසු r;

        නම් (streq(p, "0"))
                ආපසු -EREMOTE; /* PID is in foreign PID namespace? */
        නම් (streq(p, "-1"))
                ආපසු -ESRCH;   /* refers to reaped process? */

        ආපසු parse_pid(p, ret);
}

static පූර්ණ_සංඛ්‍යා pidfd_get_pid_ioctl(පූර්ණ_සංඛ්‍යා fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        නම් (r < 0)
                ආපසු r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        නම් (ret)
                *ret = info.pid;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidfd_get_pid(පූර්ණ_සංඛ්‍යා fd, pid_t *ret) {
        පූර්ණ_සංඛ්‍යා r;

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
        නම් (r != -EOPNOTSUPP)
                ආපසු r;

        ආපසු pidfd_get_pid_fdinfo(fd, ret);
}

පූර්ණ_සංඛ්‍යා pidfd_verify_pid(පූර්ණ_සංඛ්‍යා pidfd, pid_t pid) {
        pid_t current_pid;
        පූර්ණ_සංඛ්‍යා r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        නම් (r < 0)
                ආපසු r;

        ආපසු current_pid != pid ? -ESRCH : 0;
}

පූර්ණ_සංඛ්‍යා pidfd_get_ppid(පූර්ණ_සංඛ්‍යා fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        නම් (r < 0)
                ආපසු r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        නම් (info.ppid == 0) /* See comments in pid_get_ppid() */
                ආපසු -EADDRNOTAVAIL;

        නම් (ret)
                *ret = info.ppid;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidfd_get_uid(පූර්ණ_සංඛ්‍යා fd, uid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        නම් (r < 0)
                ආපසු r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        නම් (ret)
                *ret = info.ruid;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidfd_get_cgroupid(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        නම් (r < 0)
                ආපසු r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        නම් (ret)
                *ret = info.cgroupid;
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා pidfd_get_inode_id_impl(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        නම් (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                නම් (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ආපසු r;

                file_handle_supported = false;
        }

#නම් SIZEOF_INO_T == 8
        sa_nracanaa stat st;
        නම් (fstat(fd, &st) < 0)
                ආපසු -errno;

        නම් (ret)
                *ret = (uint64_t) st.st_ino;
        ආපසු 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ආපසු -EOPNOTSUPP;
#නැතහොත්
#  error Unsupported ino_t size
#endif
}

පූර්ණ_සංඛ්‍යා pidfd_get_inode_id(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret) {
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        නම් (r < 0)
                ආපසු r;
        නම් (r == 0)
                ආපසු -EOPNOTSUPP;

        ආපසු pidfd_get_inode_id_impl(fd, ret);
}

පූර්ණ_සංඛ්‍යා pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        පූර්ණ_සංඛ්‍යා r;

        assert(ret);

        නම් (initialized == getpid_cached()) {
                *ret = cached;
                ආපසු 0;
        }
        නම් (initialized < 0)
                ආපසු initialized;

        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = pidfd_open(getpid_cached(), 0);
        නම් (fd < 0)
                ආපසු -errno;

        r = pidfd_get_inode_id(fd, &cached);
        නම් (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ආපසු (initialized = -EOPNOTSUPP);
        නම් (r < 0)
                ආපසු r;

        *ret = cached;
        initialized = getpid_cached();
        ආපසු 0;
}
