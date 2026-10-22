/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <linux/btrfs.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "btrfs-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "string-util.h"

முழு btrfs_validate_subvolume_name(மாறிலி எழுத்து *name) {

        எனில் (!filename_is_valid(name))
                திரும்பு -EINVAL;

        எனில் (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                திரும்பு -E2BIG;

        திரும்பு 0;
}

நிலையான முழு extract_subvolume_name(மாறிலி எழுத்து *path, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *fn = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(ret);

        r = path_extract_filename(path, &fn);
        எனில் (r < 0)
                திரும்பு r;

        r = btrfs_validate_subvolume_name(fn);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(fn);
        திரும்பு 0;
}

முழு btrfs_subvol_make(முழு dir_fd, மாறிலி எழுத்து *path) {
        கட்டமைப்பு btrfs_ioctl_vol_args args = {};
        _cleanup_free_ எழுத்து *subvolume = வெற்று, *parent = வெற்று;
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        எனில் (r < 0)
                திரும்பு r;

        r = path_extract_directory(path, &parent);
        எனில் (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                திரும்பு r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு fd;

        strncpy(args.name, subvolume, அளவு(args.name)-1);

        திரும்பு RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

முழு btrfs_subvol_make_fallback(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode) {
        mode_t old, combined;
        முழு r;

        ப்ரதிஜ்ஞா(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        எனில் (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        எனில் (r >= 0)
                திரும்பு 1; /* subvol worked */
        எனில் (!ERRNO_IS_NOT_SUPPORTED(r))
                திரும்பு r;

        எனில் (mkdirat(dir_fd, path, mode) < 0)
                திரும்பு -errno;

        திரும்பு 0; /* plain directory */
}
