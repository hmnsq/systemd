/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <linux/fs.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

ಪೂರ್ಣಾಙ್ಕ chattr_full(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, unsigned ಮೌಲ್ಯ, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ chattr_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, unsigned ಮೌಲ್ಯ, unsigned mask) {
        ಪ್ರತ್ಯಾಗಮ chattr_full(dir_fd, path, ಮೌಲ್ಯ, mask, ರಿಕ್ತ, ರಿಕ್ತ, 0);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ chattr_fd(ಪೂರ್ಣಾಙ್ಕ fd, unsigned ಮೌಲ್ಯ, unsigned mask) {
        ಪ್ರತ್ಯಾಗಮ chattr_full(fd, ರಿಕ್ತ, ಮೌಲ್ಯ, mask, ರಿಕ್ತ, ರಿಕ್ತ, 0);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ chattr_path(ಧ್ರುವ ವರ್ಣ *path, unsigned ಮೌಲ್ಯ, unsigned mask) {
        ಪ್ರತ್ಯಾಗಮ chattr_full(AT_FDCWD, path, ಮೌಲ್ಯ, mask, ರಿಕ್ತ, ರಿಕ್ತ, 0);
}

ಪೂರ್ಣಾಙ್ಕ read_attr_fd(ಪೂರ್ಣಾಙ್ಕ fd, unsigned *ret);
ಪೂರ್ಣಾಙ್ಕ read_attr_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, unsigned *ret);
ಪೂರ್ಣಾಙ್ಕ read_fs_xattr_fd(ಪೂರ್ಣಾಙ್ಕ fd, uint32_t *ret_xflags, uint32_t *ret_projid);

ಪೂರ್ಣಾಙ್ಕ set_proj_id(ಪೂರ್ಣಾಙ್ಕ fd, uint32_t proj_id);
ಪೂರ್ಣಾಙ್ಕ set_proj_id_recursive(ಪೂರ್ಣಾಙ್ಕ fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ chattr_secret(ಪೂರ್ಣಾಙ್ಕ fd, ChattrApplyFlags flags) {
        ಪ್ರತ್ಯಾಗಮ chattr_full(fd, ರಿಕ್ತ, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ರಿಕ್ತ, ರಿಕ್ತ, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
