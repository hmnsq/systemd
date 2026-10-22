/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa ChaseFlags {
        CHASE_PREFIX_ROOT        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

ཧྲིལ statx_unsafe_transition(const sa_nracanaa statx *a, const sa_nracanaa statx *b);
bool stat_unsafe_transition(const sa_nracanaa stat *a, const sa_nracanaa stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

ཧྲིལ chase(const var_na *path_with_prefix, const var_na *root, ChaseFlags flags, var_na **ret_path, ཧྲིལ *ret_fd);

ཧྲིལ chaseat_prefix_root(const var_na *path, const var_na *root, var_na **ret);
ཧྲིལ chase_extract_filename(const var_na *path, const var_na *root, var_na **ret);

ཧྲིལ chase_and_open(const var_na *path, const var_na *root, ChaseFlags chase_flags, ཧྲིལ open_flags, var_na **ret_path);
ཧྲིལ chase_and_opendir(const var_na *path, const var_na *root, ChaseFlags chase_flags, var_na **ret_path, DIR **ret_dir);
ཧྲིལ chase_and_stat(const var_na *path, const var_na *root, ChaseFlags chase_flags, var_na **ret_path, sa_nracanaa stat *ret_stat);
ཧྲིལ chase_and_access(const var_na *path, const var_na *root, ChaseFlags chase_flags, ཧྲིལ access_mode, var_na **ret_path);
ཧྲིལ chase_and_fopen_unlocked(const var_na *path, const var_na *root, ChaseFlags chase_flags, const var_na *open_flags, var_na **ret_path, FILE **ret_file);
ཧྲིལ chase_and_unlink(const var_na *path, const var_na *root, ChaseFlags chase_flags, ཧྲིལ unlink_flags, var_na **ret_path);
ཧྲིལ chase_and_open_parent(const var_na *path, const var_na *root, ChaseFlags chase_flags, var_na **ret_filename);

ཧྲིལ chaseat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags flags, var_na **ret_path, ཧྲིལ *ret_fd);

ཧྲིལ chase_and_openat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, ཧྲིལ open_flags, var_na **ret_path);
ཧྲིལ chase_and_opendirat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, var_na **ret_path, DIR **ret_dir);
ཧྲིལ chase_and_statat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, var_na **ret_path, sa_nracanaa stat *ret_stat);
ཧྲིལ chase_and_accessat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, ཧྲིལ access_mode, var_na **ret_path);
ཧྲིལ chase_and_fopenat_unlocked(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, const var_na *open_flags, var_na **ret_path, FILE **ret_file);
ཧྲིལ chase_and_unlinkat(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, ཧྲིལ unlink_flags, var_na **ret_path);
ཧྲིལ chase_and_open_parent_at(ཧྲིལ root_fd, ཧྲིལ dir_fd, const var_na *path, ChaseFlags chase_flags, var_na **ret_filename);
