/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/magic.h>
#समावेश <sys/ioctl.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "mountpoint-util.h"
#समावेश "parse-util.h"
#समावेश "pidfd-util.h"
#समावेश "process-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"

अटल thread_local पूर्णांक have_pidfs = -1;

पूर्णांक pidfd_check_pidfs(पूर्णांक pid_fd) {

        /* NB: the passed fd *must* be acquired via pidfd_open(), i.e. must be a true pidfd! */

        अगर (have_pidfs >= 0)
                वापस have_pidfs;

        _cleanup_close_ पूर्णांक our_fd = -EBADF;
        अगर (pid_fd < 0) {
                our_fd = pidfd_open(getpid_cached(), /* flags= */ 0);
                अगर (our_fd < 0)
                        वापस -त्रुटि_सं;

                pid_fd = our_fd;
        }

        वापस (have_pidfs = fd_is_fs_type(pid_fd, PID_FS_MAGIC));
}

पूर्णांक pidfd_get_namespace(पूर्णांक fd, अचिन्हित दीर्घ ns_type_cmd) {
        अटल bool cached_supported = true;

        /* Obtain the namespace fd from pidfd directly through ioctl(PIDFD_GET_*_NAMESPACE).
         *
         * Returns -EOPNOTSUPP if ioctl on pidfds are not supported, -ENOPKG if the requested namespace
         * is disabled in kernel. (The errno used are different from what kernel returns via ioctl(),
         * see below) */

        निश्चित(fd >= 0);

        /* If we know ahead of time that pidfs is unavailable, shortcut things. But otherwise we don't
         * call pidfd_check_pidfs() here, which is kinda extraneous and our own cache is required
         * anyways (pidfs is introduced in kernel 6.9 while ioctl support there is added in 6.11). */
        अगर (have_pidfs == 0 || !cached_supported)
                वापस -EOPNOTSUPP;

        पूर्णांक nsfd = ioctl(fd, ns_type_cmd, 0);
        अगर (nsfd < 0) {
                /* Kernel returns EOPNOTSUPP if the ns type in question is disabled. Hence we need to look
                 * at precise errno instead of generic ERRNO_IS_(IOCTL_)NOT_SUPPORTED. */
                अगर (IN_SET(त्रुटि_सं, ENOTTY, EINVAL)) {
                        cached_supported = false;
                        वापस -EOPNOTSUPP;
                }
                अगर (त्रुटि_सं == EOPNOTSUPP) /* Translate to something more recognizable */
                        वापस -ENOPKG;

                वापस -त्रुटि_सं;
        }

        वापस nsfd;
}

पूर्णांक pidfd_get_info(पूर्णांक fd, काष्ठा pidfd_info *info) {
        अटल bool cached_supported = true;

        निश्चित(fd >= 0);
        निश्चित(info);

        अगर (have_pidfs == 0 || !cached_supported)
                वापस -EOPNOTSUPP;

        अगर (ioctl(fd, PIDFD_GET_INFO, info) < 0) {
                अगर (ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं)) {
                        cached_supported = false;
                        वापस -EOPNOTSUPP;
                }

                वापस -त्रुटि_सं;
        }

        वापस 0;
}

पूर्णांक pidfd_info_mask_is_supported(uint64_t mask) {
        अटल thread_local uint64_t cached_mask = 0;
        अटल thread_local पूर्णांक cached = 0; /* 0 when not cached, 1 when cached, negative errno on failure. */
        पूर्णांक r;

        अगर (cached < 0)
                वापस cached;

        अगर (cached == 0) {
                _cleanup_close_ पूर्णांक pidfd = r = RET_NERRNO(pidfd_open(getpid_cached(), /* flags= */ 0));
                अगर (r < 0)
                        वापस r;

                काष्ठा pidfd_info info = {
                        .mask = PIDFD_INFO_SUPPORTED_MASK, /* Since dfd78546c95330db2252e0d7e937a15ab5eddb4e (v6.19). */
                };

                r = pidfd_get_info(pidfd, &info);
                अगर (r == -EOPNOTSUPP)
                        वापस (cached = -EOPNOTSUPP);
                अगर (r < 0)
                        वापस r;

                अगर (FLAGS_SET(info.mask, PIDFD_INFO_SUPPORTED_MASK) &&
                    FLAGS_SET(info.supported_mask, PIDFD_INFO_SUPPORTED_MASK))
                        /* The kernel is v6.19 or newer. Let's use the provided supported_mask. */
                        cached_mask = info.supported_mask;
                अन्यथा
                        /* The kernel is older than v6.19. These three flags have been supported since
                         * ioctl(PIDFD_GET_INFO) was introduced, so a successful pidfd_get_info() call
                         * guarantees they are supported. However, there is no reliable way to
                         * determine whether PIDFD_INFO_EXIT and PIDFD_INFO_COREDUMP are supported, as
                         * they predate PIDFD_INFO_SUPPORTED_MASK, so assume they are not. */
                        cached_mask = PIDFD_INFO_PID | PIDFD_INFO_CREDS | PIDFD_INFO_CGROUPID;

                cached = true;
        }

        /* Return true when all requested features are supported. */
        वापस FLAGS_SET(cached_mask, mask);
}

अटल पूर्णांक pidfd_get_pid_fdinfo(पूर्णांक fd, pid_t *ret) {
        अक्षर path[STRLEN("/proc/self/fdinfo/") + DECIMAL_STR_MAX(पूर्णांक)];
        _cleanup_free_ अक्षर *p = शून्य;
        पूर्णांक r;

        निश्चित(fd >= 0);

        xsprintf(path, "/proc/self/fdinfo/%i", fd);

        r = get_proc_field(path, "Pid", &p);
        अगर (r == -ENOENT)
                वापस -EBADF;
        अगर (r == -ENODATA) /* not a pidfd? */
                वापस -ENOTTY;
        अगर (r < 0)
                वापस r;

        अगर (streq(p, "0"))
                वापस -EREMOTE; /* PID is in foreign PID namespace? */
        अगर (streq(p, "-1"))
                वापस -ESRCH;   /* refers to reaped process? */

        वापस parse_pid(p, ret);
}

अटल पूर्णांक pidfd_get_pid_ioctl(पूर्णांक fd, pid_t *ret) {
        काष्ठा pidfd_info info = { .mask = PIDFD_INFO_PID };
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = pidfd_get_info(fd, &info);
        अगर (r < 0)
                वापस r;

        निश्चित(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        अगर (ret)
                *ret = info.pid;
        वापस 0;
}

पूर्णांक pidfd_get_pid(पूर्णांक fd, pid_t *ret) {
        पूर्णांक r;

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

        निश्चित(fd >= 0);

        r = pidfd_get_pid_ioctl(fd, ret);
        अगर (r != -EOPNOTSUPP)
                वापस r;

        वापस pidfd_get_pid_fdinfo(fd, ret);
}

पूर्णांक pidfd_verify_pid(पूर्णांक pidfd, pid_t pid) {
        pid_t current_pid;
        पूर्णांक r;

        निश्चित(pidfd >= 0);
        निश्चित(pid > 0);

        r = pidfd_get_pid(pidfd, &current_pid);
        अगर (r < 0)
                वापस r;

        वापस current_pid != pid ? -ESRCH : 0;
}

पूर्णांक pidfd_get_ppid(पूर्णांक fd, pid_t *ret) {
        काष्ठा pidfd_info info = { .mask = PIDFD_INFO_PID };
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = pidfd_get_info(fd, &info);
        अगर (r < 0)
                वापस r;

        निश्चित(FLAGS_SET(info.mask, PIDFD_INFO_PID));

        अगर (info.ppid == 0) /* See comments in pid_get_ppid() */
                वापस -EADDRNOTAVAIL;

        अगर (ret)
                *ret = info.ppid;
        वापस 0;
}

पूर्णांक pidfd_get_uid(पूर्णांक fd, uid_t *ret) {
        काष्ठा pidfd_info info = { .mask = PIDFD_INFO_CREDS };
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = pidfd_get_info(fd, &info);
        अगर (r < 0)
                वापस r;

        निश्चित(FLAGS_SET(info.mask, PIDFD_INFO_CREDS));

        अगर (ret)
                *ret = info.ruid;
        वापस 0;
}

पूर्णांक pidfd_get_cgroupid(पूर्णांक fd, uint64_t *ret) {
        काष्ठा pidfd_info info = { .mask = PIDFD_INFO_CGROUPID };
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = pidfd_get_info(fd, &info);
        अगर (r < 0)
                वापस r;

        निश्चित(FLAGS_SET(info.mask, PIDFD_INFO_CGROUPID));

        अगर (ret)
                *ret = info.cgroupid;
        वापस 0;
}

पूर्णांक pidfd_get_inode_id_impl(पूर्णांक fd, uint64_t *ret) {
        अटल thread_local bool file_handle_supported = true;
        पूर्णांक r;

        निश्चित(fd >= 0);

        /* Since kernel 6.14 (b3caba8f7a34a2bbaf45ffc6ff3a49b70afeb192), we can use name_to_handle_at(). */
        अगर (file_handle_supported) {
                r = fd_to_handle_u64(fd, ret);
                अगर (r >= 0 || is_name_to_handle_at_fatal_error(r))
                        वापस r;

                file_handle_supported = false;
        }

#अगर SIZEOF_INO_T == 8
        काष्ठा stat st;
        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        अगर (ret)
                *ret = (uint64_t) st.st_ino;
        वापस 0;

#या_अगर SIZEOF_INO_T == 4
        /* On 32-bit systems (where sizeof(ino_t) == 4), the inode id returned by fstat() cannot be used to
         * reliably identify the process, nor can we communicate the origin of the id with the clients.
         * Hence let's just refuse to acquire pidfdid through fstat() here. All clients shall also insist on
         * the 64-bit id from name_to_handle_at(). */
        वापस -EOPNOTSUPP;
#अन्यथा
#  error Unsupported ino_t आकार
#पूर्ण_अगर
}

पूर्णांक pidfd_get_inode_id(पूर्णांक fd, uint64_t *ret) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = pidfd_check_pidfs(fd);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EOPNOTSUPP;

        वापस pidfd_get_inode_id_impl(fd, ret);
}

पूर्णांक pidfd_get_inode_id_self_cached(uint64_t *ret) {
        अटल thread_local uint64_t cached = 0;
        अटल thread_local pid_t initialized = 0; /* < 0: cached error; == 0: invalid; > 0: valid and pid that was current */
        पूर्णांक r;

        निश्चित(ret);

        अगर (initialized == getpid_cached()) {
                *ret = cached;
                वापस 0;
        }
        अगर (initialized < 0)
                वापस initialized;

        _cleanup_close_ पूर्णांक fd = pidfd_open(getpid_cached(), 0);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        r = pidfd_get_inode_id(fd, &cached);
        अगर (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                वापस (initialized = -EOPNOTSUPP);
        अगर (r < 0)
                वापस r;

        *ret = cached;
        initialized = getpid_cached();
        वापस 0;
}
