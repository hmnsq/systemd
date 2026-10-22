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

पूर्णाङ्क name_to_handle_at_loop(पूर्णाङ्क fd, अचर वर्ण *path, संरचना file_handle **ret_handle, पूर्णाङ्क *ret_mnt_id, uint64_t *ret_unique_mnt_id, पूर्णाङ्क flags);
पूर्णाङ्क name_to_handle_at_try_fid(पूर्णाङ्क fd, अचर वर्ण *path, संरचना file_handle **ret_handle, पूर्णाङ्क *ret_mnt_id, uint64_t *ret_unique_mnt_id, पूर्णाङ्क flags);
पूर्णाङ्क name_to_handle_at_u64(पूर्णाङ्क fd, अचर वर्ण *path, uint64_t *ret);
स्थिर अन्तःस्थ पूर्णाङ्क path_to_handle_u64(अचर वर्ण *path, uint64_t *ret) {
        फर्काऊ name_to_handle_at_u64(AT_FDCWD, path, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fd_to_handle_u64(पूर्णाङ्क fd, uint64_t *ret) {
        फर्काऊ name_to_handle_at_u64(fd, रिक्त, ret);
}

bool file_handle_equal(अचर संरचना file_handle *a, अचर संरचना file_handle *b);
संरचना file_handle* file_handle_dup(अचर संरचना file_handle *fh);

पूर्णाङ्क path_get_mnt_id_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, पूर्णाङ्क *ret);
स्थिर अन्तःस्थ पूर्णाङ्क path_get_mnt_id(अचर वर्ण *path, पूर्णाङ्क *ret) {
        फर्काऊ path_get_mnt_id_at(AT_FDCWD, path, ret);
}
पूर्णाङ्क path_get_unique_mnt_id_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, uint64_t *ret);

पूर्णाङ्क is_mount_point_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, पूर्णाङ्क flags);
पूर्णाङ्क path_is_mount_point_full(अचर वर्ण *path, अचर वर्ण *जरा, पूर्णाङ्क flags);
स्थिर अन्तःस्थ पूर्णाङ्क path_is_mount_point(अचर वर्ण *path) {
        फर्काऊ path_is_mount_point_full(path, रिक्त, 0);
}

bool fstype_is_network(अचर वर्ण *fstype);
bool fstype_needs_quota(अचर वर्ण *fstype);
bool fstype_has_internal_quota(अचर वर्ण *fstype);
bool fstype_is_api_vfs(अचर वर्ण *fstype);
bool fstype_is_blockdev_backed(अचर वर्ण *fstype);
bool fstype_is_ro(अचर वर्ण *fsype);
bool fstype_can_discard(अचर वर्ण *fstype);
bool fstype_can_ownership(अचर वर्ण *fstype);
bool fstype_can_uid_gid(अचर वर्ण *fstype);
bool fstype_can_fmask_dmask(अचर वर्ण *fstype);

अचर वर्ण* fstype_norecovery_option(अचर वर्ण *fstype);

पूर्णाङ्क dev_is_devtmpfs(शून्य);

पूर्णाङ्क mount_nofollow(
                अचर वर्ण *source,
                अचर वर्ण *target,
                अचर वर्ण *filesystemtype,
                अचिन्हित दीर्घ mountflags,
                अचर शून्य *data);

अचर वर्ण* mount_propagation_flag_to_string(अचिन्हित दीर्घ flags);
पूर्णाङ्क mount_propagation_flag_from_string(अचर वर्ण *name, अचिन्हित दीर्घ *ret);
bool mount_propagation_flag_is_valid(अचिन्हित दीर्घ flag);

bool mount_new_api_supported(शून्य);

पूर्णाङ्क mount_option_supported(अचर वर्ण *fstype, अचर वर्ण *साँचो, अचर वर्ण *मान);

bool path_below_api_vfs(अचर वर्ण *p);
