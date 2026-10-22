/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include "bitfield.h"
#include "chattr-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "log.h"
#include "recurse-dir.h"
#include "string-util.h"

целое chattr_full(
              целое dir_fd,
              const char *path,
              unsigned value,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ целое fd = -EBADF;
        unsigned old_attr, new_attr;
        целое set_flags_errno = 0;
        struct stat st;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        если (fd < 0)
                вернуть fd;

        если (fstat(fd, &st) < 0)
                вернуть -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        если (!inode_type_can_chattr(st.st_mode))
                вернуть -ENOTTY;

        если (mask == 0 && !ret_previous && !ret_final)
                вернуть 0;

        если (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                вернуть -errno;

        new_attr = (old_attr & ~mask) | (value & mask);
        если (new_attr == old_attr) {
                если (ret_previous)
                        *ret_previous = old_attr;
                если (ret_final)
                        *ret_final = old_attr;
                вернуть 0;
        }

        если (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                если (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        вернуть -errno;

                если (new_attr == attr) {
                        если (ret_previous)
                                *ret_previous = old_attr;
                        если (ret_final)
                                *ret_final = new_attr;
                        вернуть 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        если (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                вернуть -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(value, mask_one));
                если (new_one == current_attr)
                        continue;

                если (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        если (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                вернуть -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        если (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        continue;
                }

                если (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        вернуть -errno;
        }

        если (ret_previous)
                *ret_previous = old_attr;
        если (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        вернуть current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

целое read_attr_fd(целое fd, unsigned *ret) {
        struct stat st;

        assert(fd >= 0);
        assert(ret);

        если (fstat(fd, &st) < 0)
                вернуть -errno;

        если (!inode_type_can_chattr(st.st_mode))
                вернуть -ENOTTY;

        _cleanup_close_ целое fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        если (fd < 0)
                вернуть fd;

        вернуть RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

целое read_attr_at(целое dir_fd, const char *path, unsigned *ret) {
        _cleanup_close_ целое fd_close = -EBADF;
        целое fd;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(ret);

        если (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        иначе {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                если (fd_close < 0)
                        вернуть fd_close;

                fd = fd_close;
        }

        вернуть read_attr_fd(fd, ret);
}

целое read_fs_xattr_fd(целое fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        struct fsxattr attrs;
        _cleanup_close_ целое fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        если (fd < 0)
                вернуть fd;

        если (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                вернуть -errno;

        если (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        если (ret_projid)
                *ret_projid = attrs.fsx_projid;

        вернуть 0;
}

целое set_proj_id(целое fd, uint32_t proj_id) {
        struct fsxattr attrs;
        _cleanup_close_ целое fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        если (fd < 0)
                вернуть fd;

        если (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                вернуть -errno;

        struct stat statbuf;
        если (fstat(fd, &statbuf) < 0)
                вернуть -errno;

        если (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                вернуть 0;

        attrs.fsx_projid = proj_id;
        если (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        вернуть RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

static целое set_proj_id_cb(
                RecurseDirEvent event,
                const char *path,
                целое dir_fd,
                целое inode_fd,
                const struct dirent *de,
                const struct statx *sx,
                void *userdata) {

        если (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                вернуть RECURSE_DIR_CONTINUE;

        если (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                вернуть RECURSE_DIR_CONTINUE;

        вернуть set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

целое set_proj_id_recursive(целое fd, uint32_t proj_id) {
        вернуть recurse_dir_at(
                        fd,
                        /* path= */ NULL,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        вернуть IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
