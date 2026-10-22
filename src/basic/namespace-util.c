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
        [NAMESPACE_TIME]   =  { "time",   "ns/time",   CLONE_NEWTIME,   PIDFD_GET_TIME_NAMESPACE,   PROC_TIME_INIT_INO   },
        {}, /* Allow callers to iterate over the array without using _NAMESPACE_TYPE_MAX. */
};

#define pid_namespace_path(pid, type) procfs_file_alloca(pid, namespace_info[type].proc_path)

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag) {
        עבור (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                אם (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        החזר t;

        החזר _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ثابت حرف *p = pid_namespace_path(0, type);
        החזר access(p, F_OK) >= 0;
}

ساكن صحيح pidref_namespace_open_by_type_internal(ثابت PidRef *pidref, NamespaceType type, bool *need_verify) {
        صحيح r;

        أكد(pidref_is_set(pidref));
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                אם (r == -ENOPKG)
                        החזר log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                אם (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        החזר r;
        }

        אם (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ صحيح nsfd = -EBADF;
        ثابت حرف *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        אם (nsfd == -ENOENT) {
                r = proc_mounted();
                אם (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        החזר log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                אם (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        החזר log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        אם (nsfd < 0)
                החזר nsfd;

        אם (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                אם (r < 0)
                        החזר r;
        }

        החזר TAKE_FD(nsfd);
}

صحيح pidref_namespace_open_by_type(ثابت PidRef *pidref, NamespaceType type) {
        החזר pidref_namespace_open_by_type_internal(pidref, type, عدم);
}

صحيح namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        صحيح r;

        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        אם (r < 0)
                החזר r;

        החזר pidref_namespace_open_by_type(&self, type);
}

صحيح pidref_namespace_open(
                ثابت PidRef *pidref,
                صحيح *ret_pidns_fd,
                صحيح *ret_mntns_fd,
                صحيح *ret_netns_fd,
                صحيح *ret_userns_fd,
                صحيح *ret_root_fd) {

        _cleanup_close_ صحيح pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, root_fd = -EBADF;
        bool need_verify = false;
        صحيح r;

        أكد(pidref_is_set(pidref));

        אם (pidref_is_remote(pidref))
                החזר -EREMOTE;

        אם (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                אם (pidns_fd < 0)
                        החזר pidns_fd;
        }

        אם (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                אם (mntns_fd < 0)
                        החזר mntns_fd;
        }

        אם (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                אם (netns_fd < 0)
                        החזר netns_fd;
        }

        אם (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                אם (userns_fd < 0 && userns_fd != -ENOPKG)
                        החזר userns_fd;
        }

        אם (ret_root_fd) {
                ثابت حرف *root;

                root = procfs_file_alloca(pidref->pid, "root");
                root_fd = RET_NERRNO(open(root, O_CLOEXEC|O_DIRECTORY));
                אם (root_fd == -ENOENT && proc_mounted() == 0)
                        החזר -ENOSYS;
                אם (root_fd < 0)
                        החזר root_fd;

                need_verify = true;
        }

        אם (need_verify) {
                r = pidref_verify(pidref);
                אם (r < 0)
                        החזר r;
        }

        אם (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        אם (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        אם (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        אם (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        אם (ret_root_fd)
                *ret_root_fd = TAKE_FD(root_fd);

        החזר 0;
}

صحيح namespace_open(
                pid_t pid,
                صحيح *ret_pidns_fd,
                صحيح *ret_mntns_fd,
                صحيح *ret_netns_fd,
                صحيح *ret_userns_fd,
                صحيح *ret_root_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        صحيح r;

        r = pidref_set_pid(&pidref, pid);
        אם (r < 0)
                החזר r;

        החזר pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_root_fd);
}

صحيح namespace_enter_full(
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح root_fd,
                NamespaceEnterFlags flags) {

        صحيح r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        אם (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                אם (r < 0)
                        החזר r;
                אם (r > 0)
                        pidns_fd = -EBADF;
        }

        אם (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                אם (r < 0)
                        החזר r;
                אם (r > 0)
                        mntns_fd = -EBADF;
        }

        אם (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                אם (r < 0)
                        החזר r;
                אם (r > 0)
                        netns_fd = -EBADF;
        }

        אם (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                אם (r < 0)
                        החזר r;
                אם (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        אם (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED)) {
                r = userns_has_root_mapping(userns_fd);
                אם (r < 0)
                        החזר r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        אם (r < 0)
                החזר r;

        bool have_cap_sys_admin = r > 0;

        אם (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                אם (userns_fd >= 0) {
                        אם (setns(userns_fd, CLONE_NEWUSER) < 0)
                                החזר -errno;
                } אחרת אם (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || root_fd >= 0)
                        החזר log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        אם (pidns_fd >= 0)
                אם (setns(pidns_fd, CLONE_NEWPID) < 0)
                        החזר -errno;

        אם (mntns_fd >= 0)
                אם (setns(mntns_fd, CLONE_NEWNS) < 0)
                        החזר -errno;

        אם (netns_fd >= 0)
                אם (setns(netns_fd, CLONE_NEWNET) < 0)
                        החזר -errno;

        אם (userns_fd >= 0 && have_cap_sys_admin)
                אם (setns(userns_fd, CLONE_NEWUSER) < 0)
                        החזר -errno;

        אם (root_fd >= 0) {
                אם (fchdir(root_fd) < 0)
                        החזר -errno;

                אם (chroot(".") < 0)
                        החזר -errno;
        }

        אם (userns_fd < 0)
                החזר 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ عدم);
        אם (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        אם (keep_uid_gid)
                החזר 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        אם (setresgid(0, 0, 0) < 0)
                החזר log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        אם (setresuid(0, 0, 0) < 0)
                החזר log_debug_errno(errno, "Failed to become root in user namespace: %m");

        החזר 0;
}

صحيح fd_is_namespace(صحيح fd, NamespaceType type) {
        صحيح r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        أكد(fd >= 0);
        أكد(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        אם (r <= 0)
                החזר r;

        אם (type < 0)
                החזר true;

        صحيح clone_flag = ioctl(fd, NS_GET_NSTYPE);
        אם (clone_flag < 0) {
                אם (errno != ENOTTY)
                        החזר -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ حرف *path = عدم;
                r = fd_get_path(fd, &path);
                אם (r < 0)
                        החזר r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                אם (path_is_absolute(path))
                        החזר -ENOTTY;

                ثابت حرف *p = startswith(path, namespace_info[type].proc_name);
                אם (!p)
                        החזר false;

                החזר startswith(p, ":[") != عدم;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        אם (found_type < 0)
                החזר -EBADF; /* Uh? Unknown namespace type? */

        החזר found_type == type;
}

صحيح is_our_namespace(صحيح fd, NamespaceType type) {
        صحيح r;

        أكد(fd >= 0);
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        אם (r < 0 && r != -ENOTTY)
                החזר r;
        אם (r == 0) /* Not a namespace or not of the right type? */
                החזר -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ صحيح our_ns = namespace_open_by_type(type);
        אם (our_ns < 0)
                החזר our_ns;

        r = fd_inode_same(fd, our_ns);
        אם (r == 0 && !type_verified)
                החזר -ENOTTY;

        החזר r;
}

صحيح are_our_namespaces(صحيح pidns_fd, صحيح mntns_fd, صحيح netns_fd, صحيح userns_fd, صحيح root_fd) {
        صحيح r;

        אם (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                אם (r <= 0)
                        החזר r;
        }

        אם (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                אם (r <= 0)
                        החזר r;
        }

        אם (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                אם (r <= 0)
                        החזר r;
        }

        אם (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                אם (r <= 0)
                        החזר r;
        }

        אם (root_fd >= 0) {
                r = dir_fd_is_root(root_fd);
                אם (r <= 0)
                        החזר r;
        }

        החזר true;
}

صحيح network_namespace_is_init(صحيح socket_fd) {
        بنية stat a, b;
        صحيح r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        אם (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                אם (r < 0)
                        החזר -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                החזר r ? true : -ENOSYS;
        }
        אם (r < 0)
                החזר r;

        אם (socket_fd >= 0) {
                _cleanup_close_ صحيح netns = ioctl(socket_fd, SIOCGSKNS);
                אם (netns < 0)
                        החזר -errno;

                אם (fstat(netns, &b) < 0)
                        החזר -errno;
        } אחרת {
                אם (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        החזר -errno;
        }

        החזר stat_inode_same(&a, &b);
}

صحيح namespace_is_init(NamespaceType type) {
        صحيح r;

        أكد(type >= 0);
        أكد(type < _NAMESPACE_TYPE_MAX);

        אם (type == NAMESPACE_NET)
                החזר network_namespace_is_init(/* socket_fd= */ -EBADF);

        אם (namespace_info[type].root_inode == 0)
                החזר -EBADR; /* Cannot answer this question */

        ثابت حرف *p = pid_namespace_path(0, type);

        بنية stat st;
        r = RET_NERRNO(stat(p, &st));
        אם (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                אם (r < 0)
                        החזר -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                החזר r ? true : -ENOSYS;
        }
        אם (r < 0)
                החזר r;

        החזר st.st_ino == namespace_info[type].root_inode;
}

صحيح pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ صحيح ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        أكد(!pid1 || pidref_is_set(pid1));
        أكد(!pid2 || pidref_is_set(pid2));
        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        אם (pidref_equal(pid1, pid2))
                החזר true;

        אם (!pid1)
                ns1 = namespace_open_by_type(type);
        אחרת
                ns1 = pidref_namespace_open_by_type(pid1, type);
        אם (ns1 < 0)
                החזר ns1;

        אם (!pid2)
                ns2 = namespace_open_by_type(type);
        אחרת
                ns2 = pidref_namespace_open_by_type(pid2, type);
        אם (ns2 < 0)
                החזר ns2;

        החזר fd_inode_same(ns1, ns2);
}

صحيح in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        أكد(pid1 >= 0);
        أكد(pid2 >= 0);
        החזר pidref_in_same_namespace(pid1 == 0 ? عدم : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? عدم : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

صحيح namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        صحيح r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        أكد(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        أكد(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        עבור (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                אם (r < 0)
                        החזר r;

                r = pidref_in_same_namespace(c, &parent, type);
                אם (r < 0)
                        החזר r;
                אם (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        אם (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        אחרת {
                                r = pidref_copy(c, ret);
                                אם (r < 0)
                                        החזר r;
                        }

                        החזר 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

صحيح detach_mount_namespace(فراغ) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        אם (unshare(CLONE_NEWNS) < 0)
                החזר log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        אם (mount(عدم, "/", عدم, MS_SLAVE | MS_REC, عدم) < 0)
                החזר log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        אם (mount(عدم, "/", عدم, MS_SHARED | MS_REC, عدم) < 0)
                החזר log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        החזר 0;
}

صحيح detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
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

        אם (!uid_is_valid(target_uid))
                החזר -EINVAL;
        אם (!gid_is_valid(target_gid))
                החזר -EINVAL;

        r = detach_mount_namespace();
        אם (r != -EPERM)
                החזר r;

        from_uid = getuid();
        from_gid = getgid();

        אם (unshare(CLONE_NEWUSER) < 0)
                החזר log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to write gid map: %m");

        החזר detach_mount_namespace();
}

صحيح detach_mount_namespace_userns(صحيح userns_fd) {
        صحيح r;

        أكد(userns_fd >= 0);

        אם (setns(userns_fd, CLONE_NEWUSER) < 0)
                החזר log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to become root in user namespace: %m");

        החזר detach_mount_namespace();
}

صحيح parse_userns_uid_range(ثابت حرف *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ حرف *buffer = عدم;
        ثابت حرف *range, *shift;
        صحيح r;
        uid_t uid_shift, uid_range = 65536;

        أكد(s);

        range = strchr(s, ':');
        אם (range) {
                buffer = strndup(s, range - s);
                אם (!buffer)
                        החזר log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                אם (r < 0)
                        החזר log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } אחרת
                shift = s;

        r = parse_uid(shift, &uid_shift);
        אם (r < 0)
                החזר log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        אם (!userns_shift_range_valid(uid_shift, uid_range))
                החזר log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        אם (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        אם (ret_uid_range)
                *ret_uid_range = uid_range;

        החזר 0;
}

صحيح userns_acquire_empty(فراغ) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحيح r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        אם (r < 0)
                החזר r;
        أكد(r > 0);

        החזר pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

صحيح userns_acquire(ثابت حرف *uid_map, ثابت حرف *gid_map, bool setgroups_deny) {
        حرف path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحيح r;

        أكد(uid_map);
        أكد(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        אם (r < 0)
                החזר r;
        أكد(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to write UID map: %m");

        אם (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                אם (r < 0)
                        החזר log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to write GID map: %m");

        החזר pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

صحيح userns_acquire_self_root(فراغ) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ حرف *uid_map = عدم, *gid_map = عدم;
        אם (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                החזר -ENOMEM;
        אם (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                החזר -ENOMEM;

        החזר userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

صحيح userns_enter_and_pin(صحيح userns_fd, PidRef *ret) {
        _cleanup_close_pair_ صحيح pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        حرف x;
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(ret);

        אם (pipe2(pfd, O_CLOEXEC) < 0)
                החזר -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ عدم,
                        (صحيح[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        אם (r < 0)
                החזר r;
        אם (r == 0) {
                /* Child. */

                אם (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = اكتب(pfd[1], &(ثابت حرف) { 'x' }, 1);
                אם (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                أكد(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = اقرأ(pfd[0], &x, 1);
        אם (n < 0)
                החזר -errno;
        אם (n == 0)
                החזר -EPROTO;
        أكد(n == 1);
        أكد(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        החזר 0;
}

bool userns_supported(فراغ) {
        החזר access("/proc/self/uid_map", F_OK) >= 0;
}

صحيح userns_get_base_uid(صحيح userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        אם (r < 0)
                החזר r;

        uid_t uid;
        r = uid_map_search_root(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        אם (r < 0)
                החזר r;

        gid_t gid;
        r = uid_map_search_root(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        אם (r < 0)
                החזר r;

        אם (!ret_gid && uid != gid)
                החזר -EUCLEAN;

        אם (ret_uid)
                *ret_uid = uid;
        אם (ret_gid)
                *ret_gid = gid;

        החזר 0;
}

صحيح userns_has_root_mapping(صحيح userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = عدم, *gid_range = عدم;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        אם (r < 0)
                החזר r;

        ثابت حرف *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        אם (r < 0)
                החזר r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        אם (r < 0)
                החזר r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        אם (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                החזר log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        החזר uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

صحيح process_is_owned_by_uid(ثابت PidRef *pidref, uid_t uid) {
        صحيح r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        أكد(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        אם (process_uid == uid)
                החזר true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ صحيح userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        אם (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                החזר false;
        אם (userns_fd < 0)
                החזר log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        עבור (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                אם (r < 0)
                        החזר log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                אם (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        החזר false;
                }

                אם (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        החזר log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                אם (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        החזר true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                אם (iteration > 16)
                        החזר log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ صحيح parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                אם (parent_fd < 0) {
                        אם (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                החזר false;
                        }

                        החזר log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
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

        אם (ns_id == 0)
                החזר -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        אם (r < 0)
                החזר r;
        אם (r == 0)
                החזר -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        אם (r < 0)
                החזר r;
        אם (r == 0)
                החזר -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        אם (r < 0)
                החזר r;
        אם (r == 0)
                החזר -EPERM;

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
        fh.file_handle.handle_bytes = حجم(بنية nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        החזר RET_NERRNO(open_by_handle_at(FD_NSFS_ROOT, &fh.file_handle, O_PATH|O_CLOEXEC));
}

صحيح is_idmapping_supported(ثابت حرف *path) {
        _cleanup_close_ صحيح mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        صحيح r;

        أكد(path);

        אם (!mount_new_api_supported())
                החזר false;

        userns_fd = r = userns_acquire_self_root();
        אם (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                החזר false;
        אם (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                החזר false;
        }
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        אם (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                החזר false;
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        אם (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                החזר false;
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(بنية mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, حجم(بنية mount_attr)));
        אם (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                החזר false;
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        החזר true;
}

صحيح netns_acquire(فراغ) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        صحيح r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        אם (r < 0)
                החזר log_debug_errno(r, "Failed to fork process into new netns: %m");
        أكد(r > 0);

        החזר pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
