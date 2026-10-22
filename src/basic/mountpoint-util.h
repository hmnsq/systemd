/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

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

bool is_name_to_handle_at_fatal_error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_to_handle_at_loop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ସଂରଚନା file_handle **ret_handle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mnt_id, uint64_t *ret_unique_mnt_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_to_handle_at_try_fid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ସଂରଚନା file_handle **ret_handle, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mnt_id, uint64_t *ret_unique_mnt_id, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_to_handle_at_u64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_to_handle_u64(ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret) {
        ଫେରନ୍ତୁ name_to_handle_at_u64(AT_FDCWD, path, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_to_handle_u64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ଫେରନ୍ତୁ name_to_handle_at_u64(fd, ଖାଲି, ret);
}

bool file_handle_equal(ଅଚଳ ସଂରଚନା file_handle *a, ଅଚଳ ସଂରଚନା file_handle *b);
ସଂରଚନା file_handle* file_handle_dup(ଅଚଳ ସଂରଚନା file_handle *fh);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_get_mnt_id_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_get_mnt_id(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        ଫେରନ୍ତୁ path_get_mnt_id_at(AT_FDCWD, path, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_get_unique_mnt_id_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_mount_point_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_mount_point_full(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_mount_point(ଅଚଳ ଅକ୍ଷର *path) {
        ଫେରନ୍ତୁ path_is_mount_point_full(path, ଖାଲି, 0);
}

bool fstype_is_network(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_needs_quota(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_has_internal_quota(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_is_api_vfs(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_is_blockdev_backed(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_is_ro(ଅଚଳ ଅକ୍ଷର *fsype);
bool fstype_can_discard(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_can_ownership(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_can_uid_gid(ଅଚଳ ଅକ୍ଷର *fstype);
bool fstype_can_fmask_dmask(ଅଚଳ ଅକ୍ଷର *fstype);

ଅଚଳ ଅକ୍ଷର* fstype_norecovery_option(ଅଚଳ ଅକ୍ଷର *fstype);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dev_is_devtmpfs(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_nofollow(
                ଅଚଳ ଅକ୍ଷର *source,
                ଅଚଳ ଅକ୍ଷର *target,
                ଅଚଳ ଅକ୍ଷର *filesystemtype,
                unsigned long mountflags,
                ଅଚଳ ଶୂନ୍ୟ *data);

ଅଚଳ ଅକ୍ଷର* mount_propagation_flag_to_string(unsigned long flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_propagation_flag_from_string(ଅଚଳ ଅକ୍ଷର *name, unsigned long *ret);
bool mount_propagation_flag_is_valid(unsigned long flag);

bool mount_new_api_supported(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_option_supported(ଅଚଳ ଅକ୍ଷର *fstype, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ);

bool path_below_api_vfs(ଅଚଳ ଅକ୍ଷର *p);
