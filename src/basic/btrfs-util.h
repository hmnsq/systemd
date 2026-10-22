/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

முழு btrfs_validate_subvolume_name(மாறிலி எழுத்து *name);

முழு btrfs_subvol_make(முழு dir_fd, மாறிலி எழுத்து *path);

முழு btrfs_subvol_make_fallback(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode);
