/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ NamespaceType {
        NAMESPACE_CGROUP,
        NAMESPACE_IPC,
        NAMESPACE_NET,
        NAMESPACE_MOUNT,
        NAMESPACE_PID,
        NAMESPACE_USER,
        NAMESPACE_UTS,
        NAMESPACE_ಸಮಯ,
        _NAMESPACE_TYPE_MAX,
        _NAMESPACE_TYPE_INVALID = -EINVAL,
} NamespaceType;

ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ namespace_info {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proc_name;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *proc_ಮಾರ್ಗ;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ clone_flag;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ pidfd_get_ns_ioctl_cmd;
        ino_t ಬೇರು_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_ಗಮ್ಯ_namespace_type(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ clone_flag);

bool namespace_type_supported(NamespaceType type);

ಪೂರ್ಣಾಂಕ pidref_namespace_open_by_type(ಸ್ಥಿರಾಂಕ PidRef *pidref, NamespaceType type);
ಪೂರ್ಣಾಂಕ namespace_open_by_type(NamespaceType type);

ಪೂರ್ಣಾಂಕ pidref_namespace_open(
                ಸ್ಥಿರಾಂಕ PidRef *pidref,
                ಪೂರ್ಣಾಂಕ *ret_pidns_fd,
                ಪೂರ್ಣಾಂಕ *ret_mntns_fd,
                ಪೂರ್ಣಾಂಕ *ret_netns_fd,
                ಪೂರ್ಣಾಂಕ *ret_userns_fd,
                ಪೂರ್ಣಾಂಕ *ret_ಬೇರು_fd);
ಪೂರ್ಣಾಂಕ namespace_open(
                pid_t pid,
                ಪೂರ್ಣಾಂಕ *ret_pidns_fd,
                ಪೂರ್ಣಾಂಕ *ret_mntns_fd,
                ಪೂರ್ಣಾಂಕ *ret_netns_fd,
                ಪೂರ್ಣಾಂಕ *ret_userns_fd,
                ಪೂರ್ಣಾಂಕ *ret_ಬೇರು_fd);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ಬೇರು_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
ಪೂರ್ಣಾಂಕ namespace_enter_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ pidns_fd,
                ಪೂರ್ಣಾಂಕ mntns_fd,
                ಪೂರ್ಣಾಂಕ netns_fd,
                ಪೂರ್ಣಾಂಕ userns_fd,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                NamespaceEnterFlags flags);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ namespace_enter(ಪೂರ್ಣಾಂಕ pidns_fd, ಪೂರ್ಣಾಂಕ mntns_fd, ಪೂರ್ಣಾಂಕ netns_fd, ಪೂರ್ಣಾಂಕ userns_fd, ಪೂರ್ಣಾಂಕ ಬೇರು_fd) {
        ಹಿಂತಿರುಗಿ namespace_enter_ತುಂಬಿದ(pidns_fd, mntns_fd, netns_fd, userns_fd, ಬೇರು_fd, /* flags= */ 0);
}

ಪೂರ್ಣಾಂಕ fd_is_namespace(ಪೂರ್ಣಾಂಕ fd, NamespaceType type);
ಪೂರ್ಣಾಂಕ is_our_namespace(ಪೂರ್ಣಾಂಕ fd, NamespaceType type);
ಪೂರ್ಣಾಂಕ are_our_namespaces(ಪೂರ್ಣಾಂಕ pidns_fd, ಪೂರ್ಣಾಂಕ mntns_fd, ಪೂರ್ಣಾಂಕ netns_fd, ಪೂರ್ಣಾಂಕ userns_fd, ಪೂರ್ಣಾಂಕ ಬೇರು_fd);

ಪೂರ್ಣಾಂಕ ಜಾಲ_namespace_is_init(ಪೂರ್ಣಾಂಕ socket_fd);
ಪೂರ್ಣಾಂಕ namespace_is_init(NamespaceType type);

ಪೂರ್ಣಾಂಕ pidref_ಒಳ_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
ಪೂರ್ಣಾಂಕ ಒಳ_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

ಪೂರ್ಣಾಂಕ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

ಪೂರ್ಣಾಂಕ detach_mount_namespace(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ detach_mount_namespace_harder(uid_t ಗುರಿ_uid, gid_t ಗುರಿ_gid);
ಪೂರ್ಣಾಂಕ detach_mount_namespace_userns(ಪೂರ್ಣಾಂಕ userns_fd);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool userns_ಸರಿಸು_range_valid(uid_t ಸರಿಸು, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ಆದರೆ (range <= 0)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಸರಿಸು > (uid_t) -1 - range)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ parse_userns_uid_range(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uid_t *ret_uid_ಸರಿಸು, uid_t *ret_uid_range);

ಪೂರ್ಣಾಂಕ userns_acquire_ಖಾಲಿಗಳು(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ userns_acquire(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *uid_map, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *gid_map, bool setgroups_deny);
ಪೂರ್ಣಾಂಕ userns_acquire_self_ಬೇರು(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ userns_enter_and_pin(ಪೂರ್ಣಾಂಕ userns_fd, PidRef *ret);
bool userns_supported(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ userns_get_ತಳ_uid(ಪೂರ್ಣಾಂಕ userns_fd, uid_t *ret_uid, gid_t *ret_gid);
ಪೂರ್ಣಾಂಕ userns_has_ಬೇರು_mapping(ಪೂರ್ಣಾಂಕ userns_fd);

ಪೂರ್ಣಾಂಕ namespace_open_by_id(uint64_t ns_id);

ಪೂರ್ಣಾಂಕ process_is_owned_by_uid(ಸ್ಥಿರಾಂಕ PidRef *pidref, uid_t uid);

ಪೂರ್ಣಾಂಕ is_idmapping_supported(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ netns_acquire(ಶೂನ್ಯ);
