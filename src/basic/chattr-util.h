/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <linux/fs.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#ವ್ಯಾಖ್ಯಾನಿಸು CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#ವ್ಯಾಖ್ಯಾನಿಸು CHATTR_ALL_FL                           \
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

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

ಪೂರ್ಣಾಂಕ chattr_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ_3, ಚಿಹ್ನೆರಹಿತ *ret_ಹಿಂದಿನ, ಚಿಹ್ನೆರಹಿತ *ret_final, ChattrApplyFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ chattr_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ_3) {
        ಹಿಂತಿರುಗಿ chattr_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ, ಮೌಲ್ಯ, ಮುಖವಾಡ_3, ಖಾಲಿ, ಖಾಲಿ, 0);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ chattr_fd(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ_3) {
        ಹಿಂತಿರುಗಿ chattr_ತುಂಬಿದ(fd, ಖಾಲಿ, ಮೌಲ್ಯ, ಮುಖವಾಡ_3, ಖಾಲಿ, ಖಾಲಿ, 0);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ chattr_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ, ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ_3) {
        ಹಿಂತಿರುಗಿ chattr_ತುಂಬಿದ(AT_FDCWD, ಮಾರ್ಗ, ಮೌಲ್ಯ, ಮುಖವಾಡ_3, ಖಾಲಿ, ಖಾಲಿ, 0);
}

ಪೂರ್ಣಾಂಕ read_attr_fd(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ *ret);
ಪೂರ್ಣಾಂಕ read_attr_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ *ret);
ಪೂರ್ಣಾಂಕ read_fs_xattr_fd(ಪೂರ್ಣಾಂಕ fd, uint32_t *ret_xflags, uint32_t *ret_projid);

ಪೂರ್ಣಾಂಕ ಗಣ_proj_id(ಪೂರ್ಣಾಂಕ fd, uint32_t proj_id);
ಪೂರ್ಣಾಂಕ ಗಣ_proj_id_recursive(ಪೂರ್ಣಾಂಕ fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#ವ್ಯಾಖ್ಯಾನಿಸು CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ chattr_secret(ಪೂರ್ಣಾಂಕ fd, ChattrApplyFlags flags) {
        ಹಿಂತಿರುಗಿ chattr_ತುಂಬಿದ(fd, ಖಾಲಿ, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ಖಾಲಿ, ಖಾಲಿ, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t ಬಹುಲಕ);
