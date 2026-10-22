/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار NamespaceType {
        NAMESPACE_CGROUP,
        NAMESPACE_IPC,
        NAMESPACE_NET,
        NAMESPACE_MOUNT,
        NAMESPACE_PID,
        NAMESPACE_USER,
        NAMESPACE_UTS,
        NAMESPACE_وقت,
        _NAMESPACE_TYPE_MAX,
        _NAMESPACE_TYPE_INVALID = -EINVAL,
} NamespaceType;

خارجی ثابت ساخت namespace_info {
        ثابت حرف *proc_name;
        ثابت حرف *proc_راستہ;
        غیرعلامتی طویل clone_flag;
        غیرعلامتی طویل pidfd_get_ns_ioctl_cmd;
        ino_t جڑ_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_مقصد_namespace_type(غیرعلامتی طویل clone_flag);

bool namespace_type_supported(NamespaceType type);

صحیح pidref_namespace_open_by_type(ثابت PidRef *pidref, NamespaceType type);
صحیح namespace_open_by_type(NamespaceType type);

صحیح pidref_namespace_open(
                ثابت PidRef *pidref,
                صحیح *ret_pidns_fd,
                صحیح *ret_mntns_fd,
                صحیح *ret_netns_fd,
                صحیح *ret_userns_fd,
                صحیح *ret_جڑ_fd);
صحیح namespace_open(
                pid_t pid,
                صحیح *ret_pidns_fd,
                صحیح *ret_mntns_fd,
                صحیح *ret_netns_fd,
                صحیح *ret_userns_fd,
                صحیح *ret_جڑ_fd);

قسمتعریف شمار NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_جڑ_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
صحیح namespace_enter_بھرا(
                صحیح pidns_fd,
                صحیح mntns_fd,
                صحیح netns_fd,
                صحیح userns_fd,
                صحیح جڑ_fd,
                NamespaceEnterFlags flags);

ساکن اندرونی صحیح namespace_enter(صحیح pidns_fd, صحیح mntns_fd, صحیح netns_fd, صحیح userns_fd, صحیح جڑ_fd) {
        واپس namespace_enter_بھرا(pidns_fd, mntns_fd, netns_fd, userns_fd, جڑ_fd, /* flags= */ 0);
}

صحیح fd_is_namespace(صحیح fd, NamespaceType type);
صحیح is_our_namespace(صحیح fd, NamespaceType type);
صحیح are_our_namespaces(صحیح pidns_fd, صحیح mntns_fd, صحیح netns_fd, صحیح userns_fd, صحیح جڑ_fd);

صحیح جال_namespace_is_init(صحیح socket_fd);
صحیح namespace_is_init(NamespaceType type);

صحیح pidref_اندر_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
صحیح اندر_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

صحیح namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

صحیح detach_mount_namespace(خالی);
صحیح detach_mount_namespace_harder(uid_t ہدف_uid, gid_t ہدف_gid);
صحیح detach_mount_namespace_userns(صحیح userns_fd);

ساکن اندرونی bool userns_سرکاؤ_range_valid(uid_t سرکاؤ, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        اگر (range <= 0)
                واپس false;

        اگر (سرکاؤ > (uid_t) -1 - range)
                واپس false;

        واپس true;
}

صحیح parse_userns_uid_range(ثابت حرف *s, uid_t *ret_uid_سرکاؤ, uid_t *ret_uid_range);

صحیح userns_acquire_خالی_خانے(خالی);
صحیح userns_acquire(ثابت حرف *uid_map, ثابت حرف *gid_map, bool setgroups_deny);
صحیح userns_acquire_self_جڑ(خالی);
صحیح userns_enter_and_pin(صحیح userns_fd, PidRef *ret);
bool userns_supported(خالی);

صحیح userns_get_بنیاد_uid(صحیح userns_fd, uid_t *ret_uid, gid_t *ret_gid);
صحیح userns_has_جڑ_mapping(صحیح userns_fd);

صحیح namespace_open_by_id(uint64_t ns_id);

صحیح process_is_owned_by_uid(ثابت PidRef *pidref, uid_t uid);

صحیح is_idmapping_supported(ثابت حرف *راستہ);

صحیح netns_acquire(خالی);
