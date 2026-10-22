/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <linux/magic.h>
#अन्तर्भाव <linux/nsfs.h>
#अन्तर्भाव <linux/sockios.h>
#अन्तर्भाव <sched.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/mount.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "capability-util.h"
#अन्तर्भाव "dlfcn-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mountpoint-util.h"
#अन्तर्भाव "namespace-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidfd-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "uid-range.h"
#अन्तर्भाव "unaligned.h"
#अन्तर्भाव "user-util.h"

स्थिरांक संरचना namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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

NamespaceType clone_flag_to_namespace_type(अचिन्हित दीर्घ clone_flag) {
        साठी (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                जर (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        परत t;

        परत _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        प्रतिज्ञा(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        स्थिरांक अक्षर *p = pid_namespace_path(0, type);
        परत access(p, F_OK) >= 0;
}

स्थिर पूर्णांक pidref_namespace_open_by_type_internal(स्थिरांक PidRef *pidref, NamespaceType type, bool *need_verify) {
        पूर्णांक r;

        प्रतिज्ञा(pidref_is_set(pidref));
        प्रतिज्ञा(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                जर (r == -ENOPKG)
                        परत log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                जर (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        परत r;
        }

        जर (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ पूर्णांक nsfd = -EBADF;
        स्थिरांक अक्षर *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(खोलो(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        जर (nsfd == -ENOENT) {
                r = proc_mounted();
                जर (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        परत log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                जर (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        परत log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        जर (nsfd < 0)
                परत nsfd;

        जर (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                जर (r < 0)
                        परत r;
        }

        परत TAKE_FD(nsfd);
}

पूर्णांक pidref_namespace_open_by_type(स्थिरांक PidRef *pidref, NamespaceType type) {
        परत pidref_namespace_open_by_type_internal(pidref, type, रिक्त);
}

पूर्णांक namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        पूर्णांक r;

        प्रतिज्ञा(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        जर (r < 0)
                परत r;

        परत pidref_namespace_open_by_type(&self, type);
}

पूर्णांक pidref_namespace_open(
                स्थिरांक PidRef *pidref,
                पूर्णांक *ret_pidns_fd,
                पूर्णांक *ret_mntns_fd,
                पूर्णांक *ret_netns_fd,
                पूर्णांक *ret_userns_fd,
                पूर्णांक *ret_मूळ_fd) {

        _cleanup_close_ पूर्णांक pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, मूळ_fd = -EBADF;
        bool need_verify = false;
        पूर्णांक r;

        प्रतिज्ञा(pidref_is_set(pidref));

        जर (pidref_is_remote(pidref))
                परत -EREMOTE;

        जर (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                जर (pidns_fd < 0)
                        परत pidns_fd;
        }

        जर (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                जर (mntns_fd < 0)
                        परत mntns_fd;
        }

        जर (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                जर (netns_fd < 0)
                        परत netns_fd;
        }

        जर (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                जर (userns_fd < 0 && userns_fd != -ENOPKG)
                        परत userns_fd;
        }

        जर (ret_मूळ_fd) {
                स्थिरांक अक्षर *मूळ;

                मूळ = procfs_file_alloca(pidref->pid, "root");
                मूळ_fd = RET_NERRNO(खोलो(मूळ, O_CLOEXEC|O_DIRECTORY));
                जर (मूळ_fd == -ENOENT && proc_mounted() == 0)
                        परत -ENOSYS;
                जर (मूळ_fd < 0)
                        परत मूळ_fd;

                need_verify = true;
        }

        जर (need_verify) {
                r = pidref_verify(pidref);
                जर (r < 0)
                        परत r;
        }

        जर (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        जर (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        जर (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        जर (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        जर (ret_मूळ_fd)
                *ret_मूळ_fd = TAKE_FD(मूळ_fd);

        परत 0;
}

पूर्णांक namespace_open(
                pid_t pid,
                पूर्णांक *ret_pidns_fd,
                पूर्णांक *ret_mntns_fd,
                पूर्णांक *ret_netns_fd,
                पूर्णांक *ret_userns_fd,
                पूर्णांक *ret_मूळ_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        पूर्णांक r;

        r = pidref_set_pid(&pidref, pid);
        जर (r < 0)
                परत r;

        परत pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_मूळ_fd);
}

पूर्णांक namespace_enter_full(
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूळ_fd,
                NamespaceEnterFlags flags) {

        पूर्णांक r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        जर (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                जर (r < 0)
                        परत r;
                जर (r > 0)
                        pidns_fd = -EBADF;
        }

        जर (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                जर (r < 0)
                        परत r;
                जर (r > 0)
                        mntns_fd = -EBADF;
        }

        जर (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                जर (r < 0)
                        परत r;
                जर (r > 0)
                        netns_fd = -EBADF;
        }

        जर (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                जर (r < 0)
                        परत r;
                जर (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        जर (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_मूळ_UNMAPPED)) {
                r = userns_has_मूळ_mapping(userns_fd);
                जर (r < 0)
                        परत r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        जर (r < 0)
                परत r;

        bool have_cap_sys_admin = r > 0;

        जर (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                जर (userns_fd >= 0) {
                        जर (setns(userns_fd, CLONE_NEWUSER) < 0)
                                परत -त्रुटि_सं;
                } अन्यथा जर (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || मूळ_fd >= 0)
                        परत log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        जर (pidns_fd >= 0)
                जर (setns(pidns_fd, CLONE_NEWPID) < 0)
                        परत -त्रुटि_सं;

        जर (mntns_fd >= 0)
                जर (setns(mntns_fd, CLONE_NEWNS) < 0)
                        परत -त्रुटि_सं;

        जर (netns_fd >= 0)
                जर (setns(netns_fd, CLONE_NEWNET) < 0)
                        परत -त्रुटि_सं;

        जर (userns_fd >= 0 && have_cap_sys_admin)
                जर (setns(userns_fd, CLONE_NEWUSER) < 0)
                        परत -त्रुटि_सं;

        जर (मूळ_fd >= 0) {
                जर (fchdir(मूळ_fd) < 0)
                        परत -त्रुटि_सं;

                जर (chroot(".") < 0)
                        परत -त्रुटि_सं;
        }

        जर (userns_fd < 0)
                परत 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ रिक्त);
        जर (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        जर (keep_uid_gid)
                परत 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        जर (setresgid(0, 0, 0) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to become root group in user namespace: %m");

        जर (setresuid(0, 0, 0) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to become root in user namespace: %m");

        परत 0;
}

पूर्णांक fd_is_namespace(पूर्णांक fd, NamespaceType type) {
        पूर्णांक r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        जर (r <= 0)
                परत r;

        जर (type < 0)
                परत true;

        पूर्णांक clone_flag = ioctl(fd, NS_GET_NSTYPE);
        जर (clone_flag < 0) {
                जर (त्रुटि_सं != ENOTTY)
                        परत -त्रुटि_सं;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ अक्षर *path = रिक्त;
                r = fd_get_path(fd, &path);
                जर (r < 0)
                        परत r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                जर (path_is_absolute(path))
                        परत -ENOTTY;

                स्थिरांक अक्षर *p = startswith(path, namespace_info[type].proc_name);
                जर (!p)
                        परत false;

                परत startswith(p, ":[") != रिक्त;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        जर (found_type < 0)
                परत -EBADF; /* Uh? Unknown namespace type? */

        परत found_type == type;
}

पूर्णांक is_our_namespace(पूर्णांक fd, NamespaceType type) {
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        जर (r < 0 && r != -ENOTTY)
                परत r;
        जर (r == 0) /* Not a namespace or not of the right type? */
                परत -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ पूर्णांक our_ns = namespace_open_by_type(type);
        जर (our_ns < 0)
                परत our_ns;

        r = fd_inode_same(fd, our_ns);
        जर (r == 0 && !type_verified)
                परत -ENOTTY;

        परत r;
}

पूर्णांक are_our_namespaces(पूर्णांक pidns_fd, पूर्णांक mntns_fd, पूर्णांक netns_fd, पूर्णांक userns_fd, पूर्णांक मूळ_fd) {
        पूर्णांक r;

        जर (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                जर (r <= 0)
                        परत r;
        }

        जर (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                जर (r <= 0)
                        परत r;
        }

        जर (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                जर (r <= 0)
                        परत r;
        }

        जर (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                जर (r <= 0)
                        परत r;
        }

        जर (मूळ_fd >= 0) {
                r = dir_fd_is_मूळ(मूळ_fd);
                जर (r <= 0)
                        परत r;
        }

        परत true;
}

पूर्णांक network_namespace_is_init(पूर्णांक socket_fd) {
        संरचना stat a, b;
        पूर्णांक r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        जर (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                जर (r < 0)
                        परत -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                परत r ? true : -ENOSYS;
        }
        जर (r < 0)
                परत r;

        जर (socket_fd >= 0) {
                _cleanup_close_ पूर्णांक netns = ioctl(socket_fd, SIOCGSKNS);
                जर (netns < 0)
                        परत -त्रुटि_सं;

                जर (ख_स्थिति(netns, &b) < 0)
                        परत -त्रुटि_सं;
        } अन्यथा {
                जर (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        परत -त्रुटि_सं;
        }

        परत stat_inode_same(&a, &b);
}

पूर्णांक namespace_is_init(NamespaceType type) {
        पूर्णांक r;

        प्रतिज्ञा(type >= 0);
        प्रतिज्ञा(type < _NAMESPACE_TYPE_MAX);

        जर (type == NAMESPACE_NET)
                परत network_namespace_is_init(/* socket_fd= */ -EBADF);

        जर (namespace_info[type].मूळ_inode == 0)
                परत -EBADR; /* Cannot answer this question */

        स्थिरांक अक्षर *p = pid_namespace_path(0, type);

        संरचना stat st;
        r = RET_NERRNO(stat(p, &st));
        जर (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                जर (r < 0)
                        परत -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                परत r ? true : -ENOSYS;
        }
        जर (r < 0)
                परत r;

        परत st.st_ino == namespace_info[type].मूळ_inode;
}

पूर्णांक pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ पूर्णांक ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        प्रतिज्ञा(!pid1 || pidref_is_set(pid1));
        प्रतिज्ञा(!pid2 || pidref_is_set(pid2));
        प्रतिज्ञा(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        जर (pidref_equal(pid1, pid2))
                परत true;

        जर (!pid1)
                ns1 = namespace_open_by_type(type);
        अन्यथा
                ns1 = pidref_namespace_open_by_type(pid1, type);
        जर (ns1 < 0)
                परत ns1;

        जर (!pid2)
                ns2 = namespace_open_by_type(type);
        अन्यथा
                ns2 = pidref_namespace_open_by_type(pid2, type);
        जर (ns2 < 0)
                परत ns2;

        परत fd_inode_same(ns1, ns2);
}

पूर्णांक in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        प्रतिज्ञा(pid1 >= 0);
        प्रतिज्ञा(pid2 >= 0);
        परत pidref_in_same_namespace(pid1 == 0 ? रिक्त : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? रिक्त : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

पूर्णांक namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        पूर्णांक r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        प्रतिज्ञा(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        प्रतिज्ञा(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        साठी (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                जर (r < 0)
                        परत r;

                r = pidref_in_same_namespace(c, &parent, type);
                जर (r < 0)
                        परत r;
                जर (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        जर (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        अन्यथा {
                                r = pidref_copy(c, ret);
                                जर (r < 0)
                                        परत r;
                        }

                        परत 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

पूर्णांक detach_mount_namespace(शून्य) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        जर (unshare(CLONE_NEWNS) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to acquire mount namespace: %m");

        जर (mount(रिक्त, "/", रिक्त, MS_SLAVE | MS_REC, रिक्त) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        जर (mount(रिक्त, "/", रिक्त, MS_SHARED | MS_REC, रिक्त) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        परत 0;
}

पूर्णांक detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        पूर्णांक r;

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

        जर (!uid_is_valid(target_uid))
                परत -EINVAL;
        जर (!gid_is_valid(target_gid))
                परत -EINVAL;

        r = detach_mount_namespace();
        जर (r != -EPERM)
                परत r;

        from_uid = getuid();
        from_gid = getgid();

        जर (unshare(CLONE_NEWUSER) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to write gid map: %m");

        परत detach_mount_namespace();
}

पूर्णांक detach_mount_namespace_userns(पूर्णांक userns_fd) {
        पूर्णांक r;

        प्रतिज्ञा(userns_fd >= 0);

        जर (setns(userns_fd, CLONE_NEWUSER) < 0)
                परत log_debug_errno(त्रुटि_सं, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        जर (r < 0)
                परत log_debug_errno(r, "Failed to become root in user namespace: %m");

        परत detach_mount_namespace();
}

पूर्णांक parse_userns_uid_range(स्थिरांक अक्षर *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ अक्षर *buffer = रिक्त;
        स्थिरांक अक्षर *range, *shift;
        पूर्णांक r;
        uid_t uid_shift, uid_range = 65536;

        प्रतिज्ञा(s);

        range = म_अक्षर(s, ':');
        जर (range) {
                buffer = strndup(s, range - s);
                जर (!buffer)
                        परत log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                जर (r < 0)
                        परत log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } अन्यथा
                shift = s;

        r = parse_uid(shift, &uid_shift);
        जर (r < 0)
                परत log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        जर (!userns_shift_range_valid(uid_shift, uid_range))
                परत log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        जर (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        जर (ret_uid_range)
                *ret_uid_range = uid_range;

        परत 0;
}

पूर्णांक userns_acquire_empty(शून्य) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        पूर्णांक r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        जर (r < 0)
                परत r;
        प्रतिज्ञा(r > 0);

        परत pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

पूर्णांक userns_acquire(स्थिरांक अक्षर *uid_map, स्थिरांक अक्षर *gid_map, bool setgroups_deny) {
        अक्षर path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        पूर्णांक r;

        प्रतिज्ञा(uid_map);
        प्रतिज्ञा(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        जर (r < 0)
                परत r;
        प्रतिज्ञा(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to write UID map: %m");

        जर (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                जर (r < 0)
                        परत log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to write GID map: %m");

        परत pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

पूर्णांक userns_acquire_self_मूळ(शून्य) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ अक्षर *uid_map = रिक्त, *gid_map = रिक्त;
        जर (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                परत -ENOMEM;
        जर (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                परत -ENOMEM;

        परत userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

पूर्णांक userns_enter_and_pin(पूर्णांक userns_fd, PidRef *ret) {
        _cleanup_close_pair_ पूर्णांक pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        अक्षर x;
        पूर्णांक r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(ret);

        जर (pipe2(pfd, O_CLOEXEC) < 0)
                परत -त्रुटि_सं;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ रिक्त,
                        (पूर्णांक[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        जर (r < 0)
                परत r;
        जर (r == 0) {
                /* Child. */

                जर (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(त्रुटि_सं, "Failed to join userns: %m");
                        _exit(निकास_त्रुटि);
                }

                userns_fd = safe_close(userns_fd);

                n = लिख(pfd[1], &(स्थिरांक अक्षर) { 'x' }, 1);
                जर (n < 0) {
                        log_debug_errno(त्रुटि_सं, "Failed to write to pipe: %m");
                        _exit(निकास_त्रुटि);
                }
                प्रतिज्ञा(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = पठ(pfd[0], &x, 1);
        जर (n < 0)
                परत -त्रुटि_सं;
        जर (n == 0)
                परत -EPROTO;
        प्रतिज्ञा(n == 1);
        प्रतिज्ञा(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        परत 0;
}

bool userns_supported(शून्य) {
        परत access("/proc/self/uid_map", F_OK) >= 0;
}

पूर्णांक userns_get_base_uid(पूर्णांक userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        पूर्णांक r;

        प्रतिज्ञा(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        जर (r < 0)
                परत r;

        uid_t uid;
        r = uid_map_शोध_मूळ(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        जर (r < 0)
                परत r;

        gid_t gid;
        r = uid_map_शोध_मूळ(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        जर (r < 0)
                परत r;

        जर (!ret_gid && uid != gid)
                परत -EUCLEAN;

        जर (ret_uid)
                *ret_uid = uid;
        जर (ret_gid)
                *ret_gid = gid;

        परत 0;
}

पूर्णांक userns_has_मूळ_mapping(पूर्णांक userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = रिक्त, *gid_range = रिक्त;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        पूर्णांक r;

        प्रतिज्ञा(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        जर (r < 0)
                परत r;

        स्थिरांक अक्षर *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        जर (r < 0)
                परत r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        जर (r < 0)
                परत r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        जर (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                परत log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        परत uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

पूर्णांक process_is_owned_by_uid(स्थिरांक PidRef *pidref, uid_t uid) {
        पूर्णांक r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        प्रतिज्ञा(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        जर (process_uid == uid)
                परत true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ पूर्णांक userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        जर (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                परत false;
        जर (userns_fd < 0)
                परत log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        साठी (अचिन्हित iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                जर (r < 0)
                        परत log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                जर (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        परत false;
                }

                जर (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        परत log_debug_errno(त्रुटि_सं, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                जर (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        परत true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                जर (iteration > 16)
                        परत log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ पूर्णांक parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                जर (parent_fd < 0) {
                        जर (त्रुटि_सं == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                परत false;
                        }

                        परत log_debug_errno(त्रुटि_सं, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

पूर्णांक namespace_open_by_id(uint64_t ns_id) {
        पूर्णांक r;

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

        जर (ns_id == 0)
                परत -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        जर (r < 0)
                परत r;
        जर (r == 0)
                परत -EPERM;

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

        संघ {
                संरचना file_handle file_handle;
                uint8_t space[दुरत्व(संरचना file_handle, f_handle) + आकार(संरचना nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = आकार(संरचना nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        परत RET_NERRNO(open_by_handle_at(FD_NSFS_मूळ, &fh.file_handle, O_PATH|O_CLOEXEC));
}

पूर्णांक is_idmapping_supported(स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(path);

        जर (!mount_new_api_supported())
                परत false;

        userns_fd = r = userns_acquire_self_मूळ();
        जर (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                परत false;
        जर (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                परत false;
        }
        जर (r < 0)
                परत log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(खोलो(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        जर (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                परत false;
        जर (r < 0)
                परत log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        जर (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                परत false;
        जर (r < 0)
                परत log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(संरचना mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, आकार(संरचना mount_attr)));
        जर (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                परत false;
        जर (r < 0)
                परत log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        परत true;
}

पूर्णांक netns_acquire(शून्य) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        पूर्णांक r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        जर (r < 0)
                परत log_debug_errno(r, "Failed to fork process into new netns: %m");
        प्रतिज्ञा(r > 0);

        परत pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
