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

puur_naa_nka chattr_full(
              puur_naa_nka dir_fd,
              const var_na *path,
              unsigned value,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ puur_naa_nka fd = -EBADF;
        unsigned old_attr, new_attr;
        puur_naa_nka set_flags_errno = 0;
        sa_nracanaa stat st;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ fd;

        ᨀᨘ (fstat(fd, &st) < 0)
                ᨀᨘᨒ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ᨀᨘ (!inode_type_can_chattr(st.st_mode))
                ᨀᨘᨒ -ENOTTY;

        ᨀᨘ (mask == 0 && !ret_previous && !ret_final)
                ᨀᨘᨒ 0;

        ᨀᨘ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ᨀᨘᨒ -errno;

        new_attr = (old_attr & ~mask) | (value & mask);
        ᨀᨘ (new_attr == old_attr) {
                ᨀᨘ (ret_previous)
                        *ret_previous = old_attr;
                ᨀᨘ (ret_final)
                        *ret_final = old_attr;
                ᨀᨘᨒ 0;
        }

        ᨀᨘ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ᨀᨘ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ᨀᨘᨒ -errno;

                ᨀᨘ (new_attr == attr) {
                        ᨀᨘ (ret_previous)
                                *ret_previous = old_attr;
                        ᨀᨘ (ret_final)
                                *ret_final = new_attr;
                        ᨀᨘᨒ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ᨀᨘ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ᨀᨘᨒ -errno;

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
                ᨀᨘ (new_one == current_attr)
                        continue;

                ᨀᨘ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ᨀᨘ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ᨀᨘᨒ -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ᨀᨘ (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        continue;
                }

                ᨀᨘ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ᨀᨘᨒ -errno;
        }

        ᨀᨘ (ret_previous)
                *ret_previous = old_attr;
        ᨀᨘ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ᨀᨘᨒ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

puur_naa_nka read_attr_fd(puur_naa_nka fd, unsigned *ret) {
        sa_nracanaa stat st;

        assert(fd >= 0);
        assert(ret);

        ᨀᨘ (fstat(fd, &st) < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘ (!inode_type_can_chattr(st.st_mode))
                ᨀᨘᨒ -ENOTTY;

        _cleanup_close_ puur_naa_nka fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ fd;

        ᨀᨘᨒ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

puur_naa_nka read_attr_at(puur_naa_nka dir_fd, const var_na *path, unsigned *ret) {
        _cleanup_close_ puur_naa_nka fd_close = -EBADF;
        puur_naa_nka fd;

        assert(dir_fd >= 0 || dir_fd == AT_FDCWD);
        assert(ret);

        ᨀᨘ (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ᨀᨘᨁ {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ᨀᨘ (fd_close < 0)
                        ᨀᨘᨒ fd_close;

                fd = fd_close;
        }

        ᨀᨘᨒ read_attr_fd(fd, ret);
}

puur_naa_nka read_fs_xattr_fd(puur_naa_nka fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        sa_nracanaa fsxattr attrs;
        _cleanup_close_ puur_naa_nka fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ fd;

        ᨀᨘ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ᨀᨘ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ᨀᨘᨒ 0;
}

puur_naa_nka set_proj_id(puur_naa_nka fd, uint32_t proj_id) {
        sa_nracanaa fsxattr attrs;
        _cleanup_close_ puur_naa_nka fd_reopened = -EBADF;

        assert(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ fd;

        ᨀᨘ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ᨀᨘᨒ -errno;

        sa_nracanaa stat statbuf;
        ᨀᨘ (fstat(fd, &statbuf) < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ᨀᨘᨒ 0;

        attrs.fsx_projid = proj_id;
        ᨀᨘ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ᨀᨘᨒ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

static puur_naa_nka set_proj_id_cb(
                RecurseDirEvent event,
                const var_na *path,
                puur_naa_nka dir_fd,
                puur_naa_nka inode_fd,
                const sa_nracanaa dirent *de,
                const sa_nracanaa statx *sx,
                void *userdata) {

        ᨀᨘ (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ᨀᨘᨒ RECURSE_DIR_CONTINUE;

        ᨀᨘ (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ᨀᨘᨒ RECURSE_DIR_CONTINUE;

        ᨀᨘᨒ set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

puur_naa_nka set_proj_id_recursive(puur_naa_nka fd, uint32_t proj_id) {
        ᨀᨘᨒ recurse_dir_at(
                        fd,
                        /* path= */ NULL,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ᨀᨘᨒ IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
