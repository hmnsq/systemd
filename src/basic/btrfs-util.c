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

целое btrfs_validate_subvolume_name(const char *name) {

        если (!filename_is_valid(name))
                вернуть -EINVAL;

        если (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                вернуть -E2BIG;

        вернуть 0;
}

static целое extract_subvolume_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        целое r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        если (r < 0)
                вернуть r;

        r = btrfs_validate_subvolume_name(fn);
        если (r < 0)
                вернуть r;

        *ret = TAKE_PTR(fn);
        вернуть 0;
}

целое btrfs_subvol_make(целое dir_fd, const char *path) {
        struct btrfs_ioctl_vol_args args = {};
        _cleanup_free_ char *subvolume = NULL, *parent = NULL;
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        если (r < 0)
                вернуть r;

        r = path_extract_directory(path, &parent);
        если (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                вернуть r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        если (fd < 0)
                вернуть fd;

        strncpy(args.name, subvolume, sizeof(args.name)-1);

        вернуть RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

целое btrfs_subvol_make_fallback(целое dir_fd, const char *path, mode_t mode) {
        mode_t old, combined;
        целое r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        если (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        если (r >= 0)
                вернуть 1; /* subvol worked */
        если (!ERRNO_IS_NOT_SUPPORTED(r))
                вернуть r;

        если (mkdirat(dir_fd, path, mode) < 0)
                вернуть -errno;

        вернуть 0; /* plain directory */
}
