/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن <linux/fs.h>

#ضمن "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#تعریف_کړه CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#تعریف_کړه CHATTR_ALL_FL                           \
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

تعريفنوع تعداد ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

صحيح chattr_ډک(صحيح dir_fd, ثابت حرف *لار, بینښه ارزښت, بینښه ماسک_3, بینښه *ret_پخوانی, بینښه *ret_final, ChattrApplyFlags flags);
ساكن مضمن صحيح chattr_at(صحيح dir_fd, ثابت حرف *لار, بینښه ارزښت, بینښه ماسک_3) {
        أرجع chattr_ډک(dir_fd, لار, ارزښت, ماسک_3, عدم, عدم, 0);
}
ساكن مضمن صحيح chattr_fd(صحيح fd, بینښه ارزښت, بینښه ماسک_3) {
        أرجع chattr_ډک(fd, عدم, ارزښت, ماسک_3, عدم, عدم, 0);
}
ساكن مضمن صحيح chattr_لار(ثابت حرف *لار, بینښه ارزښت, بینښه ماسک_3) {
        أرجع chattr_ډک(AT_FDCWD, لار, ارزښت, ماسک_3, عدم, عدم, 0);
}

صحيح read_attr_fd(صحيح fd, بینښه *ret);
صحيح read_attr_at(صحيح dir_fd, ثابت حرف *لار, بینښه *ret);
صحيح read_fs_xattr_fd(صحيح fd, uint32_t *ret_xflags, uint32_t *ret_projid);

صحيح سېټ_proj_id(صحيح fd, uint32_t proj_id);
صحيح سېټ_proj_id_recursive(صحيح fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#تعریف_کړه CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ساكن مضمن صحيح chattr_secret(صحيح fd, ChattrApplyFlags flags) {
        أرجع chattr_ډک(fd, عدم, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, عدم, عدم, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t ډېرپېښ);
