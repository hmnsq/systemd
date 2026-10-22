/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ChaseFlags {
        CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
        CHASE_NONEXISTENT        = 1 << 1,  /* It's OK if the path doesn't actually exist. */
        CHASE_NO_AUTOFS          = 1 << 2,  /* Return -EREMOTE if autofs mount point found */
        CHASE_TRIGGER_AUTOFS     = 1 << 3,  /* Use open_tree() rather than openat() to trigger autofs mount */
        CHASE_ਸੁਰੱਖਿਅਤ               = 1 << 4,  /* Return -EPERM if we ever traverse from unprivileged to privileged files or directories */
        CHASE_TRAIL_SLASH        = 1 << 5,  /* Any trailing slash will be preserved */
        CHASE_ਕਦਮ               = 1 << 6,  /* Just execute a single step of the normalization */
        CHASE_NOFOLLOW           = 1 << 7,  /* Do not follow the path's right-most component. With ret_fd, when the path's
                                             * right-most component refers to symlink, return O_PATH fd of the symlink. */
        CHASE_WARN               = 1 << 8,  /* Emit an appropriate warning when an error is encountered.
                                             * Note: this may do an NSS lookup, hence this flag cannot be used in PID 1. */
        CHASE_PROHIBIT_SYMLINKS  = 1 << 9,  /* Refuse all symlinks */
        CHASE_ਮਾਪਾ             = 1 << 10, /* Chase the parent directory of the given path. Note that the
                                             * full path is still stored in ret_path and only the returned
                                             * file descriptor will point to the parent directory. Note that
                                             * the result path is the root or '.', then the file descriptor
                                             * also points to the result path even if this flag is set.
                                             * When this specified, chase() will succeed with 1 even if the
                                             * file points to the last path component does not exist. */
        CHASE_MKDIR_0755         = 1 << 11, /* Create any missing directories in the given path. */
        CHASE_ਕੱਢੋ_FILENAME   = 1 << 12, /* Only return the last component of the resolved path */
        CHASE_MUST_BE_DIRECTORY  = 1 << 13, /* Fail if returned inode fd is not a dir */
        CHASE_MUST_BE_REGULAR    = 1 << 14, /* Fail if returned inode fd is not a regular file */
        CHASE_MUST_BE_SOCKET     = 1 << 15, /* Fail if returned inode fd is not a socket */
} ChaseFlags;

ਪੂਰਨ statx_unsafe_transition(ਅਚਲ ਬਣਤਰ statx *a, ਅਚਲ ਬਣਤਰ statx *b);
bool stat_unsafe_transition(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b);

/* How many iterations to execute before returning -ELOOP */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHASE_MAX 128U

ਪੂਰਨ chase(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ_with_ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);

ਪੂਰਨ chaseat_ਅਗੇਤਰ_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret);
ਪੂਰਨ chase_ਕੱਢੋ_filename(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret);

ਪੂਰਨ chase_and_open(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਪੂਰਨ open_flags, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ chase_and_opendir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, DIR **ret_dir);
ਪੂਰਨ chase_and_stat(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਬਣਤਰ stat *ret_stat);
ਪੂਰਨ chase_and_access(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਪੂਰਨ access_ਬਹੁਲਕ, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ chase_and_fopen_unlocked(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅਚਲ ਅੱਖਰ *open_flags, ਅੱਖਰ **ret_ਰਸਤਾ, FILE **ret_file);
ਪੂਰਨ chase_and_unlink(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਪੂਰਨ unlink_flags, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ chase_and_open_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ChaseFlags chase_flags, ਅੱਖਰ **ret_filename);

ਪੂਰਨ chaseat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd);

ਪੂਰਨ chase_and_openat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਪੂਰਨ open_flags, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ chase_and_opendirat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, DIR **ret_dir);
ਪੂਰਨ chase_and_statat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅੱਖਰ **ret_ਰਸਤਾ, ਬਣਤਰ stat *ret_stat);
ਪੂਰਨ chase_and_accessat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਪੂਰਨ access_ਬਹੁਲਕ, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ chase_and_fopenat_unlocked(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅਚਲ ਅੱਖਰ *open_flags, ਅੱਖਰ **ret_ਰਸਤਾ, FILE **ret_file);
ਪੂਰਨ chase_and_unlinkat(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਪੂਰਨ unlink_flags, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ chase_and_open_ਮਾਪਾ_at(ਪੂਰਨ ਜੜ੍ਹ_fd, ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ChaseFlags chase_flags, ਅੱਖਰ **ret_filename);
