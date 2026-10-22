/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या गणना BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

पूर्णांक btrfs_validate_subvolume_name(स्थिरांक अक्षर *name);

पूर्णांक btrfs_subvol_make(पूर्णांक dir_fd, स्थिरांक अक्षर *path);

पूर्णांक btrfs_subvol_make_fallback(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode);
