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

ಪೂರ್ಣಾಙ್ಕ btrfs_validate_subvolume_name(ಧ್ರುವ ವರ್ಣ *name) {

        ಯದಿ (!filename_is_valid(name))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ಪ್ರತ್ಯಾಗಮ -E2BIG;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ extract_subvolume_name(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *fn = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = path_extract_filename(path, &fn);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = btrfs_validate_subvolume_name(fn);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(fn);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ btrfs_subvol_make(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path) {
        ಸಂರಚನಾ btrfs_ioctl_vol_args args = {};
        _cleanup_free_ ವರ್ಣ *subvolume = ರಿಕ್ತ, *parent = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = path_extract_directory(path, &parent);
        ಯದಿ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ಪ್ರತ್ಯಾಗಮ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        strncpy(args.name, subvolume, ಪರಿಮಾಣ(args.name)-1);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

ಪೂರ್ಣಾಙ್ಕ btrfs_subvol_make_fallback(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode) {
        mode_t old, combined;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ಯದಿ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ 1; /* subvol worked */
        ಯದಿ (!ERRNO_IS_NOT_SUPPORTED(r))
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (mkdirat(dir_fd, path, mode) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0; /* plain directory */
}
