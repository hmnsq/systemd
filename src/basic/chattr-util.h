/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <linux/fs.h>

#شامل "forward.h"

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

تعریفنوع شمارش ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

صحیح chattr_full(صحیح dir_fd, ثابت نویسه *path, unsigned مقدار, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
ایستا درونخطی صحیح chattr_at(صحیح dir_fd, ثابت نویسه *path, unsigned مقدار, unsigned mask) {
        بازگشت chattr_full(dir_fd, path, مقدار, mask, پوچ, پوچ, 0);
}
ایستا درونخطی صحیح chattr_fd(صحیح fd, unsigned مقدار, unsigned mask) {
        بازگشت chattr_full(fd, پوچ, مقدار, mask, پوچ, پوچ, 0);
}
ایستا درونخطی صحیح chattr_path(ثابت نویسه *path, unsigned مقدار, unsigned mask) {
        بازگشت chattr_full(AT_FDCWD, path, مقدار, mask, پوچ, پوچ, 0);
}

صحیح read_attr_fd(صحیح fd, unsigned *ret);
صحیح read_attr_at(صحیح dir_fd, ثابت نویسه *path, unsigned *ret);
صحیح read_fs_xattr_fd(صحیح fd, uint32_t *ret_xflags, uint32_t *ret_projid);

صحیح set_proj_id(صحیح fd, uint32_t proj_id);
صحیح set_proj_id_recursive(صحیح fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ایستا درونخطی صحیح chattr_secret(صحیح fd, ChattrApplyFlags flags) {
        بازگشت chattr_full(fd, پوچ, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, پوچ, پوچ, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
