/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "bitfield.h"
#ಅನ್ತರ್ಭಾವ "chattr-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "recurse-dir.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಙ್ಕ chattr_full(
              ಪೂರ್ಣಾಙ್ಕ dir_fd,
              ಧ್ರುವ ವರ್ಣ *path,
              unsigned ಮೌಲ್ಯ,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        unsigned old_attr, new_attr;
        ಪೂರ್ಣಾಙ್ಕ set_flags_errno = 0;
        ಸಂರಚನಾ stat st;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ಯದಿ (!inode_type_can_chattr(st.st_mode))
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        ಯದಿ (mask == 0 && !ret_previous && !ret_final)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        new_attr = (old_attr & ~mask) | (ಮೌಲ್ಯ & mask);
        ಯದಿ (new_attr == old_attr) {
                ಯದಿ (ret_previous)
                        *ret_previous = old_attr;
                ಯದಿ (ret_final)
                        *ret_final = old_attr;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ಯದಿ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (new_attr == attr) {
                        ಯದಿ (ret_previous)
                                *ret_previous = old_attr;
                        ಯದಿ (ret_final)
                                *ret_final = new_attr;
                        ಪ್ರತ್ಯಾಗಮ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ಯದಿ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(ಮೌಲ್ಯ, mask_one));
                ಯದಿ (new_one == current_attr)
                        ಅನುವರ್ತ;

                ಯದಿ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ಯದಿ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ಯದಿ (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        ಅನುವರ್ತ;
                }

                ಯದಿ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಯದಿ (ret_previous)
                *ret_previous = old_attr;
        ಯದಿ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ಪ್ರತ್ಯಾಗಮ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

ಪೂರ್ಣಾಙ್ಕ read_attr_fd(ಪೂರ್ಣಾಙ್ಕ fd, unsigned *ret) {
        ಸಂರಚನಾ stat st;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (!inode_type_can_chattr(st.st_mode))
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

ಪೂರ್ಣಾಙ್ಕ read_attr_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, unsigned *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_close = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ fd;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ಅನ್ಯಥಾ {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ಯದಿ (fd_close < 0)
                        ಪ್ರತ್ಯಾಗಮ fd_close;

                fd = fd_close;
        }

        ಪ್ರತ್ಯಾಗಮ read_attr_fd(fd, ret);
}

ಪೂರ್ಣಾಙ್ಕ read_fs_xattr_fd(ಪೂರ್ಣಾಙ್ಕ fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ಸಂರಚನಾ fsxattr attrs;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_reopened = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ಯದಿ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ set_proj_id(ಪೂರ್ಣಾಙ್ಕ fd, uint32_t proj_id) {
        ಸಂರಚನಾ fsxattr attrs;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd_reopened = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಸಂರಚನಾ stat statbuf;
        ಯದಿ (fstat(fd, &statbuf) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ಪ್ರತ್ಯಾಗಮ 0;

        attrs.fsx_projid = proj_id;
        ಯದಿ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ set_proj_id_cb(
                RecurseDirEvent event,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಪೂರ್ಣಾಙ್ಕ inode_fd,
                ಧ್ರುವ ಸಂರಚನಾ dirent *de,
                ಧ್ರುವ ಸಂರಚನಾ statx *sx,
                ಶೂನ್ಯ *userdata) {

        ಯದಿ (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ಪ್ರತ್ಯಾಗಮ RECURSE_DIR_CONTINUE;

        ಯದಿ (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ಪ್ರತ್ಯಾಗಮ RECURSE_DIR_CONTINUE;

        ಪ್ರತ್ಯಾಗಮ set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

ಪೂರ್ಣಾಙ್ಕ set_proj_id_recursive(ಪೂರ್ಣಾಙ್ಕ fd, uint32_t proj_id) {
        ಪ್ರತ್ಯಾಗಮ recurse_dir_at(
                        fd,
                        /* path= */ ರಿಕ್ತ,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
