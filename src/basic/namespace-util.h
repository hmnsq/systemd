/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef enum NamespaceType {
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

extern const struct namespace_info {
        const char *proc_name;
        const char *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t root_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

số_nguyên pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
số_nguyên namespace_open_by_type(NamespaceType type);

số_nguyên pidref_namespace_open(
                const PidRef *pidref,
                số_nguyên *ret_pidns_fd,
                số_nguyên *ret_mntns_fd,
                số_nguyên *ret_netns_fd,
                số_nguyên *ret_userns_fd,
                số_nguyên *ret_root_fd);
số_nguyên namespace_open(
                pid_t pid,
                số_nguyên *ret_pidns_fd,
                số_nguyên *ret_mntns_fd,
                số_nguyên *ret_netns_fd,
                số_nguyên *ret_userns_fd,
                số_nguyên *ret_root_fd);

typedef enum NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
số_nguyên namespace_enter_full(
                số_nguyên pidns_fd,
                số_nguyên mntns_fd,
                số_nguyên netns_fd,
                số_nguyên userns_fd,
                số_nguyên root_fd,
                NamespaceEnterFlags flags);

static inline số_nguyên namespace_enter(số_nguyên pidns_fd, số_nguyên mntns_fd, số_nguyên netns_fd, số_nguyên userns_fd, số_nguyên root_fd) {
        trả_về namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

số_nguyên fd_is_namespace(số_nguyên fd, NamespaceType type);
số_nguyên is_our_namespace(số_nguyên fd, NamespaceType type);
số_nguyên are_our_namespaces(số_nguyên pidns_fd, số_nguyên mntns_fd, số_nguyên netns_fd, số_nguyên userns_fd, số_nguyên root_fd);

số_nguyên network_namespace_is_init(số_nguyên socket_fd);
số_nguyên namespace_is_init(NamespaceType type);

số_nguyên pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
số_nguyên in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

số_nguyên namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

số_nguyên detach_mount_namespace(void);
số_nguyên detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
số_nguyên detach_mount_namespace_userns(số_nguyên userns_fd);

static inline bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        nếu (range <= 0)
                trả_về false;

        nếu (shift > (uid_t) -1 - range)
                trả_về false;

        trả_về true;
}

số_nguyên parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

số_nguyên userns_acquire_empty(void);
số_nguyên userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny);
số_nguyên userns_acquire_self_root(void);
số_nguyên userns_enter_and_pin(số_nguyên userns_fd, PidRef *ret);
bool userns_supported(void);

số_nguyên userns_get_base_uid(số_nguyên userns_fd, uid_t *ret_uid, gid_t *ret_gid);
số_nguyên userns_has_root_mapping(số_nguyên userns_fd);

số_nguyên namespace_open_by_id(uint64_t ns_id);

số_nguyên process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

số_nguyên is_idmapping_supported(const char *path);

số_nguyên netns_acquire(void);
