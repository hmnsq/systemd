/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना ChaseFlags {
        CHASE_PREFIX_जरा        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
        CHASE_NONEXISTENT        = 1 << 1,  /* It's OK if the path doesn't actually exist. */
        CHASE_NO_AUTOFS          = 1 << 2,  /* Return -EREMOTE if autofs mount point found */
        CHASE_TRIGGER_AUTOFS     = 1 << 3,  /* Use open_tree() rather than openat() to trigger autofs mount */
        CHASE_SAFE               = 1 << 4,  /* Return -EPERM if we ever traverse from unprivileged to privileged files or directories */
        CHASE_TRAIL_SLASH        = 1 << 5,  /* Any trailing slash will be preserved */
        CHASE_STEP               = 1 << 6,  /* Just execute a single step of the normalization */
        CHASE_NOFOLLOW           = 1 << 7,  /* Do not follow the path's right-most component. With ret_fd, when the path's
                                             * right-most component refers to symlink, return O_PATH fd of the symlink. */
        CHASE_WARN               = 1 << 8,  /* Emit an appropriate warning when an error is encountered.
                                             * Note: this may do an NSS lookup, hence this flag cannot be used in PID 1. */
        CHASE_PROHIBIT_SYMLINKS  = 1 << 9,  /* Refuse all symlinks */
        CHASE_PARENT             = 1 << 10, /* Chase the parent directory of the given path. Note that the
                                             * full path is still stored in ret_path and only the returned
                                             * file descriptor will point to the parent directory. Note that
                                             * the result path is the root or '.', then the file descriptor
                                             * also points to the result path even if this flag is set.
                                             * When this specified, chase() will succeed with 1 even if the
                                             * file points to the last path component does not exist. */
        CHASE_MKDIR_0755         = 1 << 11, /* Create any missing directories in the given path. */
        CHASE_EXTRACT_FILENAME   = 1 << 12, /* Only return the last component of the resolved path */
        CHASE_MUST_BE_DIRECTORY  = 1 << 13, /* Fail if returned inode fd is not a dir */
        CHASE_MUST_BE_REGULAR    = 1 << 14, /* Fail if returned inode fd is not a regular file */
        CHASE_MUST_BE_SOCKET     = 1 << 15, /* Fail if returned inode fd is not a socket */
} ChaseFlags;

पूर्णाङ्क statx_unsafe_transition(अचर संरचना statx *a, अचर संरचना statx *b);
bool stat_unsafe_transition(अचर संरचना stat *a, अचर संरचना stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

पूर्णाङ्क chase(अचर वर्ण *path_with_prefix, अचर वर्ण *जरा, ChaseFlags flags, वर्ण **ret_path, पूर्णाङ्क *ret_fd);

पूर्णाङ्क chaseat_prefix_जरा(अचर वर्ण *path, अचर वर्ण *जरा, वर्ण **ret);
पूर्णाङ्क chase_extract_filename(अचर वर्ण *path, अचर वर्ण *जरा, वर्ण **ret);

पूर्णाङ्क chase_and_open(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, पूर्णाङ्क open_flags, वर्ण **ret_path);
पूर्णाङ्क chase_and_opendir(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, वर्ण **ret_path, सूची **ret_dir);
पूर्णाङ्क chase_and_stat(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, वर्ण **ret_path, संरचना stat *ret_stat);
पूर्णाङ्क chase_and_access(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, पूर्णाङ्क access_mode, वर्ण **ret_path);
पूर्णाङ्क chase_and_fopen_unlocked(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, अचर वर्ण *open_flags, वर्ण **ret_path, खाता **ret_file);
पूर्णाङ्क chase_and_unlink(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, पूर्णाङ्क unlink_flags, वर्ण **ret_path);
पूर्णाङ्क chase_and_open_parent(अचर वर्ण *path, अचर वर्ण *जरा, ChaseFlags chase_flags, वर्ण **ret_filename);

पूर्णाङ्क chaseat(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags flags, वर्ण **ret_path, पूर्णाङ्क *ret_fd);

पूर्णाङ्क chase_and_openat(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, पूर्णाङ्क open_flags, वर्ण **ret_path);
पूर्णाङ्क chase_and_opendirat(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, वर्ण **ret_path, सूची **ret_dir);
पूर्णाङ्क chase_and_statat(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, वर्ण **ret_path, संरचना stat *ret_stat);
पूर्णाङ्क chase_and_accessat(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, पूर्णाङ्क access_mode, वर्ण **ret_path);
पूर्णाङ्क chase_and_fopenat_unlocked(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, अचर वर्ण *open_flags, वर्ण **ret_path, खाता **ret_file);
पूर्णाङ्क chase_and_unlinkat(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, पूर्णाङ्क unlink_flags, वर्ण **ret_path);
पूर्णाङ्क chase_and_open_parent_at(पूर्णाङ्क जरा_fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, ChaseFlags chase_flags, वर्ण **ret_filename);
