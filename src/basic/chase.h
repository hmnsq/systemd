/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع شمارش ChaseFlags {
        CHASE_PREFIX_ریشه        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

صحیح statx_unsafe_transition(ثابت ساختار statx *a, ثابت ساختار statx *b);
bool stat_unsafe_transition(ثابت ساختار stat *a, ثابت ساختار stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

صحیح chase(ثابت نویسه *path_with_prefix, ثابت نویسه *ریشه, ChaseFlags flags, نویسه **ret_path, صحیح *ret_fd);

صحیح chaseat_prefix_ریشه(ثابت نویسه *path, ثابت نویسه *ریشه, نویسه **ret);
صحیح chase_extract_filename(ثابت نویسه *path, ثابت نویسه *ریشه, نویسه **ret);

صحیح chase_and_open(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, صحیح open_flags, نویسه **ret_path);
صحیح chase_and_opendir(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, نویسه **ret_path, DIR **ret_dir);
صحیح chase_and_stat(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, نویسه **ret_path, ساختار stat *ret_stat);
صحیح chase_and_access(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, صحیح access_mode, نویسه **ret_path);
صحیح chase_and_fopen_unlocked(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, ثابت نویسه *open_flags, نویسه **ret_path, FILE **ret_file);
صحیح chase_and_unlink(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, صحیح unlink_flags, نویسه **ret_path);
صحیح chase_and_open_parent(ثابت نویسه *path, ثابت نویسه *ریشه, ChaseFlags chase_flags, نویسه **ret_filename);

صحیح chaseat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags flags, نویسه **ret_path, صحیح *ret_fd);

صحیح chase_and_openat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, صحیح open_flags, نویسه **ret_path);
صحیح chase_and_opendirat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, نویسه **ret_path, DIR **ret_dir);
صحیح chase_and_statat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, نویسه **ret_path, ساختار stat *ret_stat);
صحیح chase_and_accessat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, صحیح access_mode, نویسه **ret_path);
صحیح chase_and_fopenat_unlocked(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, ثابت نویسه *open_flags, نویسه **ret_path, FILE **ret_file);
صحیح chase_and_unlinkat(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, صحیح unlink_flags, نویسه **ret_path);
صحیح chase_and_open_parent_at(صحیح ریشه_fd, صحیح dir_fd, ثابت نویسه *path, ChaseFlags chase_flags, نویسه **ret_filename);
