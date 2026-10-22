/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <linux/nsfs.h>
#ಅನ್ತರ್ಭಾವ <linux/sockios.h>
#ಅನ್ತರ್ಭಾವ <sched.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/mount.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "capability-util.h"
#ಅನ್ತರ್ಭಾವ "dlfcn-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mountpoint-util.h"
#ಅನ್ತರ್ಭಾವ "namespace-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidfd-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "uid-range.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಸ್ಥಿರಾಂಕ ರಚನೆ namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
        [NAMESPACE_CGROUP] =  { "cgroup", "ns/cgroup", CLONE_NEWCGROUP, PIDFD_GET_CGROUP_NAMESPACE, PROC_CGROUP_INIT_INO },
        [NAMESPACE_IPC]    =  { "ipc",    "ns/ipc",    CLONE_NEWIPC,    PIDFD_GET_IPC_NAMESPACE,    PROC_IPC_INIT_INO    },
        [NAMESPACE_NET]    =  { "net",    "ns/net",    CLONE_NEWNET,    PIDFD_GET_NET_NAMESPACE,    0                    },
        /* So, the mount namespace flag is called CLONE_NEWNS for historical
         * reasons. Let's expose it here under a more explanatory name: "mnt".
         * This is in-line with how the kernel exposes namespaces in /proc/$PID/ns. */
        [NAMESPACE_MOUNT]  =  { "mnt",    "ns/mnt",    CLONE_NEWNS,     PIDFD_GET_MNT_NAMESPACE,    0                    },
        [NAMESPACE_PID]    =  { "pid",    "ns/pid",    CLONE_NEWPID,    PIDFD_GET_PID_NAMESPACE,    PROC_PID_INIT_INO    },
        [NAMESPACE_USER]   =  { "user",   "ns/user",   CLONE_NEWUSER,   PIDFD_GET_USER_NAMESPACE,   PROC_USER_INIT_INO   },
        [NAMESPACE_UTS]    =  { "uts",    "ns/uts",    CLONE_NEWUTS,    PIDFD_GET_UTS_NAMESPACE,    PROC_UTS_INIT_INO    },
        [NAMESPACE_ಸಮಯ]   =  { "time",   "ns/time",   CLONE_NEWTIME,   PIDFD_GET_ಸಮಯ_NAMESPACE,   PROC_ಸಮಯ_INIT_INO   },
        {}, /* Allow callers to iterate over the array without using _NAMESPACE_TYPE_MAX. */
};

#ವ್ಯಾಖ್ಯಾನಿಸು pid_namespace_ಮಾರ್ಗ(pid, type) procfs_file_alloca(pid, namespace_info[type].proc_ಮಾರ್ಗ)

NamespaceType clone_flag_ಗಮ್ಯ_namespace_type(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ clone_flag) {
        ಗಾಗಿ (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                ಆದರೆ (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        ಹಿಂತಿರುಗಿ t;

        ಹಿಂತಿರುಗಿ _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = pid_namespace_ಮಾರ್ಗ(0, type);
        ಹಿಂತಿರುಗಿ access(p, F_OK) >= 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ pidref_namespace_open_by_type_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ PidRef *pidref, NamespaceType type, bool *need_verify) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref_is_ಗಣ(pidref));
        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                ಆದರೆ (r == -ENOPKG)
                        ಹಿಂತಿರುಗಿ log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                ಆದರೆ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ nsfd = -EBADF;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        p = pid_namespace_ಮಾರ್ಗ(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        ಆದರೆ (nsfd == -ENOENT) {
                r = proc_mounted();
                ಆದರೆ (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        ಹಿಂತಿರುಗಿ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                ಆದರೆ (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        ಹಿಂತಿರುಗಿ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        ಆದರೆ (nsfd < 0)
                ಹಿಂತಿರುಗಿ nsfd;

        ಆದರೆ (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ TAKE_FD(nsfd);
}

ಪೂರ್ಣಾಂಕ pidref_namespace_open_by_type(ಸ್ಥಿರಾಂಕ PidRef *pidref, NamespaceType type) {
        ಹಿಂತಿರುಗಿ pidref_namespace_open_by_type_ಆಂತರಿಕ(pidref, type, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_ಮುಗಿದ) PidRef self = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_ಗಣ_self(&self);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pidref_namespace_open_by_type(&self, type);
}

ಪೂರ್ಣಾಂಕ pidref_namespace_open(
                ಸ್ಥಿರಾಂಕ PidRef *pidref,
                ಪೂರ್ಣಾಂಕ *ret_pidns_fd,
                ಪೂರ್ಣಾಂಕ *ret_mntns_fd,
                ಪೂರ್ಣಾಂಕ *ret_netns_fd,
                ಪೂರ್ಣಾಂಕ *ret_userns_fd,
                ಪೂರ್ಣಾಂಕ *ret_ಬೇರು_fd) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, ಬೇರು_fd = -EBADF;
        bool need_verify = false;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref_is_ಗಣ(pidref));

        ಆದರೆ (pidref_is_remote(pidref))
                ಹಿಂತಿರುಗಿ -EREMOTE;

        ಆದರೆ (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_ಆಂತರಿಕ(pidref, NAMESPACE_PID, &need_verify);
                ಆದರೆ (pidns_fd < 0)
                        ಹಿಂತಿರುಗಿ pidns_fd;
        }

        ಆದರೆ (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_ಆಂತರಿಕ(pidref, NAMESPACE_MOUNT, &need_verify);
                ಆದರೆ (mntns_fd < 0)
                        ಹಿಂತಿರುಗಿ mntns_fd;
        }

        ಆದರೆ (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_ಆಂತರಿಕ(pidref, NAMESPACE_NET, &need_verify);
                ಆದರೆ (netns_fd < 0)
                        ಹಿಂತಿರುಗಿ netns_fd;
        }

        ಆದರೆ (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_ಆಂತರಿಕ(pidref, NAMESPACE_USER, &need_verify);
                ಆದರೆ (userns_fd < 0 && userns_fd != -ENOPKG)
                        ಹಿಂತಿರುಗಿ userns_fd;
        }

        ಆದರೆ (ret_ಬೇರು_fd) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು;

                ಬೇರು = procfs_file_alloca(pidref->pid, "root");
                ಬೇರು_fd = RET_NERRNO(open(ಬೇರು, O_CLOEXEC|O_DIRECTORY));
                ಆದರೆ (ಬೇರು_fd == -ENOENT && proc_mounted() == 0)
                        ಹಿಂತಿರುಗಿ -ENOSYS;
                ಆದರೆ (ಬೇರು_fd < 0)
                        ಹಿಂತಿರುಗಿ ಬೇರು_fd;

                need_verify = true;
        }

        ಆದರೆ (need_verify) {
                r = pidref_verify(pidref);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        ಆದರೆ (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        ಆದರೆ (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        ಆದರೆ (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        ಆದರೆ (ret_ಬೇರು_fd)
                *ret_ಬೇರು_fd = TAKE_FD(ಬೇರು_fd);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ namespace_open(
                pid_t pid,
                ಪೂರ್ಣಾಂಕ *ret_pidns_fd,
                ಪೂರ್ಣಾಂಕ *ret_mntns_fd,
                ಪೂರ್ಣಾಂಕ *ret_netns_fd,
                ಪೂರ್ಣಾಂಕ *ret_userns_fd,
                ಪೂರ್ಣಾಂಕ *ret_ಬೇರು_fd) {

        _cleanup_(pidref_ಮುಗಿದ) PidRef pidref = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        r = pidref_ಗಣ_pid(&pidref, pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_ಬೇರು_fd);
}

ಪೂರ್ಣಾಂಕ namespace_enter_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ pidns_fd,
                ಪೂರ್ಣಾಂಕ mntns_fd,
                ಪೂರ್ಣಾಂಕ netns_fd,
                ಪೂರ್ಣಾಂಕ userns_fd,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                NamespaceEnterFlags flags) {

        ಪೂರ್ಣಾಂಕ r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        ಖಂಡ_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        ಆದರೆ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        pidns_fd = -EBADF;
        }

        ಆದರೆ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        mntns_fd = -EBADF;
        }

        ಆದರೆ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        netns_fd = -EBADF;
        }

        ಆದರೆ (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        ಆದರೆ (userns_fd >= 0 && FLAGS_ಗಣ(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ಬೇರು_UNMAPPED)) {
                r = userns_has_ಬೇರು_mapping(userns_fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        bool have_cap_sys_admin = r > 0;

        ಆದರೆ (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                ಆದರೆ (userns_fd >= 0) {
                        ಆದರೆ (setns(userns_fd, CLONE_NEWUSER) < 0)
                                ಹಿಂತಿರುಗಿ -errno;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || ಬೇರು_fd >= 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        ಆದರೆ (pidns_fd >= 0)
                ಆದರೆ (setns(pidns_fd, CLONE_NEWPID) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (mntns_fd >= 0)
                ಆದರೆ (setns(mntns_fd, CLONE_NEWNS) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (netns_fd >= 0)
                ಆದರೆ (setns(netns_fd, CLONE_NEWNET) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (userns_fd >= 0 && have_cap_sys_admin)
                ಆದರೆ (setns(userns_fd, CLONE_NEWUSER) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ಬೇರು_fd >= 0) {
                ಆದರೆ (fchdir(ಬೇರು_fd) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (chroot(".") < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಆದರೆ (userns_fd < 0)
                ಹಿಂತಿರುಗಿ 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        ಆದರೆ (keep_uid_gid)
                ಹಿಂತಿರುಗಿ 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        ಆದರೆ (setresgid(0, 0, 0) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        ಆದರೆ (setresuid(0, 0, 0) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to become root in user namespace: %m");

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ fd_is_namespace(ಪೂರ್ಣಾಂಕ fd, NamespaceType type) {
        ಪೂರ್ಣಾಂಕ r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (type < 0)
                ಹಿಂತಿರುಗಿ true;

        ಪೂರ್ಣಾಂಕ clone_flag = ioctl(fd, NS_GET_NSTYPE);
        ಆದರೆ (clone_flag < 0) {
                ಆದರೆ (errno != ENOTTY)
                        ಹಿಂತಿರುಗಿ -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
                r = fd_get_ಮಾರ್ಗ(fd, &ಮಾರ್ಗ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                ಆದರೆ (ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ -ENOTTY;

                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = startswith(ಮಾರ್ಗ, namespace_info[type].proc_name);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ false;

                ಹಿಂತಿರುಗಿ startswith(p, ":[") != ಖಾಲಿ;
        }

        NamespaceType ಸಿಕ್ಕಿತು_type = clone_flag_ಗಮ್ಯ_namespace_type(clone_flag);
        ಆದರೆ (ಸಿಕ್ಕಿತು_type < 0)
                ಹಿಂತಿರುಗಿ -EBADF; /* Uh? Unknown namespace type? */

        ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು_type == type;
}

ಪೂರ್ಣಾಂಕ is_our_namespace(ಪೂರ್ಣಾಂಕ fd, NamespaceType type) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        ಆದರೆ (r < 0 && r != -ENOTTY)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0) /* Not a namespace or not of the right type? */
                ಹಿಂತಿರುಗಿ -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ our_ns = namespace_open_by_type(type);
        ಆದರೆ (our_ns < 0)
                ಹಿಂತಿರುಗಿ our_ns;

        r = fd_inode_same(fd, our_ns);
        ಆದರೆ (r == 0 && !type_verified)
                ಹಿಂತಿರುಗಿ -ENOTTY;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ are_our_namespaces(ಪೂರ್ಣಾಂಕ pidns_fd, ಪೂರ್ಣಾಂಕ mntns_fd, ಪೂರ್ಣಾಂಕ netns_fd, ಪೂರ್ಣಾಂಕ userns_fd, ಪೂರ್ಣಾಂಕ ಬೇರು_fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ಬೇರು_fd >= 0) {
                r = dir_fd_is_ಬೇರು(ಬೇರು_fd);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ ಜಾಲ_namespace_is_init(ಪೂರ್ಣಾಂಕ socket_fd) {
        ರಚನೆ stat a, b;
        ಪೂರ್ಣಾಂಕ r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_ಮಾರ್ಗ(1, NAMESPACE_NET), &a));
        ಆದರೆ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ಹಿಂತಿರುಗಿ r ? true : -ENOSYS;
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (socket_fd >= 0) {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ netns = ioctl(socket_fd, SIOCGSKNS);
                ಆದರೆ (netns < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (fstat(netns, &b) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (stat(pid_namespace_ಮಾರ್ಗ(0, NAMESPACE_NET), &b) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ stat_inode_same(&a, &b);
}

ಪೂರ್ಣಾಂಕ namespace_is_init(NamespaceType type) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(type >= 0);
        ಪ್ರತಿಜ್ಞಾ(type < _NAMESPACE_TYPE_MAX);

        ಆದರೆ (type == NAMESPACE_NET)
                ಹಿಂತಿರುಗಿ ಜಾಲ_namespace_is_init(/* socket_fd= */ -EBADF);

        ಆದರೆ (namespace_info[type].ಬೇರು_inode == 0)
                ಹಿಂತಿರುಗಿ -EBADR; /* Cannot answer this question */

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = pid_namespace_ಮಾರ್ಗ(0, type);

        ರಚನೆ stat st;
        r = RET_NERRNO(stat(p, &st));
        ಆದರೆ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ಹಿಂತಿರುಗಿ r ? true : -ENOSYS;
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ st.st_ino == namespace_info[type].ಬೇರು_inode;
}

ಪೂರ್ಣಾಂಕ pidref_ಒಳ_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        ಪ್ರತಿಜ್ಞಾ(!pid1 || pidref_is_ಗಣ(pid1));
        ಪ್ರತಿಜ್ಞಾ(!pid2 || pidref_is_ಗಣ(pid2));
        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ಆದರೆ (pidref_equal(pid1, pid2))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (!pid1)
                ns1 = namespace_open_by_type(type);
        ಇಲ್ಲದಿದ್ದರೆ
                ns1 = pidref_namespace_open_by_type(pid1, type);
        ಆದರೆ (ns1 < 0)
                ಹಿಂತಿರುಗಿ ns1;

        ಆದರೆ (!pid2)
                ns2 = namespace_open_by_type(type);
        ಇಲ್ಲದಿದ್ದರೆ
                ns2 = pidref_namespace_open_by_type(pid2, type);
        ಆದರೆ (ns2 < 0)
                ಹಿಂತಿರುಗಿ ns2;

        ಹಿಂತಿರುಗಿ fd_inode_same(ns1, ns2);
}

ಪೂರ್ಣಾಂಕ ಒಳ_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        ಪ್ರತಿಜ್ಞಾ(pid1 >= 0);
        ಪ್ರತಿಜ್ಞಾ(pid2 >= 0);
        ಹಿಂತಿರುಗಿ pidref_ಒಳ_same_namespace(pid1 == 0 ? ಖಾಲಿ : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ಖಾಲಿ : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

ಪೂರ್ಣಾಂಕ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        ಪೂರ್ಣಾಂಕ r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        ಪ್ರತಿಜ್ಞಾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_(pidref_ಮುಗಿದ) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        ಗಾಗಿ (;;) {
                _cleanup_(pidref_ಮುಗಿದ) PidRef ಪೋಷಕ = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &ಪೋಷಕ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                r = pidref_ಒಳ_same_namespace(c, &ಪೋಷಕ, type);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        ಆದರೆ (pidref_is_ಗಣ(&current))
                                *ret = TAKE_PIDREF(current);
                        ಇಲ್ಲದಿದ್ದರೆ {
                                r = pidref_copy(c, ret);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        ಹಿಂತಿರುಗಿ 0;
                }

                pidref_ಮುಗಿದ(&current);
                current = TAKE_PIDREF(ಪೋಷಕ);
                c = &current;
        }
}

ಪೂರ್ಣಾಂಕ detach_mount_namespace(ಶೂನ್ಯ) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        ಆದರೆ (unshare(CLONE_NEWNS) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        ಆದರೆ (mount(ಖಾಲಿ, "/", ಖಾಲಿ, MS_SLAVE | MS_REC, ಖಾಲಿ) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        ಆದರೆ (mount(ಖಾಲಿ, "/", ಖಾಲಿ, MS_SHARED | MS_REC, ಖಾಲಿ) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ detach_mount_namespace_harder(uid_t ಗುರಿ_uid, gid_t ಗುರಿ_gid) {
        uid_t from_uid;
        gid_t from_gid;
        ಪೂರ್ಣಾಂಕ r;

        /* Tried detach_mount_namespace() first. If that doesn't work due to permissions, opens up an
         * unprivileged user namespace with a mapping of the originating UID/GID to the specified target
         * UID/GID. Then, tries detach_mount_namespace() again.
         *
         * Or in other words: tries much harder to get a mount namespace, making use of unprivileged user
         * namespaces if need be.
         *
         * Note that after this function completed:
         *
         *    → if we had privs, afterwards uids/gids on files and processes are as before
         *
         *    → if we had no privs, our own id and all our files will show up owned by target_uid/target_gid,
         *    and everything else owned by nobody.
         *
         * Yes, that's quite a difference. */

        ಆದರೆ (!uid_is_valid(ಗುರಿ_uid))
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಆದರೆ (!gid_is_valid(ಗುರಿ_gid))
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = detach_mount_namespace();
        ಆದರೆ (r != -EPERM)
                ಹಿಂತಿರುಗಿ r;

        from_uid = getuid();
        from_gid = getgid();

        ಆದರೆ (unshare(CLONE_NEWUSER) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", ಗುರಿ_uid, from_uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", ಗುರಿ_gid, from_gid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write gid map: %m");

        ಹಿಂತಿರುಗಿ detach_mount_namespace();
}

ಪೂರ್ಣಾಂಕ detach_mount_namespace_userns(ಪೂರ್ಣಾಂಕ userns_fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);

        ಆದರೆ (setns(userns_fd, CLONE_NEWUSER) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to become root in user namespace: %m");

        ಹಿಂತಿರುಗಿ detach_mount_namespace();
}

ಪೂರ್ಣಾಂಕ parse_userns_uid_range(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uid_t *ret_uid_ಸರಿಸು, uid_t *ret_uid_range) {
        _cleanup_free_ ಅಕ್ಷರ *ಬಫರ್ = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *range, *ಸರಿಸು;
        ಪೂರ್ಣಾಂಕ r;
        uid_t uid_ಸರಿಸು, uid_range = 65536;

        ಪ್ರತಿಜ್ಞಾ(s);

        range = strchr(s, ':');
        ಆದರೆ (range) {
                ಬಫರ್ = strndup(s, range - s);
                ಆದರೆ (!ಬಫರ್)
                        ಹಿಂತಿರುಗಿ log_oom();
                ಸರಿಸು = ಬಫರ್;

                range++;
                r = ಸುರಕ್ಷಿತ_atou32(range, &uid_range);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } ಇಲ್ಲದಿದ್ದರೆ
                ಸರಿಸು = s;

        r = parse_uid(ಸರಿಸು, &uid_ಸರಿಸು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        ಆದರೆ (!userns_ಸರಿಸು_range_valid(uid_ಸರಿಸು, uid_range))
                ಹಿಂತಿರುಗಿ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        ಆದರೆ (ret_uid_ಸರಿಸು)
                *ret_uid_ಸರಿಸು = uid_ಸರಿಸು;

        ಆದರೆ (ret_uid_range)
                *ret_uid_range = uid_range;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ userns_acquire_ಖಾಲಿಗಳು(ಶೂನ್ಯ) {
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pid = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        r = pidref_ಸುರಕ್ಷಿತ_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(r > 0);

        ಹಿಂತಿರುಗಿ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ಪೂರ್ಣಾಂಕ userns_acquire(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uid_map, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *gid_map, bool setgroups_deny) {
        ಅಕ್ಷರ ಮಾರ್ಗ[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pid = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(uid_map);
        ಪ್ರತಿಜ್ಞಾ(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_ಸುರಕ್ಷಿತ_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಪ್ರತಿಜ್ಞಾ(r > 0);

        xsprintf(ಮಾರ್ಗ, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(ಮಾರ್ಗ, uid_map, WRITE_STRING_FILE_DISABLE_ಬಫರ್);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write UID map: %m");

        ಆದರೆ (setgroups_deny) {
                xsprintf(ಮಾರ್ಗ, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(ಮಾರ್ಗ, "deny", WRITE_STRING_FILE_DISABLE_ಬಫರ್);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(ಮಾರ್ಗ, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(ಮಾರ್ಗ, gid_map, WRITE_STRING_FILE_DISABLE_ಬಫರ್);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to write GID map: %m");

        ಹಿಂತಿರುಗಿ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ಪೂರ್ಣಾಂಕ userns_acquire_self_ಬೇರು(ಶೂನ್ಯ) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ ಅಕ್ಷರ *uid_map = ಖಾಲಿ, *gid_map = ಖಾಲಿ;
        ಆದರೆ (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        ಆದರೆ (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

ಪೂರ್ಣಾಂಕ userns_enter_and_pin(ಪೂರ್ಣಾಂಕ userns_fd, PidRef *ret) {
        _cleanup_close_pair_ ಪೂರ್ಣಾಂಕ pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        ಅಕ್ಷರ x;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (pipe2(pfd, O_CLOEXEC) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = pidref_ಸುರಕ್ಷಿತ_fork_ತುಂಬಿದ(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ಖಾಲಿ,
                        (ಪೂರ್ಣಾಂಕ[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0) {
                /* Child. */

                ಆದರೆ (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = ಸುರಕ್ಷಿತ_close(userns_fd);

                n = ಲಿಖ(pfd[1], &(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ) { 'x' }, 1);
                ಆದರೆ (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                ಪ್ರತಿಜ್ಞಾ(n == 1);

                freeze();
        }

        pfd[1] = ಸುರಕ್ಷಿತ_close(pfd[1]);

        n = ಪಠ(pfd[0], &x, 1);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ -errno;
        ಆದರೆ (n == 0)
                ಹಿಂತಿರುಗಿ -EPROTO;
        ಪ್ರತಿಜ್ಞಾ(n == 1);
        ಪ್ರತಿಜ್ಞಾ(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        ಹಿಂತಿರುಗಿ 0;
}

bool userns_supported(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ access("/proc/self/uid_map", F_OK) >= 0;
}

ಪೂರ್ಣಾಂಕ userns_get_ತಳ_uid(ಪೂರ್ಣಾಂಕ userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        uid_t uid;
        r = uid_map_ಹುಡುಕು_ಬೇರು(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        gid_t gid;
        r = uid_map_ಹುಡುಕು_ಬೇರು(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!ret_gid && uid != gid)
                ಹಿಂತಿರುಗಿ -EUCLEAN;

        ಆದರೆ (ret_uid)
                *ret_uid = uid;
        ಆದರೆ (ret_gid)
                *ret_gid = gid;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ userns_has_ಬೇರು_mapping(ಪೂರ್ಣಾಂಕ userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ಖಾಲಿ, *gid_range = ಖಾಲಿ;
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_ತುಂಬು_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_ತುಂಬು_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        ಆದರೆ (uid_range_is_ಖಾಲಿಗಳು(uid_range) || uid_range_is_ಖಾಲಿಗಳು(gid_range))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        ಹಿಂತಿರುಗಿ uid_range_ಒಳಗೊಂಡಿದೆ(uid_range, 0) && uid_range_ಒಳಗೊಂಡಿದೆ(gid_range, 0);
}

ಪೂರ್ಣಾಂಕ process_is_owned_by_uid(ಸ್ಥಿರಾಂಕ PidRef *pidref, uid_t uid) {
        ಪೂರ್ಣಾಂಕ r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        ಪ್ರತಿಜ್ಞಾ(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        ಆದರೆ (process_uid == uid)
                ಹಿಂತಿರುಗಿ true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ ಪೂರ್ಣಾಂಕ userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        ಆದರೆ (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (userns_fd < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                ಆದರೆ (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        ಹಿಂತಿರುಗಿ false;
                }

                ಆದರೆ (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                ಆದರೆ (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        ಹಿಂತಿರುಗಿ true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                ಆದರೆ (iteration > 16)
                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ ಪೂರ್ಣಾಂಕ ಪೋಷಕ_fd = ioctl(userns_fd, NS_GET_USERNS);
                ಆದರೆ (ಪೋಷಕ_fd < 0) {
                        ಆದರೆ (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                ಹಿಂತಿರುಗಿ false;
                        }

                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, ಪೋಷಕ_fd);
        }
}

ಪೂರ್ಣಾಂಕ namespace_open_by_id(uint64_t ns_id) {
        ಪೂರ್ಣಾಂಕ r;

        /* Looks up a namespace by its unique boot-stable identifier and returns an O_PATH fd to it.
         * Requires kernel ≥ 6.13.
         *
         * Returns -ESTALE if the namespace no longer exists, or if the kernel refuses the lookup
         * for permission reasons. The latter happens outside the initial user namespace: the
         * kernel only permits open_by_handle_at() on nsfs when the caller is in the initial user
         * and pid namespaces with CAP_SYS_ADMIN, with a narrow exception for lookups of the
         * caller's own user namespace and its ancestors. To avoid conflating "namespace is dead"
         * with "kernel refused us", we refuse early with -EPERM when we aren't in the initial
         * user/pid namespace or missing CAP_SYS_ADMIN and let the caller skip the check. */

        ಆದರೆ (ns_id == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EPERM;

        /* The natural way to write this would be a compound designated initializer:
         *
         *         union { ... } fh = {
         *                 .file_handle.handle_bytes = sizeof(struct nsfs_file_handle),
         *                 .file_handle.handle_type = FILEID_NSFS,
         *         };
         *
         * but that only zero-initializes the named struct members of struct file_handle.
         * struct file_handle ends with a flexible array (`unsigned char f_handle[]`), whose
         * storage comes from the overlapping `space[]` member of the union. Bytes in that storage
         * are not covered by the partial struct initializer and end up as stack garbage. Zero the
         * entire union first, then fill in the fields explicitly. */

        ಒಕ್ಕೂಟ {
                ರಚನೆ file_handle file_handle;
                uint8_t space[offsetof(ರಚನೆ file_handle, f_handle) + ಗಾತ್ರ(ರಚನೆ nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_ಬೈಟುಗಳು = ಗಾತ್ರ(ರಚನೆ nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        ಹಿಂತಿರುಗಿ RET_NERRNO(open_by_handle_at(FD_NSFS_ಬೇರು, &fh.file_handle, O_ಮಾರ್ಗ|O_CLOEXEC));
}

ಪೂರ್ಣಾಂಕ is_idmapping_supported(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        ಆದರೆ (!mount_new_api_supported())
                ಹಿಂತಿರುಗಿ false;

        userns_fd = r = userns_acquire_self_ಬೇರು();
        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                ಹಿಂತಿರುಗಿ false;
        }
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", ಮಾರ್ಗ);

        dir_fd = r = RET_NERRNO(open(ಮಾರ್ಗ, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", ಮಾರ್ಗ);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", ಮಾರ್ಗ);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ,
                       &(ರಚನೆ mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, ಗಾತ್ರ(ರಚನೆ mount_attr)));
        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", ಮಾರ್ಗ);

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ netns_acquire(ಶೂನ್ಯ) {
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pid = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_ಸುರಕ್ಷಿತ_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to fork process into new netns: %m");
        ಪ್ರತಿಜ್ಞಾ(r > 0);

        ಹಿಂತಿರುಗಿ pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
