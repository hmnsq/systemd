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

पूर्णाङ्क chattr_full(
              पूर्णाङ्क dir_fd,
              ध्रुव वर्ण *path,
              unsigned मानम्,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        unsigned old_attr, new_attr;
        पूर्णाङ्क set_flags_errno = 0;
        संरचना stat st;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        जइ (fd < 0)
                प्रत्यागम fd;

        जइ (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        जइ (!inode_type_can_chattr(st.st_mode))
                प्रत्यागम -ENOTTY;

        जइ (mask == 0 && !ret_previous && !ret_final)
                प्रत्यागम 0;

        जइ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                प्रत्यागम -errno;

        new_attr = (old_attr & ~mask) | (मानम् & mask);
        जइ (new_attr == old_attr) {
                जइ (ret_previous)
                        *ret_previous = old_attr;
                जइ (ret_final)
                        *ret_final = old_attr;
                प्रत्यागम 0;
        }

        जइ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                जइ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        प्रत्यागम -errno;

                जइ (new_attr == attr) {
                        जइ (ret_previous)
                                *ret_previous = old_attr;
                        जइ (ret_final)
                                *ret_final = new_attr;
                        प्रत्यागम 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        जइ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                प्रत्यागम -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(मानम्, mask_one));
                जइ (new_one == current_attr)
                        अनुवर्त;

                जइ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        जइ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                प्रत्यागम -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        जइ (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        अनुवर्त;
                }

                जइ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        प्रत्यागम -errno;
        }

        जइ (ret_previous)
                *ret_previous = old_attr;
        जइ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        प्रत्यागम current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

पूर्णाङ्क read_attr_fd(पूर्णाङ्क fd, unsigned *ret) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        जइ (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        जइ (!inode_type_can_chattr(st.st_mode))
                प्रत्यागम -ENOTTY;

        _cleanup_close_ पूर्णाङ्क fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        जइ (fd < 0)
                प्रत्यागम fd;

        प्रत्यागम RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

पूर्णाङ्क read_attr_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, unsigned *ret) {
        _cleanup_close_ पूर्णाङ्क fd_close = -EBADF;
        पूर्णाङ्क fd;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(ret);

        जइ (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        अण्णहा {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                जइ (fd_close < 0)
                        प्रत्यागम fd_close;

                fd = fd_close;
        }

        प्रत्यागम read_attr_fd(fd, ret);
}

पूर्णाङ्क read_fs_xattr_fd(पूर्णाङ्क fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        संरचना fsxattr attrs;
        _cleanup_close_ पूर्णाङ्क fd_reopened = -EBADF;

        प्रतिज्ञा(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        जइ (fd < 0)
                प्रत्यागम fd;

        जइ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                प्रत्यागम -errno;

        जइ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        जइ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        प्रत्यागम 0;
}

पूर्णाङ्क set_proj_id(पूर्णाङ्क fd, uint32_t proj_id) {
        संरचना fsxattr attrs;
        _cleanup_close_ पूर्णाङ्क fd_reopened = -EBADF;

        प्रतिज्ञा(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        जइ (fd < 0)
                प्रत्यागम fd;

        जइ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                प्रत्यागम -errno;

        संरचना stat statbuf;
        जइ (fstat(fd, &statbuf) < 0)
                प्रत्यागम -errno;

        जइ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                प्रत्यागम 0;

        attrs.fsx_projid = proj_id;
        जइ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        प्रत्यागम RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

स्थिर पूर्णाङ्क set_proj_id_cb(
                RecurseDirEvent event,
                ध्रुव वर्ण *path,
                पूर्णाङ्क dir_fd,
                पूर्णाङ्क inode_fd,
                ध्रुव संरचना dirent *de,
                ध्रुव संरचना statx *sx,
                शून्य *userdata) {

        जइ (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                प्रत्यागम RECURSE_DIR_CONTINUE;

        जइ (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                प्रत्यागम RECURSE_DIR_CONTINUE;

        प्रत्यागम set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

पूर्णाङ्क set_proj_id_recursive(पूर्णाङ्क fd, uint32_t proj_id) {
        प्रत्यागम recurse_dir_at(
                        fd,
                        /* path= */ रिक्त,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        प्रत्यागम IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
