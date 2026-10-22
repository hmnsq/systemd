/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <linux/fs.h>

#شامل "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#تعریف_کرو CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#تعریف_کرو CHATTR_ALL_FL                           \
        (FS_NOATIME_FL      |                   \
         FS_SYNC_FL         |                   \
         FS_DIRSYNC_FL      |                   \
         FS_APPEND_FL       |                   \
         FS_COMPR_FL        |                   \
         FS_NODUMP_FL       |                   \
         FS_EXTENT_FL       |                   \
         FS_IMMUTABLE_FL    |                   \
         FS_JOURNAL_DATA_FL |                   \
         FS_SECRM_FL        |                   \
         FS_UNRM_FL         |                   \
         FS_NOTAIL_FL       |                   \
         FS_TOPDIR_FL       |                   \
         FS_NOCOW_FL        |                   \
         FS_PROJINHERIT_FL)

قسمتعریف شمار ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

صحیح chattr_بھرا(صحیح dir_fd, ثابت حرف *راستہ, غیرعلامتی قدر, غیرعلامتی نقاب_3, غیرعلامتی *ret_پچھلا, غیرعلامتی *ret_final, ChattrApplyFlags flags);
ساکن اندرونی صحیح chattr_at(صحیح dir_fd, ثابت حرف *راستہ, غیرعلامتی قدر, غیرعلامتی نقاب_3) {
        واپس chattr_بھرا(dir_fd, راستہ, قدر, نقاب_3, تہی, تہی, 0);
}
ساکن اندرونی صحیح chattr_fd(صحیح fd, غیرعلامتی قدر, غیرعلامتی نقاب_3) {
        واپس chattr_بھرا(fd, تہی, قدر, نقاب_3, تہی, تہی, 0);
}
ساکن اندرونی صحیح chattr_راستہ(ثابت حرف *راستہ, غیرعلامتی قدر, غیرعلامتی نقاب_3) {
        واپس chattr_بھرا(AT_FDCWD, راستہ, قدر, نقاب_3, تہی, تہی, 0);
}

صحیح read_attr_fd(صحیح fd, غیرعلامتی *ret);
صحیح read_attr_at(صحیح dir_fd, ثابت حرف *راستہ, غیرعلامتی *ret);
صحیح read_fs_xattr_fd(صحیح fd, uint32_t *ret_xflags, uint32_t *ret_projid);

صحیح سیٹ_proj_id(صحیح fd, uint32_t proj_id);
صحیح سیٹ_proj_id_recursive(صحیح fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#تعریف_کرو CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ساکن اندرونی صحیح chattr_secret(صحیح fd, ChattrApplyFlags flags) {
        واپس chattr_بھرا(fd, تہی, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, تہی, تہی, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t منوال);
