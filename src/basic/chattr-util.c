/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "bitfield.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chattr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "recurse-dir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਪੂਰਨ chattr_ਭਰਿਆ(
              ਪੂਰਨ dir_fd,
              ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
              ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ,
              ਬਿਨਾਨਿਸ਼ਾਨੀ ਨਕਾਬ_3,
              ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪਿਛਲਾ,
              ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਬਿਨਾਨਿਸ਼ਾਨੀ old_attr, new_attr;
        ਪੂਰਨ ਸੈੱਟ_flags_errno = 0;
        ਬਣਤਰ stat st;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, ਰਸਤਾ, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ਜੇ (!inode_type_can_chattr(st.st_mode))
                ਵਾਪਸ -ENOTTY;

        ਜੇ (ਨਕਾਬ_3 == 0 && !ret_ਪਿਛਲਾ && !ret_final)
                ਵਾਪਸ 0;

        ਜੇ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ਵਾਪਸ -errno;

        new_attr = (old_attr & ~ਨਕਾਬ_3) | (ਮੁੱਲ & ਨਕਾਬ_3);
        ਜੇ (new_attr == old_attr) {
                ਜੇ (ret_ਪਿਛਲਾ)
                        *ret_ਪਿਛਲਾ = old_attr;
                ਜੇ (ret_final)
                        *ret_final = old_attr;
                ਵਾਪਸ 0;
        }

        ਜੇ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ਜੇ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (new_attr == attr) {
                        ਜੇ (ret_ਪਿਛਲਾ)
                                *ret_ਪਿਛਲਾ = old_attr;
                        ਜੇ (ret_final)
                                *ret_final = new_attr;
                        ਵਾਪਸ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ਜੇ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_ਸੈੱਟ(flags, CHATTR_FALLBACK_BITWISE))
                ਵਾਪਸ -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        ਬਿਨਾਨਿਸ਼ਾਨੀ current_attr = old_attr;

        ਦੋਆਧਾਰੀ_ਅੰਕ_FOREACH(i, ਨਕਾਬ_3) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ new_one, ਨਕਾਬ_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, ਨਕਾਬ_one, FLAGS_ਸੈੱਟ(ਮੁੱਲ, ਨਕਾਬ_one));
                ਜੇ (new_one == current_attr)
                        ਜਾਰੀ;

                ਜੇ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ਜੇ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ਵਾਪਸ -errno;

                        log_ਭਰਿਆ_errno(FLAGS_ਸੈੱਟ(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", ਨਕਾਬ_one, strna(ਰਸਤਾ));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ਜੇ (ਸੈੱਟ_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ਸੈੱਟ_flags_errno = -errno;

                        ਜਾਰੀ;
                }

                ਜੇ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ਵਾਪਸ -errno;
        }

        ਜੇ (ret_ਪਿਛਲਾ)
                *ret_ਪਿਛਲਾ = old_attr;
        ਜੇ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ਵਾਪਸ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(ਸੈੱਟ_flags_errno) ? ਸੈੱਟ_flags_errno : -ENOANO;
}

ਪੂਰਨ read_attr_fd(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret) {
        ਬਣਤਰ stat st;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (!inode_type_can_chattr(st.st_mode))
                ਵਾਪਸ -ENOTTY;

        _cleanup_close_ ਪੂਰਨ fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_ਰਸਤਾ, &fd_close); /* drop O_PATH if it is set */
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਵਾਪਸ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

ਪੂਰਨ read_attr_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret) {
        _cleanup_close_ ਪੂਰਨ fd_close = -EBADF;
        ਪੂਰਨ fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (isempty(ਰਸਤਾ) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ਨਹੀਂਤਾਂ {
                fd_close = xopenat(dir_fd, ਰਸਤਾ, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ਜੇ (fd_close < 0)
                        ਵਾਪਸ fd_close;

                fd = fd_close;
        }

        ਵਾਪਸ read_attr_fd(fd, ret);
}

ਪੂਰਨ read_fs_xattr_fd(ਪੂਰਨ fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ਬਣਤਰ fsxattr attrs;
        _cleanup_close_ ਪੂਰਨ fd_reopened = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_ਰਸਤਾ, &fd_reopened);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਜੇ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ਜੇ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੈੱਟ_proj_id(ਪੂਰਨ fd, uint32_t proj_id) {
        ਬਣਤਰ fsxattr attrs;
        _cleanup_close_ ਪੂਰਨ fd_reopened = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_ਰਸਤਾ, &fd_reopened);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਜੇ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ਵਾਪਸ -errno;

        ਬਣਤਰ stat statbuf;
        ਜੇ (fstat(fd, &statbuf) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_ਸੈੱਟ(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ਵਾਪਸ 0;

        attrs.fsx_projid = proj_id;
        ਜੇ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ਵਾਪਸ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ਸਥਿਰ ਪੂਰਨ ਸੈੱਟ_proj_id_cb(
                RecurseDirEvent event,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ dir_fd,
                ਪੂਰਨ inode_fd,
                ਅਚਲ ਬਣਤਰ dirent *de,
                ਅਚਲ ਬਣਤਰ statx *sx,
                ਖਾਲੀ *userdata) {

        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(event, RECURSE_DIR_ENTER, RECURSE_DIR_ਇੰਦਰਾਜ))
                ਵਾਪਸ RECURSE_DIR_CONTINUE;

        ਜੇ (de && !ਵਿੱਚ_ਸੈੱਟ(de->d_type, DT_DIR, DT_REG))
                ਵਾਪਸ RECURSE_DIR_CONTINUE;

        ਵਾਪਸ ਸੈੱਟ_proj_id(inode_fd, PTR_ਮੰਜ਼ਿਲ_UINT32(userdata));
}

ਪੂਰਨ ਸੈੱਟ_proj_id_recursive(ਪੂਰਨ fd, uint32_t proj_id) {
        ਵਾਪਸ recurse_dir_at(
                        fd,
                        /* path= */ ਨਿਰਾ,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        ਸੈੱਟ_proj_id_cb,
                        UINT32_ਮੰਜ਼ਿਲ_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ & S_IFMT, S_IFREG, S_IFDIR);
}
