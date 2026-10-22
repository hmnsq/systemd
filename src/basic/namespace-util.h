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

정수 pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
정수 namespace_open_by_type(NamespaceType type);

정수 pidref_namespace_open(
                const PidRef *pidref,
                정수 *ret_pidns_fd,
                정수 *ret_mntns_fd,
                정수 *ret_netns_fd,
                정수 *ret_userns_fd,
                정수 *ret_root_fd);
정수 namespace_open(
                pid_t pid,
                정수 *ret_pidns_fd,
                정수 *ret_mntns_fd,
                정수 *ret_netns_fd,
                정수 *ret_userns_fd,
                정수 *ret_root_fd);

typedef enum NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
정수 namespace_enter_full(
                정수 pidns_fd,
                정수 mntns_fd,
                정수 netns_fd,
                정수 userns_fd,
                정수 root_fd,
                NamespaceEnterFlags flags);

static inline 정수 namespace_enter(정수 pidns_fd, 정수 mntns_fd, 정수 netns_fd, 정수 userns_fd, 정수 root_fd) {
        반환 namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

정수 fd_is_namespace(정수 fd, NamespaceType type);
정수 is_our_namespace(정수 fd, NamespaceType type);
정수 are_our_namespaces(정수 pidns_fd, 정수 mntns_fd, 정수 netns_fd, 정수 userns_fd, 정수 root_fd);

정수 network_namespace_is_init(정수 socket_fd);
정수 namespace_is_init(NamespaceType type);

정수 pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
정수 in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

정수 namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

정수 detach_mount_namespace(void);
정수 detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
정수 detach_mount_namespace_userns(정수 userns_fd);

static inline bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        만약 (range <= 0)
                반환 false;

        만약 (shift > (uid_t) -1 - range)
                반환 false;

        반환 true;
}

정수 parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

정수 userns_acquire_empty(void);
정수 userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny);
정수 userns_acquire_self_root(void);
정수 userns_enter_and_pin(정수 userns_fd, PidRef *ret);
bool userns_supported(void);

정수 userns_get_base_uid(정수 userns_fd, uid_t *ret_uid, gid_t *ret_gid);
정수 userns_has_root_mapping(정수 userns_fd);

정수 namespace_open_by_id(uint64_t ns_id);

정수 process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

정수 is_idmapping_supported(const char *path);

정수 netns_acquire(void);
