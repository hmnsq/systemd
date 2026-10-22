/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <dirent.h>

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना RecurseDirFlags {
        /* Interpreted by readdir_all() */
        RECURSE_DIR_क्रमय              = 1 << 0,  /* sort file directory entries before processing them */
        RECURSE_DIR_IGNORE_DOT        = 1 << 1,  /* ignore all dot files ("." and ".." are always ignored) */
        RECURSE_DIR_ENSURE_TYPE       = 1 << 2,  /* guarantees that 'd_type' field of 'de' is not DT_UNKNOWN */
        RECURSE_DIR_MUST_BE_DIRECTORY = 1 << 3,  /* ignore all entries that aren't directories */
        RECURSE_DIR_MUST_BE_REGULAR   = 1 << 4,  /* ignore all entries that aren't regular files */
        RECURSE_DIR_MUST_BE_SYMLINK   = 1 << 5,  /* ignore all entries that aren't symlinks */
        RECURSE_DIR_MUST_BE_SOCKET    = 1 << 6,  /* ignore all entries that aren't socket */
        RECURSE_DIR_MUST_BE_आवरणम्     = RECURSE_DIR_MUST_BE_DIRECTORY|RECURSE_DIR_MUST_BE_REGULAR|RECURSE_DIR_MUST_BE_SYMLINK|RECURSE_DIR_MUST_BE_SOCKET,

        /* Interpreted by recurse_dir() */
        RECURSE_DIR_SAME_MOUNT        = 1 << 7,  /* skips over subdirectories that are submounts */
        RECURSE_DIR_INODE_FD          = 1 << 8,  /* passes an opened inode fd (O_DIRECTORY fd in case of dirs, O_PATH otherwise) */
        RECURSE_DIR_TOPLEVEL          = 1 << 9,  /* call RECURSE_DIR_ENTER/RECURSE_DIR_LEAVE once for top-level dir, too, with dir_fd=-1 and NULL dirent */
} RecurseDirFlags;

प्रकारनिर्देश संरचना DirectoryEntries {
        size_t n_entries;
        संरचना dirent **entries;
        size_t बफर_आकार;
        संरचना dirent बफर[];
} DirectoryEntries;

पूर्णाङ्क readdir_all(पूर्णाङ्क dir_fd, RecurseDirFlags flags, DirectoryEntries **ret);
पूर्णाङ्क readdir_all_at(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, RecurseDirFlags flags, DirectoryEntries **ret);

प्रकारनिर्देश गणना RecurseDirEvent {
        RECURSE_DIR_ENTER,      /* only for dir inodes */
        RECURSE_DIR_LEAVE,      /* only for dir inodes */
        RECURSE_DIR_प्रविष्टिः,      /* only for non-dir inodes */
        RECURSE_DIR_SKIP_MOUNT, /* only for dir inodes: when we don't descent into submounts */
        RECURSE_DIR_SKIP_गभीरता, /* only for dir inodes: when we reached the max depth */

        /* If we hit an error opening/stating an entry, then we'll fire a
         * 'RECURSE_DIR_SKIP_{OPEN_DIR|OPEN_INODE|STAT_INODE}_ERROR_BASE + errno' event. In this case 'de'
         * will be valid, but the statx data NULL and the inode fd -1. */
        RECURSE_DIR_SKIP_OPEN_DIR_ERROR_आधारः,
        RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX = RECURSE_DIR_SKIP_OPEN_DIR_ERROR_आधारः + ERRNO_MAX,

        RECURSE_DIR_SKIP_OPEN_INODE_ERROR_आधारः,
        RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX = RECURSE_DIR_SKIP_OPEN_INODE_ERROR_आधारः + ERRNO_MAX,

        RECURSE_DIR_SKIP_STAT_INODE_ERROR_आधारः,
        RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX = RECURSE_DIR_SKIP_STAT_INODE_ERROR_आधारः + ERRNO_MAX,

        _RECURSE_DIR_EVENT_MAX,
        _RECURSE_DIR_EVENT_INVALID = -EINVAL,
} RecurseDirEvent;

#परिभाषय RECURSE_DIR_CONTINUE 0
#परिभाषय RECURSE_DIR_LEAVE_DIRECTORY INT_MIN
#परिभाषय RECURSE_DIR_SKIP_प्रविष्टिः (INT_MIN+1)
#परिभाषय RECURSE_DIR_UNLINK (INT_MIN+2)
#परिभाषय RECURSE_DIR_UNLINK_GRACEFUL (INT_MIN+3)

/* Make sure that the negative errno range and these two special returns don't overlap */
assert_cc(RECURSE_DIR_LEAVE_DIRECTORY < -ERRNO_MAX);
assert_cc(RECURSE_DIR_SKIP_प्रविष्टिः < -ERRNO_MAX);

/* Prototype for the callback function that is called whenever we enter or leave a dir inode, or find another dir entry. Return values are:
 *
 * RECURSE_DIR_CONTINUE (i.e. 0) → continue with next entry
 * RECURSE_DIR_LEAVE_DIRECTORY   → leave current directory immediately, don't process further siblings
 * RECURSE_DIR_SKIP_ENTRY        → skip this entry otherwise (only makes sense on RECURSE_DIR_ENTER)
 * others                        → terminate iteration entirely, return the specified value (idea is that
 *                                 < 0 indicates errors and > 0 indicates various forms of success)
 */
प्रकारनिर्देश पूर्णाङ्क (*recurse_dir_func_t)(
                RecurseDirEvent event,
                ध्रुव वर्ण *मार्गः,        /* Full non-normalized path, i.e. the path specified during recurise_dir() with what we found appended */
                पूर्णाङ्क dir_fd,              /* fd of the current dir */
                पूर्णाङ्क inode_fd,            /* fd of the current entry in the current dir (O_DIRECTORY if directory, and O_PATH otherwise, but only if RECURSE_DIR_INODE_FD was set) */
                ध्रुव संरचना dirent *de, /* directory entry (always valid) */
                ध्रुव संरचना statx *sx,  /* statx data (only if statx_mask was non-zero) */
                शून्य *userdata);

पूर्णाङ्क recurse_dir(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, अचिह्नित statx_आवरणम्, अचिह्नित n_गभीरता_max, RecurseDirFlags flags, recurse_dir_func_t func, शून्य *userdata);
पूर्णाङ्क recurse_dir_at(पूर्णाङ्क atfd, ध्रुव वर्ण *मार्गः, अचिह्नित statx_आवरणम्, अचिह्नित n_गभीरता_max, RecurseDirFlags flags, recurse_dir_func_t func, शून्य *userdata);
