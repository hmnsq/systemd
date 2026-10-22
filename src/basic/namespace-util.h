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

පූර්ණ_සංඛ්‍යා pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
පූර්ණ_සංඛ්‍යා namespace_open_by_type(NamespaceType type);

පූර්ණ_සංඛ්‍යා pidref_namespace_open(
                const PidRef *pidref,
                පූර්ණ_සංඛ්‍යා *ret_pidns_fd,
                පූර්ණ_සංඛ්‍යා *ret_mntns_fd,
                පූර්ණ_සංඛ්‍යා *ret_netns_fd,
                පූර්ණ_සංඛ්‍යා *ret_userns_fd,
                පූර්ණ_සංඛ්‍යා *ret_root_fd);
පූර්ණ_සංඛ්‍යා namespace_open(
                pid_t pid,
                පූර්ණ_සංඛ්‍යා *ret_pidns_fd,
                පූර්ණ_සංඛ්‍යා *ret_mntns_fd,
                පූර්ණ_සංඛ්‍යා *ret_netns_fd,
                පූර්ණ_සංඛ්‍යා *ret_userns_fd,
                පූර්ණ_සංඛ්‍යා *ret_root_fd);

typedef ga_nanaa NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
පූර්ණ_සංඛ්‍යා namespace_enter_full(
                පූර්ණ_සංඛ්‍යා pidns_fd,
                පූර්ණ_සංඛ්‍යා mntns_fd,
                පූර්ණ_සංඛ්‍යා netns_fd,
                පූර්ණ_සංඛ්‍යා userns_fd,
                පූර්ණ_සංඛ්‍යා root_fd,
                NamespaceEnterFlags flags);

static anta_hstha පූර්ණ_සංඛ්‍යා namespace_enter(පූර්ණ_සංඛ්‍යා pidns_fd, පූර්ණ_සංඛ්‍යා mntns_fd, පූර්ණ_සංඛ්‍යා netns_fd, පූර්ණ_සංඛ්‍යා userns_fd, පූර්ණ_සංඛ්‍යා root_fd) {
        ආපසු namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

පූර්ණ_සංඛ්‍යා fd_is_namespace(පූර්ණ_සංඛ්‍යා fd, NamespaceType type);
පූර්ණ_සංඛ්‍යා is_our_namespace(පූර්ණ_සංඛ්‍යා fd, NamespaceType type);
පූර්ණ_සංඛ්‍යා are_our_namespaces(පූර්ණ_සංඛ්‍යා pidns_fd, පූර්ණ_සංඛ්‍යා mntns_fd, පූර්ණ_සංඛ්‍යා netns_fd, පූර්ණ_සංඛ්‍යා userns_fd, පූර්ණ_සංඛ්‍යා root_fd);

පූර්ණ_සංඛ්‍යා network_namespace_is_init(පූර්ණ_සංඛ්‍යා socket_fd);
පූර්ණ_සංඛ්‍යා namespace_is_init(NamespaceType type);

පූර්ණ_සංඛ්‍යා pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
පූර්ණ_සංඛ්‍යා in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

පූර්ණ_සංඛ්‍යා namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

පූර්ණ_සංඛ්‍යා detach_mount_namespace(void);
පූර්ණ_සංඛ්‍යා detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
පූර්ණ_සංඛ්‍යා detach_mount_namespace_userns(පූර්ණ_සංඛ්‍යා userns_fd);

static anta_hstha bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        නම් (range <= 0)
                ආපසු false;

        නම් (shift > (uid_t) -1 - range)
                ආපසු false;

        ආපසු true;
}

පූර්ණ_සංඛ්‍යා parse_userns_uid_range(const var_na *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

පූර්ණ_සංඛ්‍යා userns_acquire_empty(void);
පූර්ණ_සංඛ්‍යා userns_acquire(const var_na *uid_map, const var_na *gid_map, bool setgroups_deny);
පූර්ණ_සංඛ්‍යා userns_acquire_self_root(void);
පූර්ණ_සංඛ්‍යා userns_enter_and_pin(පූර්ණ_සංඛ්‍යා userns_fd, PidRef *ret);
bool userns_supported(void);

පූර්ණ_සංඛ්‍යා userns_get_base_uid(පූර්ණ_සංඛ්‍යා userns_fd, uid_t *ret_uid, gid_t *ret_gid);
පූර්ණ_සංඛ්‍යා userns_has_root_mapping(පූර්ණ_සංඛ්‍යා userns_fd);

පූර්ණ_සංඛ්‍යා namespace_open_by_id(uint64_t ns_id);

පූර්ණ_සංඛ්‍යා process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

පූර්ණ_සංඛ්‍යා is_idmapping_supported(const var_na *path);

පූර්ණ_සංඛ්‍යා netns_acquire(void);
