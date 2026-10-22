/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা ChaseFlags {
        CHASE_PREFIX_মূল        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
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

পূর্ণসংখ্যা statx_unsafe_transition(ধ্রুবক কাঠামো statx *a, ধ্রুবক কাঠামো statx *b);
bool stat_unsafe_transition(ধ্রুবক কাঠামো stat *a, ধ্রুবক কাঠামো stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

পূর্ণসংখ্যা chase(ধ্রুবক অক্ষর *path_with_prefix, ধ্রুবক অক্ষর *মূল, ChaseFlags flags, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd);

পূর্ণসংখ্যা chaseat_prefix_মূল(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, অক্ষর **ret);
পূর্ণসংখ্যা chase_extract_filename(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, অক্ষর **ret);

পূর্ণসংখ্যা chase_and_open(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, পূর্ণসংখ্যা open_flags, অক্ষর **ret_path);
পূর্ণসংখ্যা chase_and_opendir(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, অক্ষর **ret_path, DIR **ret_dir);
পূর্ণসংখ্যা chase_and_stat(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, অক্ষর **ret_path, কাঠামো stat *ret_stat);
পূর্ণসংখ্যা chase_and_access(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, পূর্ণসংখ্যা access_mode, অক্ষর **ret_path);
পূর্ণসংখ্যা chase_and_fopen_unlocked(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, ধ্রুবক অক্ষর *open_flags, অক্ষর **ret_path, FILE **ret_file);
পূর্ণসংখ্যা chase_and_unlink(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, পূর্ণসংখ্যা unlink_flags, অক্ষর **ret_path);
পূর্ণসংখ্যা chase_and_open_parent(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ChaseFlags chase_flags, অক্ষর **ret_filename);

পূর্ণসংখ্যা chaseat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags flags, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd);

পূর্ণসংখ্যা chase_and_openat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, পূর্ণসংখ্যা open_flags, অক্ষর **ret_path);
পূর্ণসংখ্যা chase_and_opendirat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, অক্ষর **ret_path, DIR **ret_dir);
পূর্ণসংখ্যা chase_and_statat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, অক্ষর **ret_path, কাঠামো stat *ret_stat);
পূর্ণসংখ্যা chase_and_accessat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, পূর্ণসংখ্যা access_mode, অক্ষর **ret_path);
পূর্ণসংখ্যা chase_and_fopenat_unlocked(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, ধ্রুবক অক্ষর *open_flags, অক্ষর **ret_path, FILE **ret_file);
পূর্ণসংখ্যা chase_and_unlinkat(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, পূর্ণসংখ্যা unlink_flags, অক্ষর **ret_path);
পূর্ণসংখ্যা chase_and_open_parent_at(পূর্ণসংখ্যা মূল_fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ChaseFlags chase_flags, অক্ষর **ret_filename);
