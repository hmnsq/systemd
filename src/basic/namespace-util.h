/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ NamespaceType {
        NAMESPACE_CGROUP,
        NAMESPACE_IPC,
        NAMESPACE_NET,
        NAMESPACE_MOUNT,
        NAMESPACE_PID,
        NAMESPACE_USER,
        NAMESPACE_UTS,
        NAMESPACE_ਸਮਾਂ,
        _NAMESPACE_TYPE_MAX,
        _NAMESPACE_TYPE_INVALID = -EINVAL,
} NamespaceType;

ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ namespace_info {
        ਅਚਲ ਅੱਖਰ *proc_name;
        ਅਚਲ ਅੱਖਰ *proc_ਰਸਤਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ clone_flag;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ pidfd_get_ns_ioctl_cmd;
        ino_t ਜੜ੍ਹ_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_ਮੰਜ਼ਿਲ_namespace_type(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ clone_flag);

bool namespace_type_supported(NamespaceType type);

ਪੂਰਨ pidref_namespace_open_by_type(ਅਚਲ PidRef *pidref, NamespaceType type);
ਪੂਰਨ namespace_open_by_type(NamespaceType type);

ਪੂਰਨ pidref_namespace_open(
                ਅਚਲ PidRef *pidref,
                ਪੂਰਨ *ret_pidns_fd,
                ਪੂਰਨ *ret_mntns_fd,
                ਪੂਰਨ *ret_netns_fd,
                ਪੂਰਨ *ret_userns_fd,
                ਪੂਰਨ *ret_ਜੜ੍ਹ_fd);
ਪੂਰਨ namespace_open(
                pid_t pid,
                ਪੂਰਨ *ret_pidns_fd,
                ਪੂਰਨ *ret_mntns_fd,
                ਪੂਰਨ *ret_netns_fd,
                ਪੂਰਨ *ret_userns_fd,
                ਪੂਰਨ *ret_ਜੜ੍ਹ_fd);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ਜੜ੍ਹ_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
ਪੂਰਨ namespace_enter_ਭਰਿਆ(
                ਪੂਰਨ pidns_fd,
                ਪੂਰਨ mntns_fd,
                ਪੂਰਨ netns_fd,
                ਪੂਰਨ userns_fd,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                NamespaceEnterFlags flags);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ namespace_enter(ਪੂਰਨ pidns_fd, ਪੂਰਨ mntns_fd, ਪੂਰਨ netns_fd, ਪੂਰਨ userns_fd, ਪੂਰਨ ਜੜ੍ਹ_fd) {
        ਵਾਪਸ namespace_enter_ਭਰਿਆ(pidns_fd, mntns_fd, netns_fd, userns_fd, ਜੜ੍ਹ_fd, /* flags= */ 0);
}

ਪੂਰਨ fd_is_namespace(ਪੂਰਨ fd, NamespaceType type);
ਪੂਰਨ is_our_namespace(ਪੂਰਨ fd, NamespaceType type);
ਪੂਰਨ are_our_namespaces(ਪੂਰਨ pidns_fd, ਪੂਰਨ mntns_fd, ਪੂਰਨ netns_fd, ਪੂਰਨ userns_fd, ਪੂਰਨ ਜੜ੍ਹ_fd);

ਪੂਰਨ ਜਾਲ_namespace_is_init(ਪੂਰਨ socket_fd);
ਪੂਰਨ namespace_is_init(NamespaceType type);

ਪੂਰਨ pidref_ਵਿੱਚ_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
ਪੂਰਨ ਵਿੱਚ_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

ਪੂਰਨ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

ਪੂਰਨ detach_mount_namespace(ਖਾਲੀ);
ਪੂਰਨ detach_mount_namespace_harder(uid_t ਨਿਸ਼ਾਨਾ_uid, gid_t ਨਿਸ਼ਾਨਾ_gid);
ਪੂਰਨ detach_mount_namespace_userns(ਪੂਰਨ userns_fd);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool userns_ਖਿਸਕਾਓ_range_valid(uid_t ਖਿਸਕਾਓ, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ਜੇ (range <= 0)
                ਵਾਪਸ false;

        ਜੇ (ਖਿਸਕਾਓ > (uid_t) -1 - range)
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ parse_userns_uid_range(ਅਚਲ ਅੱਖਰ *s, uid_t *ret_uid_ਖਿਸਕਾਓ, uid_t *ret_uid_range);

ਪੂਰਨ userns_acquire_ਖਾਲੀ_ਥਾਵਾਂ(ਖਾਲੀ);
ਪੂਰਨ userns_acquire(ਅਚਲ ਅੱਖਰ *uid_map, ਅਚਲ ਅੱਖਰ *gid_map, bool setgroups_deny);
ਪੂਰਨ userns_acquire_self_ਜੜ੍ਹ(ਖਾਲੀ);
ਪੂਰਨ userns_enter_and_pin(ਪੂਰਨ userns_fd, PidRef *ret);
bool userns_supported(ਖਾਲੀ);

ਪੂਰਨ userns_get_ਅਧਾਰ_uid(ਪੂਰਨ userns_fd, uid_t *ret_uid, gid_t *ret_gid);
ਪੂਰਨ userns_has_ਜੜ੍ਹ_mapping(ਪੂਰਨ userns_fd);

ਪੂਰਨ namespace_open_by_id(uint64_t ns_id);

ਪੂਰਨ process_is_owned_by_uid(ਅਚਲ PidRef *pidref, uid_t uid);

ਪੂਰਨ is_idmapping_supported(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ netns_acquire(ਖਾਲੀ);
