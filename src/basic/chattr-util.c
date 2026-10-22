/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ "bitfield.h"
#ଅନ୍ତର୍ଭାଵ "chattr-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "recurse-dir.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chattr_full(
              ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
              ଅଚଳ ଅକ୍ଷର *path,
              unsigned ମୂଲ୍ୟ,
              unsigned mask,
              unsigned *ret_previous,
              unsigned *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        unsigned old_attr, new_attr;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_flags_errno = 0;
        ସଂରଚନା stat st;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ଯଦି (!inode_type_can_chattr(st.st_mode))
                ଫେରନ୍ତୁ -ENOTTY;

        ଯଦି (mask == 0 && !ret_previous && !ret_final)
                ଫେରନ୍ତୁ 0;

        ଯଦି (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                ଫେରନ୍ତୁ -errno;

        new_attr = (old_attr & ~mask) | (ମୂଲ୍ୟ & mask);
        ଯଦି (new_attr == old_attr) {
                ଯଦି (ret_previous)
                        *ret_previous = old_attr;
                ଯଦି (ret_final)
                        *ret_final = old_attr;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                unsigned attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ଯଦି (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (new_attr == attr) {
                        ଯଦି (ret_previous)
                                *ret_previous = old_attr;
                        ଯଦି (ret_final)
                                *ret_final = new_attr;
                        ଫେରନ୍ତୁ 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ଯଦି (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_SET(flags, CHATTR_FALLBACK_BITWISE))
                ଫେରନ୍ତୁ -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        unsigned current_attr = old_attr;

        BIT_FOREACH(i, mask) {
                unsigned new_one, mask_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, mask_one, FLAGS_SET(ମୂଲ୍ୟ, mask_one));
                ଯଦି (new_one == current_attr)
                        ଜାରି;

                ଯଦି (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ଯଦି (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                ଫେରନ୍ତୁ -errno;

                        log_full_errno(FLAGS_SET(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", mask_one, strna(path));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ଯଦି (set_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                set_flags_errno = -errno;

                        ଜାରି;
                }

                ଯଦି (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଯଦି (ret_previous)
                *ret_previous = old_attr;
        ଯଦି (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        ଫେରନ୍ତୁ current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(set_flags_errno) ? set_flags_errno : -ENOANO;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_attr_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned *ret) {
        ସଂରଚନା stat st;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (!inode_type_can_chattr(st.st_mode))
                ଫେରନ୍ତୁ -ENOTTY;

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_close); /* drop O_PATH if it is set */
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଫେରନ୍ତୁ RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_attr_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, unsigned *ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_close = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (isempty(path) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        ଅନ୍ୟଥା {
                fd_close = xopenat(dir_fd, path, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ଯଦି (fd_close < 0)
                        ଫେରନ୍ତୁ fd_close;

                fd = fd_close;
        }

        ଫେରନ୍ତୁ read_attr_fd(fd, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_fs_xattr_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        ସଂରଚନା fsxattr attrs;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopened = -EBADF;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ଯଦି (ret_projid)
                *ret_projid = attrs.fsx_projid;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_proj_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint32_t proj_id) {
        ସଂରଚନା fsxattr attrs;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_reopened = -EBADF;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_PATH, &fd_reopened);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                ଫେରନ୍ତୁ -errno;

        ସଂରଚନା stat statbuf;
        ଯଦି (fstat(fd, &statbuf) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_SET(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                ଫେରନ୍ତୁ 0;

        attrs.fsx_projid = proj_id;
        ଯଦି (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        ଫେରନ୍ତୁ RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_proj_id_cb(
                RecurseDirEvent event,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_fd,
                ଅଚଳ ସଂରଚନା dirent *de,
                ଅଚଳ ସଂରଚନା statx *sx,
                ଶୂନ୍ୟ *userdata) {

        ଯଦି (!IN_SET(event, RECURSE_DIR_ENTER, RECURSE_DIR_ENTRY))
                ଫେରନ୍ତୁ RECURSE_DIR_CONTINUE;

        ଯଦି (de && !IN_SET(de->d_type, DT_DIR, DT_REG))
                ଫେରନ୍ତୁ RECURSE_DIR_CONTINUE;

        ଫେରନ୍ତୁ set_proj_id(inode_fd, PTR_TO_UINT32(userdata));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_proj_id_recursive(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint32_t proj_id) {
        ଫେରନ୍ତୁ recurse_dir_at(
                        fd,
                        /* path= */ ଖାଲି,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        set_proj_id_cb,
                        UINT32_TO_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t mode) {
        ଫେରନ୍ତୁ IN_SET(mode & S_IFMT, S_IFREG, S_IFDIR);
}
