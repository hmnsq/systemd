/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_ಖಾಸಗಿ_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_ಖಾಲಿಗಳು_OR_ALMOST ",size=4m,nr_inodes=1k"
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_SYS             TMPFS_LIMITS_ಖಾಲಿಗಳು_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_ಸತತ             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#ವ್ಯಾಖ್ಯಾನಿಸು NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#ವ್ಯಾಖ್ಯಾನಿಸು TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_ಗಮ್ಯ_handle_at_fatal_error(ಪೂರ್ಣಾಂಕ err);

ಪೂರ್ಣಾಂಕ name_ಗಮ್ಯ_handle_at_loop(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ರಚನೆ file_handle **ret_handle, ಪೂರ್ಣಾಂಕ *ret_mnt_id, uint64_t *ret_unique_mnt_id, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ name_ಗಮ್ಯ_handle_at_try_fid(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ರಚನೆ file_handle **ret_handle, ಪೂರ್ಣಾಂಕ *ret_mnt_id, uint64_t *ret_unique_mnt_id, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ name_ಗಮ್ಯ_handle_at_u64(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_ಗಮ್ಯ_handle_u64(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret) {
        ಹಿಂತಿರುಗಿ name_ಗಮ್ಯ_handle_at_u64(AT_FDCWD, ಮಾರ್ಗ, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_ಗಮ್ಯ_handle_u64(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ಹಿಂತಿರುಗಿ name_ಗಮ್ಯ_handle_at_u64(fd, ಖಾಲಿ, ret);
}

bool file_handle_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ file_handle *a, ಸ್ಥಿರಾಂಕ ರಚನೆ file_handle *b);
ರಚನೆ file_handle* file_handle_dup(ಸ್ಥಿರಾಂಕ ರಚನೆ file_handle *fh);

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_get_mnt_id_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_get_mnt_id(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_get_mnt_id_at(AT_FDCWD, ಮಾರ್ಗ, ret);
}
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_get_unique_mnt_id_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret);

ಪೂರ್ಣಾಂಕ is_mount_ಬಿಂದು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_mount_ಬಿಂದು_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_mount_ಬಿಂದು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_is_mount_ಬಿಂದು_ತುಂಬಿದ(ಮಾರ್ಗ, ಖಾಲಿ, 0);
}

bool fstype_is_ಜಾಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_needs_quota(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_has_ಆಂತರಿಕ_quota(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_is_api_vfs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_is_blockdev_backed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_is_ro(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fsype);
bool fstype_can_discard(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_can_ownership(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_can_uid_gid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);
bool fstype_can_fmask_dmask(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* fstype_norecovery_option(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype);

ಪೂರ್ಣಾಂಕ dev_is_devtmpfs(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ mount_nofollow(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filesystemtype,
                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mountflags,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* mount_ಪ್ರಸರಣ_flag_ಗಮ್ಯ_string(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags);
ಪೂರ್ಣಾಂಕ mount_ಪ್ರಸರಣ_flag_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ret);
bool mount_ಪ್ರಸರಣ_flag_is_valid(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flag);

bool mount_new_api_supported(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ mount_option_supported(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ);

bool ಮಾರ್ಗ_ಕೆಳಗೆ_api_vfs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p);
