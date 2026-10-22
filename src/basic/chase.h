/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع تعداد ChaseFlags {
        CHASE_اگیتر_جڑ        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
        CHASE_NONEXISTENT        = 1 << 1,  /* It's OK if the path doesn't actually exist. */
        CHASE_NO_AUTOFS          = 1 << 2,  /* Return -EREMOTE if autofs mount point found */
        CHASE_TRIGGER_AUTOFS     = 1 << 3,  /* Use open_tree() rather than openat() to trigger autofs mount */
        CHASE_محفوظ               = 1 << 4,  /* Return -EPERM if we ever traverse from unprivileged to privileged files or directories */
        CHASE_TRAIL_SLASH        = 1 << 5,  /* Any trailing slash will be preserved */
        CHASE_قدم               = 1 << 6,  /* Just execute a single step of the normalization */
        CHASE_NOFOLLOW           = 1 << 7,  /* Do not follow the path's right-most component. With ret_fd, when the path's
                                             * right-most component refers to symlink, return O_PATH fd of the symlink. */
        CHASE_WARN               = 1 << 8,  /* Emit an appropriate warning when an error is encountered.
                                             * Note: this may do an NSS lookup, hence this flag cannot be used in PID 1. */
        CHASE_PROHIBIT_SYMLINKS  = 1 << 9,  /* Refuse all symlinks */
        CHASE_ماپا             = 1 << 10, /* Chase the parent directory of the given path. Note that the
                                             * full path is still stored in ret_path and only the returned
                                             * file descriptor will point to the parent directory. Note that
                                             * the result path is the root or '.', then the file descriptor
                                             * also points to the result path even if this flag is set.
                                             * When this specified, chase() will succeed with 1 even if the
                                             * file points to the last path component does not exist. */
        CHASE_MKDIR_0755         = 1 << 11, /* Create any missing directories in the given path. */
        CHASE_کڈھو_FILENAME   = 1 << 12, /* Only return the last component of the resolved path */
        CHASE_MUST_BE_DIRECTORY  = 1 << 13, /* Fail if returned inode fd is not a dir */
        CHASE_MUST_BE_REGULAR    = 1 << 14, /* Fail if returned inode fd is not a regular file */
        CHASE_MUST_BE_SOCKET     = 1 << 15, /* Fail if returned inode fd is not a socket */
} ChaseFlags;

صحيح statx_unsafe_transition(ثابت بنية statx *a, ثابت بنية statx *b);
bool stat_unsafe_transition(ثابت بنية stat *a, ثابت بنية stat *b);

/* How many iterations to execute before returning -ELOOP */
#تعریف_کرو CHASE_MAX 128U

صحيح chase(ثابت حرف *رستہ_with_اگیتر, ثابت حرف *جڑ, ChaseFlags flags, حرف **ret_رستہ, صحيح *ret_fd);

صحيح chaseat_اگیتر_جڑ(ثابت حرف *رستہ, ثابت حرف *جڑ, حرف **ret);
صحيح chase_کڈھو_filename(ثابت حرف *رستہ, ثابت حرف *جڑ, حرف **ret);

صحيح chase_and_open(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحيح open_flags, حرف **ret_رستہ);
صحيح chase_and_opendir(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_رستہ, DIR **ret_dir);
صحيح chase_and_stat(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_رستہ, بنية stat *ret_stat);
صحيح chase_and_access(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحيح access_منوال, حرف **ret_رستہ);
صحيح chase_and_fopen_unlocked(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, ثابت حرف *open_flags, حرف **ret_رستہ, FILE **ret_file);
صحيح chase_and_unlink(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, صحيح unlink_flags, حرف **ret_رستہ);
صحيح chase_and_open_ماپا(ثابت حرف *رستہ, ثابت حرف *جڑ, ChaseFlags chase_flags, حرف **ret_filename);

صحيح chaseat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags flags, حرف **ret_رستہ, صحيح *ret_fd);

صحيح chase_and_openat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, صحيح open_flags, حرف **ret_رستہ);
صحيح chase_and_opendirat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, حرف **ret_رستہ, DIR **ret_dir);
صحيح chase_and_statat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, حرف **ret_رستہ, بنية stat *ret_stat);
صحيح chase_and_accessat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, صحيح access_منوال, حرف **ret_رستہ);
صحيح chase_and_fopenat_unlocked(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, ثابت حرف *open_flags, حرف **ret_رستہ, FILE **ret_file);
صحيح chase_and_unlinkat(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, صحيح unlink_flags, حرف **ret_رستہ);
صحيح chase_and_open_ماپا_at(صحيح جڑ_fd, صحيح dir_fd, ثابت حرف *رستہ, ChaseFlags chase_flags, حرف **ret_filename);
