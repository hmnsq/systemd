/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <linux/fs.h>

#અન્તર્ભાવ "forward.h"

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

પ્રકારવ્યાખ્યા ગણના ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

પૂર્ણાંક chattr_full(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, unsigned કિંમત, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
સ્થિર અંતઃરેખ પૂર્ણાંક chattr_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, unsigned કિંમત, unsigned mask) {
        પાછા chattr_full(dir_fd, path, કિંમત, mask, ખાલી, ખાલી, 0);
}
સ્થિર અંતઃરેખ પૂર્ણાંક chattr_fd(પૂર્ણાંક fd, unsigned કિંમત, unsigned mask) {
        પાછા chattr_full(fd, ખાલી, કિંમત, mask, ખાલી, ખાલી, 0);
}
સ્થિર અંતઃરેખ પૂર્ણાંક chattr_path(અચલ અક્ષર *path, unsigned કિંમત, unsigned mask) {
        પાછા chattr_full(AT_FDCWD, path, કિંમત, mask, ખાલી, ખાલી, 0);
}

પૂર્ણાંક read_attr_fd(પૂર્ણાંક fd, unsigned *ret);
પૂર્ણાંક read_attr_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, unsigned *ret);
પૂર્ણાંક read_fs_xattr_fd(પૂર્ણાંક fd, uint32_t *ret_xflags, uint32_t *ret_projid);

પૂર્ણાંક set_proj_id(પૂર્ણાંક fd, uint32_t proj_id);
પૂર્ણાંક set_proj_id_recursive(પૂર્ણાંક fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

સ્થિર અંતઃરેખ પૂર્ણાંક chattr_secret(પૂર્ણાંક fd, ChattrApplyFlags flags) {
        પાછા chattr_full(fd, ખાલી, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ખાલી, ખાલી, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
