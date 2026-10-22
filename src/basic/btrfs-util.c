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

จำนวนเต็ม btrfs_validate_subvolume_name(const var_na *name) {

        ถ้า (!filename_is_valid(name))
                คืน -EINVAL;

        ถ้า (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                คืน -E2BIG;

        คืน 0;
}

static จำนวนเต็ม extract_subvolume_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        ถ้า (r < 0)
                คืน r;

        r = btrfs_validate_subvolume_name(fn);
        ถ้า (r < 0)
                คืน r;

        *ret = TAKE_PTR(fn);
        คืน 0;
}

จำนวนเต็ม btrfs_subvol_make(จำนวนเต็ม dir_fd, const var_na *path) {
        sa_nracanaa btrfs_ioctl_vol_args args = {};
        _cleanup_free_ var_na *subvolume = NULL, *parent = NULL;
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        จำนวนเต็ม r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ถ้า (r < 0)
                คืน r;

        r = path_extract_directory(path, &parent);
        ถ้า (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                คืน r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ถ้า (fd < 0)
                คืน fd;

        strncpy(args.name, subvolume, parimaa_na(args.name)-1);

        คืน RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

จำนวนเต็ม btrfs_subvol_make_fallback(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode) {
        mode_t old, combined;
        จำนวนเต็ม r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ถ้า (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ถ้า (r >= 0)
                คืน 1; /* subvol worked */
        ถ้า (!ERRNO_IS_NOT_SUPPORTED(r))
                คืน r;

        ถ้า (mkdirat(dir_fd, path, mode) < 0)
                คืน -errno;

        คืน 0; /* plain directory */
}
