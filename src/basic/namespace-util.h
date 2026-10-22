/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା NamespaceType {
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

ବାହ୍ୟ ଅଚଳ ସଂରଚନା namespace_info {
        ଅଚଳ ଅକ୍ଷର *proc_name;
        ଅଚଳ ଅକ୍ଷର *proc_path;
        unsigned long clone_flag;
        unsigned long pidfd_get_ns_ioctl_cmd;
        ino_t ମୂଳ_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(unsigned long clone_flag);

bool namespace_type_supported(NamespaceType type);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_namespace_open_by_type(ଅଚଳ PidRef *pidref, NamespaceType type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_open_by_type(NamespaceType type);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_namespace_open(
                ଅଚଳ PidRef *pidref,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_ମୂଳ_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_open(
                pid_t pid,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_ମୂଳ_fd);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_ମୂଳ_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_enter_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                NamespaceEnterFlags flags);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_enter(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd) {
        ଫେରନ୍ତୁ namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, ମୂଳ_fd, /* flags= */ 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, NamespaceType type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_our_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, NamespaceType type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା are_our_namespaces(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mntns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା network_namespace_is_init(ପୂର୍ଣ୍ଣସଂଖ୍ୟା socket_fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_is_init(NamespaceType type);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା detach_mount_namespace(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା detach_mount_namespace_userns(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ଯଦି (range <= 0)
                ଫେରନ୍ତୁ false;

        ଯଦି (shift > (uid_t) -1 - range)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_userns_uid_range(ଅଚଳ ଅକ୍ଷର *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_acquire_empty(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_acquire(ଅଚଳ ଅକ୍ଷର *uid_map, ଅଚଳ ଅକ୍ଷର *gid_map, bool setgroups_deny);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_acquire_self_ମୂଳ(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_enter_and_pin(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, PidRef *ret);
bool userns_supported(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_get_base_uid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, uid_t *ret_uid, gid_t *ret_gid);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_has_ମୂଳ_mapping(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା namespace_open_by_id(uint64_t ns_id);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା process_is_owned_by_uid(ଅଚଳ PidRef *pidref, uid_t uid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_idmapping_supported(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା netns_acquire(ଶୂନ୍ୟ);
