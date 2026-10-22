/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

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

bool is_name_to_handle_at_fatal_error(صحيح err);

صحيح name_to_handle_at_loop(صحيح fd, ثابت حرف *path, بنية file_handle **ret_handle, صحيح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحيح flags);
صحيح name_to_handle_at_try_fid(صحيح fd, ثابت حرف *path, بنية file_handle **ret_handle, صحيح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحيح flags);
صحيح name_to_handle_at_u64(صحيح fd, ثابت حرف *path, uint64_t *ret);
ساكن مضمن صحيح path_to_handle_u64(ثابت حرف *path, uint64_t *ret) {
        أرجع name_to_handle_at_u64(AT_FDCWD, path, ret);
}
ساكن مضمن صحيح fd_to_handle_u64(صحيح fd, uint64_t *ret) {
        أرجع name_to_handle_at_u64(fd, عدم, ret);
}

bool file_handle_equal(ثابت بنية file_handle *a, ثابت بنية file_handle *b);
بنية file_handle* file_handle_dup(ثابت بنية file_handle *fh);

صحيح path_get_mnt_id_at(صحيح dir_fd, ثابت حرف *path, صحيح *ret);
ساكن مضمن صحيح path_get_mnt_id(ثابت حرف *path, صحيح *ret) {
        أرجع path_get_mnt_id_at(AT_FDCWD, path, ret);
}
صحيح path_get_unique_mnt_id_at(صحيح dir_fd, ثابت حرف *path, uint64_t *ret);

صحيح is_mount_point_at(صحيح dir_fd, ثابت حرف *path, صحيح flags);
صحيح path_is_mount_point_full(ثابت حرف *path, ثابت حرف *جذر, صحيح flags);
ساكن مضمن صحيح path_is_mount_point(ثابت حرف *path) {
        أرجع path_is_mount_point_full(path, عدم, 0);
}

bool fstype_is_network(ثابت حرف *fstype);
bool fstype_needs_quota(ثابت حرف *fstype);
bool fstype_has_internal_quota(ثابت حرف *fstype);
bool fstype_is_api_vfs(ثابت حرف *fstype);
bool fstype_is_blockdev_backed(ثابت حرف *fstype);
bool fstype_is_ro(ثابت حرف *fsype);
bool fstype_can_discard(ثابت حرف *fstype);
bool fstype_can_ownership(ثابت حرف *fstype);
bool fstype_can_uid_gid(ثابت حرف *fstype);
bool fstype_can_fmask_dmask(ثابت حرف *fstype);

ثابت حرف* fstype_norecovery_option(ثابت حرف *fstype);

صحيح dev_is_devtmpfs(فراغ);

صحيح mount_nofollow(
                ثابت حرف *source,
                ثابت حرف *target,
                ثابت حرف *filesystemtype,
                unsigned long mountflags,
                ثابت فراغ *data);

ثابت حرف* mount_propagation_flag_to_string(unsigned long flags);
صحيح mount_propagation_flag_from_string(ثابت حرف *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(فراغ);

صحيح mount_option_supported(ثابت حرف *fstype, ثابت حرف *مفتاح, ثابت حرف *قيمة);

bool path_below_api_vfs(ثابت حرف *p);
