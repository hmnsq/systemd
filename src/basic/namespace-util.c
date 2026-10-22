/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <linux/magic.h>
#અન્તર્ભાવ <linux/nsfs.h>
#અન્તર્ભાવ <linux/sockios.h>
#અન્તર્ભાવ <sched.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/mount.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "capability-util.h"
#અન્તર્ભાવ "dlfcn-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mountpoint-util.h"
#અન્તર્ભાવ "namespace-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidfd-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "uid-range.h"
#અન્તર્ભાવ "unaligned.h"
#અન્તર્ભાવ "user-util.h"

અચલ સંરચના namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        માટે (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                જો (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        પાછા t;

        પાછા _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        પ્રતિજ્ઞા(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        અચલ અક્ષર *p = pid_namespace_path(0, type);
        પાછા access(p, F_OK) >= 0;
}

સ્થિર પૂર્ણાંક pidref_namespace_open_by_type_internal(અચલ PidRef *pidref, NamespaceType type, bool *need_verify) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref_is_set(pidref));
        પ્રતિજ્ઞા(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                જો (r == -ENOPKG)
                        પાછા log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                જો (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        પાછા r;
        }

        જો (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ પૂર્ણાંક nsfd = -EBADF;
        અચલ અક્ષર *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        જો (nsfd == -ENOENT) {
                r = proc_mounted();
                જો (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        પાછા log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                જો (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        પાછા log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        જો (nsfd < 0)
                પાછા nsfd;

        જો (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                જો (r < 0)
                        પાછા r;
        }

        પાછા TAKE_FD(nsfd);
}

પૂર્ણાંક pidref_namespace_open_by_type(અચલ PidRef *pidref, NamespaceType type) {
        પાછા pidref_namespace_open_by_type_internal(pidref, type, ખાલી);
}

પૂર્ણાંક namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        જો (r < 0)
                પાછા r;

        પાછા pidref_namespace_open_by_type(&self, type);
}

પૂર્ણાંક pidref_namespace_open(
                અચલ PidRef *pidref,
                પૂર્ણાંક *ret_pidns_fd,
                પૂર્ણાંક *ret_mntns_fd,
                પૂર્ણાંક *ret_netns_fd,
                પૂર્ણાંક *ret_userns_fd,
                પૂર્ણાંક *ret_મૂળ_fd) {

        _cleanup_close_ પૂર્ણાંક pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, મૂળ_fd = -EBADF;
        bool need_verify = false;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref_is_set(pidref));

        જો (pidref_is_remote(pidref))
                પાછા -EREMOTE;

        જો (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                જો (pidns_fd < 0)
                        પાછા pidns_fd;
        }

        જો (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                જો (mntns_fd < 0)
                        પાછા mntns_fd;
        }

        જો (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                જો (netns_fd < 0)
                        પાછા netns_fd;
        }

        જો (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                જો (userns_fd < 0 && userns_fd != -ENOPKG)
                        પાછા userns_fd;
        }

        જો (ret_મૂળ_fd) {
                અચલ અક્ષર *મૂળ;

                મૂળ = procfs_file_alloca(pidref->pid, "root");
                મૂળ_fd = RET_NERRNO(open(મૂળ, O_CLOEXEC|O_DIRECTORY));
                જો (મૂળ_fd == -ENOENT && proc_mounted() == 0)
                        પાછા -ENOSYS;
                જો (મૂળ_fd < 0)
                        પાછા મૂળ_fd;

                need_verify = true;
        }

        જો (need_verify) {
                r = pidref_verify(pidref);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        જો (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        જો (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        જો (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        જો (ret_મૂળ_fd)
                *ret_મૂળ_fd = TAKE_FD(મૂળ_fd);

        પાછા 0;
}

પૂર્ણાંક namespace_open(
                pid_t pid,
                પૂર્ણાંક *ret_pidns_fd,
                પૂર્ણાંક *ret_mntns_fd,
                પૂર્ણાંક *ret_netns_fd,
                પૂર્ણાંક *ret_userns_fd,
                પૂર્ણાંક *ret_મૂળ_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        પૂર્ણાંક r;

        r = pidref_set_pid(&pidref, pid);
        જો (r < 0)
                પાછા r;

        પાછા pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_મૂળ_fd);
}

પૂર્ણાંક namespace_enter_full(
                પૂર્ણાંક pidns_fd,
                પૂર્ણાંક mntns_fd,
                પૂર્ણાંક netns_fd,
                પૂર્ણાંક userns_fd,
                પૂર્ણાંક મૂળ_fd,
                NamespaceEnterFlags flags) {

        પૂર્ણાંક r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        જો (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                જો (r < 0)
                        પાછા r;
                જો (r > 0)
                        pidns_fd = -EBADF;
        }

        જો (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                જો (r < 0)
                        પાછા r;
                જો (r > 0)
                        mntns_fd = -EBADF;
        }

        જો (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                જો (r < 0)
                        પાછા r;
                જો (r > 0)
                        netns_fd = -EBADF;
        }

        જો (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                જો (r < 0)
                        પાછા r;
                જો (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        જો (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_મૂળ_UNMAPPED)) {
                r = userns_has_મૂળ_mapping(userns_fd);
                જો (r < 0)
                        પાછા r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        જો (r < 0)
                પાછા r;

        bool have_cap_sys_admin = r > 0;

        જો (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                જો (userns_fd >= 0) {
                        જો (setns(userns_fd, CLONE_NEWUSER) < 0)
                                પાછા -errno;
                } નહીંતર જો (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || મૂળ_fd >= 0)
                        પાછા log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        જો (pidns_fd >= 0)
                જો (setns(pidns_fd, CLONE_NEWPID) < 0)
                        પાછા -errno;

        જો (mntns_fd >= 0)
                જો (setns(mntns_fd, CLONE_NEWNS) < 0)
                        પાછા -errno;

        જો (netns_fd >= 0)
                જો (setns(netns_fd, CLONE_NEWNET) < 0)
                        પાછા -errno;

        જો (userns_fd >= 0 && have_cap_sys_admin)
                જો (setns(userns_fd, CLONE_NEWUSER) < 0)
                        પાછા -errno;

        જો (મૂળ_fd >= 0) {
                જો (fchdir(મૂળ_fd) < 0)
                        પાછા -errno;

                જો (chroot(".") < 0)
                        પાછા -errno;
        }

        જો (userns_fd < 0)
                પાછા 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ખાલી);
        જો (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        જો (keep_uid_gid)
                પાછા 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        જો (setresgid(0, 0, 0) < 0)
                પાછા log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        જો (setresuid(0, 0, 0) < 0)
                પાછા log_debug_errno(errno, "Failed to become root in user namespace: %m");

        પાછા 0;
}

પૂર્ણાંક fd_is_namespace(પૂર્ણાંક fd, NamespaceType type) {
        પૂર્ણાંક r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        જો (r <= 0)
                પાછા r;

        જો (type < 0)
                પાછા true;

        પૂર્ણાંક clone_flag = ioctl(fd, NS_GET_NSTYPE);
        જો (clone_flag < 0) {
                જો (errno != ENOTTY)
                        પાછા -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ અક્ષર *path = ખાલી;
                r = fd_get_path(fd, &path);
                જો (r < 0)
                        પાછા r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                જો (path_is_absolute(path))
                        પાછા -ENOTTY;

                અચલ અક્ષર *p = startswith(path, namespace_info[type].proc_name);
                જો (!p)
                        પાછા false;

                પાછા startswith(p, ":[") != ખાલી;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        જો (found_type < 0)
                પાછા -EBADF; /* Uh? Unknown namespace type? */

        પાછા found_type == type;
}

પૂર્ણાંક is_our_namespace(પૂર્ણાંક fd, NamespaceType type) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        જો (r < 0 && r != -ENOTTY)
                પાછા r;
        જો (r == 0) /* Not a namespace or not of the right type? */
                પાછા -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ પૂર્ણાંક our_ns = namespace_open_by_type(type);
        જો (our_ns < 0)
                પાછા our_ns;

        r = fd_inode_same(fd, our_ns);
        જો (r == 0 && !type_verified)
                પાછા -ENOTTY;

        પાછા r;
}

પૂર્ણાંક are_our_namespaces(પૂર્ણાંક pidns_fd, પૂર્ણાંક mntns_fd, પૂર્ણાંક netns_fd, પૂર્ણાંક userns_fd, પૂર્ણાંક મૂળ_fd) {
        પૂર્ણાંક r;

        જો (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                જો (r <= 0)
                        પાછા r;
        }

        જો (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                જો (r <= 0)
                        પાછા r;
        }

        જો (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                જો (r <= 0)
                        પાછા r;
        }

        જો (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                જો (r <= 0)
                        પાછા r;
        }

        જો (મૂળ_fd >= 0) {
                r = dir_fd_is_મૂળ(મૂળ_fd);
                જો (r <= 0)
                        પાછા r;
        }

        પાછા true;
}

પૂર્ણાંક network_namespace_is_init(પૂર્ણાંક socket_fd) {
        સંરચના stat a, b;
        પૂર્ણાંક r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        જો (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                જો (r < 0)
                        પાછા -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                પાછા r ? true : -ENOSYS;
        }
        જો (r < 0)
                પાછા r;

        જો (socket_fd >= 0) {
                _cleanup_close_ પૂર્ણાંક netns = ioctl(socket_fd, SIOCGSKNS);
                જો (netns < 0)
                        પાછા -errno;

                જો (fstat(netns, &b) < 0)
                        પાછા -errno;
        } નહીંતર {
                જો (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        પાછા -errno;
        }

        પાછા stat_inode_same(&a, &b);
}

પૂર્ણાંક namespace_is_init(NamespaceType type) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(type >= 0);
        પ્રતિજ્ઞા(type < _NAMESPACE_TYPE_MAX);

        જો (type == NAMESPACE_NET)
                પાછા network_namespace_is_init(/* socket_fd= */ -EBADF);

        જો (namespace_info[type].મૂળ_inode == 0)
                પાછા -EBADR; /* Cannot answer this question */

        અચલ અક્ષર *p = pid_namespace_path(0, type);

        સંરચના stat st;
        r = RET_NERRNO(stat(p, &st));
        જો (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                જો (r < 0)
                        પાછા -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                પાછા r ? true : -ENOSYS;
        }
        જો (r < 0)
                પાછા r;

        પાછા st.st_ino == namespace_info[type].મૂળ_inode;
}

પૂર્ણાંક pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ પૂર્ણાંક ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        પ્રતિજ્ઞા(!pid1 || pidref_is_set(pid1));
        પ્રતિજ્ઞા(!pid2 || pidref_is_set(pid2));
        પ્રતિજ્ઞા(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        જો (pidref_equal(pid1, pid2))
                પાછા true;

        જો (!pid1)
                ns1 = namespace_open_by_type(type);
        નહીંતર
                ns1 = pidref_namespace_open_by_type(pid1, type);
        જો (ns1 < 0)
                પાછા ns1;

        જો (!pid2)
                ns2 = namespace_open_by_type(type);
        નહીંતર
                ns2 = pidref_namespace_open_by_type(pid2, type);
        જો (ns2 < 0)
                પાછા ns2;

        પાછા fd_inode_same(ns1, ns2);
}

પૂર્ણાંક in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        પ્રતિજ્ઞા(pid1 >= 0);
        પ્રતિજ્ઞા(pid2 >= 0);
        પાછા pidref_in_same_namespace(pid1 == 0 ? ખાલી : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ખાલી : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

પૂર્ણાંક namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        પૂર્ણાંક r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        પ્રતિજ્ઞા(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        પ્રતિજ્ઞા(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        માટે (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                જો (r < 0)
                        પાછા r;

                r = pidref_in_same_namespace(c, &parent, type);
                જો (r < 0)
                        પાછા r;
                જો (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        જો (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        નહીંતર {
                                r = pidref_copy(c, ret);
                                જો (r < 0)
                                        પાછા r;
                        }

                        પાછા 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

પૂર્ણાંક detach_mount_namespace(શૂન્ય) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        જો (unshare(CLONE_NEWNS) < 0)
                પાછા log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        જો (mount(ખાલી, "/", ખાલી, MS_SLAVE | MS_REC, ખાલી) < 0)
                પાછા log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        જો (mount(ખાલી, "/", ખાલી, MS_SHARED | MS_REC, ખાલી) < 0)
                પાછા log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        પાછા 0;
}

પૂર્ણાંક detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        પૂર્ણાંક r;

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

        જો (!uid_is_valid(target_uid))
                પાછા -EINVAL;
        જો (!gid_is_valid(target_gid))
                પાછા -EINVAL;

        r = detach_mount_namespace();
        જો (r != -EPERM)
                પાછા r;

        from_uid = getuid();
        from_gid = getgid();

        જો (unshare(CLONE_NEWUSER) < 0)
                પાછા log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to write gid map: %m");

        પાછા detach_mount_namespace();
}

પૂર્ણાંક detach_mount_namespace_userns(પૂર્ણાંક userns_fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(userns_fd >= 0);

        જો (setns(userns_fd, CLONE_NEWUSER) < 0)
                પાછા log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to become root in user namespace: %m");

        પાછા detach_mount_namespace();
}

પૂર્ણાંક parse_userns_uid_range(અચલ અક્ષર *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ અક્ષર *buffer = ખાલી;
        અચલ અક્ષર *range, *shift;
        પૂર્ણાંક r;
        uid_t uid_shift, uid_range = 65536;

        પ્રતિજ્ઞા(s);

        range = strchr(s, ':');
        જો (range) {
                buffer = strndup(s, range - s);
                જો (!buffer)
                        પાછા log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                જો (r < 0)
                        પાછા log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } નહીંતર
                shift = s;

        r = parse_uid(shift, &uid_shift);
        જો (r < 0)
                પાછા log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        જો (!userns_shift_range_valid(uid_shift, uid_range))
                પાછા log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        જો (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        જો (ret_uid_range)
                *ret_uid_range = uid_range;

        પાછા 0;
}

પૂર્ણાંક userns_acquire_empty(શૂન્ય) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        પૂર્ણાંક r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        જો (r < 0)
                પાછા r;
        પ્રતિજ્ઞા(r > 0);

        પાછા pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

પૂર્ણાંક userns_acquire(અચલ અક્ષર *uid_map, અચલ અક્ષર *gid_map, bool setgroups_deny) {
        અક્ષર path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(uid_map);
        પ્રતિજ્ઞા(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        જો (r < 0)
                પાછા r;
        પ્રતિજ્ઞા(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to write UID map: %m");

        જો (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                જો (r < 0)
                        પાછા log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to write GID map: %m");

        પાછા pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

પૂર્ણાંક userns_acquire_self_મૂળ(શૂન્ય) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ અક્ષર *uid_map = ખાલી, *gid_map = ખાલી;
        જો (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                પાછા -ENOMEM;
        જો (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                પાછા -ENOMEM;

        પાછા userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

પૂર્ણાંક userns_enter_and_pin(પૂર્ણાંક userns_fd, PidRef *ret) {
        _cleanup_close_pair_ પૂર્ણાંક pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        અક્ષર x;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(userns_fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (pipe2(pfd, O_CLOEXEC) < 0)
                પાછા -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ખાલી,
                        (પૂર્ણાંક[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        જો (r < 0)
                પાછા r;
        જો (r == 0) {
                /* Child. */

                જો (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = લિખ(pfd[1], &(અચલ અક્ષર) { 'x' }, 1);
                જો (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                પ્રતિજ્ઞા(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = પઠ(pfd[0], &x, 1);
        જો (n < 0)
                પાછા -errno;
        જો (n == 0)
                પાછા -EPROTO;
        પ્રતિજ્ઞા(n == 1);
        પ્રતિજ્ઞા(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        પાછા 0;
}

bool userns_supported(શૂન્ય) {
        પાછા access("/proc/self/uid_map", F_OK) >= 0;
}

પૂર્ણાંક userns_get_base_uid(પૂર્ણાંક userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        જો (r < 0)
                પાછા r;

        uid_t uid;
        r = uid_map_શોધ_મૂળ(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        જો (r < 0)
                પાછા r;

        gid_t gid;
        r = uid_map_શોધ_મૂળ(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        જો (r < 0)
                પાછા r;

        જો (!ret_gid && uid != gid)
                પાછા -EUCLEAN;

        જો (ret_uid)
                *ret_uid = uid;
        જો (ret_gid)
                *ret_gid = gid;

        પાછા 0;
}

પૂર્ણાંક userns_has_મૂળ_mapping(પૂર્ણાંક userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ખાલી, *gid_range = ખાલી;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        જો (r < 0)
                પાછા r;

        અચલ અક્ષર *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        જો (r < 0)
                પાછા r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        જો (r < 0)
                પાછા r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        જો (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        પાછા uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

પૂર્ણાંક process_is_owned_by_uid(અચલ PidRef *pidref, uid_t uid) {
        પૂર્ણાંક r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        પ્રતિજ્ઞા(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        જો (process_uid == uid)
                પાછા true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ પૂર્ણાંક userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        જો (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                પાછા false;
        જો (userns_fd < 0)
                પાછા log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        માટે (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                જો (r < 0)
                        પાછા log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                જો (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        પાછા false;
                }

                જો (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        પાછા log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                જો (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        પાછા true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                જો (iteration > 16)
                        પાછા log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ પૂર્ણાંક parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                જો (parent_fd < 0) {
                        જો (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                પાછા false;
                        }

                        પાછા log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

પૂર્ણાંક namespace_open_by_id(uint64_t ns_id) {
        પૂર્ણાંક r;

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

        જો (ns_id == 0)
                પાછા -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EPERM;

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

        સંઘ {
                સંરચના file_handle file_handle;
                uint8_t space[offsetof(સંરચના file_handle, f_handle) + કદ(સંરચના nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = કદ(સંરચના nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        પાછા RET_NERRNO(open_by_handle_at(FD_NSFS_મૂળ, &fh.file_handle, O_PATH|O_CLOEXEC));
}

પૂર્ણાંક is_idmapping_supported(અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        જો (!mount_new_api_supported())
                પાછા false;

        userns_fd = r = userns_acquire_self_મૂળ();
        જો (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                પાછા false;
        જો (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                પાછા false;
        }
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        જો (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                પાછા false;
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        જો (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                પાછા false;
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(સંરચના mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, કદ(સંરચના mount_attr)));
        જો (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                પાછા false;
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        પાછા true;
}

પૂર્ણાંક netns_acquire(શૂન્ય) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        પૂર્ણાંક r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to fork process into new netns: %m");
        પ્રતિજ્ઞા(r > 0);

        પાછા pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
