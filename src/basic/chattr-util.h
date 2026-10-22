/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <linux/fs.h>

#अन्तर्भाव "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#define CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#define CHATTR_ALL_FL                           \
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

प्रकारव्याख्या गणना ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

पूर्णांक chattr_full(पूर्णांक dir_fd, स्थिरांक अक्षर *path, अचिन्हित मूल्य, अचिन्हित mask, अचिन्हित *ret_previous, अचिन्हित *ret_final, ChattrApplyFlags flags);
स्थिर ओळीत पूर्णांक chattr_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, अचिन्हित मूल्य, अचिन्हित mask) {
        परत chattr_full(dir_fd, path, मूल्य, mask, रिक्त, रिक्त, 0);
}
स्थिर ओळीत पूर्णांक chattr_fd(पूर्णांक fd, अचिन्हित मूल्य, अचिन्हित mask) {
        परत chattr_full(fd, रिक्त, मूल्य, mask, रिक्त, रिक्त, 0);
}
स्थिर ओळीत पूर्णांक chattr_path(स्थिरांक अक्षर *path, अचिन्हित मूल्य, अचिन्हित mask) {
        परत chattr_full(AT_FDCWD, path, मूल्य, mask, रिक्त, रिक्त, 0);
}

पूर्णांक read_attr_fd(पूर्णांक fd, अचिन्हित *ret);
पूर्णांक read_attr_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, अचिन्हित *ret);
पूर्णांक read_fs_xattr_fd(पूर्णांक fd, uint32_t *ret_xflags, uint32_t *ret_projid);

पूर्णांक set_proj_id(पूर्णांक fd, uint32_t proj_id);
पूर्णांक set_proj_id_recursive(पूर्णांक fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

स्थिर ओळीत पूर्णांक chattr_secret(पूर्णांक fd, ChattrApplyFlags flags) {
        परत chattr_full(fd, रिक्त, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, रिक्त, रिक्त, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
