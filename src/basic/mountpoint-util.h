/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

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

bool is_name_to_handle_at_fatal_error(整数 err);

整数 name_to_handle_at_loop(整数 fd, const char *path, struct file_handle **ret_handle, 整数 *ret_mnt_id, uint64_t *ret_unique_mnt_id, 整数 flags);
整数 name_to_handle_at_try_fid(整数 fd, const char *path, struct file_handle **ret_handle, 整数 *ret_mnt_id, uint64_t *ret_unique_mnt_id, 整数 flags);
整数 name_to_handle_at_u64(整数 fd, const char *path, uint64_t *ret);
static inline 整数 path_to_handle_u64(const char *path, uint64_t *ret) {
        返回 name_to_handle_at_u64(AT_FDCWD, path, ret);
}
static inline 整数 fd_to_handle_u64(整数 fd, uint64_t *ret) {
        返回 name_to_handle_at_u64(fd, NULL, ret);
}

bool file_handle_equal(const struct file_handle *a, const struct file_handle *b);
struct file_handle* file_handle_dup(const struct file_handle *fh);

整数 path_get_mnt_id_at(整数 dir_fd, const char *path, 整数 *ret);
static inline 整数 path_get_mnt_id(const char *path, 整数 *ret) {
        返回 path_get_mnt_id_at(AT_FDCWD, path, ret);
}
整数 path_get_unique_mnt_id_at(整数 dir_fd, const char *path, uint64_t *ret);

整数 is_mount_point_at(整数 dir_fd, const char *path, 整数 flags);
整数 path_is_mount_point_full(const char *path, const char *root, 整数 flags);
static inline 整数 path_is_mount_point(const char *path) {
        返回 path_is_mount_point_full(path, NULL, 0);
}

bool fstype_is_network(const char *fstype);
bool fstype_needs_quota(const char *fstype);
bool fstype_has_internal_quota(const char *fstype);
bool fstype_is_api_vfs(const char *fstype);
bool fstype_is_blockdev_backed(const char *fstype);
bool fstype_is_ro(const char *fsype);
bool fstype_can_discard(const char *fstype);
bool fstype_can_ownership(const char *fstype);
bool fstype_can_uid_gid(const char *fstype);
bool fstype_can_fmask_dmask(const char *fstype);

const char* fstype_norecovery_option(const char *fstype);

整数 dev_is_devtmpfs(void);

整数 mount_nofollow(
                const char *source,
                const char *target,
                const char *filesystemtype,
                unsigned long mountflags,
                const void *data);

const char* mount_propagation_flag_to_string(unsigned long flags);
整数 mount_propagation_flag_from_string(const char *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(void);

整数 mount_option_supported(const char *fstype, const char *key, const char *value);

bool path_below_api_vfs(const char *p);
