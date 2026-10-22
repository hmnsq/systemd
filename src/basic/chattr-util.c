/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/stat.h>

#ضمن "bitfield.h"
#ضمن "chattr-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "recurse-dir.h"
#ضمن "string-util.h"

صحيح chattr_full(
              صحيح dir_fd,
              ثابت حرف *path,
              unsigned value,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ صحيح fd = -EBADF;
        unsigned old_attr, new_attr;
        صحيح set_flags_errno = 0;
        بنية stat st;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        ܐܢ (fstat(fd, &st) < 0)
                ܦܢܐ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ܐܢ (!inode_type_can_chattr(st.st_mode))
                ܦܢܐ -ENOTTY;

        ܐܢ (mask == 0 && !ret_previous && !ret_final)
                ܦܢܐ 0;

        ܐܢ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ܦܢܐ -errno;

        new_attr = (old_attr & ~mask) | (value & mask);
        ܐܢ (new_attr == old_attr) {
                ܐܢ (ret_previous)
                        *ret_previous = old_attr;
                ܐܢ (ret_final)
                        *ret_final = old_attr;
                ܦܢܐ 0;
        }

        ܐܢ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ܐܢ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ܦܢܐ -errno;

                ܐܢ (new_attr == attr) {
                        ܐܢ (ret_previous)
                                *ret_previous = old_attr;
                        ܐܢ (ret_final)
                                *ret_final = new_attr;
                        ܦܢܐ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ܐܢ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ܦܢܐ -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(value, mask_one));
                ܐܢ (new_one == current_attr)
                        استمر;

                ܐܢ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ܐܢ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ܦܢܐ -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ܐܢ (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        استمر;
                }

                ܐܢ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ܦܢܐ -errno;
        }

        ܐܢ (ret_previous)
                *ret_previous = old_attr;
        ܐܢ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ܦܢܐ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

صحيح read_attr_fd(صحيح fd, unsigned *ret) {
        بنية stat st;

        أكد(fd >= 0);
        أكد(ret);

        ܐܢ (fstat(fd, &st) < 0)
                ܦܢܐ -errno;

        ܐܢ (!inode_type_can_chattr(st.st_mode))
                ܦܢܐ -ENOTTY;

        _cleanup_close_ صحيح fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        ܦܢܐ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

صحيح read_attr_at(صحيح dir_fd, ثابت حرف *path, unsigned *ret) {
        _cleanup_close_ صحيح fd_close = -EBADF;
        صحيح fd;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(ret);

        ܐܢ (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ܐܚܪܢܐ {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ܐܢ (fd_close < 0)
                        ܦܢܐ fd_close;

                fd = fd_close;
        }

        ܦܢܐ read_attr_fd(fd, ret);
}

صحيح read_fs_xattr_fd(صحيح fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        بنية fsxattr attrs;
        _cleanup_close_ صحيح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        ܐܢ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ܦܢܐ -errno;

        ܐܢ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ܐܢ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ܦܢܐ 0;
}

صحيح set_proj_id(صحيح fd, uint32_t proj_id) {
        بنية fsxattr attrs;
        _cleanup_close_ صحيح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        ܐܢ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ܦܢܐ -errno;

        بنية stat statbuf;
        ܐܢ (fstat(fd, &statbuf) < 0)
                ܦܢܐ -errno;

        ܐܢ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ܦܢܐ 0;

        attrs.fsx_projid = proj_id;
        ܐܢ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ܦܢܐ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ساكن صحيح set_proj_id_cb(
                RecurseDirEvent event,
                ثابت حرف *path,
                صحيح dir_fd,
                صحيح inode_fd,
                ثابت بنية dirent *de,
                ثابت بنية statx *sx,
                فراغ *userdata) {

        ܐܢ (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ܦܢܐ RECURSE_DIR_CONTINUE;

        ܐܢ (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ܦܢܐ RECURSE_DIR_CONTINUE;

        ܦܢܐ set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

صحيح set_proj_id_recursive(صحيح fd, uint32_t proj_id) {
        ܦܢܐ recurse_dir_at(
                        fd,
                        /* path= */ عدم,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ܦܢܐ IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
