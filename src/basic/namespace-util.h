/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع شمارش NamespaceType {
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

بیرونی ثابت ساختار namespace_info {
        ثابت نویسه *proc_name;
        ثابت نویسه *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t ریشه_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

صحیح pidref_namespace_open_by_type(ثابت PidRef *pidref, NamespaceType type);
صحیح namespace_open_by_type(NamespaceType type);

صحیح pidref_namespace_open(
                ثابت PidRef *pidref,
                صحیح *ret_pidns_fd,
                صحیح *ret_mntns_fd,
                صحیح *ret_netns_fd,
                صحیح *ret_userns_fd,
                صحیح *ret_ریشه_fd);
صحیح namespace_open(
                pid_t pid,
                صحیح *ret_pidns_fd,
                صحیح *ret_mntns_fd,
                صحیح *ret_netns_fd,
                صحیح *ret_userns_fd,
                صحیح *ret_ریشه_fd);

تعریفنوع شمارش NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ریشه_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
صحیح namespace_enter_full(
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح ریشه_fd,
                NamespaceEnterFlags flags);

ایستا درونخطی صحیح namespace_enter(صحیح pidns_fd, صحیح mntns_fd, صحیح netns_fd, صحیح userns_fd, صحیح ریشه_fd) {
        بازگشت namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, ریشه_fd, /* flags= */ 0);
}

صحیح fd_is_namespace(صحیح fd, NamespaceType type);
صحیح is_our_namespace(صحیح fd, NamespaceType type);
صحیح are_our_namespaces(صحیح pidns_fd, صحیح mntns_fd, صحیح netns_fd, صحیح userns_fd, صحیح ریشه_fd);

صحیح network_namespace_is_init(صحیح socket_fd);
صحیح namespace_is_init(NamespaceType type);

صحیح pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
صحیح in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

صحیح namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

صحیح detach_mount_namespace(تهی);
صحیح detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
صحیح detach_mount_namespace_userns(صحیح userns_fd);

ایستا درونخطی bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        اگر (range <= 0)
                بازگشت false;

        اگر (shift > (uid_t) -1 - range)
                بازگشت false;

        بازگشت true;
}

صحیح parse_userns_uid_range(ثابت نویسه *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

صحیح userns_acquire_empty(تهی);
صحیح userns_acquire(ثابت نویسه *uid_map, ثابت نویسه *gid_map, bool setgroups_deny);
صحیح userns_acquire_self_ریشه(تهی);
صحیح userns_enter_and_pin(صحیح userns_fd, PidRef *ret);
bool userns_supported(تهی);

صحیح userns_get_base_uid(صحیح userns_fd, uid_t *ret_uid, gid_t *ret_gid);
صحیح userns_has_ریشه_mapping(صحیح userns_fd);

صحیح namespace_open_by_id(uint64_t ns_id);

صحیح process_is_owned_by_uid(ثابت PidRef *pidref, uid_t uid);

صحیح is_idmapping_supported(ثابت نویسه *path);

صحیح netns_acquire(تهی);
