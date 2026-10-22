/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        BTRFS_SUBVOL_FLAGS_आवरणम्      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

पूर्णाङ्क btrfs_validate_subvolume_name(ध्रुव वर्ण *name);

पूर्णाङ्क btrfs_subvol_make(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः);

पूर्णाङ्क btrfs_subvol_make_fallback(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, mode_t बहुलकम्);
