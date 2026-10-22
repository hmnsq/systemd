/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/magic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/nsfs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/sockios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sched.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mount.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "capability-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dlfcn-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mountpoint-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "namespace-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidfd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uid-range.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

ਅਚਲ ਬਣਤਰ namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        [NAMESPACE_ਸਮਾਂ]   =  { "time",   "ns/time",   CLONE_NEWTIME,   PIDFD_GET_ਸਮਾਂ_NAMESPACE,   PROC_ਸਮਾਂ_INIT_INO   },
        {}, /* Allow callers to iterate over the array without using _NAMESPACE_TYPE_MAX. */
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ pid_namespace_ਰਸਤਾ(pid, type) procfs_file_alloca(pid, namespace_info[type].proc_ਰਸਤਾ)

NamespaceType clone_flag_ਮੰਜ਼ਿਲ_namespace_type(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ clone_flag) {
        ਲਈ (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                ਜੇ (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        ਵਾਪਸ t;

        ਵਾਪਸ _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ਅਚਲ ਅੱਖਰ *p = pid_namespace_ਰਸਤਾ(0, type);
        ਵਾਪਸ access(p, F_OK) >= 0;
}

ਸਥਿਰ ਪੂਰਨ pidref_namespace_open_by_type_ਅੰਦਰੂਨੀ(ਅਚਲ PidRef *pidref, NamespaceType type, bool *need_verify) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref_is_ਸੈੱਟ(pidref));
        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                ਜੇ (r == -ENOPKG)
                        ਵਾਪਸ log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                ਜੇ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ਵਾਪਸ r;
        }

        ਜੇ (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ ਪੂਰਨ nsfd = -EBADF;
        ਅਚਲ ਅੱਖਰ *p;

        p = pid_namespace_ਰਸਤਾ(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        ਜੇ (nsfd == -ENOENT) {
                r = proc_mounted();
                ਜੇ (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        ਵਾਪਸ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                ਜੇ (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        ਵਾਪਸ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        ਜੇ (nsfd < 0)
                ਵਾਪਸ nsfd;

        ਜੇ (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ TAKE_FD(nsfd);
}

ਪੂਰਨ pidref_namespace_open_by_type(ਅਚਲ PidRef *pidref, NamespaceType type) {
        ਵਾਪਸ pidref_namespace_open_by_type_ਅੰਦਰੂਨੀ(pidref, type, ਨਿਰਾ);
}

ਪੂਰਨ namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_ਪੂਰਾ) PidRef self = PIDREF_NULL;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_ਸੈੱਟ_self(&self);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ pidref_namespace_open_by_type(&self, type);
}

ਪੂਰਨ pidref_namespace_open(
                ਅਚਲ PidRef *pidref,
                ਪੂਰਨ *ret_pidns_fd,
                ਪੂਰਨ *ret_mntns_fd,
                ਪੂਰਨ *ret_netns_fd,
                ਪੂਰਨ *ret_userns_fd,
                ਪੂਰਨ *ret_ਜੜ੍ਹ_fd) {

        _cleanup_close_ ਪੂਰਨ pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, ਜੜ੍ਹ_fd = -EBADF;
        bool need_verify = false;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref_is_ਸੈੱਟ(pidref));

        ਜੇ (pidref_is_remote(pidref))
                ਵਾਪਸ -EREMOTE;

        ਜੇ (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_ਅੰਦਰੂਨੀ(pidref, NAMESPACE_PID, &need_verify);
                ਜੇ (pidns_fd < 0)
                        ਵਾਪਸ pidns_fd;
        }

        ਜੇ (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_ਅੰਦਰੂਨੀ(pidref, NAMESPACE_MOUNT, &need_verify);
                ਜੇ (mntns_fd < 0)
                        ਵਾਪਸ mntns_fd;
        }

        ਜੇ (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_ਅੰਦਰੂਨੀ(pidref, NAMESPACE_NET, &need_verify);
                ਜੇ (netns_fd < 0)
                        ਵਾਪਸ netns_fd;
        }

        ਜੇ (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_ਅੰਦਰੂਨੀ(pidref, NAMESPACE_USER, &need_verify);
                ਜੇ (userns_fd < 0 && userns_fd != -ENOPKG)
                        ਵਾਪਸ userns_fd;
        }

        ਜੇ (ret_ਜੜ੍ਹ_fd) {
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ;

                ਜੜ੍ਹ = procfs_file_alloca(pidref->pid, "root");
                ਜੜ੍ਹ_fd = RET_NERRNO(open(ਜੜ੍ਹ, O_CLOEXEC|O_DIRECTORY));
                ਜੇ (ਜੜ੍ਹ_fd == -ENOENT && proc_mounted() == 0)
                        ਵਾਪਸ -ENOSYS;
                ਜੇ (ਜੜ੍ਹ_fd < 0)
                        ਵਾਪਸ ਜੜ੍ਹ_fd;

                need_verify = true;
        }

        ਜੇ (need_verify) {
                r = pidref_verify(pidref);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        ਜੇ (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        ਜੇ (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        ਜੇ (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        ਜੇ (ret_ਜੜ੍ਹ_fd)
                *ret_ਜੜ੍ਹ_fd = TAKE_FD(ਜੜ੍ਹ_fd);

        ਵਾਪਸ 0;
}

ਪੂਰਨ namespace_open(
                pid_t pid,
                ਪੂਰਨ *ret_pidns_fd,
                ਪੂਰਨ *ret_mntns_fd,
                ਪੂਰਨ *ret_netns_fd,
                ਪੂਰਨ *ret_userns_fd,
                ਪੂਰਨ *ret_ਜੜ੍ਹ_fd) {

        _cleanup_(pidref_ਪੂਰਾ) PidRef pidref = PIDREF_NULL;
        ਪੂਰਨ r;

        r = pidref_ਸੈੱਟ_pid(&pidref, pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_ਜੜ੍ਹ_fd);
}

ਪੂਰਨ namespace_enter_ਭਰਿਆ(
                ਪੂਰਨ pidns_fd,
                ਪੂਰਨ mntns_fd,
                ਪੂਰਨ netns_fd,
                ਪੂਰਨ userns_fd,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                NamespaceEnterFlags flags) {

        ਪੂਰਨ r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        ਖੰਡ_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        ਜੇ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        pidns_fd = -EBADF;
        }

        ਜੇ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        mntns_fd = -EBADF;
        }

        ਜੇ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        netns_fd = -EBADF;
        }

        ਜੇ (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        ਜੇ (userns_fd >= 0 && FLAGS_ਸੈੱਟ(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ਜੜ੍ਹ_UNMAPPED)) {
                r = userns_has_ਜੜ੍ਹ_mapping(userns_fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        bool have_cap_sys_admin = r > 0;

        ਜੇ (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                ਜੇ (userns_fd >= 0) {
                        ਜੇ (setns(userns_fd, CLONE_NEWUSER) < 0)
                                ਵਾਪਸ -errno;
                } ਨਹੀਂਤਾਂ ਜੇ (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || ਜੜ੍ਹ_fd >= 0)
                        ਵਾਪਸ log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        ਜੇ (pidns_fd >= 0)
                ਜੇ (setns(pidns_fd, CLONE_NEWPID) < 0)
                        ਵਾਪਸ -errno;

        ਜੇ (mntns_fd >= 0)
                ਜੇ (setns(mntns_fd, CLONE_NEWNS) < 0)
                        ਵਾਪਸ -errno;

        ਜੇ (netns_fd >= 0)
                ਜੇ (setns(netns_fd, CLONE_NEWNET) < 0)
                        ਵਾਪਸ -errno;

        ਜੇ (userns_fd >= 0 && have_cap_sys_admin)
                ਜੇ (setns(userns_fd, CLONE_NEWUSER) < 0)
                        ਵਾਪਸ -errno;

        ਜੇ (ਜੜ੍ਹ_fd >= 0) {
                ਜੇ (fchdir(ਜੜ੍ਹ_fd) < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (chroot(".") < 0)
                        ਵਾਪਸ -errno;
        }

        ਜੇ (userns_fd < 0)
                ਵਾਪਸ 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        ਜੇ (keep_uid_gid)
                ਵਾਪਸ 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        ਜੇ (setresgid(0, 0, 0) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        ਜੇ (setresuid(0, 0, 0) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to become root in user namespace: %m");

        ਵਾਪਸ 0;
}

ਪੂਰਨ fd_is_namespace(ਪੂਰਨ fd, NamespaceType type) {
        ਪੂਰਨ r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        ਜੇ (r <= 0)
                ਵਾਪਸ r;

        ਜੇ (type < 0)
                ਵਾਪਸ true;

        ਪੂਰਨ clone_flag = ioctl(fd, NS_GET_NSTYPE);
        ਜੇ (clone_flag < 0) {
                ਜੇ (errno != ENOTTY)
                        ਵਾਪਸ -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
                r = fd_get_ਰਸਤਾ(fd, &ਰਸਤਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                ਜੇ (ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                        ਵਾਪਸ -ENOTTY;

                ਅਚਲ ਅੱਖਰ *p = startswith(ਰਸਤਾ, namespace_info[type].proc_name);
                ਜੇ (!p)
                        ਵਾਪਸ false;

                ਵਾਪਸ startswith(p, ":[") != ਨਿਰਾ;
        }

        NamespaceType ਮਿਲਿਆ_type = clone_flag_ਮੰਜ਼ਿਲ_namespace_type(clone_flag);
        ਜੇ (ਮਿਲਿਆ_type < 0)
                ਵਾਪਸ -EBADF; /* Uh? Unknown namespace type? */

        ਵਾਪਸ ਮਿਲਿਆ_type == type;
}

ਪੂਰਨ is_our_namespace(ਪੂਰਨ fd, NamespaceType type) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        ਜੇ (r < 0 && r != -ENOTTY)
                ਵਾਪਸ r;
        ਜੇ (r == 0) /* Not a namespace or not of the right type? */
                ਵਾਪਸ -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ ਪੂਰਨ our_ns = namespace_open_by_type(type);
        ਜੇ (our_ns < 0)
                ਵਾਪਸ our_ns;

        r = fd_inode_same(fd, our_ns);
        ਜੇ (r == 0 && !type_verified)
                ਵਾਪਸ -ENOTTY;

        ਵਾਪਸ r;
}

ਪੂਰਨ are_our_namespaces(ਪੂਰਨ pidns_fd, ਪੂਰਨ mntns_fd, ਪੂਰਨ netns_fd, ਪੂਰਨ userns_fd, ਪੂਰਨ ਜੜ੍ਹ_fd) {
        ਪੂਰਨ r;

        ਜੇ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ਜੜ੍ਹ_fd >= 0) {
                r = dir_fd_is_ਜੜ੍ਹ(ਜੜ੍ਹ_fd);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ true;
}

ਪੂਰਨ ਜਾਲ_namespace_is_init(ਪੂਰਨ socket_fd) {
        ਬਣਤਰ stat a, b;
        ਪੂਰਨ r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_ਰਸਤਾ(1, NAMESPACE_NET), &a));
        ਜੇ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ਜੇ (r < 0)
                        ਵਾਪਸ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ਵਾਪਸ r ? true : -ENOSYS;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (socket_fd >= 0) {
                _cleanup_close_ ਪੂਰਨ netns = ioctl(socket_fd, SIOCGSKNS);
                ਜੇ (netns < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (fstat(netns, &b) < 0)
                        ਵਾਪਸ -errno;
        } ਨਹੀਂਤਾਂ {
                ਜੇ (stat(pid_namespace_ਰਸਤਾ(0, NAMESPACE_NET), &b) < 0)
                        ਵਾਪਸ -errno;
        }

        ਵਾਪਸ stat_inode_same(&a, &b);
}

ਪੂਰਨ namespace_is_init(NamespaceType type) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(type < _NAMESPACE_TYPE_MAX);

        ਜੇ (type == NAMESPACE_NET)
                ਵਾਪਸ ਜਾਲ_namespace_is_init(/* socket_fd= */ -EBADF);

        ਜੇ (namespace_info[type].ਜੜ੍ਹ_inode == 0)
                ਵਾਪਸ -EBADR; /* Cannot answer this question */

        ਅਚਲ ਅੱਖਰ *p = pid_namespace_ਰਸਤਾ(0, type);

        ਬਣਤਰ stat st;
        r = RET_NERRNO(stat(p, &st));
        ਜੇ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ਜੇ (r < 0)
                        ਵਾਪਸ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ਵਾਪਸ r ? true : -ENOSYS;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ st.st_ino == namespace_info[type].ਜੜ੍ਹ_inode;
}

ਪੂਰਨ pidref_ਵਿੱਚ_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ ਪੂਰਨ ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        ਪ੍ਰਤਿਜ੍ਞਾ(!pid1 || pidref_is_ਸੈੱਟ(pid1));
        ਪ੍ਰਤਿਜ੍ਞਾ(!pid2 || pidref_is_ਸੈੱਟ(pid2));
        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ਜੇ (pidref_equal(pid1, pid2))
                ਵਾਪਸ true;

        ਜੇ (!pid1)
                ns1 = namespace_open_by_type(type);
        ਨਹੀਂਤਾਂ
                ns1 = pidref_namespace_open_by_type(pid1, type);
        ਜੇ (ns1 < 0)
                ਵਾਪਸ ns1;

        ਜੇ (!pid2)
                ns2 = namespace_open_by_type(type);
        ਨਹੀਂਤਾਂ
                ns2 = pidref_namespace_open_by_type(pid2, type);
        ਜੇ (ns2 < 0)
                ਵਾਪਸ ns2;

        ਵਾਪਸ fd_inode_same(ns1, ns2);
}

ਪੂਰਨ ਵਿੱਚ_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pid1 >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(pid2 >= 0);
        ਵਾਪਸ pidref_ਵਿੱਚ_same_namespace(pid1 == 0 ? ਨਿਰਾ : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ਨਿਰਾ : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

ਪੂਰਨ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        ਪੂਰਨ r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        ਪ੍ਰਤਿਜ੍ਞਾ(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        _cleanup_(pidref_ਪੂਰਾ) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        ਲਈ (;;) {
                _cleanup_(pidref_ਪੂਰਾ) PidRef ਮਾਪਾ = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &ਮਾਪਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                r = pidref_ਵਿੱਚ_same_namespace(c, &ਮਾਪਾ, type);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        ਜੇ (pidref_is_ਸੈੱਟ(&current))
                                *ret = TAKE_PIDREF(current);
                        ਨਹੀਂਤਾਂ {
                                r = pidref_copy(c, ret);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        ਵਾਪਸ 0;
                }

                pidref_ਪੂਰਾ(&current);
                current = TAKE_PIDREF(ਮਾਪਾ);
                c = &current;
        }
}

ਪੂਰਨ detach_mount_namespace(ਖਾਲੀ) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        ਜੇ (unshare(CLONE_NEWNS) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        ਜੇ (mount(ਨਿਰਾ, "/", ਨਿਰਾ, MS_SLAVE | MS_REC, ਨਿਰਾ) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        ਜੇ (mount(ਨਿਰਾ, "/", ਨਿਰਾ, MS_SHARED | MS_REC, ਨਿਰਾ) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        ਵਾਪਸ 0;
}

ਪੂਰਨ detach_mount_namespace_harder(uid_t ਨਿਸ਼ਾਨਾ_uid, gid_t ਨਿਸ਼ਾਨਾ_gid) {
        uid_t from_uid;
        gid_t from_gid;
        ਪੂਰਨ r;

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

        ਜੇ (!uid_is_valid(ਨਿਸ਼ਾਨਾ_uid))
                ਵਾਪਸ -EINVAL;
        ਜੇ (!gid_is_valid(ਨਿਸ਼ਾਨਾ_gid))
                ਵਾਪਸ -EINVAL;

        r = detach_mount_namespace();
        ਜੇ (r != -EPERM)
                ਵਾਪਸ r;

        from_uid = getuid();
        from_gid = getgid();

        ਜੇ (unshare(CLONE_NEWUSER) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", ਨਿਸ਼ਾਨਾ_uid, from_uid);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", ਨਿਸ਼ਾਨਾ_gid, from_gid);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to write gid map: %m");

        ਵਾਪਸ detach_mount_namespace();
}

ਪੂਰਨ detach_mount_namespace_userns(ਪੂਰਨ userns_fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(userns_fd >= 0);

        ਜੇ (setns(userns_fd, CLONE_NEWUSER) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to become root in user namespace: %m");

        ਵਾਪਸ detach_mount_namespace();
}

ਪੂਰਨ parse_userns_uid_range(ਅਚਲ ਅੱਖਰ *s, uid_t *ret_uid_ਖਿਸਕਾਓ, uid_t *ret_uid_range) {
        _cleanup_free_ ਅੱਖਰ *ਬਫਰ = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *range, *ਖਿਸਕਾਓ;
        ਪੂਰਨ r;
        uid_t uid_ਖਿਸਕਾਓ, uid_range = 65536;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        range = strchr(s, ':');
        ਜੇ (range) {
                ਬਫਰ = strndup(s, range - s);
                ਜੇ (!ਬਫਰ)
                        ਵਾਪਸ log_oom();
                ਖਿਸਕਾਓ = ਬਫਰ;

                range++;
                r = ਸੁਰੱਖਿਅਤ_atou32(range, &uid_range);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } ਨਹੀਂਤਾਂ
                ਖਿਸਕਾਓ = s;

        r = parse_uid(ਖਿਸਕਾਓ, &uid_ਖਿਸਕਾਓ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        ਜੇ (!userns_ਖਿਸਕਾਓ_range_valid(uid_ਖਿਸਕਾਓ, uid_range))
                ਵਾਪਸ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        ਜੇ (ret_uid_ਖਿਸਕਾਓ)
                *ret_uid_ਖਿਸਕਾਓ = uid_ਖਿਸਕਾਓ;

        ਜੇ (ret_uid_range)
                *ret_uid_range = uid_range;

        ਵਾਪਸ 0;
}

ਪੂਰਨ userns_acquire_ਖਾਲੀ_ਥਾਵਾਂ(ਖਾਲੀ) {
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pid = PIDREF_NULL;
        ਪੂਰਨ r;

        r = pidref_ਸੁਰੱਖਿਅਤ_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

        ਵਾਪਸ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ਪੂਰਨ userns_acquire(ਅਚਲ ਅੱਖਰ *uid_map, ਅਚਲ ਅੱਖਰ *gid_map, bool setgroups_deny) {
        ਅੱਖਰ ਰਸਤਾ[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pid = PIDREF_NULL;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(uid_map);
        ਪ੍ਰਤਿਜ੍ਞਾ(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_ਸੁਰੱਖਿਅਤ_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

        xsprintf(ਰਸਤਾ, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(ਰਸਤਾ, uid_map, WRITE_STRING_FILE_DISABLE_ਬਫਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to write UID map: %m");

        ਜੇ (setgroups_deny) {
                xsprintf(ਰਸਤਾ, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(ਰਸਤਾ, "deny", WRITE_STRING_FILE_DISABLE_ਬਫਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(ਰਸਤਾ, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(ਰਸਤਾ, gid_map, WRITE_STRING_FILE_DISABLE_ਬਫਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to write GID map: %m");

        ਵਾਪਸ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ਪੂਰਨ userns_acquire_self_ਜੜ੍ਹ(ਖਾਲੀ) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ ਅੱਖਰ *uid_map = ਨਿਰਾ, *gid_map = ਨਿਰਾ;
        ਜੇ (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                ਵਾਪਸ -ENOMEM;
        ਜੇ (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

ਪੂਰਨ userns_enter_and_pin(ਪੂਰਨ userns_fd, PidRef *ret) {
        _cleanup_close_pair_ ਪੂਰਨ pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        ਅੱਖਰ x;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(userns_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (pipe2(pfd, O_CLOEXEC) < 0)
                ਵਾਪਸ -errno;

        r = pidref_ਸੁਰੱਖਿਅਤ_fork_ਭਰਿਆ(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ਨਿਰਾ,
                        (ਪੂਰਨ[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0) {
                /* Child. */

                ਜੇ (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = ਸੁਰੱਖਿਅਤ_close(userns_fd);

                n = ਲਿਖ(pfd[1], &(ਅਚਲ ਅੱਖਰ) { 'x' }, 1);
                ਜੇ (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                ਪ੍ਰਤਿਜ੍ਞਾ(n == 1);

                freeze();
        }

        pfd[1] = ਸੁਰੱਖਿਅਤ_close(pfd[1]);

        n = ਪਠ(pfd[0], &x, 1);
        ਜੇ (n < 0)
                ਵਾਪਸ -errno;
        ਜੇ (n == 0)
                ਵਾਪਸ -EPROTO;
        ਪ੍ਰਤਿਜ੍ਞਾ(n == 1);
        ਪ੍ਰਤਿਜ੍ਞਾ(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        ਵਾਪਸ 0;
}

bool userns_supported(ਖਾਲੀ) {
        ਵਾਪਸ access("/proc/self/uid_map", F_OK) >= 0;
}

ਪੂਰਨ userns_get_ਅਧਾਰ_uid(ਪੂਰਨ userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        uid_t uid;
        r = uid_map_ਖੋਜ_ਜੜ੍ਹ(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        gid_t gid;
        r = uid_map_ਖੋਜ_ਜੜ੍ਹ(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!ret_gid && uid != gid)
                ਵਾਪਸ -EUCLEAN;

        ਜੇ (ret_uid)
                *ret_uid = uid;
        ਜੇ (ret_gid)
                *ret_gid = gid;

        ਵਾਪਸ 0;
}

ਪੂਰਨ userns_has_ਜੜ੍ਹ_mapping(ਪੂਰਨ userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ਨਿਰਾ, *gid_range = ਨਿਰਾ;
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਅਚਲ ਅੱਖਰ *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_ਲੱਦੋ_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_ਲੱਦੋ_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        ਜੇ (uid_range_is_ਖਾਲੀ_ਥਾਵਾਂ(uid_range) || uid_range_is_ਖਾਲੀ_ਥਾਵਾਂ(gid_range))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        ਵਾਪਸ uid_range_ਸ਼ਾਮਲ_ਹੈ(uid_range, 0) && uid_range_ਸ਼ਾਮਲ_ਹੈ(gid_range, 0);
}

ਪੂਰਨ process_is_owned_by_uid(ਅਚਲ PidRef *pidref, uid_t uid) {
        ਪੂਰਨ r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        ਪ੍ਰਤਿਜ੍ਞਾ(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        ਜੇ (process_uid == uid)
                ਵਾਪਸ true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ ਪੂਰਨ userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        ਜੇ (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                ਵਾਪਸ false;
        ਜੇ (userns_fd < 0)
                ਵਾਪਸ log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                ਜੇ (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        ਵਾਪਸ false;
                }

                ਜੇ (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        ਵਾਪਸ log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                ਜੇ (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        ਵਾਪਸ true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                ਜੇ (iteration > 16)
                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ ਪੂਰਨ ਮਾਪਾ_fd = ioctl(userns_fd, NS_GET_USERNS);
                ਜੇ (ਮਾਪਾ_fd < 0) {
                        ਜੇ (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                ਵਾਪਸ false;
                        }

                        ਵਾਪਸ log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, ਮਾਪਾ_fd);
        }
}

ਪੂਰਨ namespace_open_by_id(uint64_t ns_id) {
        ਪੂਰਨ r;

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

        ਜੇ (ns_id == 0)
                ਵਾਪਸ -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EPERM;

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

        ਸੰਘ {
                ਬਣਤਰ file_handle file_handle;
                uint8_t space[offsetof(ਬਣਤਰ file_handle, f_handle) + ਆਕਾਰ(ਬਣਤਰ nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_ਬਾਈਟਾਂ = ਆਕਾਰ(ਬਣਤਰ nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        ਵਾਪਸ RET_NERRNO(open_by_handle_at(FD_NSFS_ਜੜ੍ਹ, &fh.file_handle, O_ਰਸਤਾ|O_CLOEXEC));
}

ਪੂਰਨ is_idmapping_supported(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        ਜੇ (!mount_new_api_supported())
                ਵਾਪਸ false;

        userns_fd = r = userns_acquire_self_ਜੜ੍ਹ();
        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ਵਾਪਸ false;
        ਜੇ (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                ਵਾਪਸ false;
        }
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", ਰਸਤਾ);

        dir_fd = r = RET_NERRNO(open(ਰਸਤਾ, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", ਰਸਤਾ);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", ਰਸਤਾ);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ,
                       &(ਬਣਤਰ mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, ਆਕਾਰ(ਬਣਤਰ mount_attr)));
        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", ਰਸਤਾ);

        ਵਾਪਸ true;
}

ਪੂਰਨ netns_acquire(ਖਾਲੀ) {
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pid = PIDREF_NULL;
        ਪੂਰਨ r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_ਸੁਰੱਖਿਅਤ_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to fork process into new netns: %m");
        ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

        ਵਾਪਸ pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
