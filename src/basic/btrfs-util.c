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

цяло btrfs_validate_subvolume_name(const char *name) {

        ако (!filename_is_valid(name))
                върни -EINVAL;

        ако (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                върни -E2BIG;

        върни 0;
}

static цяло extract_subvolume_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        цяло r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        ако (r < 0)
                върни r;

        r = btrfs_validate_subvolume_name(fn);
        ако (r < 0)
                върни r;

        *ret = TAKE_PTR(fn);
        върни 0;
}

цяло btrfs_subvol_make(цяло dir_fd, const char *path) {
        struct btrfs_ioctl_vol_args args = {};
        _cleanup_free_ char *subvolume = NULL, *parent = NULL;
        _cleanup_close_ цяло fd = -EBADF;
        цяло r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ако (r < 0)
                върни r;

        r = path_extract_directory(path, &parent);
        ако (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                върни r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ако (fd < 0)
                върни fd;

        strncpy(args.name, subvolume, sizeof(args.name)-1);

        върни RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

цяло btrfs_subvol_make_fallback(цяло dir_fd, const char *path, mode_t mode) {
        mode_t old, combined;
        цяло r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ако (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ако (r >= 0)
                върни 1; /* subvol worked */
        ако (!ERRNO_IS_NOT_SUPPORTED(r))
                върни r;

        ако (mkdirat(dir_fd, path, mode) < 0)
                върни -errno;

        върни 0; /* plain directory */
}
