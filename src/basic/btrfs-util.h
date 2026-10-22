/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

පූර්ණ_සංඛ්‍යා btrfs_validate_subvolume_name(const var_na *name);

පූර්ණ_සංඛ්‍යා btrfs_subvol_make(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path);

පූර්ණ_සංඛ්‍යා btrfs_subvol_make_fallback(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode);
