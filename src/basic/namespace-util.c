/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <linux/magic.h>
#ଅନ୍ତର୍ଭାଵ <linux/nsfs.h>
#ଅନ୍ତର୍ଭାଵ <linux/sockios.h>
#ଅନ୍ତର୍ଭାଵ <sched.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/mount.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "capability-util.h"
#ଅନ୍ତର୍ଭାଵ "dlfcn-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mountpoint-util.h"
#ଅନ୍ତର୍ଭାଵ "namespace-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidfd-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "uid-range.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

ଅଚଳ ସଂରଚନା namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        ପାଇଁ (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                ଯଦି (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        ଫେରନ୍ତୁ t;

        ଫେରନ୍ତୁ _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ଅଚଳ ଅକ୍ଷର *p = pid_namespace_path(0, type);
        ଫେରନ୍ତୁ access(p, F_OK) >= 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_namespace_open_by_type_internal(ଅଚଳ PidRef *pidref, NamespaceType type, bool *need_verify) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref_is_set(pidref));
        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                ଯଦି (r == -ENOPKG)
                        ଫେରନ୍ତୁ log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                ଯଦି (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nsfd = -EBADF;
        ଅଚଳ ଅକ୍ଷର *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        ଯଦି (nsfd == -ENOENT) {
                r = proc_mounted();
                ଯଦି (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        ଫେରନ୍ତୁ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                ଯଦି (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        ଫେରନ୍ତୁ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        ଯଦି (nsfd < 0)
                ଫେରନ୍ତୁ nsfd;

        ଯଦି (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ TAKE_FD(nsfd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_namespace_open_by_type(ଅଚଳ PidRef *pidref, NamespaceType type) {
        ଫେରନ୍ତୁ pidref_namespace_open_by_type_internal(pidref, type, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pidref_namespace_open_by_type(&self, type);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_namespace_open(
                ଅଚଳ PidRef *pidref,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_ମୂଳ_fd) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, ମୂଳ_fd = -EBADF;
        bool need_verify = false;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref_is_set(pidref));

        ଯଦି (pidref_is_remote(pidref))
                ଫେରନ୍ତୁ -EREMOTE;

        ଯଦି (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                ଯଦି (pidns_fd < 0)
                        ଫେରନ୍ତୁ pidns_fd;
        }

        ଯଦି (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                ଯଦି (mntns_fd < 0)
                        ଫେରନ୍ତୁ mntns_fd;
        }

        ଯଦି (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                ଯଦି (netns_fd < 0)
                        ଫେରନ୍ତୁ netns_fd;
        }

        ଯଦି (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                ଯଦି (userns_fd < 0 && userns_fd != -ENOPKG)
                        ଫେରନ୍ତୁ userns_fd;
        }

        ଯଦି (ret_ମୂଳ_fd) {
                ଅଚଳ ଅକ୍ଷର *ମୂଳ;

                ମୂଳ = procfs_file_alloca(pidref->pid, "root");
                ମୂଳ_fd = RET_NERRNO(open(ମୂଳ, O_CLOEXEC|O_DIRECTORY));
                ଯଦି (ମୂଳ_fd == -ENOENT && proc_mounted() == 0)
                        ଫେରନ୍ତୁ -ENOSYS;
                ଯଦି (ମୂଳ_fd < 0)
                        ଫେରନ୍ତୁ ମୂଳ_fd;

                need_verify = true;
        }

        ଯଦି (need_verify) {
                r = pidref_verify(pidref);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        ଯଦି (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        ଯଦି (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        ଯଦି (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        ଯଦି (ret_ମୂଳ_fd)
                *ret_ମୂଳ_fd = TAKE_FD(ମୂଳ_fd);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_open(
                pid_t pid,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_ମୂଳ_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = pidref_set_pid(&pidref, pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_ମୂଳ_fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_enter_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                NamespaceEnterFlags flags) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        ଯଦି (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        pidns_fd = -EBADF;
        }

        ଯଦି (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        mntns_fd = -EBADF;
        }

        ଯଦି (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        netns_fd = -EBADF;
        }

        ଯଦି (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        ଯଦି (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ମୂଳ_UNMAPPED)) {
                r = userns_has_ମୂଳ_mapping(userns_fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        bool have_cap_sys_admin = r > 0;

        ଯଦି (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                ଯଦି (userns_fd >= 0) {
                        ଯଦି (setns(userns_fd, CLONE_NEWUSER) < 0)
                                ଫେରନ୍ତୁ -errno;
                } ଅନ୍ୟଥା ଯଦି (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || ମୂଳ_fd >= 0)
                        ଫେରନ୍ତୁ log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        ଯଦି (pidns_fd >= 0)
                ଯଦି (setns(pidns_fd, CLONE_NEWPID) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (mntns_fd >= 0)
                ଯଦି (setns(mntns_fd, CLONE_NEWNS) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (netns_fd >= 0)
                ଯଦି (setns(netns_fd, CLONE_NEWNET) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (userns_fd >= 0 && have_cap_sys_admin)
                ଯଦି (setns(userns_fd, CLONE_NEWUSER) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (ମୂଳ_fd >= 0) {
                ଯଦି (fchdir(ମୂଳ_fd) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (chroot(".") < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଯଦି (userns_fd < 0)
                ଫେରନ୍ତୁ 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ ଖାଲି);
        ଯଦି (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        ଯଦି (keep_uid_gid)
                ଫେରନ୍ତୁ 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        ଯଦି (setresgid(0, 0, 0) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        ଯଦି (setresuid(0, 0, 0) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to become root in user namespace: %m");

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, NamespaceType type) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (type < 0)
                ଫେରନ୍ତୁ true;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା clone_flag = ioctl(fd, NS_GET_NSTYPE);
        ଯଦି (clone_flag < 0) {
                ଯଦି (errno != ENOTTY)
                        ଫେରନ୍ତୁ -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;
                r = fd_get_path(fd, &path);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                ଯଦି (path_is_absolute(path))
                        ଫେରନ୍ତୁ -ENOTTY;

                ଅଚଳ ଅକ୍ଷର *p = startswith(path, namespace_info[type].proc_name);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ false;

                ଫେରନ୍ତୁ startswith(p, ":[") != ଖାଲି;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        ଯଦି (found_type < 0)
                ଫେରନ୍ତୁ -EBADF; /* Uh? Unknown namespace type? */

        ଫେରନ୍ତୁ found_type == type;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_our_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, NamespaceType type) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        ଯଦି (r < 0 && r != -ENOTTY)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0) /* Not a namespace or not of the right type? */
                ଫେରନ୍ତୁ -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା our_ns = namespace_open_by_type(type);
        ଯଦି (our_ns < 0)
                ଫେରନ୍ତୁ our_ns;

        r = fd_inode_same(fd, our_ns);
        ଯଦି (r == 0 && !type_verified)
                ଫେରନ୍ତୁ -ENOTTY;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା are_our_namespaces(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ମୂଳ_fd >= 0) {
                r = dir_fd_is_ମୂଳ(ମୂଳ_fd);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା network_namespace_is_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_fd) {
        ସଂରଚନା stat a, b;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        ଯଦି (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ଫେରନ୍ତୁ r ? true : -ENOSYS;
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (socket_fd >= 0) {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns = ioctl(socket_fd, SIOCGSKNS);
                ଯଦି (netns < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (fstat(netns, &b) < 0)
                        ଫେରନ୍ତୁ -errno;
        } ଅନ୍ୟଥା {
                ଯଦି (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ stat_inode_same(&a, &b);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_is_init(NamespaceType type) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(type >= 0);
        ପ୍ରତିଜ୍ଞା(type < _NAMESPACE_TYPE_MAX);

        ଯଦି (type == NAMESPACE_NET)
                ଫେରନ୍ତୁ network_namespace_is_init(/* socket_fd= */ -EBADF);

        ଯଦି (namespace_info[type].ମୂଳ_inode == 0)
                ଫେରନ୍ତୁ -EBADR; /* Cannot answer this question */

        ଅଚଳ ଅକ୍ଷର *p = pid_namespace_path(0, type);

        ସଂରଚନା stat st;
        r = RET_NERRNO(stat(p, &st));
        ଯଦି (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ଫେରନ୍ତୁ r ? true : -ENOSYS;
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ st.st_ino == namespace_info[type].ମୂଳ_inode;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        ପ୍ରତିଜ୍ଞା(!pid1 || pidref_is_set(pid1));
        ପ୍ରତିଜ୍ଞା(!pid2 || pidref_is_set(pid2));
        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ଯଦି (pidref_equal(pid1, pid2))
                ଫେରନ୍ତୁ true;

        ଯଦି (!pid1)
                ns1 = namespace_open_by_type(type);
        ଅନ୍ୟଥା
                ns1 = pidref_namespace_open_by_type(pid1, type);
        ଯଦି (ns1 < 0)
                ଫେରନ୍ତୁ ns1;

        ଯଦି (!pid2)
                ns2 = namespace_open_by_type(type);
        ଅନ୍ୟଥା
                ns2 = pidref_namespace_open_by_type(pid2, type);
        ଯଦି (ns2 < 0)
                ଫେରନ୍ତୁ ns2;

        ଫେରନ୍ତୁ fd_inode_same(ns1, ns2);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        ପ୍ରତିଜ୍ଞା(pid1 >= 0);
        ପ୍ରତିଜ୍ଞା(pid2 >= 0);
        ଫେରନ୍ତୁ pidref_in_same_namespace(pid1 == 0 ? ଖାଲି : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? ଖାଲି : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        ପ୍ରତିଜ୍ଞା(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        ପ୍ରତିଜ୍ଞା(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        ପାଇଁ (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                r = pidref_in_same_namespace(c, &parent, type);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        ଯଦି (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        ଅନ୍ୟଥା {
                                r = pidref_copy(c, ret);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        ଫେରନ୍ତୁ 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା detach_mount_namespace(ଶୂନ୍ୟ) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        ଯଦି (unshare(CLONE_NEWNS) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        ଯଦି (mount(ଖାଲି, "/", ଖାଲି, MS_SLAVE | MS_REC, ଖାଲି) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        ଯଦି (mount(ଖାଲି, "/", ଖାଲି, MS_SHARED | MS_REC, ଖାଲି) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ଯଦି (!uid_is_valid(target_uid))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (!gid_is_valid(target_gid))
                ଫେରନ୍ତୁ -EINVAL;

        r = detach_mount_namespace();
        ଯଦି (r != -EPERM)
                ଫେରନ୍ତୁ r;

        from_uid = getuid();
        from_gid = getgid();

        ଯଦି (unshare(CLONE_NEWUSER) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write gid map: %m");

        ଫେରନ୍ତୁ detach_mount_namespace();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା detach_mount_namespace_userns(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(userns_fd >= 0);

        ଯଦି (setns(userns_fd, CLONE_NEWUSER) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to become root in user namespace: %m");

        ଫେରନ୍ତୁ detach_mount_namespace();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_userns_uid_range(ଅଚଳ ଅକ୍ଷର *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ ଅକ୍ଷର *buffer = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *range, *shift;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
        uid_t uid_shift, uid_range = 65536;

        ପ୍ରତିଜ୍ଞା(s);

        range = strchr(s, ':');
        ଯଦି (range) {
                buffer = strndup(s, range - s);
                ଯଦି (!buffer)
                        ଫେରନ୍ତୁ log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } ଅନ୍ୟଥା
                shift = s;

        r = parse_uid(shift, &uid_shift);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        ଯଦି (!userns_shift_range_valid(uid_shift, uid_range))
                ଫେରନ୍ତୁ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        ଯଦି (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        ଯଦି (ret_uid_range)
                *ret_uid_range = uid_range;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_acquire_empty(ଶୂନ୍ୟ) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(r > 0);

        ଫେରନ୍ତୁ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_acquire(ଅଚଳ ଅକ୍ଷର *uid_map, ଅଚଳ ଅକ୍ଷର *gid_map, bool setgroups_deny) {
        ଅକ୍ଷର path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(uid_map);
        ପ୍ରତିଜ୍ଞା(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ପ୍ରତିଜ୍ଞା(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write UID map: %m");

        ଯଦି (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to write GID map: %m");

        ଫେରନ୍ତୁ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_acquire_self_ମୂଳ(ଶୂନ୍ୟ) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ ଅକ୍ଷର *uid_map = ଖାଲି, *gid_map = ଖାଲି;
        ଯଦି (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                ଫେରନ୍ତୁ -ENOMEM;
        ଯଦି (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_enter_and_pin(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, PidRef *ret) {
        _cleanup_close_pair_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        ଅକ୍ଷର x;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(userns_fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (pipe2(pfd, O_CLOEXEC) < 0)
                ଫେରନ୍ତୁ -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ ଖାଲି,
                        (ପୂର୍ଣ୍ଣସଂଖ୍ୟା[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0) {
                /* Child. */

                ଯଦି (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = ଲିଖ(pfd[1], &(ଅଚଳ ଅକ୍ଷର) { 'x' }, 1);
                ଯଦି (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                ପ୍ରତିଜ୍ଞା(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = ପଠ(pfd[0], &x, 1);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ -errno;
        ଯଦି (n == 0)
                ଫେରନ୍ତୁ -EPROTO;
        ପ୍ରତିଜ୍ଞା(n == 1);
        ପ୍ରତିଜ୍ଞା(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        ଫେରନ୍ତୁ 0;
}

bool userns_supported(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ access("/proc/self/uid_map", F_OK) >= 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_get_base_uid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        uid_t uid;
        r = uid_map_ସନ୍ଧାନ_ମୂଳ(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        gid_t gid;
        r = uid_map_ସନ୍ଧାନ_ମୂଳ(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!ret_gid && uid != gid)
                ଫେରନ୍ତୁ -EUCLEAN;

        ଯଦି (ret_uid)
                *ret_uid = uid;
        ଯଦି (ret_gid)
                *ret_gid = gid;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_has_ମୂଳ_mapping(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = ଖାଲି, *gid_range = ଖାଲି;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଅଚଳ ଅକ୍ଷର *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        ଯଦି (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        ଫେରନ୍ତୁ uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା process_is_owned_by_uid(ଅଚଳ PidRef *pidref, uid_t uid) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        ପ୍ରତିଜ୍ଞା(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        ଯଦି (process_uid == uid)
                ଫେରନ୍ତୁ true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        ଯଦି (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                ଫେରନ୍ତୁ false;
        ଯଦି (userns_fd < 0)
                ଫେରନ୍ତୁ log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        ପାଇଁ (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                ଯଦି (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        ଫେରନ୍ତୁ false;
                }

                ଯଦି (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                ଯଦି (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        ଫେରନ୍ତୁ true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                ଯଦି (iteration > 16)
                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                ଯଦି (parent_fd < 0) {
                        ଯଦି (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                ଫେରନ୍ତୁ false;
                        }

                        ଫେରନ୍ତୁ log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_open_by_id(uint64_t ns_id) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ଯଦି (ns_id == 0)
                ଫେରନ୍ତୁ -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EPERM;

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

        ସଂଘ {
                ସଂରଚନା file_handle file_handle;
                uint8_t space[offsetof(ସଂରଚନା file_handle, f_handle) + ଆକାର(ସଂରଚନା nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = ଆକାର(ସଂରଚନା nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        ଫେରନ୍ତୁ RET_NERRNO(open_by_handle_at(FD_NSFS_ମୂଳ, &fh.file_handle, O_PATH|O_CLOEXEC));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_idmapping_supported(ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        ଯଦି (!mount_new_api_supported())
                ଫେରନ୍ତୁ false;

        userns_fd = r = userns_acquire_self_ମୂଳ();
        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ଫେରନ୍ତୁ false;
        ଯଦି (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                ଫେରନ୍ତୁ false;
        }
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(ସଂରଚନା mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, ଆକାର(ସଂରଚନା mount_attr)));
        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_acquire(ଶୂନ୍ୟ) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to fork process into new netns: %m");
        ପ୍ରତିଜ୍ଞା(r > 0);

        ଫେରନ୍ତୁ pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
