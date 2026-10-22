/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/fs.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHATTR_ALL_FL                           \
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

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

ਪੂਰਨ chattr_ਭਰਿਆ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ_3, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪਿਛਲਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_final, ChattrApplyFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ chattr_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ_3) {
        ਵਾਪਸ chattr_ਭਰਿਆ(dir_fd, ਰਸਤਾ, ਮੁੱਲ, ਨਕਾਬ_3, ਨਿਰਾ, ਨਿਰਾ, 0);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ chattr_fd(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ_3) {
        ਵਾਪਸ chattr_ਭਰਿਆ(fd, ਨਿਰਾ, ਮੁੱਲ, ਨਕਾਬ_3, ਨਿਰਾ, ਨਿਰਾ, 0);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ chattr_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ_3) {
        ਵਾਪਸ chattr_ਭਰਿਆ(AT_FDCWD, ਰਸਤਾ, ਮੁੱਲ, ਨਕਾਬ_3, ਨਿਰਾ, ਨਿਰਾ, 0);
}

ਪੂਰਨ read_attr_fd(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret);
ਪੂਰਨ read_attr_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret);
ਪੂਰਨ read_fs_xattr_fd(ਪੂਰਨ fd, uint32_t *ret_xflags, uint32_t *ret_projid);

ਪੂਰਨ ਸੈੱਟ_proj_id(ਪੂਰਨ fd, uint32_t proj_id);
ਪੂਰਨ ਸੈੱਟ_proj_id_recursive(ਪੂਰਨ fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ chattr_secret(ਪੂਰਨ fd, ChattrApplyFlags flags) {
        ਵਾਪਸ chattr_ਭਰਿਆ(fd, ਨਿਰਾ, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ਨਿਰਾ, ਨਿਰਾ, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t ਬਹੁਲਕ);
