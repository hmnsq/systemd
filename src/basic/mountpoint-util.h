/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* The limit used for /dev itself. 4MB should be enough since device nodes and symlinks don't
 * consume any space and udev isn't supposed to create regular file either. There's no limit on the
 * max number of inodes since such limit is hard to guess especially on large storage array
 * systems. */
#घोषणा TMPFS_LIMITS_DEV             ",size=4m"

/* The limit used for /dev in private namespaces. 4MB for contents of regular files. The number of
 * inodes should be relatively low in private namespaces but for now use a 64k limit. */
#घोषणा TMPFS_LIMITS_PRIVATE_DEV     ",size=4m,nr_inodes=64k"

/* Very little, if any use expected */
#घोषणा TMPFS_LIMITS_EMPTY_OR_ALMOST ",size=4m,nr_inodes=1k"
#घोषणा TMPFS_LIMITS_SYS             TMPFS_LIMITS_EMPTY_OR_ALMOST

/* On an extremely small device with only 256MB of RAM, 20% of RAM should be enough for the re-execution of
 * PID1 because 16MB of free space is required. */
#घोषणा TMPFS_LIMITS_RUN             ",size=20%,nr_inodes=800k"

/* The limit used for various nested tmpfs mounts, in particular for guests started by systemd-nspawn.
 * 10% of RAM (using 16GB of RAM as a baseline) translates to 400k inodes (assuming 4k each) and 25%
 * translates to 1M inodes.
 * (On the host, /tmp is configured through a .mount unit file.) */
#घोषणा NESTED_TMPFS_LIMITS          ",size=10%,nr_inodes=400k"

/* More space for volatile root and /var */
#घोषणा TMPFS_LIMITS_VAR             ",size=25%,nr_inodes=1m"
#घोषणा TMPFS_LIMITS_ROOTFS          TMPFS_LIMITS_VAR
#घोषणा TMPFS_LIMITS_VOLATILE_STATE  TMPFS_LIMITS_VAR

bool is_name_to_handle_at_fatal_error(पूर्णांक err);

पूर्णांक name_to_handle_at_loop(पूर्णांक fd, स्थिर अक्षर *path, काष्ठा file_handle **ret_handle, पूर्णांक *ret_mnt_id, uint64_t *ret_unique_mnt_id, पूर्णांक flags);
पूर्णांक name_to_handle_at_try_fid(पूर्णांक fd, स्थिर अक्षर *path, काष्ठा file_handle **ret_handle, पूर्णांक *ret_mnt_id, uint64_t *ret_unique_mnt_id, पूर्णांक flags);
पूर्णांक name_to_handle_at_u64(पूर्णांक fd, स्थिर अक्षर *path, uint64_t *ret);
अटल अंतरभूत पूर्णांक path_to_handle_u64(स्थिर अक्षर *path, uint64_t *ret) {
        वापस name_to_handle_at_u64(AT_FDCWD, path, ret);
}
अटल अंतरभूत पूर्णांक fd_to_handle_u64(पूर्णांक fd, uint64_t *ret) {
        वापस name_to_handle_at_u64(fd, शून्य, ret);
}

bool file_handle_equal(स्थिर काष्ठा file_handle *a, स्थिर काष्ठा file_handle *b);
काष्ठा file_handle* file_handle_dup(स्थिर काष्ठा file_handle *fh);

पूर्णांक path_get_mnt_id_at(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक *ret);
अटल अंतरभूत पूर्णांक path_get_mnt_id(स्थिर अक्षर *path, पूर्णांक *ret) {
        वापस path_get_mnt_id_at(AT_FDCWD, path, ret);
}
पूर्णांक path_get_unique_mnt_id_at(पूर्णांक dir_fd, स्थिर अक्षर *path, uint64_t *ret);

पूर्णांक is_mount_point_at(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक flags);
पूर्णांक path_is_mount_point_full(स्थिर अक्षर *path, स्थिर अक्षर *मूल, पूर्णांक flags);
अटल अंतरभूत पूर्णांक path_is_mount_point(स्थिर अक्षर *path) {
        वापस path_is_mount_point_full(path, शून्य, 0);
}

bool fstype_is_network(स्थिर अक्षर *fstype);
bool fstype_needs_quota(स्थिर अक्षर *fstype);
bool fstype_has_internal_quota(स्थिर अक्षर *fstype);
bool fstype_is_api_vfs(स्थिर अक्षर *fstype);
bool fstype_is_blockdev_backed(स्थिर अक्षर *fstype);
bool fstype_is_ro(स्थिर अक्षर *fsype);
bool fstype_can_discard(स्थिर अक्षर *fstype);
bool fstype_can_ownership(स्थिर अक्षर *fstype);
bool fstype_can_uid_gid(स्थिर अक्षर *fstype);
bool fstype_can_fmask_dmask(स्थिर अक्षर *fstype);

स्थिर अक्षर* fstype_norecovery_option(स्थिर अक्षर *fstype);

पूर्णांक dev_is_devtmpfs(व्योम);

पूर्णांक mount_nofollow(
                स्थिर अक्षर *source,
                स्थिर अक्षर *target,
                स्थिर अक्षर *filesystemtype,
                अचिन्हित दीर्घ mountflags,
                स्थिर व्योम *तथ्य);

स्थिर अक्षर* mount_propagation_flag_to_string(अचिन्हित दीर्घ flags);
पूर्णांक mount_propagation_flag_from_string(स्थिर अक्षर *name, अचिन्हित दीर्घ *ret);
bool mount_propagation_flag_is_valid(अचिन्हित दीर्घ flag);

bool mount_new_api_supported(व्योम);

पूर्णांक mount_option_supported(स्थिर अक्षर *fstype, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य);

bool path_below_api_vfs(स्थिर अक्षर *p);
