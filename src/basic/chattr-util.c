/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ "bitfield.h"
#அந்தர்பாவ "chattr-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "recurse-dir.h"
#அந்தர்பாவ "string-util.h"

முழு chattr_full(
              முழு dir_fd,
              மாறிலி எழுத்து *path,
              unsigned மதிப்பு,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ முழு fd = -EBADF;
        unsigned old_attr, new_attr;
        முழு set_flags_errno = 0;
        கட்டமைப்பு stat st;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        எனில் (fd < 0)
                திரும்பு fd;

        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        எனில் (!inode_type_can_chattr(st.st_mode))
                திரும்பு -ENOTTY;

        எனில் (mask == 0 && !ret_previous && !ret_final)
                திரும்பு 0;

        எனில் (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                திரும்பு -errno;

        new_attr = (old_attr & ~mask) | (மதிப்பு & mask);
        எனில் (new_attr == old_attr) {
                எனில் (ret_previous)
                        *ret_previous = old_attr;
                எனில் (ret_final)
                        *ret_final = old_attr;
                திரும்பு 0;
        }

        எனில் (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                எனில் (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        திரும்பு -errno;

                எனில் (new_attr == attr) {
                        எனில் (ret_previous)
                                *ret_previous = old_attr;
                        எனில் (ret_final)
                                *ret_final = new_attr;
                        திரும்பு 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        எனில் (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                திரும்பு -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(மதிப்பு, mask_one));
                எனில் (new_one == current_attr)
                        தொடர்;

                எனில் (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        எனில் (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                திரும்பு -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        எனில் (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        தொடர்;
                }

                எனில் (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        திரும்பு -errno;
        }

        எனில் (ret_previous)
                *ret_previous = old_attr;
        எனில் (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        திரும்பு current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

முழு read_attr_fd(முழு fd, unsigned *ret) {
        கட்டமைப்பு stat st;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        எனில் (!inode_type_can_chattr(st.st_mode))
                திரும்பு -ENOTTY;

        _cleanup_close_ முழு fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        எனில் (fd < 0)
                திரும்பு fd;

        திரும்பு RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

முழு read_attr_at(முழு dir_fd, மாறிலி எழுத்து *path, unsigned *ret) {
        _cleanup_close_ முழு fd_close = -EBADF;
        முழு fd;

        ப்ரதிஜ்ஞா(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ப்ரதிஜ்ஞா(ret);

        எனில் (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        இல்லையெனில் {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                எனில் (fd_close < 0)
                        திரும்பு fd_close;

                fd = fd_close;
        }

        திரும்பு read_attr_fd(fd, ret);
}

முழு read_fs_xattr_fd(முழு fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        கட்டமைப்பு fsxattr attrs;
        _cleanup_close_ முழு fd_reopened = -EBADF;

        ப்ரதிஜ்ஞா(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        எனில் (fd < 0)
                திரும்பு fd;

        எனில் (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                திரும்பு -errno;

        எனில் (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        எனில் (ret_projid)
                *ret_projid = attrs.fsx_projid;

        திரும்பு 0;
}

முழு set_proj_id(முழு fd, uint32_t proj_id) {
        கட்டமைப்பு fsxattr attrs;
        _cleanup_close_ முழு fd_reopened = -EBADF;

        ப்ரதிஜ்ஞா(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        எனில் (fd < 0)
                திரும்பு fd;

        எனில் (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                திரும்பு -errno;

        கட்டமைப்பு stat statbuf;
        எனில் (fstat(fd, &statbuf) < 0)
                திரும்பு -errno;

        எனில் (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                திரும்பு 0;

        attrs.fsx_projid = proj_id;
        எனில் (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        திரும்பு RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

நிலையான முழு set_proj_id_cb(
                RecurseDirEvent event,
                மாறிலி எழுத்து *path,
                முழு dir_fd,
                முழு inode_fd,
                மாறிலி கட்டமைப்பு dirent *de,
                மாறிலி கட்டமைப்பு statx *sx,
                வெறுமை *userdata) {

        எனில் (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                திரும்பு RECURSE_DIR_CONTINUE;

        எனில் (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                திரும்பு RECURSE_DIR_CONTINUE;

        திரும்பு set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

முழு set_proj_id_recursive(முழு fd, uint32_t proj_id) {
        திரும்பு recurse_dir_at(
                        fd,
                        /* path= */ வெற்று,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        திரும்பு IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
