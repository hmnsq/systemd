/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ BtrfsSubvolFlags {
        BTRFS_SUBVOL_RO               = 1 << 0,
        BTRFS_SUBVOL_NODATACOW        = 1 << 1,
        _BTRFS_SUBVOL_FLAGS_MASK      = BTRFS_SUBVOL_NODATACOW|BTRFS_SUBVOL_RO,
        _BTRFS_SUBVOL_FLAGS_INVALID   = -EINVAL,
        _BTRFS_SUBVOL_FLAGS_ERRNO_MAX = -ERRNO_MAX, /* Ensure the whole errno range fits into this enum */
} BtrfsSubvolFlags;

പൂർണ്ണസംഖ്യ btrfs_validate_subvolume_name(സ്ഥിരം അക്ഷരം *name);

പൂർണ്ണസംഖ്യ btrfs_subvol_make(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ btrfs_subvol_make_fallback(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode);
