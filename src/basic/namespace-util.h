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

puur_naa_nka pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
puur_naa_nka namespace_open_by_type(NamespaceType type);

puur_naa_nka pidref_namespace_open(
                const PidRef *pidref,
                puur_naa_nka *ret_pidns_fd,
                puur_naa_nka *ret_mntns_fd,
                puur_naa_nka *ret_netns_fd,
                puur_naa_nka *ret_userns_fd,
                puur_naa_nka *ret_root_fd);
puur_naa_nka namespace_open(
                pid_t pid,
                puur_naa_nka *ret_pidns_fd,
                puur_naa_nka *ret_mntns_fd,
                puur_naa_nka *ret_netns_fd,
                puur_naa_nka *ret_userns_fd,
                puur_naa_nka *ret_root_fd);

typedef ga_nanaa NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
puur_naa_nka namespace_enter_full(
                puur_naa_nka pidns_fd,
                puur_naa_nka mntns_fd,
                puur_naa_nka netns_fd,
                puur_naa_nka userns_fd,
                puur_naa_nka root_fd,
                NamespaceEnterFlags flags);

static anta_hstha puur_naa_nka namespace_enter(puur_naa_nka pidns_fd, puur_naa_nka mntns_fd, puur_naa_nka netns_fd, puur_naa_nka userns_fd, puur_naa_nka root_fd) {
        ပွၵ်ႈ namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

puur_naa_nka fd_is_namespace(puur_naa_nka fd, NamespaceType type);
puur_naa_nka is_our_namespace(puur_naa_nka fd, NamespaceType type);
puur_naa_nka are_our_namespaces(puur_naa_nka pidns_fd, puur_naa_nka mntns_fd, puur_naa_nka netns_fd, puur_naa_nka userns_fd, puur_naa_nka root_fd);

puur_naa_nka network_namespace_is_init(puur_naa_nka socket_fd);
puur_naa_nka namespace_is_init(NamespaceType type);

puur_naa_nka pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
puur_naa_nka in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

puur_naa_nka namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

puur_naa_nka detach_mount_namespace(void);
puur_naa_nka detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
puur_naa_nka detach_mount_namespace_userns(puur_naa_nka userns_fd);

static anta_hstha bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ပေႃး (range <= 0)
                ပွၵ်ႈ false;

        ပေႃး (shift > (uid_t) -1 - range)
                ပွၵ်ႈ false;

        ပွၵ်ႈ true;
}

puur_naa_nka parse_userns_uid_range(const var_na *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

puur_naa_nka userns_acquire_empty(void);
puur_naa_nka userns_acquire(const var_na *uid_map, const var_na *gid_map, bool setgroups_deny);
puur_naa_nka userns_acquire_self_root(void);
puur_naa_nka userns_enter_and_pin(puur_naa_nka userns_fd, PidRef *ret);
bool userns_supported(void);

puur_naa_nka userns_get_base_uid(puur_naa_nka userns_fd, uid_t *ret_uid, gid_t *ret_gid);
puur_naa_nka userns_has_root_mapping(puur_naa_nka userns_fd);

puur_naa_nka namespace_open_by_id(uint64_t ns_id);

puur_naa_nka process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

puur_naa_nka is_idmapping_supported(const var_na *path);

puur_naa_nka netns_acquire(void);
