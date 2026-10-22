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
        सचे (fd < 0)
                पटिनिवत्त fd;

        सचे (fstat(fd, &st) < 0)
                पटिनिवत्त -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        सचे (!inode_type_can_chattr(st.st_mode))
                पटिनिवत्त -ENOTTY;

        सचे (mask == 0 && !ret_previous && !ret_final)
                पटिनिवत्त 0;

        सचे (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                पटिनिवत्त -errno;

        new_attr = (old_attr & ~mask) | (मानम् & mask);
        सचे (new_attr == old_attr) {
                सचे (ret_previous)
                        *ret_previous = old_attr;
                सचे (ret_final)
                        *ret_final = old_attr;
                पटिनिवत्त 0;
        }

        सचे (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                सचे (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        पटिनिवत्त -errno;

                सचे (new_attr == attr) {
                        सचे (ret_previous)
                                *ret_previous = old_attr;
                        सचे (ret_final)
                                *ret_final = new_attr;
                        पटिनिवत्त 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        सचे (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                पटिनिवत्त -errno;

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
                सचे (new_one == current_attr)
                        अनुवर्त;

                सचे (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        सचे (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                पटिनिवत्त -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        सचे (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        अनुवर्त;
                }

                सचे (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        पटिनिवत्त -errno;
        }

        सचे (ret_previous)
                *ret_previous = old_attr;
        सचे (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        पटिनिवत्त current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

पूर्णाङ्क read_attr_fd(पूर्णाङ्क fd, unsigned *ret) {
        संरचना stat st;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        सचे (fstat(fd, &st) < 0)
                पटिनिवत्त -errno;

        सचे (!inode_type_can_chattr(st.st_mode))
                पटिनिवत्त -ENOTTY;

        _cleanup_close_ पूर्णाङ्क fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        सचे (fd < 0)
                पटिनिवत्त fd;

        पटिनिवत्त RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

पूर्णाङ्क read_attr_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, unsigned *ret) {
        _cleanup_close_ पूर्णाङ्क fd_close = -EBADF;
        पूर्णाङ्क fd;

        प्रतिज्ञा(dir_fd >= 0 || dir_fd == AT_FDCWD);
        प्रतिज्ञा(ret);

        सचे (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        अञ्ञथा {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                सचे (fd_close < 0)
                        पटिनिवत्त fd_close;

                fd = fd_close;
        }

        पटिनिवत्त read_attr_fd(fd, ret);
}

पूर्णाङ्क read_fs_xattr_fd(पूर्णाङ्क fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        संरचना fsxattr attrs;
        _cleanup_close_ पूर्णाङ्क fd_reopened = -EBADF;

        प्रतिज्ञा(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        सचे (fd < 0)
                पटिनिवत्त fd;

        सचे (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                पटिनिवत्त -errno;

        सचे (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        सचे (ret_projid)
                *ret_projid = attrs.fsx_projid;

        पटिनिवत्त 0;
}

पूर्णाङ्क set_proj_id(पूर्णाङ्क fd, uint32_t proj_id) {
        संरचना fsxattr attrs;
        _cleanup_close_ पूर्णाङ्क fd_reopened = -EBADF;

        प्रतिज्ञा(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        सचे (fd < 0)
                पटिनिवत्त fd;

        सचे (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                पटिनिवत्त -errno;

        संरचना stat statbuf;
        सचे (fstat(fd, &statbuf) < 0)
                पटिनिवत्त -errno;

        सचे (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                पटिनिवत्त 0;

        attrs.fsx_projid = proj_id;
        सचे (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        पटिनिवत्त RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

स्थिर पूर्णाङ्क set_proj_id_cb(
                RecurseDirEvent event,
                ध्रुव वर्ण *path,
                पूर्णाङ्क dir_fd,
                पूर्णाङ्क inode_fd,
                ध्रुव संरचना dirent *de,
                ध्रुव संरचना statx *sx,
                शून्य *userdata) {

        सचे (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                पटिनिवत्त RECURSE_DIR_CONTINUE;

        सचे (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                पटिनिवत्त RECURSE_DIR_CONTINUE;

        पटिनिवत्त set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

पूर्णाङ्क set_proj_id_recursive(पूर्णाङ्क fd, uint32_t proj_id) {
        पटिनिवत्त recurse_dir_at(
                        fd,
                        /* path= */ रिक्त,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        पटिनिवत्त IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
