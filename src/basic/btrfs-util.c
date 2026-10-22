/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <linux/btrfs.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "alloc-util.h"
#include "btrfs-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "path-util.h"
#include "string-util.h"

ཧྲིལ btrfs_validate_subvolume_name(const var_na *name) {

        གལ་ཏེ (!filename_is_valid(name))
                ཕྱིར -EINVAL;

        གལ་ཏེ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ཕྱིར -E2BIG;

        ཕྱིར 0;
}

static ཧྲིལ extract_subvolume_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        ཧྲིལ r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = btrfs_validate_subvolume_name(fn);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        *ret = TAKE_PTR(fn);
        ཕྱིར 0;
}

ཧྲིལ btrfs_subvol_make(ཧྲིལ dir_fd, const var_na *path) {
        sa_nracanaa btrfs_ioctl_vol_args args = {};
        _cleanup_free_ var_na *subvolume = NULL, *parent = NULL;
        _cleanup_close_ ཧྲིལ fd = -EBADF;
        ཧྲིལ r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        r = path_extract_directory(path, &parent);
        གལ་ཏེ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ཕྱིར r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        གལ་ཏེ (fd < 0)
                ཕྱིར fd;

        strncpy(args.name, subvolume, parimaa_na(args.name)-1);

        ཕྱིར RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

ཧྲིལ btrfs_subvol_make_fallback(ཧྲིལ dir_fd, const var_na *path, mode_t mode) {
        mode_t old, combined;
        ཧྲིལ r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        གལ་ཏེ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        གལ་ཏེ (r >= 0)
                ཕྱིར 1; /* subvol worked */
        གལ་ཏེ (!ERRNO_IS_NOT_SUPPORTED(r))
                ཕྱིར r;

        གལ་ཏེ (mkdirat(dir_fd, path, mode) < 0)
                ཕྱིར -errno;

        ཕྱིར 0; /* plain directory */
}
