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

ціле btrfs_validate_subvolume_name(const char *name) {

        якщо (!filename_is_valid(name))
                повернути -EINVAL;

        якщо (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                повернути -E2BIG;

        повернути 0;
}

static ціле extract_subvolume_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        ціле r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        якщо (r < 0)
                повернути r;

        r = btrfs_validate_subvolume_name(fn);
        якщо (r < 0)
                повернути r;

        *ret = TAKE_PTR(fn);
        повернути 0;
}

ціле btrfs_subvol_make(ціле dir_fd, const char *path) {
        struct btrfs_ioctl_vol_args args = {};
        _cleanup_free_ char *subvolume = NULL, *parent = NULL;
        _cleanup_close_ ціле fd = -EBADF;
        ціле r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        якщо (r < 0)
                повернути r;

        r = path_extract_directory(path, &parent);
        якщо (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                повернути r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        якщо (fd < 0)
                повернути fd;

        strncpy(args.name, subvolume, sizeof(args.name)-1);

        повернути RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

ціле btrfs_subvol_make_fallback(ціле dir_fd, const char *path, mode_t mode) {
        mode_t old, combined;
        ціле r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        якщо (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        якщо (r >= 0)
                повернути 1; /* subvol worked */
        якщо (!ERRNO_IS_NOT_SUPPORTED(r))
                повернути r;

        якщо (mkdirat(dir_fd, path, mode) < 0)
                повернути -errno;

        повернути 0; /* plain directory */
}
