/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <linux/magic.h>
#അന്തര്ഭാവ <linux/nsfs.h>
#അന്തര്ഭാവ <linux/sockios.h>
#അന്തര്ഭാവ <sched.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/mount.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "capability-util.h"
#അന്തര്ഭാവ "dlfcn-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mountpoint-util.h"
#അന്തര്ഭാവ "namespace-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidfd-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "uid-range.h"
#അന്തര്ഭാവ "unaligned.h"
#അന്തര്ഭാവ "user-util.h"

സ്ഥിരം ഘടന namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        [NAMESPACE_TIME]   =  { "time",   "ns/time",   CLONE_NEWTIME,   PIDFD_GET_TIME_NAMESPACE,   PROC_TIME_INIT_INO   },
        {}, /* Allow callers to iterate over the array without using _NAMESPACE_TYPE_MAX. */
};

#define pid_namespace_path(pid, type) procfs_file_alloca(pid, namespace_info[type].proc_path)

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag) {
        വേണ്ടി (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                എങ്കിൽ (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        തിരികെ t;

        തിരികെ _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        പ്രതിജ്ഞാ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        സ്ഥിരം അക്ഷരം *p = pid_namespace_path(0, type);
        തിരികെ access(p, F_OK) >= 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ pidref_namespace_open_by_type_internal(സ്ഥിരം PidRef *pidref, NamespaceType type, bool *need_verify) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref_is_set(pidref));
        പ്രതിജ്ഞാ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                എങ്കിൽ (r == -ENOPKG)
                        തിരികെ log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                എങ്കിൽ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        തിരികെ r;
        }

        എങ്കിൽ (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ പൂർണ്ണസംഖ്യ nsfd = -EBADF;
        സ്ഥിരം അക്ഷരം *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        എങ്കിൽ (nsfd == -ENOENT) {
                r = proc_mounted();
                എങ്കിൽ (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        തിരികെ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                എങ്കിൽ (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        തിരികെ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        എങ്കിൽ (nsfd < 0)
                തിരികെ nsfd;

        എങ്കിൽ (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ TAKE_FD(nsfd);
}

പൂർണ്ണസംഖ്യ pidref_namespace_open_by_type(സ്ഥിരം PidRef *pidref, NamespaceType type) {
        തിരികെ pidref_namespace_open_by_type_internal(pidref, type, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ pidref_namespace_open_by_type(&self, type);
}

പൂർണ്ണസംഖ്യ pidref_namespace_open(
                സ്ഥിരം PidRef *pidref,
                പൂർണ്ണസംഖ്യ *ret_pidns_fd,
                പൂർണ്ണസംഖ്യ *ret_mntns_fd,
                പൂർണ്ണസംഖ്യ *ret_netns_fd,
                പൂർണ്ണസംഖ്യ *ret_userns_fd,
                പൂർണ്ണസംഖ്യ *ret_വേര്_fd) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, വേര്_fd = -EBADF;
        bool need_verify = false;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref_is_set(pidref));

        എങ്കിൽ (pidref_is_remote(pidref))
                തിരികെ -EREMOTE;

        എങ്കിൽ (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                എങ്കിൽ (pidns_fd < 0)
                        തിരികെ pidns_fd;
        }

        എങ്കിൽ (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                എങ്കിൽ (mntns_fd < 0)
                        തിരികെ mntns_fd;
        }

        എങ്കിൽ (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                എങ്കിൽ (netns_fd < 0)
                        തിരികെ netns_fd;
        }

        എങ്കിൽ (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                എങ്കിൽ (userns_fd < 0 && userns_fd != -ENOPKG)
                        തിരികെ userns_fd;
        }

        എങ്കിൽ (ret_വേര്_fd) {
                സ്ഥിരം അക്ഷരം *വേര്;

                വേര് = procfs_file_alloca(pidref->pid, "root");
                വേര്_fd = RET_NERRNO(open(വേര്, O_CLOEXEC|O_DIRECTORY));
                എങ്കിൽ (വേര്_fd == -ENOENT && proc_mounted() == 0)
                        തിരികെ -ENOSYS;
                എങ്കിൽ (വേര്_fd < 0)
                        തിരികെ വേര്_fd;

                need_verify = true;
        }

        എങ്കിൽ (need_verify) {
                r = pidref_verify(pidref);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        എങ്കിൽ (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        എങ്കിൽ (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        എങ്കിൽ (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        എങ്കിൽ (ret_വേര്_fd)
                *ret_വേര്_fd = TAKE_FD(വേര്_fd);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ namespace_open(
                pid_t pid,
                പൂർണ്ണസംഖ്യ *ret_pidns_fd,
                പൂർണ്ണസംഖ്യ *ret_mntns_fd,
                പൂർണ്ണസംഖ്യ *ret_netns_fd,
                പൂർണ്ണസംഖ്യ *ret_userns_fd,
                പൂർണ്ണസംഖ്യ *ret_വേര്_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        r = pidref_set_pid(&pidref, pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_വേര്_fd);
}

പൂർണ്ണസംഖ്യ namespace_enter_full(
                പൂർണ്ണസംഖ്യ pidns_fd,
                പൂർണ്ണസംഖ്യ mntns_fd,
                പൂർണ്ണസംഖ്യ netns_fd,
                പൂർണ്ണസംഖ്യ userns_fd,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                NamespaceEnterFlags flags) {

        പൂർണ്ണസംഖ്യ r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        എങ്കിൽ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r > 0)
                        pidns_fd = -EBADF;
        }

        എങ്കിൽ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r > 0)
                        mntns_fd = -EBADF;
        }

        എങ്കിൽ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r > 0)
                        netns_fd = -EBADF;
        }

        എങ്കിൽ (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        എങ്കിൽ (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_വേര്_UNMAPPED)) {
                r = userns_has_വേര്_mapping(userns_fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        bool have_cap_sys_admin = r > 0;

        എങ്കിൽ (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                എങ്കിൽ (userns_fd >= 0) {
                        എങ്കിൽ (setns(userns_fd, CLONE_NEWUSER) < 0)
                                തിരികെ -errno;
                } അല്ലെങ്കിൽ എങ്കിൽ (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || വേര്_fd >= 0)
                        തിരികെ log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        എങ്കിൽ (pidns_fd >= 0)
                എങ്കിൽ (setns(pidns_fd, CLONE_NEWPID) < 0)
                        തിരികെ -errno;

        എങ്കിൽ (mntns_fd >= 0)
                എങ്കിൽ (setns(mntns_fd, CLONE_NEWNS) < 0)
                        തിരികെ -errno;

        എങ്കിൽ (netns_fd >= 0)
                എങ്കിൽ (setns(netns_fd, CLONE_NEWNET) < 0)
                        തിരികെ -errno;

        എങ്കിൽ (userns_fd >= 0 && have_cap_sys_admin)
                എങ്കിൽ (setns(userns_fd, CLONE_NEWUSER) < 0)
                        തിരികെ -errno;

        എങ്കിൽ (വേര്_fd >= 0) {
                എങ്കിൽ (fchdir(വേര്_fd) < 0)
                        തിരികെ -errno;

                എങ്കിൽ (chroot(".") < 0)
                        തിരികെ -errno;
        }

        എങ്കിൽ (userns_fd < 0)
                തിരികെ 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        എങ്കിൽ (keep_uid_gid)
                തിരികെ 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        എങ്കിൽ (setresgid(0, 0, 0) < 0)
                തിരികെ log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        എങ്കിൽ (setresuid(0, 0, 0) < 0)
                തിരികെ log_debug_errno(errno, "Failed to become root in user namespace: %m");

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ fd_is_namespace(പൂർണ്ണസംഖ്യ fd, NamespaceType type) {
        പൂർണ്ണസംഖ്യ r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        എങ്കിൽ (r <= 0)
                തിരികെ r;

        എങ്കിൽ (type < 0)
                തിരികെ true;

        പൂർണ്ണസംഖ്യ clone_flag = ioctl(fd, NS_GET_NSTYPE);
        എങ്കിൽ (clone_flag < 0) {
                എങ്കിൽ (errno != ENOTTY)
                        തിരികെ -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ അക്ഷരം *path = ശൂന്യ;
                r = fd_get_path(fd, &path);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                എങ്കിൽ (path_is_absolute(path))
                        തിരികെ -ENOTTY;

                സ്ഥിരം അക്ഷരം *p = startswith(path, namespace_info[type].proc_name);
                എങ്കിൽ (!p)
                        തിരികെ false;

                തിരികെ startswith(p, ":[") != ശൂന്യ;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        എങ്കിൽ (found_type < 0)
                തിരികെ -EBADF; /* Uh? Unknown namespace type? */

        തിരികെ found_type == type;
}

പൂർണ്ണസംഖ്യ is_our_namespace(പൂർണ്ണസംഖ്യ fd, NamespaceType type) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        എങ്കിൽ (r < 0 && r != -ENOTTY)
                തിരികെ r;
        എങ്കിൽ (r == 0) /* Not a namespace or not of the right type? */
                തിരികെ -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ പൂർണ്ണസംഖ്യ our_ns = namespace_open_by_type(type);
        എങ്കിൽ (our_ns < 0)
                തിരികെ our_ns;

        r = fd_inode_same(fd, our_ns);
        എങ്കിൽ (r == 0 && !type_verified)
                തിരികെ -ENOTTY;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ are_our_namespaces(പൂർണ്ണസംഖ്യ pidns_fd, പൂർണ്ണസംഖ്യ mntns_fd, പൂർണ്ണസംഖ്യ netns_fd, പൂർണ്ണസംഖ്യ userns_fd, പൂർണ്ണസംഖ്യ വേര്_fd) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;
        }

        എങ്കിൽ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;
        }

        എങ്കിൽ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;
        }

        എങ്കിൽ (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;
        }

        എങ്കിൽ (വേര്_fd >= 0) {
                r = dir_fd_is_വേര്(വേര്_fd);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;
        }

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ network_namespace_is_init(പൂർണ്ണസംഖ്യ socket_fd) {
        ഘടന stat a, b;
        പൂർണ്ണസംഖ്യ r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        എങ്കിൽ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                എങ്കിൽ (r < 0)
                        തിരികെ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                തിരികെ r ? true : -ENOSYS;
        }
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (socket_fd >= 0) {
                _cleanup_close_ പൂർണ്ണസംഖ്യ netns = ioctl(socket_fd, SIOCGSKNS);
                എങ്കിൽ (netns < 0)
                        തിരികെ -errno;

                എങ്കിൽ (fstat(netns, &b) < 0)
                        തിരികെ -errno;
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        തിരികെ -errno;
        }

        തിരികെ stat_inode_same(&a, &b);
}

പൂർണ്ണസംഖ്യ namespace_is_init(NamespaceType type) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(type >= 0);
        പ്രതിജ്ഞാ(type < _NAMESPACE_TYPE_MAX);

        എങ്കിൽ (type == NAMESPACE_NET)
                തിരികെ network_namespace_is_init(/* socket_fd= */ -EBADF);

        എങ്കിൽ (namespace_info[type].വേര്_inode == 0)
                തിരികെ -EBADR; /* Cannot answer this question */

        സ്ഥിരം അക്ഷരം *p = pid_namespace_path(0, type);

        ഘടന stat st;
        r = RET_NERRNO(stat(p, &st));
        എങ്കിൽ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                എങ്കിൽ (r < 0)
                        തിരികെ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                തിരികെ r ? true : -ENOSYS;
        }
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ st.st_ino == namespace_info[type].വേര്_inode;
}

പൂർണ്ണസംഖ്യ pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        പ്രതിജ്ഞാ(!pid1 || pidref_is_set(pid1));
        പ്രതിജ്ഞാ(!pid2 || pidref_is_set(pid2));
        പ്രതിജ്ഞാ(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        എങ്കിൽ (pidref_equal(pid1, pid2))
                തിരികെ true;

        എങ്കിൽ (!pid1)
                ns1 = namespace_open_by_type(type);
        അല്ലെങ്കിൽ
                ns1 = pidref_namespace_open_by_type(pid1, type);
        എങ്കിൽ (ns1 < 0)
                തിരികെ ns1;

        എങ്കിൽ (!pid2)
                ns2 = namespace_open_by_type(type);
        അല്ലെങ്കിൽ
                ns2 = pidref_namespace_open_by_type(pid2, type);
        എങ്കിൽ (ns2 < 0)
                തിരികെ ns2;

        തിരികെ fd_inode_same(ns1, ns2);
}

പൂർണ്ണസംഖ്യ in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        പ്രതിജ്ഞാ(pid1 >= 0);
        പ്രതിജ്ഞാ(pid2 >= 0);
        തിരികെ pidref_in_same_namespace(pid1 == 0 ? ശൂന്യ : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ശൂന്യ : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

പൂർണ്ണസംഖ്യ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        പൂർണ്ണസംഖ്യ r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        പ്രതിജ്ഞാ(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        പ്രതിജ്ഞാ(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        വേണ്ടി (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                r = pidref_in_same_namespace(c, &parent, type);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        എങ്കിൽ (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        അല്ലെങ്കിൽ {
                                r = pidref_copy(c, ret);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        തിരികെ 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

പൂർണ്ണസംഖ്യ detach_mount_namespace(ശൂന്യം) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        എങ്കിൽ (unshare(CLONE_NEWNS) < 0)
                തിരികെ log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        എങ്കിൽ (mount(ശൂന്യ, "/", ശൂന്യ, MS_SLAVE | MS_REC, ശൂന്യ) < 0)
                തിരികെ log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        എങ്കിൽ (mount(ശൂന്യ, "/", ശൂന്യ, MS_SHARED | MS_REC, ശൂന്യ) < 0)
                തിരികെ log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        പൂർണ്ണസംഖ്യ r;

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

        എങ്കിൽ (!uid_is_valid(target_uid))
                തിരികെ -EINVAL;
        എങ്കിൽ (!gid_is_valid(target_gid))
                തിരികെ -EINVAL;

        r = detach_mount_namespace();
        എങ്കിൽ (r != -EPERM)
                തിരികെ r;

        from_uid = getuid();
        from_gid = getgid();

        എങ്കിൽ (unshare(CLONE_NEWUSER) < 0)
                തിരികെ log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to write gid map: %m");

        തിരികെ detach_mount_namespace();
}

പൂർണ്ണസംഖ്യ detach_mount_namespace_userns(പൂർണ്ണസംഖ്യ userns_fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(userns_fd >= 0);

        എങ്കിൽ (setns(userns_fd, CLONE_NEWUSER) < 0)
                തിരികെ log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to become root in user namespace: %m");

        തിരികെ detach_mount_namespace();
}

പൂർണ്ണസംഖ്യ parse_userns_uid_range(സ്ഥിരം അക്ഷരം *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ അക്ഷരം *buffer = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *range, *shift;
        പൂർണ്ണസംഖ്യ r;
        uid_t uid_shift, uid_range = 65536;

        പ്രതിജ്ഞാ(s);

        range = strchr(s, ':');
        എങ്കിൽ (range) {
                buffer = strndup(s, range - s);
                എങ്കിൽ (!buffer)
                        തിരികെ log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                എങ്കിൽ (r < 0)
                        തിരികെ log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } അല്ലെങ്കിൽ
                shift = s;

        r = parse_uid(shift, &uid_shift);
        എങ്കിൽ (r < 0)
                തിരികെ log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        എങ്കിൽ (!userns_shift_range_valid(uid_shift, uid_range))
                തിരികെ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        എങ്കിൽ (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        എങ്കിൽ (ret_uid_range)
                *ret_uid_range = uid_range;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ userns_acquire_empty(ശൂന്യം) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(r > 0);

        തിരികെ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

പൂർണ്ണസംഖ്യ userns_acquire(സ്ഥിരം അക്ഷരം *uid_map, സ്ഥിരം അക്ഷരം *gid_map, bool setgroups_deny) {
        അക്ഷരം path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(uid_map);
        പ്രതിജ്ഞാ(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        പ്രതിജ്ഞാ(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to write UID map: %m");

        എങ്കിൽ (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to write GID map: %m");

        തിരികെ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

പൂർണ്ണസംഖ്യ userns_acquire_self_വേര്(ശൂന്യം) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ അക്ഷരം *uid_map = ശൂന്യ, *gid_map = ശൂന്യ;
        എങ്കിൽ (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                തിരികെ -ENOMEM;
        എങ്കിൽ (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                തിരികെ -ENOMEM;

        തിരികെ userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

പൂർണ്ണസംഖ്യ userns_enter_and_pin(പൂർണ്ണസംഖ്യ userns_fd, PidRef *ret) {
        _cleanup_close_pair_ പൂർണ്ണസംഖ്യ pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        അക്ഷരം x;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(userns_fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (pipe2(pfd, O_CLOEXEC) < 0)
                തിരികെ -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ശൂന്യ,
                        (പൂർണ്ണസംഖ്യ[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0) {
                /* Child. */

                എങ്കിൽ (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = ലിഖ(pfd[1], &(സ്ഥിരം അക്ഷരം) { 'x' }, 1);
                എങ്കിൽ (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                പ്രതിജ്ഞാ(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = പഠ(pfd[0], &x, 1);
        എങ്കിൽ (n < 0)
                തിരികെ -errno;
        എങ്കിൽ (n == 0)
                തിരികെ -EPROTO;
        പ്രതിജ്ഞാ(n == 1);
        പ്രതിജ്ഞാ(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        തിരികെ 0;
}

bool userns_supported(ശൂന്യം) {
        തിരികെ access("/proc/self/uid_map", F_OK) >= 0;
}

പൂർണ്ണസംഖ്യ userns_get_base_uid(പൂർണ്ണസംഖ്യ userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        uid_t uid;
        r = uid_map_തിരയൽ_വേര്(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        gid_t gid;
        r = uid_map_തിരയൽ_വേര്(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!ret_gid && uid != gid)
                തിരികെ -EUCLEAN;

        എങ്കിൽ (ret_uid)
                *ret_uid = uid;
        എങ്കിൽ (ret_gid)
                *ret_gid = gid;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ userns_has_വേര്_mapping(പൂർണ്ണസംഖ്യ userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ശൂന്യ, *gid_range = ശൂന്യ;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        സ്ഥിരം അക്ഷരം *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        എങ്കിൽ (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        തിരികെ uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

പൂർണ്ണസംഖ്യ process_is_owned_by_uid(സ്ഥിരം PidRef *pidref, uid_t uid) {
        പൂർണ്ണസംഖ്യ r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        പ്രതിജ്ഞാ(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        എങ്കിൽ (process_uid == uid)
                തിരികെ true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ പൂർണ്ണസംഖ്യ userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        എങ്കിൽ (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                തിരികെ false;
        എങ്കിൽ (userns_fd < 0)
                തിരികെ log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        വേണ്ടി (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                എങ്കിൽ (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        തിരികെ false;
                }

                എങ്കിൽ (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        തിരികെ log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                എങ്കിൽ (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        തിരികെ true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                എങ്കിൽ (iteration > 16)
                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ പൂർണ്ണസംഖ്യ parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                എങ്കിൽ (parent_fd < 0) {
                        എങ്കിൽ (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                തിരികെ false;
                        }

                        തിരികെ log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

പൂർണ്ണസംഖ്യ namespace_open_by_id(uint64_t ns_id) {
        പൂർണ്ണസംഖ്യ r;

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

        എങ്കിൽ (ns_id == 0)
                തിരികെ -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EPERM;

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

        സംഘം {
                ഘടന file_handle file_handle;
                uint8_t space[offsetof(ഘടന file_handle, f_handle) + വലുപ്പം(ഘടന nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = വലുപ്പം(ഘടന nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        തിരികെ RET_NERRNO(open_by_handle_at(FD_NSFS_വേര്, &fh.file_handle, O_PATH|O_CLOEXEC));
}

പൂർണ്ണസംഖ്യ is_idmapping_supported(സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        എങ്കിൽ (!mount_new_api_supported())
                തിരികെ false;

        userns_fd = r = userns_acquire_self_വേര്();
        എങ്കിൽ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                തിരികെ false;
        എങ്കിൽ (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                തിരികെ false;
        }
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        എങ്കിൽ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        എങ്കിൽ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(ഘടന mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, വലുപ്പം(ഘടന mount_attr)));
        എങ്കിൽ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ netns_acquire(ശൂന്യം) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to fork process into new netns: %m");
        പ്രതിജ്ഞാ(r > 0);

        തിരികെ pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
