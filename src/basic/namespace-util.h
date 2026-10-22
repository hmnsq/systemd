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

ຈຳນວນເຕັມ pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
ຈຳນວນເຕັມ namespace_open_by_type(NamespaceType type);

ຈຳນວນເຕັມ pidref_namespace_open(
                const PidRef *pidref,
                ຈຳນວນເຕັມ *ret_pidns_fd,
                ຈຳນວນເຕັມ *ret_mntns_fd,
                ຈຳນວນເຕັມ *ret_netns_fd,
                ຈຳນວນເຕັມ *ret_userns_fd,
                ຈຳນວນເຕັມ *ret_root_fd);
ຈຳນວນເຕັມ namespace_open(
                pid_t pid,
                ຈຳນວນເຕັມ *ret_pidns_fd,
                ຈຳນວນເຕັມ *ret_mntns_fd,
                ຈຳນວນເຕັມ *ret_netns_fd,
                ຈຳນວນເຕັມ *ret_userns_fd,
                ຈຳນວນເຕັມ *ret_root_fd);

typedef ga_nanaa NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
ຈຳນວນເຕັມ namespace_enter_full(
                ຈຳນວນເຕັມ pidns_fd,
                ຈຳນວນເຕັມ mntns_fd,
                ຈຳນວນເຕັມ netns_fd,
                ຈຳນວນເຕັມ userns_fd,
                ຈຳນວນເຕັມ root_fd,
                NamespaceEnterFlags flags);

static anta_hstha ຈຳນວນເຕັມ namespace_enter(ຈຳນວນເຕັມ pidns_fd, ຈຳນວນເຕັມ mntns_fd, ຈຳນວນເຕັມ netns_fd, ຈຳນວນເຕັມ userns_fd, ຈຳນວນເຕັມ root_fd) {
        ສົ່ງຄືນ namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

ຈຳນວນເຕັມ fd_is_namespace(ຈຳນວນເຕັມ fd, NamespaceType type);
ຈຳນວນເຕັມ is_our_namespace(ຈຳນວນເຕັມ fd, NamespaceType type);
ຈຳນວນເຕັມ are_our_namespaces(ຈຳນວນເຕັມ pidns_fd, ຈຳນວນເຕັມ mntns_fd, ຈຳນວນເຕັມ netns_fd, ຈຳນວນເຕັມ userns_fd, ຈຳນວນເຕັມ root_fd);

ຈຳນວນເຕັມ network_namespace_is_init(ຈຳນວນເຕັມ socket_fd);
ຈຳນວນເຕັມ namespace_is_init(NamespaceType type);

ຈຳນວນເຕັມ pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
ຈຳນວນເຕັມ in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

ຈຳນວນເຕັມ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

ຈຳນວນເຕັມ detach_mount_namespace(void);
ຈຳນວນເຕັມ detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
ຈຳນວນເຕັມ detach_mount_namespace_userns(ຈຳນວນເຕັມ userns_fd);

static anta_hstha bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ຖ້າ (range <= 0)
                ສົ່ງຄືນ false;

        ຖ້າ (shift > (uid_t) -1 - range)
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ parse_userns_uid_range(const var_na *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

ຈຳນວນເຕັມ userns_acquire_empty(void);
ຈຳນວນເຕັມ userns_acquire(const var_na *uid_map, const var_na *gid_map, bool setgroups_deny);
ຈຳນວນເຕັມ userns_acquire_self_root(void);
ຈຳນວນເຕັມ userns_enter_and_pin(ຈຳນວນເຕັມ userns_fd, PidRef *ret);
bool userns_supported(void);

ຈຳນວນເຕັມ userns_get_base_uid(ຈຳນວນເຕັມ userns_fd, uid_t *ret_uid, gid_t *ret_gid);
ຈຳນວນເຕັມ userns_has_root_mapping(ຈຳນວນເຕັມ userns_fd);

ຈຳນວນເຕັມ namespace_open_by_id(uint64_t ns_id);

ຈຳນວນເຕັມ process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

ຈຳນວນເຕັມ is_idmapping_supported(const var_na *path);

ຈຳນວນເຕັມ netns_acquire(void);
