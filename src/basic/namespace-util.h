/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన NamespaceType {
        NAMESPACE_CGROUP,
        NAMESPACE_IPC,
        NAMESPACE_NET,
        NAMESPACE_MOUNT,
        NAMESPACE_PID,
        NAMESPACE_USER,
        NAMESPACE_UTS,
        NAMESPACE_సమయం,
        _NAMESPACE_TYPE_MAX,
        _NAMESPACE_TYPE_INVALID = -EINVAL,
} NamespaceType;

బాహ్య స్థిరరాశి నిర్మాణం namespace_info {
        స్థిరరాశి అక్షరం *proc_name;
        స్థిరరాశి అక్షరం *proc_మార్గం;
        గుర్తులేని దీర్ఘ clone_flag;
        గుర్తులేని దీర్ఘ pidfd_get_ns_ioctl_cmd;
        ino_t మూలం_inode;
} namespace_info[_NAMESPACE_TYPE_MAX + 1];

NamespaceType clone_flag_గమ్యం_namespace_type(గుర్తులేని దీర్ఘ clone_flag);

bool namespace_type_supported(NamespaceType type);

పూర్ణాంకం pidref_namespace_open_by_type(స్థిరరాశి PidRef *pidref, NamespaceType type);
పూర్ణాంకం namespace_open_by_type(NamespaceType type);

పూర్ణాంకం pidref_namespace_open(
                స్థిరరాశి PidRef *pidref,
                పూర్ణాంకం *ret_pidns_fd,
                పూర్ణాంకం *ret_mntns_fd,
                పూర్ణాంకం *ret_netns_fd,
                పూర్ణాంకం *ret_userns_fd,
                పూర్ణాంకం *ret_మూలం_fd);
పూర్ణాంకం namespace_open(
                pid_t pid,
                పూర్ణాంకం *ret_pidns_fd,
                పూర్ణాంకం *ret_mntns_fd,
                పూర్ణాంకం *ret_netns_fd,
                పూర్ణాంకం *ret_userns_fd,
                పూర్ణాంకం *ret_మూలం_fd);

రకనిర్వచనం గణన NamespaceEnterFlags {
        /* If root is not mapped in the user namespace, keep our UID/GID instead of failing. The caller must
         * ensure that retaining its original identity is appropriate in that case. Entering still fails
         * with -ENODATA if the namespace has no mappings at all yet, since root might still get mapped. */
        NAMESPACE_ENTER_KEEP_UID_GID_IF_మూలం_UNMAPPED = 1 << 0,
} NamespaceEnterFlags;

/* If a user namespace is entered we become root in it, and fail if that's not possible. */
పూర్ణాంకం namespace_enter_నిండిన(
                పూర్ణాంకం pidns_fd,
                పూర్ణాంకం mntns_fd,
                పూర్ణాంకం netns_fd,
                పూర్ణాంకం userns_fd,
                పూర్ణాంకం మూలం_fd,
                NamespaceEnterFlags flags);

స్థిర అంతఃవరుస పూర్ణాంకం namespace_enter(పూర్ణాంకం pidns_fd, పూర్ణాంకం mntns_fd, పూర్ణాంకం netns_fd, పూర్ణాంకం userns_fd, పూర్ణాంకం మూలం_fd) {
        తిరిగి namespace_enter_నిండిన(pidns_fd, mntns_fd, netns_fd, userns_fd, మూలం_fd, /* flags= */ 0);
}

పూర్ణాంకం fd_is_namespace(పూర్ణాంకం fd, NamespaceType type);
పూర్ణాంకం is_our_namespace(పూర్ణాంకం fd, NamespaceType type);
పూర్ణాంకం are_our_namespaces(పూర్ణాంకం pidns_fd, పూర్ణాంకం mntns_fd, పూర్ణాంకం netns_fd, పూర్ణాంకం userns_fd, పూర్ణాంకం మూలం_fd);

పూర్ణాంకం జాలం_namespace_is_init(పూర్ణాంకం socket_fd);
పూర్ణాంకం namespace_is_init(NamespaceType type);

పూర్ణాంకం pidref_లోపల_same_namespace(PidRef *pid1, PidRef *pid2, NamespaceType type);
పూర్ణాంకం లోపల_same_namespace(pid_t pid1, pid_t pid2, NamespaceType type);

పూర్ణాంకం namespace_get_leader(PidRef *pidref, NamespaceType type, PidRef *ret);

పూర్ణాంకం detach_mount_namespace(శూన్య);
పూర్ణాంకం detach_mount_namespace_harder(uid_t లక్ష్యం_uid, gid_t లక్ష్యం_gid);
పూర్ణాంకం detach_mount_namespace_userns(పూర్ణాంకం userns_fd);

స్థిర అంతఃవరుస bool userns_జరుపు_range_valid(uid_t జరుపు, uid_t range) {
        /* Checks that the specified userns range makes sense, i.e. contains at least one UID, and the end
         * doesn't overflow uid_t. */

        assert_cc((uid_t) -1 > 0); /* verify that uid_t is unsigned */

        ఒకవేళ (range <= 0)
                తిరిగి false;

        ఒకవేళ (జరుపు > (uid_t) -1 - range)
                తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం parse_userns_uid_range(స్థిరరాశి అక్షరం *s, uid_t *ret_uid_జరుపు, uid_t *ret_uid_range);

పూర్ణాంకం userns_acquire_ఖాళీలు(శూన్య);
పూర్ణాంకం userns_acquire(స్థిరరాశి అక్షరం *uid_map, స్థిరరాశి అక్షరం *gid_map, bool setgroups_deny);
పూర్ణాంకం userns_acquire_self_మూలం(శూన్య);
పూర్ణాంకం userns_enter_and_pin(పూర్ణాంకం userns_fd, PidRef *ret);
bool userns_supported(శూన్య);

పూర్ణాంకం userns_get_పునాది_uid(పూర్ణాంకం userns_fd, uid_t *ret_uid, gid_t *ret_gid);
పూర్ణాంకం userns_has_మూలం_mapping(పూర్ణాంకం userns_fd);

పూర్ణాంకం namespace_open_by_id(uint64_t ns_id);

పూర్ణాంకం process_is_owned_by_uid(స్థిరరాశి PidRef *pidref, uid_t uid);

పూర్ణాంకం is_idmapping_supported(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం netns_acquire(శూన్య);
