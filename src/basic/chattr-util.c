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

صحيح chattr_پر(
              صحيح dir_fd,
              ثابت حرف *مسیر,
              بیعلامت مقدار,
              بیعلامت ماسک_3,
              بیعلامت *ret_قبلی,
              بیعلامت *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ صحيح fd = -EBADF;
        بیعلامت old_attr, new_attr;
        صحيح مجموعه_flags_errno = 0;
        بنية stat st;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, مسیر, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        إذا (fd < 0)
                أرجع fd;

        إذا (fstat(fd, &st) < 0)
                أرجع -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        إذا (!inode_type_can_chattr(st.st_mode))
                أرجع -ENOTTY;

        إذا (ماسک_3 == 0 && !ret_قبلی && !ret_final)
                أرجع 0;

        إذا (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                أرجع -errno;

        new_attr = (old_attr & ~ماسک_3) | (مقدار & ماسک_3);
        إذا (new_attr == old_attr) {
                إذا (ret_قبلی)
                        *ret_قبلی = old_attr;
                إذا (ret_final)
                        *ret_final = old_attr;
                أرجع 0;
        }

        إذا (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                بیعلامت attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                إذا (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        أرجع -errno;

                إذا (new_attr == attr) {
                        إذا (ret_قبلی)
                                *ret_قبلی = old_attr;
                        إذا (ret_final)
                                *ret_final = new_attr;
                        أرجع 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        إذا (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_مجموعه(flags, CHATTR_FALLBACK_BITWISE))
                أرجع -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        بیعلامت current_attr = old_attr;

        بیت_FOREACH(i, ماسک_3) {
                بیعلامت new_one, ماسک_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, ماسک_one, FLAGS_مجموعه(مقدار, ماسک_one));
                إذا (new_one == current_attr)
                        استمر;

                إذا (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        إذا (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                أرجع -errno;

                        log_پر_errno(FLAGS_مجموعه(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", ماسک_one, strna(مسیر));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        إذا (مجموعه_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                مجموعه_flags_errno = -errno;

                        استمر;
                }

                إذا (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        أرجع -errno;
        }

        إذا (ret_قبلی)
                *ret_قبلی = old_attr;
        إذا (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        أرجع current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(مجموعه_flags_errno) ? مجموعه_flags_errno : -ENOANO;
}

صحيح read_attr_fd(صحيح fd, بیعلامت *ret) {
        بنية stat st;

        أكد(fd >= 0);
        أكد(ret);

        إذا (fstat(fd, &st) < 0)
                أرجع -errno;

        إذا (!inode_type_can_chattr(st.st_mode))
                أرجع -ENOTTY;

        _cleanup_close_ صحيح fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_مسیر, &fd_close); /* drop O_PATH if it is set */
        إذا (fd < 0)
                أرجع fd;

        أرجع RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

صحيح read_attr_at(صحيح dir_fd, ثابت حرف *مسیر, بیعلامت *ret) {
        _cleanup_close_ صحيح fd_close = -EBADF;
        صحيح fd;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(ret);

        إذا (isempty(مسیر) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        وإلا {
                fd_close = xopenat(dir_fd, مسیر, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                إذا (fd_close < 0)
                        أرجع fd_close;

                fd = fd_close;
        }

        أرجع read_attr_fd(fd, ret);
}

صحيح read_fs_xattr_fd(صحيح fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        بنية fsxattr attrs;
        _cleanup_close_ صحيح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_مسیر, &fd_reopened);
        إذا (fd < 0)
                أرجع fd;

        إذا (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                أرجع -errno;

        إذا (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        إذا (ret_projid)
                *ret_projid = attrs.fsx_projid;

        أرجع 0;
}

صحيح مجموعه_proj_id(صحيح fd, uint32_t proj_id) {
        بنية fsxattr attrs;
        _cleanup_close_ صحيح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_مسیر, &fd_reopened);
        إذا (fd < 0)
                أرجع fd;

        إذا (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                أرجع -errno;

        بنية stat statbuf;
        إذا (fstat(fd, &statbuf) < 0)
                أرجع -errno;

        إذا (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_مجموعه(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                أرجع 0;

        attrs.fsx_projid = proj_id;
        إذا (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        أرجع RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ساكن صحيح مجموعه_proj_id_cb(
                RecurseDirEvent event,
                ثابت حرف *مسیر,
                صحيح dir_fd,
                صحيح inode_fd,
                ثابت بنية dirent *de,
                ثابت بنية statx *sx,
                فراغ *userdata) {

        إذا (!درون_مجموعه(event, RECURSE_DIR_ENTER, RECURSE_DIR_مدخل))
                أرجع RECURSE_DIR_CONTINUE;

        إذا (de && !درون_مجموعه(de->d_type, DT_DIR, DT_REG))
                أرجع RECURSE_DIR_CONTINUE;

        أرجع مجموعه_proj_id(inode_fd, PTR_مقصد_UINT32(userdata));
}

صحيح مجموعه_proj_id_recursive(صحيح fd, uint32_t proj_id) {
        أرجع recurse_dir_at(
                        fd,
                        /* path= */ عدم,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        مجموعه_proj_id_cb,
                        UINT32_مقصد_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t نما) {
        أرجع درون_مجموعه(نما & S_IFMT, S_IFREG, S_IFDIR);
}
