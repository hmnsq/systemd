/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <sys/ioctl.h>
#समावेश <sys/stat.h>

#समावेश "bitfield.h"
#समावेश "chattr-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "recurse-dir.h"
#समावेश "string-util.h"

पूर्णांक chattr_full(
              पूर्णांक dir_fd,
              स्थिर अक्षर *path,
              अचिन्हित मूल्य,
              अचिन्हित mask,
              अचिन्हित *ret_previous,
              अचिन्हित *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ पूर्णांक fd = -EBADF;
        अचिन्हित old_attr, new_attr;
        पूर्णांक set_flags_errno = 0;
        काष्ठा stat st;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        अगर (fd < 0)
                वापस fd;

        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        अगर (!inode_type_can_chattr(st.st_mode))
                वापस -ENOTTY;

        अगर (mask == 0 && !ret_previous && !ret_final)
                वापस 0;

        अगर (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                वापस -त्रुटि_सं;

        new_attr = (old_attr & ~mask) | (मूल्य & mask);
        अगर (new_attr == old_attr) {
                अगर (ret_previous)
                        *ret_previous = old_attr;
                अगर (ret_final)
                        *ret_final = old_attr;
                वापस 0;
        }

        अगर (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                अचिन्हित attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                अगर (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        वापस -त्रुटि_सं;

                अगर (new_attr == attr) {
                        अगर (ret_previous)
                                *ret_previous = old_attr;
                        अगर (ret_final)
                                *ret_final = new_attr;
                        वापस 1;
                }

                /* Trigger the fallback logic. */
                त्रुटि_सं = EINVAL;
        }

        अगर (!ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                वापस -त्रुटि_सं;

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
                अगर (new_one == current_attr)
                        जारी;

                अगर (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        अगर (!ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं))
                                वापस -त्रुटि_सं;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       त्रुटि_सं,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        अगर (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(त्रुटि_सं))
                                set_flags_errno = -त्रुटि_सं;

                        जारी;
                }

                अगर (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        वापस -त्रुटि_सं;
        }

        अगर (ret_previous)
                *ret_previous = old_attr;
        अगर (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        वापस current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

पूर्णांक read_attr_fd(पूर्णांक fd, अचिन्हित *ret) {
        काष्ठा stat st;

        निश्चित(fd >= 0);
        निश्चित(ret);

        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        अगर (!inode_type_can_chattr(st.st_mode))
                वापस -ENOTTY;

        _cleanup_close_ पूर्णांक fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        अगर (fd < 0)
                वापस fd;

        वापस RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

पूर्णांक read_attr_at(पूर्णांक dir_fd, स्थिर अक्षर *path, अचिन्हित *ret) {
        _cleanup_close_ पूर्णांक fd_close = -EBADF;
        पूर्णांक fd;

        निश्चित(dir_fd >= 0 || dir_fd == AT_FDCWD);
        निश्चित(ret);

        अगर (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        अन्यथा {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                अगर (fd_close < 0)
                        वापस fd_close;

                fd = fd_close;
        }

        वापस read_attr_fd(fd, ret);
}

पूर्णांक read_fs_xattr_fd(पूर्णांक fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        काष्ठा fsxattr attrs;
        _cleanup_close_ पूर्णांक fd_reopened = -EBADF;

        निश्चित(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        अगर (fd < 0)
                वापस fd;

        अगर (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        अगर (ret_projid)
                *ret_projid = attrs.fsx_projid;

        वापस 0;
}

पूर्णांक set_proj_id(पूर्णांक fd, uint32_t proj_id) {
        काष्ठा fsxattr attrs;
        _cleanup_close_ पूर्णांक fd_reopened = -EBADF;

        निश्चित(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        अगर (fd < 0)
                वापस fd;

        अगर (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                वापस -त्रुटि_सं;

        काष्ठा stat statbuf;
        अगर (ख_स्थिति(fd, &statbuf) < 0)
                वापस -त्रुटि_सं;

        अगर (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                वापस 0;

        attrs.fsx_projid = proj_id;
        अगर (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        वापस RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

अटल पूर्णांक set_proj_id_cb(
                RecurseDirEvent event,
                स्थिर अक्षर *path,
                पूर्णांक dir_fd,
                पूर्णांक inode_fd,
                स्थिर काष्ठा dirent *de,
                स्थिर काष्ठा statx *sx,
                व्योम *userdata) {

        अगर (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                वापस RECURSE_DIR_CONTINUE;

        अगर (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                वापस RECURSE_DIR_CONTINUE;

        वापस set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

पूर्णांक set_proj_id_recursive(पूर्णांक fd, uint32_t proj_id) {
        वापस recurse_dir_at(
                        fd,
                        /* path= */ शून्य,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ अच_पूर्णांक_उच्च,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        वापस IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
