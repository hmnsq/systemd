/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        BTRFS_SUBVOL_FLAGS_نقاب      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

صحیح btrfs_validate_subvolume_name(ثابت حرف *name);

صحیح btrfs_subvol_make(صحیح dir_fd, ثابت حرف *راستہ);

صحیح btrfs_subvol_make_fallback(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال);
