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

số_nguyên btrfs_validate_subvolume_name(const char *name) {

        nếu (!filename_is_valid(name))
                trả_về -EINVAL;

        nếu (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                trả_về -E2BIG;

        trả_về 0;
}

static số_nguyên extract_subvolume_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        số_nguyên r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        nếu (r < 0)
                trả_về r;

        r = btrfs_validate_subvolume_name(fn);
        nếu (r < 0)
                trả_về r;

        *ret = TAKE_PTR(fn);
        trả_về 0;
}

số_nguyên btrfs_subvol_make(số_nguyên dir_fd, const char *path) {
        struct btrfs_ioctl_vol_args args = {};
        _cleanup_free_ char *subvolume = NULL, *parent = NULL;
        _cleanup_close_ số_nguyên fd = -EBADF;
        số_nguyên r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        nếu (r < 0)
                trả_về r;

        r = path_extract_directory(path, &parent);
        nếu (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                trả_về r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        nếu (fd < 0)
                trả_về fd;

        strncpy(args.name, subvolume, sizeof(args.name)-1);

        trả_về RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

số_nguyên btrfs_subvol_make_fallback(số_nguyên dir_fd, const char *path, mode_t mode) {
        mode_t old, combined;
        số_nguyên r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        nếu (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        nếu (r >= 0)
                trả_về 1; /* subvol worked */
        nếu (!ERRNO_IS_NOT_SUPPORTED(r))
                trả_về r;

        nếu (mkdirat(dir_fd, path, mode) < 0)
                trả_về -errno;

        trả_về 0; /* plain directory */
}
