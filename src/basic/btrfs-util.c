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

정수 btrfs_validate_subvolume_name(const char *name) {

        만약 (!filename_is_valid(name))
                반환 -EINVAL;

        만약 (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                반환 -E2BIG;

        반환 0;
}

static 정수 extract_subvolume_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        정수 r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        만약 (r < 0)
                반환 r;

        r = btrfs_validate_subvolume_name(fn);
        만약 (r < 0)
                반환 r;

        *ret = TAKE_PTR(fn);
        반환 0;
}

정수 btrfs_subvol_make(정수 dir_fd, const char *path) {
        struct btrfs_ioctl_vol_args args = {};
        _cleanup_free_ char *subvolume = NULL, *parent = NULL;
        _cleanup_close_ 정수 fd = -EBADF;
        정수 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        만약 (r < 0)
                반환 r;

        r = path_extract_directory(path, &parent);
        만약 (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                반환 r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        만약 (fd < 0)
                반환 fd;

        strncpy(args.name, subvolume, sizeof(args.name)-1);

        반환 RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

정수 btrfs_subvol_make_fallback(정수 dir_fd, const char *path, mode_t mode) {
        mode_t old, combined;
        정수 r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        만약 (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        만약 (r >= 0)
                반환 1; /* subvol worked */
        만약 (!ERRNO_IS_NOT_SUPPORTED(r))
                반환 r;

        만약 (mkdirat(dir_fd, path, mode) < 0)
                반환 -errno;

        반환 0; /* plain directory */
}
