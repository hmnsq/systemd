/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <linux/fs.h>

#include "forward.h"

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

typedef enum ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

정수 chattr_full(정수 dir_fd, const char *path, unsigned value, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
static inline 정수 chattr_at(정수 dir_fd, const char *path, unsigned value, unsigned mask) {
        반환 chattr_full(dir_fd, path, value, mask, NULL, NULL, 0);
}
static inline 정수 chattr_fd(정수 fd, unsigned value, unsigned mask) {
        반환 chattr_full(fd, NULL, value, mask, NULL, NULL, 0);
}
static inline 정수 chattr_path(const char *path, unsigned value, unsigned mask) {
        반환 chattr_full(AT_FDCWD, path, value, mask, NULL, NULL, 0);
}

정수 read_attr_fd(정수 fd, unsigned *ret);
정수 read_attr_at(정수 dir_fd, const char *path, unsigned *ret);
정수 read_fs_xattr_fd(정수 fd, uint32_t *ret_xflags, uint32_t *ret_projid);

정수 set_proj_id(정수 fd, uint32_t proj_id);
정수 set_proj_id_recursive(정수 fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

static inline 정수 chattr_secret(정수 fd, ChattrApplyFlags flags) {
        반환 chattr_full(fd, NULL, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, NULL, NULL, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
