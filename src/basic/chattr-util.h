/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <linux/fs.h>

#அந்தர்பாவ "forward.h"

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

வகைவரை எண்ணிக்கை ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

முழு chattr_full(முழு dir_fd, மாறிலி எழுத்து *path, unsigned மதிப்பு, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
நிலையான உள்வரி முழு chattr_at(முழு dir_fd, மாறிலி எழுத்து *path, unsigned மதிப்பு, unsigned mask) {
        திரும்பு chattr_full(dir_fd, path, மதிப்பு, mask, வெற்று, வெற்று, 0);
}
நிலையான உள்வரி முழு chattr_fd(முழு fd, unsigned மதிப்பு, unsigned mask) {
        திரும்பு chattr_full(fd, வெற்று, மதிப்பு, mask, வெற்று, வெற்று, 0);
}
நிலையான உள்வரி முழு chattr_path(மாறிலி எழுத்து *path, unsigned மதிப்பு, unsigned mask) {
        திரும்பு chattr_full(AT_FDCWD, path, மதிப்பு, mask, வெற்று, வெற்று, 0);
}

முழு read_attr_fd(முழு fd, unsigned *ret);
முழு read_attr_at(முழு dir_fd, மாறிலி எழுத்து *path, unsigned *ret);
முழு read_fs_xattr_fd(முழு fd, uint32_t *ret_xflags, uint32_t *ret_projid);

முழு set_proj_id(முழு fd, uint32_t proj_id);
முழு set_proj_id_recursive(முழு fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

நிலையான உள்வரி முழு chattr_secret(முழு fd, ChattrApplyFlags flags) {
        திரும்பு chattr_full(fd, வெற்று, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, வெற்று, வெற்று, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
