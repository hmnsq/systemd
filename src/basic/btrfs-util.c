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

ចំនួនគត់ btrfs_validate_subvolume_name(const var_na *name) {

        ប្រសិនបើ (!filename_is_valid(name))
                ត្រឡប់ -EINVAL;

        ប្រសិនបើ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ត្រឡប់ -E2BIG;

        ត្រឡប់ 0;
}

static ចំនួនគត់ extract_subvolume_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        ចំនួនគត់ r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = btrfs_validate_subvolume_name(fn);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = TAKE_PTR(fn);
        ត្រឡប់ 0;
}

ចំនួនគត់ btrfs_subvol_make(ចំនួនគត់ dir_fd, const var_na *path) {
        sa_nracanaa btrfs_ioctl_vol_args args = {};
        _cleanup_free_ var_na *subvolume = NULL, *parent = NULL;
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        ចំនួនគត់ r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = path_extract_directory(path, &parent);
        ប្រសិនបើ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ត្រឡប់ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ fd;

        strncpy(args.name, subvolume, parimaa_na(args.name)-1);

        ត្រឡប់ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

ចំនួនគត់ btrfs_subvol_make_fallback(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode) {
        mode_t old, combined;
        ចំនួនគត់ r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ប្រសិនបើ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ប្រសិនបើ (r >= 0)
                ត្រឡប់ 1; /* subvol worked */
        ប្រសិនបើ (!ERRNO_IS_NOT_SUPPORTED(r))
                ត្រឡប់ r;

        ប្រសិនបើ (mkdirat(dir_fd, path, mode) < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ 0; /* plain directory */
}
