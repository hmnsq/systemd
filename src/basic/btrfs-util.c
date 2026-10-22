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

        إذا (مزي_اوږدوالی(name) > BTRFS_SUBVOL_NAME_MAX)
                أرجع -E2BIG;

        أرجع 0;
}

ساكن صحيح ایستل_subvolume_name(ثابت حرف *لار, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(لار);
        أكد(ret);

        r = لار_ایستل_filename(لار, &fn);
        إذا (r < 0)
                أرجع r;

        r = btrfs_validate_subvolume_name(fn);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(fn);
        أرجع 0;
}

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *لار) {
        بنية btrfs_ioctl_vol_args دلیلونه = {};
        _cleanup_free_ حرف *subvolume = عدم, *پلار = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(لار));

        r = ایستل_subvolume_name(لار, &subvolume);
        إذا (r < 0)
                أرجع r;

        r = لار_ایستل_directory(لار, &پلار);
        إذا (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                أرجع r;

        fd = xopenat(dir_fd, پلار ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع fd;

        strncpy(دلیلونه.name, subvolume, حجم(دلیلونه.name)-1);

        أرجع RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &دلیلونه));
}

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ) {
        mode_t old, combined;
        صحيح r;

        أكد(لار);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~ډېرپېښ);
        combined = old | ~ډېرپېښ;
        إذا (combined != ~ډېرپېښ)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, لار);
        umask(old);

        إذا (r >= 0)
                أرجع 1; /* subvol worked */
        إذا (!ERRNO_IS_NOT_SUPPORTED(r))
                أرجع r;

        إذا (mkdirat(dir_fd, لار, ډېرپېښ) < 0)
                أرجع -errno;

        أرجع 0; /* plain directory */
}
