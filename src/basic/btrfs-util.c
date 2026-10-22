/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <linux/btrfs.h>
#شامل <sys/ioctl.h>
#شامل <sys/stat.h>

#شامل "alloc-util.h"
#شامل "btrfs-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "path-util.h"
#شامل "string-util.h"

صحیح btrfs_validate_subvolume_name(ثابت نویسه *name) {

        اگر (!filename_is_valid(name))
                بازگشت -EINVAL;

        اگر (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                بازگشت -E2BIG;

        بازگشت 0;
}

ایستا صحیح extract_subvolume_name(ثابت نویسه *path, نویسه **ret) {
        _cleanup_free_ نویسه *fn = پوچ;
        صحیح r;

        أكد(path);
        أكد(ret);

        r = path_extract_filename(path, &fn);
        اگر (r < 0)
                بازگشت r;

        r = btrfs_validate_subvolume_name(fn);
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(fn);
        بازگشت 0;
}

صحیح btrfs_subvol_make(صحیح dir_fd, ثابت نویسه *path) {
        ساختار btrfs_ioctl_vol_args args = {};
        _cleanup_free_ نویسه *subvolume = پوچ, *parent = پوچ;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        اگر (r < 0)
                بازگشت r;

        r = path_extract_directory(path, &parent);
        اگر (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                بازگشت r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت fd;

        strncpy(args.name, subvolume, اندازه(args.name)-1);

        بازگشت RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

صحیح btrfs_subvol_make_fallback(صحیح dir_fd, ثابت نویسه *path, mode_t mode) {
        mode_t old, combined;
        صحیح r;

        أكد(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        اگر (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        اگر (r >= 0)
                بازگشت 1; /* subvol worked */
        اگر (!ERRNO_IS_NOT_SUPPORTED(r))
                بازگشت r;

        اگر (mkdirat(dir_fd, path, mode) < 0)
                بازگشت -errno;

        بازگشت 0; /* plain directory */
}
