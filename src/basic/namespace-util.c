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
        ために (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                もし (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        戻る t;

        戻る _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        const char *p = pid_namespace_path(0, type);
        戻る access(p, F_OK) >= 0;
}

static 整数 pidref_namespace_open_by_type_internal(const PidRef *pidref, NamespaceType type, bool *need_verify) {
        整数 r;

        assert(pidref_is_set(pidref));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        もし (pidref_is_remote(pidref))
                戻る -EREMOTE;

        もし (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                もし (r == -ENOPKG)
                        戻る log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                もし (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        戻る r;
        }

        もし (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ 整数 nsfd = -EBADF;
        const char *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        もし (nsfd == -ENOENT) {
                r = proc_mounted();
                もし (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        戻る log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                もし (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        戻る log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        もし (nsfd < 0)
                戻る nsfd;

        もし (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                もし (r < 0)
                        戻る r;
        }

        戻る TAKE_FD(nsfd);
}

整数 pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type) {
        戻る pidref_namespace_open_by_type_internal(pidref, type, NULL);
}

整数 namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        整数 r;

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        もし (r < 0)
                戻る r;

        戻る pidref_namespace_open_by_type(&self, type);
}

整数 pidref_namespace_open(
                const PidRef *pidref,
                整数 *ret_pidns_fd,
                整数 *ret_mntns_fd,
                整数 *ret_netns_fd,
                整数 *ret_userns_fd,
                整数 *ret_root_fd) {

        _cleanup_close_ 整数 pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, root_fd = -EBADF;
        bool need_verify = false;
        整数 r;

        assert(pidref_is_set(pidref));

        もし (pidref_is_remote(pidref))
                戻る -EREMOTE;

        もし (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                もし (pidns_fd < 0)
                        戻る pidns_fd;
        }

        もし (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                もし (mntns_fd < 0)
                        戻る mntns_fd;
        }

        もし (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                もし (netns_fd < 0)
                        戻る netns_fd;
        }

        もし (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                もし (userns_fd < 0 && userns_fd != -ENOPKG)
                        戻る userns_fd;
        }

        もし (ret_root_fd) {
                const char *root;

                root = procfs_file_alloca(pidref->pid, "root");
                root_fd = RET_NERRNO(open(root, O_CLOEXEC|O_DIRECTORY));
                もし (root_fd == -ENOENT && proc_mounted() == 0)
                        戻る -ENOSYS;
                もし (root_fd < 0)
                        戻る root_fd;

                need_verify = true;
        }

        もし (need_verify) {
                r = pidref_verify(pidref);
                もし (r < 0)
                        戻る r;
        }

        もし (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        もし (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        もし (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        もし (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        もし (ret_root_fd)
                *ret_root_fd = TAKE_FD(root_fd);

        戻る 0;
}

整数 namespace_open(
                pid_t pid,
                整数 *ret_pidns_fd,
                整数 *ret_mntns_fd,
                整数 *ret_netns_fd,
                整数 *ret_userns_fd,
                整数 *ret_root_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        整数 r;

        r = pidref_set_pid(&pidref, pid);
        もし (r < 0)
                戻る r;

        戻る pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_root_fd);
}

整数 namespace_enter_full(
                整数 pidns_fd,
                整数 mntns_fd,
                整数 netns_fd,
                整数 userns_fd,
                整数 root_fd,
                NamespaceEnterFlags flags) {

        整数 r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        もし (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                もし (r < 0)
                        戻る r;
                もし (r > 0)
                        pidns_fd = -EBADF;
        }

        もし (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                もし (r < 0)
                        戻る r;
                もし (r > 0)
                        mntns_fd = -EBADF;
        }

        もし (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                もし (r < 0)
                        戻る r;
                もし (r > 0)
                        netns_fd = -EBADF;
        }

        もし (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                もし (r < 0)
                        戻る r;
                もし (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        もし (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED)) {
                r = userns_has_root_mapping(userns_fd);
                もし (r < 0)
                        戻る r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        もし (r < 0)
                戻る r;

        bool have_cap_sys_admin = r > 0;

        もし (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                もし (userns_fd >= 0) {
                        もし (setns(userns_fd, CLONE_NEWUSER) < 0)
                                戻る -errno;
                } 否则 もし (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || root_fd >= 0)
                        戻る log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        もし (pidns_fd >= 0)
                もし (setns(pidns_fd, CLONE_NEWPID) < 0)
                        戻る -errno;

        もし (mntns_fd >= 0)
                もし (setns(mntns_fd, CLONE_NEWNS) < 0)
                        戻る -errno;

        もし (netns_fd >= 0)
                もし (setns(netns_fd, CLONE_NEWNET) < 0)
                        戻る -errno;

        もし (userns_fd >= 0 && have_cap_sys_admin)
                もし (setns(userns_fd, CLONE_NEWUSER) < 0)
                        戻る -errno;

        もし (root_fd >= 0) {
                もし (fchdir(root_fd) < 0)
                        戻る -errno;

                もし (chroot(".") < 0)
                        戻る -errno;
        }

        もし (userns_fd < 0)
                戻る 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ NULL);
        もし (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        もし (keep_uid_gid)
                戻る 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        もし (setresgid(0, 0, 0) < 0)
                戻る log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        もし (setresuid(0, 0, 0) < 0)
                戻る log_debug_errno(errno, "Failed to become root in user namespace: %m");

        戻る 0;
}

整数 fd_is_namespace(整数 fd, NamespaceType type) {
        整数 r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        assert(fd >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        もし (r <= 0)
                戻る r;

        もし (type < 0)
                戻る true;

        整数 clone_flag = ioctl(fd, NS_GET_NSTYPE);
        もし (clone_flag < 0) {
                もし (errno != ENOTTY)
                        戻る -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ char *path = NULL;
                r = fd_get_path(fd, &path);
                もし (r < 0)
                        戻る r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                もし (path_is_absolute(path))
                        戻る -ENOTTY;

                const char *p = startswith(path, namespace_info[type].proc_name);
                もし (!p)
                        戻る false;

                戻る startswith(p, ":[") != NULL;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        もし (found_type < 0)
                戻る -EBADF; /* Uh? Unknown namespace type? */

        戻る found_type == type;
}

整数 is_our_namespace(整数 fd, NamespaceType type) {
        整数 r;

        assert(fd >= 0);
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        もし (r < 0 && r != -ENOTTY)
                戻る r;
        もし (r == 0) /* Not a namespace or not of the right type? */
                戻る -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ 整数 our_ns = namespace_open_by_type(type);
        もし (our_ns < 0)
                戻る our_ns;

        r = fd_inode_same(fd, our_ns);
        もし (r == 0 && !type_verified)
                戻る -ENOTTY;

        戻る r;
}

整数 are_our_namespaces(整数 pidns_fd, 整数 mntns_fd, 整数 netns_fd, 整数 userns_fd, 整数 root_fd) {
        整数 r;

        もし (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                もし (r <= 0)
                        戻る r;
        }

        もし (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                もし (r <= 0)
                        戻る r;
        }

        もし (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                もし (r <= 0)
                        戻る r;
        }

        もし (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                もし (r <= 0)
                        戻る r;
        }

        もし (root_fd >= 0) {
                r = dir_fd_is_root(root_fd);
                もし (r <= 0)
                        戻る r;
        }

        戻る true;
}

整数 network_namespace_is_init(整数 socket_fd) {
        struct stat a, b;
        整数 r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        もし (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                もし (r < 0)
                        戻る -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                戻る r ? true : -ENOSYS;
        }
        もし (r < 0)
                戻る r;

        もし (socket_fd >= 0) {
                _cleanup_close_ 整数 netns = ioctl(socket_fd, SIOCGSKNS);
                もし (netns < 0)
                        戻る -errno;

                もし (fstat(netns, &b) < 0)
                        戻る -errno;
        } 否则 {
                もし (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        戻る -errno;
        }

        戻る stat_inode_same(&a, &b);
}

整数 namespace_is_init(NamespaceType type) {
        整数 r;

        assert(type >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        もし (type == NAMESPACE_NET)
                戻る network_namespace_is_init(/* socket_fd= */ -EBADF);

        もし (namespace_info[type].root_inode == 0)
                戻る -EBADR; /* Cannot answer this question */

        const char *p = pid_namespace_path(0, type);

        struct stat st;
        r = RET_NERRNO(stat(p, &st));
        もし (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                もし (r < 0)
                        戻る -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                戻る r ? true : -ENOSYS;
        }
        もし (r < 0)
                戻る r;

        戻る st.st_ino == namespace_info[type].root_inode;
}

整数 pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ 整数 ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        assert(!pid1 || pidref_is_set(pid1));
        assert(!pid2 || pidref_is_set(pid2));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        もし (pidref_equal(pid1, pid2))
                戻る true;

        もし (!pid1)
                ns1 = namespace_open_by_type(type);
        否则
                ns1 = pidref_namespace_open_by_type(pid1, type);
        もし (ns1 < 0)
                戻る ns1;

        もし (!pid2)
                ns2 = namespace_open_by_type(type);
        否则
                ns2 = pidref_namespace_open_by_type(pid2, type);
        もし (ns2 < 0)
                戻る ns2;

        戻る fd_inode_same(ns1, ns2);
}

整数 in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        assert(pid1 >= 0);
        assert(pid2 >= 0);
        戻る pidref_in_same_namespace(pid1 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

整数 namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        整数 r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        assert(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        ために (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                もし (r < 0)
                        戻る r;

                r = pidref_in_same_namespace(c, &parent, type);
                もし (r < 0)
                        戻る r;
                もし (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        もし (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        否则 {
                                r = pidref_copy(c, ret);
                                もし (r < 0)
                                        戻る r;
                        }

                        戻る 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

整数 detach_mount_namespace(void) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        もし (unshare(CLONE_NEWNS) < 0)
                戻る log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        もし (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0)
                戻る log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        もし (mount(NULL, "/", NULL, MS_SHARED | MS_REC, NULL) < 0)
                戻る log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        戻る 0;
}

整数 detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        整数 r;

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

        もし (!uid_is_valid(target_uid))
                戻る -EINVAL;
        もし (!gid_is_valid(target_gid))
                戻る -EINVAL;

        r = detach_mount_namespace();
        もし (r != -EPERM)
                戻る r;

        from_uid = getuid();
        from_gid = getgid();

        もし (unshare(CLONE_NEWUSER) < 0)
                戻る log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to write gid map: %m");

        戻る detach_mount_namespace();
}

整数 detach_mount_namespace_userns(整数 userns_fd) {
        整数 r;

        assert(userns_fd >= 0);

        もし (setns(userns_fd, CLONE_NEWUSER) < 0)
                戻る log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to become root in user namespace: %m");

        戻る detach_mount_namespace();
}

整数 parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ char *buffer = NULL;
        const char *range, *shift;
        整数 r;
        uid_t uid_shift, uid_range = 65536;

        assert(s);

        range = strchr(s, ':');
        もし (range) {
                buffer = strndup(s, range - s);
                もし (!buffer)
                        戻る log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                もし (r < 0)
                        戻る log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } 否则
                shift = s;

        r = parse_uid(shift, &uid_shift);
        もし (r < 0)
                戻る log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        もし (!userns_shift_range_valid(uid_shift, uid_range))
                戻る log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        もし (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        もし (ret_uid_range)
                *ret_uid_range = uid_range;

        戻る 0;
}

整数 userns_acquire_empty(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        整数 r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        もし (r < 0)
                戻る r;
        assert(r > 0);

        戻る pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

整数 userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny) {
        char path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        整数 r;

        assert(uid_map);
        assert(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        もし (r < 0)
                戻る r;
        assert(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to write UID map: %m");

        もし (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                もし (r < 0)
                        戻る log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to write GID map: %m");

        戻る pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

整数 userns_acquire_self_root(void) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ char *uid_map = NULL, *gid_map = NULL;
        もし (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                戻る -ENOMEM;
        もし (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                戻る -ENOMEM;

        戻る userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

整数 userns_enter_and_pin(整数 userns_fd, PidRef *ret) {
        _cleanup_close_pair_ 整数 pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        char x;
        整数 r;

        assert(userns_fd >= 0);
        assert(ret);

        もし (pipe2(pfd, O_CLOEXEC) < 0)
                戻る -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ NULL,
                        (整数[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        もし (r < 0)
                戻る r;
        もし (r == 0) {
                /* Child. */

                もし (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = write(pfd[1], &(const char) { 'x' }, 1);
                もし (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                assert(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = read(pfd[0], &x, 1);
        もし (n < 0)
                戻る -errno;
        もし (n == 0)
                戻る -EPROTO;
        assert(n == 1);
        assert(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        戻る 0;
}

bool userns_supported(void) {
        戻る access("/proc/self/uid_map", F_OK) >= 0;
}

整数 userns_get_base_uid(整数 userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        整数 r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        もし (r < 0)
                戻る r;

        uid_t uid;
        r = uid_map_search_root(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        もし (r < 0)
                戻る r;

        gid_t gid;
        r = uid_map_search_root(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        もし (r < 0)
                戻る r;

        もし (!ret_gid && uid != gid)
                戻る -EUCLEAN;

        もし (ret_uid)
                *ret_uid = uid;
        もし (ret_gid)
                *ret_gid = gid;

        戻る 0;
}

整数 userns_has_root_mapping(整数 userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = NULL, *gid_range = NULL;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        整数 r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        もし (r < 0)
                戻る r;

        const char *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        もし (r < 0)
                戻る r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        もし (r < 0)
                戻る r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        もし (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                戻る log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        戻る uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

整数 process_is_owned_by_uid(const PidRef *pidref, uid_t uid) {
        整数 r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        assert(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        もし (process_uid == uid)
                戻る true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ 整数 userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        もし (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                戻る false;
        もし (userns_fd < 0)
                戻る log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        ために (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                もし (r < 0)
                        戻る log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                もし (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        戻る false;
                }

                もし (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        戻る log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                もし (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        戻る true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                もし (iteration > 16)
                        戻る log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ 整数 parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                もし (parent_fd < 0) {
                        もし (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                戻る false;
                        }

                        戻る log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

整数 namespace_open_by_id(uint64_t ns_id) {
        整数 r;

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

        もし (ns_id == 0)
                戻る -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        もし (r < 0)
                戻る r;
        もし (r == 0)
                戻る -EPERM;

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

        戻る RET_NERRNO(open_by_handle_at(FD_NSFS_ROOT, &fh.file_handle, O_PATH|O_CLOEXEC));
}

整数 is_idmapping_supported(const char *path) {
        _cleanup_close_ 整数 mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        整数 r;

        assert(path);

        もし (!mount_new_api_supported())
                戻る false;

        userns_fd = r = userns_acquire_self_root();
        もし (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                戻る false;
        もし (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                戻る false;
        }
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        もし (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                戻る false;
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        もし (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                戻る false;
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(struct mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, sizeof(struct mount_attr)));
        もし (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                戻る false;
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        戻る true;
}

整数 netns_acquire(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        整数 r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        もし (r < 0)
                戻る log_debug_errno(r, "Failed to fork process into new netns: %m");
        assert(r > 0);

        戻る pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
