/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

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

bool is_name_to_handle_at_fatal_error(પૂર્ણાંક err);

પૂર્ણાંક name_to_handle_at_loop(પૂર્ણાંક fd, અચલ અક્ષર *path, સંરચના file_handle **ret_handle, પૂર્ણાંક *ret_mnt_id, uint64_t *ret_unique_mnt_id, પૂર્ણાંક flags);
પૂર્ણાંક name_to_handle_at_try_fid(પૂર્ણાંક fd, અચલ અક્ષર *path, સંરચના file_handle **ret_handle, પૂર્ણાંક *ret_mnt_id, uint64_t *ret_unique_mnt_id, પૂર્ણાંક flags);
પૂર્ણાંક name_to_handle_at_u64(પૂર્ણાંક fd, અચલ અક્ષર *path, uint64_t *ret);
સ્થિર અંતઃરેખ પૂર્ણાંક path_to_handle_u64(અચલ અક્ષર *path, uint64_t *ret) {
        પાછા name_to_handle_at_u64(AT_FDCWD, path, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક fd_to_handle_u64(પૂર્ણાંક fd, uint64_t *ret) {
        પાછા name_to_handle_at_u64(fd, ખાલી, ret);
}

bool file_handle_equal(અચલ સંરચના file_handle *a, અચલ સંરચના file_handle *b);
સંરચના file_handle* file_handle_dup(અચલ સંરચના file_handle *fh);

પૂર્ણાંક path_get_mnt_id_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક *ret);
સ્થિર અંતઃરેખ પૂર્ણાંક path_get_mnt_id(અચલ અક્ષર *path, પૂર્ણાંક *ret) {
        પાછા path_get_mnt_id_at(AT_FDCWD, path, ret);
}
પૂર્ણાંક path_get_unique_mnt_id_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, uint64_t *ret);

પૂર્ણાંક is_mount_point_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક flags);
પૂર્ણાંક path_is_mount_point_full(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, પૂર્ણાંક flags);
સ્થિર અંતઃરેખ પૂર્ણાંક path_is_mount_point(અચલ અક્ષર *path) {
        પાછા path_is_mount_point_full(path, ખાલી, 0);
}

bool fstype_is_network(અચલ અક્ષર *fstype);
bool fstype_needs_quota(અચલ અક્ષર *fstype);
bool fstype_has_internal_quota(અચલ અક્ષર *fstype);
bool fstype_is_api_vfs(અચલ અક્ષર *fstype);
bool fstype_is_blockdev_backed(અચલ અક્ષર *fstype);
bool fstype_is_ro(અચલ અક્ષર *fsype);
bool fstype_can_discard(અચલ અક્ષર *fstype);
bool fstype_can_ownership(અચલ અક્ષર *fstype);
bool fstype_can_uid_gid(અચલ અક્ષર *fstype);
bool fstype_can_fmask_dmask(અચલ અક્ષર *fstype);

અચલ અક્ષર* fstype_norecovery_option(અચલ અક્ષર *fstype);

પૂર્ણાંક dev_is_devtmpfs(શૂન્ય);

પૂર્ણાંક mount_nofollow(
                અચલ અક્ષર *source,
                અચલ અક્ષર *target,
                અચલ અક્ષર *filesystemtype,
                unsigned long mountflags,
                અચલ શૂન્ય *data);

અચલ અક્ષર* mount_propagation_flag_to_string(unsigned long flags);
પૂર્ણાંક mount_propagation_flag_from_string(અચલ અક્ષર *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(શૂન્ય);

પૂર્ણાંક mount_option_supported(અચલ અક્ષર *fstype, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);

bool path_below_api_vfs(અચલ અક્ષર *p);
