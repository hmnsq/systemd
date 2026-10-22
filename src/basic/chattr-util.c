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

ಪೂರ್ಣಾಂಕ chattr_ತುಂಬಿದ(
              ಪೂರ್ಣಾಂಕ dir_fd,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
              ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ,
              ಚಿಹ್ನೆರಹಿತ ಮುಖವಾಡ_3,
              ಚಿಹ್ನೆರಹಿತ *ret_ಹಿಂದಿನ,
              ಚಿಹ್ನೆರಹಿತ *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಚಿಹ್ನೆರಹಿತ old_attr, new_attr;
        ಪೂರ್ಣಾಂಕ ಗಣ_flags_errno = 0;
        ರಚನೆ stat st;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, ಮಾರ್ಗ, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ಆದರೆ (!inode_type_can_chattr(st.st_mode))
                ಹಿಂತಿರುಗಿ -ENOTTY;

        ಆದರೆ (ಮುಖವಾಡ_3 == 0 && !ret_ಹಿಂದಿನ && !ret_final)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        new_attr = (old_attr & ~ಮುಖವಾಡ_3) | (ಮೌಲ್ಯ & ಮುಖವಾಡ_3);
        ಆದರೆ (new_attr == old_attr) {
                ಆದರೆ (ret_ಹಿಂದಿನ)
                        *ret_ಹಿಂದಿನ = old_attr;
                ಆದರೆ (ret_final)
                        *ret_final = old_attr;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                ಚಿಹ್ನೆರಹಿತ attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ಆದರೆ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (new_attr == attr) {
                        ಆದರೆ (ret_ಹಿಂದಿನ)
                                *ret_ಹಿಂದಿನ = old_attr;
                        ಆದರೆ (ret_final)
                                *ret_final = new_attr;
                        ಹಿಂತಿರುಗಿ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ಆದರೆ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_ಗಣ(flags, CHATTR_FALLBACK_BITWISE))
                ಹಿಂತಿರುಗಿ -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        ಚಿಹ್ನೆರಹಿತ current_attr = old_attr;

        ಬಿಟ್_FOREACH(i, ಮುಖವಾಡ_3) {
                ಚಿಹ್ನೆರಹಿತ new_one, ಮುಖವಾಡ_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, ಮುಖವಾಡ_one, FLAGS_ಗಣ(ಮೌಲ್ಯ, ಮುಖವಾಡ_one));
                ಆದರೆ (new_one == current_attr)
                        ಮುಂದುವರಿ;

                ಆದರೆ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ಆದರೆ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ಹಿಂತಿರುಗಿ -errno;

                        log_ತುಂಬಿದ_errno(FLAGS_ಗಣ(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", ಮುಖವಾಡ_one, strna(ಮಾರ್ಗ));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ಆದರೆ (ಗಣ_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ಗಣ_flags_errno = -errno;

                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಆದರೆ (ret_ಹಿಂದಿನ)
                *ret_ಹಿಂದಿನ = old_attr;
        ಆದರೆ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ಹಿಂತಿರುಗಿ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(ಗಣ_flags_errno) ? ಗಣ_flags_errno : -ENOANO;
}

ಪೂರ್ಣಾಂಕ read_attr_fd(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ *ret) {
        ರಚನೆ stat st;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (!inode_type_can_chattr(st.st_mode))
                ಹಿಂತಿರುಗಿ -ENOTTY;

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_ಮಾರ್ಗ, &fd_close); /* drop O_PATH if it is set */
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

ಪೂರ್ಣಾಂಕ read_attr_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಚಿಹ್ನೆರಹಿತ *ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_close = -EBADF;
        ಪೂರ್ಣಾಂಕ fd;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (isempty(ಮಾರ್ಗ) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ಇಲ್ಲದಿದ್ದರೆ {
                fd_close = xopenat(dir_fd, ಮಾರ್ಗ, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ಆದರೆ (fd_close < 0)
                        ಹಿಂತಿರುಗಿ fd_close;

                fd = fd_close;
        }

        ಹಿಂತಿರುಗಿ read_attr_fd(fd, ret);
}

ಪೂರ್ಣಾಂಕ read_fs_xattr_fd(ಪೂರ್ಣಾಂಕ fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ರಚನೆ fsxattr attrs;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_reopened = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_ಮಾರ್ಗ, &fd_reopened);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ಆದರೆ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಗಣ_proj_id(ಪೂರ್ಣಾಂಕ fd, uint32_t proj_id) {
        ರಚನೆ fsxattr attrs;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd_reopened = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_ಮಾರ್ಗ, &fd_reopened);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ರಚನೆ stat statbuf;
        ಆದರೆ (fstat(fd, &statbuf) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_ಗಣ(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ಹಿಂತಿರುಗಿ 0;

        attrs.fsx_projid = proj_id;
        ಆದರೆ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಗಣ_proj_id_cb(
                RecurseDirEvent event,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಪೂರ್ಣಾಂಕ inode_fd,
                ಸ್ಥಿರಾಂಕ ರಚನೆ dirent *de,
                ಸ್ಥಿರಾಂಕ ರಚನೆ statx *sx,
                ಶೂನ್ಯ *userdata) {

        ಆದರೆ (!ಒಳ_ಗಣ(event, RECURSE_DIR_ENTER, RECURSE_DIR_ನಮೂದು))
                ಹಿಂತಿರುಗಿ RECURSE_DIR_CONTINUE;

        ಆದರೆ (de && !ಒಳ_ಗಣ(de->d_type, DT_DIR, DT_REG))
                ಹಿಂತಿರುಗಿ RECURSE_DIR_CONTINUE;

        ಹಿಂತಿರುಗಿ ಗಣ_proj_id(inode_fd, PTR_ಗಮ್ಯ_UINT32(userdata));
}

ಪೂರ್ಣಾಂಕ ಗಣ_proj_id_recursive(ಪೂರ್ಣಾಂಕ fd, uint32_t proj_id) {
        ಹಿಂತಿರುಗಿ recurse_dir_at(
                        fd,
                        /* path= */ ಖಾಲಿ,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        ಗಣ_proj_id_cb,
                        UINT32_ಗಮ್ಯ_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(ಬಹುಲಕ & S_IFMT, S_IFREG, S_IFDIR);
}
