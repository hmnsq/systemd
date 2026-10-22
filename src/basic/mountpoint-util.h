/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

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

bool is_name_to_handle_at_fatal_error(পূর্ণসংখ্যা err);

পূর্ণসংখ্যা name_to_handle_at_loop(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, কাঠামো file_handle **ret_handle, পূর্ণসংখ্যা *ret_mnt_id, uint64_t *ret_unique_mnt_id, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা name_to_handle_at_try_fid(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, কাঠামো file_handle **ret_handle, পূর্ণসংখ্যা *ret_mnt_id, uint64_t *ret_unique_mnt_id, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা name_to_handle_at_u64(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, uint64_t *ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_to_handle_u64(ধ্রুবক অক্ষর *path, uint64_t *ret) {
        ফেরত name_to_handle_at_u64(AT_FDCWD, path, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_to_handle_u64(পূর্ণসংখ্যা fd, uint64_t *ret) {
        ফেরত name_to_handle_at_u64(fd, ফাঁকা, ret);
}

bool file_handle_equal(ধ্রুবক কাঠামো file_handle *a, ধ্রুবক কাঠামো file_handle *b);
কাঠামো file_handle* file_handle_dup(ধ্রুবক কাঠামো file_handle *fh);

পূর্ণসংখ্যা path_get_mnt_id_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা *ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_get_mnt_id(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা *ret) {
        ফেরত path_get_mnt_id_at(AT_FDCWD, path, ret);
}
পূর্ণসংখ্যা path_get_unique_mnt_id_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, uint64_t *ret);

পূর্ণসংখ্যা is_mount_point_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা path_is_mount_point_full(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, পূর্ণসংখ্যা flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_is_mount_point(ধ্রুবক অক্ষর *path) {
        ফেরত path_is_mount_point_full(path, ফাঁকা, 0);
}

bool fstype_is_network(ধ্রুবক অক্ষর *fstype);
bool fstype_needs_quota(ধ্রুবক অক্ষর *fstype);
bool fstype_has_internal_quota(ধ্রুবক অক্ষর *fstype);
bool fstype_is_api_vfs(ধ্রুবক অক্ষর *fstype);
bool fstype_is_blockdev_backed(ধ্রুবক অক্ষর *fstype);
bool fstype_is_ro(ধ্রুবক অক্ষর *fsype);
bool fstype_can_discard(ধ্রুবক অক্ষর *fstype);
bool fstype_can_ownership(ধ্রুবক অক্ষর *fstype);
bool fstype_can_uid_gid(ধ্রুবক অক্ষর *fstype);
bool fstype_can_fmask_dmask(ধ্রুবক অক্ষর *fstype);

ধ্রুবক অক্ষর* fstype_norecovery_option(ধ্রুবক অক্ষর *fstype);

পূর্ণসংখ্যা dev_is_devtmpfs(শূন্য);

পূর্ণসংখ্যা mount_nofollow(
                ধ্রুবক অক্ষর *source,
                ধ্রুবক অক্ষর *target,
                ধ্রুবক অক্ষর *filesystemtype,
                unsigned long mountflags,
                ধ্রুবক শূন্য *তথ্য);

ধ্রুবক অক্ষর* mount_propagation_flag_to_string(unsigned long flags);
পূর্ণসংখ্যা mount_propagation_flag_from_string(ধ্রুবক অক্ষর *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(শূন্য);

পূর্ণসংখ্যা mount_option_supported(ধ্রুবক অক্ষর *fstype, ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান);

bool path_below_api_vfs(ধ্রুবক অক্ষর *p);
