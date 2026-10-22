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

整数 btrfs_validate_subvolume_name(const char *name) {

        如果 (!filename_is_valid(name))
                返回 -EINVAL;

        如果 (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                返回 -E2BIG;

        返回 0;
}

static 整数 extract_subvolume_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        整数 r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        如果 (r < 0)
                返回 r;

        r = btrfs_validate_subvolume_name(fn);
        如果 (r < 0)
                返回 r;

        *ret = TAKE_PTR(fn);
        返回 0;
}

整数 btrfs_subvol_make(整数 dir_fd, const char *path) {
        struct btrfs_ioctl_vol_args args = {};
        _cleanup_free_ char *subvolume = NULL, *parent = NULL;
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        如果 (r < 0)
                返回 r;

        r = path_extract_directory(path, &parent);
        如果 (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                返回 r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        如果 (fd < 0)
                返回 fd;

        strncpy(args.name, subvolume, sizeof(args.name)-1);

        返回 RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

整数 btrfs_subvol_make_fallback(整数 dir_fd, const char *path, mode_t mode) {
        mode_t old, combined;
        整数 r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        如果 (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        如果 (r >= 0)
                返回 1; /* subvol worked */
        如果 (!ERRNO_IS_NOT_SUPPORTED(r))
                返回 r;

        如果 (mkdirat(dir_fd, path, mode) < 0)
                返回 -errno;

        返回 0; /* plain directory */
}
