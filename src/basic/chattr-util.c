/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ "bitfield.h"
#അന്തര്ഭാവ "chattr-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "recurse-dir.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ chattr_full(
              പൂർണ്ണസംഖ്യ dir_fd,
              സ്ഥിരം അക്ഷരം *path,
              unsigned മൂല്യം,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        unsigned old_attr, new_attr;
        പൂർണ്ണസംഖ്യ set_flags_errno = 0;
        ഘടന stat st;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        എങ്കിൽ (!inode_type_can_chattr(st.st_mode))
                തിരികെ -ENOTTY;

        എങ്കിൽ (mask == 0 && !ret_previous && !ret_final)
                തിരികെ 0;

        എങ്കിൽ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                തിരികെ -errno;

        new_attr = (old_attr & ~mask) | (മൂല്യം & mask);
        എങ്കിൽ (new_attr == old_attr) {
                എങ്കിൽ (ret_previous)
                        *ret_previous = old_attr;
                എങ്കിൽ (ret_final)
                        *ret_final = old_attr;
                തിരികെ 0;
        }

        എങ്കിൽ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                എങ്കിൽ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        തിരികെ -errno;

                എങ്കിൽ (new_attr == attr) {
                        എങ്കിൽ (ret_previous)
                                *ret_previous = old_attr;
                        എങ്കിൽ (ret_final)
                                *ret_final = new_attr;
                        തിരികെ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        എങ്കിൽ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                തിരികെ -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(മൂല്യം, mask_one));
                എങ്കിൽ (new_one == current_attr)
                        തുടരുക;

                എങ്കിൽ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        എങ്കിൽ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                തിരികെ -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        എങ്കിൽ (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        തുടരുക;
                }

                എങ്കിൽ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        തിരികെ -errno;
        }

        എങ്കിൽ (ret_previous)
                *ret_previous = old_attr;
        എങ്കിൽ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        തിരികെ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

പൂർണ്ണസംഖ്യ read_attr_fd(പൂർണ്ണസംഖ്യ fd, unsigned *ret) {
        ഘടന stat st;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        എങ്കിൽ (!inode_type_can_chattr(st.st_mode))
                തിരികെ -ENOTTY;

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        തിരികെ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

പൂർണ്ണസംഖ്യ read_attr_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, unsigned *ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd_close = -EBADF;
        പൂർണ്ണസംഖ്യ fd;

        പ്രതിജ്ഞാ(dir_fd >= 0 || dir_fd == AT_FDCWD);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        അല്ലെങ്കിൽ {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                എങ്കിൽ (fd_close < 0)
                        തിരികെ fd_close;

                fd = fd_close;
        }

        തിരികെ read_attr_fd(fd, ret);
}

പൂർണ്ണസംഖ്യ read_fs_xattr_fd(പൂർണ്ണസംഖ്യ fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ഘടന fsxattr attrs;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd_reopened = -EBADF;

        പ്രതിജ്ഞാ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        എങ്കിൽ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        എങ്കിൽ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ set_proj_id(പൂർണ്ണസംഖ്യ fd, uint32_t proj_id) {
        ഘടന fsxattr attrs;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd_reopened = -EBADF;

        പ്രതിജ്ഞാ(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        എങ്കിൽ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                തിരികെ -errno;

        ഘടന stat statbuf;
        എങ്കിൽ (fstat(fd, &statbuf) < 0)
                തിരികെ -errno;

        എങ്കിൽ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                തിരികെ 0;

        attrs.fsx_projid = proj_id;
        എങ്കിൽ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        തിരികെ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ set_proj_id_cb(
                RecurseDirEvent event,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ dir_fd,
                പൂർണ്ണസംഖ്യ inode_fd,
                സ്ഥിരം ഘടന dirent *de,
                സ്ഥിരം ഘടന statx *sx,
                ശൂന്യം *userdata) {

        എങ്കിൽ (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                തിരികെ RECURSE_DIR_CONTINUE;

        എങ്കിൽ (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                തിരികെ RECURSE_DIR_CONTINUE;

        തിരികെ set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

പൂർണ്ണസംഖ്യ set_proj_id_recursive(പൂർണ്ണസംഖ്യ fd, uint32_t proj_id) {
        തിരികെ recurse_dir_at(
                        fd,
                        /* path= */ ശൂന്യ,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        തിരികെ IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
