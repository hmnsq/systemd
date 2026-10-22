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

static thread_local ကိန်းပြည့် have_pidfs = -1;

ကိန်းပြည့် pidfd_check_pidfs(ကိန်းပြည့် pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        အကယ်၍ (have_pidfs >= 0)
                ပြန်ပေး have_pidfs;

        _cleanup_close_ ကိန်းပြည့် our_fd = -EBADF;
        အကယ်၍ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                အကယ်၍ (our_fd < 0)
                        ပြန်ပေး -errno;

                pid_fd = our_fd;
        }

        ပြန်ပေး (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

ကိန်းပြည့် pidfd_get_namespace(ကိန်းပြည့် fd, unsigned long ns_type_cmd) {
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
        အကယ်၍ (have_pidfs == 0 || !cached_supported)
                ပြန်ပေး -EOPNOTSUPP;

        ကိန်းပြည့် nsfd = ioctl(fd, ns_type_cmd, 0);
        အကယ်၍ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                အကယ်၍ (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ပြန်ပေး -EOPNOTSUPP;
                }
                အကယ်၍ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ပြန်ပေး -ENOPKG;

                ပြန်ပေး -errno;
        }

        ပြန်ပေး nsfd;
}

ကိန်းပြည့် pidfd_get_info(ကိန်းပြည့် fd, sa_nracanaa pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        အကယ်၍ (have_pidfs == 0 || !cached_supported)
                ပြန်ပေး -EOPNOTSUPP;

        အကယ်၍ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                အကယ်၍ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ပြန်ပေး -EOPNOTSUPP;
                }

                ပြန်ပေး -errno;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local ကိန်းပြည့် cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        ကိန်းပြည့် r;

        အကယ်၍ (cached < 0)
                ပြန်ပေး cached;

        အကယ်၍ (cached == 0) {
                _cleanup_close_ ကိန်းပြည့် pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                sa_nracanaa pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                အကယ်၍ (r == -EOPNOTSUPP)
                        ပြန်ပေး (cached = -EOPNOTSUPP);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                မဟုတ်ပါက
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ပြန်ပေး FLAGS_SET(cached_mask, mask);
}

static ကိန်းပြည့် pidfd_get_pid_fdinfo(ကိန်းပြည့် fd, pid_t *ret) {
        var_na path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(ကိန်းပြည့်)];
        _cleanup_free_ var_na *p = NULL;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        အကယ်၍ (r == -ENOENT)
                ပြန်ပေး -EBADF;
        အကယ်၍ (r == -ENODATA) /* not a pidfd? */
                ပြန်ပေး -ENOTTY;
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (streq(p, "0"))
                ပြန်ပေး -EREMOTE; /* PID is in foreign PID namespace? */
        အကယ်၍ (streq(p, "-1"))
                ပြန်ပေး -ESRCH;   /* refers to reaped process? */

        ပြန်ပေး parse_pid(p, ret);
}

static ကိန်းပြည့် pidfd_get_pid_ioctl(ကိန်းပြည့် fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        အကယ်၍ (ret)
                *ret = info.pid;
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidfd_get_pid(ကိန်းပြည့် fd, pid_t *ret) {
        ကိန်းပြည့် r;

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
        အကယ်၍ (r != -EOPNOTSUPP)
                ပြန်ပေး r;

        ပြန်ပေး pidfd_get_pid_fdinfo(fd, ret);
}

ကိန်းပြည့် pidfd_verify_pid(ကိန်းပြည့် pidfd, pid_t pid) {
        pid_t current_pid;
        ကိန်းပြည့် r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး current_pid != pid ? -ESRCH : 0;
}

ကိန်းပြည့် pidfd_get_ppid(ကိန်းပြည့် fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        အကယ်၍ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ပြန်ပေး -EADDRNOTAVAIL;

        အကယ်၍ (ret)
                *ret = info.ppid;
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidfd_get_uid(ကိန်းပြည့် fd, uid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        အကယ်၍ (ret)
                *ret = info.ruid;
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidfd_get_cgroupid(ကိန်းပြည့် fd, uint64_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        အကယ်၍ (ret)
                *ret = info.cgroupid;
        ပြန်ပေး 0;
}

ကိန်းပြည့် pidfd_get_inode_id_impl(ကိန်းပြည့် fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        အကယ်၍ (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                အကယ်၍ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ပြန်ပေး r;

                file_handle_supported = false;
        }

#အကယ်၍ SIZEOF_INO_T == 8
        sa_nracanaa stat st;
        အကယ်၍ (fstat(fd, &st) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (ret)
                *ret = (uint64_t) st.st_ino;
        ပြန်ပေး 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ပြန်ပေး -EOPNOTSUPP;
#မဟုတ်ပါက
#  error Unsupported ino_t size
#endif
}

ကိန်းပြည့် pidfd_get_inode_id(ကိန်းပြည့် fd, uint64_t *ret) {
        ကိန်းပြည့် r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r == 0)
                ပြန်ပေး -EOPNOTSUPP;

        ပြန်ပေး pidfd_get_inode_id_impl(fd, ret);
}

ကိန်းပြည့် pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        ကိန်းပြည့် r;

        assert(ret);

        အကယ်၍ (initialized == getpid_cached()) {
                *ret = cached;
                ပြန်ပေး 0;
        }
        အကယ်၍ (initialized < 0)
                ပြန်ပေး initialized;

        _cleanup_close_ ကိန်းပြည့် fd = pidfd_open(getpid_cached(), 0);
        အကယ်၍ (fd < 0)
                ပြန်ပေး -errno;

        r = pidfd_get_inode_id(fd, &cached);
        အကယ်၍ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ပြန်ပေး (initialized = -EOPNOTSUPP);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = cached;
        initialized = getpid_cached();
        ပြန်ပေး 0;
}
