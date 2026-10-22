/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <linux/magic.h>
#include <linux/nsfs.h>
#include <linux/sockios.h>
#include <sched.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <unistd.h>

#include "capability-util.h"
#include "dlfcn-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "log.h"
#include "mountpoint-util.h"
#include "namespace-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidfd-util.h"
#include "pidref.h"
#include "process-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"
#include "uid-range.h"
#include "unaligned.h"
#include "user-util.h"

const struct namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        для (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                якщо (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        повернути t;

        повернути _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        const char *p = pid_namespace_path(0, type);
        повернути access(p, F_OK) >= 0;
}

static ціле pidref_namespace_open_by_type_internal(const PidRef *pidref, NamespaceType type, bool *need_verify) {
        ціле r;

        assert(pidref_is_set(pidref));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                якщо (r == -ENOPKG)
                        повернути log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                якщо (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        повернути r;
        }

        якщо (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ ціле nsfd = -EBADF;
        const char *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        якщо (nsfd == -ENOENT) {
                r = proc_mounted();
                якщо (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        повернути log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                якщо (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        повернути log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        якщо (nsfd < 0)
                повернути nsfd;

        якщо (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                якщо (r < 0)
                        повернути r;
        }

        повернути TAKE_FD(nsfd);
}

ціле pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type) {
        повернути pidref_namespace_open_by_type_internal(pidref, type, NULL);
}

ціле namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        ціле r;

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        якщо (r < 0)
                повернути r;

        повернути pidref_namespace_open_by_type(&self, type);
}

ціле pidref_namespace_open(
                const PidRef *pidref,
                ціле *ret_pidns_fd,
                ціле *ret_mntns_fd,
                ціле *ret_netns_fd,
                ціле *ret_userns_fd,
                ціле *ret_root_fd) {

        _cleanup_close_ ціле pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, root_fd = -EBADF;
        bool need_verify = false;
        ціле r;

        assert(pidref_is_set(pidref));

        якщо (pidref_is_remote(pidref))
                повернути -EREMOTE;

        якщо (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                якщо (pidns_fd < 0)
                        повернути pidns_fd;
        }

        якщо (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                якщо (mntns_fd < 0)
                        повернути mntns_fd;
        }

        якщо (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                якщо (netns_fd < 0)
                        повернути netns_fd;
        }

        якщо (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                якщо (userns_fd < 0 && userns_fd != -ENOPKG)
                        повернути userns_fd;
        }

        якщо (ret_root_fd) {
                const char *root;

                root = procfs_file_alloca(pidref->pid, "root");
                root_fd = RET_NERRNO(open(root, O_CLOEXEC|O_DIRECTORY));
                якщо (root_fd == -ENOENT && proc_mounted() == 0)
                        повернути -ENOSYS;
                якщо (root_fd < 0)
                        повернути root_fd;

                need_verify = true;
        }

        якщо (need_verify) {
                r = pidref_verify(pidref);
                якщо (r < 0)
                        повернути r;
        }

        якщо (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        якщо (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        якщо (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        якщо (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        якщо (ret_root_fd)
                *ret_root_fd = TAKE_FD(root_fd);

        повернути 0;
}

ціле namespace_open(
                pid_t pid,
                ціле *ret_pidns_fd,
                ціле *ret_mntns_fd,
                ціле *ret_netns_fd,
                ціле *ret_userns_fd,
                ціле *ret_root_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        ціле r;

        r = pidref_set_pid(&pidref, pid);
        якщо (r < 0)
                повернути r;

        повернути pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_root_fd);
}

ціле namespace_enter_full(
                ціле pidns_fd,
                ціле mntns_fd,
                ціле netns_fd,
                ціле userns_fd,
                ціле root_fd,
                NamespaceEnterFlags flags) {

        ціле r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        якщо (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                якщо (r < 0)
                        повернути r;
                якщо (r > 0)
                        pidns_fd = -EBADF;
        }

        якщо (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                якщо (r < 0)
                        повернути r;
                якщо (r > 0)
                        mntns_fd = -EBADF;
        }

        якщо (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                якщо (r < 0)
                        повернути r;
                якщо (r > 0)
                        netns_fd = -EBADF;
        }

        якщо (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                якщо (r < 0)
                        повернути r;
                якщо (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        якщо (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED)) {
                r = userns_has_root_mapping(userns_fd);
                якщо (r < 0)
                        повернути r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        якщо (r < 0)
                повернути r;

        bool have_cap_sys_admin = r > 0;

        якщо (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                якщо (userns_fd >= 0) {
                        якщо (setns(userns_fd, CLONE_NEWUSER) < 0)
                                повернути -errno;
                } інакше якщо (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || root_fd >= 0)
                        повернути log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        якщо (pidns_fd >= 0)
                якщо (setns(pidns_fd, CLONE_NEWPID) < 0)
                        повернути -errno;

        якщо (mntns_fd >= 0)
                якщо (setns(mntns_fd, CLONE_NEWNS) < 0)
                        повернути -errno;

        якщо (netns_fd >= 0)
                якщо (setns(netns_fd, CLONE_NEWNET) < 0)
                        повернути -errno;

        якщо (userns_fd >= 0 && have_cap_sys_admin)
                якщо (setns(userns_fd, CLONE_NEWUSER) < 0)
                        повернути -errno;

        якщо (root_fd >= 0) {
                якщо (fchdir(root_fd) < 0)
                        повернути -errno;

                якщо (chroot(".") < 0)
                        повернути -errno;
        }

        якщо (userns_fd < 0)
                повернути 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ NULL);
        якщо (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        якщо (keep_uid_gid)
                повернути 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        якщо (setresgid(0, 0, 0) < 0)
                повернути log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        якщо (setresuid(0, 0, 0) < 0)
                повернути log_debug_errno(errno, "Failed to become root in user namespace: %m");

        повернути 0;
}

ціле fd_is_namespace(ціле fd, NamespaceType type) {
        ціле r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        assert(fd >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        якщо (r <= 0)
                повернути r;

        якщо (type < 0)
                повернути true;

        ціле clone_flag = ioctl(fd, NS_GET_NSTYPE);
        якщо (clone_flag < 0) {
                якщо (errno != ENOTTY)
                        повернути -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ char *path = NULL;
                r = fd_get_path(fd, &path);
                якщо (r < 0)
                        повернути r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                якщо (path_is_absolute(path))
                        повернути -ENOTTY;

                const char *p = startswith(path, namespace_info[type].proc_name);
                якщо (!p)
                        повернути false;

                повернути startswith(p, ":[") != NULL;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        якщо (found_type < 0)
                повернути -EBADF; /* Uh? Unknown namespace type? */

        повернути found_type == type;
}

ціле is_our_namespace(ціле fd, NamespaceType type) {
        ціле r;

        assert(fd >= 0);
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        якщо (r < 0 && r != -ENOTTY)
                повернути r;
        якщо (r == 0) /* Not a namespace or not of the right type? */
                повернути -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ ціле our_ns = namespace_open_by_type(type);
        якщо (our_ns < 0)
                повернути our_ns;

        r = fd_inode_same(fd, our_ns);
        якщо (r == 0 && !type_verified)
                повернути -ENOTTY;

        повернути r;
}

ціле are_our_namespaces(ціле pidns_fd, ціле mntns_fd, ціле netns_fd, ціле userns_fd, ціле root_fd) {
        ціле r;

        якщо (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                якщо (r <= 0)
                        повернути r;
        }

        якщо (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                якщо (r <= 0)
                        повернути r;
        }

        якщо (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                якщо (r <= 0)
                        повернути r;
        }

        якщо (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                якщо (r <= 0)
                        повернути r;
        }

        якщо (root_fd >= 0) {
                r = dir_fd_is_root(root_fd);
                якщо (r <= 0)
                        повернути r;
        }

        повернути true;
}

ціле network_namespace_is_init(ціле socket_fd) {
        struct stat a, b;
        ціле r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        якщо (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                якщо (r < 0)
                        повернути -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                повернути r ? true : -ENOSYS;
        }
        якщо (r < 0)
                повернути r;

        якщо (socket_fd >= 0) {
                _cleanup_close_ ціле netns = ioctl(socket_fd, SIOCGSKNS);
                якщо (netns < 0)
                        повернути -errno;

                якщо (fstat(netns, &b) < 0)
                        повернути -errno;
        } інакше {
                якщо (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        повернути -errno;
        }

        повернути stat_inode_same(&a, &b);
}

ціле namespace_is_init(NamespaceType type) {
        ціле r;

        assert(type >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        якщо (type == NAMESPACE_NET)
                повернути network_namespace_is_init(/* socket_fd= */ -EBADF);

        якщо (namespace_info[type].root_inode == 0)
                повернути -EBADR; /* Cannot answer this question */

        const char *p = pid_namespace_path(0, type);

        struct stat st;
        r = RET_NERRNO(stat(p, &st));
        якщо (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                якщо (r < 0)
                        повернути -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                повернути r ? true : -ENOSYS;
        }
        якщо (r < 0)
                повернути r;

        повернути st.st_ino == namespace_info[type].root_inode;
}

ціле pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ ціле ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        assert(!pid1 || pidref_is_set(pid1));
        assert(!pid2 || pidref_is_set(pid2));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        якщо (pidref_equal(pid1, pid2))
                повернути true;

        якщо (!pid1)
                ns1 = namespace_open_by_type(type);
        інакше
                ns1 = pidref_namespace_open_by_type(pid1, type);
        якщо (ns1 < 0)
                повернути ns1;

        якщо (!pid2)
                ns2 = namespace_open_by_type(type);
        інакше
                ns2 = pidref_namespace_open_by_type(pid2, type);
        якщо (ns2 < 0)
                повернути ns2;

        повернути fd_inode_same(ns1, ns2);
}

ціле in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        assert(pid1 >= 0);
        assert(pid2 >= 0);
        повернути pidref_in_same_namespace(pid1 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

ціле namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        ціле r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        assert(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        для (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                якщо (r < 0)
                        повернути r;

                r = pidref_in_same_namespace(c, &parent, type);
                якщо (r < 0)
                        повернути r;
                якщо (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        якщо (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        інакше {
                                r = pidref_copy(c, ret);
                                якщо (r < 0)
                                        повернути r;
                        }

                        повернути 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

ціле detach_mount_namespace(void) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        якщо (unshare(CLONE_NEWNS) < 0)
                повернути log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        якщо (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0)
                повернути log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        якщо (mount(NULL, "/", NULL, MS_SHARED | MS_REC, NULL) < 0)
                повернути log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        повернути 0;
}

ціле detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        ціле r;

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

        якщо (!uid_is_valid(target_uid))
                повернути -EINVAL;
        якщо (!gid_is_valid(target_gid))
                повернути -EINVAL;

        r = detach_mount_namespace();
        якщо (r != -EPERM)
                повернути r;

        from_uid = getuid();
        from_gid = getgid();

        якщо (unshare(CLONE_NEWUSER) < 0)
                повернути log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to write gid map: %m");

        повернути detach_mount_namespace();
}

ціле detach_mount_namespace_userns(ціле userns_fd) {
        ціле r;

        assert(userns_fd >= 0);

        якщо (setns(userns_fd, CLONE_NEWUSER) < 0)
                повернути log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to become root in user namespace: %m");

        повернути detach_mount_namespace();
}

ціле parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ char *buffer = NULL;
        const char *range, *shift;
        ціле r;
        uid_t uid_shift, uid_range = 65536;

        assert(s);

        range = strchr(s, ':');
        якщо (range) {
                buffer = strndup(s, range - s);
                якщо (!buffer)
                        повернути log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                якщо (r < 0)
                        повернути log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } інакше
                shift = s;

        r = parse_uid(shift, &uid_shift);
        якщо (r < 0)
                повернути log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        якщо (!userns_shift_range_valid(uid_shift, uid_range))
                повернути log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        якщо (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        якщо (ret_uid_range)
                *ret_uid_range = uid_range;

        повернути 0;
}

ціле userns_acquire_empty(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ціле r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        якщо (r < 0)
                повернути r;
        assert(r > 0);

        повернути pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ціле userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny) {
        char path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ціле r;

        assert(uid_map);
        assert(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        якщо (r < 0)
                повернути r;
        assert(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to write UID map: %m");

        якщо (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                якщо (r < 0)
                        повернути log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to write GID map: %m");

        повернути pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ціле userns_acquire_self_root(void) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ char *uid_map = NULL, *gid_map = NULL;
        якщо (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                повернути -ENOMEM;
        якщо (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                повернути -ENOMEM;

        повернути userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

ціле userns_enter_and_pin(ціле userns_fd, PidRef *ret) {
        _cleanup_close_pair_ ціле pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        char x;
        ціле r;

        assert(userns_fd >= 0);
        assert(ret);

        якщо (pipe2(pfd, O_CLOEXEC) < 0)
                повернути -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ NULL,
                        (ціле[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        якщо (r < 0)
                повернути r;
        якщо (r == 0) {
                /* Child. */

                якщо (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = write(pfd[1], &(const char) { 'x' }, 1);
                якщо (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                assert(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = read(pfd[0], &x, 1);
        якщо (n < 0)
                повернути -errno;
        якщо (n == 0)
                повернути -EPROTO;
        assert(n == 1);
        assert(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        повернути 0;
}

bool userns_supported(void) {
        повернути access("/proc/self/uid_map", F_OK) >= 0;
}

ціле userns_get_base_uid(ціле userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ціле r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        якщо (r < 0)
                повернути r;

        uid_t uid;
        r = uid_map_search_root(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        якщо (r < 0)
                повернути r;

        gid_t gid;
        r = uid_map_search_root(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        якщо (r < 0)
                повернути r;

        якщо (!ret_gid && uid != gid)
                повернути -EUCLEAN;

        якщо (ret_uid)
                *ret_uid = uid;
        якщо (ret_gid)
                *ret_gid = gid;

        повернути 0;
}

ціле userns_has_root_mapping(ціле userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = NULL, *gid_range = NULL;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ціле r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        якщо (r < 0)
                повернути r;

        const char *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        якщо (r < 0)
                повернути r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        якщо (r < 0)
                повернути r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        якщо (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                повернути log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        повернути uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

ціле process_is_owned_by_uid(const PidRef *pidref, uid_t uid) {
        ціле r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        assert(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        якщо (process_uid == uid)
                повернути true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ ціле userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        якщо (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                повернути false;
        якщо (userns_fd < 0)
                повернути log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        для (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                якщо (r < 0)
                        повернути log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                якщо (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        повернути false;
                }

                якщо (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        повернути log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                якщо (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        повернути true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                якщо (iteration > 16)
                        повернути log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ ціле parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                якщо (parent_fd < 0) {
                        якщо (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                повернути false;
                        }

                        повернути log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

ціле namespace_open_by_id(uint64_t ns_id) {
        ціле r;

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

        якщо (ns_id == 0)
                повернути -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        якщо (r < 0)
                повернути r;
        якщо (r == 0)
                повернути -EPERM;

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

        union {
                struct file_handle file_handle;
                uint8_t space[offsetof(struct file_handle, f_handle) + sizeof(struct nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = sizeof(struct nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        повернути RET_NERRNO(open_by_handle_at(FD_NSFS_ROOT, &fh.file_handle, O_PATH|O_CLOEXEC));
}

ціле is_idmapping_supported(const char *path) {
        _cleanup_close_ ціле mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        ціле r;

        assert(path);

        якщо (!mount_new_api_supported())
                повернути false;

        userns_fd = r = userns_acquire_self_root();
        якщо (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                повернути false;
        якщо (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                повернути false;
        }
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        якщо (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                повернути false;
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        якщо (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                повернути false;
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(struct mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, sizeof(struct mount_attr)));
        якщо (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                повернути false;
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        повернути true;
}

ціле netns_acquire(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ціле r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        якщо (r < 0)
                повернути log_debug_errno(r, "Failed to fork process into new netns: %m");
        assert(r > 0);

        повернути pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
