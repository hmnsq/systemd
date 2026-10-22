/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <sys/ioctl.h>
#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ "bitfield.h"
#অন্তর্ভাৱ "chattr-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "recurse-dir.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক chattr_full(
              পূর্ণাঙ্ক dir_fd,
              ধ্রুৱ ৱর্ণ *path,
              unsigned মান,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        unsigned old_attr, new_attr;
        পূর্ণাঙ্ক set_flags_errno = 0;
        সংরচনা stat st;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        যদি (fd < 0)
                ঘূৰাই fd;

        যদি (fstat(fd, &st) < 0)
                ঘূৰাই -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        যদি (!inode_type_can_chattr(st.st_mode))
                ঘূৰাই -ENOTTY;

        যদি (mask == 0 && !ret_previous && !ret_final)
                ঘূৰাই 0;

        যদি (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ঘূৰাই -errno;

        new_attr = (old_attr & ~mask) | (মান & mask);
        যদি (new_attr == old_attr) {
                যদি (ret_previous)
                        *ret_previous = old_attr;
                যদি (ret_final)
                        *ret_final = old_attr;
                ঘূৰাই 0;
        }

        যদি (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                যদি (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ঘূৰাই -errno;

                যদি (new_attr == attr) {
                        যদি (ret_previous)
                                *ret_previous = old_attr;
                        যদি (ret_final)
                                *ret_final = new_attr;
                        ঘূৰাই 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        যদি (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ঘূৰাই -errno;

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
                        অনুৱর্ত;

                যদি (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        যদি (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ঘূৰাই -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        যদি (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        অনুৱর্ত;
                }

                যদি (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ঘূৰাই -errno;
        }

        যদি (ret_previous)
                *ret_previous = old_attr;
        যদি (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ঘূৰাই current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

পূর্ণাঙ্ক read_attr_fd(পূর্ণাঙ্ক fd, unsigned *ret) {
        সংরচনা stat st;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (fstat(fd, &st) < 0)
                ঘূৰাই -errno;

        যদি (!inode_type_can_chattr(st.st_mode))
                ঘূৰাই -ENOTTY;

        _cleanup_close_ পূর্ণাঙ্ক fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        যদি (fd < 0)
                ঘূৰাই fd;

        ঘূৰাই RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

পূর্ণাঙ্ক read_attr_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, unsigned *ret) {
        _cleanup_close_ পূর্ণাঙ্ক fd_close = -EBADF;
        পূর্ণাঙ্ক fd;

        প্রতিজ্ঞা(dir_fd >= 0 || dir_fd == AT_FDCWD);
        প্রতিজ্ঞা(ret);

        যদি (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        অন্যথা {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                যদি (fd_close < 0)
                        ঘূৰাই fd_close;

                fd = fd_close;
        }

        ঘূৰাই read_attr_fd(fd, ret);
}

পূর্ণাঙ্ক read_fs_xattr_fd(পূর্ণাঙ্ক fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        সংরচনা fsxattr attrs;
        _cleanup_close_ পূর্ণাঙ্ক fd_reopened = -EBADF;

        প্রতিজ্ঞা(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        যদি (fd < 0)
                ঘূৰাই fd;

        যদি (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ঘূৰাই -errno;

        যদি (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        যদি (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক set_proj_id(পূর্ণাঙ্ক fd, uint32_t proj_id) {
        সংরচনা fsxattr attrs;
        _cleanup_close_ পূর্ণাঙ্ক fd_reopened = -EBADF;

        প্রতিজ্ঞা(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        যদি (fd < 0)
                ঘূৰাই fd;

        যদি (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ঘূৰাই -errno;

        সংরচনা stat statbuf;
        যদি (fstat(fd, &statbuf) < 0)
                ঘূৰাই -errno;

        যদি (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ঘূৰাই 0;

        attrs.fsx_projid = proj_id;
        যদি (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ঘূৰাই RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

স্থির পূর্ণাঙ্ক set_proj_id_cb(
                RecurseDirEvent event,
                ধ্রুৱ ৱর্ণ *path,
                পূর্ণাঙ্ক dir_fd,
                পূর্ণাঙ্ক inode_fd,
                ধ্রুৱ সংরচনা dirent *de,
                ধ্রুৱ সংরচনা statx *sx,
                শূন্য *userdata) {

        যদি (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ঘূৰাই RECURSE_DIR_CONTINUE;

        যদি (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ঘূৰাই RECURSE_DIR_CONTINUE;

        ঘূৰাই set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

পূর্ণাঙ্ক set_proj_id_recursive(পূর্ণাঙ্ক fd, uint32_t proj_id) {
        ঘূৰাই recurse_dir_at(
                        fd,
                        /* path= */ রিক্ত,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ঘূৰাই IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
