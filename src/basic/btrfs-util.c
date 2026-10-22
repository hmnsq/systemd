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

පූර්ණ_සංඛ්‍යා btrfs_validate_subvolume_name(const var_na *name) {

        නම් (!filename_is_valid(name))
                ආපසු -EINVAL;

        නම් (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ආපසු -E2BIG;

        ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා extract_subvolume_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        නම් (r < 0)
                ආපසු r;

        r = btrfs_validate_subvolume_name(fn);
        නම් (r < 0)
                ආපසු r;

        *ret = TAKE_PTR(fn);
        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා btrfs_subvol_make(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path) {
        sa_nracanaa btrfs_ioctl_vol_args args = {};
        _cleanup_free_ var_na *subvolume = NULL, *parent = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;
        පූර්ණ_සංඛ්‍යා r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        නම් (r < 0)
                ආපසු r;

        r = path_extract_directory(path, &parent);
        නම් (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ආපසු r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        නම් (fd < 0)
                ආපසු fd;

        strncpy(args.name, subvolume, parimaa_na(args.name)-1);

        ආපසු RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

පූර්ණ_සංඛ්‍යා btrfs_subvol_make_fallback(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode) {
        mode_t old, combined;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        නම් (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        නම් (r >= 0)
                ආපසු 1; /* subvol worked */
        නම් (!ERRNO_IS_NOT_SUPPORTED(r))
                ආපසු r;

        නම් (mkdirat(dir_fd, path, mode) < 0)
                ආපසු -errno;

        ආපසු 0; /* plain directory */
}
