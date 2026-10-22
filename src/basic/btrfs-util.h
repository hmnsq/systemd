/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

પૂર્ણાંક btrfs_validate_subvolume_name(અચલ અક્ષર *name);

પૂર્ણાંક btrfs_subvol_make(પૂર્ણાંક dir_fd, અચલ અક્ષર *path);

પૂર્ણાંક btrfs_subvol_make_fallback(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode);
