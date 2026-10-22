/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை ChaseFlags {
        CHASE_PREFIX_வேர்        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

முழு statx_unsafe_transition(மாறிலி கட்டமைப்பு statx *a, மாறிலி கட்டமைப்பு statx *b);
bool stat_unsafe_transition(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

முழு chase(மாறிலி எழுத்து *path_with_prefix, மாறிலி எழுத்து *வேர், ChaseFlags flags, எழுத்து **ret_path, முழு *ret_fd);

முழு chaseat_prefix_வேர்(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், எழுத்து **ret);
முழு chase_extract_filename(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், எழுத்து **ret);

முழு chase_and_open(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, முழு open_flags, எழுத்து **ret_path);
முழு chase_and_opendir(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, எழுத்து **ret_path, DIR **ret_dir);
முழு chase_and_stat(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, எழுத்து **ret_path, கட்டமைப்பு stat *ret_stat);
முழு chase_and_access(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, முழு access_mode, எழுத்து **ret_path);
முழு chase_and_fopen_unlocked(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, மாறிலி எழுத்து *open_flags, எழுத்து **ret_path, FILE **ret_file);
முழு chase_and_unlink(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, முழு unlink_flags, எழுத்து **ret_path);
முழு chase_and_open_parent(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ChaseFlags chase_flags, எழுத்து **ret_filename);

முழு chaseat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags flags, எழுத்து **ret_path, முழு *ret_fd);

முழு chase_and_openat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, முழு open_flags, எழுத்து **ret_path);
முழு chase_and_opendirat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, எழுத்து **ret_path, DIR **ret_dir);
முழு chase_and_statat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, எழுத்து **ret_path, கட்டமைப்பு stat *ret_stat);
முழு chase_and_accessat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, முழு access_mode, எழுத்து **ret_path);
முழு chase_and_fopenat_unlocked(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, மாறிலி எழுத்து *open_flags, எழுத்து **ret_path, FILE **ret_file);
முழு chase_and_unlinkat(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, முழு unlink_flags, எழுத்து **ret_path);
முழு chase_and_open_parent_at(முழு வேர்_fd, முழு dir_fd, மாறிலி எழுத்து *path, ChaseFlags chase_flags, எழுத்து **ret_filename);
