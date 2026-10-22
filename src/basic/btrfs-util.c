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

        إذا (!filename_is_valid(name))
                أرجع -EINVAL;

        إذا (طول_رشته(name) > BTRFS_SUBVOL_NAME_MAX)
                أرجع -E2BIG;

        أرجع 0;
}

ساكن صحيح استخراج_subvolume_name(ثابت حرف *مسیر, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(مسیر);
        أكد(ret);

        r = مسیر_استخراج_filename(مسیر, &fn);
        إذا (r < 0)
                أرجع r;

        r = btrfs_validate_subvolume_name(fn);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(fn);
        أرجع 0;
}

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *مسیر) {
        بنية btrfs_ioctl_vol_args آرگومانها = {};
        _cleanup_free_ حرف *subvolume = عدم, *والد = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(مسیر));

        r = استخراج_subvolume_name(مسیر, &subvolume);
        إذا (r < 0)
                أرجع r;

        r = مسیر_استخراج_directory(مسیر, &والد);
        إذا (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                أرجع r;

        fd = xopenat(dir_fd, والد ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع fd;

        strncpy(آرگومانها.name, subvolume, حجم(آرگومانها.name)-1);

        أرجع RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &آرگومانها));
}

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *مسیر, mode_t نما) {
        mode_t old, combined;
        صحيح r;

        أكد(مسیر);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~نما);
        combined = old | ~نما;
        إذا (combined != ~نما)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, مسیر);
        umask(old);

        إذا (r >= 0)
                أرجع 1; /* subvol worked */
        إذا (!ERRNO_IS_NOT_SUPPORTED(r))
                أرجع r;

        إذا (mkdirat(dir_fd, مسیر, نما) < 0)
                أرجع -errno;

        أرجع 0; /* plain directory */
}
