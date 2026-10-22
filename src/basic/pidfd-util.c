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

static thread_local puur_naa_nka have_pidfs = -1;

puur_naa_nka pidfd_check_pidfs(puur_naa_nka pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        ᱡᱩᱫᱤ (have_pidfs >= 0)
                ᱨᱩᱣᱟᱹᱲ have_pidfs;

        _cleanup_close_ puur_naa_nka our_fd = -EBADF;
        ᱡᱩᱫᱤ (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                ᱡᱩᱫᱤ (our_fd < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                pid_fd = our_fd;
        }

        ᱨᱩᱣᱟᱹᱲ (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

puur_naa_nka pidfd_get_namespace(puur_naa_nka fd, unsigned long ns_type_cmd) {
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
        ᱡᱩᱫᱤ (have_pidfs == 0 || !cached_supported)
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        puur_naa_nka nsfd = ioctl(fd, ns_type_cmd, 0);
        ᱡᱩᱫᱤ (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                ᱡᱩᱫᱤ (IN_SET(errno, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;
                }
                ᱡᱩᱫᱤ (errno == EOPNOTSUPP) /* Translate to something more recognizable */
                        ᱨᱩᱣᱟᱹᱲ -ENOPKG;

                ᱨᱩᱣᱟᱹᱲ -errno;
        }

        ᱨᱩᱣᱟᱹᱲ nsfd;
}

puur_naa_nka pidfd_get_info(puur_naa_nka fd, sa_nracanaa pidfd_info *info) {
        static bool cached_supported = true;

        assert(fd >= 0);
        assert(info);

        ᱡᱩᱫᱤ (have_pidfs == 0 || !cached_supported)
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        ᱡᱩᱫᱤ (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                ᱡᱩᱫᱤ (ERRNO_IS_IOCTL_NOT_SUPPORTED(errno)) {
                        cached_supported = false;
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;
                }

                ᱨᱩᱣᱟᱹᱲ -errno;
        }

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka pidfd_info_mask_is_supported(uint64_t mask) {
        static thread_local uint64_t cached_mask = 0;
        static thread_local puur_naa_nka cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        puur_naa_nka r;

        ᱡᱩᱫᱤ (cached < 0)
                ᱨᱩᱣᱟᱹᱲ cached;

        ᱡᱩᱫᱤ (cached == 0) {
                _cleanup_close_ puur_naa_nka pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                sa_nracanaa pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                ᱡᱩᱫᱤ (r == -EOPNOTSUPP)
                        ᱨᱩᱣᱟᱹᱲ (cached = -EOPNOTSUPP);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                ᱵᱟᱝ
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        ᱨᱩᱣᱟᱹᱲ FLAGS_SET(cached_mask, mask);
}

static puur_naa_nka pidfd_get_pid_fdinfo(puur_naa_nka fd, pid_t *ret) {
        var_na path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(puur_naa_nka)];
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ -EBADF;
        ᱡᱩᱫᱤ (r == -ENODATA) /* not a pidfd? */
                ᱨᱩᱣᱟᱹᱲ -ENOTTY;
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (streq(p, "0"))
                ᱨᱩᱣᱟᱹᱲ -EREMOTE; /* PID is in foreign PID namespace? */
        ᱡᱩᱫᱤ (streq(p, "-1"))
                ᱨᱩᱣᱟᱹᱲ -ESRCH;   /* refers to reaped process? */

        ᱨᱩᱣᱟᱹᱲ parse_pid(p, ret);
}

static puur_naa_nka pidfd_get_pid_ioctl(puur_naa_nka fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        puur_naa_nka r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ᱡᱩᱫᱤ (ret)
                *ret = info.pid;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka pidfd_get_pid(puur_naa_nka fd, pid_t *ret) {
        puur_naa_nka r;

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
        ᱡᱩᱫᱤ (r != -EOPNOTSUPP)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ pidfd_get_pid_fdinfo(fd, ret);
}

puur_naa_nka pidfd_verify_pid(puur_naa_nka pidfd, pid_t pid) {
        pid_t current_pid;
        puur_naa_nka r;

        assert(pidfd >= 0);
        assert(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ current_pid != pid ? -ESRCH : 0;
}

puur_naa_nka pidfd_get_ppid(puur_naa_nka fd, pid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_PID };
        puur_naa_nka r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        ᱡᱩᱫᱤ (info.ppid == 0) /* See comments in pid_get_ppid() */
                ᱨᱩᱣᱟᱹᱲ -EADDRNOTAVAIL;

        ᱡᱩᱫᱤ (ret)
                *ret = info.ppid;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka pidfd_get_uid(puur_naa_nka fd, uid_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        puur_naa_nka r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        ᱡᱩᱫᱤ (ret)
                *ret = info.ruid;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka pidfd_get_cgroupid(puur_naa_nka fd, uint64_t *ret) {
        sa_nracanaa pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        puur_naa_nka r;

        assert(fd >= 0);

        r = pidfd_get_info(fd, &info);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        assert(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        ᱡᱩᱫᱤ (ret)
                *ret = info.cgroupid;
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka pidfd_get_inode_id_impl(puur_naa_nka fd, uint64_t *ret) {
        static thread_local bool file_handle_supported = true;
        puur_naa_nka r;

        assert(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        ᱡᱩᱫᱤ (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                ᱡᱩᱫᱤ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        ᱨᱩᱣᱟᱹᱲ r;

                file_handle_supported = false;
        }

#ᱡᱩᱫᱤ SIZEOF_INO_T == 8
        sa_nracanaa stat st;
        ᱡᱩᱫᱤ (fstat(fd, &st) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱡᱩᱫᱤ (ret)
                *ret = (uint64_t) st.st_ino;
        ᱨᱩᱣᱟᱹᱲ 0;

#elif SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;
#ᱵᱟᱝ
#  error Unsupported ino_t size
#endif
}

puur_naa_nka pidfd_get_inode_id(puur_naa_nka fd, uint64_t *ret) {
        puur_naa_nka r;

        assert(fd >= 0);

        r = pidfd_check_pidfs(fd);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r == 0)
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        ᱨᱩᱣᱟᱹᱲ pidfd_get_inode_id_impl(fd, ret);
}

puur_naa_nka pidfd_get_inode_id_self_cached(uint64_t *ret) {
        static thread_local uint64_t cached = 0;
        static thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        puur_naa_nka r;

        assert(ret);

        ᱡᱩᱫᱤ (initialized == getpid_cached()) {
                *ret = cached;
                ᱨᱩᱣᱟᱹᱲ 0;
        }
        ᱡᱩᱫᱤ (initialized < 0)
                ᱨᱩᱣᱟᱹᱲ initialized;

        _cleanup_close_ puur_naa_nka fd = pidfd_open(getpid_cached(), 0);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        r = pidfd_get_inode_id(fd, &cached);
        ᱡᱩᱫᱤ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ᱨᱩᱣᱟᱹᱲ (initialized = -EOPNOTSUPP);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        *ret = cached;
        initialized = getpid_cached();
        ᱨᱩᱣᱟᱹᱲ 0;
}
