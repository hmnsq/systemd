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

        إذا (لڑی_لمبائی(name) > BTRFS_SUBVOL_NAME_MAX)
                أرجع -E2BIG;

        أرجع 0;
}

ساكن صحيح کڈھو_subvolume_name(ثابت حرف *رستہ, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(رستہ);
        أكد(ret);

        r = رستہ_کڈھو_filename(رستہ, &fn);
        إذا (r < 0)
                أرجع r;

        r = btrfs_validate_subvolume_name(fn);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(fn);
        أرجع 0;
}

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *رستہ) {
        بنية btrfs_ioctl_vol_args دلیلاں = {};
        _cleanup_free_ حرف *subvolume = عدم, *ماپا = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(رستہ));

        r = کڈھو_subvolume_name(رستہ, &subvolume);
        إذا (r < 0)
                أرجع r;

        r = رستہ_کڈھو_directory(رستہ, &ماپا);
        إذا (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                أرجع r;

        fd = xopenat(dir_fd, ماپا ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع fd;

        strncpy(دلیلاں.name, subvolume, حجم(دلیلاں.name)-1);

        أرجع RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &دلیلاں));
}

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال) {
        mode_t old, combined;
        صحيح r;

        أكد(رستہ);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~منوال);
        combined = old | ~منوال;
        إذا (combined != ~منوال)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, رستہ);
        umask(old);

        إذا (r >= 0)
                أرجع 1; /* subvol worked */
        إذا (!ERRNO_IS_NOT_SUPPORTED(r))
                أرجع r;

        إذا (mkdirat(dir_fd, رستہ, منوال) < 0)
                أرجع -errno;

        أرجع 0; /* plain directory */
}
