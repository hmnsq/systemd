/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <linux/magic.h>
#అన్తర్భావ <linux/nsfs.h>
#అన్తర్భావ <linux/sockios.h>
#అన్తర్భావ <sched.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/mount.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "capability-util.h"
#అన్తర్భావ "dlfcn-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mountpoint-util.h"
#అన్తర్భావ "namespace-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidfd-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "uid-range.h"
#అన్తర్భావ "unaligned.h"
#అన్తర్భావ "user-util.h"

స్థిరరాశి నిర్మాణం namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        [NAMESPACE_సమయం]   =  { "time",   "ns/time",   CLONE_NEWTIME,   PIDFD_GET_సమయం_NAMESPACE,   PROC_సమయం_INIT_INO   },
        {}, /* Allow callers to iterate over the array without using _NAMESPACE_TYPE_MAX. */
};

#నిర్వచించు pid_namespace_మార్గం(pid, type) procfs_file_alloca(pid, namespace_info[type].proc_మార్గం)

NamespaceType clone_flag_గమ్యం_namespace_type(గుర్తులేని దీర్ఘ clone_flag) {
        కొరకు (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                ఒకవేళ (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        తిరిగి t;

        తిరిగి _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        ప్రతిజ్ఞా(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        స్థిరరాశి అక్షరం *p = pid_namespace_మార్గం(0, type);
        తిరిగి access(p, F_OK) >= 0;
}

స్థిర పూర్ణాంకం pidref_namespace_open_by_type_అంతర్గత(స్థిరరాశి PidRef *pidref, NamespaceType type, bool *need_verify) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref_is_సమితి(pidref));
        ప్రతిజ్ఞా(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                ఒకవేళ (r == -ENOPKG)
                        తిరిగి log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                ఒకవేళ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        తిరిగి r;
        }

        ఒకవేళ (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ పూర్ణాంకం nsfd = -EBADF;
        స్థిరరాశి అక్షరం *p;

        p = pid_namespace_మార్గం(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        ఒకవేళ (nsfd == -ENOENT) {
                r = proc_mounted();
                ఒకవేళ (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        తిరిగి log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                ఒకవేళ (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        తిరిగి log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        ఒకవేళ (nsfd < 0)
                తిరిగి nsfd;

        ఒకవేళ (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి TAKE_FD(nsfd);
}

పూర్ణాంకం pidref_namespace_open_by_type(స్థిరరాశి PidRef *pidref, NamespaceType type) {
        తిరిగి pidref_namespace_open_by_type_అంతర్గత(pidref, type, ఖాళీ);
}

పూర్ణాంకం namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_పూర్తి) PidRef self = PIDREF_NULL;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_సమితి_self(&self);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి pidref_namespace_open_by_type(&self, type);
}

పూర్ణాంకం pidref_namespace_open(
                స్థిరరాశి PidRef *pidref,
                పూర్ణాంకం *ret_pidns_fd,
                పూర్ణాంకం *ret_mntns_fd,
                పూర్ణాంకం *ret_netns_fd,
                పూర్ణాంకం *ret_userns_fd,
                పూర్ణాంకం *ret_మూలం_fd) {

        _cleanup_close_ పూర్ణాంకం pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, మూలం_fd = -EBADF;
        bool need_verify = false;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref_is_సమితి(pidref));

        ఒకవేళ (pidref_is_remote(pidref))
                తిరిగి -EREMOTE;

        ఒకవేళ (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_అంతర్గత(pidref, NAMESPACE_PID, &need_verify);
                ఒకవేళ (pidns_fd < 0)
                        తిరిగి pidns_fd;
        }

        ఒకవేళ (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_అంతర్గత(pidref, NAMESPACE_MOUNT, &need_verify);
                ఒకవేళ (mntns_fd < 0)
                        తిరిగి mntns_fd;
        }

        ఒకవేళ (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_అంతర్గత(pidref, NAMESPACE_NET, &need_verify);
                ఒకవేళ (netns_fd < 0)
                        తిరిగి netns_fd;
        }

        ఒకవేళ (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_అంతర్గత(pidref, NAMESPACE_USER, &need_verify);
                ఒకవేళ (userns_fd < 0 && userns_fd != -ENOPKG)
                        తిరిగి userns_fd;
        }

        ఒకవేళ (ret_మూలం_fd) {
                స్థిరరాశి అక్షరం *మూలం;

                మూలం = procfs_file_alloca(pidref->pid, "root");
                మూలం_fd = RET_NERRNO(open(మూలం, O_CLOEXEC|O_DIRECTORY));
                ఒకవేళ (మూలం_fd == -ENOENT && proc_mounted() == 0)
                        తిరిగి -ENOSYS;
                ఒకవేళ (మూలం_fd < 0)
                        తిరిగి మూలం_fd;

                need_verify = true;
        }

        ఒకవేళ (need_verify) {
                r = pidref_verify(pidref);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        ఒకవేళ (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        ఒకవేళ (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        ఒకవేళ (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        ఒకవేళ (ret_మూలం_fd)
                *ret_మూలం_fd = TAKE_FD(మూలం_fd);

        తిరిగి 0;
}

పూర్ణాంకం namespace_open(
                pid_t pid,
                పూర్ణాంకం *ret_pidns_fd,
                పూర్ణాంకం *ret_mntns_fd,
                పూర్ణాంకం *ret_netns_fd,
                పూర్ణాంకం *ret_userns_fd,
                పూర్ణాంకం *ret_మూలం_fd) {

        _cleanup_(pidref_పూర్తి) PidRef pidref = PIDREF_NULL;
        పూర్ణాంకం r;

        r = pidref_సమితి_pid(&pidref, pid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_మూలం_fd);
}

పూర్ణాంకం namespace_enter_నిండిన(
                పూర్ణాంకం pidns_fd,
                పూర్ణాంకం mntns_fd,
                పూర్ణాంకం netns_fd,
                పూర్ణాంకం userns_fd,
                పూర్ణాంకం మూలం_fd,
                NamespaceEnterFlags flags) {

        పూర్ణాంకం r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        ఖండం_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        ఒకవేళ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        pidns_fd = -EBADF;
        }

        ఒకవేళ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        mntns_fd = -EBADF;
        }

        ఒకవేళ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        netns_fd = -EBADF;
        }

        ఒకవేళ (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        ఒకవేళ (userns_fd >= 0 && FLAGS_సమితి(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_మూలం_UNMAPPED)) {
                r = userns_has_మూలం_mapping(userns_fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        ఒకవేళ (r < 0)
                తిరిగి r;

        bool have_cap_sys_admin = r > 0;

        ఒకవేళ (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                ఒకవేళ (userns_fd >= 0) {
                        ఒకవేళ (setns(userns_fd, CLONE_NEWUSER) < 0)
                                తిరిగి -errno;
                } లేకపోతే ఒకవేళ (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || మూలం_fd >= 0)
                        తిరిగి log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        ఒకవేళ (pidns_fd >= 0)
                ఒకవేళ (setns(pidns_fd, CLONE_NEWPID) < 0)
                        తిరిగి -errno;

        ఒకవేళ (mntns_fd >= 0)
                ఒకవేళ (setns(mntns_fd, CLONE_NEWNS) < 0)
                        తిరిగి -errno;

        ఒకవేళ (netns_fd >= 0)
                ఒకవేళ (setns(netns_fd, CLONE_NEWNET) < 0)
                        తిరిగి -errno;

        ఒకవేళ (userns_fd >= 0 && have_cap_sys_admin)
                ఒకవేళ (setns(userns_fd, CLONE_NEWUSER) < 0)
                        తిరిగి -errno;

        ఒకవేళ (మూలం_fd >= 0) {
                ఒకవేళ (fchdir(మూలం_fd) < 0)
                        తిరిగి -errno;

                ఒకవేళ (chroot(".") < 0)
                        తిరిగి -errno;
        }

        ఒకవేళ (userns_fd < 0)
                తిరిగి 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ఖాళీ);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        ఒకవేళ (keep_uid_gid)
                తిరిగి 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        ఒకవేళ (setresgid(0, 0, 0) < 0)
                తిరిగి log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        ఒకవేళ (setresuid(0, 0, 0) < 0)
                తిరిగి log_debug_errno(errno, "Failed to become root in user namespace: %m");

        తిరిగి 0;
}

పూర్ణాంకం fd_is_namespace(పూర్ణాంకం fd, NamespaceType type) {
        పూర్ణాంకం r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        ఒకవేళ (r <= 0)
                తిరిగి r;

        ఒకవేళ (type < 0)
                తిరిగి true;

        పూర్ణాంకం clone_flag = ioctl(fd, NS_GET_NSTYPE);
        ఒకవేళ (clone_flag < 0) {
                ఒకవేళ (errno != ENOTTY)
                        తిరిగి -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;
                r = fd_get_మార్గం(fd, &మార్గం);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                ఒకవేళ (మార్గం_is_absolute(మార్గం))
                        తిరిగి -ENOTTY;

                స్థిరరాశి అక్షరం *p = startswith(మార్గం, namespace_info[type].proc_name);
                ఒకవేళ (!p)
                        తిరిగి false;

                తిరిగి startswith(p, ":[") != ఖాళీ;
        }

        NamespaceType దొరికింది_type = clone_flag_గమ్యం_namespace_type(clone_flag);
        ఒకవేళ (దొరికింది_type < 0)
                తిరిగి -EBADF; /* Uh? Unknown namespace type? */

        తిరిగి దొరికింది_type == type;
}

పూర్ణాంకం is_our_namespace(పూర్ణాంకం fd, NamespaceType type) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        ఒకవేళ (r < 0 && r != -ENOTTY)
                తిరిగి r;
        ఒకవేళ (r == 0) /* Not a namespace or not of the right type? */
                తిరిగి -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ పూర్ణాంకం our_ns = namespace_open_by_type(type);
        ఒకవేళ (our_ns < 0)
                తిరిగి our_ns;

        r = fd_inode_same(fd, our_ns);
        ఒకవేళ (r == 0 && !type_verified)
                తిరిగి -ENOTTY;

        తిరిగి r;
}

పూర్ణాంకం are_our_namespaces(పూర్ణాంకం pidns_fd, పూర్ణాంకం mntns_fd, పూర్ణాంకం netns_fd, పూర్ణాంకం userns_fd, పూర్ణాంకం మూలం_fd) {
        పూర్ణాంకం r;

        ఒకవేళ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ఒకవేళ (r <= 0)
                        తిరిగి r;
        }

        ఒకవేళ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ఒకవేళ (r <= 0)
                        తిరిగి r;
        }

        ఒకవేళ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ఒకవేళ (r <= 0)
                        తిరిగి r;
        }

        ఒకవేళ (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ఒకవేళ (r <= 0)
                        తిరిగి r;
        }

        ఒకవేళ (మూలం_fd >= 0) {
                r = dir_fd_is_మూలం(మూలం_fd);
                ఒకవేళ (r <= 0)
                        తిరిగి r;
        }

        తిరిగి true;
}

పూర్ణాంకం జాలం_namespace_is_init(పూర్ణాంకం socket_fd) {
        నిర్మాణం stat a, b;
        పూర్ణాంకం r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_మార్గం(1, NAMESPACE_NET), &a));
        ఒకవేళ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ఒకవేళ (r < 0)
                        తిరిగి -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                తిరిగి r ? true : -ENOSYS;
        }
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (socket_fd >= 0) {
                _cleanup_close_ పూర్ణాంకం netns = ioctl(socket_fd, SIOCGSKNS);
                ఒకవేళ (netns < 0)
                        తిరిగి -errno;

                ఒకవేళ (fstat(netns, &b) < 0)
                        తిరిగి -errno;
        } లేకపోతే {
                ఒకవేళ (stat(pid_namespace_మార్గం(0, NAMESPACE_NET), &b) < 0)
                        తిరిగి -errno;
        }

        తిరిగి stat_inode_same(&a, &b);
}

పూర్ణాంకం namespace_is_init(NamespaceType type) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(type >= 0);
        ప్రతిజ్ఞా(type < _NAMESPACE_TYPE_MAX);

        ఒకవేళ (type == NAMESPACE_NET)
                తిరిగి జాలం_namespace_is_init(/* socket_fd= */ -EBADF);

        ఒకవేళ (namespace_info[type].మూలం_inode == 0)
                తిరిగి -EBADR; /* Cannot answer this question */

        స్థిరరాశి అక్షరం *p = pid_namespace_మార్గం(0, type);

        నిర్మాణం stat st;
        r = RET_NERRNO(stat(p, &st));
        ఒకవేళ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ఒకవేళ (r < 0)
                        తిరిగి -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                తిరిగి r ? true : -ENOSYS;
        }
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి st.st_ino == namespace_info[type].మూలం_inode;
}

పూర్ణాంకం pidref_లోపల_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ పూర్ణాంకం ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        ప్రతిజ్ఞా(!pid1 || pidref_is_సమితి(pid1));
        ప్రతిజ్ఞా(!pid2 || pidref_is_సమితి(pid2));
        ప్రతిజ్ఞా(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ఒకవేళ (pidref_equal(pid1, pid2))
                తిరిగి true;

        ఒకవేళ (!pid1)
                ns1 = namespace_open_by_type(type);
        లేకపోతే
                ns1 = pidref_namespace_open_by_type(pid1, type);
        ఒకవేళ (ns1 < 0)
                తిరిగి ns1;

        ఒకవేళ (!pid2)
                ns2 = namespace_open_by_type(type);
        లేకపోతే
                ns2 = pidref_namespace_open_by_type(pid2, type);
        ఒకవేళ (ns2 < 0)
                తిరిగి ns2;

        తిరిగి fd_inode_same(ns1, ns2);
}

పూర్ణాంకం లోపల_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        ప్రతిజ్ఞా(pid1 >= 0);
        ప్రతిజ్ఞా(pid2 >= 0);
        తిరిగి pidref_లోపల_same_namespace(pid1 == 0 ? ఖాళీ : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ఖాళీ : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

పూర్ణాంకం namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        పూర్ణాంకం r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        ప్రతిజ్ఞా(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        ప్రతిజ్ఞా(ret);

        _cleanup_(pidref_పూర్తి) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        కొరకు (;;) {
                _cleanup_(pidref_పూర్తి) PidRef మాతృ = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &మాతృ);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                r = pidref_లోపల_same_namespace(c, &మాతృ, type);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        ఒకవేళ (pidref_is_సమితి(&current))
                                *ret = TAKE_PIDREF(current);
                        లేకపోతే {
                                r = pidref_copy(c, ret);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        తిరిగి 0;
                }

                pidref_పూర్తి(&current);
                current = TAKE_PIDREF(మాతృ);
                c = &current;
        }
}

పూర్ణాంకం detach_mount_namespace(శూన్య) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        ఒకవేళ (unshare(CLONE_NEWNS) < 0)
                తిరిగి log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        ఒకవేళ (mount(ఖాళీ, "/", ఖాళీ, MS_SLAVE | MS_REC, ఖాళీ) < 0)
                తిరిగి log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        ఒకవేళ (mount(ఖాళీ, "/", ఖాళీ, MS_SHARED | MS_REC, ఖాళీ) < 0)
                తిరిగి log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        తిరిగి 0;
}

పూర్ణాంకం detach_mount_namespace_harder(uid_t లక్ష్యం_uid, gid_t లక్ష్యం_gid) {
        uid_t from_uid;
        gid_t from_gid;
        పూర్ణాంకం r;

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

        ఒకవేళ (!uid_is_valid(లక్ష్యం_uid))
                తిరిగి -EINVAL;
        ఒకవేళ (!gid_is_valid(లక్ష్యం_gid))
                తిరిగి -EINVAL;

        r = detach_mount_namespace();
        ఒకవేళ (r != -EPERM)
                తిరిగి r;

        from_uid = getuid();
        from_gid = getgid();

        ఒకవేళ (unshare(CLONE_NEWUSER) < 0)
                తిరిగి log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", లక్ష్యం_uid, from_uid);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", లక్ష్యం_gid, from_gid);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to write gid map: %m");

        తిరిగి detach_mount_namespace();
}

పూర్ణాంకం detach_mount_namespace_userns(పూర్ణాంకం userns_fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(userns_fd >= 0);

        ఒకవేళ (setns(userns_fd, CLONE_NEWUSER) < 0)
                తిరిగి log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to become root in user namespace: %m");

        తిరిగి detach_mount_namespace();
}

పూర్ణాంకం parse_userns_uid_range(స్థిరరాశి అక్షరం *s, uid_t *ret_uid_జరుపు, uid_t *ret_uid_range) {
        _cleanup_free_ అక్షరం *బఫరు = ఖాళీ;
        స్థిరరాశి అక్షరం *range, *జరుపు;
        పూర్ణాంకం r;
        uid_t uid_జరుపు, uid_range = 65536;

        ప్రతిజ్ఞా(s);

        range = strchr(s, ':');
        ఒకవేళ (range) {
                బఫరు = strndup(s, range - s);
                ఒకవేళ (!బఫరు)
                        తిరిగి log_oom();
                జరుపు = బఫరు;

                range++;
                r = సురక్షిత_atou32(range, &uid_range);
                ఒకవేళ (r < 0)
                        తిరిగి log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } లేకపోతే
                జరుపు = s;

        r = parse_uid(జరుపు, &uid_జరుపు);
        ఒకవేళ (r < 0)
                తిరిగి log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        ఒకవేళ (!userns_జరుపు_range_valid(uid_జరుపు, uid_range))
                తిరిగి log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        ఒకవేళ (ret_uid_జరుపు)
                *ret_uid_జరుపు = uid_జరుపు;

        ఒకవేళ (ret_uid_range)
                *ret_uid_range = uid_range;

        తిరిగి 0;
}

పూర్ణాంకం userns_acquire_ఖాళీలు(శూన్య) {
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pid = PIDREF_NULL;
        పూర్ణాంకం r;

        r = pidref_సురక్షిత_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(r > 0);

        తిరిగి pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

పూర్ణాంకం userns_acquire(స్థిరరాశి అక్షరం *uid_map, స్థిరరాశి అక్షరం *gid_map, bool setgroups_deny) {
        అక్షరం మార్గం[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pid = PIDREF_NULL;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(uid_map);
        ప్రతిజ్ఞా(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_సురక్షిత_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ప్రతిజ్ఞా(r > 0);

        xsprintf(మార్గం, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(మార్గం, uid_map, WRITE_STRING_FILE_DISABLE_బఫరు);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to write UID map: %m");

        ఒకవేళ (setgroups_deny) {
                xsprintf(మార్గం, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(మార్గం, "deny", WRITE_STRING_FILE_DISABLE_బఫరు);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(మార్గం, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(మార్గం, gid_map, WRITE_STRING_FILE_DISABLE_బఫరు);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to write GID map: %m");

        తిరిగి pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

పూర్ణాంకం userns_acquire_self_మూలం(శూన్య) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ అక్షరం *uid_map = ఖాళీ, *gid_map = ఖాళీ;
        ఒకవేళ (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                తిరిగి -ENOMEM;
        ఒకవేళ (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                తిరిగి -ENOMEM;

        తిరిగి userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

పూర్ణాంకం userns_enter_and_pin(పూర్ణాంకం userns_fd, PidRef *ret) {
        _cleanup_close_pair_ పూర్ణాంకం pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        అక్షరం x;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(userns_fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (pipe2(pfd, O_CLOEXEC) < 0)
                తిరిగి -errno;

        r = pidref_సురక్షిత_fork_నిండిన(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ఖాళీ,
                        (పూర్ణాంకం[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0) {
                /* Child. */

                ఒకవేళ (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = సురక్షిత_close(userns_fd);

                n = లిఖ(pfd[1], &(స్థిరరాశి అక్షరం) { 'x' }, 1);
                ఒకవేళ (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                ప్రతిజ్ఞా(n == 1);

                freeze();
        }

        pfd[1] = సురక్షిత_close(pfd[1]);

        n = పఠ(pfd[0], &x, 1);
        ఒకవేళ (n < 0)
                తిరిగి -errno;
        ఒకవేళ (n == 0)
                తిరిగి -EPROTO;
        ప్రతిజ్ఞా(n == 1);
        ప్రతిజ్ఞా(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        తిరిగి 0;
}

bool userns_supported(శూన్య) {
        తిరిగి access("/proc/self/uid_map", F_OK) >= 0;
}

పూర్ణాంకం userns_get_పునాది_uid(పూర్ణాంకం userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pidref = PIDREF_NULL;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        uid_t uid;
        r = uid_map_శోధన_మూలం(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        gid_t gid;
        r = uid_map_శోధన_మూలం(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!ret_gid && uid != gid)
                తిరిగి -EUCLEAN;

        ఒకవేళ (ret_uid)
                *ret_uid = uid;
        ఒకవేళ (ret_gid)
                *ret_gid = gid;

        తిరిగి 0;
}

పూర్ణాంకం userns_has_మూలం_mapping(పూర్ణాంకం userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ఖాళీ, *gid_range = ఖాళీ;
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pidref = PIDREF_NULL;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        స్థిరరాశి అక్షరం *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_ఎక్కించు_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        ఒకవేళ (r < 0)
                తిరిగి r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_ఎక్కించు_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        ఒకవేళ (uid_range_is_ఖాళీలు(uid_range) || uid_range_is_ఖాళీలు(gid_range))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        తిరిగి uid_range_కలిగియుంది(uid_range, 0) && uid_range_కలిగియుంది(gid_range, 0);
}

పూర్ణాంకం process_is_owned_by_uid(స్థిరరాశి PidRef *pidref, uid_t uid) {
        పూర్ణాంకం r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        ప్రతిజ్ఞా(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        ఒకవేళ (process_uid == uid)
                తిరిగి true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ పూర్ణాంకం userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        ఒకవేళ (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                తిరిగి false;
        ఒకవేళ (userns_fd < 0)
                తిరిగి log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        కొరకు (గుర్తులేని iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                ఒకవేళ (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        తిరిగి false;
                }

                ఒకవేళ (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        తిరిగి log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                ఒకవేళ (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        తిరిగి true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                ఒకవేళ (iteration > 16)
                        తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ పూర్ణాంకం మాతృ_fd = ioctl(userns_fd, NS_GET_USERNS);
                ఒకవేళ (మాతృ_fd < 0) {
                        ఒకవేళ (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                తిరిగి false;
                        }

                        తిరిగి log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, మాతృ_fd);
        }
}

పూర్ణాంకం namespace_open_by_id(uint64_t ns_id) {
        పూర్ణాంకం r;

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

        ఒకవేళ (ns_id == 0)
                తిరిగి -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EPERM;

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

        సంఘం {
                నిర్మాణం file_handle file_handle;
                uint8_t space[offsetof(నిర్మాణం file_handle, f_handle) + పరిమాణం(నిర్మాణం nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_బైట్లు = పరిమాణం(నిర్మాణం nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        తిరిగి RET_NERRNO(open_by_handle_at(FD_NSFS_మూలం, &fh.file_handle, O_మార్గం|O_CLOEXEC));
}

పూర్ణాంకం is_idmapping_supported(స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        ఒకవేళ (!mount_new_api_supported())
                తిరిగి false;

        userns_fd = r = userns_acquire_self_మూలం();
        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                తిరిగి false;
        ఒకవేళ (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                తిరిగి false;
        }
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", మార్గం);

        dir_fd = r = RET_NERRNO(open(మార్గం, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", మార్గం);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_ఖాళీలు_మార్గం | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", మార్గం);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_ఖాళీలు_మార్గం,
                       &(నిర్మాణం mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, పరిమాణం(నిర్మాణం mount_attr)));
        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", మార్గం);

        తిరిగి true;
}

పూర్ణాంకం netns_acquire(శూన్య) {
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pid = PIDREF_NULL;
        పూర్ణాంకం r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_సురక్షిత_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to fork process into new netns: %m");
        ప్రతిజ్ఞా(r > 0);

        తిరిగి pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
