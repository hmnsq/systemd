/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <linux/fs.h>

#అన్తర్భావ "forward.h"

/* The chattr() flags to apply when creating a new file *before* writing to it. In particular, flags such as
 * FS_NOCOW_FL don't work if applied a-posteriori. All other flags are fine (or even necessary, think
 * FS_IMMUTABLE_FL!) to apply after writing to the files. */
#నిర్వచించు CHATTR_EARLY_FL                         \
        (FS_NOATIME_FL |                        \
         FS_COMPR_FL   |                        \
         FS_NOCOW_FL   |                        \
         FS_NOCOMP_FL  |                        \
         FS_PROJINHERIT_FL)

#నిర్వచించు CHATTR_ALL_FL                           \
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

రకనిర్వచనం గణన ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

పూర్ణాంకం chattr_నిండిన(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని విలువ, గుర్తులేని ముసుగు_3, గుర్తులేని *ret_మునుపటి, గుర్తులేని *ret_final, ChattrApplyFlags flags);
స్థిర అంతఃవరుస పూర్ణాంకం chattr_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని విలువ, గుర్తులేని ముసుగు_3) {
        తిరిగి chattr_నిండిన(dir_fd, మార్గం, విలువ, ముసుగు_3, ఖాళీ, ఖాళీ, 0);
}
స్థిర అంతఃవరుస పూర్ణాంకం chattr_fd(పూర్ణాంకం fd, గుర్తులేని విలువ, గుర్తులేని ముసుగు_3) {
        తిరిగి chattr_నిండిన(fd, ఖాళీ, విలువ, ముసుగు_3, ఖాళీ, ఖాళీ, 0);
}
స్థిర అంతఃవరుస పూర్ణాంకం chattr_మార్గం(స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని విలువ, గుర్తులేని ముసుగు_3) {
        తిరిగి chattr_నిండిన(AT_FDCWD, మార్గం, విలువ, ముసుగు_3, ఖాళీ, ఖాళీ, 0);
}

పూర్ణాంకం read_attr_fd(పూర్ణాంకం fd, గుర్తులేని *ret);
పూర్ణాంకం read_attr_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని *ret);
పూర్ణాంకం read_fs_xattr_fd(పూర్ణాంకం fd, uint32_t *ret_xflags, uint32_t *ret_projid);

పూర్ణాంకం సమితి_proj_id(పూర్ణాంకం fd, uint32_t proj_id);
పూర్ణాంకం సమితి_proj_id_recursive(పూర్ణాంకం fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#నిర్వచించు CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

స్థిర అంతఃవరుస పూర్ణాంకం chattr_secret(పూర్ణాంకం fd, ChattrApplyFlags flags) {
        తిరిగి chattr_నిండిన(fd, ఖాళీ, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ఖాళీ, ఖాళీ, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t బహుళకం);
