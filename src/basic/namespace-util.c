/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <linux/magic.h>
#شامل <linux/nsfs.h>
#شامل <linux/sockios.h>
#شامل <sched.h>
#شامل <sys/ioctl.h>
#شامل <sys/mount.h>
#شامل <unistd.h>

#شامل "capability-util.h"
#شامل "dlfcn-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "log.h"
#شامل "mountpoint-util.h"
#شامل "namespace-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "pidfd-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"
#شامل "uid-range.h"
#شامل "unaligned.h"
#شامل "user-util.h"

ثابت ساختار namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        برای (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                اگر (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        بازگشت t;

        بازگشت _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ثابت نویسه *p = pid_namespace_path(0, type);
        بازگشت access(p, F_OK) >= 0;
}

ایستا صحیح pidref_namespace_open_by_type_internal(ثابت PidRef *pidref, NamespaceType type, bool *need_verify) {
        صحیح r;

        أكد(pidref_is_set(pidref));
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                اگر (r == -ENOPKG)
                        بازگشت log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                اگر (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        بازگشت r;
        }

        اگر (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ صحیح nsfd = -EBADF;
        ثابت نویسه *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        اگر (nsfd == -ENOENT) {
                r = proc_mounted();
                اگر (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        بازگشت log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                اگر (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        بازگشت log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        اگر (nsfd < 0)
                بازگشت nsfd;

        اگر (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت TAKE_FD(nsfd);
}

صحیح pidref_namespace_open_by_type(ثابت PidRef *pidref, NamespaceType type) {
        بازگشت pidref_namespace_open_by_type_internal(pidref, type, پوچ);
}

صحیح namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        صحیح r;

        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        اگر (r < 0)
                بازگشت r;

        بازگشت pidref_namespace_open_by_type(&self, type);
}

صحیح pidref_namespace_open(
                ثابت PidRef *pidref,
                صحیح *ret_pidns_fd,
                صحیح *ret_mntns_fd,
                صحیح *ret_netns_fd,
                صحیح *ret_userns_fd,
                صحیح *ret_ریشه_fd) {

        _cleanup_close_ صحیح pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, ریشه_fd = -EBADF;
        bool need_verify = false;
        صحیح r;

        أكد(pidref_is_set(pidref));

        اگر (pidref_is_remote(pidref))
                بازگشت -EREMOTE;

        اگر (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                اگر (pidns_fd < 0)
                        بازگشت pidns_fd;
        }

        اگر (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                اگر (mntns_fd < 0)
                        بازگشت mntns_fd;
        }

        اگر (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                اگر (netns_fd < 0)
                        بازگشت netns_fd;
        }

        اگر (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                اگر (userns_fd < 0 && userns_fd != -ENOPKG)
                        بازگشت userns_fd;
        }

        اگر (ret_ریشه_fd) {
                ثابت نویسه *ریشه;

                ریشه = procfs_file_alloca(pidref->pid, "root");
                ریشه_fd = RET_NERRNO(open(ریشه, O_CLOEXEC|O_DIRECTORY));
                اگر (ریشه_fd == -ENOENT && proc_mounted() == 0)
                        بازگشت -ENOSYS;
                اگر (ریشه_fd < 0)
                        بازگشت ریشه_fd;

                need_verify = true;
        }

        اگر (need_verify) {
                r = pidref_verify(pidref);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        اگر (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        اگر (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        اگر (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        اگر (ret_ریشه_fd)
                *ret_ریشه_fd = TAKE_FD(ریشه_fd);

        بازگشت 0;
}

صحیح namespace_open(
                pid_t pid,
                صحیح *ret_pidns_fd,
                صحیح *ret_mntns_fd,
                صحیح *ret_netns_fd,
                صحیح *ret_userns_fd,
                صحیح *ret_ریشه_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        صحیح r;

        r = pidref_set_pid(&pidref, pid);
        اگر (r < 0)
                بازگشت r;

        بازگشت pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_ریشه_fd);
}

صحیح namespace_enter_full(
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح ریشه_fd,
                NamespaceEnterFlags flags) {

        صحیح r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        اگر (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                اگر (r < 0)
                        بازگشت r;
                اگر (r > 0)
                        pidns_fd = -EBADF;
        }

        اگر (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                اگر (r < 0)
                        بازگشت r;
                اگر (r > 0)
                        mntns_fd = -EBADF;
        }

        اگر (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                اگر (r < 0)
                        بازگشت r;
                اگر (r > 0)
                        netns_fd = -EBADF;
        }

        اگر (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                اگر (r < 0)
                        بازگشت r;
                اگر (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        اگر (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ریشه_UNMAPPED)) {
                r = userns_has_ریشه_mapping(userns_fd);
                اگر (r < 0)
                        بازگشت r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        اگر (r < 0)
                بازگشت r;

        bool have_cap_sys_admin = r > 0;

        اگر (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                اگر (userns_fd >= 0) {
                        اگر (setns(userns_fd, CLONE_NEWUSER) < 0)
                                بازگشت -errno;
                } وگرنه اگر (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || ریشه_fd >= 0)
                        بازگشت log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        اگر (pidns_fd >= 0)
                اگر (setns(pidns_fd, CLONE_NEWPID) < 0)
                        بازگشت -errno;

        اگر (mntns_fd >= 0)
                اگر (setns(mntns_fd, CLONE_NEWNS) < 0)
                        بازگشت -errno;

        اگر (netns_fd >= 0)
                اگر (setns(netns_fd, CLONE_NEWNET) < 0)
                        بازگشت -errno;

        اگر (userns_fd >= 0 && have_cap_sys_admin)
                اگر (setns(userns_fd, CLONE_NEWUSER) < 0)
                        بازگشت -errno;

        اگر (ریشه_fd >= 0) {
                اگر (fchdir(ریشه_fd) < 0)
                        بازگشت -errno;

                اگر (chroot(".") < 0)
                        بازگشت -errno;
        }

        اگر (userns_fd < 0)
                بازگشت 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ پوچ);
        اگر (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        اگر (keep_uid_gid)
                بازگشت 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        اگر (setresgid(0, 0, 0) < 0)
                بازگشت log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        اگر (setresuid(0, 0, 0) < 0)
                بازگشت log_debug_errno(errno, "Failed to become root in user namespace: %m");

        بازگشت 0;
}

صحیح fd_is_namespace(صحیح fd, NamespaceType type) {
        صحیح r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        أكد(fd >= 0);
        أكد(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        اگر (r <= 0)
                بازگشت r;

        اگر (type < 0)
                بازگشت true;

        صحیح clone_flag = ioctl(fd, NS_GET_NSTYPE);
        اگر (clone_flag < 0) {
                اگر (errno != ENOTTY)
                        بازگشت -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ نویسه *path = پوچ;
                r = fd_get_path(fd, &path);
                اگر (r < 0)
                        بازگشت r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                اگر (path_is_absolute(path))
                        بازگشت -ENOTTY;

                ثابت نویسه *p = startswith(path, namespace_info[type].proc_name);
                اگر (!p)
                        بازگشت false;

                بازگشت startswith(p, ":[") != پوچ;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        اگر (found_type < 0)
                بازگشت -EBADF; /* Uh? Unknown namespace type? */

        بازگشت found_type == type;
}

صحیح is_our_namespace(صحیح fd, NamespaceType type) {
        صحیح r;

        أكد(fd >= 0);
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        اگر (r < 0 && r != -ENOTTY)
                بازگشت r;
        اگر (r == 0) /* Not a namespace or not of the right type? */
                بازگشت -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ صحیح our_ns = namespace_open_by_type(type);
        اگر (our_ns < 0)
                بازگشت our_ns;

        r = fd_inode_same(fd, our_ns);
        اگر (r == 0 && !type_verified)
                بازگشت -ENOTTY;

        بازگشت r;
}

صحیح are_our_namespaces(صحیح pidns_fd, صحیح mntns_fd, صحیح netns_fd, صحیح userns_fd, صحیح ریشه_fd) {
        صحیح r;

        اگر (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                اگر (r <= 0)
                        بازگشت r;
        }

        اگر (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                اگر (r <= 0)
                        بازگشت r;
        }

        اگر (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                اگر (r <= 0)
                        بازگشت r;
        }

        اگر (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                اگر (r <= 0)
                        بازگشت r;
        }

        اگر (ریشه_fd >= 0) {
                r = dir_fd_is_ریشه(ریشه_fd);
                اگر (r <= 0)
                        بازگشت r;
        }

        بازگشت true;
}

صحیح network_namespace_is_init(صحیح socket_fd) {
        ساختار stat a, b;
        صحیح r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        اگر (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                اگر (r < 0)
                        بازگشت -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                بازگشت r ? true : -ENOSYS;
        }
        اگر (r < 0)
                بازگشت r;

        اگر (socket_fd >= 0) {
                _cleanup_close_ صحیح netns = ioctl(socket_fd, SIOCGSKNS);
                اگر (netns < 0)
                        بازگشت -errno;

                اگر (fstat(netns, &b) < 0)
                        بازگشت -errno;
        } وگرنه {
                اگر (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        بازگشت -errno;
        }

        بازگشت stat_inode_same(&a, &b);
}

صحیح namespace_is_init(NamespaceType type) {
        صحیح r;

        أكد(type >= 0);
        أكد(type < _NAMESPACE_TYPE_MAX);

        اگر (type == NAMESPACE_NET)
                بازگشت network_namespace_is_init(/* socket_fd= */ -EBADF);

        اگر (namespace_info[type].ریشه_inode == 0)
                بازگشت -EBADR; /* Cannot answer this question */

        ثابت نویسه *p = pid_namespace_path(0, type);

        ساختار stat st;
        r = RET_NERRNO(stat(p, &st));
        اگر (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                اگر (r < 0)
                        بازگشت -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                بازگشت r ? true : -ENOSYS;
        }
        اگر (r < 0)
                بازگشت r;

        بازگشت st.st_ino == namespace_info[type].ریشه_inode;
}

صحیح pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ صحیح ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        أكد(!pid1 || pidref_is_set(pid1));
        أكد(!pid2 || pidref_is_set(pid2));
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        اگر (pidref_equal(pid1, pid2))
                بازگشت true;

        اگر (!pid1)
                ns1 = namespace_open_by_type(type);
        وگرنه
                ns1 = pidref_namespace_open_by_type(pid1, type);
        اگر (ns1 < 0)
                بازگشت ns1;

        اگر (!pid2)
                ns2 = namespace_open_by_type(type);
        وگرنه
                ns2 = pidref_namespace_open_by_type(pid2, type);
        اگر (ns2 < 0)
                بازگشت ns2;

        بازگشت fd_inode_same(ns1, ns2);
}

صحیح in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        أكد(pid1 >= 0);
        أكد(pid2 >= 0);
        بازگشت pidref_in_same_namespace(pid1 == 0 ? پوچ : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? پوچ : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

صحیح namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        صحیح r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        أكد(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        برای (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                اگر (r < 0)
                        بازگشت r;

                r = pidref_in_same_namespace(c, &parent, type);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        اگر (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        وگرنه {
                                r = pidref_copy(c, ret);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        بازگشت 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

صحیح detach_mount_namespace(تهی) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        اگر (unshare(CLONE_NEWNS) < 0)
                بازگشت log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        اگر (mount(پوچ, "/", پوچ, MS_SLAVE | MS_REC, پوچ) < 0)
                بازگشت log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        اگر (mount(پوچ, "/", پوچ, MS_SHARED | MS_REC, پوچ) < 0)
                بازگشت log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        بازگشت 0;
}

صحیح detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        صحیح r;

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

        اگر (!uid_is_valid(target_uid))
                بازگشت -EINVAL;
        اگر (!gid_is_valid(target_gid))
                بازگشت -EINVAL;

        r = detach_mount_namespace();
        اگر (r != -EPERM)
                بازگشت r;

        from_uid = getuid();
        from_gid = getgid();

        اگر (unshare(CLONE_NEWUSER) < 0)
                بازگشت log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to write gid map: %m");

        بازگشت detach_mount_namespace();
}

صحیح detach_mount_namespace_userns(صحیح userns_fd) {
        صحیح r;

        أكد(userns_fd >= 0);

        اگر (setns(userns_fd, CLONE_NEWUSER) < 0)
                بازگشت log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to become root in user namespace: %m");

        بازگشت detach_mount_namespace();
}

صحیح parse_userns_uid_range(ثابت نویسه *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ نویسه *buffer = پوچ;
        ثابت نویسه *range, *shift;
        صحیح r;
        uid_t uid_shift, uid_range = 65536;

        أكد(s);

        range = strchr(s, ':');
        اگر (range) {
                buffer = strndup(s, range - s);
                اگر (!buffer)
                        بازگشت log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                اگر (r < 0)
                        بازگشت log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } وگرنه
                shift = s;

        r = parse_uid(shift, &uid_shift);
        اگر (r < 0)
                بازگشت log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        اگر (!userns_shift_range_valid(uid_shift, uid_range))
                بازگشت log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        اگر (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        اگر (ret_uid_range)
                *ret_uid_range = uid_range;

        بازگشت 0;
}

صحیح userns_acquire_empty(تهی) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحیح r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        اگر (r < 0)
                بازگشت r;
        أكد(r > 0);

        بازگشت pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

صحیح userns_acquire(ثابت نویسه *uid_map, ثابت نویسه *gid_map, bool setgroups_deny) {
        نویسه path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحیح r;

        أكد(uid_map);
        أكد(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        اگر (r < 0)
                بازگشت r;
        أكد(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to write UID map: %m");

        اگر (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to write GID map: %m");

        بازگشت pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

صحیح userns_acquire_self_ریشه(تهی) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ نویسه *uid_map = پوچ, *gid_map = پوچ;
        اگر (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                بازگشت -ENOMEM;
        اگر (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                بازگشت -ENOMEM;

        بازگشت userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

صحیح userns_enter_and_pin(صحیح userns_fd, PidRef *ret) {
        _cleanup_close_pair_ صحیح pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        نویسه x;
        صحیح r;

        أكد(userns_fd >= 0);
        أكد(ret);

        اگر (pipe2(pfd, O_CLOEXEC) < 0)
                بازگشت -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ پوچ,
                        (صحیح[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0) {
                /* Child. */

                اگر (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = اكتب(pfd[1], &(ثابت نویسه) { 'x' }, 1);
                اگر (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                أكد(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = اقرأ(pfd[0], &x, 1);
        اگر (n < 0)
                بازگشت -errno;
        اگر (n == 0)
                بازگشت -EPROTO;
        أكد(n == 1);
        أكد(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        بازگشت 0;
}

bool userns_supported(تهی) {
        بازگشت access("/proc/self/uid_map", F_OK) >= 0;
}

صحیح userns_get_base_uid(صحیح userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحیح r;

        أكد(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        اگر (r < 0)
                بازگشت r;

        uid_t uid;
        r = uid_map_جستجو_ریشه(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        اگر (r < 0)
                بازگشت r;

        gid_t gid;
        r = uid_map_جستجو_ریشه(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        اگر (r < 0)
                بازگشت r;

        اگر (!ret_gid && uid != gid)
                بازگشت -EUCLEAN;

        اگر (ret_uid)
                *ret_uid = uid;
        اگر (ret_gid)
                *ret_gid = gid;

        بازگشت 0;
}

صحیح userns_has_ریشه_mapping(صحیح userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = پوچ, *gid_range = پوچ;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحیح r;

        أكد(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        اگر (r < 0)
                بازگشت r;

        ثابت نویسه *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        اگر (r < 0)
                بازگشت r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        اگر (r < 0)
                بازگشت r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        اگر (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        بازگشت uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

صحیح process_is_owned_by_uid(ثابت PidRef *pidref, uid_t uid) {
        صحیح r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        أكد(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        اگر (process_uid == uid)
                بازگشت true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ صحیح userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        اگر (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                بازگشت false;
        اگر (userns_fd < 0)
                بازگشت log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        برای (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                اگر (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        بازگشت false;
                }

                اگر (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        بازگشت log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                اگر (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        بازگشت true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                اگر (iteration > 16)
                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ صحیح parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                اگر (parent_fd < 0) {
                        اگر (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                بازگشت false;
                        }

                        بازگشت log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

صحیح namespace_open_by_id(uint64_t ns_id) {
        صحیح r;

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

        اگر (ns_id == 0)
                بازگشت -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EPERM;

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

        اجتماع {
                ساختار file_handle file_handle;
                uint8_t space[offsetof(ساختار file_handle, f_handle) + اندازه(ساختار nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = اندازه(ساختار nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        بازگشت RET_NERRNO(open_by_handle_at(FD_NSFS_ریشه, &fh.file_handle, O_PATH|O_CLOEXEC));
}

صحیح is_idmapping_supported(ثابت نویسه *path) {
        _cleanup_close_ صحیح mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        صحیح r;

        أكد(path);

        اگر (!mount_new_api_supported())
                بازگشت false;

        userns_fd = r = userns_acquire_self_ریشه();
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                بازگشت false;
        اگر (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                بازگشت false;
        }
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                بازگشت false;
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                بازگشت false;
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(ساختار mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, اندازه(ساختار mount_attr)));
        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                بازگشت false;
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        بازگشت true;
}

صحیح netns_acquire(تهی) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحیح r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to fork process into new netns: %m");
        أكد(r > 0);

        بازگشت pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
