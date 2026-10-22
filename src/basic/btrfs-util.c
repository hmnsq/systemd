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

ကိန်းပြည့် btrfs_validate_subvolume_name(const var_na *name) {

        အကယ်၍ (!filename_is_valid(name))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ပြန်ပေး -E2BIG;

        ပြန်ပေး 0;
}

static ကိန်းပြည့် extract_subvolume_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        ကိန်းပြည့် r;

        assert(path);
        assert(ret);

        r = path_extract_filename(path, &fn);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = btrfs_validate_subvolume_name(fn);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        *ret = TAKE_PTR(fn);
        ပြန်ပေး 0;
}

ကိန်းပြည့် btrfs_subvol_make(ကိန်းပြည့် dir_fd, const var_na *path) {
        sa_nracanaa btrfs_ioctl_vol_args args = {};
        _cleanup_free_ var_na *subvolume = NULL, *parent = NULL;
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;
        ကိန်းပြည့် r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        r = path_extract_directory(path, &parent);
        အကယ်၍ (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ပြန်ပေး r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        strncpy(args.name, subvolume, parimaa_na(args.name)-1);

        ပြန်ပေး RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

ကိန်းပြည့် btrfs_subvol_make_fallback(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode) {
        mode_t old, combined;
        ကိန်းပြည့် r;

        assert(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        အကယ်၍ (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        အကယ်၍ (r >= 0)
                ပြန်ပေး 1; /* subvol worked */
        အကယ်၍ (!ERRNO_IS_NOT_SUPPORTED(r))
                ပြန်ပေး r;

        အကယ်၍ (mkdirat(dir_fd, path, mode) < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး 0; /* plain directory */
}
