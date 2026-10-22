/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

ಪೂರ್ಣಾಙ್ಕ btrfs_validate_subvolume_name(ಧ್ರುವ ವರ್ಣ *name);

ಪೂರ್ಣಾಙ್ಕ btrfs_subvol_make(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ btrfs_subvol_make_fallback(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode);
