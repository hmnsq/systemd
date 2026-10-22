/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <linux/btrfs.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/stat.h>

#ضمن "alloc-util.h"
#ضمن "btrfs-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"

صحيح btrfs_validate_subvolume_name(ثابت حرف *name) {

        ܐܢ (!filename_is_valid(name))
                ܦܢܐ -EINVAL;

        ܐܢ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ܦܢܐ -E2BIG;

        ܦܢܐ 0;
}

ساكن صحيح extract_subvolume_name(ثابت حرف *path, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(path);
        أكد(ret);

        r = path_extract_filename(path, &fn);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = btrfs_validate_subvolume_name(fn);
        ܐܢ (r < 0)
                ܦܢܐ r;

        *ret = TAKE_PTR(fn);
        ܦܢܐ 0;
}

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *path) {
        بنية btrfs_ioctl_vol_args args = {};
        _cleanup_free_ حرف *subvolume = عدم, *parent = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = path_extract_directory(path, &parent);
        ܐܢ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ܦܢܐ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        strncpy(args.name, subvolume, حجم(args.name)-1);

        ܦܢܐ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *path, mode_t mode) {
        mode_t old, combined;
        صحيح r;

        أكد(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ܐܢ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ܐܢ (r >= 0)
                ܦܢܐ 1; /* subvol worked */
        ܐܢ (!ERRNO_IS_NOT_SUPPORTED(r))
                ܦܢܐ r;

        ܐܢ (mkdirat(dir_fd, path, mode) < 0)
                ܦܢܐ -errno;

        ܦܢܐ 0; /* plain directory */
}
