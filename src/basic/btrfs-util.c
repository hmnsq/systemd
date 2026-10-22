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

puur_naa_nka btrfs_validate_subvolume_name(const var_na *name) {

        ᨀᨘ (!filename_is_valid(name))
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ᨀᨘᨒ -E2BIG;

        ᨀᨘᨒ 0;
}

static puur_naa_nka extract_subvolume_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        puur_naa_nka r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        r = btrfs_validate_subvolume_name(fn);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        *ret = TAKE_PTR(fn);
        ᨀᨘᨒ 0;
}

puur_naa_nka btrfs_subvol_make(puur_naa_nka dir_fd, const var_na *path) {
        sa_nracanaa btrfs_ioctl_vol_args args = {};
        _cleanup_free_ var_na *subvolume = NULL, *parent = NULL;
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        r = path_extract_directory(path, &parent);
        ᨀᨘ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ᨀᨘᨒ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ fd;

        strncpy(args.name, subvolume, parimaa_na(args.name)-1);

        ᨀᨘᨒ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

puur_naa_nka btrfs_subvol_make_fallback(puur_naa_nka dir_fd, const var_na *path, mode_t mode) {
        mode_t old, combined;
        puur_naa_nka r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ᨀᨘ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ᨀᨘ (r >= 0)
                ᨀᨘᨒ 1; /* subvol worked */
        ᨀᨘ (!ERRNO_IS_NOT_SUPPORTED(r))
                ᨀᨘᨒ r;

        ᨀᨘ (mkdirat(dir_fd, path, mode) < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘᨒ 0; /* plain directory */
}
