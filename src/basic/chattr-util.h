/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <linux/fs.h>

#অন্তর্ভাব "forward.h"

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

প্রকারলেখ গণনা ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

পূর্ণসংখ্যা chattr_full(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, unsigned মান, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা chattr_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, unsigned মান, unsigned mask) {
        ফেরত chattr_full(dir_fd, path, মান, mask, ফাঁকা, ফাঁকা, 0);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা chattr_fd(পূর্ণসংখ্যা fd, unsigned মান, unsigned mask) {
        ফেরত chattr_full(fd, ফাঁকা, মান, mask, ফাঁকা, ফাঁকা, 0);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা chattr_path(ধ্রুবক অক্ষর *path, unsigned মান, unsigned mask) {
        ফেরত chattr_full(AT_FDCWD, path, মান, mask, ফাঁকা, ফাঁকা, 0);
}

পূর্ণসংখ্যা read_attr_fd(পূর্ণসংখ্যা fd, unsigned *ret);
পূর্ণসংখ্যা read_attr_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, unsigned *ret);
পূর্ণসংখ্যা read_fs_xattr_fd(পূর্ণসংখ্যা fd, uint32_t *ret_xflags, uint32_t *ret_projid);

পূর্ণসংখ্যা set_proj_id(পূর্ণসংখ্যা fd, uint32_t proj_id);
পূর্ণসংখ্যা set_proj_id_recursive(পূর্ণসংখ্যা fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

স্থির অন্তঃরেখ পূর্ণসংখ্যা chattr_secret(পূর্ণসংখ্যা fd, ChattrApplyFlags flags) {
        ফেরত chattr_full(fd, ফাঁকা, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ফাঁকা, ফাঁকা, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
