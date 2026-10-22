/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <linux/magic.h>
#অন্তর্ভাব <linux/nsfs.h>
#অন্তর্ভাব <linux/sockios.h>
#অন্তর্ভাব <sched.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/mount.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "capability-util.h"
#অন্তর্ভাব "dlfcn-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mountpoint-util.h"
#অন্তর্ভাব "namespace-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "pidfd-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "uid-range.h"
#অন্তর্ভাব "unaligned.h"
#অন্তর্ভাব "user-util.h"

ধ্রুবক কাঠামো namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        জন্য (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                যদি (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        ফেরত t;

        ফেরত _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        প্রতিজ্ঞা(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ধ্রুবক অক্ষর *p = pid_namespace_path(0, type);
        ফেরত access(p, F_OK) >= 0;
}

স্থির পূর্ণসংখ্যা pidref_namespace_open_by_type_internal(ধ্রুবক PidRef *pidref, NamespaceType type, bool *need_verify) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref_is_set(pidref));
        প্রতিজ্ঞা(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                যদি (r == -ENOPKG)
                        ফেরত log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                যদি (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ফেরত r;
        }

        যদি (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ পূর্ণসংখ্যা nsfd = -EBADF;
        ধ্রুবক অক্ষর *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        যদি (nsfd == -ENOENT) {
                r = proc_mounted();
                যদি (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        ফেরত log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                যদি (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        ফেরত log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        যদি (nsfd < 0)
                ফেরত nsfd;

        যদি (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত TAKE_FD(nsfd);
}

পূর্ণসংখ্যা pidref_namespace_open_by_type(ধ্রুবক PidRef *pidref, NamespaceType type) {
        ফেরত pidref_namespace_open_by_type_internal(pidref, type, ফাঁকা);
}

পূর্ণসংখ্যা namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        যদি (r < 0)
                ফেরত r;

        ফেরত pidref_namespace_open_by_type(&self, type);
}

পূর্ণসংখ্যা pidref_namespace_open(
                ধ্রুবক PidRef *pidref,
                পূর্ণসংখ্যা *ret_pidns_fd,
                পূর্ণসংখ্যা *ret_mntns_fd,
                পূর্ণসংখ্যা *ret_netns_fd,
                পূর্ণসংখ্যা *ret_userns_fd,
                পূর্ণসংখ্যা *ret_মূল_fd) {

        _cleanup_close_ পূর্ণসংখ্যা pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, মূল_fd = -EBADF;
        bool need_verify = false;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref_is_set(pidref));

        যদি (pidref_is_remote(pidref))
                ফেরত -EREMOTE;

        যদি (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                যদি (pidns_fd < 0)
                        ফেরত pidns_fd;
        }

        যদি (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                যদি (mntns_fd < 0)
                        ফেরত mntns_fd;
        }

        যদি (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                যদি (netns_fd < 0)
                        ফেরত netns_fd;
        }

        যদি (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                যদি (userns_fd < 0 && userns_fd != -ENOPKG)
                        ফেরত userns_fd;
        }

        যদি (ret_মূল_fd) {
                ধ্রুবক অক্ষর *মূল;

                মূল = procfs_file_alloca(pidref->pid, "root");
                মূল_fd = RET_NERRNO(open(মূল, O_CLOEXEC|O_DIRECTORY));
                যদি (মূল_fd == -ENOENT && proc_mounted() == 0)
                        ফেরত -ENOSYS;
                যদি (মূল_fd < 0)
                        ফেরত মূল_fd;

                need_verify = true;
        }

        যদি (need_verify) {
                r = pidref_verify(pidref);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        যদি (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        যদি (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        যদি (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        যদি (ret_মূল_fd)
                *ret_মূল_fd = TAKE_FD(মূল_fd);

        ফেরত 0;
}

পূর্ণসংখ্যা namespace_open(
                pid_t pid,
                পূর্ণসংখ্যা *ret_pidns_fd,
                পূর্ণসংখ্যা *ret_mntns_fd,
                পূর্ণসংখ্যা *ret_netns_fd,
                পূর্ণসংখ্যা *ret_userns_fd,
                পূর্ণসংখ্যা *ret_মূল_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        r = pidref_set_pid(&pidref, pid);
        যদি (r < 0)
                ফেরত r;

        ফেরত pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_মূল_fd);
}

পূর্ণসংখ্যা namespace_enter_full(
                পূর্ণসংখ্যা pidns_fd,
                পূর্ণসংখ্যা mntns_fd,
                পূর্ণসংখ্যা netns_fd,
                পূর্ণসংখ্যা userns_fd,
                পূর্ণসংখ্যা মূল_fd,
                NamespaceEnterFlags flags) {

        পূর্ণসংখ্যা r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        যদি (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                যদি (r < 0)
                        ফেরত r;
                যদি (r > 0)
                        pidns_fd = -EBADF;
        }

        যদি (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                যদি (r < 0)
                        ফেরত r;
                যদি (r > 0)
                        mntns_fd = -EBADF;
        }

        যদি (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                যদি (r < 0)
                        ফেরত r;
                যদি (r > 0)
                        netns_fd = -EBADF;
        }

        যদি (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                যদি (r < 0)
                        ফেরত r;
                যদি (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        যদি (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_মূল_UNMAPPED)) {
                r = userns_has_মূল_mapping(userns_fd);
                যদি (r < 0)
                        ফেরত r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        যদি (r < 0)
                ফেরত r;

        bool have_cap_sys_admin = r > 0;

        যদি (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                যদি (userns_fd >= 0) {
                        যদি (setns(userns_fd, CLONE_NEWUSER) < 0)
                                ফেরত -errno;
                } অন্যথা যদি (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || মূল_fd >= 0)
                        ফেরত log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        যদি (pidns_fd >= 0)
                যদি (setns(pidns_fd, CLONE_NEWPID) < 0)
                        ফেরত -errno;

        যদি (mntns_fd >= 0)
                যদি (setns(mntns_fd, CLONE_NEWNS) < 0)
                        ফেরত -errno;

        যদি (netns_fd >= 0)
                যদি (setns(netns_fd, CLONE_NEWNET) < 0)
                        ফেরত -errno;

        যদি (userns_fd >= 0 && have_cap_sys_admin)
                যদি (setns(userns_fd, CLONE_NEWUSER) < 0)
                        ফেরত -errno;

        যদি (মূল_fd >= 0) {
                যদি (fchdir(মূল_fd) < 0)
                        ফেরত -errno;

                যদি (chroot(".") < 0)
                        ফেরত -errno;
        }

        যদি (userns_fd < 0)
                ফেরত 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ফাঁকা);
        যদি (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        যদি (keep_uid_gid)
                ফেরত 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        যদি (setresgid(0, 0, 0) < 0)
                ফেরত log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        যদি (setresuid(0, 0, 0) < 0)
                ফেরত log_debug_errno(errno, "Failed to become root in user namespace: %m");

        ফেরত 0;
}

পূর্ণসংখ্যা fd_is_namespace(পূর্ণসংখ্যা fd, NamespaceType type) {
        পূর্ণসংখ্যা r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        যদি (r <= 0)
                ফেরত r;

        যদি (type < 0)
                ফেরত true;

        পূর্ণসংখ্যা clone_flag = ioctl(fd, NS_GET_NSTYPE);
        যদি (clone_flag < 0) {
                যদি (errno != ENOTTY)
                        ফেরত -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ অক্ষর *path = ফাঁকা;
                r = fd_get_path(fd, &path);
                যদি (r < 0)
                        ফেরত r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                যদি (path_is_absolute(path))
                        ফেরত -ENOTTY;

                ধ্রুবক অক্ষর *p = startswith(path, namespace_info[type].proc_name);
                যদি (!p)
                        ফেরত false;

                ফেরত startswith(p, ":[") != ফাঁকা;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        যদি (found_type < 0)
                ফেরত -EBADF; /* Uh? Unknown namespace type? */

        ফেরত found_type == type;
}

পূর্ণসংখ্যা is_our_namespace(পূর্ণসংখ্যা fd, NamespaceType type) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        যদি (r < 0 && r != -ENOTTY)
                ফেরত r;
        যদি (r == 0) /* Not a namespace or not of the right type? */
                ফেরত -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ পূর্ণসংখ্যা our_ns = namespace_open_by_type(type);
        যদি (our_ns < 0)
                ফেরত our_ns;

        r = fd_inode_same(fd, our_ns);
        যদি (r == 0 && !type_verified)
                ফেরত -ENOTTY;

        ফেরত r;
}

পূর্ণসংখ্যা are_our_namespaces(পূর্ণসংখ্যা pidns_fd, পূর্ণসংখ্যা mntns_fd, পূর্ণসংখ্যা netns_fd, পূর্ণসংখ্যা userns_fd, পূর্ণসংখ্যা মূল_fd) {
        পূর্ণসংখ্যা r;

        যদি (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                যদি (r <= 0)
                        ফেরত r;
        }

        যদি (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                যদি (r <= 0)
                        ফেরত r;
        }

        যদি (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                যদি (r <= 0)
                        ফেরত r;
        }

        যদি (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                যদি (r <= 0)
                        ফেরত r;
        }

        যদি (মূল_fd >= 0) {
                r = dir_fd_is_মূল(মূল_fd);
                যদি (r <= 0)
                        ফেরত r;
        }

        ফেরত true;
}

পূর্ণসংখ্যা network_namespace_is_init(পূর্ণসংখ্যা socket_fd) {
        কাঠামো stat a, b;
        পূর্ণসংখ্যা r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        যদি (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                যদি (r < 0)
                        ফেরত -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ফেরত r ? true : -ENOSYS;
        }
        যদি (r < 0)
                ফেরত r;

        যদি (socket_fd >= 0) {
                _cleanup_close_ পূর্ণসংখ্যা netns = ioctl(socket_fd, SIOCGSKNS);
                যদি (netns < 0)
                        ফেরত -errno;

                যদি (fstat(netns, &b) < 0)
                        ফেরত -errno;
        } অন্যথা {
                যদি (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        ফেরত -errno;
        }

        ফেরত stat_inode_same(&a, &b);
}

পূর্ণসংখ্যা namespace_is_init(NamespaceType type) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(type >= 0);
        প্রতিজ্ঞা(type < _NAMESPACE_TYPE_MAX);

        যদি (type == NAMESPACE_NET)
                ফেরত network_namespace_is_init(/* socket_fd= */ -EBADF);

        যদি (namespace_info[type].মূল_inode == 0)
                ফেরত -EBADR; /* Cannot answer this question */

        ধ্রুবক অক্ষর *p = pid_namespace_path(0, type);

        কাঠামো stat st;
        r = RET_NERRNO(stat(p, &st));
        যদি (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                যদি (r < 0)
                        ফেরত -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ফেরত r ? true : -ENOSYS;
        }
        যদি (r < 0)
                ফেরত r;

        ফেরত st.st_ino == namespace_info[type].মূল_inode;
}

পূর্ণসংখ্যা pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ পূর্ণসংখ্যা ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        প্রতিজ্ঞা(!pid1 || pidref_is_set(pid1));
        প্রতিজ্ঞা(!pid2 || pidref_is_set(pid2));
        প্রতিজ্ঞা(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        যদি (pidref_equal(pid1, pid2))
                ফেরত true;

        যদি (!pid1)
                ns1 = namespace_open_by_type(type);
        অন্যথা
                ns1 = pidref_namespace_open_by_type(pid1, type);
        যদি (ns1 < 0)
                ফেরত ns1;

        যদি (!pid2)
                ns2 = namespace_open_by_type(type);
        অন্যথা
                ns2 = pidref_namespace_open_by_type(pid2, type);
        যদি (ns2 < 0)
                ফেরত ns2;

        ফেরত fd_inode_same(ns1, ns2);
}

পূর্ণসংখ্যা in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        প্রতিজ্ঞা(pid1 >= 0);
        প্রতিজ্ঞা(pid2 >= 0);
        ফেরত pidref_in_same_namespace(pid1 == 0 ? ফাঁকা : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ফাঁকা : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

পূর্ণসংখ্যা namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        পূর্ণসংখ্যা r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        প্রতিজ্ঞা(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        প্রতিজ্ঞা(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        জন্য (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                যদি (r < 0)
                        ফেরত r;

                r = pidref_in_same_namespace(c, &parent, type);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        যদি (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        অন্যথা {
                                r = pidref_copy(c, ret);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        ফেরত 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

পূর্ণসংখ্যা detach_mount_namespace(শূন্য) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        যদি (unshare(CLONE_NEWNS) < 0)
                ফেরত log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        যদি (mount(ফাঁকা, "/", ফাঁকা, MS_SLAVE | MS_REC, ফাঁকা) < 0)
                ফেরত log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        যদি (mount(ফাঁকা, "/", ফাঁকা, MS_SHARED | MS_REC, ফাঁকা) < 0)
                ফেরত log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        ফেরত 0;
}

পূর্ণসংখ্যা detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        পূর্ণসংখ্যা r;

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

        যদি (!uid_is_valid(target_uid))
                ফেরত -EINVAL;
        যদি (!gid_is_valid(target_gid))
                ফেরত -EINVAL;

        r = detach_mount_namespace();
        যদি (r != -EPERM)
                ফেরত r;

        from_uid = getuid();
        from_gid = getgid();

        যদি (unshare(CLONE_NEWUSER) < 0)
                ফেরত log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to write gid map: %m");

        ফেরত detach_mount_namespace();
}

পূর্ণসংখ্যা detach_mount_namespace_userns(পূর্ণসংখ্যা userns_fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(userns_fd >= 0);

        যদি (setns(userns_fd, CLONE_NEWUSER) < 0)
                ফেরত log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to become root in user namespace: %m");

        ফেরত detach_mount_namespace();
}

পূর্ণসংখ্যা parse_userns_uid_range(ধ্রুবক অক্ষর *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ অক্ষর *বাফার = ফাঁকা;
        ধ্রুবক অক্ষর *range, *shift;
        পূর্ণসংখ্যা r;
        uid_t uid_shift, uid_range = 65536;

        প্রতিজ্ঞা(s);

        range = strchr(s, ':');
        যদি (range) {
                বাফার = strndup(s, range - s);
                যদি (!বাফার)
                        ফেরত log_oom();
                shift = বাফার;

                range++;
                r = safe_atou32(range, &uid_range);
                যদি (r < 0)
                        ফেরত log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } অন্যথা
                shift = s;

        r = parse_uid(shift, &uid_shift);
        যদি (r < 0)
                ফেরত log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        যদি (!userns_shift_range_valid(uid_shift, uid_range))
                ফেরত log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        যদি (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        যদি (ret_uid_range)
                *ret_uid_range = uid_range;

        ফেরত 0;
}

পূর্ণসংখ্যা userns_acquire_empty(শূন্য) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(r > 0);

        ফেরত pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

পূর্ণসংখ্যা userns_acquire(ধ্রুবক অক্ষর *uid_map, ধ্রুবক অক্ষর *gid_map, bool setgroups_deny) {
        অক্ষর path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(uid_map);
        প্রতিজ্ঞা(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        যদি (r < 0)
                ফেরত r;
        প্রতিজ্ঞা(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_বাফার);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to write UID map: %m");

        যদি (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_বাফার);
                যদি (r < 0)
                        ফেরত log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_বাফার);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to write GID map: %m");

        ফেরত pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

পূর্ণসংখ্যা userns_acquire_self_মূল(শূন্য) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ অক্ষর *uid_map = ফাঁকা, *gid_map = ফাঁকা;
        যদি (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                ফেরত -ENOMEM;
        যদি (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                ফেরত -ENOMEM;

        ফেরত userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

পূর্ণসংখ্যা userns_enter_and_pin(পূর্ণসংখ্যা userns_fd, PidRef *ret) {
        _cleanup_close_pair_ পূর্ণসংখ্যা pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        অক্ষর x;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(userns_fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (pipe2(pfd, O_CLOEXEC) < 0)
                ফেরত -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ফাঁকা,
                        (পূর্ণসংখ্যা[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0) {
                /* Child. */

                যদি (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = লিখ(pfd[1], &(ধ্রুবক অক্ষর) { 'x' }, 1);
                যদি (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                প্রতিজ্ঞা(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = পঠ(pfd[0], &x, 1);
        যদি (n < 0)
                ফেরত -errno;
        যদি (n == 0)
                ফেরত -EPROTO;
        প্রতিজ্ঞা(n == 1);
        প্রতিজ্ঞা(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        ফেরত 0;
}

bool userns_supported(শূন্য) {
        ফেরত access("/proc/self/uid_map", F_OK) >= 0;
}

পূর্ণসংখ্যা userns_get_base_uid(পূর্ণসংখ্যা userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        যদি (r < 0)
                ফেরত r;

        uid_t uid;
        r = uid_map_অনুসন্ধান_মূল(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        যদি (r < 0)
                ফেরত r;

        gid_t gid;
        r = uid_map_অনুসন্ধান_মূল(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        যদি (r < 0)
                ফেরত r;

        যদি (!ret_gid && uid != gid)
                ফেরত -EUCLEAN;

        যদি (ret_uid)
                *ret_uid = uid;
        যদি (ret_gid)
                *ret_gid = gid;

        ফেরত 0;
}

পূর্ণসংখ্যা userns_has_মূল_mapping(পূর্ণসংখ্যা userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ফাঁকা, *gid_range = ফাঁকা;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        যদি (r < 0)
                ফেরত r;

        ধ্রুবক অক্ষর *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        যদি (r < 0)
                ফেরত r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        যদি (r < 0)
                ফেরত r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        যদি (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        ফেরত uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

পূর্ণসংখ্যা process_is_owned_by_uid(ধ্রুবক PidRef *pidref, uid_t uid) {
        পূর্ণসংখ্যা r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        প্রতিজ্ঞা(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        যদি (process_uid == uid)
                ফেরত true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ পূর্ণসংখ্যা userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        যদি (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                ফেরত false;
        যদি (userns_fd < 0)
                ফেরত log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        জন্য (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                যদি (r < 0)
                        ফেরত log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                যদি (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        ফেরত false;
                }

                যদি (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        ফেরত log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                যদি (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        ফেরত true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                যদি (iteration > 16)
                        ফেরত log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ পূর্ণসংখ্যা parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                যদি (parent_fd < 0) {
                        যদি (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                ফেরত false;
                        }

                        ফেরত log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

পূর্ণসংখ্যা namespace_open_by_id(uint64_t ns_id) {
        পূর্ণসংখ্যা r;

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

        যদি (ns_id == 0)
                ফেরত -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EPERM;

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

        সংঘ {
                কাঠামো file_handle file_handle;
                uint8_t space[offsetof(কাঠামো file_handle, f_handle) + আকার(কাঠামো nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = আকার(কাঠামো nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        ফেরত RET_NERRNO(open_by_handle_at(FD_NSFS_মূল, &fh.file_handle, O_PATH|O_CLOEXEC));
}

পূর্ণসংখ্যা is_idmapping_supported(ধ্রুবক অক্ষর *path) {
        _cleanup_close_ পূর্ণসংখ্যা mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        যদি (!mount_new_api_supported())
                ফেরত false;

        userns_fd = r = userns_acquire_self_মূল();
        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ফেরত false;
        যদি (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                ফেরত false;
        }
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ফেরত false;
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ফেরত false;
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(কাঠামো mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, আকার(কাঠামো mount_attr)));
        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ফেরত false;
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        ফেরত true;
}

পূর্ণসংখ্যা netns_acquire(শূন্য) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to fork process into new netns: %m");
        প্রতিজ্ঞা(r > 0);

        ফেরত pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
