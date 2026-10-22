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

पूर्णांक btrfs_validate_subvolume_name(स्थिरांक अक्षर *name) {

        जर (!filename_is_valid(name))
                परत -EINVAL;

        जर (म_माप(name) > BTRFS_SUBVOL_NAME_MAX)
                परत -E2BIG;

        परत 0;
}

स्थिर पूर्णांक extract_subvolume_name(स्थिरांक अक्षर *path, अक्षर **ret) {
        _cleanup_free_ अक्षर *fn = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        r = path_extract_filename(path, &fn);
        जर (r < 0)
                परत r;

        r = btrfs_validate_subvolume_name(fn);
        जर (r < 0)
                परत r;

        *ret = TAKE_PTR(fn);
        परत 0;
}

पूर्णांक btrfs_subvol_make(पूर्णांक dir_fd, स्थिरांक अक्षर *path) {
        संरचना btrfs_ioctl_vol_args args = {};
        _cleanup_free_ अक्षर *subvolume = रिक्त, *parent = रिक्त;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(wildcard_fd_is_valid(dir_fd));
        प्रतिज्ञा(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        जर (r < 0)
                परत r;

        r = path_extract_directory(path, &parent);
        जर (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                परत r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        जर (fd < 0)
                परत fd;

        म_नकलन(args.name, subvolume, आकार(args.name)-1);

        परत RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

पूर्णांक btrfs_subvol_make_fallback(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode) {
        mode_t old, combined;
        पूर्णांक r;

        प्रतिज्ञा(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        जर (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        जर (r >= 0)
                परत 1; /* subvol worked */
        जर (!ERRNO_IS_NOT_SUPPORTED(r))
                परत r;

        जर (mkdirat(dir_fd, path, mode) < 0)
                परत -त्रुटि_सं;

        परत 0; /* plain directory */
}
