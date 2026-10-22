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

จำนวนเต็ม chattr_full(
              จำนวนเต็ม dir_fd,
              const var_na *path,
              unsigned value,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ จำนวนเต็ม fd = -EBADF;
        unsigned old_attr, new_attr;
        จำนวนเต็ม set_flags_errno = 0;
        sa_nracanaa stat st;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ถ้า (fd < 0)
                คืน fd;

        ถ้า (fstat(fd, &st) < 0)
                คืน -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ถ้า (!inode_type_can_chattr(st.st_mode))
                คืน -ENOTTY;

        ถ้า (mask == 0 && !ret_previous && !ret_final)
                คืน 0;

        ถ้า (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                คืน -errno;

        new_attr = (old_attr & ~mask) | (value & mask);
        ถ้า (new_attr == old_attr) {
                ถ้า (ret_previous)
                        *ret_previous = old_attr;
                ถ้า (ret_final)
                        *ret_final = old_attr;
                คืน 0;
        }

        ถ้า (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ถ้า (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        คืน -errno;

                ถ้า (new_attr == attr) {
                        ถ้า (ret_previous)
                                *ret_previous = old_attr;
                        ถ้า (ret_final)
                                *ret_final = new_attr;
                        คืน 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ถ้า (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                คืน -errno;

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
                ถ้า (new_one == current_attr)
                        continue;

                ถ้า (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ถ้า (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                คืน -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ถ้า (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        continue;
                }

                ถ้า (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        คืน -errno;
        }

        ถ้า (ret_previous)
                *ret_previous = old_attr;
        ถ้า (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        คืน current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

จำนวนเต็ม read_attr_fd(จำนวนเต็ม fd, unsigned *ret) {
        sa_nracanaa stat st;

        assert(fd >= 0);
        assert(ret);

        ถ้า (fstat(fd, &st) < 0)
                คืน -errno;

        ถ้า (!inode_type_can_chattr(st.st_mode))
                คืน -ENOTTY;

        _cleanup_close_ จำนวนเต็ม fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        ถ้า (fd < 0)
                คืน fd;

        คืน RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

จำนวนเต็ม read_attr_at(จำนวนเต็ม dir_fd, const var_na *path, unsigned *ret) {
        _cleanup_close_ จำนวนเต็ม fd_close = -EBADF;
        จำนวนเต็ม fd;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(ret);

        ถ้า (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        มิฉะนั้น {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ถ้า (fd_close < 0)
                        คืน fd_close;

                fd = fd_close;
        }

        คืน read_attr_fd(fd, ret);
}

จำนวนเต็ม read_fs_xattr_fd(จำนวนเต็ม fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        sa_nracanaa fsxattr attrs;
        _cleanup_close_ จำนวนเต็ม fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ถ้า (fd < 0)
                คืน fd;

        ถ้า (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                คืน -errno;

        ถ้า (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ถ้า (ret_projid)
                *ret_projid = attrs.fsx_projid;

        คืน 0;
}

จำนวนเต็ม set_proj_id(จำนวนเต็ม fd, uint32_t proj_id) {
        sa_nracanaa fsxattr attrs;
        _cleanup_close_ จำนวนเต็ม fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ถ้า (fd < 0)
                คืน fd;

        ถ้า (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                คืน -errno;

        sa_nracanaa stat statbuf;
        ถ้า (fstat(fd, &statbuf) < 0)
                คืน -errno;

        ถ้า (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                คืน 0;

        attrs.fsx_projid = proj_id;
        ถ้า (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        คืน RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

static จำนวนเต็ม set_proj_id_cb(
                RecurseDirEvent event,
                const var_na *path,
                จำนวนเต็ม dir_fd,
                จำนวนเต็ม inode_fd,
                const sa_nracanaa dirent *de,
                const sa_nracanaa statx *sx,
                void *userdata) {

        ถ้า (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                คืน RECURSE_DIR_CONTINUE;

        ถ้า (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                คืน RECURSE_DIR_CONTINUE;

        คืน set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

จำนวนเต็ม set_proj_id_recursive(จำนวนเต็ม fd, uint32_t proj_id) {
        คืน recurse_dir_at(
                        fd,
                        /* path= */ NULL,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        คืน IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
