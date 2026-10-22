/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "bitfield.h"
#అన్తర్భావ "chattr-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "recurse-dir.h"
#అన్తర్భావ "string-util.h"

పూర్ణాంకం chattr_నిండిన(
              పూర్ణాంకం dir_fd,
              స్థిరరాశి అక్షరం *మార్గం,
              గుర్తులేని విలువ,
              గుర్తులేని ముసుగు_3,
              గుర్తులేని *ret_మునుపటి,
              గుర్తులేని *ret_final,
              ChattrApplyFlags flags) {

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        గుర్తులేని old_attr, new_attr;
        పూర్ణాంకం సమితి_flags_errno = 0;
        నిర్మాణం stat st;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);

        fd = xopenat(dir_fd, మార్గం, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        /* Explicitly check whether this is a regular file or directory. If it is anything else (such
         * as a device node or fifo), then the ioctl will not hit the file systems but possibly
         * drivers, where the ioctl might have different effects. Notably, DRM is using the same
         * ioctl() number. */

        ఒకవేళ (!inode_type_can_chattr(st.st_mode))
                తిరిగి -ENOTTY;

        ఒకవేళ (ముసుగు_3 == 0 && !ret_మునుపటి && !ret_final)
                తిరిగి 0;

        ఒకవేళ (ioctl(fd, FS_IOC_GETFLAGS, &old_attr) < 0)
                తిరిగి -errno;

        new_attr = (old_attr & ~ముసుగు_3) | (విలువ & ముసుగు_3);
        ఒకవేళ (new_attr == old_attr) {
                ఒకవేళ (ret_మునుపటి)
                        *ret_మునుపటి = old_attr;
                ఒకవేళ (ret_final)
                        *ret_final = old_attr;
                తిరిగి 0;
        }

        ఒకవేళ (ioctl(fd, FS_IOC_SETFLAGS, &new_attr) >= 0) {
                గుర్తులేని attr;

                /* Some filesystems (BTRFS) silently fail when a flag cannot be set. Let's make sure our
                 * changes actually went through by querying the flags again and verifying they're equal to
                 * the flags we tried to configure. */

                ఒకవేళ (ioctl(fd, FS_IOC_GETFLAGS, &attr) < 0)
                        తిరిగి -errno;

                ఒకవేళ (new_attr == attr) {
                        ఒకవేళ (ret_మునుపటి)
                                *ret_మునుపటి = old_attr;
                        ఒకవేళ (ret_final)
                                *ret_final = new_attr;
                        తిరిగి 1;
                }

                /* Trigger the fallback logic. */
                errno = EINVAL;
        }

        ఒకవేళ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno) || !FLAGS_సమితి(flags, CHATTR_FALLBACK_BITWISE))
                తిరిగి -errno;

        /* When -EINVAL is returned, incompatible attributes might be simultaneously specified. E.g.,
         * compress(c) and nocow(C) attributes cannot be set to files on btrfs. As a fallback, let's try to
         * set attributes one by one.
         *
         * Alternatively, when we get EINVAL or EOPNOTSUPP (or a similar error code) we assume a flag might
         * just not be supported, and we can ignore it too */

        గుర్తులేని current_attr = old_attr;

        బిట్టు_FOREACH(i, ముసుగు_3) {
                గుర్తులేని new_one, ముసుగు_one = 1u << i;

                new_one = UPDATE_FLAG(current_attr, ముసుగు_one, FLAGS_సమితి(విలువ, ముసుగు_one));
                ఒకవేళ (new_one == current_attr)
                        కొనసాగు;

                ఒకవేళ (ioctl(fd, FS_IOC_SETFLAGS, &new_one) < 0) {
                        ఒకవేళ (!ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                తిరిగి -errno;

                        log_నిండిన_errno(FLAGS_సమితి(flags, CHATTR_WARN_UNSUPPORTED_FLAGS) ? LOG_WARNING : LOG_DEBUG,
                                       errno,
                                       "Unable to set file attribute 0x%x on %s, ignoring: %m", ముసుగు_one, strna(మార్గం));

                        /* Ensures that we record whether only EOPNOTSUPP&friends are encountered, or if a more serious
                         * error (thus worth logging at a different level, etc) was seen too. */
                        ఒకవేళ (సమితి_flags_errno == 0 || !ERRNO_IS_IOCTL_NOT_SUPPORTED(errno))
                                సమితి_flags_errno = -errno;

                        కొనసాగు;
                }

                ఒకవేళ (ioctl(fd, FS_IOC_GETFLAGS, &current_attr) < 0)
                        తిరిగి -errno;
        }

        ఒకవేళ (ret_మునుపటి)
                *ret_మునుపటి = old_attr;
        ఒకవేళ (ret_final)
                *ret_final = current_attr;

        /* -ENOANO indicates that some attributes cannot be set. ERRNO_IS_IOCTL_NOT_SUPPORTED indicates that
         * all encountered failures were due to flags not supported by the FS, so return a specific error in
         * that case, so callers can handle it properly (e.g.: tmpfiles.d can use debug level logging). */
        తిరిగి current_attr == new_attr ? 1 : ERRNO_IS_IOCTL_NOT_SUPPORTED(సమితి_flags_errno) ? సమితి_flags_errno : -ENOANO;
}

పూర్ణాంకం read_attr_fd(పూర్ణాంకం fd, గుర్తులేని *ret) {
        నిర్మాణం stat st;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        ఒకవేళ (!inode_type_can_chattr(st.st_mode))
                తిరిగి -ENOTTY;

        _cleanup_close_ పూర్ణాంకం fd_close = -EBADF;
        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_మార్గం, &fd_close); /* drop O_PATH if it is set */
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        తిరిగి RET_NERRNO(ioctl(fd, FS_IOC_GETFLAGS, ret));
}

పూర్ణాంకం read_attr_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, గుర్తులేని *ret) {
        _cleanup_close_ పూర్ణాంకం fd_close = -EBADF;
        పూర్ణాంకం fd;

        ప్రతిజ్ఞా(dir_fd >= 0 || dir_fd == AT_FDCWD);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (isempty(మార్గం) && dir_fd != AT_FDCWD)
                fd = dir_fd;
        లేకపోతే {
                fd_close = xopenat(dir_fd, మార్గం, O_RDONLY|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW);
                ఒకవేళ (fd_close < 0)
                        తిరిగి fd_close;

                fd = fd_close;
        }

        తిరిగి read_attr_fd(fd, ret);
}

పూర్ణాంకం read_fs_xattr_fd(పూర్ణాంకం fd, uint32_t *ret_xflags, uint32_t *ret_projid) {
        నిర్మాణం fsxattr attrs;
        _cleanup_close_ పూర్ణాంకం fd_reopened = -EBADF;

        ప్రతిజ్ఞా(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_మార్గం, &fd_reopened);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        ఒకవేళ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_xflags)
                *ret_xflags = attrs.fsx_xflags;

        ఒకవేళ (ret_projid)
                *ret_projid = attrs.fsx_projid;

        తిరిగి 0;
}

పూర్ణాంకం సమితి_proj_id(పూర్ణాంకం fd, uint32_t proj_id) {
        నిర్మాణం fsxattr attrs;
        _cleanup_close_ పూర్ణాంకం fd_reopened = -EBADF;

        ప్రతిజ్ఞా(fd >= 0);

        fd = fd_reopen_condition(fd, O_RDONLY|O_CLOEXEC|O_NOCTTY, O_మార్గం, &fd_reopened);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        ఒకవేళ (ioctl(fd, FS_IOC_FSGETXATTR, &attrs) < 0)
                తిరిగి -errno;

        నిర్మాణం stat statbuf;
        ఒకవేళ (fstat(fd, &statbuf) < 0)
                తిరిగి -errno;

        ఒకవేళ (attrs.fsx_projid == proj_id && (!S_ISDIR(statbuf.st_mode) || FLAGS_సమితి(attrs.fsx_xflags, FS_XFLAG_PROJINHERIT)))
                తిరిగి 0;

        attrs.fsx_projid = proj_id;
        ఒకవేళ (S_ISDIR(statbuf.st_mode))
                attrs.fsx_xflags |= FS_XFLAG_PROJINHERIT;

        తిరిగి RET_NERRNO(ioctl(fd, FS_IOC_FSSETXATTR, &attrs));
}

స్థిర పూర్ణాంకం సమితి_proj_id_cb(
                RecurseDirEvent event,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం dir_fd,
                పూర్ణాంకం inode_fd,
                స్థిరరాశి నిర్మాణం dirent *de,
                స్థిరరాశి నిర్మాణం statx *sx,
                శూన్య *userdata) {

        ఒకవేళ (!లోపల_సమితి(event, RECURSE_DIR_ENTER, RECURSE_DIR_నమోదు))
                తిరిగి RECURSE_DIR_CONTINUE;

        ఒకవేళ (de && !లోపల_సమితి(de->d_type, DT_DIR, DT_REG))
                తిరిగి RECURSE_DIR_CONTINUE;

        తిరిగి సమితి_proj_id(inode_fd, PTR_గమ్యం_UINT32(userdata));
}

పూర్ణాంకం సమితి_proj_id_recursive(పూర్ణాంకం fd, uint32_t proj_id) {
        తిరిగి recurse_dir_at(
                        fd,
                        /* path= */ ఖాళీ,
                        /* statx_mask= */ 0,
                        /* n_depth_max= */ UINT_MAX,
                        RECURSE_DIR_ENSURE_TYPE|RECURSE_DIR_TOPLEVEL|RECURSE_DIR_INODE_FD,
                        సమితి_proj_id_cb,
                        UINT32_గమ్యం_PTR(proj_id));
}

bool inode_type_can_chattr(mode_t బహుళకం) {
        తిరిగి లోపల_సమితి(బహుళకం & S_IFMT, S_IFREG, S_IFDIR);
}
