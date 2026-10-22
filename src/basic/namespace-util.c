/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <linux/magic.h>
#ضمن <linux/nsfs.h>
#ضمن <linux/sockios.h>
#ضمن <sched.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/mount.h>
#ضمن <unistd.h>

#ضمن "capability-util.h"
#ضمن "dlfcn-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "log.h"
#ضمن "mountpoint-util.h"
#ضمن "namespace-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "pidfd-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"
#ضمن "uid-range.h"
#ضمن "unaligned.h"
#ضمن "user-util.h"

ثابت بنية namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        [NAMESPACE_وقت]   =  { "time",   "ns/time",   CLONE_NEWTIME,   PIDFD_GET_وقت_NAMESPACE,   PROC_وقت_INIT_INO   },
        {}, /* Allow callers to iterate over the array without using _NAMESPACE_TYPE_MAX. */
};

#وصف_ڪريو pid_namespace_رستو(pid, type) procfs_file_alloca(pid, namespace_info[type].proc_رستو)

NamespaceType clone_flag_منزل_namespace_type(بنانشان ڊگهو clone_flag) {
        لكل (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                إذا (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        أرجع t;

        أرجع _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ثابت حرف *p = pid_namespace_رستو(0, type);
        أرجع access(p, F_OK) >= 0;
}

ساكن صحيح pidref_namespace_open_by_type_اندروني(ثابت PidRef *pidref, NamespaceType type, bool *need_verify) {
        صحيح r;

        أكد(pidref_is_سيٽ(pidref));
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                إذا (r == -ENOPKG)
                        أرجع log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                إذا (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        أرجع r;
        }

        إذا (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ صحيح nsfd = -EBADF;
        ثابت حرف *p;

        p = pid_namespace_رستو(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        إذا (nsfd == -ENOENT) {
                r = proc_mounted();
                إذا (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        أرجع log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                إذا (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        أرجع log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        إذا (nsfd < 0)
                أرجع nsfd;

        إذا (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع TAKE_FD(nsfd);
}

صحيح pidref_namespace_open_by_type(ثابت PidRef *pidref, NamespaceType type) {
        أرجع pidref_namespace_open_by_type_اندروني(pidref, type, عدم);
}

صحيح namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_پورو) PidRef self = PIDREF_NULL;
        صحيح r;

        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_سيٽ_self(&self);
        إذا (r < 0)
                أرجع r;

        أرجع pidref_namespace_open_by_type(&self, type);
}

صحيح pidref_namespace_open(
                ثابت PidRef *pidref,
                صحيح *ret_pidns_fd,
                صحيح *ret_mntns_fd,
                صحيح *ret_netns_fd,
                صحيح *ret_userns_fd,
                صحيح *ret_پاڙ_fd) {

        _cleanup_close_ صحيح pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, پاڙ_fd = -EBADF;
        bool need_verify = false;
        صحيح r;

        أكد(pidref_is_سيٽ(pidref));

        إذا (pidref_is_remote(pidref))
                أرجع -EREMOTE;

        إذا (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_اندروني(pidref, NAMESPACE_PID, &need_verify);
                إذا (pidns_fd < 0)
                        أرجع pidns_fd;
        }

        إذا (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_اندروني(pidref, NAMESPACE_MOUNT, &need_verify);
                إذا (mntns_fd < 0)
                        أرجع mntns_fd;
        }

        إذا (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_اندروني(pidref, NAMESPACE_NET, &need_verify);
                إذا (netns_fd < 0)
                        أرجع netns_fd;
        }

        إذا (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_اندروني(pidref, NAMESPACE_USER, &need_verify);
                إذا (userns_fd < 0 && userns_fd != -ENOPKG)
                        أرجع userns_fd;
        }

        إذا (ret_پاڙ_fd) {
                ثابت حرف *پاڙ;

                پاڙ = procfs_file_alloca(pidref->pid, "root");
                پاڙ_fd = RET_NERRNO(open(پاڙ, O_CLOEXEC|O_DIRECTORY));
                إذا (پاڙ_fd == -ENOENT && proc_mounted() == 0)
                        أرجع -ENOSYS;
                إذا (پاڙ_fd < 0)
                        أرجع پاڙ_fd;

                need_verify = true;
        }

        إذا (need_verify) {
                r = pidref_verify(pidref);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        إذا (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        إذا (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        إذا (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        إذا (ret_پاڙ_fd)
                *ret_پاڙ_fd = TAKE_FD(پاڙ_fd);

        أرجع 0;
}

صحيح namespace_open(
                pid_t pid,
                صحيح *ret_pidns_fd,
                صحيح *ret_mntns_fd,
                صحيح *ret_netns_fd,
                صحيح *ret_userns_fd,
                صحيح *ret_پاڙ_fd) {

        _cleanup_(pidref_پورو) PidRef pidref = PIDREF_NULL;
        صحيح r;

        r = pidref_سيٽ_pid(&pidref, pid);
        إذا (r < 0)
                أرجع r;

        أرجع pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_پاڙ_fd);
}

صحيح namespace_enter_ڀريل(
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح پاڙ_fd,
                NamespaceEnterFlags flags) {

        صحيح r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        بلاڪ_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        إذا (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        pidns_fd = -EBADF;
        }

        إذا (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        mntns_fd = -EBADF;
        }

        إذا (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        netns_fd = -EBADF;
        }

        إذا (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        إذا (userns_fd >= 0 && FLAGS_سيٽ(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_پاڙ_UNMAPPED)) {
                r = userns_has_پاڙ_mapping(userns_fd);
                إذا (r < 0)
                        أرجع r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        إذا (r < 0)
                أرجع r;

        bool have_cap_sys_admin = r > 0;

        إذا (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                إذا (userns_fd >= 0) {
                        إذا (setns(userns_fd, CLONE_NEWUSER) < 0)
                                أرجع -errno;
                } وإلا إذا (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || پاڙ_fd >= 0)
                        أرجع log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        إذا (pidns_fd >= 0)
                إذا (setns(pidns_fd, CLONE_NEWPID) < 0)
                        أرجع -errno;

        إذا (mntns_fd >= 0)
                إذا (setns(mntns_fd, CLONE_NEWNS) < 0)
                        أرجع -errno;

        إذا (netns_fd >= 0)
                إذا (setns(netns_fd, CLONE_NEWNET) < 0)
                        أرجع -errno;

        إذا (userns_fd >= 0 && have_cap_sys_admin)
                إذا (setns(userns_fd, CLONE_NEWUSER) < 0)
                        أرجع -errno;

        إذا (پاڙ_fd >= 0) {
                إذا (fchdir(پاڙ_fd) < 0)
                        أرجع -errno;

                إذا (chroot(".") < 0)
                        أرجع -errno;
        }

        إذا (userns_fd < 0)
                أرجع 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ عدم);
        إذا (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        إذا (keep_uid_gid)
                أرجع 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        إذا (setresgid(0, 0, 0) < 0)
                أرجع log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        إذا (setresuid(0, 0, 0) < 0)
                أرجع log_debug_errno(errno, "Failed to become root in user namespace: %m");

        أرجع 0;
}

صحيح fd_is_namespace(صحيح fd, NamespaceType type) {
        صحيح r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        أكد(fd >= 0);
        أكد(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        إذا (r <= 0)
                أرجع r;

        إذا (type < 0)
                أرجع true;

        صحيح clone_flag = ioctl(fd, NS_GET_NSTYPE);
        إذا (clone_flag < 0) {
                إذا (errno != ENOTTY)
                        أرجع -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ حرف *رستو = عدم;
                r = fd_get_رستو(fd, &رستو);
                إذا (r < 0)
                        أرجع r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                إذا (رستو_is_absolute(رستو))
                        أرجع -ENOTTY;

                ثابت حرف *p = startswith(رستو, namespace_info[type].proc_name);
                إذا (!p)
                        أرجع false;

                أرجع startswith(p, ":[") != عدم;
        }

        NamespaceType مليو_type = clone_flag_منزل_namespace_type(clone_flag);
        إذا (مليو_type < 0)
                أرجع -EBADF; /* Uh? Unknown namespace type? */

        أرجع مليو_type == type;
}

صحيح is_our_namespace(صحيح fd, NamespaceType type) {
        صحيح r;

        أكد(fd >= 0);
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        إذا (r < 0 && r != -ENOTTY)
                أرجع r;
        إذا (r == 0) /* Not a namespace or not of the right type? */
                أرجع -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ صحيح our_ns = namespace_open_by_type(type);
        إذا (our_ns < 0)
                أرجع our_ns;

        r = fd_inode_same(fd, our_ns);
        إذا (r == 0 && !type_verified)
                أرجع -ENOTTY;

        أرجع r;
}

صحيح are_our_namespaces(صحيح pidns_fd, صحيح mntns_fd, صحيح netns_fd, صحيح userns_fd, صحيح پاڙ_fd) {
        صحيح r;

        إذا (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                إذا (r <= 0)
                        أرجع r;
        }

        إذا (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                إذا (r <= 0)
                        أرجع r;
        }

        إذا (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                إذا (r <= 0)
                        أرجع r;
        }

        إذا (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                إذا (r <= 0)
                        أرجع r;
        }

        إذا (پاڙ_fd >= 0) {
                r = dir_fd_is_پاڙ(پاڙ_fd);
                إذا (r <= 0)
                        أرجع r;
        }

        أرجع true;
}

صحيح ڄار_namespace_is_init(صحيح socket_fd) {
        بنية stat a, b;
        صحيح r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_رستو(1, NAMESPACE_NET), &a));
        إذا (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                إذا (r < 0)
                        أرجع -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                أرجع r ? true : -ENOSYS;
        }
        إذا (r < 0)
                أرجع r;

        إذا (socket_fd >= 0) {
                _cleanup_close_ صحيح netns = ioctl(socket_fd, SIOCGSKNS);
                إذا (netns < 0)
                        أرجع -errno;

                إذا (fstat(netns, &b) < 0)
                        أرجع -errno;
        } وإلا {
                إذا (stat(pid_namespace_رستو(0, NAMESPACE_NET), &b) < 0)
                        أرجع -errno;
        }

        أرجع stat_inode_same(&a, &b);
}

صحيح namespace_is_init(NamespaceType type) {
        صحيح r;

        أكد(type >= 0);
        أكد(type < _NAMESPACE_TYPE_MAX);

        إذا (type == NAMESPACE_NET)
                أرجع ڄار_namespace_is_init(/* socket_fd= */ -EBADF);

        إذا (namespace_info[type].پاڙ_inode == 0)
                أرجع -EBADR; /* Cannot answer this question */

        ثابت حرف *p = pid_namespace_رستو(0, type);

        بنية stat st;
        r = RET_NERRNO(stat(p, &st));
        إذا (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                إذا (r < 0)
                        أرجع -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                أرجع r ? true : -ENOSYS;
        }
        إذا (r < 0)
                أرجع r;

        أرجع st.st_ino == namespace_info[type].پاڙ_inode;
}

صحيح pidref_اندريون_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ صحيح ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        أكد(!pid1 || pidref_is_سيٽ(pid1));
        أكد(!pid2 || pidref_is_سيٽ(pid2));
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        إذا (pidref_equal(pid1, pid2))
                أرجع true;

        إذا (!pid1)
                ns1 = namespace_open_by_type(type);
        وإلا
                ns1 = pidref_namespace_open_by_type(pid1, type);
        إذا (ns1 < 0)
                أرجع ns1;

        إذا (!pid2)
                ns2 = namespace_open_by_type(type);
        وإلا
                ns2 = pidref_namespace_open_by_type(pid2, type);
        إذا (ns2 < 0)
                أرجع ns2;

        أرجع fd_inode_same(ns1, ns2);
}

صحيح اندريون_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        أكد(pid1 >= 0);
        أكد(pid2 >= 0);
        أرجع pidref_اندريون_same_namespace(pid1 == 0 ? عدم : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? عدم : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

صحيح namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        صحيح r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        أكد(ret);

        _cleanup_(pidref_پورو) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        لكل (;;) {
                _cleanup_(pidref_پورو) PidRef والد = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &والد);
                إذا (r < 0)
                        أرجع r;

                r = pidref_اندريون_same_namespace(c, &والد, type);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        إذا (pidref_is_سيٽ(&current))
                                *ret = TAKE_PIDREF(current);
                        وإلا {
                                r = pidref_copy(c, ret);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        أرجع 0;
                }

                pidref_پورو(&current);
                current = TAKE_PIDREF(والد);
                c = &current;
        }
}

صحيح detach_mount_namespace(فراغ) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        إذا (unshare(CLONE_NEWNS) < 0)
                أرجع log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        إذا (mount(عدم, "/", عدم, MS_SLAVE | MS_REC, عدم) < 0)
                أرجع log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        إذا (mount(عدم, "/", عدم, MS_SHARED | MS_REC, عدم) < 0)
                أرجع log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        أرجع 0;
}

صحيح detach_mount_namespace_harder(uid_t هدف_uid, gid_t هدف_gid) {
        uid_t from_uid;
        gid_t from_gid;
        صحيح r;

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

        إذا (!uid_is_valid(هدف_uid))
                أرجع -EINVAL;
        إذا (!gid_is_valid(هدف_gid))
                أرجع -EINVAL;

        r = detach_mount_namespace();
        إذا (r != -EPERM)
                أرجع r;

        from_uid = getuid();
        from_gid = getgid();

        إذا (unshare(CLONE_NEWUSER) < 0)
                أرجع log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", هدف_uid, from_uid);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", هدف_gid, from_gid);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to write gid map: %m");

        أرجع detach_mount_namespace();
}

صحيح detach_mount_namespace_userns(صحيح userns_fd) {
        صحيح r;

        أكد(userns_fd >= 0);

        إذا (setns(userns_fd, CLONE_NEWUSER) < 0)
                أرجع log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to become root in user namespace: %m");

        أرجع detach_mount_namespace();
}

صحيح parse_userns_uid_range(ثابت حرف *s, uid_t *ret_uid_سرڪايو, uid_t *ret_uid_range) {
        _cleanup_free_ حرف *بفر = عدم;
        ثابت حرف *range, *سرڪايو;
        صحيح r;
        uid_t uid_سرڪايو, uid_range = 65536;

        أكد(s);

        range = strchr(s, ':');
        إذا (range) {
                بفر = strndup(s, range - s);
                إذا (!بفر)
                        أرجع log_oom();
                سرڪايو = بفر;

                range++;
                r = محفوظ_atou32(range, &uid_range);
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } وإلا
                سرڪايو = s;

        r = parse_uid(سرڪايو, &uid_سرڪايو);
        إذا (r < 0)
                أرجع log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        إذا (!userns_سرڪايو_range_valid(uid_سرڪايو, uid_range))
                أرجع log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        إذا (ret_uid_سرڪايو)
                *ret_uid_سرڪايو = uid_سرڪايو;

        إذا (ret_uid_range)
                *ret_uid_range = uid_range;

        أرجع 0;
}

صحيح userns_acquire_خالي_جايون(فراغ) {
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحيح r;

        r = pidref_محفوظ_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        إذا (r < 0)
                أرجع r;
        أكد(r > 0);

        أرجع pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

صحيح userns_acquire(ثابت حرف *uid_map, ثابت حرف *gid_map, bool setgroups_deny) {
        حرف رستو[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحيح r;

        أكد(uid_map);
        أكد(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_محفوظ_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        إذا (r < 0)
                أرجع r;
        أكد(r > 0);

        xsprintf(رستو, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(رستو, uid_map, WRITE_STRING_FILE_DISABLE_بفر);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to write UID map: %m");

        إذا (setgroups_deny) {
                xsprintf(رستو, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(رستو, "deny", WRITE_STRING_FILE_DISABLE_بفر);
                إذا (r < 0)
                        أرجع log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(رستو, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(رستو, gid_map, WRITE_STRING_FILE_DISABLE_بفر);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to write GID map: %m");

        أرجع pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

صحيح userns_acquire_self_پاڙ(فراغ) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ حرف *uid_map = عدم, *gid_map = عدم;
        إذا (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                أرجع -ENOMEM;
        إذا (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                أرجع -ENOMEM;

        أرجع userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

صحيح userns_enter_and_pin(صحيح userns_fd, PidRef *ret) {
        _cleanup_close_pair_ صحيح pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        حرف x;
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(ret);

        إذا (pipe2(pfd, O_CLOEXEC) < 0)
                أرجع -errno;

        r = pidref_محفوظ_fork_ڀريل(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ عدم,
                        (صحيح[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) {
                /* Child. */

                إذا (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = محفوظ_close(userns_fd);

                n = اكتب(pfd[1], &(ثابت حرف) { 'x' }, 1);
                إذا (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                أكد(n == 1);

                freeze();
        }

        pfd[1] = محفوظ_close(pfd[1]);

        n = اقرأ(pfd[0], &x, 1);
        إذا (n < 0)
                أرجع -errno;
        إذا (n == 0)
                أرجع -EPROTO;
        أكد(n == 1);
        أكد(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        أرجع 0;
}

bool userns_supported(فراغ) {
        أرجع access("/proc/self/uid_map", F_OK) >= 0;
}

صحيح userns_get_بنياد_uid(صحيح userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        إذا (r < 0)
                أرجع r;

        uid_t uid;
        r = uid_map_ڳولا_پاڙ(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        إذا (r < 0)
                أرجع r;

        gid_t gid;
        r = uid_map_ڳولا_پاڙ(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        إذا (r < 0)
                أرجع r;

        إذا (!ret_gid && uid != gid)
                أرجع -EUCLEAN;

        إذا (ret_uid)
                *ret_uid = uid;
        إذا (ret_gid)
                *ret_gid = gid;

        أرجع 0;
}

صحيح userns_has_پاڙ_mapping(صحيح userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = عدم, *gid_range = عدم;
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        إذا (r < 0)
                أرجع r;

        ثابت حرف *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_ڀريو_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        إذا (r < 0)
                أرجع r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_ڀريو_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        إذا (r < 0)
                أرجع r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        إذا (uid_range_is_خالي_جايون(uid_range) || uid_range_is_خالي_جايون(gid_range))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        أرجع uid_range_شامل_آهي(uid_range, 0) && uid_range_شامل_آهي(gid_range, 0);
}

صحيح process_is_owned_by_uid(ثابت PidRef *pidref, uid_t uid) {
        صحيح r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        أكد(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        إذا (process_uid == uid)
                أرجع true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ صحيح userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        إذا (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                أرجع false;
        إذا (userns_fd < 0)
                أرجع log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        لكل (بنانشان iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                إذا (r < 0)
                        أرجع log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                إذا (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        أرجع false;
                }

                إذا (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        أرجع log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                إذا (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        أرجع true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                إذا (iteration > 16)
                        أرجع log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ صحيح والد_fd = ioctl(userns_fd, NS_GET_USERNS);
                إذا (والد_fd < 0) {
                        إذا (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                أرجع false;
                        }

                        أرجع log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, والد_fd);
        }
}

صحيح namespace_open_by_id(uint64_t ns_id) {
        صحيح r;

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

        إذا (ns_id == 0)
                أرجع -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EPERM;

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

        اتحاد {
                بنية file_handle file_handle;
                uint8_t space[offsetof(بنية file_handle, f_handle) + حجم(بنية nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_بائيٽون = حجم(بنية nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        أرجع RET_NERRNO(open_by_handle_at(FD_NSFS_پاڙ, &fh.file_handle, O_رستو|O_CLOEXEC));
}

صحيح is_idmapping_supported(ثابت حرف *رستو) {
        _cleanup_close_ صحيح mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        صحيح r;

        أكد(رستو);

        إذا (!mount_new_api_supported())
                أرجع false;

        userns_fd = r = userns_acquire_self_پاڙ();
        إذا (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                أرجع false;
        إذا (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                أرجع false;
        }
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", رستو);

        dir_fd = r = RET_NERRNO(open(رستو, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        إذا (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                أرجع false;
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", رستو);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_خالي_جايون_رستو | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        إذا (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                أرجع false;
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", رستو);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_خالي_جايون_رستو,
                       &(بنية mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, حجم(بنية mount_attr)));
        إذا (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                أرجع false;
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", رستو);

        أرجع true;
}

صحيح netns_acquire(فراغ) {
        _cleanup_(pidref_پورو_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحيح r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_محفوظ_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to fork process into new netns: %m");
        أكد(r > 0);

        أرجع pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
