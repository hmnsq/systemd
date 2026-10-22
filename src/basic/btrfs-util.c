/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <linux/btrfs.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "btrfs-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା btrfs_validate_subvolume_name(ଅଚଳ ଅକ୍ଷର *name) {

        ଯଦି (!filename_is_valid(name))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ଫେରନ୍ତୁ -E2BIG;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା extract_subvolume_name(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *fn = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(ret);

        r = path_extract_filename(path, &fn);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = btrfs_validate_subvolume_name(fn);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(fn);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା btrfs_subvol_make(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path) {
        ସଂରଚନା btrfs_ioctl_vol_args args = {};
        _cleanup_free_ ଅକ୍ଷର *subvolume = ଖାଲି, *parent = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = path_extract_directory(path, &parent);
        ଯଦି (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ଫେରନ୍ତୁ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        strncpy(args.name, subvolume, ଆକାର(args.name)-1);

        ଫେରନ୍ତୁ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା btrfs_subvol_make_fallback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode) {
        mode_t old, combined;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        ଯଦି (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ 1; /* subvol worked */
        ଯଦି (!ERRNO_IS_NOT_SUPPORTED(r))
                ଫେରନ୍ତୁ r;

        ଯଦି (mkdirat(dir_fd, path, mode) < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0; /* plain directory */
}
