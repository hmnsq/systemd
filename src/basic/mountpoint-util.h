/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_ਨਿੱਜੀ_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_ਖਾਲੀ_ਥਾਵਾਂ_OR_ALMOST ",size=4m,nr_inodes=1k"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_SYS             TMPFS_LIMITS_ਖਾਲੀ_ਥਾਵਾਂ_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_ਲੜੀਵਾਰ             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_ਮੰਜ਼ਿਲ_handle_at_fatal_error(ਪੂਰਨ err);

ਪੂਰਨ name_ਮੰਜ਼ਿਲ_handle_at_loop(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਣਤਰ file_handle **ret_handle, ਪੂਰਨ *ret_mnt_id, uint64_t *ret_unique_mnt_id, ਪੂਰਨ flags);
ਪੂਰਨ name_ਮੰਜ਼ਿਲ_handle_at_try_fid(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਣਤਰ file_handle **ret_handle, ਪੂਰਨ *ret_mnt_id, uint64_t *ret_unique_mnt_id, ਪੂਰਨ flags);
ਪੂਰਨ name_ਮੰਜ਼ਿਲ_handle_at_u64(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_ਮੰਜ਼ਿਲ_handle_u64(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret) {
        ਵਾਪਸ name_ਮੰਜ਼ਿਲ_handle_at_u64(AT_FDCWD, ਰਸਤਾ, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_ਮੰਜ਼ਿਲ_handle_u64(ਪੂਰਨ fd, uint64_t *ret) {
        ਵਾਪਸ name_ਮੰਜ਼ਿਲ_handle_at_u64(fd, ਨਿਰਾ, ret);
}

bool file_handle_equal(ਅਚਲ ਬਣਤਰ file_handle *a, ਅਚਲ ਬਣਤਰ file_handle *b);
ਬਣਤਰ file_handle* file_handle_dup(ਅਚਲ ਬਣਤਰ file_handle *fh);

ਪੂਰਨ ਰਸਤਾ_get_mnt_id_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_get_mnt_id(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ *ret) {
        ਵਾਪਸ ਰਸਤਾ_get_mnt_id_at(AT_FDCWD, ਰਸਤਾ, ret);
}
ਪੂਰਨ ਰਸਤਾ_get_unique_mnt_id_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret);

ਪੂਰਨ is_mount_ਬਿੰਦੂ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags);
ਪੂਰਨ ਰਸਤਾ_is_mount_ਬਿੰਦੂ_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਪੂਰਨ flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_is_mount_ਬਿੰਦੂ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਵਾਪਸ ਰਸਤਾ_is_mount_ਬਿੰਦੂ_ਭਰਿਆ(ਰਸਤਾ, ਨਿਰਾ, 0);
}

bool fstype_is_ਜਾਲ(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_needs_quota(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_has_ਅੰਦਰੂਨੀ_quota(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_is_api_vfs(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_is_blockdev_backed(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_is_ro(ਅਚਲ ਅੱਖਰ *fsype);
bool fstype_can_discard(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_can_ownership(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_can_uid_gid(ਅਚਲ ਅੱਖਰ *fstype);
bool fstype_can_fmask_dmask(ਅਚਲ ਅੱਖਰ *fstype);

ਅਚਲ ਅੱਖਰ* fstype_norecovery_option(ਅਚਲ ਅੱਖਰ *fstype);

ਪੂਰਨ dev_is_devtmpfs(ਖਾਲੀ);

ਪੂਰਨ mount_nofollow(
                ਅਚਲ ਅੱਖਰ *ਸਰੋਤ,
                ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ,
                ਅਚਲ ਅੱਖਰ *filesystemtype,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mountflags,
                ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ);

ਅਚਲ ਅੱਖਰ* mount_ਪ੍ਰਸਾਰ_flag_ਮੰਜ਼ਿਲ_string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags);
ਪੂਰਨ mount_ਪ੍ਰਸਾਰ_flag_from_string(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret);
bool mount_ਪ੍ਰਸਾਰ_flag_is_valid(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flag);

bool mount_new_api_supported(ਖਾਲੀ);

ਪੂਰਨ mount_option_supported(ਅਚਲ ਅੱਖਰ *fstype, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ);

bool ਰਸਤਾ_ਹੇਠਾਂ_api_vfs(ਅਚਲ ਅੱਖਰ *p);
