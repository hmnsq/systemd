/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <linux/magic.h>
#அந்தர்பாவ <linux/nsfs.h>
#அந்தர்பாவ <linux/sockios.h>
#அந்தர்பாவ <sched.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/mount.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "capability-util.h"
#அந்தர்பாவ "dlfcn-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mountpoint-util.h"
#அந்தர்பாவ "namespace-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidfd-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "uid-range.h"
#அந்தர்பாவ "unaligned.h"
#அந்தர்பாவ "user-util.h"

மாறிலி கட்டமைப்பு namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        க்கு (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                எனில் (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        திரும்பு t;

        திரும்பு _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        ப்ரதிஜ்ஞா(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        மாறிலி எழுத்து *p = pid_namespace_path(0, type);
        திரும்பு access(p, F_OK) >= 0;
}

நிலையான முழு pidref_namespace_open_by_type_internal(மாறிலி PidRef *pidref, NamespaceType type, bool *need_verify) {
        முழு r;

        ப்ரதிஜ்ஞா(pidref_is_set(pidref));
        ப்ரதிஜ்ஞா(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                எனில் (r == -ENOPKG)
                        திரும்பு log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                எனில் (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        திரும்பு r;
        }

        எனில் (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ முழு nsfd = -EBADF;
        மாறிலி எழுத்து *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        எனில் (nsfd == -ENOENT) {
                r = proc_mounted();
                எனில் (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        திரும்பு log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                எனில் (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        திரும்பு log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        எனில் (nsfd < 0)
                திரும்பு nsfd;

        எனில் (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு TAKE_FD(nsfd);
}

முழு pidref_namespace_open_by_type(மாறிலி PidRef *pidref, NamespaceType type) {
        திரும்பு pidref_namespace_open_by_type_internal(pidref, type, வெற்று);
}

முழு namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        முழு r;

        ப்ரதிஜ்ஞா(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு pidref_namespace_open_by_type(&self, type);
}

முழு pidref_namespace_open(
                மாறிலி PidRef *pidref,
                முழு *ret_pidns_fd,
                முழு *ret_mntns_fd,
                முழு *ret_netns_fd,
                முழு *ret_userns_fd,
                முழு *ret_வேர்_fd) {

        _cleanup_close_ முழு pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, வேர்_fd = -EBADF;
        bool need_verify = false;
        முழு r;

        ப்ரதிஜ்ஞா(pidref_is_set(pidref));

        எனில் (pidref_is_remote(pidref))
                திரும்பு -EREMOTE;

        எனில் (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                எனில் (pidns_fd < 0)
                        திரும்பு pidns_fd;
        }

        எனில் (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                எனில் (mntns_fd < 0)
                        திரும்பு mntns_fd;
        }

        எனில் (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                எனில் (netns_fd < 0)
                        திரும்பு netns_fd;
        }

        எனில் (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                எனில் (userns_fd < 0 && userns_fd != -ENOPKG)
                        திரும்பு userns_fd;
        }

        எனில் (ret_வேர்_fd) {
                மாறிலி எழுத்து *வேர்;

                வேர் = procfs_file_alloca(pidref->pid, "root");
                வேர்_fd = RET_NERRNO(open(வேர், O_CLOEXEC|O_DIRECTORY));
                எனில் (வேர்_fd == -ENOENT && proc_mounted() == 0)
                        திரும்பு -ENOSYS;
                எனில் (வேர்_fd < 0)
                        திரும்பு வேர்_fd;

                need_verify = true;
        }

        எனில் (need_verify) {
                r = pidref_verify(pidref);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        எனில் (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        எனில் (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        எனில் (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        எனில் (ret_வேர்_fd)
                *ret_வேர்_fd = TAKE_FD(வேர்_fd);

        திரும்பு 0;
}

முழு namespace_open(
                pid_t pid,
                முழு *ret_pidns_fd,
                முழு *ret_mntns_fd,
                முழு *ret_netns_fd,
                முழு *ret_userns_fd,
                முழு *ret_வேர்_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        முழு r;

        r = pidref_set_pid(&pidref, pid);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_வேர்_fd);
}

முழு namespace_enter_full(
                முழு pidns_fd,
                முழு mntns_fd,
                முழு netns_fd,
                முழு userns_fd,
                முழு வேர்_fd,
                NamespaceEnterFlags flags) {

        முழு r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        எனில் (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r > 0)
                        pidns_fd = -EBADF;
        }

        எனில் (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r > 0)
                        mntns_fd = -EBADF;
        }

        எனில் (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r > 0)
                        netns_fd = -EBADF;
        }

        எனில் (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        எனில் (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_வேர்_UNMAPPED)) {
                r = userns_has_வேர்_mapping(userns_fd);
                எனில் (r < 0)
                        திரும்பு r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        எனில் (r < 0)
                திரும்பு r;

        bool have_cap_sys_admin = r > 0;

        எனில் (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                எனில் (userns_fd >= 0) {
                        எனில் (setns(userns_fd, CLONE_NEWUSER) < 0)
                                திரும்பு -errno;
                } இல்லையெனில் எனில் (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || வேர்_fd >= 0)
                        திரும்பு log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        எனில் (pidns_fd >= 0)
                எனில் (setns(pidns_fd, CLONE_NEWPID) < 0)
                        திரும்பு -errno;

        எனில் (mntns_fd >= 0)
                எனில் (setns(mntns_fd, CLONE_NEWNS) < 0)
                        திரும்பு -errno;

        எனில் (netns_fd >= 0)
                எனில் (setns(netns_fd, CLONE_NEWNET) < 0)
                        திரும்பு -errno;

        எனில் (userns_fd >= 0 && have_cap_sys_admin)
                எனில் (setns(userns_fd, CLONE_NEWUSER) < 0)
                        திரும்பு -errno;

        எனில் (வேர்_fd >= 0) {
                எனில் (fchdir(வேர்_fd) < 0)
                        திரும்பு -errno;

                எனில் (chroot(".") < 0)
                        திரும்பு -errno;
        }

        எனில் (userns_fd < 0)
                திரும்பு 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ வெற்று);
        எனில் (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        எனில் (keep_uid_gid)
                திரும்பு 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        எனில் (setresgid(0, 0, 0) < 0)
                திரும்பு log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        எனில் (setresuid(0, 0, 0) < 0)
                திரும்பு log_debug_errno(errno, "Failed to become root in user namespace: %m");

        திரும்பு 0;
}

முழு fd_is_namespace(முழு fd, NamespaceType type) {
        முழு r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        எனில் (r <= 0)
                திரும்பு r;

        எனில் (type < 0)
                திரும்பு true;

        முழு clone_flag = ioctl(fd, NS_GET_NSTYPE);
        எனில் (clone_flag < 0) {
                எனில் (errno != ENOTTY)
                        திரும்பு -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ எழுத்து *path = வெற்று;
                r = fd_get_path(fd, &path);
                எனில் (r < 0)
                        திரும்பு r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                எனில் (path_is_absolute(path))
                        திரும்பு -ENOTTY;

                மாறிலி எழுத்து *p = startswith(path, namespace_info[type].proc_name);
                எனில் (!p)
                        திரும்பு false;

                திரும்பு startswith(p, ":[") != வெற்று;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        எனில் (found_type < 0)
                திரும்பு -EBADF; /* Uh? Unknown namespace type? */

        திரும்பு found_type == type;
}

முழு is_our_namespace(முழு fd, NamespaceType type) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        எனில் (r < 0 && r != -ENOTTY)
                திரும்பு r;
        எனில் (r == 0) /* Not a namespace or not of the right type? */
                திரும்பு -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ முழு our_ns = namespace_open_by_type(type);
        எனில் (our_ns < 0)
                திரும்பு our_ns;

        r = fd_inode_same(fd, our_ns);
        எனில் (r == 0 && !type_verified)
                திரும்பு -ENOTTY;

        திரும்பு r;
}

முழு are_our_namespaces(முழு pidns_fd, முழு mntns_fd, முழு netns_fd, முழு userns_fd, முழு வேர்_fd) {
        முழு r;

        எனில் (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                எனில் (r <= 0)
                        திரும்பு r;
        }

        எனில் (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                எனில் (r <= 0)
                        திரும்பு r;
        }

        எனில் (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                எனில் (r <= 0)
                        திரும்பு r;
        }

        எனில் (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                எனில் (r <= 0)
                        திரும்பு r;
        }

        எனில் (வேர்_fd >= 0) {
                r = dir_fd_is_வேர்(வேர்_fd);
                எனில் (r <= 0)
                        திரும்பு r;
        }

        திரும்பு true;
}

முழு network_namespace_is_init(முழு socket_fd) {
        கட்டமைப்பு stat a, b;
        முழு r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        எனில் (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                எனில் (r < 0)
                        திரும்பு -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                திரும்பு r ? true : -ENOSYS;
        }
        எனில் (r < 0)
                திரும்பு r;

        எனில் (socket_fd >= 0) {
                _cleanup_close_ முழு netns = ioctl(socket_fd, SIOCGSKNS);
                எனில் (netns < 0)
                        திரும்பு -errno;

                எனில் (fstat(netns, &b) < 0)
                        திரும்பு -errno;
        } இல்லையெனில் {
                எனில் (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        திரும்பு -errno;
        }

        திரும்பு stat_inode_same(&a, &b);
}

முழு namespace_is_init(NamespaceType type) {
        முழு r;

        ப்ரதிஜ்ஞா(type >= 0);
        ப்ரதிஜ்ஞா(type < _NAMESPACE_TYPE_MAX);

        எனில் (type == NAMESPACE_NET)
                திரும்பு network_namespace_is_init(/* socket_fd= */ -EBADF);

        எனில் (namespace_info[type].வேர்_inode == 0)
                திரும்பு -EBADR; /* Cannot answer this question */

        மாறிலி எழுத்து *p = pid_namespace_path(0, type);

        கட்டமைப்பு stat st;
        r = RET_NERRNO(stat(p, &st));
        எனில் (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                எனில் (r < 0)
                        திரும்பு -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                திரும்பு r ? true : -ENOSYS;
        }
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு st.st_ino == namespace_info[type].வேர்_inode;
}

முழு pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ முழு ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        ப்ரதிஜ்ஞா(!pid1 || pidref_is_set(pid1));
        ப்ரதிஜ்ஞா(!pid2 || pidref_is_set(pid2));
        ப்ரதிஜ்ஞா(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        எனில் (pidref_equal(pid1, pid2))
                திரும்பு true;

        எனில் (!pid1)
                ns1 = namespace_open_by_type(type);
        இல்லையெனில்
                ns1 = pidref_namespace_open_by_type(pid1, type);
        எனில் (ns1 < 0)
                திரும்பு ns1;

        எனில் (!pid2)
                ns2 = namespace_open_by_type(type);
        இல்லையெனில்
                ns2 = pidref_namespace_open_by_type(pid2, type);
        எனில் (ns2 < 0)
                திரும்பு ns2;

        திரும்பு fd_inode_same(ns1, ns2);
}

முழு in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        ப்ரதிஜ்ஞா(pid1 >= 0);
        ப்ரதிஜ்ஞா(pid2 >= 0);
        திரும்பு pidref_in_same_namespace(pid1 == 0 ? வெற்று : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? வெற்று : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

முழு namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        முழு r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        ப்ரதிஜ்ஞா(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        ப்ரதிஜ்ஞா(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        க்கு (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                எனில் (r < 0)
                        திரும்பு r;

                r = pidref_in_same_namespace(c, &parent, type);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        எனில் (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        இல்லையெனில் {
                                r = pidref_copy(c, ret);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        திரும்பு 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

முழு detach_mount_namespace(வெறுமை) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        எனில் (unshare(CLONE_NEWNS) < 0)
                திரும்பு log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        எனில் (mount(வெற்று, "/", வெற்று, MS_SLAVE | MS_REC, வெற்று) < 0)
                திரும்பு log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        எனில் (mount(வெற்று, "/", வெற்று, MS_SHARED | MS_REC, வெற்று) < 0)
                திரும்பு log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        திரும்பு 0;
}

முழு detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        முழு r;

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

        எனில் (!uid_is_valid(target_uid))
                திரும்பு -EINVAL;
        எனில் (!gid_is_valid(target_gid))
                திரும்பு -EINVAL;

        r = detach_mount_namespace();
        எனில் (r != -EPERM)
                திரும்பு r;

        from_uid = getuid();
        from_gid = getgid();

        எனில் (unshare(CLONE_NEWUSER) < 0)
                திரும்பு log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to write gid map: %m");

        திரும்பு detach_mount_namespace();
}

முழு detach_mount_namespace_userns(முழு userns_fd) {
        முழு r;

        ப்ரதிஜ்ஞா(userns_fd >= 0);

        எனில் (setns(userns_fd, CLONE_NEWUSER) < 0)
                திரும்பு log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to become root in user namespace: %m");

        திரும்பு detach_mount_namespace();
}

முழு parse_userns_uid_range(மாறிலி எழுத்து *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ எழுத்து *buffer = வெற்று;
        மாறிலி எழுத்து *range, *shift;
        முழு r;
        uid_t uid_shift, uid_range = 65536;

        ப்ரதிஜ்ஞா(s);

        range = strchr(s, ':');
        எனில் (range) {
                buffer = strndup(s, range - s);
                எனில் (!buffer)
                        திரும்பு log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                எனில் (r < 0)
                        திரும்பு log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } இல்லையெனில்
                shift = s;

        r = parse_uid(shift, &uid_shift);
        எனில் (r < 0)
                திரும்பு log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        எனில் (!userns_shift_range_valid(uid_shift, uid_range))
                திரும்பு log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        எனில் (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        எனில் (ret_uid_range)
                *ret_uid_range = uid_range;

        திரும்பு 0;
}

முழு userns_acquire_empty(வெறுமை) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        முழு r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(r > 0);

        திரும்பு pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

முழு userns_acquire(மாறிலி எழுத்து *uid_map, மாறிலி எழுத்து *gid_map, bool setgroups_deny) {
        எழுத்து path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        முழு r;

        ப்ரதிஜ்ஞா(uid_map);
        ப்ரதிஜ்ஞா(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        எனில் (r < 0)
                திரும்பு r;
        ப்ரதிஜ்ஞா(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to write UID map: %m");

        எனில் (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to write GID map: %m");

        திரும்பு pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

முழு userns_acquire_self_வேர்(வெறுமை) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ எழுத்து *uid_map = வெற்று, *gid_map = வெற்று;
        எனில் (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                திரும்பு -ENOMEM;
        எனில் (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                திரும்பு -ENOMEM;

        திரும்பு userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

முழு userns_enter_and_pin(முழு userns_fd, PidRef *ret) {
        _cleanup_close_pair_ முழு pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        எழுத்து x;
        முழு r;

        ப்ரதிஜ்ஞா(userns_fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (pipe2(pfd, O_CLOEXEC) < 0)
                திரும்பு -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ வெற்று,
                        (முழு[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0) {
                /* Child. */

                எனில் (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = லிக(pfd[1], &(மாறிலி எழுத்து) { 'x' }, 1);
                எனில் (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                ப்ரதிஜ்ஞா(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = பட(pfd[0], &x, 1);
        எனில் (n < 0)
                திரும்பு -errno;
        எனில் (n == 0)
                திரும்பு -EPROTO;
        ப்ரதிஜ்ஞா(n == 1);
        ப்ரதிஜ்ஞா(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        திரும்பு 0;
}

bool userns_supported(வெறுமை) {
        திரும்பு access("/proc/self/uid_map", F_OK) >= 0;
}

முழு userns_get_base_uid(முழு userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        முழு r;

        ப்ரதிஜ்ஞா(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        எனில் (r < 0)
                திரும்பு r;

        uid_t uid;
        r = uid_map_தேடல்_வேர்(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        எனில் (r < 0)
                திரும்பு r;

        gid_t gid;
        r = uid_map_தேடல்_வேர்(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!ret_gid && uid != gid)
                திரும்பு -EUCLEAN;

        எனில் (ret_uid)
                *ret_uid = uid;
        எனில் (ret_gid)
                *ret_gid = gid;

        திரும்பு 0;
}

முழு userns_has_வேர்_mapping(முழு userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = வெற்று, *gid_range = வெற்று;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        முழு r;

        ப்ரதிஜ்ஞா(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        எனில் (r < 0)
                திரும்பு r;

        மாறிலி எழுத்து *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        எனில் (r < 0)
                திரும்பு r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        எனில் (r < 0)
                திரும்பு r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        எனில் (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        திரும்பு uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

முழு process_is_owned_by_uid(மாறிலி PidRef *pidref, uid_t uid) {
        முழு r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        ப்ரதிஜ்ஞா(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        எனில் (process_uid == uid)
                திரும்பு true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ முழு userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        எனில் (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                திரும்பு false;
        எனில் (userns_fd < 0)
                திரும்பு log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        க்கு (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                எனில் (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        திரும்பு false;
                }

                எனில் (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        திரும்பு log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                எனில் (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        திரும்பு true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                எனில் (iteration > 16)
                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ முழு parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                எனில் (parent_fd < 0) {
                        எனில் (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                திரும்பு false;
                        }

                        திரும்பு log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

முழு namespace_open_by_id(uint64_t ns_id) {
        முழு r;

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

        எனில் (ns_id == 0)
                திரும்பு -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EPERM;

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

        ஒன்றிப்பு {
                கட்டமைப்பு file_handle file_handle;
                uint8_t space[offsetof(கட்டமைப்பு file_handle, f_handle) + அளவு(கட்டமைப்பு nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = அளவு(கட்டமைப்பு nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        திரும்பு RET_NERRNO(open_by_handle_at(FD_NSFS_வேர், &fh.file_handle, O_PATH|O_CLOEXEC));
}

முழு is_idmapping_supported(மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        எனில் (!mount_new_api_supported())
                திரும்பு false;

        userns_fd = r = userns_acquire_self_வேர்();
        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                திரும்பு false;
        எனில் (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                திரும்பு false;
        }
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(கட்டமைப்பு mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, அளவு(கட்டமைப்பு mount_attr)));
        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        திரும்பு true;
}

முழு netns_acquire(வெறுமை) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        முழு r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to fork process into new netns: %m");
        ப்ரதிஜ்ஞா(r > 0);

        திரும்பு pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
