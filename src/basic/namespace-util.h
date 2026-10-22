/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ NamespaceType {
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

ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ namespace_info {
        ಧ್ರುವ ವರ್ಣ *proc_name;
        ಧ್ರುವ ವರ್ಣ *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t ಬೇರು_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

ಪೂರ್ಣಾಙ್ಕ pidref_namespace_open_by_type(ಧ್ರುವ PidRef *pidref, NamespaceType type);
ಪೂರ್ಣಾಙ್ಕ namespace_open_by_type(NamespaceType type);

ಪೂರ್ಣಾಙ್ಕ pidref_namespace_open(
                ಧ್ರುವ PidRef *pidref,
                ಪೂರ್ಣಾಙ್ಕ *ret_pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_netns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_userns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_ಬೇರು_fd);
ಪೂರ್ಣಾಙ್ಕ namespace_open(
                pid_t pid,
                ಪೂರ್ಣಾಙ್ಕ *ret_pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_netns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_userns_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_ಬೇರು_fd);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ಬೇರು_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
ಪೂರ್ಣಾಙ್ಕ namespace_enter_full(
                ಪೂರ್ಣಾಙ್ಕ pidns_fd,
                ಪೂರ್ಣಾಙ್ಕ mntns_fd,
                ಪೂರ್ಣಾಙ್ಕ netns_fd,
                ಪೂರ್ಣಾಙ್ಕ userns_fd,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                NamespaceEnterFlags flags);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ namespace_enter(ಪೂರ್ಣಾಙ್ಕ pidns_fd, ಪೂರ್ಣಾಙ್ಕ mntns_fd, ಪೂರ್ಣಾಙ್ಕ netns_fd, ಪೂರ್ಣಾಙ್ಕ userns_fd, ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd) {
        ಪ್ರತ್ಯಾಗಮ namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, ಬೇರು_fd, /* flags= */ 0);
}

ಪೂರ್ಣಾಙ್ಕ fd_is_namespace(ಪೂರ್ಣಾಙ್ಕ fd, NamespaceType type);
ಪೂರ್ಣಾಙ್ಕ is_our_namespace(ಪೂರ್ಣಾಙ್ಕ fd, NamespaceType type);
ಪೂರ್ಣಾಙ್ಕ are_our_namespaces(ಪೂರ್ಣಾಙ್ಕ pidns_fd, ಪೂರ್ಣಾಙ್ಕ mntns_fd, ಪೂರ್ಣಾಙ್ಕ netns_fd, ಪೂರ್ಣಾಙ್ಕ userns_fd, ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd);

ಪೂರ್ಣಾಙ್ಕ network_namespace_is_init(ಪೂರ್ಣಾಙ್ಕ socket_fd);
ಪೂರ್ಣಾಙ್ಕ namespace_is_init(NamespaceType type);

ಪೂರ್ಣಾಙ್ಕ pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
ಪೂರ್ಣಾಙ್ಕ in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

ಪೂರ್ಣಾಙ್ಕ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

ಪೂರ್ಣಾಙ್ಕ detach_mount_namespace(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
ಪೂರ್ಣಾಙ್ಕ detach_mount_namespace_userns(ಪೂರ್ಣಾಙ್ಕ userns_fd);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ಯದಿ (range <= 0)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (shift > (uid_t) -1 - range)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ parse_userns_uid_range(ಧ್ರುವ ವರ್ಣ *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

ಪೂರ್ಣಾಙ್ಕ userns_acquire_empty(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ userns_acquire(ಧ್ರುವ ವರ್ಣ *uid_map, ಧ್ರುವ ವರ್ಣ *gid_map, bool setgroups_deny);
ಪೂರ್ಣಾಙ್ಕ userns_acquire_self_ಬೇರು(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ userns_enter_and_pin(ಪೂರ್ಣಾಙ್ಕ userns_fd, PidRef *ret);
bool userns_supported(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ userns_get_base_uid(ಪೂರ್ಣಾಙ್ಕ userns_fd, uid_t *ret_uid, gid_t *ret_gid);
ಪೂರ್ಣಾಙ್ಕ userns_has_ಬೇರು_mapping(ಪೂರ್ಣಾಙ್ಕ userns_fd);

ಪೂರ್ಣಾಙ್ಕ namespace_open_by_id(uint64_t ns_id);

ಪೂರ್ಣಾಙ್ಕ process_is_owned_by_uid(ಧ್ರುವ PidRef *pidref, uid_t uid);

ಪೂರ್ಣಾಙ್ಕ is_idmapping_supported(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ netns_acquire(ಶೂನ್ಯ);
