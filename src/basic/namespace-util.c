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
        cho (NamespaceType t = 0; t < _NAMESPACE_TYPE_MAX; t++)
                nếu (((namespace_info[t].clone_flag ^ clone_flag) & (CLONE_NEWCGROUP|CLONE_NEWIPC|CLONE_NEWNET|CLONE_NEWNS|CLONE_NEWPID|CLONE_NEWUSER|CLONE_NEWUTS|CLONE_NEWTIME)) == 0)
                        trả_về t;

        trả_về _NAMESPACE_TYPE_INVALID;
}

bool namespace_type_supported(NamespaceType type) {
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        const char *p = pid_namespace_path(0, type);
        trả_về access(p, F_OK) >= 0;
}

static số_nguyên pidref_namespace_open_by_type_internal(const PidRef *pidref, NamespaceType type, bool *need_verify) {
        số_nguyên r;

        assert(pidref_is_set(pidref));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (pidref->fd >= 0) {
                r = pidfd_get_namespace(pidref->fd, namespace_info[type].pidfd_get_ns_ioctl_cmd);
                nếu (r == -ENOPKG)
                        trả_về log_debug_errno(
                                        r,
                                        "Cannot open %s namespace for PID "PID_FMT" as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);
                nếu (!ERRNO_IS_NEG_NOT_SUPPORTED(r))
                        trả_về r;
        }

        nếu (need_verify) /* The caller shall call pidref_verify() later */
                *need_verify = true;

        _cleanup_close_ số_nguyên nsfd = -EBADF;
        const char *p;

        p = pid_namespace_path(pidref->pid, type);
        nsfd = RET_NERRNO(open(p, O_RDONLY|O_NOCTTY|O_CLOEXEC));
        nếu (nsfd == -ENOENT) {
                r = proc_mounted();
                nếu (r == 0)
                        /* /proc/ is not available or not set up properly, we're most likely in some chroot environment. */
                        trả_về log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOSYS),
                                        "Cannot open %s namespace for PID "PID_FMT" as /proc is not mounted",
                                        namespace_info[type].proc_name, pidref->pid);
                nếu (r > 0)
                        /* If /proc/ is definitely around then this means the namespace type is not supported */
                        trả_về log_debug_errno(
                                        SYNTHETIC_ERRNO(ENOPKG),
                                        "Cannot open %s namespace for PID "PID_FMT" via /proc as the namespace type is not supported by the kernel",
                                        namespace_info[type].proc_name, pidref->pid);

                /* can't determine? then propagate original error */
        }
        nếu (nsfd < 0)
                trả_về nsfd;

        nếu (!need_verify) { /* Otherwise we verify on our own */
                r = pidref_verify(pidref);
                nếu (r < 0)
                        trả_về r;
        }

        trả_về TAKE_FD(nsfd);
}

số_nguyên pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type) {
        trả_về pidref_namespace_open_by_type_internal(pidref, type, NULL);
}

số_nguyên namespace_open_by_type(NamespaceType type) {
        _cleanup_(pidref_done) PidRef self = PIDREF_NULL;
        số_nguyên r;

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = pidref_set_self(&self);
        nếu (r < 0)
                trả_về r;

        trả_về pidref_namespace_open_by_type(&self, type);
}

số_nguyên pidref_namespace_open(
                const PidRef *pidref,
                số_nguyên *ret_pidns_fd,
                số_nguyên *ret_mntns_fd,
                số_nguyên *ret_netns_fd,
                số_nguyên *ret_userns_fd,
                số_nguyên *ret_root_fd) {

        _cleanup_close_ số_nguyên pidns_fd = -EBADF, mntns_fd = -EBADF, netns_fd = -EBADF,
                userns_fd = -EBADF, root_fd = -EBADF;
        bool need_verify = false;
        số_nguyên r;

        assert(pidref_is_set(pidref));

        nếu (pidref_is_remote(pidref))
                trả_về -EREMOTE;

        nếu (ret_pidns_fd) {
                pidns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_PID, &need_verify);
                nếu (pidns_fd < 0)
                        trả_về pidns_fd;
        }

        nếu (ret_mntns_fd) {
                mntns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_MOUNT, &need_verify);
                nếu (mntns_fd < 0)
                        trả_về mntns_fd;
        }

        nếu (ret_netns_fd) {
                netns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_NET, &need_verify);
                nếu (netns_fd < 0)
                        trả_về netns_fd;
        }

        nếu (ret_userns_fd) {
                userns_fd = pidref_namespace_open_by_type_internal(pidref, NAMESPACE_USER, &need_verify);
                nếu (userns_fd < 0 && userns_fd != -ENOPKG)
                        trả_về userns_fd;
        }

        nếu (ret_root_fd) {
                const char *root;

                root = procfs_file_alloca(pidref->pid, "root");
                root_fd = RET_NERRNO(open(root, O_CLOEXEC|O_DIRECTORY));
                nếu (root_fd == -ENOENT && proc_mounted() == 0)
                        trả_về -ENOSYS;
                nếu (root_fd < 0)
                        trả_về root_fd;

                need_verify = true;
        }

        nếu (need_verify) {
                r = pidref_verify(pidref);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (ret_pidns_fd)
                *ret_pidns_fd = TAKE_FD(pidns_fd);

        nếu (ret_mntns_fd)
                *ret_mntns_fd = TAKE_FD(mntns_fd);

        nếu (ret_netns_fd)
                *ret_netns_fd = TAKE_FD(netns_fd);

        nếu (ret_userns_fd)
                *ret_userns_fd = TAKE_FD(userns_fd);

        nếu (ret_root_fd)
                *ret_root_fd = TAKE_FD(root_fd);

        trả_về 0;
}

số_nguyên namespace_open(
                pid_t pid,
                số_nguyên *ret_pidns_fd,
                số_nguyên *ret_mntns_fd,
                số_nguyên *ret_netns_fd,
                số_nguyên *ret_userns_fd,
                số_nguyên *ret_root_fd) {

        _cleanup_(pidref_done) PidRef pidref = PIDREF_NULL;
        số_nguyên r;

        r = pidref_set_pid(&pidref, pid);
        nếu (r < 0)
                trả_về r;

        trả_về pidref_namespace_open(&pidref, ret_pidns_fd, ret_mntns_fd, ret_netns_fd, ret_userns_fd, ret_root_fd);
}

số_nguyên namespace_enter_full(
                số_nguyên pidns_fd,
                số_nguyên mntns_fd,
                số_nguyên netns_fd,
                số_nguyên userns_fd,
                số_nguyên root_fd,
                NamespaceEnterFlags flags) {

        số_nguyên r;

        /* Block dlopen() now, to avoid us inadvertently loading shared library from another namespace */
        block_dlopen();

         /* Join namespaces, but only if we're not part of them already. This is important if we don't
          * necessarily own the namespace in question, as kernel would unconditionally return EPERM otherwise. */

        nếu (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                nếu (r < 0)
                        trả_về r;
                nếu (r > 0)
                        pidns_fd = -EBADF;
        }

        nếu (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                nếu (r < 0)
                        trả_về r;
                nếu (r > 0)
                        mntns_fd = -EBADF;
        }

        nếu (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                nếu (r < 0)
                        trả_về r;
                nếu (r > 0)
                        netns_fd = -EBADF;
        }

        nếu (userns_fd >= 0) {
                /* Can't setns to your own userns, since then you could escalate from non-root to root in
                 * your own namespace, so check if namespaces are equal before attempting to enter. */

                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                nếu (r < 0)
                        trả_về r;
                nếu (r > 0)
                        userns_fd = -EBADF;
        }

        /* Determine this before joining anything, as /proc might not be available afterwards. */
        bool keep_uid_gid = false;
        nếu (userns_fd >= 0 && FLAGS_SET(flags, NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED)) {
                r = userns_has_root_mapping(userns_fd);
                nếu (r < 0)
                        trả_về r;
                keep_uid_gid = r == 0;
        }

        r = have_effective_cap(CAP_SYS_ADMIN);
        nếu (r < 0)
                trả_về r;

        bool have_cap_sys_admin = r > 0;

        nếu (!have_cap_sys_admin) {
                /* If we don't have CAP_SYS_ADMIN in our own user namespace, our best bet is to enter the
                 * user namespace first (if we got one) to get CAP_SYS_ADMIN within the child user namespace,
                 * and then hope the other namespaces are owned by the child user namespace. If they aren't,
                 * we'll just get an EPERM later on when trying to setns() to them. If there's no namespace
                 * left to join, there's nothing to need privileges for. */

                nếu (userns_fd >= 0) {
                        nếu (setns(userns_fd, CLONE_NEWUSER) < 0)
                                trả_về -errno;
                } không_thì nếu (pidns_fd >= 0 || mntns_fd >= 0 || netns_fd >= 0 || root_fd >= 0)
                        trả_về log_debug_errno(
                                        SYNTHETIC_ERRNO(EPERM),
                                        "Need CAP_SYS_ADMIN or a child user namespace to enter namespaces.");
        }

        nếu (pidns_fd >= 0)
                nếu (setns(pidns_fd, CLONE_NEWPID) < 0)
                        trả_về -errno;

        nếu (mntns_fd >= 0)
                nếu (setns(mntns_fd, CLONE_NEWNS) < 0)
                        trả_về -errno;

        nếu (netns_fd >= 0)
                nếu (setns(netns_fd, CLONE_NEWNET) < 0)
                        trả_về -errno;

        nếu (userns_fd >= 0 && have_cap_sys_admin)
                nếu (setns(userns_fd, CLONE_NEWUSER) < 0)
                        trả_về -errno;

        nếu (root_fd >= 0) {
                nếu (fchdir(root_fd) < 0)
                        trả_về -errno;

                nếu (chroot(".") < 0)
                        trả_về -errno;
        }

        nếu (userns_fd < 0)
                trả_về 0;

        /* Drop supplementary groups where possible. setgroups() may be denied, either via
         * /proc/self/setgroups or by policy (e.g. a BPF LSM), in which case there's nothing we can do
         * about them. */
        r = maybe_setgroups(/* size= */ 0, /* list= */ NULL);
        nếu (r < 0)
                log_debug_errno(r, "Unable to drop supplementary groups, ignoring: %m");

        nếu (keep_uid_gid)
                trả_về 0;

        /* Joining a user namespace preserves the underlying UID/GID. Require becoming root to succeed, as
         * otherwise we'd continue running with our original identity inside the namespace. */
        nếu (setresgid(0, 0, 0) < 0)
                trả_về log_debug_errno(errno, "Failed to become root group in user namespace: %m");

        nếu (setresuid(0, 0, 0) < 0)
                trả_về log_debug_errno(errno, "Failed to become root in user namespace: %m");

        trả_về 0;
}

số_nguyên fd_is_namespace(số_nguyên fd, NamespaceType type) {
        số_nguyên r;

        /* Checks whether the specified file descriptor refers to a namespace (of type if type != _NAMESPACE_INVALID). */

        assert(fd >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        r = fd_is_fs_type(fd, NSFS_MAGIC);
        nếu (r <= 0)
                trả_về r;

        nếu (type < 0)
                trả_về true;

        số_nguyên clone_flag = ioctl(fd, NS_GET_NSTYPE);
        nếu (clone_flag < 0) {
                nếu (errno != ENOTTY)
                        trả_về -errno;

                // FIXME: This compatibility code path shall be removed once kernel 6.3
                //        becomes the new minimal baseline
                /* Before Linux 6.3, nsfs ioctls lack 32-bit compat support on 64-bit kernels.
                 * The nsfs link name also identifies the type, e.g. "mnt:[4026531841]". */
                _cleanup_free_ char *path = NULL;
                r = fd_get_path(fd, &path);
                nếu (r < 0)
                        trả_về r;

                /* Bind-mounted namespaces expose their mount path instead of the nsfs link name. */
                nếu (path_is_absolute(path))
                        trả_về -ENOTTY;

                const char *p = startswith(path, namespace_info[type].proc_name);
                nếu (!p)
                        trả_về false;

                trả_về startswith(p, ":[") != NULL;
        }

        NamespaceType found_type = clone_flag_to_namespace_type(clone_flag);
        nếu (found_type < 0)
                trả_về -EBADF; /* Uh? Unknown namespace type? */

        trả_về found_type == type;
}

số_nguyên is_our_namespace(số_nguyên fd, NamespaceType type) {
        số_nguyên r;

        assert(fd >= 0);
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        r = fd_is_namespace(fd, type);
        /* If neither the ioctl nor the nsfs link name identifies the type, we can still recognize our own
         * namespace by inode. */
        nếu (r < 0 && r != -ENOTTY)
                trả_về r;
        nếu (r == 0) /* Not a namespace or not of the right type? */
                trả_về -EUCLEAN;

        bool type_verified = r > 0;

        _cleanup_close_ số_nguyên our_ns = namespace_open_by_type(type);
        nếu (our_ns < 0)
                trả_về our_ns;

        r = fd_inode_same(fd, our_ns);
        nếu (r == 0 && !type_verified)
                trả_về -ENOTTY;

        trả_về r;
}

số_nguyên are_our_namespaces(số_nguyên pidns_fd, số_nguyên mntns_fd, số_nguyên netns_fd, số_nguyên userns_fd, số_nguyên root_fd) {
        số_nguyên r;

        nếu (pidns_fd >= 0) {
                r = is_our_namespace(pidns_fd, NAMESPACE_PID);
                nếu (r <= 0)
                        trả_về r;
        }

        nếu (mntns_fd >= 0) {
                r = is_our_namespace(mntns_fd, NAMESPACE_MOUNT);
                nếu (r <= 0)
                        trả_về r;
        }

        nếu (netns_fd >= 0) {
                r = is_our_namespace(netns_fd, NAMESPACE_NET);
                nếu (r <= 0)
                        trả_về r;
        }

        nếu (userns_fd >= 0) {
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                nếu (r <= 0)
                        trả_về r;
        }

        nếu (root_fd >= 0) {
                r = dir_fd_is_root(root_fd);
                nếu (r <= 0)
                        trả_về r;
        }

        trả_về true;
}

số_nguyên network_namespace_is_init(số_nguyên socket_fd) {
        struct stat a, b;
        số_nguyên r;

        /* This works only when privileged. */

        r = RET_NERRNO(stat(pid_namespace_path(1, NAMESPACE_NET), &a));
        nếu (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                nếu (r < 0)
                        trả_về -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                trả_về r ? true : -ENOSYS;
        }
        nếu (r < 0)
                trả_về r;

        nếu (socket_fd >= 0) {
                _cleanup_close_ số_nguyên netns = ioctl(socket_fd, SIOCGSKNS);
                nếu (netns < 0)
                        trả_về -errno;

                nếu (fstat(netns, &b) < 0)
                        trả_về -errno;
        } không_thì {
                nếu (stat(pid_namespace_path(0, NAMESPACE_NET), &b) < 0)
                        trả_về -errno;
        }

        trả_về stat_inode_same(&a, &b);
}

số_nguyên namespace_is_init(NamespaceType type) {
        số_nguyên r;

        assert(type >= 0);
        assert(type < _NAMESPACE_TYPE_MAX);

        nếu (type == NAMESPACE_NET)
                trả_về network_namespace_is_init(/* socket_fd= */ -EBADF);

        nếu (namespace_info[type].root_inode == 0)
                trả_về -EBADR; /* Cannot answer this question */

        const char *p = pid_namespace_path(0, type);

        struct stat st;
        r = RET_NERRNO(stat(p, &st));
        nếu (r == -ENOENT) {
                /* If the /proc/ns/<type> API is not around in /proc/ then ns is off in the kernel and we are in the init ns */
                r = proc_mounted();
                nếu (r < 0)
                        trả_về -ENOENT; /* If we can't determine if /proc/ is mounted propagate original error */

                trả_về r ? true : -ENOSYS;
        }
        nếu (r < 0)
                trả_về r;

        trả_về st.st_ino == namespace_info[type].root_inode;
}

số_nguyên pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type) {
        _cleanup_close_ số_nguyên ns1 = -EBADF, ns2 = -EBADF;

        /* Accepts NULL to indicate our own process */

        assert(!pid1 || pidref_is_set(pid1));
        assert(!pid2 || pidref_is_set(pid2));
        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);

        nếu (pidref_equal(pid1, pid2))
                trả_về true;

        nếu (!pid1)
                ns1 = namespace_open_by_type(type);
        không_thì
                ns1 = pidref_namespace_open_by_type(pid1, type);
        nếu (ns1 < 0)
                trả_về ns1;

        nếu (!pid2)
                ns2 = namespace_open_by_type(type);
        không_thì
                ns2 = pidref_namespace_open_by_type(pid2, type);
        nếu (ns2 < 0)
                trả_về ns2;

        trả_về fd_inode_same(ns1, ns2);
}

số_nguyên in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type) {
        assert(pid1 >= 0);
        assert(pid2 >= 0);
        trả_về pidref_in_same_namespace(pid1 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid1),
                                        pid2 == 0 ? NULL : &PIDREF_MAKE_FROM_PID(pid2),
                                        type);
}

số_nguyên namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret) {
        số_nguyên r;

        /* Note: we don't bother with pidref_is_set()/pidref_is_remote() here, as the first call we do,
         * pidref_get_ppid_as_pidref() calls those anyway */

        assert(type >= 0 && type < _NAMESPACE_TYPE_MAX);
        assert(ret);

        _cleanup_(pidref_done) PidRef current = PIDREF_NULL;
        PidRef *c = pidref;

        cho (;;) {
                _cleanup_(pidref_done) PidRef parent = PIDREF_NULL;

                r = pidref_get_ppid_as_pidref(c, &parent);
                nếu (r < 0)
                        trả_về r;

                r = pidref_in_same_namespace(c, &parent, type);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0) {
                        /* If the parent and the child are not in the same namespace, then the child is
                         * the leader we are looking for. */

                        nếu (pidref_is_set(&current))
                                *ret = TAKE_PIDREF(current);
                        không_thì {
                                r = pidref_copy(c, ret);
                                nếu (r < 0)
                                        trả_về r;
                        }

                        trả_về 0;
                }

                pidref_done(&current);
                current = TAKE_PIDREF(parent);
                c = &current;
        }
}

số_nguyên detach_mount_namespace(void) {
        /* Detaches the mount namespace, disabling propagation from our namespace to the host. Sets
         * propagation first to MS_SLAVE for all mounts (disabling propagation), and then back to MS_SHARED
         * (so that we create a new peer group).  */

        nếu (unshare(CLONE_NEWNS) < 0)
                trả_về log_debug_errno(errno, "Failed to acquire mount namespace: %m");

        nếu (mount(NULL, "/", NULL, MS_SLAVE | MS_REC, NULL) < 0)
                trả_về log_debug_errno(errno, "Failed to set mount propagation to MS_SLAVE for all mounts: %m");

        nếu (mount(NULL, "/", NULL, MS_SHARED | MS_REC, NULL) < 0)
                trả_về log_debug_errno(errno, "Failed to set mount propagation back to MS_SHARED for all mounts: %m");

        trả_về 0;
}

số_nguyên detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid) {
        uid_t from_uid;
        gid_t from_gid;
        số_nguyên r;

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

        nếu (!uid_is_valid(target_uid))
                trả_về -EINVAL;
        nếu (!gid_is_valid(target_gid))
                trả_về -EINVAL;

        r = detach_mount_namespace();
        nếu (r != -EPERM)
                trả_về r;

        from_uid = getuid();
        from_gid = getgid();

        nếu (unshare(CLONE_NEWUSER) < 0)
                trả_về log_debug_errno(errno, "Failed to acquire user namespace: %m");

        r = write_string_filef("/proc/self/uid_map", 0,
                               UID_FMT " " UID_FMT " 1\n", target_uid, from_uid);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to write uid map: %m");

        r = write_string_file("/proc/self/setgroups", "deny", 0);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to write setgroups file: %m");

        r = write_string_filef("/proc/self/gid_map", 0,
                               GID_FMT " " GID_FMT " 1\n", target_gid, from_gid);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to write gid map: %m");

        trả_về detach_mount_namespace();
}

số_nguyên detach_mount_namespace_userns(số_nguyên userns_fd) {
        số_nguyên r;

        assert(userns_fd >= 0);

        nếu (setns(userns_fd, CLONE_NEWUSER) < 0)
                trả_về log_debug_errno(errno, "Failed to join user namespace: %m");

        r = reset_uid_gid();
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to become root in user namespace: %m");

        trả_về detach_mount_namespace();
}

số_nguyên parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range) {
        _cleanup_free_ char *buffer = NULL;
        const char *range, *shift;
        số_nguyên r;
        uid_t uid_shift, uid_range = 65536;

        assert(s);

        range = strchr(s, ':');
        nếu (range) {
                buffer = strndup(s, range - s);
                nếu (!buffer)
                        trả_về log_oom();
                shift = buffer;

                range++;
                r = safe_atou32(range, &uid_range);
                nếu (r < 0)
                        trả_về log_error_errno(r, "Failed to parse UID range \"%s\": %m", range);
        } không_thì
                shift = s;

        r = parse_uid(shift, &uid_shift);
        nếu (r < 0)
                trả_về log_error_errno(r, "Failed to parse UID \"%s\": %m", s);

        nếu (!userns_shift_range_valid(uid_shift, uid_range))
                trả_về log_error_errno(SYNTHETIC_ERRNO(EINVAL), "UID range cannot be empty or go beyond " UID_FMT ".", UID_INVALID);

        nếu (ret_uid_shift)
                *ret_uid_shift = uid_shift;

        nếu (ret_uid_range)
                *ret_uid_range = uid_range;

        trả_về 0;
}

số_nguyên userns_acquire_empty(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        số_nguyên r;

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        nếu (r < 0)
                trả_về r;
        assert(r > 0);

        trả_về pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

số_nguyên userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny) {
        char path[STRLEN("/proc//setgroups") + DECIMAL_STR_MAX(pid_t) + 1];
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        số_nguyên r;

        assert(uid_map);
        assert(gid_map);

        /* Forks off a process in a new userns, configures the specified uidmap/gidmap, acquires an fd to it,
         * and then kills the process again. This way we have a userns fd that is not bound to any
         * process. We can use that for file system mounts and similar. */

        r = pidref_safe_fork("(sd-mkuserns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_USERNS|FORK_FREEZE, &pid);
        nếu (r < 0)
                trả_về r;
        assert(r > 0);

        xsprintf(path, "/proc/" PID_FMT "/uid_map", pid.pid);
        r = write_string_file(path, uid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to write UID map: %m");

        nếu (setgroups_deny) {
                xsprintf(path, "/proc/" PID_FMT "/setgroups", pid.pid);
                r = write_string_file(path, "deny", WRITE_STRING_FILE_DISABLE_BUFFER);
                nếu (r < 0)
                        trả_về log_debug_errno(r, "Failed to write setgroups file: %m");
        }

        xsprintf(path, "/proc/" PID_FMT "/gid_map", pid.pid);
        r = write_string_file(path, gid_map, WRITE_STRING_FILE_DISABLE_BUFFER);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to write GID map: %m");

        trả_về pidref_namespace_open_by_type(&pid, NAMESPACE_USER);
}

số_nguyên userns_acquire_self_root(void) {

        /* Returns a user namespace with only our own uid/gid mapped to root, and everything else unmapped.
         *
         * Note: this can be acquired unprivileged! */

        _cleanup_free_ char *uid_map = NULL, *gid_map = NULL;
        nếu (asprintf(&uid_map, "0 " UID_FMT " 1", getuid()) < 0)
                trả_về -ENOMEM;
        nếu (asprintf(&gid_map, "0 " GID_FMT " 1", getgid()) < 0)
                trả_về -ENOMEM;

        trả_về userns_acquire(uid_map, gid_map, /* setgroups_deny= */ true);
}

số_nguyên userns_enter_and_pin(số_nguyên userns_fd, PidRef *ret) {
        _cleanup_close_pair_ số_nguyên pfd[2] = EBADF_PAIR;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ssize_t n;
        char x;
        số_nguyên r;

        assert(userns_fd >= 0);
        assert(ret);

        nếu (pipe2(pfd, O_CLOEXEC) < 0)
                trả_về -errno;

        r = pidref_safe_fork_full(
                        "(sd-pinuserns)",
                        /* stdio_fds= */ NULL,
                        (số_nguyên[]) { pfd[1], userns_fd }, 2,
                        FORK_CLOSE_ALL_FDS|FORK_REOPEN_LOG|FORK_DEATHSIG_SIGKILL,
                        &pidref);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0) {
                /* Child. */

                nếu (setns(userns_fd, CLONE_NEWUSER) < 0) {
                        log_debug_errno(errno, "Failed to join userns: %m");
                        _exit(EXIT_FAILURE);
                }

                userns_fd = safe_close(userns_fd);

                n = write(pfd[1], &(const char) { 'x' }, 1);
                nếu (n < 0) {
                        log_debug_errno(errno, "Failed to write to pipe: %m");
                        _exit(EXIT_FAILURE);
                }
                assert(n == 1);

                freeze();
        }

        pfd[1] = safe_close(pfd[1]);

        n = read(pfd[0], &x, 1);
        nếu (n < 0)
                trả_về -errno;
        nếu (n == 0)
                trả_về -EPROTO;
        assert(n == 1);
        assert(x == 'x');

        *ret = TAKE_PIDREF(pidref);
        trả_về 0;
}

bool userns_supported(void) {
        trả_về access("/proc/self/uid_map", F_OK) >= 0;
}

số_nguyên userns_get_base_uid(số_nguyên userns_fd, uid_t *ret_uid, gid_t *ret_gid) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        số_nguyên r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        nếu (r < 0)
                trả_về r;

        uid_t uid;
        r = uid_map_search_root(pidref.pid, UID_RANGE_USERNS_OUTSIDE, &uid);
        nếu (r < 0)
                trả_về r;

        gid_t gid;
        r = uid_map_search_root(pidref.pid, GID_RANGE_USERNS_OUTSIDE, &gid);
        nếu (r < 0)
                trả_về r;

        nếu (!ret_gid && uid != gid)
                trả_về -EUCLEAN;

        nếu (ret_uid)
                *ret_uid = uid;
        nếu (ret_gid)
                *ret_gid = gid;

        trả_về 0;
}

số_nguyên userns_has_root_mapping(số_nguyên userns_fd) {
        _cleanup_(uid_range_freep) UIDRange *uid_range = NULL, *gid_range = NULL;
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        số_nguyên r;

        assert(userns_fd >= 0);

        r = userns_enter_and_pin(userns_fd, &pidref);
        nếu (r < 0)
                trả_về r;

        const char *p = procfs_file_alloca(pidref.pid, "uid_map");
        r = uid_range_load_userns(p, UID_RANGE_USERNS_INSIDE, &uid_range);
        nếu (r < 0)
                trả_về r;

        p = procfs_file_alloca(pidref.pid, "gid_map");
        r = uid_range_load_userns(p, GID_RANGE_USERNS_INSIDE, &gid_range);
        nếu (r < 0)
                trả_về r;

        /* The maps can be written only once. While they are empty the namespace is still being set up and
         * root might yet get mapped, so refuse to answer rather than guess. */
        nếu (uid_range_is_empty(uid_range) || uid_range_is_empty(gid_range))
                trả_về log_debug_errno(SYNTHETIC_ERRNO(ENODATA),
                                       "User namespace has no UID/GID mapping yet.");

        trả_về uid_range_contains(uid_range, 0) && uid_range_contains(gid_range, 0);
}

số_nguyên process_is_owned_by_uid(const PidRef *pidref, uid_t uid) {
        số_nguyên r;

        /* Checks if the specified process either is owned directly by the specified user, or if it is inside
         * a user namespace owned by it. */

        assert(uid_is_valid(uid));

        uid_t process_uid;
        r = pidref_get_uid(pidref, &process_uid);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to get UID of process " PID_FMT ": %m", pidref ? pidref->pid : 0);
        nếu (process_uid == uid)
                trả_về true;

        log_debug("Process " PID_FMT " has UID " UID_FMT ", which doesn't match expected UID " UID_FMT ", checking user namespace ownership.",
                  pidref->pid, process_uid, uid);

        _cleanup_close_ số_nguyên userns_fd = -EBADF;
        userns_fd = pidref_namespace_open_by_type(pidref, NAMESPACE_USER);
        nếu (userns_fd == -ENOPKG) /* If userns is not supported, then they don't matter for ownership */
                trả_về false;
        nếu (userns_fd < 0)
                trả_về log_debug_errno(userns_fd, "Failed to open user namespace of process " PID_FMT ": %m", pidref->pid);

        cho (unsigned iteration = 0;; iteration++) {
                uid_t ns_uid;

                /* This process is in our own userns? Then we are done, in our own userns only the UIDs
                 * themselves matter. */
                r = is_our_namespace(userns_fd, NAMESPACE_USER);
                nếu (r < 0)
                        trả_về log_debug_errno(r, "Failed to check if user namespace of process " PID_FMT " is our own (iteration %u): %m", pidref->pid, iteration);
                nếu (r > 0) {
                        log_debug("User namespace of process " PID_FMT " is our own namespace (iteration %u), not owned by expected UID.", pidref->pid, iteration);
                        trả_về false;
                }

                nếu (ioctl(userns_fd, NS_GET_OWNER_UID, &ns_uid) < 0)
                        trả_về log_debug_errno(errno, "Failed to get owner UID of user namespace of process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                nếu (ns_uid == uid) {
                        log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT " (iteration %u), ownership check passed.", pidref->pid, uid, iteration);
                        trả_về true;
                }

                log_debug("User namespace of process " PID_FMT " is owned by UID " UID_FMT ", expected UID " UID_FMT " (iteration %u), going up the tree.",
                          pidref->pid, ns_uid, uid, iteration);

                /* Paranoia check */
                nếu (iteration > 16)
                        trả_về log_debug_errno(SYNTHETIC_ERRNO(ELOOP), "Giving up while tracing parents of user namespaces after %u steps.", iteration);

                /* Go up the tree */
                _cleanup_close_ số_nguyên parent_fd = ioctl(userns_fd, NS_GET_USERNS);
                nếu (parent_fd < 0) {
                        nếu (errno == EPERM) { /* EPERM means we left our own userns */
                                log_debug("NS_GET_USERNS ioctl returned EPERM for process " PID_FMT " (iteration %u), left our own userns.", pidref->pid, iteration);
                                trả_về false;
                        }

                        trả_về log_debug_errno(errno, "NS_GET_USERNS ioctl failed for process " PID_FMT " (iteration %u): %m", pidref->pid, iteration);
                }

                close_and_replace(userns_fd, parent_fd);
        }
}

số_nguyên namespace_open_by_id(uint64_t ns_id) {
        số_nguyên r;

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

        nếu (ns_id == 0)
                trả_về -EINVAL;

        r = namespace_is_init(NAMESPACE_USER);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EPERM;

        r = namespace_is_init(NAMESPACE_PID);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EPERM;

        r = have_effective_cap(CAP_SYS_ADMIN);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0)
                trả_về -EPERM;

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

        trả_về RET_NERRNO(open_by_handle_at(FD_NSFS_ROOT, &fh.file_handle, O_PATH|O_CLOEXEC));
}

số_nguyên is_idmapping_supported(const char *path) {
        _cleanup_close_ số_nguyên mount_fd = -EBADF, userns_fd = -EBADF, dir_fd = -EBADF;
        số_nguyên r;

        assert(path);

        nếu (!mount_new_api_supported())
                trả_về false;

        userns_fd = r = userns_acquire_self_root();
        nếu (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                trả_về false;
        nếu (r == -ENOSPC) {
                log_debug_errno(r, "Failed to acquire new user namespace, user.max_user_namespaces seems to be exhausted or maybe even zero, assuming ID-mapping is not supported: %m");
                trả_về false;
        }
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to acquire new user namespace for checking if '%s' supports ID-mapping: %m", path);

        dir_fd = r = RET_NERRNO(open(path, O_RDONLY | O_CLOEXEC | O_NOFOLLOW));
        nếu (ERRNO_IS_NEG_NOT_SUPPORTED(r))
                trả_về false;
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to open '%s', cannot determine if ID-mapping is supported: %m", path);

        mount_fd = r = RET_NERRNO(open_tree(dir_fd, "", AT_EMPTY_PATH | OPEN_TREE_CLONE | OPEN_TREE_CLOEXEC));
        nếu (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                trả_về false;
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to open mount tree '%s', cannot determine if ID-mapping is supported: %m", path);

        r = RET_NERRNO(mount_setattr(mount_fd, "", AT_EMPTY_PATH,
                       &(struct mount_attr) {
                                .attr_set = MOUNT_ATTR_IDMAP | MOUNT_ATTR_NOSUID | MOUNT_ATTR_NOEXEC | MOUNT_ATTR_RDONLY | MOUNT_ATTR_NODEV,
                                .userns_fd = userns_fd,
                        }, sizeof(struct mount_attr)));
        nếu (ERRNO_IS_NEG_NOT_SUPPORTED(r) || ERRNO_IS_NEG_PRIVILEGE(r) || r == -EINVAL)
                trả_về false;
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to set mount attribute to '%s', cannot determine if ID-mapping is supported: %m", path);

        trả_về true;
}

số_nguyên netns_acquire(void) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pid = PIDREF_NULL;
        số_nguyên r;

        /* Forks off a process in a new network namespace, acquires a network namespace fd, and then kills
         * the process again. This way we have a netns fd that is not bound to any process. */

        r = pidref_safe_fork("(sd-mknetns)", FORK_CLOSE_ALL_FDS|FORK_DEATHSIG_SIGKILL|FORK_NEW_NETNS|FORK_FREEZE, &pid);
        nếu (r < 0)
                trả_về log_debug_errno(r, "Failed to fork process into new netns: %m");
        assert(r > 0);

        trả_về pidref_namespace_open_by_type(&pid, NAMESPACE_NET);
}
