/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <linux/fs.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ChattrApplyFlags {
        CHATTR_FALLBACK_BITWISE       = 1 << 0,
        CHATTR_WARN_UNSUPPORTED_FLAGS = 1 << 1,
} ChattrApplyFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chattr_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, unsigned ମୂଲ୍ୟ, unsigned mask, unsigned *ret_previous, unsigned *ret_final, ChattrApplyFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା chattr_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, unsigned ମୂଲ୍ୟ, unsigned mask) {
        ଫେରନ୍ତୁ chattr_full(dir_fd, path, ମୂଲ୍ୟ, mask, ଖାଲି, ଖାଲି, 0);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା chattr_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned ମୂଲ୍ୟ, unsigned mask) {
        ଫେରନ୍ତୁ chattr_full(fd, ଖାଲି, ମୂଲ୍ୟ, mask, ଖାଲି, ଖାଲି, 0);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା chattr_path(ଅଚଳ ଅକ୍ଷର *path, unsigned ମୂଲ୍ୟ, unsigned mask) {
        ଫେରନ୍ତୁ chattr_full(AT_FDCWD, path, ମୂଲ୍ୟ, mask, ଖାଲି, ଖାଲି, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_attr_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_attr_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, unsigned *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_fs_xattr_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint32_t *ret_xflags, uint32_t *ret_projid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_proj_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint32_t proj_id);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_proj_id_recursive(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint32_t proj_id);

/* Combination of chattr flags, that should be appropriate for secrets stored on disk: Secure Remove +
 * Exclusion from Dumping + Synchronous Writing (i.e. not caching in memory) + In-Place Updating (i.e. not
 * spurious copies). */
#define CHATTR_SECRET_FLAGS (FS_SECRM_FL|FS_NODUMP_FL|FS_SYNC_FL|FS_NOCOW_FL)

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା chattr_secret(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ChattrApplyFlags flags) {
        ଫେରନ୍ତୁ chattr_full(fd, ଖାଲି, CHATTR_SECRET_FLAGS, CHATTR_SECRET_FLAGS, ଖାଲି, ଖାଲି, flags|CHATTR_FALLBACK_BITWISE);
}

bool inode_type_can_chattr(mode_t mode);
