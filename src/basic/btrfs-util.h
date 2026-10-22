/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ গণনা BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

পূর্ণাঙ্ক btrfs_validate_subvolume_name(ধ্রুৱ ৱর্ণ *name);

পূর্ণাঙ্ক btrfs_subvol_make(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path);

পূর্ণাঙ্ক btrfs_subvol_make_fallback(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode);
