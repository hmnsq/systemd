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

        אם (!filename_is_valid(name))
                החזר -EINVAL;

        אם (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                החזר -E2BIG;

        החזר 0;
}

ساكن صحيح extract_subvolume_name(ثابت حرف *path, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(path);
        أكد(ret);

        r = path_extract_filename(path, &fn);
        אם (r < 0)
                החזר r;

        r = btrfs_validate_subvolume_name(fn);
        אם (r < 0)
                החזר r;

        *ret = TAKE_PTR(fn);
        החזר 0;
}

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *path) {
        بنية btrfs_ioctl_vol_args args = {};
        _cleanup_free_ حرف *subvolume = عدم, *parent = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        אם (r < 0)
                החזר r;

        r = path_extract_directory(path, &parent);
        אם (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                החזר r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        אם (fd < 0)
                החזר fd;

        strncpy(args.name, subvolume, حجم(args.name)-1);

        החזר RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *path, mode_t mode) {
        mode_t old, combined;
        صحيح r;

        أكد(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        אם (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        אם (r >= 0)
                החזר 1; /* subvol worked */
        אם (!ERRNO_IS_NOT_SUPPORTED(r))
                החזר r;

        אם (mkdirat(dir_fd, path, mode) < 0)
                החזר -errno;

        החזר 0; /* plain directory */
}
