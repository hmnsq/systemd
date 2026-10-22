/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "bitfield.h"
#अन्तर्भाव "chattr-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "recurse-dir.h"
#अन्तर्भाव "string-util.h"

पूर्णांक chattr_full(
              पूर्णांक dir_fd,
              स्थिरांक अक्षर *path,
              अचिन्हित मूल्य,
              अचिन्हित mask,
              अचिन्हित *ret_previous,
              अचिन्हित *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ पूर्णांक fd = -EBADF;
        अचिन्हित old_attr, new_attr;
        पूर्णांक set_flags_errno = 0;
        संरचना stat st;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        जर (fd < 0)
                परत fd;

        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        जर (!inode_type_can_chattr(st.st_mode))
                परत -ENOTTY;

        जर (mask == 0 && !ret_previous && !ret_final)
                परत 0;

        जर (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                परत -त्रुटि_सं;

        new_attr = (old_attr & ~mask) | (मूल्य & mask);
        जर (new_attr == old_attr) {
                जर (ret_previous)
                        *ret_previous = old_attr;
                जर (ret_final)
                        *ret_final = old_attr;
                परत 0;
        }

        जर (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                अचिन्हित attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                जर (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        परत -त्रुटि_सं;

                जर (new_attr == attr) {
                        जर (ret_previous)
                                *ret_previous = old_attr;
                        जर (ret_final)
                                *ret_final = new_attr;
                        परत 1;
                }

                /* Trigger the fallback logic. */
                त्रुटि_सं = EINVAL;
        }

        जर (!ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                परत -त्रुटि_सं;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        अचिन्हित current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                अचिन्हित new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(मूल्य, mask_one));
                जर (new_one == current_attr)
                        सुरूठेवा;

                जर (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        जर (!ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं))
                                परत -त्रुटि_सं;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       त्रुटि_सं,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        जर (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं))
                                set_flags_errno = -त्रुटि_सं;

                        सुरूठेवा;
                }

                जर (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        परत -त्रुटि_सं;
        }

        जर (ret_previous)
                *ret_previous = old_attr;
        जर (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        परत current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

पूर्णांक read_attr_fd(पूर्णांक fd, अचिन्हित *ret) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        जर (!inode_type_can_chattr(st.st_mode))
                परत -ENOTTY;

        _cleanup_close_ पूर्णांक fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        जर (fd < 0)
                परत fd;

        परत RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

पूर्णांक read_attr_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, अचिन्हित *ret) {
        _cleanup_close_ पूर्णांक fd_close = -EBADF;
        पूर्णांक fd;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(ret);

        जर (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        अन्यथा {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                जर (fd_close < 0)
                        परत fd_close;

                fd = fd_close;
        }

        परत read_attr_fd(fd, ret);
}

पूर्णांक read_fs_xattr_fd(पूर्णांक fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        संरचना fsxattr attrs;
        _cleanup_close_ पूर्णांक fd_reopened = -EBADF;

        प्रतिज्ञा(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        जर (fd < 0)
                परत fd;

        जर (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                परत -त्रुटि_सं;

        जर (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        जर (ret_projid)
                *ret_projid = attrs.fsx_projid;

        परत 0;
}

पूर्णांक set_proj_id(पूर्णांक fd, uint32_t proj_id) {
        संरचना fsxattr attrs;
        _cleanup_close_ पूर्णांक fd_reopened = -EBADF;

        प्रतिज्ञा(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        जर (fd < 0)
                परत fd;

        जर (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                परत -त्रुटि_सं;

        संरचना stat statbuf;
        जर (ख_स्थिति(fd, &statbuf) < 0)
                परत -त्रुटि_सं;

        जर (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                परत 0;

        attrs.fsx_projid = proj_id;
        जर (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        परत RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

स्थिर पूर्णांक set_proj_id_cb(
                RecurseDirEvent event,
                स्थिरांक अक्षर *path,
                पूर्णांक dir_fd,
                पूर्णांक inode_fd,
                स्थिरांक संरचना dirent *de,
                स्थिरांक संरचना statx *sx,
                शून्य *userdata) {

        जर (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                परत RECURSE_DIR_CONTINUE;

        जर (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                परत RECURSE_DIR_CONTINUE;

        परत set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

पूर्णांक set_proj_id_recursive(पूर्णांक fd, uint32_t proj_id) {
        परत recurse_dir_at(
                        fd,
                        /* path= */ रिक्त,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ अच_पूर्णांक_उच्च,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        परत IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
