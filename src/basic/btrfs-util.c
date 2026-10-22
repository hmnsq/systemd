/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <linux/btrfs.h>
#অন্তর্ভাৱ <sys/ioctl.h>
#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "btrfs-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক btrfs_validate_subvolume_name(ধ্রুৱ ৱর্ণ *name) {

        যদি (!filename_is_valid(name))
                ঘূৰাই -EINVAL;

        যদি (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ঘূৰাই -E2BIG;

        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক extract_subvolume_name(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret) {
        _cleanup_free_ ৱর্ণ *fn = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ঘূৰাই r;

        r = btrfs_validate_subvolume_name(fn);
        যদি (r < 0)
                ঘূৰাই r;

        *ret = TAKE_PTR(fn);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক btrfs_subvol_make(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path) {
        সংরচনা btrfs_ioctl_vol_args args = {};
        _cleanup_free_ ৱর্ণ *subvolume = রিক্ত, *parent = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        যদি (r < 0)
                ঘূৰাই r;

        r = path_extract_directory(path, &parent);
        যদি (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ঘূৰাই r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        যদি (fd < 0)
                ঘূৰাই fd;

        strncpy(args.name, subvolume, পরিমাণ(args.name)-1);

        ঘূৰাই RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

পূর্ণাঙ্ক btrfs_subvol_make_fallback(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode) {
        mode_t old, combined;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        যদি (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        যদি (r >= 0)
                ঘূৰাই 1; /* subvol worked */
        যদি (!ERRNO_IS_NOT_SUPPORTED(r))
                ঘূৰাই r;

        যদি (mkdirat(dir_fd, path, mode) < 0)
                ঘূৰাই -errno;

        ঘূৰাই 0; /* plain directory */
}
