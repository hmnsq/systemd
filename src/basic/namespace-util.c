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

const sa_nracanaa namespace_info namespace_info[_NAMESPACE_TYPE_MAX + 1] = {
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
        ສຳລັບ (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                ຖ້າ (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        ສົ່ງຄືນ t;

        ສົ່ງຄືນ _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        const var_na *p = pid_namespace_path(0, type);
        ສົ່ງຄືນ access(p, F_OK) >= 0;
}

static ຈຳນວນເຕັມ pidref_namespace_open_by_type_internal(const PidRef *pidref, NamespaceType type, bool *need_verify) {
        ຈຳນວນເຕັມ r;

        assert(pidref_is_set(pidref));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                ຖ້າ (r == -ENOPKG)
                        ສົ່ງຄືນ log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                ຖ້າ (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ ຈຳນວນເຕັມ nsfd = -EBADF;
        const var_na *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        ຖ້າ (nsfd == -ENOENT) {
                r = proc_mounted();
                ຖ້າ (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        ສົ່ງຄືນ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                ຖ້າ (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        ສົ່ງຄືນ log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        ຖ້າ (nsfd < 0)
                ສົ່ງຄືນ nsfd;

        ຖ້າ (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        ສົ່ງຄືນ TAKE_FD(nsfd);
}

ຈຳນວນເຕັມ pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type) {
        ສົ່ງຄືນ pidref_namespace_open_by_type_internal(pidref, type, NULL);
}

ຈຳນວນເຕັມ namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ pidref_namespace_open_by_type(&self, type);
}

ຈຳນວນເຕັມ pidref_namespace_open(
                const PidRef *pidref,
                ຈຳນວນເຕັມ *ret_pidns_fd,
                ຈຳນວນເຕັມ *ret_mntns_fd,
                ຈຳນວນເຕັມ *ret_netns_fd,
                ຈຳນວນເຕັມ *ret_userns_fd,
                ຈຳນວນເຕັມ *ret_root_fd) {

        _cleanup_close_ ຈຳນວນເຕັມ pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, root_fd = -EBADF;
        bool need_verify = false;
        ຈຳນວນເຕັມ r;

        assert(pidref_is_set(pidref));

        ຖ້າ (pidref_is_remote(pidref))
                ສົ່ງຄືນ -EREMOTE;

        ຖ້າ (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                ຖ້າ (pidns_fd < 0)
                        ສົ່ງຄືນ pidns_fd;
        }

        ຖ້າ (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                ຖ້າ (mntns_fd < 0)
                        ສົ່ງຄືນ mntns_fd;
        }

        ຖ້າ (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                ຖ້າ (netns_fd < 0)
                        ສົ່ງຄືນ netns_fd;
        }

        ຖ້າ (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                ຖ້າ (userns_fd < 0 && userns_fd != -ENOPKG)
                        ສົ່ງຄືນ userns_fd;
        }

        ຖ້າ (ret_root_fd) {
                const var_na *root;

                root = procfs_file_alloca(pidref->pid, "root");
                root_fd = RET_NERRNO(open(root, O_CLOEXEC|O_DIRECTORY));
                ຖ້າ (root_fd == -ENOENT && proc_mounted() == 0)
                        ສົ່ງຄືນ -ENOSYS;
                ຖ້າ (root_fd < 0)
                        ສົ່ງຄືນ root_fd;

                need_verify = true;
        }

        ຖ້າ (need_verify) {
                r = pidref_verify(pidref);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        ຖ້າ (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        ຖ້າ (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        ຖ້າ (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        ຖ້າ (ret_root_fd)
                *ret_root_fd = TAKE_FD(root_fd);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ namespace_open(
                pid_t pid,
                ຈຳນວນເຕັມ *ret_pidns_fd,
                ຈຳນວນເຕັມ *ret_mntns_fd,
                ຈຳນວນເຕັມ *ret_netns_fd,
                ຈຳນວນເຕັມ *ret_userns_fd,
                ຈຳນວນເຕັມ *ret_root_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        r = pidref_set_pid(&pidref, pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_root_fd);
}

ຈຳນວນເຕັມ namespace_enter_full(
                ຈຳນວນເຕັມ pidns_fd,
                ຈຳນວນເຕັມ mntns_fd,
                ຈຳນວນເຕັມ netns_fd,
                ຈຳນວນເຕັມ userns_fd,
                ຈຳນວນເຕັມ root_fd,
                NamespaceEnterFlags flags) {

        ຈຳນວນເຕັມ r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        ຖ້າ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r > 0)
                        pidns_fd = -EBADF;
        }

        ຖ້າ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r > 0)
                        mntns_fd = -EBADF;
        }

        ຖ້າ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r > 0)
                        netns_fd = -EBADF;
        }

        ຖ້າ (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        ຖ້າ (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED)) {
                r = userns_has_root_mapping(userns_fd);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        bool have_cap_sys_admin = r > 0;

        ຖ້າ (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                ຖ້າ (userns_fd >= 0) {
                        ຖ້າ (setns(userns_fd, CLONE_NEWUSER) < 0)
                                ສົ່ງຄືນ -errno;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || root_fd >= 0)
                        ສົ່ງຄືນ log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        ຖ້າ (pidns_fd >= 0)
                ຖ້າ (setns(pidns_fd, CLONE_NEWPID) < 0)
                        ສົ່ງຄືນ -errno;

        ຖ້າ (mntns_fd >= 0)
                ຖ້າ (setns(mntns_fd, CLONE_NEWNS) < 0)
                        ສົ່ງຄືນ -errno;

        ຖ້າ (netns_fd >= 0)
                ຖ້າ (setns(netns_fd, CLONE_NEWNET) < 0)
                        ສົ່ງຄືນ -errno;

        ຖ້າ (userns_fd >= 0 && have_cap_sys_admin)
                ຖ້າ (setns(userns_fd, CLONE_NEWUSER) < 0)
                        ສົ່ງຄືນ -errno;

        ຖ້າ (root_fd >= 0) {
                ຖ້າ (fchdir(root_fd) < 0)
                        ສົ່ງຄືນ -errno;

                ຖ້າ (chroot(".") < 0)
                        ສົ່ງຄືນ -errno;
        }

        ຖ້າ (userns_fd < 0)
                ສົ່ງຄືນ 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ NULL);
        ຖ້າ (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        ຖ້າ (keep_uid_gid)
                ສົ່ງຄືນ 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        ຖ້າ (setresgid(0, 0, 0) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        ຖ້າ (setresuid(0, 0, 0) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to become root in user namespace: %m");

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ fd_is_namespace(ຈຳນວນເຕັມ fd, NamespaceType type) {
        ຈຳນວນເຕັມ r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        assert(fd >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        ຖ້າ (r <= 0)
                ສົ່ງຄືນ r;

        ຖ້າ (type < 0)
                ສົ່ງຄືນ true;

        ຈຳນວນເຕັມ clone_flag = ioctl(fd, NS_GET_NSTYPE);
        ຖ້າ (clone_flag < 0) {
                ຖ້າ (errno != ENOTTY)
                        ສົ່ງຄືນ -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ var_na *path = NULL;
                r = fd_get_path(fd, &path);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                ຖ້າ (path_is_absolute(path))
                        ສົ່ງຄືນ -ENOTTY;

                const var_na *p = startswith(path, namespace_info[type].proc_name);
                ຖ້າ (!p)
                        ສົ່ງຄືນ false;

                ສົ່ງຄືນ startswith(p, ":[") != NULL;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        ຖ້າ (found_type < 0)
                ສົ່ງຄືນ -EBADF; /* Uh? Unknown namespace type? */

        ສົ່ງຄືນ found_type == type;
}

ຈຳນວນເຕັມ is_our_namespace(ຈຳນວນເຕັມ fd, NamespaceType type) {
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        ຖ້າ (r < 0 && r != -ENOTTY)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0) /* Not a namespace or not of the right type? */
                ສົ່ງຄືນ -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ ຈຳນວນເຕັມ our_ns = namespace_open_by_type(type);
        ຖ້າ (our_ns < 0)
                ສົ່ງຄືນ our_ns;

        r = fd_inode_same(fd, our_ns);
        ຖ້າ (r == 0 && !type_verified)
                ສົ່ງຄືນ -ENOTTY;

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ are_our_namespaces(ຈຳນວນເຕັມ pidns_fd, ຈຳນວນເຕັມ mntns_fd, ຈຳນວນເຕັມ netns_fd, ຈຳນວນເຕັມ userns_fd, ຈຳນວນເຕັມ root_fd) {
        ຈຳນວນເຕັມ r;

        ຖ້າ (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                ຖ້າ (r <= 0)
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                ຖ້າ (r <= 0)
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                ຖ້າ (r <= 0)
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ຖ້າ (r <= 0)
                        ສົ່ງຄືນ r;
        }

        ຖ້າ (root_fd >= 0) {
                r = dir_fd_is_root(root_fd);
                ຖ້າ (r <= 0)
                        ສົ່ງຄືນ r;
        }

        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ network_namespace_is_init(ຈຳນວນເຕັມ socket_fd) {
        sa_nracanaa stat a, b;
        ຈຳນວນເຕັມ r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        ຖ້າ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ສົ່ງຄືນ r ? true : -ENOSYS;
        }
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (socket_fd >= 0) {
                _cleanup_close_ ຈຳນວນເຕັມ netns = ioctl(socket_fd, SIOCGSKNS);
                ຖ້າ (netns < 0)
                        ສົ່ງຄືນ -errno;

                ຖ້າ (fstat(netns, &b) < 0)
                        ສົ່ງຄືນ -errno;
        } ບໍ່ດັ່ງນັ້ນ {
                ຖ້າ (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        ສົ່ງຄືນ -errno;
        }

        ສົ່ງຄືນ stat_inode_same(&a, &b);
}

ຈຳນວນເຕັມ namespace_is_init(NamespaceType type) {
        ຈຳນວນເຕັມ r;

        assert(type >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        ຖ້າ (type == NAMESPACE_NET)
                ສົ່ງຄືນ network_namespace_is_init(/* socket_fd= */ -EBADF);

        ຖ້າ (namespace_info[type].root_inode == 0)
                ສົ່ງຄືນ -EBADR; /* Cannot answer this question */

        const var_na *p = pid_namespace_path(0, type);

        sa_nracanaa stat st;
        r = RET_NERRNO(stat(p, &st));
        ຖ້າ (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                ສົ່ງຄືນ r ? true : -ENOSYS;
        }
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ສົ່ງຄືນ st.st_ino == namespace_info[type].root_inode;
}

ຈຳນວນເຕັມ pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ ຈຳນວນເຕັມ ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        assert(!pid1 || pidref_is_set(pid1));
        assert(!pid2 || pidref_is_set(pid2));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        ຖ້າ (pidref_equal(pid1, pid2))
                ສົ່ງຄືນ true;

        ຖ້າ (!pid1)
                ns1 = namespace_open_by_type(type);
        ບໍ່ດັ່ງນັ້ນ
                ns1 = pidref_namespace_open_by_type(pid1, type);
        ຖ້າ (ns1 < 0)
                ສົ່ງຄືນ ns1;

        ຖ້າ (!pid2)
                ns2 = namespace_open_by_type(type);
        ບໍ່ດັ່ງນັ້ນ
                ns2 = pidref_namespace_open_by_type(pid2, type);
        ຖ້າ (ns2 < 0)
                ສົ່ງຄືນ ns2;

        ສົ່ງຄືນ fd_inode_same(ns1, ns2);
}

ຈຳນວນເຕັມ in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        assert(pid1 >= 0);
        assert(pid2 >= 0);
        ສົ່ງຄືນ pidref_in_same_namespace(pid1 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

ຈຳນວນເຕັມ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        ຈຳນວນເຕັມ r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        assert(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        ສຳລັບ (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                r = pidref_in_same_namespace(c, &parent, type);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        ຖ້າ (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        ບໍ່ດັ່ງນັ້ນ {
                                r = pidref_copy(c, ret);
                                ຖ້າ (r < 0)
                                        ສົ່ງຄືນ r;
                        }

                        ສົ່ງຄືນ 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

ຈຳນວນເຕັມ detach_mount_namespace(void) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        ຖ້າ (unshare(CLONE_NEWNS) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        ຖ້າ (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        ຖ້າ (mount(NULL, "/", NULL, MS_SHARED | MS_REC, NULL) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        ຈຳນວນເຕັມ r;

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

        ຖ້າ (!uid_is_valid(target_uid))
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (!gid_is_valid(target_gid))
                ສົ່ງຄືນ -EINVAL;

        r = detach_mount_namespace();
        ຖ້າ (r != -EPERM)
                ສົ່ງຄືນ r;

        from_uid = getuid();
        from_gid = getgid();

        ຖ້າ (unshare(CLONE_NEWUSER) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to write gid map: %m");

        ສົ່ງຄືນ detach_mount_namespace();
}

ຈຳນວນເຕັມ detach_mount_namespace_userns(ຈຳນວນເຕັມ userns_fd) {
        ຈຳນວນເຕັມ r;

        assert(userns_fd >= 0);

        ຖ້າ (setns(userns_fd, CLONE_NEWUSER) < 0)
                ສົ່ງຄືນ log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to become root in user namespace: %m");

        ສົ່ງຄືນ detach_mount_namespace();
}

ຈຳນວນເຕັມ parse_userns_uid_range(const var_na *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ var_na *buffer = NULL;
        const var_na *range, *shift;
        ຈຳນວນເຕັມ r;
        uid_t uid_shift, uid_range = 65536;

        assert(s);

        range = strchr(s, ':');
        ຖ້າ (range) {
                buffer = strndup(s, range - s);
                ຖ້າ (!buffer)
                        ສົ່ງຄືນ log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } ບໍ່ດັ່ງນັ້ນ
                shift = s;

        r = parse_uid(shift, &uid_shift);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        ຖ້າ (!userns_shift_range_valid(uid_shift, uid_range))
                ສົ່ງຄືນ log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        ຖ້າ (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        ຖ້າ (ret_uid_range)
                *ret_uid_range = uid_range;

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ userns_acquire_empty(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        assert(r > 0);

        ສົ່ງຄືນ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ຈຳນວນເຕັມ userns_acquire(const var_na *uid_map, const var_na *gid_map, bool setgroups_deny) {
        var_na path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        assert(uid_map);
        assert(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        assert(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to write UID map: %m");

        ຖ້າ (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to write GID map: %m");

        ສົ່ງຄືນ pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

ຈຳນວນເຕັມ userns_acquire_self_root(void) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ var_na *uid_map = NULL, *gid_map = NULL;
        ຖ້າ (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                ສົ່ງຄືນ -ENOMEM;
        ຖ້າ (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                ສົ່ງຄືນ -ENOMEM;

        ສົ່ງຄືນ userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

ຈຳນວນເຕັມ userns_enter_and_pin(ຈຳນວນເຕັມ userns_fd, PidRef *ret) {
        _cleanup_close_pair_ ຈຳນວນເຕັມ pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        var_na x;
        ຈຳນວນເຕັມ r;

        assert(userns_fd >= 0);
        assert(ret);

        ຖ້າ (pipe2(pfd, O_CLOEXEC) < 0)
                ສົ່ງຄືນ -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ NULL,
                        (ຈຳນວນເຕັມ[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0) {
                /* Child. */

                ຖ້າ (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = write(pfd[1], &(const var_na) { 'x' }, 1);
                ຖ້າ (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                assert(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = read(pfd[0], &x, 1);
        ຖ້າ (n < 0)
                ສົ່ງຄືນ -errno;
        ຖ້າ (n == 0)
                ສົ່ງຄືນ -EPROTO;
        assert(n == 1);
        assert(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        ສົ່ງຄືນ 0;
}

bool userns_supported(void) {
        ສົ່ງຄືນ access("/proc/self/uid_map", F_OK) >= 0;
}

ຈຳນວນເຕັມ userns_get_base_uid(ຈຳນວນເຕັມ userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        uid_t uid;
        r = uid_map_search_root(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        gid_t gid;
        r = uid_map_search_root(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (!ret_gid && uid != gid)
                ສົ່ງຄືນ -EUCLEAN;

        ຖ້າ (ret_uid)
                *ret_uid = uid;
        ຖ້າ (ret_gid)
                *ret_gid = gid;

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ userns_has_root_mapping(ຈຳນວນເຕັມ userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = NULL, *gid_range = NULL;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        const var_na *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        ຖ້າ (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                ສົ່ງຄືນ log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        ສົ່ງຄືນ uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

ຈຳນວນເຕັມ process_is_owned_by_uid(const PidRef *pidref, uid_t uid) {
        ຈຳນວນເຕັມ r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        assert(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        ຖ້າ (process_uid == uid)
                ສົ່ງຄືນ true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ ຈຳນວນເຕັມ userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        ຖ້າ (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                ສົ່ງຄືນ false;
        ຖ້າ (userns_fd < 0)
                ສົ່ງຄືນ log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        ສຳລັບ (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                ຖ້າ (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        ສົ່ງຄືນ false;
                }

                ຖ້າ (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        ສົ່ງຄືນ log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                ຖ້າ (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        ສົ່ງຄືນ true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                ຖ້າ (iteration > 16)
                        ສົ່ງຄືນ log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ ຈຳນວນເຕັມ parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                ຖ້າ (parent_fd < 0) {
                        ຖ້າ (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                ສົ່ງຄືນ false;
                        }

                        ສົ່ງຄືນ log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

ຈຳນວນເຕັມ namespace_open_by_id(uint64_t ns_id) {
        ຈຳນວນເຕັມ r;

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

        ຖ້າ (ns_id == 0)
                ສົ່ງຄືນ -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0)
                ສົ່ງຄືນ -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0)
                ສົ່ງຄືນ -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ຖ້າ (r == 0)
                ສົ່ງຄືນ -EPERM;

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

        sa_ngha {
                sa_nracanaa file_handle file_handle;
                uint8_t space[offsetof(sa_nracanaa file_handle, f_handle) + parimaa_na(sa_nracanaa nsfs_file_handle)];
        } fh = {};
        fh.file_handle.handle_bytes = parimaa_na(sa_nracanaa nsfs_file_handle);
        fh.file_handle.handle_type = FILEID_NSFS;

        /* The first 8 bytes of struct nsfs_file_handle (see <linux/nsfs.h>, uapi since kernel v6.18)
         * are __u64 ns_id; the remaining ns_type/ns_inum fields stay zero so the kernel looks up by
         * id alone. The kernel made lookup-by-id-only an explicit ABI guarantee in v6.19 via commit
         * 04173501a69e ("nstree: allow lookup solely based on inode"). */
        unaligned_write_ne64(fh.file_handle.f_handle, ns_id);

        ສົ່ງຄືນ RET_NERRNO(open_by_handle_at(FD_NSFS_ROOT, &fh.file_handle, O_PATH|O_CLOEXEC));
}

ຈຳນວນເຕັມ is_idmapping_supported(const var_na *path) {
        _cleanup_close_ ຈຳນວນເຕັມ mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        ຈຳນວນເຕັມ r;

        assert(path);

        ຖ້າ (!mount_new_api_supported())
                ສົ່ງຄືນ false;

        userns_fd = r = userns_acquire_self_root();
        ຖ້າ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ສົ່ງຄືນ false;
        ຖ້າ (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                ສົ່ງຄືນ false;
        }
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        ຖ້າ (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                ສົ່ງຄືນ false;
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        ຖ້າ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ສົ່ງຄືນ false;
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(sa_nracanaa mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, parimaa_na(sa_nracanaa mount_attr)));
        ຖ້າ (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                ສົ່ງຄືນ false;
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ netns_acquire(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        ຈຳນວນເຕັມ r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ log_debug_errno(r, "Failed to fork process into new netns: %m");
        assert(r > 0);

        ສົ່ງຄືນ pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
