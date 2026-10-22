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

صحیح btrfs_validate_subvolume_name(ثابت حرف *name) {

        اگر (!filename_is_valid(name))
                واپس -EINVAL;

        اگر (لڑی_لمبائی(name) > BTRFS_SUBVOL_NAME_MAX)
                واپس -E2BIG;

        واپس 0;
}

ساکن صحیح نکالو_subvolume_name(ثابت حرف *راستہ, حرف **ret) {
        _cleanup_free_ حرف *fn = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(ret);

        r = راستہ_نکالو_filename(راستہ, &fn);
        اگر (r < 0)
                واپس r;

        r = btrfs_validate_subvolume_name(fn);
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(fn);
        واپس 0;
}

صحیح btrfs_subvol_make(صحیح dir_fd, ثابت حرف *راستہ) {
        ساخت btrfs_ioctl_vol_args دلائل = {};
        _cleanup_free_ حرف *subvolume = تہی, *والد = تہی;
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(!isempty(راستہ));

        r = نکالو_subvolume_name(راستہ, &subvolume);
        اگر (r < 0)
                واپس r;

        r = راستہ_نکالو_directory(راستہ, &والد);
        اگر (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                واپس r;

        fd = xopenat(dir_fd, والد ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        اگر (fd < 0)
                واپس fd;

        strncpy(دلائل.name, subvolume, حجم(دلائل.name)-1);

        واپس RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &دلائل));
}

صحیح btrfs_subvol_make_fallback(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال) {
        mode_t old, combined;
        صحیح r;

        أكد(راستہ);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~منوال);
        combined = old | ~منوال;
        اگر (combined != ~منوال)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, راستہ);
        umask(old);

        اگر (r >= 0)
                واپس 1; /* subvol worked */
        اگر (!ERRNO_IS_NOT_SUPPORTED(r))
                واپس r;

        اگر (mkdirat(dir_fd, راستہ, منوال) < 0)
                واپس -errno;

        واپس 0; /* plain directory */
}
