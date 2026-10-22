/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/mount.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "filesystems.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mountpoint-util.h"
#অন্তর্ভাব "nulstr-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(পূর্ণাঙ্ক err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        প্রতিজ্ঞা(err < 0);

        যদি (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                প্রত্যাগম false;
        যদি (ERRNO_IS_NEG_PRIVILEGE(err))
                প্রত্যাগম false;

        প্রত্যাগম !IN_SET(err, -EOVERFLOW, -EINVAL);
}

পূর্ণাঙ্ক name_to_handle_at_loop(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                সংরচনা file_handle **ret_handle,
                পূর্ণাঙ্ক *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                পূর্ণাঙ্ক flags) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);
        প্রতিজ্ঞা((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        যদি (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        প্রতি (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ সংরচনা file_handle *h = রিক্ত;

                h = malloc0(offsetof(সংরচনা file_handle, f_handle) + n);
                যদি (!h)
                        প্রত্যাগম -ENOMEM;

                h->handle_bytes = n;

                যদি (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (পূর্ণাঙ্ক *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        যদি (r >= 0) {
                                যদি (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                যদি (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                প্রত্যাগম 1;
                        }
                        যদি (errno == EOVERFLOW)
                                গচ্ছ grow;
                        যদি (errno != EINVAL)
                                প্রত্যাগম -errno;
                }

                পূর্ণাঙ্ক mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                যদি (r >= 0) {
                        যদি (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                সংরচনা statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                যদি (r >= 0) {
                                        যদি (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        যদি (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        প্রত্যাগম 1;
                                }
                                যদি (r != -EUNATCH || !ret_mnt_id)
                                        প্রত্যাগম r;

                                *ret_unique_mnt_id = 0;
                        }

                        যদি (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        যদি (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        প্রত্যাগম 0;
                }
                যদি (errno != EOVERFLOW)
                        প্রত্যাগম -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                যদি (h->handle_bytes <= n)
                        প্রত্যাগম -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                যদি (n > UINT_MAX - offsetof(সংরচনা file_handle, f_handle))
                        প্রত্যাগম -EOVERFLOW;
        }
}

পূর্ণাঙ্ক name_to_handle_at_try_fid(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                সংরচনা file_handle **ret_handle,
                পূর্ণাঙ্ক *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                পূর্ণাঙ্ক flags) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        যদি (r >= 0 || is_name_to_handle_at_fatal_error(r))
                প্রত্যাগম r;

        প্রত্যাগম name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

পূর্ণাঙ্ক name_to_handle_at_u64(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, uint64_t *ret) {
        _cleanup_free_ সংরচনা file_handle *h = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ রিক্ত, /* ret_unique_mnt_id= */ রিক্ত, /* flags= */ 0);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (h->handle_bytes < পরিমাণ(uint64_t))
                প্রত্যাগম -EBADMSG;

        যদি (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        প্রত্যাগম 0;
}

bool file_handle_equal(ধ্রুব সংরচনা file_handle *a, ধ্রুব সংরচনা file_handle *b) {
        যদি (a == b)
                প্রত্যাগম true;
        যদি (!a != !b)
                প্রত্যাগম false;
        যদি (a->handle_type != b->handle_type)
                প্রত্যাগম false;

        প্রত্যাগম memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

সংরচনা file_handle* file_handle_dup(ধ্রুব সংরচনা file_handle *fh) {
        _cleanup_free_ সংরচনা file_handle *fh_copy = রিক্ত;

        প্রতিজ্ঞা(fh);

        fh_copy = malloc0(offsetof(সংরচনা file_handle, f_handle) + fh->handle_bytes);
        যদি (!fh_copy)
                প্রত্যাগম রিক্ত;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        প্রত্যাগম TAKE_PTR(fh_copy);
}

পূর্ণাঙ্ক is_mount_point_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা((flags & ~AT_SYMLINK_FOLLOW) == 0);

        যদি (path_equal(path, "/"))
                প্রত্যাগম true;

        যদি (dir_fd == XAT_FDROOT && isempty(path))
                প্রত্যাগম true;

        সংরচনা statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_মূল,
                        &sx);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_মূল))
                প্রত্যাগম true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        সংরচনা statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
পূর্ণাঙ্ক path_is_mount_point_full(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *মূল, পূর্ণাঙ্ক flags) {
        _cleanup_close_ পূর্ণাঙ্ক dir_fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা((flags & ~AT_SYMLINK_FOLLOW) == 0);

        যদি (empty_or_মূল(মূল))
                প্রত্যাগম is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, মূল,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ রিক্ত, &dir_fd);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম is_mount_point_at(dir_fd, /* path= */ রিক্ত, flags);
}

স্থির পূর্ণাঙ্ক path_get_mnt_id_at_internal(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, bool unique, uint64_t *ret) {
        সংরচনা statx sx;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(dir_fd));
        প্রতিজ্ঞা(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        যদি (r < 0)
                প্রত্যাগম r;

        *ret = sx.stx_mnt_id;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক path_get_mnt_id_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক *ret) {
        uint64_t mnt_id;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতিজ্ঞা(mnt_id <= INT_MAX);
        *ret = (পূর্ণাঙ্ক) mnt_id;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক path_get_unique_mnt_id_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, uint64_t *ret) {
        প্রত্যাগম path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(ধ্রুব বর্ণ *fstype) {
        ধ্রুব বর্ণ *x;

        x = startswith(fstype, "fuse.");
        যদি (x)
                fstype = x;

        যদি (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].মান, fstype))
                প্রত্যাগম true;

        /* Filesystems not present in the internal database */
        প্রত্যাগম STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ধ্রুব বর্ণ *fstype) {
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
        প্রত্যাগম STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(ধ্রুব বর্ণ *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        প্রত্যাগম STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ধ্রুব বর্ণ *fstype) {
        প্রতিজ্ঞা(fstype);

        ধ্রুব FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                যদি (nulstr_contains(fs->মান, fstype))
                    প্রত্যাগম true;

        /* Filesystems not present in the internal database */
        প্রত্যাগম STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ধ্রুব বর্ণ *fstype) {
        ধ্রুব বর্ণ *x;

        x = startswith(fstype, "fuse.");
        যদি (x)
                fstype = x;

        প্রত্যাগম !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ধ্রুব বর্ণ *fstype) {
        /* All Linux file systems that are necessarily read-only */
        প্রত্যাগম STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ধ্রুব বর্ণ *fstype) {
        প্রতিজ্ঞা(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        যদি (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                প্রত্যাগম true;

        /* On new kernels we can just ask the kernel */
        প্রত্যাগম mount_option_supported(fstype, "discard", রিক্ত) > 0;
}

ধ্রুব বর্ণ* fstype_norecovery_option(ধ্রুব বর্ণ *fstype) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        যদি (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                প্রত্যাগম "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        যদি (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", রিক্ত);
                যদি (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        প্রত্যাগম "norecovery";
                }
                যদি (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                যদি (r > 0)
                        প্রত্যাগম "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        প্রত্যাগম mount_option_supported(fstype, "norecovery", রিক্ত) > 0 ? "norecovery" : রিক্ত;
}

bool fstype_can_fmask_dmask(ধ্রুব বর্ণ *fstype) {
        প্রতিজ্ঞা(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        প্রত্যাগম streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ধ্রুব বর্ণ *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        প্রত্যাগম !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ধ্রুব বর্ণ *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        প্রত্যাগম STR_IN_SET(fstype,
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

পূর্ণাঙ্ক dev_is_devtmpfs(শূন্য) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = রিক্ত;
        পূর্ণাঙ্ক mount_id, r;
        বর্ণ *e;

        r = path_get_mnt_id("/dev", &mount_id);
        যদি (r < 0)
                প্রত্যাগম r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        যদি (r == -ENOENT)
                প্রত্যাগম proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতি (;;) {
                _cleanup_free_ বর্ণ *সারি = রিক্ত;
                পূর্ণাঙ্ক mid;

                r = read_সারি(proc_self_mountinfo, LONG_সারি_MAX, &সারি);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                যদি (sscanf(সারি, "%i", &mid) != 1)
                        অনুবর্ত;

                যদি (mid != mount_id)
                        অনুবর্ত;

                e = strstrafter(সারি, " - ");
                যদি (!e)
                        অনুবর্ত;

                /* accept any name that starts with the currently expected type */
                যদি (startswith(e, "devtmpfs"))
                        প্রত্যাগম true;
        }

        প্রত্যাগম false;
}

স্থির পূর্ণাঙ্ক mount_fd(
                ধ্রুব বর্ণ *source,
                পূর্ণাঙ্ক target_fd,
                ধ্রুব বর্ণ *filesystemtype,
                unsigned long mountflags,
                ধ্রুব শূন্য *data) {

        প্রতিজ্ঞা(target_fd >= 0);

        যদি (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                যদি (errno != ENOENT)
                        প্রত্যাগম -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                যদি (proc_mounted() == 0)
                        প্রত্যাগম -ENOSYS;

                প্রত্যাগম -ENOENT;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক mount_nofollow(
                ধ্রুব বর্ণ *source,
                ধ্রুব বর্ণ *target,
                ধ্রুব বর্ণ *filesystemtype,
                unsigned long mountflags,
                ধ্রুব শূন্য *data) {

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম mount_fd(source, fd, filesystemtype, mountflags, data);
}

ধ্রুব বর্ণ* mount_propagation_flag_to_string(unsigned long flags) {

        বিকল্প (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        পক্ষ 0:
                প্রত্যাগম "";
        পক্ষ MS_SHARED:
                প্রত্যাগম "shared";
        পক্ষ MS_SLAVE:
                প্রত্যাগম "slave";
        পক্ষ MS_PRIVATE:
                প্রত্যাগম "private";
        }

        প্রত্যাগম রিক্ত;
}

পূর্ণাঙ্ক mount_propagation_flag_from_string(ধ্রুব বর্ণ *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        প্রতিজ্ঞা(ret);

        যদি (isempty(name))
                *ret = 0;
        অন্যথা যদি (streq(name, "shared"))
                *ret = MS_SHARED;
        অন্যথা যদি (streq(name, "slave"))
                *ret = MS_SLAVE;
        অন্যথা যদি (streq(name, "private"))
                *ret = MS_PRIVATE;
        অন্যথা
                প্রত্যাগম -EINVAL;
        প্রত্যাগম 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        প্রত্যাগম IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(শূন্য) {
        স্থির পূর্ণাঙ্ক cache = -1;
        পূর্ণাঙ্ক r;

        যদি (cache >= 0)
                প্রত্যাগম cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, রিক্ত, 0, রিক্ত, 0));
        যদি (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                প্রত্যাগম (cache = false);

        প্রত্যাগম (cache = true);
}

পূর্ণাঙ্ক mount_option_supported(ধ্রুব বর্ণ *fstype, ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        প্রতিজ্ঞা(fstype);
        প্রতিজ্ঞা(চাবি);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        যদি (fd < 0) {
                যদি (errno == ENODEV)
                        প্রত্যাগম log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                প্রত্যাগম log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        যদি (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", রিক্ত, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                যদি (errno == EOPNOTSUPP)
                        প্রত্যাগম -EAGAIN; /* fs not converted to new mount API → don't know */
                যদি (errno != EINVAL)
                        প্রত্যাগম log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } অন্যথা
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        যদি (মান)
                r = fsconfig(fd, FSCONFIG_SET_STRING, চাবি, মান, 0);
        অন্যথা
                r = fsconfig(fd, FSCONFIG_SET_FLAG, চাবি, রিক্ত, 0);
        যদি (r < 0) {
                যদি (errno == EINVAL)
                        প্রত্যাগম false; /* EINVAL means option not supported. */

                প্রত্যাগম log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       চাবি, মান ? "=" : "", strempty(মান), fstype);
        }

        প্রত্যাগম true; /* works! */
}

bool path_below_api_vfs(ধ্রুব বর্ণ *p) {
        প্রতিজ্ঞা(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        প্রত্যাগম PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
