/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ChaseFlags {
        CHASE_PREFIX_ମୂଳ        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_unsafe_transition(ଅଚଳ ସଂରଚନା statx *a, ଅଚଳ ସଂରଚନା statx *b);
bool stat_unsafe_transition(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase(ଅଚଳ ଅକ୍ଷର *path_with_prefix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags flags, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chaseat_prefix_ମୂଳ(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_extract_filename(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_open(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, ଅକ୍ଷର **ret_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_opendir(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, DIR **ret_dir);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_stat(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, ସଂରଚନା stat *ret_stat);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_access(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_mode, ଅକ୍ଷର **ret_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_fopen_unlocked(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅଚଳ ଅକ୍ଷର *open_flags, ଅକ୍ଷର **ret_path, FILE **ret_file);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_unlink(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlink_flags, ଅକ୍ଷର **ret_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_open_parent(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ChaseFlags chase_flags, ଅକ୍ଷର **ret_filename);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chaseat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags flags, ଅକ୍ଷର **ret_path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_openat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, ଅକ୍ଷର **ret_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_opendirat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, DIR **ret_dir);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_statat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅକ୍ଷର **ret_path, ସଂରଚନା stat *ret_stat);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_accessat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_mode, ଅକ୍ଷର **ret_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_fopenat_unlocked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅଚଳ ଅକ୍ଷର *open_flags, ଅକ୍ଷର **ret_path, FILE **ret_file);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_unlinkat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlink_flags, ଅକ୍ଷର **ret_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା chase_and_open_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ChaseFlags chase_flags, ଅକ୍ଷର **ret_filename);
