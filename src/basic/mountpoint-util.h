/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

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

bool is_name_to_handle_at_fatal_error(পূর্ণাঙ্ক err);

পূর্ণাঙ্ক name_to_handle_at_loop(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *path, সংরচনা file_handle **ret_handle, পূর্ণাঙ্ক *ret_mnt_id, uint64_t *ret_unique_mnt_id, পূর্ণাঙ্ক flags);
পূর্ণাঙ্ক name_to_handle_at_try_fid(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *path, সংরচনা file_handle **ret_handle, পূর্ণাঙ্ক *ret_mnt_id, uint64_t *ret_unique_mnt_id, পূর্ণাঙ্ক flags);
পূর্ণাঙ্ক name_to_handle_at_u64(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *path, uint64_t *ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক path_to_handle_u64(ধ্রুৱ ৱর্ণ *path, uint64_t *ret) {
        ঘূৰাই name_to_handle_at_u64(AT_FDCWD, path, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক fd_to_handle_u64(পূর্ণাঙ্ক fd, uint64_t *ret) {
        ঘূৰাই name_to_handle_at_u64(fd, রিক্ত, ret);
}

bool file_handle_equal(ধ্রুৱ সংরচনা file_handle *a, ধ্রুৱ সংরচনা file_handle *b);
সংরচনা file_handle* file_handle_dup(ধ্রুৱ সংরচনা file_handle *fh);

পূর্ণাঙ্ক path_get_mnt_id_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক *ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক path_get_mnt_id(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক *ret) {
        ঘূৰাই path_get_mnt_id_at(AT_FDCWD, path, ret);
}
পূর্ণাঙ্ক path_get_unique_mnt_id_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, uint64_t *ret);

পূর্ণাঙ্ক is_mount_point_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags);
পূর্ণাঙ্ক path_is_mount_point_full(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *মূল, পূর্ণাঙ্ক flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক path_is_mount_point(ধ্রুৱ ৱর্ণ *path) {
        ঘূৰাই path_is_mount_point_full(path, রিক্ত, 0);
}

bool fstype_is_network(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_needs_quota(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_has_internal_quota(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_is_api_vfs(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_is_blockdev_backed(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_is_ro(ধ্রুৱ ৱর্ণ *fsype);
bool fstype_can_discard(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_can_ownership(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_can_uid_gid(ধ্রুৱ ৱর্ণ *fstype);
bool fstype_can_fmask_dmask(ধ্রুৱ ৱর্ণ *fstype);

ধ্রুৱ ৱর্ণ* fstype_norecovery_option(ধ্রুৱ ৱর্ণ *fstype);

পূর্ণাঙ্ক dev_is_devtmpfs(শূন্য);

পূর্ণাঙ্ক mount_nofollow(
                ধ্রুৱ ৱর্ণ *source,
                ধ্রুৱ ৱর্ণ *target,
                ধ্রুৱ ৱর্ণ *filesystemtype,
                unsigned long mountflags,
                ধ্রুৱ শূন্য *data);

ধ্রুৱ ৱর্ণ* mount_propagation_flag_to_string(unsigned long flags);
পূর্ণাঙ্ক mount_propagation_flag_from_string(ধ্রুৱ ৱর্ণ *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(শূন্য);

পূর্ণাঙ্ক mount_option_supported(ধ্রুৱ ৱর্ণ *fstype, ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);

bool path_below_api_vfs(ধ্রুৱ ৱর্ণ *p);
