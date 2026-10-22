/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના NamespaceType {
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

બાહ્ય અચલ સંરચના namespace_info {
        અચલ અક્ષર *proc_name;
        અચલ અક્ષર *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t મૂળ_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

પૂર્ણાંક pidref_namespace_open_by_type(અચલ PidRef *pidref, NamespaceType type);
પૂર્ણાંક namespace_open_by_type(NamespaceType type);

પૂર્ણાંક pidref_namespace_open(
                અચલ PidRef *pidref,
                પૂર્ણાંક *ret_pidns_fd,
                પૂર્ણાંક *ret_mntns_fd,
                પૂર્ણાંક *ret_netns_fd,
                પૂર્ણાંક *ret_userns_fd,
                પૂર્ણાંક *ret_મૂળ_fd);
પૂર્ણાંક namespace_open(
                pid_t pid,
                પૂર્ણાંક *ret_pidns_fd,
                પૂર્ણાંક *ret_mntns_fd,
                પૂર્ણાંક *ret_netns_fd,
                પૂર્ણાંક *ret_userns_fd,
                પૂર્ણાંક *ret_મૂળ_fd);

પ્રકારવ્યાખ્યા ગણના NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_મૂળ_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
પૂર્ણાંક namespace_enter_full(
                પૂર્ણાંક pidns_fd,
                પૂર્ણાંક mntns_fd,
                પૂર્ણાંક netns_fd,
                પૂર્ણાંક userns_fd,
                પૂર્ણાંક મૂળ_fd,
                NamespaceEnterFlags flags);

સ્થિર અંતઃરેખ પૂર્ણાંક namespace_enter(પૂર્ણાંક pidns_fd, પૂર્ણાંક mntns_fd, પૂર્ણાંક netns_fd, પૂર્ણાંક userns_fd, પૂર્ણાંક મૂળ_fd) {
        પાછા namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, મૂળ_fd, /* flags= */ 0);
}

પૂર્ણાંક fd_is_namespace(પૂર્ણાંક fd, NamespaceType type);
પૂર્ણાંક is_our_namespace(પૂર્ણાંક fd, NamespaceType type);
પૂર્ણાંક are_our_namespaces(પૂર્ણાંક pidns_fd, પૂર્ણાંક mntns_fd, પૂર્ણાંક netns_fd, પૂર્ણાંક userns_fd, પૂર્ણાંક મૂળ_fd);

પૂર્ણાંક network_namespace_is_init(પૂર્ણાંક socket_fd);
પૂર્ણાંક namespace_is_init(NamespaceType type);

પૂર્ણાંક pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
પૂર્ણાંક in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

પૂર્ણાંક namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

પૂર્ણાંક detach_mount_namespace(શૂન્ય);
પૂર્ણાંક detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
પૂર્ણાંક detach_mount_namespace_userns(પૂર્ણાંક userns_fd);

સ્થિર અંતઃરેખ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        જો (range <= 0)
                પાછા false;

        જો (shift > (uid_t) -1 - range)
                પાછા false;

        પાછા true;
}

પૂર્ણાંક parse_userns_uid_range(અચલ અક્ષર *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

પૂર્ણાંક userns_acquire_empty(શૂન્ય);
પૂર્ણાંક userns_acquire(અચલ અક્ષર *uid_map, અચલ અક્ષર *gid_map, bool setgroups_deny);
પૂર્ણાંક userns_acquire_self_મૂળ(શૂન્ય);
પૂર્ણાંક userns_enter_and_pin(પૂર્ણાંક userns_fd, PidRef *ret);
bool userns_supported(શૂન્ય);

પૂર્ણાંક userns_get_base_uid(પૂર્ણાંક userns_fd, uid_t *ret_uid, gid_t *ret_gid);
પૂર્ણાંક userns_has_મૂળ_mapping(પૂર્ણાંક userns_fd);

પૂર્ણાંક namespace_open_by_id(uint64_t ns_id);

પૂર્ણાંક process_is_owned_by_uid(અચલ PidRef *pidref, uid_t uid);

પૂર્ણાંક is_idmapping_supported(અચલ અક્ષર *path);

પૂર્ણાંક netns_acquire(શૂન્ય);
