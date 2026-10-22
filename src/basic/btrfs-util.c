/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <linux/btrfs.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "btrfs-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"

पूर्णाङ्क btrfs_validate_subvolume_name(अचर वर्ण *name) {

        यदि (!filename_is_valid(name))
                फर्काऊ -EINVAL;

        यदि (म_माप(name) > BTRFS_SUBVOL_NAME_MAX)
                फर्काऊ -E2BIG;

        फर्काऊ 0;
}

स्थिर पूर्णाङ्क extract_subvolume_name(अचर वर्ण *path, वर्ण **ret) {
        _cleanup_free_ वर्ण *fn = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        r = path_extract_filename(path, &fn);
        यदि (r < 0)
                फर्काऊ r;

        r = btrfs_validate_subvolume_name(fn);
        यदि (r < 0)
                फर्काऊ r;

        *ret = TAKE_PTR(fn);
        फर्काऊ 0;
}

पूर्णाङ्क btrfs_subvol_make(पूर्णाङ्क dir_fd, अचर वर्ण *path) {
        संरचना btrfs_ioctl_vol_args args = {};
        _cleanup_free_ वर्ण *subvolume = रिक्त, *parent = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        यदि (r < 0)
                फर्काऊ r;

        r = path_extract_directory(path, &parent);
        यदि (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                फर्काऊ r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        यदि (fd < 0)
                फर्काऊ fd;

        म_नकलन(args.name, subvolume, परिमाण(args.name)-1);

        फर्काऊ RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

पूर्णाङ्क btrfs_subvol_make_fallback(पूर्णाङ्क dir_fd, अचर वर्ण *path, mode_t mode) {
        mode_t old, combined;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        यदि (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        यदि (r >= 0)
                फर्काऊ 1; /* subvol worked */
        यदि (!ERRNO_IS_NOT_SUPPORTED(r))
                फर्काऊ r;

        यदि (mkdirat(dir_fd, path, mode) < 0)
                फर्काऊ -त्रुटि_सं;

        फर्काऊ 0; /* plain directory */
}
