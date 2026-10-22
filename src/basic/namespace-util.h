/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना NamespaceType {
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

बाह्य अचर संरचना namespace_info {
        अचर वर्ण *proc_name;
        अचर वर्ण *proc_path;
        अचिन्हित दीर्घ clone_flag;
        अचिन्हित दीर्घ pidfd_get_ns_ioctl_cmd;
        ino_t जरा_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(अचिन्हित दीर्घ clone_flag);

bool namespace_type_supported(NamespaceType type);

पूर्णाङ्क pidref_namespace_open_by_type(अचर PidRef *pidref, NamespaceType type);
पूर्णाङ्क namespace_open_by_type(NamespaceType type);

पूर्णाङ्क pidref_namespace_open(
                अचर PidRef *pidref,
                पूर्णाङ्क *ret_pidns_fd,
                पूर्णाङ्क *ret_mntns_fd,
                पूर्णाङ्क *ret_netns_fd,
                पूर्णाङ्क *ret_userns_fd,
                पूर्णाङ्क *ret_जरा_fd);
पूर्णाङ्क namespace_open(
                pid_t pid,
                पूर्णाङ्क *ret_pidns_fd,
                पूर्णाङ्क *ret_mntns_fd,
                पूर्णाङ्क *ret_netns_fd,
                पूर्णाङ्क *ret_userns_fd,
                पूर्णाङ्क *ret_जरा_fd);

प्रकारनिर्देश गणना NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_जरा_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
पूर्णाङ्क namespace_enter_full(
                पूर्णाङ्क pidns_fd,
                पूर्णाङ्क mntns_fd,
                पूर्णाङ्क netns_fd,
                पूर्णाङ्क userns_fd,
                पूर्णाङ्क जरा_fd,
                NamespaceEnterFlags flags);

स्थिर अन्तःस्थ पूर्णाङ्क namespace_enter(पूर्णाङ्क pidns_fd, पूर्णाङ्क mntns_fd, पूर्णाङ्क netns_fd, पूर्णाङ्क userns_fd, पूर्णाङ्क जरा_fd) {
        फर्काऊ namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, जरा_fd, /* flags= */ 0);
}

पूर्णाङ्क fd_is_namespace(पूर्णाङ्क fd, NamespaceType type);
पूर्णाङ्क is_our_namespace(पूर्णाङ्क fd, NamespaceType type);
पूर्णाङ्क are_our_namespaces(पूर्णाङ्क pidns_fd, पूर्णाङ्क mntns_fd, पूर्णाङ्क netns_fd, पूर्णाङ्क userns_fd, पूर्णाङ्क जरा_fd);

पूर्णाङ्क network_namespace_is_init(पूर्णाङ्क socket_fd);
पूर्णाङ्क namespace_is_init(NamespaceType type);

पूर्णाङ्क pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
पूर्णाङ्क in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

पूर्णाङ्क namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

पूर्णाङ्क detach_mount_namespace(शून्य);
पूर्णाङ्क detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
पूर्णाङ्क detach_mount_namespace_userns(पूर्णाङ्क userns_fd);

स्थिर अन्तःस्थ bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        यदि (range <= 0)
                फर्काऊ false;

        यदि (shift > (uid_t) -1 - range)
                फर्काऊ false;

        फर्काऊ true;
}

पूर्णाङ्क parse_userns_uid_range(अचर वर्ण *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

पूर्णाङ्क userns_acquire_empty(शून्य);
पूर्णाङ्क userns_acquire(अचर वर्ण *uid_map, अचर वर्ण *gid_map, bool setgroups_deny);
पूर्णाङ्क userns_acquire_self_जरा(शून्य);
पूर्णाङ्क userns_enter_and_pin(पूर्णाङ्क userns_fd, PidRef *ret);
bool userns_supported(शून्य);

पूर्णाङ्क userns_get_base_uid(पूर्णाङ्क userns_fd, uid_t *ret_uid, gid_t *ret_gid);
पूर्णाङ्क userns_has_जरा_mapping(पूर्णाङ्क userns_fd);

पूर्णाङ्क namespace_open_by_id(uint64_t ns_id);

पूर्णाङ्क process_is_owned_by_uid(अचर PidRef *pidref, uid_t uid);

पूर्णाङ्क is_idmapping_supported(अचर वर्ण *path);

पूर्णाङ्क netns_acquire(शून्य);
