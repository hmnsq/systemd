/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع تعداد BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        BTRFS_SUBVOL_FLAGS_ماسک      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

صحيح btrfs_validate_subvolume_name(ثابت حرف *name);

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *مسیر);

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *مسیر, mode_t نما);
