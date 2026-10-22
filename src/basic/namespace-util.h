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

ကိန်းပြည့် pidref_namespace_open_by_type(const PidRef *pidref, NamespaceType type);
ကိန်းပြည့် namespace_open_by_type(NamespaceType type);

ကိန်းပြည့် pidref_namespace_open(
                const PidRef *pidref,
                ကိန်းပြည့် *ret_pidns_fd,
                ကိန်းပြည့် *ret_mntns_fd,
                ကိန်းပြည့် *ret_netns_fd,
                ကိန်းပြည့် *ret_userns_fd,
                ကိန်းပြည့် *ret_root_fd);
ကိန်းပြည့် namespace_open(
                pid_t pid,
                ကိန်းပြည့် *ret_pidns_fd,
                ကိန်းပြည့် *ret_mntns_fd,
                ကိန်းပြည့် *ret_netns_fd,
                ကိန်းပြည့် *ret_userns_fd,
                ကိန်းပြည့် *ret_root_fd);

typedef ga_nanaa NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ROOT_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
ကိန်းပြည့် namespace_enter_full(
                ကိန်းပြည့် pidns_fd,
                ကိန်းပြည့် mntns_fd,
                ကိန်းပြည့် netns_fd,
                ကိန်းပြည့် userns_fd,
                ကိန်းပြည့် root_fd,
                NamespaceEnterFlags flags);

static anta_hstha ကိန်းပြည့် namespace_enter(ကိန်းပြည့် pidns_fd, ကိန်းပြည့် mntns_fd, ကိန်းပြည့် netns_fd, ကိန်းပြည့် userns_fd, ကိန်းပြည့် root_fd) {
        ပြန်ပေး namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, root_fd, /* flags= */ 0);
}

ကိန်းပြည့် fd_is_namespace(ကိန်းပြည့် fd, NamespaceType type);
ကိန်းပြည့် is_our_namespace(ကိန်းပြည့် fd, NamespaceType type);
ကိန်းပြည့် are_our_namespaces(ကိန်းပြည့် pidns_fd, ကိန်းပြည့် mntns_fd, ကိန်းပြည့် netns_fd, ကိန်းပြည့် userns_fd, ကိန်းပြည့် root_fd);

ကိန်းပြည့် network_namespace_is_init(ကိန်းပြည့် socket_fd);
ကိန်းပြည့် namespace_is_init(NamespaceType type);

ကိန်းပြည့် pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
ကိန်းပြည့် in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

ကိန်းပြည့် namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

ကိန်းပြည့် detach_mount_namespace(void);
ကိန်းပြည့် detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
ကိန်းပြည့် detach_mount_namespace_userns(ကိန်းပြည့် userns_fd);

static anta_hstha bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        အကယ်၍ (range <= 0)
                ပြန်ပေး false;

        အကယ်၍ (shift > (uid_t) -1 - range)
                ပြန်ပေး false;

        ပြန်ပေး true;
}

ကိန်းပြည့် parse_userns_uid_range(const var_na *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

ကိန်းပြည့် userns_acquire_empty(void);
ကိန်းပြည့် userns_acquire(const var_na *uid_map, const var_na *gid_map, bool setgroups_deny);
ကိန်းပြည့် userns_acquire_self_root(void);
ကိန်းပြည့် userns_enter_and_pin(ကိန်းပြည့် userns_fd, PidRef *ret);
bool userns_supported(void);

ကိန်းပြည့် userns_get_base_uid(ကိန်းပြည့် userns_fd, uid_t *ret_uid, gid_t *ret_gid);
ကိန်းပြည့် userns_has_root_mapping(ကိန်းပြည့် userns_fd);

ကိန်းပြည့် namespace_open_by_id(uint64_t ns_id);

ကိန်းပြည့် process_is_owned_by_uid(const PidRef *pidref, uid_t uid);

ကိန်းပြည့် is_idmapping_supported(const var_na *path);

ကိန်းပြည့် netns_acquire(void);
