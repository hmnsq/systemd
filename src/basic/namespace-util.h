/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार क्रमागत NamespaceType {
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

बाह्य स्थिर काष्ठा namespace_info {
        स्थिर अक्षर *proc_name;
        स्थिर अक्षर *proc_path;
        अचिन्हित दीर्घ clone_flag;
        अचिन्हित दीर्घ pidfd_get_ns_ioctl_cmd;
        ino_t मूल_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_to_namespace_type(अचिन्हित दीर्घ clone_flag);

bool namespace_type_supported(NamespaceType type);

पूर्णांक pidref_namespace_open_by_type(स्थिर PidRef *pidref, NamespaceType type);
पूर्णांक namespace_open_by_type(NamespaceType type);

पूर्णांक pidref_namespace_open(
                स्थिर PidRef *pidref,
                पूर्णांक *ret_pidns_fd,
                पूर्णांक *ret_mntns_fd,
                पूर्णांक *ret_netns_fd,
                पूर्णांक *ret_userns_fd,
                पूर्णांक *ret_मूल_fd);
पूर्णांक namespace_open(
                pid_t pid,
                पूर्णांक *ret_pidns_fd,
                पूर्णांक *ret_mntns_fd,
                पूर्णांक *ret_netns_fd,
                पूर्णांक *ret_userns_fd,
                पूर्णांक *ret_मूल_fd);

प्रकार क्रमागत NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_मूल_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
पूर्णांक namespace_enter_full(
                पूर्णांक pidns_fd,
                पूर्णांक mntns_fd,
                पूर्णांक netns_fd,
                पूर्णांक userns_fd,
                पूर्णांक मूल_fd,
                NamespaceEnterFlags flags);

अटल अंतरभूत पूर्णांक namespace_enter(पूर्णांक pidns_fd, पूर्णांक mntns_fd, पूर्णांक netns_fd, पूर्णांक userns_fd, पूर्णांक मूल_fd) {
        वापस namespace_enter_full(pidns_fd, mntns_fd, netns_fd, userns_fd, मूल_fd, /* flags= */ 0);
}

पूर्णांक fd_is_namespace(पूर्णांक fd, NamespaceType type);
पूर्णांक is_our_namespace(पूर्णांक fd, NamespaceType type);
पूर्णांक are_our_namespaces(पूर्णांक pidns_fd, पूर्णांक mntns_fd, पूर्णांक netns_fd, पूर्णांक userns_fd, पूर्णांक मूल_fd);

पूर्णांक network_namespace_is_init(पूर्णांक socket_fd);
पूर्णांक namespace_is_init(NamespaceType type);

पूर्णांक pidref_in_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
पूर्णांक in_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

पूर्णांक namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

पूर्णांक detach_mount_namespace(व्योम);
पूर्णांक detach_mount_namespace_harder(uid_t target_uid, gid_t target_gid);
पूर्णांक detach_mount_namespace_userns(पूर्णांक userns_fd);

अटल अंतरभूत bool userns_shift_range_valid(uid_t shift, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        अगर (range <= 0)
                वापस false;

        अगर (shift > (uid_t) -1 - range)
                वापस false;

        वापस true;
}

पूर्णांक parse_userns_uid_range(स्थिर अक्षर *s, uid_t *ret_uid_shift, uid_t *ret_uid_range);

पूर्णांक userns_acquire_empty(व्योम);
पूर्णांक userns_acquire(स्थिर अक्षर *uid_map, स्थिर अक्षर *gid_map, bool setgroups_deny);
पूर्णांक userns_acquire_self_मूल(व्योम);
पूर्णांक userns_enter_and_pin(पूर्णांक userns_fd, PidRef *ret);
bool userns_supported(व्योम);

पूर्णांक userns_get_base_uid(पूर्णांक userns_fd, uid_t *ret_uid, gid_t *ret_gid);
पूर्णांक userns_has_मूल_mapping(पूर्णांक userns_fd);

पूर्णांक namespace_open_by_id(uint64_t ns_id);

पूर्णांक process_is_owned_by_uid(स्थिर PidRef *pidref, uid_t uid);

पूर्णांक is_idmapping_supported(स्थिर अक्षर *path);

पूर्णांक netns_acquire(व्योम);
