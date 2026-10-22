/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ গণনা NamespaceType {
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

বাহ্য ধ্রুব সংরচনা namespace_info {
        ধ্রুব বর্ণ *proc_name;
        ধ্রুব বর্ণ *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t মূল_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

পূর্ণাঙ্ক pidref_namespace_open_by_type(ধ্রুব PidRef *pidref, NamespaceType type);
পূর্ণাঙ্ক namespace_open_by_type(NamespaceType type);

পূর্ণাঙ্ক pidref_namespace_open(
                ধ্রুব PidRef *pidref,
                পূর্ণাঙ্ক *ret_pidns_fd,
                পূর্ণাঙ্ক *ret_mntns_fd,
                পূর্ণাঙ্ক *ret_netns_fd,
                পূর্ণাঙ্ক *ret_userns_fd,
                পূর্ণাঙ্ক *ret_মূল_fd);
পূর্ণাঙ্ক namespace_open(
                pid_t pid,
                পূর্ণাঙ্ক *ret_pidns_fd,
                পূর্ণাঙ্ক *ret_mntns_fd,
                পূর্ণাঙ্ক *ret_netns_fd,
                পূর্ণাঙ্ক *ret_userns_fd,
                পূর্ণাঙ্ক *ret_মূল_fd);

প্রকারনির্দেশ গণনা NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_মূল_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
পূর্ণাঙ্ক namespace_enter_full(
                পূর্ণাঙ্ক pidns_fd,
                পূর্ণাঙ্ক mntns_fd,
                পূর্ণাঙ্ক netns_fd,
                পূর্ণাঙ্ক userns_fd,
                পূর্ণাঙ্ক মূল_fd,
                NamespaceEnterFlags flags);

স্থির অন্তঃস্থ পূর্ণাঙ্ক namespace_enter(পূর্ণাঙ্ক pidns_fd, পূর্ণাঙ্ক mntns_fd, পূর্ণাঙ্ক netns_fd, পূর্ণাঙ্ক userns_fd, পূর্ণাঙ্ক মূল_fd) {
        প্রত্যাগম namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, মূল_fd, /* flags= */ 0);
}

পূর্ণাঙ্ক fd_is_namespace(পূর্ণাঙ্ক fd, NamespaceType type);
পূর্ণাঙ্ক is_our_namespace(পূর্ণাঙ্ক fd, NamespaceType type);
পূর্ণাঙ্ক are_our_namespaces(পূর্ণাঙ্ক pidns_fd, পূর্ণাঙ্ক mntns_fd, পূর্ণাঙ্ক netns_fd, পূর্ণাঙ্ক userns_fd, পূর্ণাঙ্ক মূল_fd);

পূর্ণাঙ্ক network_namespace_is_init(পূর্ণাঙ্ক socket_fd);
পূর্ণাঙ্ক namespace_is_init(NamespaceType type);

পূর্ণাঙ্ক pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
পূর্ণাঙ্ক in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

পূর্ণাঙ্ক namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

পূর্ণাঙ্ক detach_mount_namespace(শূন্য);
পূর্ণাঙ্ক detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
পূর্ণাঙ্ক detach_mount_namespace_userns(পূর্ণাঙ্ক userns_fd);

স্থির অন্তঃস্থ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        যদি (range <= 0)
                প্রত্যাগম false;

        যদি (shift > (uid_t) -1 - range)
                প্রত্যাগম false;

        প্রত্যাগম true;
}

পূর্ণাঙ্ক parse_userns_uid_range(ধ্রুব বর্ণ *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

পূর্ণাঙ্ক userns_acquire_empty(শূন্য);
পূর্ণাঙ্ক userns_acquire(ধ্রুব বর্ণ *uid_map, ধ্রুব বর্ণ *gid_map, bool setgroups_deny);
পূর্ণাঙ্ক userns_acquire_self_মূল(শূন্য);
পূর্ণাঙ্ক userns_enter_and_pin(পূর্ণাঙ্ক userns_fd, PidRef *ret);
bool userns_supported(শূন্য);

পূর্ণাঙ্ক userns_get_base_uid(পূর্ণাঙ্ক userns_fd, uid_t *ret_uid, gid_t *ret_gid);
পূর্ণাঙ্ক userns_has_মূল_mapping(পূর্ণাঙ্ক userns_fd);

পূর্ণাঙ্ক namespace_open_by_id(uint64_t ns_id);

পূর্ণাঙ্ক process_is_owned_by_uid(ধ্রুব PidRef *pidref, uid_t uid);

পূর্ণাঙ্ক is_idmapping_supported(ধ্রুব বর্ণ *path);

পূর্ণাঙ্ক netns_acquire(শূন্য);
