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

        إذا (لڙي_ڊيگهه(name) > BTRFS_SUBVOL_NAME_MAX)
                أرجع -E2BIG;

        أرجع 0;
}

ساكن صحيح ڪڍو_subvolume_name(ثابت حرف *رستو, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(رستو);
        أكد(ret);

        r = رستو_ڪڍو_filename(رستو, &fn);
        إذا (r < 0)
                أرجع r;

        r = btrfs_validate_subvolume_name(fn);
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(fn);
        أرجع 0;
}

صحيح btrfs_subvol_make(صحيح dir_fd, ثابت حرف *رستو) {
        بنية btrfs_ioctl_vol_args دليلون = {};
        _cleanup_free_ حرف *subvolume = عدم, *والد = عدم;
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(رستو));

        r = ڪڍو_subvolume_name(رستو, &subvolume);
        إذا (r < 0)
                أرجع r;

        r = رستو_ڪڍو_directory(رستو, &والد);
        إذا (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                أرجع r;

        fd = xopenat(dir_fd, والد ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع fd;

        strncpy(دليلون.name, subvolume, حجم(دليلون.name)-1);

        أرجع RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &دليلون));
}

صحيح btrfs_subvol_make_fallback(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي) {
        mode_t old, combined;
        صحيح r;

        أكد(رستو);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~گهڻائي);
        combined = old | ~گهڻائي;
        إذا (combined != ~گهڻائي)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, رستو);
        umask(old);

        إذا (r >= 0)
                أرجع 1; /* subvol worked */
        إذا (!ERRNO_IS_NOT_SUPPORTED(r))
                أرجع r;

        إذا (mkdirat(dir_fd, رستو, گهڻائي) < 0)
                أرجع -errno;

        أرجع 0; /* plain directory */
}
