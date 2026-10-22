/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

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

bool is_name_to_handle_at_fatal_error(പൂർണ്ണസംഖ്യ err);

പൂർണ്ണസംഖ്യ name_to_handle_at_loop(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, ഘടന file_handle **ret_handle, പൂർണ്ണസംഖ്യ *ret_mnt_id, uint64_t *ret_unique_mnt_id, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ name_to_handle_at_try_fid(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, ഘടന file_handle **ret_handle, പൂർണ്ണസംഖ്യ *ret_mnt_id, uint64_t *ret_unique_mnt_id, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ name_to_handle_at_u64(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, uint64_t *ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_to_handle_u64(സ്ഥിരം അക്ഷരം *path, uint64_t *ret) {
        തിരികെ name_to_handle_at_u64(AT_FDCWD, path, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_to_handle_u64(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        തിരികെ name_to_handle_at_u64(fd, ശൂന്യ, ret);
}

bool file_handle_equal(സ്ഥിരം ഘടന file_handle *a, സ്ഥിരം ഘടന file_handle *b);
ഘടന file_handle* file_handle_dup(സ്ഥിരം ഘടന file_handle *fh);

പൂർണ്ണസംഖ്യ path_get_mnt_id_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ *ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_get_mnt_id(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ *ret) {
        തിരികെ path_get_mnt_id_at(AT_FDCWD, path, ret);
}
പൂർണ്ണസംഖ്യ path_get_unique_mnt_id_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, uint64_t *ret);

പൂർണ്ണസംഖ്യ is_mount_point_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ path_is_mount_point_full(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, പൂർണ്ണസംഖ്യ flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_is_mount_point(സ്ഥിരം അക്ഷരം *path) {
        തിരികെ path_is_mount_point_full(path, ശൂന്യ, 0);
}

bool fstype_is_network(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_needs_quota(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_has_internal_quota(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_is_api_vfs(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_is_blockdev_backed(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_is_ro(സ്ഥിരം അക്ഷരം *fsype);
bool fstype_can_discard(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_can_ownership(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_can_uid_gid(സ്ഥിരം അക്ഷരം *fstype);
bool fstype_can_fmask_dmask(സ്ഥിരം അക്ഷരം *fstype);

സ്ഥിരം അക്ഷരം* fstype_norecovery_option(സ്ഥിരം അക്ഷരം *fstype);

പൂർണ്ണസംഖ്യ dev_is_devtmpfs(ശൂന്യം);

പൂർണ്ണസംഖ്യ mount_nofollow(
                സ്ഥിരം അക്ഷരം *source,
                സ്ഥിരം അക്ഷരം *target,
                സ്ഥിരം അക്ഷരം *filesystemtype,
                unsigned long mountflags,
                സ്ഥിരം ശൂന്യം *data);

സ്ഥിരം അക്ഷരം* mount_propagation_flag_to_string(unsigned long flags);
പൂർണ്ണസംഖ്യ mount_propagation_flag_from_string(സ്ഥിരം അക്ഷരം *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(ശൂന്യം);

പൂർണ്ണസംഖ്യ mount_option_supported(സ്ഥിരം അക്ഷരം *fstype, സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);

bool path_below_api_vfs(സ്ഥിരം അക്ഷരം *p);
