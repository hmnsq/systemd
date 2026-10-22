/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/btrfs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "btrfs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ btrfs_validate_subvolume_name(ਅਚਲ ਅੱਖਰ *name) {

        ਜੇ (!filename_is_valid(name))
                ਵਾਪਸ -EINVAL;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(name) > BTRFS_SUBVOL_NAME_MAX)
                ਵਾਪਸ -E2BIG;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਕੱਢੋ_subvolume_name(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *fn = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, &fn);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = btrfs_validate_subvolume_name(fn);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(fn);
        ਵਾਪਸ 0;
}

ਪੂਰਨ btrfs_subvol_make(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਬਣਤਰ btrfs_ioctl_vol_args ਦਲੀਲਾਂ = {};
        _cleanup_free_ ਅੱਖਰ *subvolume = ਨਿਰਾ, *ਮਾਪਾ = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(!isempty(ਰਸਤਾ));

        r = ਕੱਢੋ_subvolume_name(ਰਸਤਾ, &subvolume);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = ਰਸਤਾ_ਕੱਢੋ_directory(ਰਸਤਾ, &ਮਾਪਾ);
        ਜੇ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ਵਾਪਸ r;

        fd = xopenat(dir_fd, ਮਾਪਾ ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        strncpy(ਦਲੀਲਾਂ.name, subvolume, ਆਕਾਰ(ਦਲੀਲਾਂ.name)-1);

        ਵਾਪਸ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &ਦਲੀਲਾਂ));
}

ਪੂਰਨ btrfs_subvol_make_fallback(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ) {
        mode_t old, combined;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~ਬਹੁਲਕ);
        combined = old | ~ਬਹੁਲਕ;
        ਜੇ (combined != ~ਬਹੁਲਕ)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, ਰਸਤਾ);
        umask(old);

        ਜੇ (r >= 0)
                ਵਾਪਸ 1; /* subvol worked */
        ਜੇ (!ERRNO_IS_NOT_SUPPORTED(r))
                ਵਾਪਸ r;

        ਜੇ (mkdirat(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ) < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ 0; /* plain directory */
}
