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

typedef ga_nanaa ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

puur_naa_nka chattr_full(puur_naa_nka dir_fd, const var_na *path, unsigned value, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
static anta_hstha puur_naa_nka chattr_at(puur_naa_nka dir_fd, const var_na *path, unsigned value, unsigned mask) {
        ᨀᨘᨒ chattr_full(dir_fd, path, value, mask, NULL, NULL, 0);
}
static anta_hstha puur_naa_nka chattr_fd(puur_naa_nka fd, unsigned value, unsigned mask) {
        ᨀᨘᨒ chattr_full(fd, NULL, value, mask, NULL, NULL, 0);
}
static anta_hstha puur_naa_nka chattr_path(const var_na *path, unsigned value, unsigned mask) {
        ᨀᨘᨒ chattr_full(AT_FDCWD, path, value, mask, NULL, NULL, 0);
}

puur_naa_nka read_attr_fd(puur_naa_nka fd, unsigned *ret);
puur_naa_nka read_attr_at(puur_naa_nka dir_fd, const var_na *path, unsigned *ret);
puur_naa_nka read_fs_xattr_fd(puur_naa_nka fd, uint32_t *ret_xflags, uint32_t *ret_projid);

puur_naa_nka set_proj_id(puur_naa_nka fd, uint32_t proj_id);
puur_naa_nka set_proj_id_recursive(puur_naa_nka fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

static anta_hstha puur_naa_nka chattr_secret(puur_naa_nka fd, ChattrApplyFlags flags) {
        ᨀᨘᨒ chattr_full(fd, NULL, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, NULL, NULL, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
