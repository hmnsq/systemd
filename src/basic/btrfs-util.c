/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <linux/btrfs.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "btrfs-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা btrfs_validate_subvolume_name(ধ্রুবক অক্ষর *name) {

        যদি (!filename_is_valid(name))
                ফেরত -EINVAL;

        যদি (strlen(name) > BTRFS_SUBVOL_NAME_MAX)
                ফেরত -E2BIG;

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা extract_subvolume_name(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *fn = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ফেরত r;

        r = btrfs_validate_subvolume_name(fn);
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(fn);
        ফেরত 0;
}

পূর্ণসংখ্যা btrfs_subvol_make(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path) {
        কাঠামো btrfs_ioctl_vol_args args = {};
        _cleanup_free_ অক্ষর *subvolume = ফাঁকা, *parent = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        যদি (r < 0)
                ফেরত r;

        r = path_extract_directory(path, &parent);
        যদি (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                ফেরত r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত fd;

        strncpy(args.name, subvolume, আকার(args.name)-1);

        ফেরত RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

পূর্ণসংখ্যা btrfs_subvol_make_fallback(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode) {
        mode_t old, combined;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        যদি (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        যদি (r >= 0)
                ফেরত 1; /* subvol worked */
        যদি (!ERRNO_IS_NOT_SUPPORTED(r))
                ফেরত r;

        যদি (mkdirat(dir_fd, path, mode) < 0)
                ফেরত -errno;

        ফেরত 0; /* plain directory */
}
