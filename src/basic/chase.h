/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ChaseFlags {
        CHASE_PREFIX_ಬೇರು        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

ಪೂರ್ಣಾಙ್ಕ statx_unsafe_transition(ಧ್ರುವ ಸಂರಚನಾ statx *a, ಧ್ರುವ ಸಂರಚನಾ statx *b);
bool stat_unsafe_transition(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

ಪೂರ್ಣಾಙ್ಕ chase(ಧ್ರುವ ವರ್ಣ *path_with_prefix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags flags, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);

ಪೂರ್ಣಾಙ್ಕ chaseat_prefix_ಬೇರು(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ chase_extract_filename(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ chase_and_open(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ open_flags, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ chase_and_opendir(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ವರ್ಣ **ret_path, DIR **ret_dir);
ಪೂರ್ಣಾಙ್ಕ chase_and_stat(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ವರ್ಣ **ret_path, ಸಂರಚನಾ stat *ret_stat);
ಪೂರ್ಣಾಙ್ಕ chase_and_access(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ access_mode, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ chase_and_fopen_unlocked(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ಧ್ರುವ ವರ್ಣ *open_flags, ವರ್ಣ **ret_path, FILE **ret_file);
ಪೂರ್ಣಾಙ್ಕ chase_and_unlink(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ unlink_flags, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ chase_and_open_parent(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ChaseFlags chase_flags, ವರ್ಣ **ret_filename);

ಪೂರ್ಣಾಙ್ಕ chaseat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags flags, ವರ್ಣ **ret_path, ಪೂರ್ಣಾಙ್ಕ *ret_fd);

ಪೂರ್ಣಾಙ್ಕ chase_and_openat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ open_flags, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ chase_and_opendirat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ವರ್ಣ **ret_path, DIR **ret_dir);
ಪೂರ್ಣಾಙ್ಕ chase_and_statat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ವರ್ಣ **ret_path, ಸಂರಚನಾ stat *ret_stat);
ಪೂರ್ಣಾಙ್ಕ chase_and_accessat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ access_mode, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ chase_and_fopenat_unlocked(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಧ್ರುವ ವರ್ಣ *open_flags, ವರ್ಣ **ret_path, FILE **ret_file);
ಪೂರ್ಣಾಙ್ಕ chase_and_unlinkat(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ಪೂರ್ಣಾಙ್ಕ unlink_flags, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ chase_and_open_parent_at(ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ChaseFlags chase_flags, ವರ್ಣ **ret_filename);
