/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை NamespaceType {
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

வெளி மாறிலி கட்டமைப்பு namespace_info {
        மாறிலி எழுத்து *proc_name;
        மாறிலி எழுத்து *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t வேர்_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

முழு pidref_namespace_open_by_type(மாறிலி PidRef *pidref, NamespaceType type);
முழு namespace_open_by_type(NamespaceType type);

முழு pidref_namespace_open(
                மாறிலி PidRef *pidref,
                முழு *ret_pidns_fd,
                முழு *ret_mntns_fd,
                முழு *ret_netns_fd,
                முழு *ret_userns_fd,
                முழு *ret_வேர்_fd);
முழு namespace_open(
                pid_t pid,
                முழு *ret_pidns_fd,
                முழு *ret_mntns_fd,
                முழு *ret_netns_fd,
                முழு *ret_userns_fd,
                முழு *ret_வேர்_fd);

வகைவரை எண்ணிக்கை NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_வேர்_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
முழு namespace_enter_full(
                முழு pidns_fd,
                முழு mntns_fd,
                முழு netns_fd,
                முழு userns_fd,
                முழு வேர்_fd,
                NamespaceEnterFlags flags);

நிலையான உள்வரி முழு namespace_enter(முழு pidns_fd, முழு mntns_fd, முழு netns_fd, முழு userns_fd, முழு வேர்_fd) {
        திரும்பு namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, வேர்_fd, /* flags= */ 0);
}

முழு fd_is_namespace(முழு fd, NamespaceType type);
முழு is_our_namespace(முழு fd, NamespaceType type);
முழு are_our_namespaces(முழு pidns_fd, முழு mntns_fd, முழு netns_fd, முழு userns_fd, முழு வேர்_fd);

முழு network_namespace_is_init(முழு socket_fd);
முழு namespace_is_init(NamespaceType type);

முழு pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
முழு in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

முழு namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

முழு detach_mount_namespace(வெறுமை);
முழு detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
முழு detach_mount_namespace_userns(முழு userns_fd);

நிலையான உள்வரி bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        எனில் (range <= 0)
                திரும்பு false;

        எனில் (shift > (uid_t) -1 - range)
                திரும்பு false;

        திரும்பு true;
}

முழு parse_userns_uid_range(மாறிலி எழுத்து *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

முழு userns_acquire_empty(வெறுமை);
முழு userns_acquire(மாறிலி எழுத்து *uid_map, மாறிலி எழுத்து *gid_map, bool setgroups_deny);
முழு userns_acquire_self_வேர்(வெறுமை);
முழு userns_enter_and_pin(முழு userns_fd, PidRef *ret);
bool userns_supported(வெறுமை);

முழு userns_get_base_uid(முழு userns_fd, uid_t *ret_uid, gid_t *ret_gid);
முழு userns_has_வேர்_mapping(முழு userns_fd);

முழு namespace_open_by_id(uint64_t ns_id);

முழு process_is_owned_by_uid(மாறிலி PidRef *pidref, uid_t uid);

முழு is_idmapping_supported(மாறிலி எழுத்து *path);

முழு netns_acquire(வெறுமை);
