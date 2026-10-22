/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#تعریف_کرو TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#تعریف_کرو TMPFS_LIMITS_نجی_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#تعریف_کرو TMPFS_LIMITS_خالی_خانے_OR_ALMOST ",size=4m,nr_inodes=1k"
#تعریف_کرو TMPFS_LIMITS_SYS             TMPFS_LIMITS_خالی_خانے_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#تعریف_کرو TMPFS_LIMITS_تسلسل             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#تعریف_کرو NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#تعریف_کرو TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#تعریف_کرو TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#تعریف_کرو TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_مقصد_handle_at_fatal_error(صحیح err);

صحیح name_مقصد_handle_at_loop(صحیح fd, ثابت حرف *راستہ, ساخت file_handle **ret_handle, صحیح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحیح flags);
صحیح name_مقصد_handle_at_try_fid(صحیح fd, ثابت حرف *راستہ, ساخت file_handle **ret_handle, صحیح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحیح flags);
صحیح name_مقصد_handle_at_u64(صحیح fd, ثابت حرف *راستہ, uint64_t *ret);
ساکن اندرونی صحیح راستہ_مقصد_handle_u64(ثابت حرف *راستہ, uint64_t *ret) {
        واپس name_مقصد_handle_at_u64(AT_FDCWD, راستہ, ret);
}
ساکن اندرونی صحیح fd_مقصد_handle_u64(صحیح fd, uint64_t *ret) {
        واپس name_مقصد_handle_at_u64(fd, تہی, ret);
}

bool file_handle_equal(ثابت ساخت file_handle *a, ثابت ساخت file_handle *b);
ساخت file_handle* file_handle_dup(ثابت ساخت file_handle *fh);

صحیح راستہ_get_mnt_id_at(صحیح dir_fd, ثابت حرف *راستہ, صحیح *ret);
ساکن اندرونی صحیح راستہ_get_mnt_id(ثابت حرف *راستہ, صحیح *ret) {
        واپس راستہ_get_mnt_id_at(AT_FDCWD, راستہ, ret);
}
صحیح راستہ_get_unique_mnt_id_at(صحیح dir_fd, ثابت حرف *راستہ, uint64_t *ret);

صحیح is_mount_نقطہ_at(صحیح dir_fd, ثابت حرف *راستہ, صحیح flags);
صحیح راستہ_is_mount_نقطہ_بھرا(ثابت حرف *راستہ, ثابت حرف *جڑ, صحیح flags);
ساکن اندرونی صحیح راستہ_is_mount_نقطہ(ثابت حرف *راستہ) {
        واپس راستہ_is_mount_نقطہ_بھرا(راستہ, تہی, 0);
}

bool fstype_is_جال(ثابت حرف *fstype);
bool fstype_needs_quota(ثابت حرف *fstype);
bool fstype_has_اندرونی_quota(ثابت حرف *fstype);
bool fstype_is_api_vfs(ثابت حرف *fstype);
bool fstype_is_blockdev_backed(ثابت حرف *fstype);
bool fstype_is_ro(ثابت حرف *fsype);
bool fstype_can_discard(ثابت حرف *fstype);
bool fstype_can_ownership(ثابت حرف *fstype);
bool fstype_can_uid_gid(ثابت حرف *fstype);
bool fstype_can_fmask_dmask(ثابت حرف *fstype);

ثابت حرف* fstype_norecovery_option(ثابت حرف *fstype);

صحیح dev_is_devtmpfs(خالی);

صحیح mount_nofollow(
                ثابت حرف *ماخذ,
                ثابت حرف *ہدف,
                ثابت حرف *filesystemtype,
                غیرعلامتی طویل mountflags,
                ثابت خالی *ڈیٹا);

ثابت حرف* mount_اشاعت_flag_مقصد_string(غیرعلامتی طویل flags);
صحیح mount_اشاعت_flag_from_string(ثابت حرف *name, غیرعلامتی طویل *ret);
bool mount_اشاعت_flag_is_valid(غیرعلامتی طویل flag);

bool mount_new_api_supported(خالی);

صحیح mount_option_supported(ثابت حرف *fstype, ثابت حرف *کلید, ثابت حرف *قدر);

bool راستہ_نیچے_api_vfs(ثابت حرف *p);
