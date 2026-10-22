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

цяло pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
цяло namespace_open_by_type(NamespaceType type);

цяло pidref_namespace_open(
                const PidRef *pidref,
                цяло *ret_pidns_fd,
                цяло *ret_mntns_fd,
                цяло *ret_netns_fd,
                цяло *ret_userns_fd,
                цяло *ret_root_fd);
цяло namespace_open(
                pid_t pid,
                цяло *ret_pidns_fd,
                цяло *ret_mntns_fd,
                цяло *ret_netns_fd,
                цяло *ret_userns_fd,
                цяло *ret_root_fd);

typedef enum NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
цяло namespace_enter_full(
                цяло pidns_fd,
                цяло mntns_fd,
                цяло netns_fd,
                цяло userns_fd,
                цяло root_fd,
                NamespaceEnterFlags flags);

static inline цяло namespace_enter(цяло pidns_fd, цяло mntns_fd, цяло netns_fd, цяло userns_fd, цяло root_fd) {
        върни namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

цяло fd_is_namespace(цяло fd, NamespaceType type);
цяло is_our_namespace(цяло fd, NamespaceType type);
цяло are_our_namespaces(цяло pidns_fd, цяло mntns_fd, цяло netns_fd, цяло userns_fd, цяло root_fd);

цяло network_namespace_is_init(цяло socket_fd);
цяло namespace_is_init(NamespaceType type);

цяло pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
цяло in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

цяло namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

цяло detach_mount_namespace(void);
цяло detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
цяло detach_mount_namespace_userns(цяло userns_fd);

static inline bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ако (range <= 0)
                върни false;

        ако (shift > (uid_t) -1 - range)
                върни false;

        върни true;
}

цяло parse_userns_uid_range(const char *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

цяло userns_acquire_empty(void);
цяло userns_acquire(const char *uid_map, const char *gid_map, bool setgroups_deny);
цяло userns_acquire_self_root(void);
цяло userns_enter_and_pin(цяло userns_fd, PidRef *ret);
bool userns_supported(void);

цяло userns_get_base_uid(цяло userns_fd, uid_t *ret_uid, gid_t *ret_gid);
цяло userns_has_root_mapping(цяло userns_fd);

цяло namespace_open_by_id(uint64_t ns_id);

цяло process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

цяло is_idmapping_supported(const char *path);

цяло netns_acquire(void);
