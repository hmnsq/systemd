/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع تعداد NamespaceType {
        NAMESPACE_CGROUP,
        NAMESPACE_IPC,
        NAMESPACE_NET,
        NAMESPACE_MOUNT,
        NAMESPACE_PID,
        NAMESPACE_USER,
        NAMESPACE_UTS,
        NAMESPACE_وخت,
        _NAMESPACE_TYPE_MAX,
        _NAMESPACE_TYPE_INVALID = -EINVAL,
} NamespaceType;

خارجي ثابت بنية namespace_info {
        ثابت حرف *proc_name;
        ثابت حرف *proc_لار;
        بینښه اوږد clone_flag;
        بینښه اوږد pidfd_get_ns_ioctl_cmd;
        ino_t ریښه_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_منزل_namespace_type(بینښه اوږد clone_flag);

bool namespace_type_supported(NamespaceType type);

صحيح pidref_namespace_open_by_type(ثابت PidRef *pidref, NamespaceType type);
صحيح namespace_open_by_type(NamespaceType type);

صحيح pidref_namespace_open(
                ثابت PidRef *pidref,
                صحيح *ret_pidns_fd,
                صحيح *ret_mntns_fd,
                صحيح *ret_netns_fd,
                صحيح *ret_userns_fd,
                صحيح *ret_ریښه_fd);
صحيح namespace_open(
                pid_t pid,
                صحيح *ret_pidns_fd,
                صحيح *ret_mntns_fd,
                صحيح *ret_netns_fd,
                صحيح *ret_userns_fd,
                صحيح *ret_ریښه_fd);

تعريفنوع تعداد NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ریښه_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
صحيح namespace_enter_ډک(
                صحيح pidns_fd,
                صحيح mntns_fd,
                صحيح netns_fd,
                صحيح userns_fd,
                صحيح ریښه_fd,
                NamespaceEnterFlags flags);

ساكن مضمن صحيح namespace_enter(صحيح pidns_fd, صحيح mntns_fd, صحيح netns_fd, صحيح userns_fd, صحيح ریښه_fd) {
        أرجع namespace_enter_ډک(pidns_fd, mntns_fd, netns_fd, userns_fd, ریښه_fd, /* flags= */ 0);
}

صحيح fd_is_namespace(صحيح fd, NamespaceType type);
صحيح is_our_namespace(صحيح fd, NamespaceType type);
صحيح are_our_namespaces(صحيح pidns_fd, صحيح mntns_fd, صحيح netns_fd, صحيح userns_fd, صحيح ریښه_fd);

صحيح شبکه_namespace_is_init(صحيح socket_fd);
صحيح namespace_is_init(NamespaceType type);

صحيح pidref_دننه_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
صحيح دننه_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

صحيح namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

صحيح detach_mount_namespace(فراغ);
صحيح detach_mount_namespace_harder(uid_t موخه_uid, gid_t موخه_gid);
صحيح detach_mount_namespace_userns(صحيح userns_fd);

ساكن مضمن bool userns_ښويدنه_range_valid(uid_t ښويدنه, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        إذا (range <= 0)
                أرجع false;

        إذا (ښويدنه > (uid_t) -1 - range)
                أرجع false;

        أرجع true;
}

صحيح parse_userns_uid_range(ثابت حرف *s, uid_t *ret_uid_ښويدنه, uid_t *ret_uid_range);

صحيح userns_acquire_تشه(فراغ);
صحيح userns_acquire(ثابت حرف *uid_map, ثابت حرف *gid_map, bool setgroups_deny);
صحيح userns_acquire_self_ریښه(فراغ);
صحيح userns_enter_and_pin(صحيح userns_fd, PidRef *ret);
bool userns_supported(فراغ);

صحيح userns_get_بنسټ_uid(صحيح userns_fd, uid_t *ret_uid, gid_t *ret_gid);
صحيح userns_has_ریښه_mapping(صحيح userns_fd);

صحيح namespace_open_by_id(uint64_t ns_id);

صحيح process_is_owned_by_uid(ثابت PidRef *pidref, uid_t uid);

صحيح is_idmapping_supported(ثابت حرف *لار);

صحيح netns_acquire(فراغ);
