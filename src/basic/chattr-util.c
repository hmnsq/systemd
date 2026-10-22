/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/ioctl.h>
#شامل <sys/stat.h>

#شامل "bitfield.h"
#شامل "chattr-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "recurse-dir.h"
#شامل "string-util.h"

صحیح chattr_full(
              صحیح dir_fd,
              ثابت نویسه *path,
              unsigned مقدار,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ صحیح fd = -EBADF;
        unsigned old_attr, new_attr;
        صحیح set_flags_errno = 0;
        ساختار stat st;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        اگر (fd < 0)
                بازگشت fd;

        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        اگر (!inode_type_can_chattr(st.st_mode))
                بازگشت -ENOTTY;

        اگر (mask == 0 && !ret_previous && !ret_final)
                بازگشت 0;

        اگر (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                بازگشت -errno;

        new_attr = (old_attr & ~mask) | (مقدار & mask);
        اگر (new_attr == old_attr) {
                اگر (ret_previous)
                        *ret_previous = old_attr;
                اگر (ret_final)
                        *ret_final = old_attr;
                بازگشت 0;
        }

        اگر (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                اگر (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        بازگشت -errno;

                اگر (new_attr == attr) {
                        اگر (ret_previous)
                                *ret_previous = old_attr;
                        اگر (ret_final)
                                *ret_final = new_attr;
                        بازگشت 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        اگر (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                بازگشت -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(مقدار, mask_one));
                اگر (new_one == current_attr)
                        ادامه;

                اگر (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        اگر (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                بازگشت -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        اگر (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        ادامه;
                }

                اگر (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        بازگشت -errno;
        }

        اگر (ret_previous)
                *ret_previous = old_attr;
        اگر (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        بازگشت current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

صحیح read_attr_fd(صحیح fd, unsigned *ret) {
        ساختار stat st;

        أكد(fd >= 0);
        أكد(ret);

        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        اگر (!inode_type_can_chattr(st.st_mode))
                بازگشت -ENOTTY;

        _cleanup_close_ صحیح fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        اگر (fd < 0)
                بازگشت fd;

        بازگشت RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

صحیح read_attr_at(صحیح dir_fd, ثابت نویسه *path, unsigned *ret) {
        _cleanup_close_ صحیح fd_close = -EBADF;
        صحیح fd;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(ret);

        اگر (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        وگرنه {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                اگر (fd_close < 0)
                        بازگشت fd_close;

                fd = fd_close;
        }

        بازگشت read_attr_fd(fd, ret);
}

صحیح read_fs_xattr_fd(صحیح fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ساختار fsxattr attrs;
        _cleanup_close_ صحیح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        اگر (fd < 0)
                بازگشت fd;

        اگر (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                بازگشت -errno;

        اگر (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        اگر (ret_projid)
                *ret_projid = attrs.fsx_projid;

        بازگشت 0;
}

صحیح set_proj_id(صحیح fd, uint32_t proj_id) {
        ساختار fsxattr attrs;
        _cleanup_close_ صحیح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        اگر (fd < 0)
                بازگشت fd;

        اگر (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                بازگشت -errno;

        ساختار stat statbuf;
        اگر (fstat(fd, &statbuf) < 0)
                بازگشت -errno;

        اگر (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                بازگشت 0;

        attrs.fsx_projid = proj_id;
        اگر (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        بازگشت RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ایستا صحیح set_proj_id_cb(
                RecurseDirEvent event,
                ثابت نویسه *path,
                صحیح dir_fd,
                صحیح inode_fd,
                ثابت ساختار dirent *de,
                ثابت ساختار statx *sx,
                تهی *userdata) {

        اگر (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                بازگشت RECURSE_DIR_CONTINUE;

        اگر (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                بازگشت RECURSE_DIR_CONTINUE;

        بازگشت set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

صحیح set_proj_id_recursive(صحیح fd, uint32_t proj_id) {
        بازگشت recurse_dir_at(
                        fd,
                        /* path= */ پوچ,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        بازگشت IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
