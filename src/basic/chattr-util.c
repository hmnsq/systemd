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

صحیح chattr_بھرا(
              صحیح dir_fd,
              ثابت حرف *راستہ,
              غیرعلامتی قدر,
              غیرعلامتی نقاب_3,
              غیرعلامتی *ret_پچھلا,
              غیرعلامتی *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ صحیح fd = -EBADF;
        غیرعلامتی old_attr, new_attr;
        صحیح سیٹ_flags_errno = 0;
        ساخت stat st;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, راستہ, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        اگر (fd < 0)
                واپس fd;

        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        اگر (!inode_type_can_chattr(st.st_mode))
                واپس -ENOTTY;

        اگر (نقاب_3 == 0 && !ret_پچھلا && !ret_final)
                واپس 0;

        اگر (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                واپس -errno;

        new_attr = (old_attr & ~نقاب_3) | (قدر & نقاب_3);
        اگر (new_attr == old_attr) {
                اگر (ret_پچھلا)
                        *ret_پچھلا = old_attr;
                اگر (ret_final)
                        *ret_final = old_attr;
                واپس 0;
        }

        اگر (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                غیرعلامتی attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                اگر (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        واپس -errno;

                اگر (new_attr == attr) {
                        اگر (ret_پچھلا)
                                *ret_پچھلا = old_attr;
                        اگر (ret_final)
                                *ret_final = new_attr;
                        واپس 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        اگر (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_سیٹ(flags, CHATTR_FALLBACK_BITWISE))
                واپس -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        غیرعلامتی current_attr = old_attr;

        ثنائی_ہندسہ_FOREACH(i, نقاب_3) {
                غیرعلامتی new_one, نقاب_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, نقاب_one, FLAGS_سیٹ(قدر, نقاب_one));
                اگر (new_one == current_attr)
                        جاری;

                اگر (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        اگر (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                واپس -errno;

                        log_بھرا_errno(FLAGS_سیٹ(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", نقاب_one, strna(راستہ));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        اگر (سیٹ_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                سیٹ_flags_errno = -errno;

                        جاری;
                }

                اگر (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        واپس -errno;
        }

        اگر (ret_پچھلا)
                *ret_پچھلا = old_attr;
        اگر (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        واپس current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(سیٹ_flags_errno) ? سیٹ_flags_errno : -ENOANO;
}

صحیح read_attr_fd(صحیح fd, غیرعلامتی *ret) {
        ساخت stat st;

        أكد(fd >= 0);
        أكد(ret);

        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        اگر (!inode_type_can_chattr(st.st_mode))
                واپس -ENOTTY;

        _cleanup_close_ صحیح fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_راستہ, &fd_close); /* drop O_PATH if it is set */
        اگر (fd < 0)
                واپس fd;

        واپس RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

صحیح read_attr_at(صحیح dir_fd, ثابت حرف *راستہ, غیرعلامتی *ret) {
        _cleanup_close_ صحیح fd_close = -EBADF;
        صحیح fd;

        أكد(dir_fd >= 0 || dir_fd == AT_FDCWD);
        أكد(ret);

        اگر (isempty(راستہ) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ورنہ {
                fd_close = xopenat(dir_fd, راستہ, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                اگر (fd_close < 0)
                        واپس fd_close;

                fd = fd_close;
        }

        واپس read_attr_fd(fd, ret);
}

صحیح read_fs_xattr_fd(صحیح fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ساخت fsxattr attrs;
        _cleanup_close_ صحیح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_راستہ, &fd_reopened);
        اگر (fd < 0)
                واپس fd;

        اگر (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                واپس -errno;

        اگر (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        اگر (ret_projid)
                *ret_projid = attrs.fsx_projid;

        واپس 0;
}

صحیح سیٹ_proj_id(صحیح fd, uint32_t proj_id) {
        ساخت fsxattr attrs;
        _cleanup_close_ صحیح fd_reopened = -EBADF;

        أكد(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_راستہ, &fd_reopened);
        اگر (fd < 0)
                واپس fd;

        اگر (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                واپس -errno;

        ساخت stat statbuf;
        اگر (fstat(fd, &statbuf) < 0)
                واپس -errno;

        اگر (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_سیٹ(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                واپس 0;

        attrs.fsx_projid = proj_id;
        اگر (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        واپس RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ساکن صحیح سیٹ_proj_id_cb(
                RecurseDirEvent event,
                ثابت حرف *راستہ,
                صحیح dir_fd,
                صحیح inode_fd,
                ثابت ساخت dirent *de,
                ثابت ساخت statx *sx,
                خالی *userdata) {

        اگر (!اندر_سیٹ(event, RECURSE_DIR_ENTER, RECURSE_DIR_اندراج))
                واپس RECURSE_DIR_CONTINUE;

        اگر (de && !اندر_سیٹ(de->d_type, DT_DIR, DT_REG))
                واپس RECURSE_DIR_CONTINUE;

        واپس سیٹ_proj_id(inode_fd, PTR_مقصد_UINT32(userdata));
}

صحیح سیٹ_proj_id_recursive(صحیح fd, uint32_t proj_id) {
        واپس recurse_dir_at(
                        fd,
                        /* path= */ تہی,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        سیٹ_proj_id_cb,
                        UINT32_مقصد_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t منوال) {
        واپس اندر_سیٹ(منوال & S_IFMT, S_IFREG, S_IFDIR);
}
