/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#تعریف_کړه TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#تعریف_کړه TMPFS_LIMITS_شخصي_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#تعریف_کړه TMPFS_LIMITS_تشه_OR_ALMOST ",size=4m,nr_inodes=1k"
#تعریف_کړه TMPFS_LIMITS_SYS             TMPFS_LIMITS_تشه_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#تعریف_کړه TMPFS_LIMITS_پرلپسې             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#تعریف_کړه NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#تعریف_کړه TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#تعریف_کړه TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#تعریف_کړه TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_منزل_handle_at_fatal_error(صحيح err);

صحيح name_منزل_handle_at_loop(صحيح fd, ثابت حرف *لار, بنية file_handle **ret_handle, صحيح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحيح flags);
صحيح name_منزل_handle_at_try_fid(صحيح fd, ثابت حرف *لار, بنية file_handle **ret_handle, صحيح *ret_mnt_id, uint64_t *ret_unique_mnt_id, صحيح flags);
صحيح name_منزل_handle_at_u64(صحيح fd, ثابت حرف *لار, uint64_t *ret);
ساكن مضمن صحيح لار_منزل_handle_u64(ثابت حرف *لار, uint64_t *ret) {
        أرجع name_منزل_handle_at_u64(AT_FDCWD, لار, ret);
}
ساكن مضمن صحيح fd_منزل_handle_u64(صحيح fd, uint64_t *ret) {
        أرجع name_منزل_handle_at_u64(fd, عدم, ret);
}

bool file_handle_equal(ثابت بنية file_handle *a, ثابت بنية file_handle *b);
بنية file_handle* file_handle_dup(ثابت بنية file_handle *fh);

صحيح لار_get_mnt_id_at(صحيح dir_fd, ثابت حرف *لار, صحيح *ret);
ساكن مضمن صحيح لار_get_mnt_id(ثابت حرف *لار, صحيح *ret) {
        أرجع لار_get_mnt_id_at(AT_FDCWD, لار, ret);
}
صحيح لار_get_unique_mnt_id_at(صحيح dir_fd, ثابت حرف *لار, uint64_t *ret);

صحيح is_mount_ټکی_at(صحيح dir_fd, ثابت حرف *لار, صحيح flags);
صحيح لار_is_mount_ټکی_ډک(ثابت حرف *لار, ثابت حرف *ریښه, صحيح flags);
ساكن مضمن صحيح لار_is_mount_ټکی(ثابت حرف *لار) {
        أرجع لار_is_mount_ټکی_ډک(لار, عدم, 0);
}

bool fstype_is_شبکه(ثابت حرف *fstype);
bool fstype_needs_quota(ثابت حرف *fstype);
bool fstype_has_دننی_quota(ثابت حرف *fstype);
bool fstype_is_api_vfs(ثابت حرف *fstype);
bool fstype_is_blockdev_backed(ثابت حرف *fstype);
bool fstype_is_ro(ثابت حرف *fsype);
bool fstype_can_discard(ثابت حرف *fstype);
bool fstype_can_ownership(ثابت حرف *fstype);
bool fstype_can_uid_gid(ثابت حرف *fstype);
bool fstype_can_fmask_dmask(ثابت حرف *fstype);

ثابت حرف* fstype_norecovery_option(ثابت حرف *fstype);

صحيح dev_is_devtmpfs(فراغ);

صحيح mount_nofollow(
                ثابت حرف *سرچینه,
                ثابت حرف *موخه,
                ثابت حرف *filesystemtype,
                بینښه اوږد mountflags,
                ثابت فراغ *معلومات);

ثابت حرف* mount_انتشار_flag_منزل_string(بینښه اوږد flags);
صحيح mount_انتشار_flag_from_string(ثابت حرف *name, بینښه اوږد *ret);
bool mount_انتشار_flag_is_valid(بینښه اوږد flag);

bool mount_new_api_supported(فراغ);

صحيح mount_option_supported(ثابت حرف *fstype, ثابت حرف *کلی, ثابت حرف *ارزښت);

bool لار_لاندې_api_vfs(ثابت حرف *p);
