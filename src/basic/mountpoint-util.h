/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

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

bool is_name_to_handle_at_fatal_error(முழு err);

முழு name_to_handle_at_loop(முழு fd, மாறிலி எழுத்து *path, கட்டமைப்பு file_handle **ret_handle, முழு *ret_mnt_id, uint64_t *ret_unique_mnt_id, முழு flags);
முழு name_to_handle_at_try_fid(முழு fd, மாறிலி எழுத்து *path, கட்டமைப்பு file_handle **ret_handle, முழு *ret_mnt_id, uint64_t *ret_unique_mnt_id, முழு flags);
முழு name_to_handle_at_u64(முழு fd, மாறிலி எழுத்து *path, uint64_t *ret);
நிலையான உள்வரி முழு path_to_handle_u64(மாறிலி எழுத்து *path, uint64_t *ret) {
        திரும்பு name_to_handle_at_u64(AT_FDCWD, path, ret);
}
நிலையான உள்வரி முழு fd_to_handle_u64(முழு fd, uint64_t *ret) {
        திரும்பு name_to_handle_at_u64(fd, வெற்று, ret);
}

bool file_handle_equal(மாறிலி கட்டமைப்பு file_handle *a, மாறிலி கட்டமைப்பு file_handle *b);
கட்டமைப்பு file_handle* file_handle_dup(மாறிலி கட்டமைப்பு file_handle *fh);

முழு path_get_mnt_id_at(முழு dir_fd, மாறிலி எழுத்து *path, முழு *ret);
நிலையான உள்வரி முழு path_get_mnt_id(மாறிலி எழுத்து *path, முழு *ret) {
        திரும்பு path_get_mnt_id_at(AT_FDCWD, path, ret);
}
முழு path_get_unique_mnt_id_at(முழு dir_fd, மாறிலி எழுத்து *path, uint64_t *ret);

முழு is_mount_point_at(முழு dir_fd, மாறிலி எழுத்து *path, முழு flags);
முழு path_is_mount_point_full(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், முழு flags);
நிலையான உள்வரி முழு path_is_mount_point(மாறிலி எழுத்து *path) {
        திரும்பு path_is_mount_point_full(path, வெற்று, 0);
}

bool fstype_is_network(மாறிலி எழுத்து *fstype);
bool fstype_needs_quota(மாறிலி எழுத்து *fstype);
bool fstype_has_internal_quota(மாறிலி எழுத்து *fstype);
bool fstype_is_api_vfs(மாறிலி எழுத்து *fstype);
bool fstype_is_blockdev_backed(மாறிலி எழுத்து *fstype);
bool fstype_is_ro(மாறிலி எழுத்து *fsype);
bool fstype_can_discard(மாறிலி எழுத்து *fstype);
bool fstype_can_ownership(மாறிலி எழுத்து *fstype);
bool fstype_can_uid_gid(மாறிலி எழுத்து *fstype);
bool fstype_can_fmask_dmask(மாறிலி எழுத்து *fstype);

மாறிலி எழுத்து* fstype_norecovery_option(மாறிலி எழுத்து *fstype);

முழு dev_is_devtmpfs(வெறுமை);

முழு mount_nofollow(
                மாறிலி எழுத்து *source,
                மாறிலி எழுத்து *target,
                மாறிலி எழுத்து *filesystemtype,
                unsigned long mountflags,
                மாறிலி வெறுமை *data);

மாறிலி எழுத்து* mount_propagation_flag_to_string(unsigned long flags);
முழு mount_propagation_flag_from_string(மாறிலி எழுத்து *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(வெறுமை);

முழு mount_option_supported(மாறிலி எழுத்து *fstype, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு);

bool path_below_api_vfs(மாறிலி எழுத்து *p);
