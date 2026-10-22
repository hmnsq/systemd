/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <linux/btrfs.h>
#समावेश <sys/ioctl.h>
#समावेश <sys/stat.h>

#समावेश "alloc-util.h"
#समावेश "btrfs-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "path-util.h"
#समावेश "string-util.h"

पूर्णांक btrfs_validate_subvolume_name(स्थिर अक्षर *name) {

        अगर (!filename_is_valid(name))
                वापस -EINVAL;

        अगर (म_माप(name) > BTRFS_SUBVOL_NAME_MAX)
                वापस -E2BIG;

        वापस 0;
}

अटल पूर्णांक extract_subvolume_name(स्थिर अक्षर *path, अक्षर **ret) {
        _cleanup_free_ अक्षर *fn = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(ret);

        r = path_extract_filename(path, &fn);
        अगर (r < 0)
                वापस r;

        r = btrfs_validate_subvolume_name(fn);
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(fn);
        वापस 0;
}

पूर्णांक btrfs_subvol_make(पूर्णांक dir_fd, स्थिर अक्षर *path) {
        काष्ठा btrfs_ioctl_vol_args args = {};
        _cleanup_free_ अक्षर *subvolume = शून्य, *parent = शून्य;
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(!isempty(path));

        r = extract_subvolume_name(path, &subvolume);
        अगर (r < 0)
                वापस r;

        r = path_extract_directory(path, &parent);
        अगर (r < 0 && r != -EDESTADDRREQ) /* Propagate error, unless only a filename was specified, which is OK */
                वापस r;

        fd = xopenat(dir_fd, parent ?: ".", O_DIRECTORY|O_RDONLY|O_CLOEXEC);
        अगर (fd < 0)
                वापस fd;

        म_नकलन(args.name, subvolume, माप(args.name)-1);

        वापस RET_NERRNO(ioctl(fd, BTRFS_IOC_SUBVOL_CREATE, &args));
}

पूर्णांक btrfs_subvol_make_fallback(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode) {
        mode_t old, combined;
        पूर्णांक r;

        निश्चित(path);

        /* Let's work like mkdir(), i.e. take the specified mode, and mask it with the current umask. */
        old = umask(~mode);
        combined = old | ~mode;
        अगर (combined != ~mode)
                umask(combined);
        r = btrfs_subvol_make(dir_fd, path);
        umask(old);

        अगर (r >= 0)
                वापस 1; /* subvol worked */
        अगर (!ERRNO_IS_NOT_SUPPORTED(r))
                वापस r;

        अगर (mkdirat(dir_fd, path, mode) < 0)
                वापस -त्रुटि_सं;

        वापस 0; /* plain directory */
}
