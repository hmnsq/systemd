/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <linux/fs.h>

#अन्तर्भाव "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#परिभाषय CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#परिभाषय CHATTR_ALL_FL                           \
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

प्रकारनिर्देश गणना ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

पूर्णाङ्क chattr_पूर्णम्(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, अचिह्नित मानम्, अचिह्नित आवरणम्_3, अचिह्नित *ret_previous, अचिह्नित *ret_final, ChattrApplyFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क chattr_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, अचिह्नित मानम्, अचिह्नित आवरणम्_3) {
        प्रत्यागम chattr_पूर्णम्(dir_fd, मार्गः, मानम्, आवरणम्_3, रिक्त, रिक्त, 0);
}
स्थिर अन्तःस्थ पूर्णाङ्क chattr_fd(पूर्णाङ्क fd, अचिह्नित मानम्, अचिह्नित आवरणम्_3) {
        प्रत्यागम chattr_पूर्णम्(fd, रिक्त, मानम्, आवरणम्_3, रिक्त, रिक्त, 0);
}
स्थिर अन्तःस्थ पूर्णाङ्क chattr_मार्गः(ध्रुव वर्ण *मार्गः, अचिह्नित मानम्, अचिह्नित आवरणम्_3) {
        प्रत्यागम chattr_पूर्णम्(AT_FDCWD, मार्गः, मानम्, आवरणम्_3, रिक्त, रिक्त, 0);
}

पूर्णाङ्क read_attr_fd(पूर्णाङ्क fd, अचिह्नित *ret);
पूर्णाङ्क read_attr_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, अचिह्नित *ret);
पूर्णाङ्क read_fs_xattr_fd(पूर्णाङ्क fd, uint32_t *ret_xflags, uint32_t *ret_projid);

पूर्णाङ्क समुच्चयः_proj_id(पूर्णाङ्क fd, uint32_t proj_id);
पूर्णाङ्क समुच्चयः_proj_id_recursive(पूर्णाङ्क fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#परिभाषय CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

स्थिर अन्तःस्थ पूर्णाङ्क chattr_secret(पूर्णाङ्क fd, ChattrApplyFlags flags) {
        प्रत्यागम chattr_पूर्णम्(fd, रिक्त, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, रिक्त, रिक्त, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t बहुलकम्);
