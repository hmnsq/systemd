/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        BTRFS_SUBVOL_FLAGS_ముసుగు      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

పూర్ణాంకం btrfs_validate_subvolume_name(స్థిరరాశి అక్షరం *name);

పూర్ణాంకం btrfs_subvol_make(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం btrfs_subvol_make_fallback(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం);
