/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/mount.h>

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "filesystems.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "mountpoint-util.h"
#شامل "nulstr-util.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#تعریف_کرو ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_مقصد_handle_at_fatal_error(صحیح err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        أكد(err < 0);

        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                واپس false;
        اگر (ERRNO_IS_NEG_PRIVILEGE(err))
                واپس false;

        واپس !اندر_سیٹ(err, -EOVERFLOW, -EINVAL);
}

صحیح name_مقصد_handle_at_loop(
                صحیح fd,
                ثابت حرف *راستہ,
                ساخت file_handle **ret_handle,
                صحیح *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                صحیح flags) {

        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((flags & ~(AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ|AT_HANDLE_FID)) == 0);

        /* We need to invoke name_to_handle_at() in a loop, given that it might return EOVERFLOW when the specified
         * buffer is too small. Note that in contrast to what the docs might suggest, MAX_HANDLE_SZ is only good as a
         * start value, it is not an upper bound on the buffer size required.
         *
         * This improves on raw name_to_handle_at() also in one other regard: ret_handle and ret_mnt_id can be passed
         * as NULL if there's no interest in either.
         *
         * If unique mount id is requested via ret_unique_mnt_id, try AT_HANDLE_MNT_ID_UNIQUE flag first
         * (needs kernel v6.12), and fall back to statx() if not supported. If neither worked, and caller
         * also specifies ret_mnt_id, then the old-style mount id is returned, -EUNATCH otherwise. */

        اگر (isempty(راستہ)) {
                flags |= AT_خالی_خانے_راستہ;
                راستہ = "";
        }

        برائے (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ ساخت file_handle *h = تہی;

                h = malloc0(offsetof(ساخت file_handle, f_handle) + n);
                اگر (!h)
                        واپس -ENOMEM;

                h->handle_بائٹیں = n;

                اگر (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_مقصد_handle_at(fd, راستہ, h, (صحیح *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        اگر (r >= 0) {
                                اگر (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                اگر (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                واپس 1;
                        }
                        اگر (errno == EOVERFLOW)
                                جاؤ grow;
                        اگر (errno != EINVAL)
                                واپس -errno;
                }

                صحیح mnt_id;
                r = name_مقصد_handle_at(fd, راستہ, h, &mnt_id, flags);
                اگر (r >= 0) {
                        اگر (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                ساخت statx sx;
                                r = xstatx(fd, راستہ,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_خالی_خانے_راستہ))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                اگر (r >= 0) {
                                        اگر (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        اگر (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        واپس 1;
                                }
                                اگر (r != -EUNATCH || !ret_mnt_id)
                                        واپس r;

                                *ret_unique_mnt_id = 0;
                        }

                        اگر (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        اگر (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        واپس 0;
                }
                اگر (errno != EOVERFLOW)
                        واپس -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                اگر (h->handle_بائٹیں <= n)
                        واپس -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_بائٹیں;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                اگر (n > UINT_MAX - offsetof(ساخت file_handle, f_handle))
                        واپس -EOVERFLOW;
        }
}

صحیح name_مقصد_handle_at_try_fid(
                صحیح fd,
                ثابت حرف *راستہ,
                ساخت file_handle **ret_handle,
                صحیح *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                صحیح flags) {

        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_مقصد_handle_at_loop(fd, راستہ, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        اگر (r >= 0 || is_name_مقصد_handle_at_fatal_error(r))
                واپس r;

        واپس name_مقصد_handle_at_loop(fd, راستہ, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

صحیح name_مقصد_handle_at_u64(صحیح fd, ثابت حرف *راستہ, uint64_t *ret) {
        _cleanup_free_ ساخت file_handle *h = تہی;
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_مقصد_handle_at_loop(fd, راستہ, &h, /* ret_mnt_id= */ تہی, /* ret_unique_mnt_id= */ تہی, /* flags= */ 0);
        اگر (r < 0)
                واپس r;
        اگر (h->handle_بائٹیں < حجم(uint64_t))
                واپس -EBADMSG;

        اگر (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        واپس 0;
}

bool file_handle_equal(ثابت ساخت file_handle *a, ثابت ساخت file_handle *b) {
        اگر (a == b)
                واپس true;
        اگر (!a != !b)
                واپس false;
        اگر (a->handle_type != b->handle_type)
                واپس false;

        واپس memcmp_nn(a->f_handle, a->handle_بائٹیں, b->f_handle, b->handle_بائٹیں) == 0;
}

ساخت file_handle* file_handle_dup(ثابت ساخت file_handle *fh) {
        _cleanup_free_ ساخت file_handle *fh_copy = تہی;

        أكد(fh);

        fh_copy = malloc0(offsetof(ساخت file_handle, f_handle) + fh->handle_بائٹیں);
        اگر (!fh_copy)
                واپس تہی;

        fh_copy->handle_بائٹیں = fh->handle_بائٹیں;
        fh_copy->handle_type = fh->handle_type;
        حافظہ_نقل(fh_copy->f_handle, fh->f_handle, fh->handle_بائٹیں);

        واپس TAKE_PTR(fh_copy);
}

صحیح is_mount_نقطہ_at(صحیح dir_fd, ثابت حرف *راستہ, صحیح flags) {
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد((flags & ~AT_SYMLINK_FOLLOW) == 0);

        اگر (راستہ_equal(راستہ, "/"))
                واپس true;

        اگر (dir_fd == XAT_FDROOT && isempty(راستہ))
                واپس true;

        ساخت statx sx;
        r = xstatx_بھرا(dir_fd, راستہ,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_جڑ,
                        &sx);
        اگر (r < 0)
                واپس r;

        اگر (FLAGS_سیٹ(sx.stx_اوصاف, STATX_ATTR_MOUNT_جڑ))
                واپس true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        ساخت statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        اگر (r < 0)
                واپس r;

        واپس statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
صحیح راستہ_is_mount_نقطہ_بھرا(ثابت حرف *راستہ, ثابت حرف *جڑ, صحیح flags) {
        _cleanup_close_ صحیح dir_fd = -EBADF;
        صحیح r;

        أكد(راستہ);
        أكد((flags & ~AT_SYMLINK_FOLLOW) == 0);

        اگر (خالی_خانے_or_جڑ(جڑ))
                واپس is_mount_نقطہ_at(AT_FDCWD, راستہ, flags);

        r = chase(راستہ, جڑ,
                  FLAGS_سیٹ(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ تہی, &dir_fd);
        اگر (r < 0)
                واپس r;

        واپس is_mount_نقطہ_at(dir_fd, /* path= */ تہی, flags);
}

ساکن صحیح راستہ_get_mnt_id_at_اندرونی(صحیح dir_fd, ثابت حرف *راستہ, bool unique, uint64_t *ret) {
        ساخت statx sx;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        r = xstatx(dir_fd, راستہ,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        اگر (r < 0)
                واپس r;

        *ret = sx.stx_mnt_id;
        واپس 0;
}

صحیح راستہ_get_mnt_id_at(صحیح dir_fd, ثابت حرف *راستہ, صحیح *ret) {
        uint64_t mnt_id;
        صحیح r;

        أكد(ret);

        r = راستہ_get_mnt_id_at_اندرونی(dir_fd, راستہ, /* unique = */ false, &mnt_id);
        اگر (r < 0)
                واپس r;

        أكد(mnt_id <= زیادہ_سے_زیادہ_عدد);
        *ret = (صحیح) mnt_id;
        واپس 0;
}

صحیح راستہ_get_unique_mnt_id_at(صحیح dir_fd, ثابت حرف *راستہ, uint64_t *ret) {
        واپس راستہ_get_mnt_id_at_اندرونی(dir_fd, راستہ, /* unique = */ true, ret);
}

bool fstype_is_جال(ثابت حرف *fstype) {
        ثابت حرف *x;

        x = startswith(fstype, "fuse.");
        اگر (x)
                fstype = x;

        اگر (nulstr_شامل_ہے(filesystem_sets[FILESYSTEM_سیٹ_جال].قدر, fstype))
                واپس true;

        /* Filesystems not present in the internal database */
        واپس STR_اندر_سیٹ(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ثابت حرف *fstype) {
       /* 1. quotacheck needs to be run for some filesystems after they are mounted
        *    if the filesystem was not unmounted cleanly.
        * 2. You may need to run quotaon to enable quota usage tracking and/or
        *    enforcement.
        * ext2     - needs 1) and 2)
        * ext3     - needs 2) if configured using usrjquota/grpjquota mount options
        * ext4     - needs 1) if created without journal, needs 2) if created without QUOTA
        *            filesystem feature
        * reiserfs - needs 2).
        * jfs      - needs 2)
        * f2fs     - needs 2) if configured using usrjquota/grpjquota/prjjquota mount options
        * xfs      - nothing needed
        * gfs2     - nothing needed
        * ocfs2    - nothing needed
        * btrfs    - nothing needed
        * for reference see filesystem and quota manpages */
        واپس STR_اندر_سیٹ(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_اندرونی_quota(ثابت حرف *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        واپس STR_اندر_سیٹ(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ثابت حرف *fstype) {
        أكد(fstype);

        ثابت Filesystem_سیٹ *fs;
        FOREACH_دلیل(fs,
                         filesystem_sets + FILESYSTEM_سیٹ_BASIC_API,
                         filesystem_sets + FILESYSTEM_سیٹ_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_سیٹ_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_سیٹ_TEMPORARY)
                اگر (nulstr_شامل_ہے(fs->قدر, fstype))
                    واپس true;

        /* Filesystems not present in the internal database */
        واپس STR_اندر_سیٹ(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ثابت حرف *fstype) {
        ثابت حرف *x;

        x = startswith(fstype, "fuse.");
        اگر (x)
                fstype = x;

        واپس !STR_اندر_سیٹ(fstype, "9p", "overlay") && !fstype_is_جال(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ثابت حرف *fstype) {
        /* All Linux file systems that are necessarily read-only */
        واپس STR_اندر_سیٹ(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ثابت حرف *fstype) {
        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        اگر (STR_اندر_سیٹ(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                واپس true;

        /* On new kernels we can just ask the kernel */
        واپس mount_option_supported(fstype, "discard", تہی) > 0;
}

ثابت حرف* fstype_norecovery_option(ثابت حرف *fstype) {
        صحیح r;

        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        اگر (STR_اندر_سیٹ(fstype, "ext3", "ext4", "xfs"))
                واپس "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        اگر (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", تہی);
                اگر (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        واپس "norecovery";
                }
                اگر (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                اگر (r > 0)
                        واپس "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        واپس mount_option_supported(fstype, "norecovery", تہی) > 0 ? "norecovery" : تہی;
}

bool fstype_can_fmask_dmask(ثابت حرف *fstype) {
        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        واپس streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ثابت حرف *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        واپس !STR_اندر_سیٹ(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ثابت حرف *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        واپس STR_اندر_سیٹ(fstype,
                          "adfs",
                          "exfat",
                          "fat",
                          "hfs",
                          "hpfs",
                          "iso9660",
                          "msdos",
                          "ntfs",
                          "vfat");
}

صحیح dev_is_devtmpfs(خالی) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = تہی;
        صحیح mount_id, r;
        حرف *e;

        r = راستہ_get_mnt_id("/dev", &mount_id);
        اگر (r < 0)
                واپس r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        اگر (r == -ENOENT)
                واپس proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        اگر (r < 0)
                واپس r;

        برائے (;;) {
                _cleanup_free_ حرف *سطر = تہی;
                صحیح mid;

                r = read_سطر(proc_self_mountinfo, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                اگر (لڑی_سے_پڑھو(سطر, "%i", &mid) != 1)
                        جاری;

                اگر (mid != mount_id)
                        جاری;

                e = strstrafter(سطر, " - ");
                اگر (!e)
                        جاری;

                /* accept any name that starts with the currently expected type */
                اگر (startswith(e, "devtmpfs"))
                        واپس true;
        }

        واپس false;
}

ساکن صحیح mount_fd(
                ثابت حرف *ماخذ,
                صحیح ہدف_fd,
                ثابت حرف *filesystemtype,
                غیرعلامتی طویل mountflags,
                ثابت خالی *ڈیٹا) {

        أكد(ہدف_fd >= 0);

        اگر (mount(ماخذ, FORMAT_PROC_FD_راستہ(ہدف_fd), filesystemtype, mountflags, ڈیٹا) < 0) {
                اگر (errno != ENOENT)
                        واپس -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                اگر (proc_mounted() == 0)
                        واپس -ENOSYS;

                واپس -ENOENT;
        }

        واپس 0;
}

صحیح mount_nofollow(
                ثابت حرف *ماخذ,
                ثابت حرف *ہدف,
                ثابت حرف *filesystemtype,
                غیرعلامتی طویل mountflags,
                ثابت خالی *ڈیٹا) {

        _cleanup_close_ صحیح fd = -EBADF;

        أكد(ہدف);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(ہدف, O_راستہ|O_CLOEXEC|O_NOFOLLOW);
        اگر (fd < 0)
                واپس -errno;

        واپس mount_fd(ماخذ, fd, filesystemtype, mountflags, ڈیٹا);
}

ثابت حرف* mount_اشاعت_flag_مقصد_string(غیرعلامتی طویل flags) {

        انتخاب (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        حالت 0:
                واپس "";
        حالت MS_SHARED:
                واپس "shared";
        حالت MS_SLAVE:
                واپس "slave";
        حالت MS_PRIVATE:
                واپس "private";
        }

        واپس تہی;
}

صحیح mount_اشاعت_flag_from_string(ثابت حرف *name, غیرعلامتی طویل *ret) {

        اشارہ_گر_MAY_BE_NULL(name);
        أكد(ret);

        اگر (isempty(name))
                *ret = 0;
        ورنہ اگر (streq(name, "shared"))
                *ret = MS_SHARED;
        ورنہ اگر (streq(name, "slave"))
                *ret = MS_SLAVE;
        ورنہ اگر (streq(name, "private"))
                *ret = MS_PRIVATE;
        ورنہ
                واپس -EINVAL;
        واپس 0;
}

bool mount_اشاعت_flag_is_valid(غیرعلامتی طویل flag) {
        واپس اندر_سیٹ(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(خالی) {
        ساکن صحیح cache = -1;
        صحیح r;

        اگر (cache >= 0)
                واپس cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, تہی, 0, تہی, 0));
        اگر (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                واپس (cache = false);

        واپس (cache = true);
}

صحیح mount_option_supported(ثابت حرف *fstype, ثابت حرف *کلید, ثابت حرف *قدر) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        أكد(fstype);
        أكد(کلید);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        اگر (fd < 0) {
                اگر (errno == ENODEV)
                        واپس log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                واپس log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        اگر (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", تہی, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                اگر (errno == EOPNOTSUPP)
                        واپس -EAGAIN; /* fs not converted to new mount API → don't know */
                اگر (errno != EINVAL)
                        واپس log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ورنہ
                واپس log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        اگر (قدر)
                r = fsconfig(fd, FSCONFIG_SET_STRING, کلید, قدر, 0);
        ورنہ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, کلید, تہی, 0);
        اگر (r < 0) {
                اگر (errno == EINVAL)
                        واپس false; /* EINVAL means option not supported. */

                واپس log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       کلید, قدر ? "=" : "", strempty(قدر), fstype);
        }

        واپس true; /* works! */
}

bool راستہ_نیچے_api_vfs(ثابت حرف *p) {
        أكد(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        واپس راستہ_STARTSWITH_سیٹ(p, "/dev", "/sys", "/proc");
}
