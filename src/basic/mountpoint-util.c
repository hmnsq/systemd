/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <sys/mount.h>

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "filesystems.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "mountpoint-util.h"
#ضمن "nulstr-util.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(صحيح err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        أكد(err < 0);

        ܐܢ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ܦܢܐ false;
        ܐܢ (ERRNO_IS_NEG_PRIVILEGE(err))
                ܦܢܐ false;

        ܦܢܐ !IN_SET(err, -EOVERFLOW, -EINVAL);
}

صحيح name_to_handle_at_loop(
                صحيح fd,
                ثابت حرف *path,
                بنية file_handle **ret_handle,
                صحيح *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                صحيح flags) {

        صحيح r;

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

        ܐܢ (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        لكل (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ بنية file_handle *h = عدم;

                h = malloc0(offsetof(بنية file_handle, f_handle) + n);
                ܐܢ (!h)
                        ܦܢܐ -ENOMEM;

                h->handle_bytes = n;

                ܐܢ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (صحيح *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ܐܢ (r >= 0) {
                                ܐܢ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ܐܢ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ܦܢܐ 1;
                        }
                        ܐܢ (errno == EOVERFLOW)
                                اذهب grow;
                        ܐܢ (errno != EINVAL)
                                ܦܢܐ -errno;
                }

                صحيح mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                ܐܢ (r >= 0) {
                        ܐܢ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                بنية statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ܐܢ (r >= 0) {
                                        ܐܢ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ܐܢ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ܦܢܐ 1;
                                }
                                ܐܢ (r != -EUNATCH || !ret_mnt_id)
                                        ܦܢܐ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ܐܢ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ܐܢ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ܦܢܐ 0;
                }
                ܐܢ (errno != EOVERFLOW)
                        ܦܢܐ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ܐܢ (h->handle_bytes <= n)
                        ܦܢܐ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ܐܢ (n > UINT_MAX - offsetof(بنية file_handle, f_handle))
                        ܦܢܐ -EOVERFLOW;
        }
}

صحيح name_to_handle_at_try_fid(
                صحيح fd,
                ثابت حرف *path,
                بنية file_handle **ret_handle,
                صحيح *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                صحيح flags) {

        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ܐܢ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                ܦܢܐ r;

        ܦܢܐ name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

صحيح name_to_handle_at_u64(صحيح fd, ثابت حرف *path, uint64_t *ret) {
        _cleanup_free_ بنية file_handle *h = عدم;
        صحيح r;

        أكد(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ عدم, /* ret_unique_mnt_id= */ عدم, /* flags= */ 0);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (h->handle_bytes < حجم(uint64_t))
                ܦܢܐ -EBADMSG;

        ܐܢ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ܦܢܐ 0;
}

bool file_handle_equal(ثابت بنية file_handle *a, ثابت بنية file_handle *b) {
        ܐܢ (a == b)
                ܦܢܐ true;
        ܐܢ (!a != !b)
                ܦܢܐ false;
        ܐܢ (a->handle_type != b->handle_type)
                ܦܢܐ false;

        ܦܢܐ memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

بنية file_handle* file_handle_dup(ثابت بنية file_handle *fh) {
        _cleanup_free_ بنية file_handle *fh_copy = عدم;

        أكد(fh);

        fh_copy = malloc0(offsetof(بنية file_handle, f_handle) + fh->handle_bytes);
        ܐܢ (!fh_copy)
                ܦܢܐ عدم;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        ܦܢܐ TAKE_PTR(fh_copy);
}

صحيح is_mount_point_at(صحيح dir_fd, ثابت حرف *path, صحيح flags) {
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ܐܢ (path_equal(path, "/"))
                ܦܢܐ true;

        ܐܢ (dir_fd == XAT_FDROOT && isempty(path))
                ܦܢܐ true;

        بنية statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ROOT,
                        &sx);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT))
                ܦܢܐ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        بنية statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
صحيح path_is_mount_point_full(ثابت حرف *path, ثابت حرف *root, صحيح flags) {
        _cleanup_close_ صحيح dir_fd = -EBADF;
        صحيح r;

        أكد(path);
        أكد((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ܐܢ (empty_or_root(root))
                ܦܢܐ is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, root,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ عدم, &dir_fd);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ is_mount_point_at(dir_fd, /* path= */ عدم, flags);
}

ساكن صحيح path_get_mnt_id_at_internal(صحيح dir_fd, ثابت حرف *path, bool unique, uint64_t *ret) {
        بنية statx sx;
        صحيح r;

        أكد(wildcard_fd_is_valid(dir_fd));
        أكد(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ܐܢ (r < 0)
                ܦܢܐ r;

        *ret = sx.stx_mnt_id;
        ܦܢܐ 0;
}

صحيح path_get_mnt_id_at(صحيح dir_fd, ثابت حرف *path, صحيح *ret) {
        uint64_t mnt_id;
        صحيح r;

        أكد(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        ܐܢ (r < 0)
                ܦܢܐ r;

        أكد(mnt_id <= INT_MAX);
        *ret = (صحيح) mnt_id;
        ܦܢܐ 0;
}

صحيح path_get_unique_mnt_id_at(صحيح dir_fd, ثابت حرف *path, uint64_t *ret) {
        ܦܢܐ path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(ثابت حرف *fstype) {
        ثابت حرف *x;

        x = startswith(fstype, "fuse.");
        ܐܢ (x)
                fstype = x;

        ܐܢ (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].value, fstype))
                ܦܢܐ true;

        /* Filesystems not present in the internal database */
        ܦܢܐ STR_IN_SET(fstype,
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
        ܦܢܐ STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(ثابت حرف *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        ܦܢܐ STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ثابت حرف *fstype) {
        أكد(fstype);

        ثابت FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                ܐܢ (nulstr_contains(fs->value, fstype))
                    ܦܢܐ true;

        /* Filesystems not present in the internal database */
        ܦܢܐ STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ثابت حرف *fstype) {
        ثابت حرف *x;

        x = startswith(fstype, "fuse.");
        ܐܢ (x)
                fstype = x;

        ܦܢܐ !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ثابت حرف *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ܦܢܐ STR_IN_SET(fstype,
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
        ܐܢ (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ܦܢܐ true;

        /* On new kernels we can just ask the kernel */
        ܦܢܐ mount_option_supported(fstype, "discard", عدم) > 0;
}

ثابت حرف* fstype_norecovery_option(ثابت حرف *fstype) {
        صحيح r;

        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ܐܢ (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                ܦܢܐ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ܐܢ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", عدم);
                ܐܢ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ܦܢܐ "norecovery";
                }
                ܐܢ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ܐܢ (r > 0)
                        ܦܢܐ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ܦܢܐ mount_option_supported(fstype, "norecovery", عدم) > 0 ? "norecovery" : عدم;
}

bool fstype_can_fmask_dmask(ثابت حرف *fstype) {
        أكد(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ܦܢܐ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ثابت حرف *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ܦܢܐ !STR_IN_SET(ASSERT_PTR(fstype),
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

        ܦܢܐ STR_IN_SET(fstype,
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

صحيح dev_is_devtmpfs(فراغ) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = عدم;
        صحيح mount_id, r;
        حرف *e;

        r = path_get_mnt_id("/dev", &mount_id);
        ܐܢ (r < 0)
                ܦܢܐ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ܐܢ (r == -ENOENT)
                ܦܢܐ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ܐܢ (r < 0)
                ܦܢܐ r;

        لكل (;;) {
                _cleanup_free_ حرف *line = عدم;
                صحيح mid;

                r = read_line(proc_self_mountinfo, LONG_LINE_MAX, &line);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        اكسر;

                ܐܢ (sscanf(line, "%i", &mid) != 1)
                        استمر;

                ܐܢ (mid != mount_id)
                        استمر;

                e = strstrafter(line, " - ");
                ܐܢ (!e)
                        استمر;

                /* accept any name that starts with the currently expected type */
                ܐܢ (startswith(e, "devtmpfs"))
                        ܦܢܐ true;
        }

        ܦܢܐ false;
}

ساكن صحيح mount_fd(
                ثابت حرف *source,
                صحيح target_fd,
                ثابت حرف *filesystemtype,
                unsigned long mountflags,
                ثابت فراغ *data) {

        أكد(target_fd >= 0);

        ܐܢ (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                ܐܢ (errno != ENOENT)
                        ܦܢܐ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ܐܢ (proc_mounted() == 0)
                        ܦܢܐ -ENOSYS;

                ܦܢܐ -ENOENT;
        }

        ܦܢܐ 0;
}

صحيح mount_nofollow(
                ثابت حرف *source,
                ثابت حرف *target,
                ثابت حرف *filesystemtype,
                unsigned long mountflags,
                ثابت فراغ *data) {

        _cleanup_close_ صحيح fd = -EBADF;

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
        ܐܢ (fd < 0)
                ܦܢܐ -errno;

        ܦܢܐ mount_fd(source, fd, filesystemtype, mountflags, data);
}

ثابت حرف* mount_propagation_flag_to_string(unsigned long flags) {

        بدل (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        حالة 0:
                ܦܢܐ "";
        حالة MS_SHARED:
                ܦܢܐ "shared";
        حالة MS_SLAVE:
                ܦܢܐ "slave";
        حالة MS_PRIVATE:
                ܦܢܐ "private";
        }

        ܦܢܐ عدم;
}

صحيح mount_propagation_flag_from_string(ثابت حرف *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        أكد(ret);

        ܐܢ (isempty(name))
                *ret = 0;
        ܐܚܪܢܐ ܐܢ (streq(name, "shared"))
                *ret = MS_SHARED;
        ܐܚܪܢܐ ܐܢ (streq(name, "slave"))
                *ret = MS_SLAVE;
        ܐܚܪܢܐ ܐܢ (streq(name, "private"))
                *ret = MS_PRIVATE;
        ܐܚܪܢܐ
                ܦܢܐ -EINVAL;
        ܦܢܐ 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        ܦܢܐ IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(فراغ) {
        ساكن صحيح cache = -1;
        صحيح r;

        ܐܢ (cache >= 0)
                ܦܢܐ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, عدم, 0, عدم, 0));
        ܐܢ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ܦܢܐ (cache = false);

        ܦܢܐ (cache = true);
}

صحيح mount_option_supported(ثابت حرف *fstype, ثابت حرف *key, ثابت حرف *value) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        أكد(fstype);
        أكد(key);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ܐܢ (fd < 0) {
                ܐܢ (errno == ENODEV)
                        ܦܢܐ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ܦܢܐ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ܐܢ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", عدم, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ܐܢ (errno == EOPNOTSUPP)
                        ܦܢܐ -EAGAIN; /* fs not converted to new mount API → don't know */
                ܐܢ (errno != EINVAL)
                        ܦܢܐ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ܐܚܪܢܐ
                ܦܢܐ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ܐܢ (value)
                r = fsconfig(fd, FSCONFIG_SET_STRING, key, value, 0);
        ܐܚܪܢܐ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, key, عدم, 0);
        ܐܢ (r < 0) {
                ܐܢ (errno == EINVAL)
                        ܦܢܐ false; /* EINVAL means option not supported. */

                ܦܢܐ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       key, value ? "=" : "", strempty(value), fstype);
        }

        ܦܢܐ true; /* works! */
}

bool path_below_api_vfs(ثابت حرف *p) {
        أكد(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ܦܢܐ PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
