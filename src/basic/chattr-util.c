/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "bitfield.h"
#অন্তর্ভাব "chattr-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "recurse-dir.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা chattr_full(
              পূর্ণসংখ্যা dir_fd,
              ধ্রুবক অক্ষর *path,
              unsigned মান,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        unsigned old_attr, new_attr;
        পূর্ণসংখ্যা set_flags_errno = 0;
        কাঠামো stat st;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        যদি (fd < 0)
                ফেরত fd;

        যদি (fstat(fd, &st) < 0)
                ফেরত -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        যদি (!inode_type_can_chattr(st.st_mode))
                ফেরত -ENOTTY;

        যদি (mask == 0 && !ret_previous && !ret_final)
                ফেরত 0;

        যদি (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ফেরত -errno;

        new_attr = (old_attr & ~mask) | (মান & mask);
        যদি (new_attr == old_attr) {
                যদি (ret_previous)
                        *ret_previous = old_attr;
                যদি (ret_final)
                        *ret_final = old_attr;
                ফেরত 0;
        }

        যদি (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                যদি (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ফেরত -errno;

                যদি (new_attr == attr) {
                        যদি (ret_previous)
                                *ret_previous = old_attr;
                        যদি (ret_final)
                                *ret_final = new_attr;
                        ফেরত 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        যদি (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ফেরত -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(মান, mask_one));
                যদি (new_one == current_attr)
                        চালু;

                যদি (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        যদি (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ফেরত -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        যদি (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        চালু;
                }

                যদি (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ফেরত -errno;
        }

        যদি (ret_previous)
                *ret_previous = old_attr;
        যদি (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ফেরত current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

পূর্ণসংখ্যা read_attr_fd(পূর্ণসংখ্যা fd, unsigned *ret) {
        কাঠামো stat st;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (fstat(fd, &st) < 0)
                ফেরত -errno;

        যদি (!inode_type_can_chattr(st.st_mode))
                ফেরত -ENOTTY;

        _cleanup_close_ পূর্ণসংখ্যা fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        যদি (fd < 0)
                ফেরত fd;

        ফেরত RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

পূর্ণসংখ্যা read_attr_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, unsigned *ret) {
        _cleanup_close_ পূর্ণসংখ্যা fd_close = -EBADF;
        পূর্ণসংখ্যা fd;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(ret);

        যদি (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        অন্যথা {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                যদি (fd_close < 0)
                        ফেরত fd_close;

                fd = fd_close;
        }

        ফেরত read_attr_fd(fd, ret);
}

পূর্ণসংখ্যা read_fs_xattr_fd(পূর্ণসংখ্যা fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        কাঠামো fsxattr attrs;
        _cleanup_close_ পূর্ণসংখ্যা fd_reopened = -EBADF;

        প্রতিজ্ঞা(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        যদি (fd < 0)
                ফেরত fd;

        যদি (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ফেরত -errno;

        যদি (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        যদি (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ফেরত 0;
}

পূর্ণসংখ্যা set_proj_id(পূর্ণসংখ্যা fd, uint32_t proj_id) {
        কাঠামো fsxattr attrs;
        _cleanup_close_ পূর্ণসংখ্যা fd_reopened = -EBADF;

        প্রতিজ্ঞা(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        যদি (fd < 0)
                ফেরত fd;

        যদি (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ফেরত -errno;

        কাঠামো stat statbuf;
        যদি (fstat(fd, &statbuf) < 0)
                ফেরত -errno;

        যদি (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ফেরত 0;

        attrs.fsx_projid = proj_id;
        যদি (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ফেরত RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

স্থির পূর্ণসংখ্যা set_proj_id_cb(
                RecurseDirEvent event,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা dir_fd,
                পূর্ণসংখ্যা inode_fd,
                ধ্রুবক কাঠামো dirent *de,
                ধ্রুবক কাঠামো statx *sx,
                শূন্য *userdata) {

        যদি (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ফেরত RECURSE_DIR_CONTINUE;

        যদি (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ফেরত RECURSE_DIR_CONTINUE;

        ফেরত set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

পূর্ণসংখ্যা set_proj_id_recursive(পূর্ণসংখ্যা fd, uint32_t proj_id) {
        ফেরত recurse_dir_at(
                        fd,
                        /* path= */ ফাঁকা,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ফেরত IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
