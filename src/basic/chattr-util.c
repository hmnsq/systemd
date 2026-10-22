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

ကိန်းပြည့် chattr_full(
              ကိန်းပြည့် dir_fd,
              const var_na *path,
              unsigned value,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;
        unsigned old_attr, new_attr;
        ကိန်းပြည့် set_flags_errno = 0;
        sa_nracanaa stat st;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        အကယ်၍ (fstat(fd, &st) < 0)
                ပြန်ပေး -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        အကယ်၍ (!inode_type_can_chattr(st.st_mode))
                ပြန်ပေး -ENOTTY;

        အကယ်၍ (mask == 0 && !ret_previous && !ret_final)
                ပြန်ပေး 0;

        အကယ်၍ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ပြန်ပေး -errno;

        new_attr = (old_attr & ~mask) | (value & mask);
        အကယ်၍ (new_attr == old_attr) {
                အကယ်၍ (ret_previous)
                        *ret_previous = old_attr;
                အကယ်၍ (ret_final)
                        *ret_final = old_attr;
                ပြန်ပေး 0;
        }

        အကယ်၍ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                အကယ်၍ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ပြန်ပေး -errno;

                အကယ်၍ (new_attr == attr) {
                        အကယ်၍ (ret_previous)
                                *ret_previous = old_attr;
                        အကယ်၍ (ret_final)
                                *ret_final = new_attr;
                        ပြန်ပေး 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        အကယ်၍ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ပြန်ပေး -errno;

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
                အကယ်၍ (new_one == current_attr)
                        continue;

                အကယ်၍ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        အကယ်၍ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ပြန်ပေး -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        အကယ်၍ (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        continue;
                }

                အကယ်၍ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ပြန်ပေး -errno;
        }

        အကယ်၍ (ret_previous)
                *ret_previous = old_attr;
        အကယ်၍ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ပြန်ပေး current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

ကိန်းပြည့် read_attr_fd(ကိန်းပြည့် fd, unsigned *ret) {
        sa_nracanaa stat st;

        assert(fd >= 0);
        assert(ret);

        အကယ်၍ (fstat(fd, &st) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (!inode_type_can_chattr(st.st_mode))
                ပြန်ပေး -ENOTTY;

        _cleanup_close_ ကိန်းပြည့် fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        ပြန်ပေး RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

ကိန်းပြည့် read_attr_at(ကိန်းပြည့် dir_fd, const var_na *path, unsigned *ret) {
        _cleanup_close_ ကိန်းပြည့် fd_close = -EBADF;
        ကိန်းပြည့် fd;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(ret);

        အကယ်၍ (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        မဟုတ်ပါက {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                အကယ်၍ (fd_close < 0)
                        ပြန်ပေး fd_close;

                fd = fd_close;
        }

        ပြန်ပေး read_attr_fd(fd, ret);
}

ကိန်းပြည့် read_fs_xattr_fd(ကိန်းပြည့် fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        sa_nracanaa fsxattr attrs;
        _cleanup_close_ ကိန်းပြည့် fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        အကယ်၍ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        အကယ်၍ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ပြန်ပေး 0;
}

ကိန်းပြည့် set_proj_id(ကိန်းပြည့် fd, uint32_t proj_id) {
        sa_nracanaa fsxattr attrs;
        _cleanup_close_ ကိန်းပြည့် fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        အကယ်၍ (fd < 0)
                ပြန်ပေး fd;

        အကယ်၍ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ပြန်ပေး -errno;

        sa_nracanaa stat statbuf;
        အကယ်၍ (fstat(fd, &statbuf) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ပြန်ပေး 0;

        attrs.fsx_projid = proj_id;
        အကယ်၍ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ပြန်ပေး RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

static ကိန်းပြည့် set_proj_id_cb(
                RecurseDirEvent event,
                const var_na *path,
                ကိန်းပြည့် dir_fd,
                ကိန်းပြည့် inode_fd,
                const sa_nracanaa dirent *de,
                const sa_nracanaa statx *sx,
                void *userdata) {

        အကယ်၍ (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ပြန်ပေး RECURSE_DIR_CONTINUE;

        အကယ်၍ (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ပြန်ပေး RECURSE_DIR_CONTINUE;

        ပြန်ပေး set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

ကိန်းပြည့် set_proj_id_recursive(ကိန်းပြည့် fd, uint32_t proj_id) {
        ပြန်ပေး recurse_dir_at(
                        fd,
                        /* path= */ NULL,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ပြန်ပေး IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
