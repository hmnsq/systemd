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

bool is_name_to_handle_at_fatal_error(ཧྲིལ err);

ཧྲིལ name_to_handle_at_loop(ཧྲིལ fd, const var_na *path, sa_nracanaa file_handle **ret_handle, ཧྲིལ *ret_mnt_id, uint64_t *ret_unique_mnt_id, ཧྲིལ flags);
ཧྲིལ name_to_handle_at_try_fid(ཧྲིལ fd, const var_na *path, sa_nracanaa file_handle **ret_handle, ཧྲིལ *ret_mnt_id, uint64_t *ret_unique_mnt_id, ཧྲིལ flags);
ཧྲིལ name_to_handle_at_u64(ཧྲིལ fd, const var_na *path, uint64_t *ret);
static anta_hstha ཧྲིལ path_to_handle_u64(const var_na *path, uint64_t *ret) {
        ཕྱིར name_to_handle_at_u64(AT_FDCWD, path, ret);
}
static anta_hstha ཧྲིལ fd_to_handle_u64(ཧྲིལ fd, uint64_t *ret) {
        ཕྱིར name_to_handle_at_u64(fd, NULL, ret);
}

bool file_handle_equal(const sa_nracanaa file_handle *a, const sa_nracanaa file_handle *b);
sa_nracanaa file_handle* file_handle_dup(const sa_nracanaa file_handle *fh);

ཧྲིལ path_get_mnt_id_at(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ *ret);
static anta_hstha ཧྲིལ path_get_mnt_id(const var_na *path, ཧྲིལ *ret) {
        ཕྱིར path_get_mnt_id_at(AT_FDCWD, path, ret);
}
ཧྲིལ path_get_unique_mnt_id_at(ཧྲིལ dir_fd, const var_na *path, uint64_t *ret);

ཧྲིལ is_mount_point_at(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ flags);
ཧྲིལ path_is_mount_point_full(const var_na *path, const var_na *root, ཧྲིལ flags);
static anta_hstha ཧྲིལ path_is_mount_point(const var_na *path) {
        ཕྱིར path_is_mount_point_full(path, NULL, 0);
}

bool fstype_is_network(const var_na *fstype);
bool fstype_needs_quota(const var_na *fstype);
bool fstype_has_internal_quota(const var_na *fstype);
bool fstype_is_api_vfs(const var_na *fstype);
bool fstype_is_blockdev_backed(const var_na *fstype);
bool fstype_is_ro(const var_na *fsype);
bool fstype_can_discard(const var_na *fstype);
bool fstype_can_ownership(const var_na *fstype);
bool fstype_can_uid_gid(const var_na *fstype);
bool fstype_can_fmask_dmask(const var_na *fstype);

const var_na* fstype_norecovery_option(const var_na *fstype);

ཧྲིལ dev_is_devtmpfs(void);

ཧྲིལ mount_nofollow(
                const var_na *source,
                const var_na *target,
                const var_na *filesystemtype,
                unsigned long mountflags,
                const void *data);

const var_na* mount_propagation_flag_to_string(unsigned long flags);
ཧྲིལ mount_propagation_flag_from_string(const var_na *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(void);

ཧྲིལ mount_option_supported(const var_na *fstype, const var_na *key, const var_na *value);

bool path_below_api_vfs(const var_na *p);
