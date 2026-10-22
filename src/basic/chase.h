/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના ChaseFlags {
        CHASE_PREFIX_મૂળ        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

પૂર્ણાંક statx_unsafe_transition(અચલ સંરચના statx *a, અચલ સંરચના statx *b);
bool stat_unsafe_transition(અચલ સંરચના stat *a, અચલ સંરચના stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

પૂર્ણાંક chase(અચલ અક્ષર *path_with_prefix, અચલ અક્ષર *મૂળ, ChaseFlags flags, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);

પૂર્ણાંક chaseat_prefix_મૂળ(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, અક્ષર **ret);
પૂર્ણાંક chase_extract_filename(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, અક્ષર **ret);

પૂર્ણાંક chase_and_open(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, પૂર્ણાંક open_flags, અક્ષર **ret_path);
પૂર્ણાંક chase_and_opendir(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, અક્ષર **ret_path, DIR **ret_dir);
પૂર્ણાંક chase_and_stat(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, અક્ષર **ret_path, સંરચના stat *ret_stat);
પૂર્ણાંક chase_and_access(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, પૂર્ણાંક access_mode, અક્ષર **ret_path);
પૂર્ણાંક chase_and_fopen_unlocked(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, અચલ અક્ષર *open_flags, અક્ષર **ret_path, FILE **ret_file);
પૂર્ણાંક chase_and_unlink(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, પૂર્ણાંક unlink_flags, અક્ષર **ret_path);
પૂર્ણાંક chase_and_open_parent(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ChaseFlags chase_flags, અક્ષર **ret_filename);

પૂર્ણાંક chaseat(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags flags, અક્ષર **ret_path, પૂર્ણાંક *ret_fd);

પૂર્ણાંક chase_and_openat(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, પૂર્ણાંક open_flags, અક્ષર **ret_path);
પૂર્ણાંક chase_and_opendirat(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, અક્ષર **ret_path, DIR **ret_dir);
પૂર્ણાંક chase_and_statat(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, અક્ષર **ret_path, સંરચના stat *ret_stat);
પૂર્ણાંક chase_and_accessat(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, પૂર્ણાંક access_mode, અક્ષર **ret_path);
પૂર્ણાંક chase_and_fopenat_unlocked(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, અચલ અક્ષર *open_flags, અક્ષર **ret_path, FILE **ret_file);
પૂર્ણાંક chase_and_unlinkat(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, પૂર્ણાંક unlink_flags, અક્ષર **ret_path);
પૂર્ણાંક chase_and_open_parent_at(પૂર્ણાંક મૂળ_fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, ChaseFlags chase_flags, અક્ષર **ret_filename);
