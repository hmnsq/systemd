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

整数 pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
整数 namespace_open_by_type(NamespaceType type);

整数 pidref_namespace_open(
                const PidRef *pidref,
                整数 *ret_pidns_fd,
                整数 *ret_mntns_fd,
                整数 *ret_netns_fd,
                整数 *ret_userns_fd,
                整数 *ret_root_fd);
整数 namespace_open(
                pid_t pid,
                整数 *ret_pidns_fd,
                整数 *ret_mntns_fd,
                整数 *ret_netns_fd,
                整数 *ret_userns_fd,
                整数 *ret_root_fd);

typedef enum NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
整数 namespace_enter_full(
                整数 pidns_fd,
                整数 mntns_fd,
                整数 netns_fd,
                整数 userns_fd,
                整数 root_fd,
                NamespaceEnterFlags flags);

static inline 整数 namespace_enter(整数 pidns_fd, 整数 mntns_fd, 整数 netns_fd, 整数 userns_fd, 整数 root_fd) {
        返回 namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

整数 fd_is_namespace(整数 fd, NamespaceType type);
整数 is_our_namespace(整数 fd, NamespaceType type);
整数 are_our_namespaces(整数 pidns_fd, 整数 mntns_fd, 整数 netns_fd, 整数 userns_fd, 整数 root_fd);

整数 network_namespace_is_init(整数 socket_fd);
整数 namespace_is_init(NamespaceType type);

整数 pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
整数 in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

整数 namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

整数 detach_mount_namespace(void);
整数 detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
整数 detach_mount_namespace_userns(整数 userns_fd);

static inline bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        如果 (range <= 0)
                返回 false;

        如果 (shift > (uid_t) -1 - range)
                返回 false;

        返回 true;
}

整数 parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

整数 userns_acquire_empty(void);
整数 userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny);
整数 userns_acquire_self_root(void);
整数 userns_enter_and_pin(整数 userns_fd, PidRef *ret);
bool userns_supported(void);

整数 userns_get_base_uid(整数 userns_fd, uid_t *ret_uid, gid_t *ret_gid);
整数 userns_has_root_mapping(整数 userns_fd);

整数 namespace_open_by_id(uint64_t ns_id);

整数 process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

整数 is_idmapping_supported(const char *path);

整数 netns_acquire(void);
