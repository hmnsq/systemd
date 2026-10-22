/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা NamespaceType {
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

বাহ্যিক ধ্রুবক কাঠামো namespace_info {
        ধ্রুবক অক্ষর *proc_name;
        ধ্রুবক অক্ষর *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t মূল_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

পূর্ণসংখ্যা pidref_namespace_open_by_type(ধ্রুবক PidRef *pidref, NamespaceType type);
পূর্ণসংখ্যা namespace_open_by_type(NamespaceType type);

পূর্ণসংখ্যা pidref_namespace_open(
                ধ্রুবক PidRef *pidref,
                পূর্ণসংখ্যা *ret_pidns_fd,
                পূর্ণসংখ্যা *ret_mntns_fd,
                পূর্ণসংখ্যা *ret_netns_fd,
                পূর্ণসংখ্যা *ret_userns_fd,
                পূর্ণসংখ্যা *ret_মূল_fd);
পূর্ণসংখ্যা namespace_open(
                pid_t pid,
                পূর্ণসংখ্যা *ret_pidns_fd,
                পূর্ণসংখ্যা *ret_mntns_fd,
                পূর্ণসংখ্যা *ret_netns_fd,
                পূর্ণসংখ্যা *ret_userns_fd,
                পূর্ণসংখ্যা *ret_মূল_fd);

প্রকারলেখ গণনা NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_মূল_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
পূর্ণসংখ্যা namespace_enter_full(
                পূর্ণসংখ্যা pidns_fd,
                পূর্ণসংখ্যা mntns_fd,
                পূর্ণসংখ্যা netns_fd,
                পূর্ণসংখ্যা userns_fd,
                পূর্ণসংখ্যা মূল_fd,
                NamespaceEnterFlags flags);

স্থির অন্তঃরেখ পূর্ণসংখ্যা namespace_enter(পূর্ণসংখ্যা pidns_fd, পূর্ণসংখ্যা mntns_fd, পূর্ণসংখ্যা netns_fd, পূর্ণসংখ্যা userns_fd, পূর্ণসংখ্যা মূল_fd) {
        ফেরত namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, মূল_fd, /* flags= */ 0);
}

পূর্ণসংখ্যা fd_is_namespace(পূর্ণসংখ্যা fd, NamespaceType type);
পূর্ণসংখ্যা is_our_namespace(পূর্ণসংখ্যা fd, NamespaceType type);
পূর্ণসংখ্যা are_our_namespaces(পূর্ণসংখ্যা pidns_fd, পূর্ণসংখ্যা mntns_fd, পূর্ণসংখ্যা netns_fd, পূর্ণসংখ্যা userns_fd, পূর্ণসংখ্যা মূল_fd);

পূর্ণসংখ্যা network_namespace_is_init(পূর্ণসংখ্যা socket_fd);
পূর্ণসংখ্যা namespace_is_init(NamespaceType type);

পূর্ণসংখ্যা pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
পূর্ণসংখ্যা in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

পূর্ণসংখ্যা namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

পূর্ণসংখ্যা detach_mount_namespace(শূন্য);
পূর্ণসংখ্যা detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
পূর্ণসংখ্যা detach_mount_namespace_userns(পূর্ণসংখ্যা userns_fd);

স্থির অন্তঃরেখ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        যদি (range <= 0)
                ফেরত false;

        যদি (shift > (uid_t) -1 - range)
                ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা parse_userns_uid_range(ধ্রুবক অক্ষর *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

পূর্ণসংখ্যা userns_acquire_empty(শূন্য);
পূর্ণসংখ্যা userns_acquire(ধ্রুবক অক্ষর *uid_map, ধ্রুবক অক্ষর *gid_map, bool setgroups_deny);
পূর্ণসংখ্যা userns_acquire_self_মূল(শূন্য);
পূর্ণসংখ্যা userns_enter_and_pin(পূর্ণসংখ্যা userns_fd, PidRef *ret);
bool userns_supported(শূন্য);

পূর্ণসংখ্যা userns_get_base_uid(পূর্ণসংখ্যা userns_fd, uid_t *ret_uid, gid_t *ret_gid);
পূর্ণসংখ্যা userns_has_মূল_mapping(পূর্ণসংখ্যা userns_fd);

পূর্ণসংখ্যা namespace_open_by_id(uint64_t ns_id);

পূর্ণসংখ্যা process_is_owned_by_uid(ধ্রুবক PidRef *pidref, uid_t uid);

পূর্ণসংখ্যা is_idmapping_supported(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা netns_acquire(শূন্য);
