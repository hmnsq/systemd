/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <linux/btrfs.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "btrfs-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ btrfs_validate_subvolume_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {

        ಆದರೆ (!filename_is_valid(name))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(name) > BTRFS_SUBVOL_NAME_MAX)
                ಹಿಂತಿರುಗಿ -E2BIG;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಹೊರತೆಗೆ_subvolume_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *fn = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, &fn);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = btrfs_validate_subvolume_name(fn);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(fn);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ btrfs_subvol_make(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        ರಚನೆ btrfs_ioctl_vol_args ವಾದಗಳು = {};
        _cleanup_free_ ಅಕ್ಷರ *subvolume = ಖಾಲಿ, *ಪೋಷಕ = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(!isempty(ಮಾರ್ಗ));

        r = ಹೊರತೆಗೆ_subvolume_name(ಮಾರ್ಗ, &subvolume);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(ಮಾರ್ಗ, &ಪೋಷಕ);
        ಆದರೆ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ಹಿಂತಿರುಗಿ r;

        fd = xopenat(dir_fd, ಪೋಷಕ ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        strncpy(ವಾದಗಳು.name, subvolume, ಗಾತ್ರ(ವಾದಗಳು.name)-1);

        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &ವಾದಗಳು));
}

ಪೂರ್ಣಾಂಕ btrfs_subvol_make_fallback(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ) {
        mode_t old, combined;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~ಬಹುಲಕ);
        combined = old | ~ಬಹುಲಕ;
        ಆದರೆ (combined != ~ಬಹುಲಕ)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, ಮಾರ್ಗ);
        umask(old);

        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ 1; /* subvol worked */
        ಆದರೆ (!ERRNO_IS_NOT_SUPPORTED(r))
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (mkdirat(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0; /* plain directory */
}
