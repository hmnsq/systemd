/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#define TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#define TMPFS_LIMITS_PRIVATE_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#define TMPFS_LIMITS_EMPTY_OR_ALMOST ",size=4m,nr_inodes=1k"
#define TMPFS_LIMITS_SYS             TMPFS_LIMITS_EMPTY_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#define TMPFS_LIMITS_RUN             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#define NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#define TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#define TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#define TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_to_handle_at_fatal_error(पूर्णाङ्क err);

पूर्णाङ्क name_to_handle_at_loop(पूर्णाङ्क fd, ध्रुव वर्ण *path, संरचना file_handle **ret_handle, पूर्णाङ्क *ret_mnt_id, uint64_t *ret_unique_mnt_id, पूर्णाङ्क flags);
पूर्णाङ्क name_to_handle_at_try_fid(पूर्णाङ्क fd, ध्रुव वर्ण *path, संरचना file_handle **ret_handle, पूर्णाङ्क *ret_mnt_id, uint64_t *ret_unique_mnt_id, पूर्णाङ्क flags);
पूर्णाङ्क name_to_handle_at_u64(पूर्णाङ्क fd, ध्रुव वर्ण *path, uint64_t *ret);
स्थिर अन्तःस्थ पूर्णाङ्क path_to_handle_u64(ध्रुव वर्ण *path, uint64_t *ret) {
        पटिनिवत्त name_to_handle_at_u64(AT_FDCWD, path, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fd_to_handle_u64(पूर्णाङ्क fd, uint64_t *ret) {
        पटिनिवत्त name_to_handle_at_u64(fd, रिक्त, ret);
}

bool file_handle_equal(ध्रुव संरचना file_handle *a, ध्रुव संरचना file_handle *b);
संरचना file_handle* file_handle_dup(ध्रुव संरचना file_handle *fh);

पूर्णाङ्क path_get_mnt_id_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क *ret);
स्थिर अन्तःस्थ पूर्णाङ्क path_get_mnt_id(ध्रुव वर्ण *path, पूर्णाङ्क *ret) {
        पटिनिवत्त path_get_mnt_id_at(AT_FDCWD, path, ret);
}
पूर्णाङ्क path_get_unique_mnt_id_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, uint64_t *ret);

पूर्णाङ्क is_mount_point_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क flags);
पूर्णाङ्क path_is_mount_point_full(ध्रुव वर्ण *path, ध्रुव वर्ण *मूलम्, पूर्णाङ्क flags);
स्थिर अन्तःस्थ पूर्णाङ्क path_is_mount_point(ध्रुव वर्ण *path) {
        पटिनिवत्त path_is_mount_point_full(path, रिक्त, 0);
}

bool fstype_is_network(ध्रुव वर्ण *fstype);
bool fstype_needs_quota(ध्रुव वर्ण *fstype);
bool fstype_has_internal_quota(ध्रुव वर्ण *fstype);
bool fstype_is_api_vfs(ध्रुव वर्ण *fstype);
bool fstype_is_blockdev_backed(ध्रुव वर्ण *fstype);
bool fstype_is_ro(ध्रुव वर्ण *fsype);
bool fstype_can_discard(ध्रुव वर्ण *fstype);
bool fstype_can_ownership(ध्रुव वर्ण *fstype);
bool fstype_can_uid_gid(ध्रुव वर्ण *fstype);
bool fstype_can_fmask_dmask(ध्रुव वर्ण *fstype);

ध्रुव वर्ण* fstype_norecovery_option(ध्रुव वर्ण *fstype);

पूर्णाङ्क dev_is_devtmpfs(शून्य);

पूर्णाङ्क mount_nofollow(
                ध्रुव वर्ण *source,
                ध्रुव वर्ण *target,
                ध्रुव वर्ण *filesystemtype,
                unsigned long mountflags,
                ध्रुव शून्य *data);

ध्रुव वर्ण* mount_propagation_flag_to_string(unsigned long flags);
पूर्णाङ्क mount_propagation_flag_from_string(ध्रुव वर्ण *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(शून्य);

पूर्णाङ्क mount_option_supported(ध्रुव वर्ण *fstype, ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);

bool path_below_api_vfs(ध्रुव वर्ण *p);
