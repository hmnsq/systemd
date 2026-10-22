/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <linux/fs.h>

#അന്തര്ഭാവ "forward.h"

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

തരനിർവചനം എണ്ണൽ ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

പൂർണ്ണസംഖ്യ chattr_full(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, unsigned മൂല്യം, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ chattr_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, unsigned മൂല്യം, unsigned mask) {
        തിരികെ chattr_full(dir_fd, path, മൂല്യം, mask, ശൂന്യ, ശൂന്യ, 0);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ chattr_fd(പൂർണ്ണസംഖ്യ fd, unsigned മൂല്യം, unsigned mask) {
        തിരികെ chattr_full(fd, ശൂന്യ, മൂല്യം, mask, ശൂന്യ, ശൂന്യ, 0);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ chattr_path(സ്ഥിരം അക്ഷരം *path, unsigned മൂല്യം, unsigned mask) {
        തിരികെ chattr_full(AT_FDCWD, path, മൂല്യം, mask, ശൂന്യ, ശൂന്യ, 0);
}

പൂർണ്ണസംഖ്യ read_attr_fd(പൂർണ്ണസംഖ്യ fd, unsigned *ret);
പൂർണ്ണസംഖ്യ read_attr_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, unsigned *ret);
പൂർണ്ണസംഖ്യ read_fs_xattr_fd(പൂർണ്ണസംഖ്യ fd, uint32_t *ret_xflags, uint32_t *ret_projid);

പൂർണ്ണസംഖ്യ set_proj_id(പൂർണ്ണസംഖ്യ fd, uint32_t proj_id);
പൂർണ്ണസംഖ്യ set_proj_id_recursive(പൂർണ്ണസംഖ്യ fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ chattr_secret(പൂർണ്ണസംഖ്യ fd, ChattrApplyFlags flags) {
        തിരികെ chattr_full(fd, ശൂന്യ, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ശൂന്യ, ശൂന്യ, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
