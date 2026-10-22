/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

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

bool is_name_to_handle_at_fatal_error(صحیح err);

صحیح name_to_handle_at_loop(صحیح fd, ثابت نویسه *path, ساختار file_handle **ret_handle, صحیح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحیح flags);
صحیح name_to_handle_at_try_fid(صحیح fd, ثابت نویسه *path, ساختار file_handle **ret_handle, صحیح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحیح flags);
صحیح name_to_handle_at_u64(صحیح fd, ثابت نویسه *path, uint64_t *ret);
ایستا درونخطی صحیح path_to_handle_u64(ثابت نویسه *path, uint64_t *ret) {
        بازگشت name_to_handle_at_u64(AT_FDCWD, path, ret);
}
ایستا درونخطی صحیح fd_to_handle_u64(صحیح fd, uint64_t *ret) {
        بازگشت name_to_handle_at_u64(fd, پوچ, ret);
}

bool file_handle_equal(ثابت ساختار file_handle *a, ثابت ساختار file_handle *b);
ساختار file_handle* file_handle_dup(ثابت ساختار file_handle *fh);

صحیح path_get_mnt_id_at(صحیح dir_fd, ثابت نویسه *path, صحیح *ret);
ایستا درونخطی صحیح path_get_mnt_id(ثابت نویسه *path, صحیح *ret) {
        بازگشت path_get_mnt_id_at(AT_FDCWD, path, ret);
}
صحیح path_get_unique_mnt_id_at(صحیح dir_fd, ثابت نویسه *path, uint64_t *ret);

صحیح is_mount_point_at(صحیح dir_fd, ثابت نویسه *path, صحیح flags);
صحیح path_is_mount_point_full(ثابت نویسه *path, ثابت نویسه *ریشه, صحیح flags);
ایستا درونخطی صحیح path_is_mount_point(ثابت نویسه *path) {
        بازگشت path_is_mount_point_full(path, پوچ, 0);
}

bool fstype_is_network(ثابت نویسه *fstype);
bool fstype_needs_quota(ثابت نویسه *fstype);
bool fstype_has_internal_quota(ثابت نویسه *fstype);
bool fstype_is_api_vfs(ثابت نویسه *fstype);
bool fstype_is_blockdev_backed(ثابت نویسه *fstype);
bool fstype_is_ro(ثابت نویسه *fsype);
bool fstype_can_discard(ثابت نویسه *fstype);
bool fstype_can_ownership(ثابت نویسه *fstype);
bool fstype_can_uid_gid(ثابت نویسه *fstype);
bool fstype_can_fmask_dmask(ثابت نویسه *fstype);

ثابت نویسه* fstype_norecovery_option(ثابت نویسه *fstype);

صحیح dev_is_devtmpfs(تهی);

صحیح mount_nofollow(
                ثابت نویسه *source,
                ثابت نویسه *target,
                ثابت نویسه *filesystemtype,
                unsigned long mountflags,
                ثابت تهی *data);

ثابت نویسه* mount_propagation_flag_to_string(unsigned long flags);
صحیح mount_propagation_flag_from_string(ثابت نویسه *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(تهی);

صحیح mount_option_supported(ثابت نویسه *fstype, ثابت نویسه *کلید, ثابت نویسه *مقدار);

bool path_below_api_vfs(ثابت نویسه *p);
