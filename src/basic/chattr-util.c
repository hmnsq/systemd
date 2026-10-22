/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ "bitfield.h"
#અન્તર્ભાવ "chattr-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "recurse-dir.h"
#અન્તર્ભાવ "string-util.h"

પૂર્ણાંક chattr_full(
              પૂર્ણાંક dir_fd,
              અચલ અક્ષર *path,
              unsigned કિંમત,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        unsigned old_attr, new_attr;
        પૂર્ણાંક set_flags_errno = 0;
        સંરચના stat st;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        જો (fd < 0)
                પાછા fd;

        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        જો (!inode_type_can_chattr(st.st_mode))
                પાછા -ENOTTY;

        જો (mask == 0 && !ret_previous && !ret_final)
                પાછા 0;

        જો (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                પાછા -errno;

        new_attr = (old_attr & ~mask) | (કિંમત & mask);
        જો (new_attr == old_attr) {
                જો (ret_previous)
                        *ret_previous = old_attr;
                જો (ret_final)
                        *ret_final = old_attr;
                પાછા 0;
        }

        જો (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                જો (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        પાછા -errno;

                જો (new_attr == attr) {
                        જો (ret_previous)
                                *ret_previous = old_attr;
                        જો (ret_final)
                                *ret_final = new_attr;
                        પાછા 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        જો (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                પાછા -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(કિંમત, mask_one));
                જો (new_one == current_attr)
                        ચાલુ;

                જો (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        જો (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                પાછા -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        જો (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        ચાલુ;
                }

                જો (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        પાછા -errno;
        }

        જો (ret_previous)
                *ret_previous = old_attr;
        જો (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        પાછા current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

પૂર્ણાંક read_attr_fd(પૂર્ણાંક fd, unsigned *ret) {
        સંરચના stat st;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        જો (!inode_type_can_chattr(st.st_mode))
                પાછા -ENOTTY;

        _cleanup_close_ પૂર્ણાંક fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        જો (fd < 0)
                પાછા fd;

        પાછા RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

પૂર્ણાંક read_attr_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, unsigned *ret) {
        _cleanup_close_ પૂર્ણાંક fd_close = -EBADF;
        પૂર્ણાંક fd;

        પ્રતિજ્ઞા(dir_fd >= 0 || dir_fd == AT_FDCWD);
        પ્રતિજ્ઞા(ret);

        જો (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        નહીંતર {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                જો (fd_close < 0)
                        પાછા fd_close;

                fd = fd_close;
        }

        પાછા read_attr_fd(fd, ret);
}

પૂર્ણાંક read_fs_xattr_fd(પૂર્ણાંક fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        સંરચના fsxattr attrs;
        _cleanup_close_ પૂર્ણાંક fd_reopened = -EBADF;

        પ્રતિજ્ઞા(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        જો (fd < 0)
                પાછા fd;

        જો (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                પાછા -errno;

        જો (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        જો (ret_projid)
                *ret_projid = attrs.fsx_projid;

        પાછા 0;
}

પૂર્ણાંક set_proj_id(પૂર્ણાંક fd, uint32_t proj_id) {
        સંરચના fsxattr attrs;
        _cleanup_close_ પૂર્ણાંક fd_reopened = -EBADF;

        પ્રતિજ્ઞા(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        જો (fd < 0)
                પાછા fd;

        જો (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                પાછા -errno;

        સંરચના stat statbuf;
        જો (fstat(fd, &statbuf) < 0)
                પાછા -errno;

        જો (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                પાછા 0;

        attrs.fsx_projid = proj_id;
        જો (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        પાછા RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

સ્થિર પૂર્ણાંક set_proj_id_cb(
                RecurseDirEvent event,
                અચલ અક્ષર *path,
                પૂર્ણાંક dir_fd,
                પૂર્ણાંક inode_fd,
                અચલ સંરચના dirent *de,
                અચલ સંરચના statx *sx,
                શૂન્ય *userdata) {

        જો (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                પાછા RECURSE_DIR_CONTINUE;

        જો (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                પાછા RECURSE_DIR_CONTINUE;

        પાછા set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

પૂર્ણાંક set_proj_id_recursive(પૂર્ણાંક fd, uint32_t proj_id) {
        પાછા recurse_dir_at(
                        fd,
                        /* path= */ ખાલી,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        પાછા IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
