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
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(صحیح err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        أكد(err < 0);

        اگر (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                بازگشت false;
        اگر (ERRNO_IS_NEG_PRIVILEGE(err))
                بازگشت false;

        بازگشت !IN_SET(err, -EOVERFLOW, -EINVAL);
}

صحیح name_to_handle_at_loop(
                صحیح fd,
                ثابت نویسه *path,
                ساختار file_handle **ret_handle,
                صحیح *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                صحیح flags) {

        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);
        أكد((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        اگر (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        برای (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ ساختار file_handle *h = پوچ;

                h = malloc0(offsetof(ساختار file_handle, f_handle) + n);
                اگر (!h)
                        بازگشت -ENOMEM;

                h->handle_bytes = n;

                اگر (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (صحیح *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        اگر (r >= 0) {
                                اگر (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                اگر (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                بازگشت 1;
                        }
                        اگر (errno == EOVERFLOW)
                                برو grow;
                        اگر (errno != EINVAL)
                                بازگشت -errno;
                }

                صحیح mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                اگر (r >= 0) {
                        اگر (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                ساختار statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                اگر (r >= 0) {
                                        اگر (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        اگر (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        بازگشت 1;
                                }
                                اگر (r != -EUNATCH || !ret_mnt_id)
                                        بازگشت r;

                                *ret_unique_mnt_id = 0;
                        }

                        اگر (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        اگر (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        بازگشت 0;
                }
                اگر (errno != EOVERFLOW)
                        بازگشت -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                اگر (h->handle_bytes <= n)
                        بازگشت -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                اگر (n > UINT_MAX - offsetof(ساختار file_handle, f_handle))
                        بازگشت -EOVERFLOW;
        }
}

صحیح name_to_handle_at_try_fid(
                صحیح fd,
                ثابت نویسه *path,
                ساختار file_handle **ret_handle,
                صحیح *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                صحیح flags) {

        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        اگر (r >= 0 || is_name_to_handle_at_fatal_error(r))
                بازگشت r;

        بازگشت name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

صحیح name_to_handle_at_u64(صحیح fd, ثابت نویسه *path, uint64_t *ret) {
        _cleanup_free_ ساختار file_handle *h = پوچ;
        صحیح r;

        أكد(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ پوچ, /* ret_unique_mnt_id= */ پوچ, /* flags= */ 0);
        اگر (r < 0)
                بازگشت r;
        اگر (h->handle_bytes < اندازه(uint64_t))
                بازگشت -EBADMSG;

        اگر (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        بازگشت 0;
}

bool file_handle_equal(ثابت ساختار file_handle *a, ثابت ساختار file_handle *b) {
        اگر (a == b)
                بازگشت true;
        اگر (!a != !b)
                بازگشت false;
        اگر (a->handle_type != b->handle_type)
                بازگشت false;

        بازگشت memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

ساختار file_handle* file_handle_dup(ثابت ساختار file_handle *fh) {
        _cleanup_free_ ساختار file_handle *fh_copy = پوچ;

        أكد(fh);

        fh_copy = malloc0(offsetof(ساختار file_handle, f_handle) + fh->handle_bytes);
        اگر (!fh_copy)
                بازگشت پوچ;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        بازگشت TAKE_PTR(fh_copy);
}

صحیح is_mount_point_at(صحیح dir_fd, ثابت نویسه *path, صحیح flags) {
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد((flags & ~AT_SYMLINK_FOLLOW) == 0);

        اگر (path_equal(path, "/"))
                بازگشت true;

        اگر (dir_fd == XAT_FDROOT && isempty(path))
                بازگشت true;

        ساختار statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ریشه,
                        &sx);
        اگر (r < 0)
                بازگشت r;

        اگر (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ریشه))
                بازگشت true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        ساختار statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        اگر (r < 0)
                بازگشت r;

        بازگشت statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
صحیح path_is_mount_point_full(ثابت نویسه *path, ثابت نویسه *ریشه, صحیح flags) {
        _cleanup_close_ صحیح dir_fd = -EBADF;
        صحیح r;

        أكد(path);
        أكد((flags & ~AT_SYMLINK_FOLLOW) == 0);

        اگر (empty_or_ریشه(ریشه))
                بازگشت is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, ریشه,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ پوچ, &dir_fd);
        اگر (r < 0)
                بازگشت r;

        بازگشت is_mount_point_at(dir_fd, /* path= */ پوچ, flags);
}

ایستا صحیح path_get_mnt_id_at_internal(صحیح dir_fd, ثابت نویسه *path, bool unique, uint64_t *ret) {
        ساختار statx sx;
        صحیح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        اگر (r < 0)
                بازگشت r;

        *ret = sx.stx_mnt_id;
        بازگشت 0;
}

صحیح path_get_mnt_id_at(صحیح dir_fd, ثابت نویسه *path, صحیح *ret) {
        uint64_t mnt_id;
        صحیح r;

        أكد(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        اگر (r < 0)
                بازگشت r;

        أكد(mnt_id <= INT_MAX);
        *ret = (صحیح) mnt_id;
        بازگشت 0;
}

صحیح path_get_unique_mnt_id_at(صحیح dir_fd, ثابت نویسه *path, uint64_t *ret) {
        بازگشت path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(ثابت نویسه *fstype) {
        ثابت نویسه *x;

        x = startswith(fstype, "fuse.");
        اگر (x)
                fstype = x;

        اگر (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].مقدار, fstype))
                بازگشت true;

        /* Filesystems not present in the internal database */
        بازگشت STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ثابت نویسه *fstype) {
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
        بازگشت STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(ثابت نویسه *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        بازگشت STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ثابت نویسه *fstype) {
        أكد(fstype);

        ثابت FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                اگر (nulstr_contains(fs->مقدار, fstype))
                    بازگشت true;

        /* Filesystems not present in the internal database */
        بازگشت STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ثابت نویسه *fstype) {
        ثابت نویسه *x;

        x = startswith(fstype, "fuse.");
        اگر (x)
                fstype = x;

        بازگشت !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ثابت نویسه *fstype) {
        /* All Linux file systems that are necessarily read-only */
        بازگشت STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ثابت نویسه *fstype) {
        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        اگر (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                بازگشت true;

        /* On new kernels we can just ask the kernel */
        بازگشت mount_option_supported(fstype, "discard", پوچ) > 0;
}

ثابت نویسه* fstype_norecovery_option(ثابت نویسه *fstype) {
        صحیح r;

        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        اگر (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                بازگشت "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        اگر (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", پوچ);
                اگر (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        بازگشت "norecovery";
                }
                اگر (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                اگر (r > 0)
                        بازگشت "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        بازگشت mount_option_supported(fstype, "norecovery", پوچ) > 0 ? "norecovery" : پوچ;
}

bool fstype_can_fmask_dmask(ثابت نویسه *fstype) {
        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        بازگشت streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ثابت نویسه *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        بازگشت !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ثابت نویسه *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        بازگشت STR_IN_SET(fstype,
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

صحیح dev_is_devtmpfs(تهی) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = پوچ;
        صحیح mount_id, r;
        نویسه *e;

        r = path_get_mnt_id("/dev", &mount_id);
        اگر (r < 0)
                بازگشت r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        اگر (r == -ENOENT)
                بازگشت proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        اگر (r < 0)
                بازگشت r;

        برای (;;) {
                _cleanup_free_ نویسه *سطر = پوچ;
                صحیح mid;

                r = read_سطر(proc_self_mountinfo, LONG_سطر_MAX, &سطر);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                اگر (sscanf(سطر, "%i", &mid) != 1)
                        ادامه;

                اگر (mid != mount_id)
                        ادامه;

                e = strstrafter(سطر, " - ");
                اگر (!e)
                        ادامه;

                /* accept any name that starts with the currently expected type */
                اگر (startswith(e, "devtmpfs"))
                        بازگشت true;
        }

        بازگشت false;
}

ایستا صحیح mount_fd(
                ثابت نویسه *source,
                صحیح target_fd,
                ثابت نویسه *filesystemtype,
                unsigned long mountflags,
                ثابت تهی *data) {

        أكد(target_fd >= 0);

        اگر (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                اگر (errno != ENOENT)
                        بازگشت -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                اگر (proc_mounted() == 0)
                        بازگشت -ENOSYS;

                بازگشت -ENOENT;
        }

        بازگشت 0;
}

صحیح mount_nofollow(
                ثابت نویسه *source,
                ثابت نویسه *target,
                ثابت نویسه *filesystemtype,
                unsigned long mountflags,
                ثابت تهی *data) {

        _cleanup_close_ صحیح fd = -EBADF;

        أكد(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        اگر (fd < 0)
                بازگشت -errno;

        بازگشت mount_fd(source, fd, filesystemtype, mountflags, data);
}

ثابت نویسه* mount_propagation_flag_to_string(unsigned long flags) {

        انتخاب (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        حالت 0:
                بازگشت "";
        حالت MS_SHARED:
                بازگشت "shared";
        حالت MS_SLAVE:
                بازگشت "slave";
        حالت MS_PRIVATE:
                بازگشت "private";
        }

        بازگشت پوچ;
}

صحیح mount_propagation_flag_from_string(ثابت نویسه *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        أكد(ret);

        اگر (isempty(name))
                *ret = 0;
        وگرنه اگر (streq(name, "shared"))
                *ret = MS_SHARED;
        وگرنه اگر (streq(name, "slave"))
                *ret = MS_SLAVE;
        وگرنه اگر (streq(name, "private"))
                *ret = MS_PRIVATE;
        وگرنه
                بازگشت -EINVAL;
        بازگشت 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        بازگشت IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(تهی) {
        ایستا صحیح cache = -1;
        صحیح r;

        اگر (cache >= 0)
                بازگشت cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, پوچ, 0, پوچ, 0));
        اگر (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                بازگشت (cache = false);

        بازگشت (cache = true);
}

صحیح mount_option_supported(ثابت نویسه *fstype, ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        أكد(fstype);
        أكد(کلید);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        اگر (fd < 0) {
                اگر (errno == ENODEV)
                        بازگشت log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                بازگشت log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        اگر (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", پوچ, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                اگر (errno == EOPNOTSUPP)
                        بازگشت -EAGAIN; /* fs not converted to new mount API → don't know */
                اگر (errno != EINVAL)
                        بازگشت log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } وگرنه
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        اگر (مقدار)
                r = fsconfig(fd, FSCONFIG_SET_STRING, کلید, مقدار, 0);
        وگرنه
                r = fsconfig(fd, FSCONFIG_SET_FLAG, کلید, پوچ, 0);
        اگر (r < 0) {
                اگر (errno == EINVAL)
                        بازگشت false; /* EINVAL means option not supported. */

                بازگشت log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       کلید, مقدار ? "=" : "", strempty(مقدار), fstype);
        }

        بازگشت true; /* works! */
}

bool path_below_api_vfs(ثابت نویسه *p) {
        أكد(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        بازگشت PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
