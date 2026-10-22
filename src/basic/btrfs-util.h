/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

পূর্ণসংখ্যা btrfs_validate_subvolume_name(ধ্রুবক অক্ষর *name);

পূর্ণসংখ্যা btrfs_subvol_make(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা btrfs_subvol_make_fallback(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode);
