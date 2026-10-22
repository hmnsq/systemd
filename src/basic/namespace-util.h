/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa NamespaceType {
        NAMESPACE_CGROUP,
        NAMESPACE_IPC,
        NAMESPACE_NET,
        NAMESPACE_MOUNT,
        NAMESPACE_PID,
        NAMESPACE_USER,
        NAMESPACE_UTS,
        NAMESPACE_TIME,
        _NAMESPACE_TYPE_MAX,
        _NAMESPACE_TYPE_INVALID = -EINVAL,
} NamespaceType;

extern const sa_nracanaa namespace_info {
        const var_na *proc_name;
        const var_na *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t root_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

จำนวนเต็ม pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
จำนวนเต็ม namespace_open_by_type(NamespaceType type);

จำนวนเต็ม pidref_namespace_open(
                const PidRef *pidref,
                จำนวนเต็ม *ret_pidns_fd,
                จำนวนเต็ม *ret_mntns_fd,
                จำนวนเต็ม *ret_netns_fd,
                จำนวนเต็ม *ret_userns_fd,
                จำนวนเต็ม *ret_root_fd);
จำนวนเต็ม namespace_open(
                pid_t pid,
                จำนวนเต็ม *ret_pidns_fd,
                จำนวนเต็ม *ret_mntns_fd,
                จำนวนเต็ม *ret_netns_fd,
                จำนวนเต็ม *ret_userns_fd,
                จำนวนเต็ม *ret_root_fd);

typedef ga_nanaa NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
จำนวนเต็ม namespace_enter_full(
                จำนวนเต็ม pidns_fd,
                จำนวนเต็ม mntns_fd,
                จำนวนเต็ม netns_fd,
                จำนวนเต็ม userns_fd,
                จำนวนเต็ม root_fd,
                NamespaceEnterFlags flags);

static anta_hstha จำนวนเต็ม namespace_enter(จำนวนเต็ม pidns_fd, จำนวนเต็ม mntns_fd, จำนวนเต็ม netns_fd, จำนวนเต็ม userns_fd, จำนวนเต็ม root_fd) {
        คืน namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

จำนวนเต็ม fd_is_namespace(จำนวนเต็ม fd, NamespaceType type);
จำนวนเต็ม is_our_namespace(จำนวนเต็ม fd, NamespaceType type);
จำนวนเต็ม are_our_namespaces(จำนวนเต็ม pidns_fd, จำนวนเต็ม mntns_fd, จำนวนเต็ม netns_fd, จำนวนเต็ม userns_fd, จำนวนเต็ม root_fd);

จำนวนเต็ม network_namespace_is_init(จำนวนเต็ม socket_fd);
จำนวนเต็ม namespace_is_init(NamespaceType type);

จำนวนเต็ม pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
จำนวนเต็ม in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

จำนวนเต็ม namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

จำนวนเต็ม detach_mount_namespace(void);
จำนวนเต็ม detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
จำนวนเต็ม detach_mount_namespace_userns(จำนวนเต็ม userns_fd);

static anta_hstha bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ถ้า (range <= 0)
                คืน false;

        ถ้า (shift > (uid_t) -1 - range)
                คืน false;

        คืน true;
}

จำนวนเต็ม parse_userns_uid_range(const var_na *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

จำนวนเต็ม userns_acquire_empty(void);
จำนวนเต็ม userns_acquire(const var_na *uid_map, const var_na *gid_map, bool setgroups_deny);
จำนวนเต็ม userns_acquire_self_root(void);
จำนวนเต็ม userns_enter_and_pin(จำนวนเต็ม userns_fd, PidRef *ret);
bool userns_supported(void);

จำนวนเต็ม userns_get_base_uid(จำนวนเต็ม userns_fd, uid_t *ret_uid, gid_t *ret_gid);
จำนวนเต็ม userns_has_root_mapping(จำนวนเต็ม userns_fd);

จำนวนเต็ม namespace_open_by_id(uint64_t ns_id);

จำนวนเต็ม process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

จำนวนเต็ม is_idmapping_supported(const var_na *path);

จำนวนเต็ม netns_acquire(void);
