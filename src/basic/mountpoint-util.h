/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#నిర్వచించు TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#నిర్వచించు TMPFS_LIMITS_వ్యక్తిగత_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#నిర్వచించు TMPFS_LIMITS_ఖాళీలు_OR_ALMOST ",size=4m,nr_inodes=1k"
#నిర్వచించు TMPFS_LIMITS_SYS             TMPFS_LIMITS_ఖాళీలు_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#నిర్వచించు TMPFS_LIMITS_నిరంతరం             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#నిర్వచించు NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#నిర్వచించు TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#నిర్వచించు TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#నిర్వచించు TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_గమ్యం_handle_at_fatal_error(పూర్ణాంకం err);

పూర్ణాంకం name_గమ్యం_handle_at_loop(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, నిర్మాణం file_handle **ret_handle, పూర్ణాంకం *ret_mnt_id, uint64_t *ret_unique_mnt_id, పూర్ణాంకం flags);
పూర్ణాంకం name_గమ్యం_handle_at_try_fid(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, నిర్మాణం file_handle **ret_handle, పూర్ణాంకం *ret_mnt_id, uint64_t *ret_unique_mnt_id, పూర్ణాంకం flags);
పూర్ణాంకం name_గమ్యం_handle_at_u64(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret);
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_గమ్యం_handle_u64(స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret) {
        తిరిగి name_గమ్యం_handle_at_u64(AT_FDCWD, మార్గం, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం fd_గమ్యం_handle_u64(పూర్ణాంకం fd, uint64_t *ret) {
        తిరిగి name_గమ్యం_handle_at_u64(fd, ఖాళీ, ret);
}

bool file_handle_equal(స్థిరరాశి నిర్మాణం file_handle *a, స్థిరరాశి నిర్మాణం file_handle *b);
నిర్మాణం file_handle* file_handle_dup(స్థిరరాశి నిర్మాణం file_handle *fh);

పూర్ణాంకం మార్గం_get_mnt_id_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం *ret);
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_get_mnt_id(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం *ret) {
        తిరిగి మార్గం_get_mnt_id_at(AT_FDCWD, మార్గం, ret);
}
పూర్ణాంకం మార్గం_get_unique_mnt_id_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret);

పూర్ణాంకం is_mount_బిందువు_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags);
పూర్ణాంకం మార్గం_is_mount_బిందువు_నిండిన(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, పూర్ణాంకం flags);
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_is_mount_బిందువు(స్థిరరాశి అక్షరం *మార్గం) {
        తిరిగి మార్గం_is_mount_బిందువు_నిండిన(మార్గం, ఖాళీ, 0);
}

bool fstype_is_జాలం(స్థిరరాశి అక్షరం *fstype);
bool fstype_needs_quota(స్థిరరాశి అక్షరం *fstype);
bool fstype_has_అంతర్గత_quota(స్థిరరాశి అక్షరం *fstype);
bool fstype_is_api_vfs(స్థిరరాశి అక్షరం *fstype);
bool fstype_is_blockdev_backed(స్థిరరాశి అక్షరం *fstype);
bool fstype_is_ro(స్థిరరాశి అక్షరం *fsype);
bool fstype_can_discard(స్థిరరాశి అక్షరం *fstype);
bool fstype_can_ownership(స్థిరరాశి అక్షరం *fstype);
bool fstype_can_uid_gid(స్థిరరాశి అక్షరం *fstype);
bool fstype_can_fmask_dmask(స్థిరరాశి అక్షరం *fstype);

స్థిరరాశి అక్షరం* fstype_norecovery_option(స్థిరరాశి అక్షరం *fstype);

పూర్ణాంకం dev_is_devtmpfs(శూన్య);

పూర్ణాంకం mount_nofollow(
                స్థిరరాశి అక్షరం *ఆధారం,
                స్థిరరాశి అక్షరం *లక్ష్యం,
                స్థిరరాశి అక్షరం *filesystemtype,
                గుర్తులేని దీర్ఘ mountflags,
                స్థిరరాశి శూన్య *దత్తాంశం);

స్థిరరాశి అక్షరం* mount_ప్రసరణ_flag_గమ్యం_string(గుర్తులేని దీర్ఘ flags);
పూర్ణాంకం mount_ప్రసరణ_flag_from_string(స్థిరరాశి అక్షరం *name, గుర్తులేని దీర్ఘ *ret);
bool mount_ప్రసరణ_flag_is_valid(గుర్తులేని దీర్ఘ flag);

bool mount_new_api_supported(శూన్య);

పూర్ణాంకం mount_option_supported(స్థిరరాశి అక్షరం *fstype, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);

bool మార్గం_క్రింద_api_vfs(స్థిరరాశి అక్షరం *p);
