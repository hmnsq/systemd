/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ NamespaceType {
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

ബാഹ്യ സ്ഥിരം ഘടന namespace_info {
        സ്ഥിരം അക്ഷരം *proc_name;
        സ്ഥിരം അക്ഷരം *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t വേര്_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

പൂർണ്ണസംഖ്യ pidref_namespace_open_by_type(സ്ഥിരം PidRef *pidref, NamespaceType type);
പൂർണ്ണസംഖ്യ namespace_open_by_type(NamespaceType type);

പൂർണ്ണസംഖ്യ pidref_namespace_open(
                സ്ഥിരം PidRef *pidref,
                പൂർണ്ണസംഖ്യ *ret_pidns_fd,
                പൂർണ്ണസംഖ്യ *ret_mntns_fd,
                പൂർണ്ണസംഖ്യ *ret_netns_fd,
                പൂർണ്ണസംഖ്യ *ret_userns_fd,
                പൂർണ്ണസംഖ്യ *ret_വേര്_fd);
പൂർണ്ണസംഖ്യ namespace_open(
                pid_t pid,
                പൂർണ്ണസംഖ്യ *ret_pidns_fd,
                പൂർണ്ണസംഖ്യ *ret_mntns_fd,
                പൂർണ്ണസംഖ്യ *ret_netns_fd,
                പൂർണ്ണസംഖ്യ *ret_userns_fd,
                പൂർണ്ണസംഖ്യ *ret_വേര്_fd);

തരനിർവചനം എണ്ണൽ NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_വേര്_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
പൂർണ്ണസംഖ്യ namespace_enter_full(
                പൂർണ്ണസംഖ്യ pidns_fd,
                പൂർണ്ണസംഖ്യ mntns_fd,
                പൂർണ്ണസംഖ്യ netns_fd,
                പൂർണ്ണസംഖ്യ userns_fd,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                NamespaceEnterFlags flags);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ namespace_enter(പൂർണ്ണസംഖ്യ pidns_fd, പൂർണ്ണസംഖ്യ mntns_fd, പൂർണ്ണസംഖ്യ netns_fd, പൂർണ്ണസംഖ്യ userns_fd, പൂർണ്ണസംഖ്യ വേര്_fd) {
        തിരികെ namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, വേര്_fd, /* flags= */ 0);
}

പൂർണ്ണസംഖ്യ fd_is_namespace(പൂർണ്ണസംഖ്യ fd, NamespaceType type);
പൂർണ്ണസംഖ്യ is_our_namespace(പൂർണ്ണസംഖ്യ fd, NamespaceType type);
പൂർണ്ണസംഖ്യ are_our_namespaces(പൂർണ്ണസംഖ്യ pidns_fd, പൂർണ്ണസംഖ്യ mntns_fd, പൂർണ്ണസംഖ്യ netns_fd, പൂർണ്ണസംഖ്യ userns_fd, പൂർണ്ണസംഖ്യ വേര്_fd);

പൂർണ്ണസംഖ്യ network_namespace_is_init(പൂർണ്ണസംഖ്യ socket_fd);
പൂർണ്ണസംഖ്യ namespace_is_init(NamespaceType type);

പൂർണ്ണസംഖ്യ pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
പൂർണ്ണസംഖ്യ in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

പൂർണ്ണസംഖ്യ namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

പൂർണ്ണസംഖ്യ detach_mount_namespace(ശൂന്യം);
പൂർണ്ണസംഖ്യ detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
പൂർണ്ണസംഖ്യ detach_mount_namespace_userns(പൂർണ്ണസംഖ്യ userns_fd);

സ്ഥിതം വരിയിൽ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        എങ്കിൽ (range <= 0)
                തിരികെ false;

        എങ്കിൽ (shift > (uid_t) -1 - range)
                തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ parse_userns_uid_range(സ്ഥിരം അക്ഷരം *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

പൂർണ്ണസംഖ്യ userns_acquire_empty(ശൂന്യം);
പൂർണ്ണസംഖ്യ userns_acquire(സ്ഥിരം അക്ഷരം *uid_map, സ്ഥിരം അക്ഷരം *gid_map, bool setgroups_deny);
പൂർണ്ണസംഖ്യ userns_acquire_self_വേര്(ശൂന്യം);
പൂർണ്ണസംഖ്യ userns_enter_and_pin(പൂർണ്ണസംഖ്യ userns_fd, PidRef *ret);
bool userns_supported(ശൂന്യം);

പൂർണ്ണസംഖ്യ userns_get_base_uid(പൂർണ്ണസംഖ്യ userns_fd, uid_t *ret_uid, gid_t *ret_gid);
പൂർണ്ണസംഖ്യ userns_has_വേര്_mapping(പൂർണ്ണസംഖ്യ userns_fd);

പൂർണ്ണസംഖ്യ namespace_open_by_id(uint64_t ns_id);

പൂർണ്ണസംഖ്യ process_is_owned_by_uid(സ്ഥിരം PidRef *pidref, uid_t uid);

പൂർണ്ണസംഖ്യ is_idmapping_supported(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ netns_acquire(ശൂന്യം);
