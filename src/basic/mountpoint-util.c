/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/mount.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "filesystems.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mountpoint-util.h"
#ಅನ್ತರ್ಭಾವ "nulstr-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(ಪೂರ್ಣಾಙ್ಕ err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        ಪ್ರತಿಜ್ಞಾ(err < 0);

        ಯದಿ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (ERRNO_IS_NEG_PRIVILEGE(err))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ !IN_SET(err, -EOVERFLOW, -EINVAL);
}

ಪೂರ್ಣಾಙ್ಕ name_to_handle_at_loop(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಸಂರಚನಾ file_handle **ret_handle,
                ಪೂರ್ಣಾಙ್ಕ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ಪೂರ್ಣಾಙ್ಕ flags) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        ಯದಿ (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        ಪ್ರತಿ (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ ಸಂರಚನಾ file_handle *h = ರಿಕ್ತ;

                h = malloc0(offsetof(ಸಂರಚನಾ file_handle, f_handle) + n);
                ಯದಿ (!h)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                h->handle_bytes = n;

                ಯದಿ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (ಪೂರ್ಣಾಙ್ಕ *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ಯದಿ (r >= 0) {
                                ಯದಿ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ಯದಿ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ಪ್ರತ್ಯಾಗಮ 1;
                        }
                        ಯದಿ (errno == EOVERFLOW)
                                ಗಚ್ಛ grow;
                        ಯದಿ (errno != EINVAL)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                }

                ಪೂರ್ಣಾಙ್ಕ mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                ಯದಿ (r >= 0) {
                        ಯದಿ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                ಸಂರಚನಾ statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ಯದಿ (r >= 0) {
                                        ಯದಿ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ಯದಿ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ಪ್ರತ್ಯಾಗಮ 1;
                                }
                                ಯದಿ (r != -EUNATCH || !ret_mnt_id)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ಯದಿ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ಯದಿ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (errno != EOVERFLOW)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ಯದಿ (h->handle_bytes <= n)
                        ಪ್ರತ್ಯಾಗಮ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ಯದಿ (n > UINT_MAX - offsetof(ಸಂರಚನಾ file_handle, f_handle))
                        ಪ್ರತ್ಯಾಗಮ -EOVERFLOW;
        }
}

ಪೂರ್ಣಾಙ್ಕ name_to_handle_at_try_fid(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಸಂರಚನಾ file_handle **ret_handle,
                ಪೂರ್ಣಾಙ್ಕ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ಪೂರ್ಣಾಙ್ಕ flags) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ಯದಿ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

ಪೂರ್ಣಾಙ್ಕ name_to_handle_at_u64(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, uint64_t *ret) {
        _cleanup_free_ ಸಂರಚನಾ file_handle *h = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ ರಿಕ್ತ, /* ret_unique_mnt_id= */ ರಿಕ್ತ, /* flags= */ 0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (h->handle_bytes < ಪರಿಮಾಣ(uint64_t))
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ಪ್ರತ್ಯಾಗಮ 0;
}

bool file_handle_equal(ಧ್ರುವ ಸಂರಚನಾ file_handle *a, ಧ್ರುವ ಸಂರಚನಾ file_handle *b) {
        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ true;
        ಯದಿ (!a != !b)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (a->handle_type != b->handle_type)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

ಸಂರಚನಾ file_handle* file_handle_dup(ಧ್ರುವ ಸಂರಚನಾ file_handle *fh) {
        _cleanup_free_ ಸಂರಚನಾ file_handle *fh_copy = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(fh);

        fh_copy = malloc0(offsetof(ಸಂರಚನಾ file_handle, f_handle) + fh->handle_bytes);
        ಯದಿ (!fh_copy)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        ಪ್ರತ್ಯಾಗಮ TAKE_PTR(fh_copy);
}

ಪೂರ್ಣಾಙ್ಕ is_mount_point_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ಯದಿ (path_equal(path, "/"))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (dir_fd == XAT_FDROOT && isempty(path))
                ಪ್ರತ್ಯಾಗಮ true;

        ಸಂರಚನಾ statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ಬೇರು,
                        &sx);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ಬೇರು))
                ಪ್ರತ್ಯಾಗಮ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        ಸಂರಚನಾ statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
ಪೂರ್ಣಾಙ್ಕ path_is_mount_point_full(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಪೂರ್ಣಾಙ್ಕ flags) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ dir_fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ಯದಿ (empty_or_ಬೇರು(ಬೇರು))
                ಪ್ರತ್ಯಾಗಮ is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, ಬೇರು,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ ರಿಕ್ತ, &dir_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ is_mount_point_at(dir_fd, /* path= */ ರಿಕ್ತ, flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ path_get_mnt_id_at_internal(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, bool unique, uint64_t *ret) {
        ಸಂರಚನಾ statx sx;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = sx.stx_mnt_id;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ path_get_mnt_id_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ *ret) {
        uint64_t mnt_id;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(mnt_id <= INT_MAX);
        *ret = (ಪೂರ್ಣಾಙ್ಕ) mnt_id;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ path_get_unique_mnt_id_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, uint64_t *ret) {
        ಪ್ರತ್ಯಾಗಮ path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಧ್ರುವ ವರ್ಣ *x;

        x = startswith(fstype, "fuse.");
        ಯದಿ (x)
                fstype = x;

        ಯದಿ (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].ಮೌಲ್ಯ, fstype))
                ಪ್ರತ್ಯಾಗಮ true;

        /* Filesystems not present in the internal database */
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ಧ್ರುವ ವರ್ಣ *fstype) {
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
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(ಧ್ರುವ ವರ್ಣ *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಪ್ರತಿಜ್ಞಾ(fstype);

        ಧ್ರುವ FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                ಯದಿ (nulstr_contains(fs->ಮೌಲ್ಯ, fstype))
                    ಪ್ರತ್ಯಾಗಮ true;

        /* Filesystems not present in the internal database */
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಧ್ರುವ ವರ್ಣ *x;

        x = startswith(fstype, "fuse.");
        ಯದಿ (x)
                fstype = x;

        ಪ್ರತ್ಯಾಗಮ !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ಧ್ರುವ ವರ್ಣ *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಪ್ರತಿಜ್ಞಾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ಯದಿ (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ಪ್ರತ್ಯಾಗಮ true;

        /* On new kernels we can just ask the kernel */
        ಪ್ರತ್ಯಾಗಮ mount_option_supported(fstype, "discard", ರಿಕ್ತ) > 0;
}

ಧ್ರುವ ವರ್ಣ* fstype_norecovery_option(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ಯದಿ (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                ಪ್ರತ್ಯಾಗಮ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ಯದಿ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", ರಿಕ್ತ);
                ಯದಿ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ಪ್ರತ್ಯಾಗಮ "norecovery";
                }
                ಯದಿ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ಯದಿ (r > 0)
                        ಪ್ರತ್ಯಾಗಮ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ಪ್ರತ್ಯಾಗಮ mount_option_supported(fstype, "norecovery", ರಿಕ್ತ) > 0 ? "norecovery" : ರಿಕ್ತ;
}

bool fstype_can_fmask_dmask(ಧ್ರುವ ವರ್ಣ *fstype) {
        ಪ್ರತಿಜ್ಞಾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ಪ್ರತ್ಯಾಗಮ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ಧ್ರುವ ವರ್ಣ *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ಪ್ರತ್ಯಾಗಮ !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ಧ್ರುವ ವರ್ಣ *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        ಪ್ರತ್ಯಾಗಮ STR_IN_SET(fstype,
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

ಪೂರ್ಣಾಙ್ಕ dev_is_devtmpfs(ಶೂನ್ಯ) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ mount_id, r;
        ವರ್ಣ *e;

        r = path_get_mnt_id("/dev", &mount_id);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
                ಪೂರ್ಣಾಙ್ಕ mid;

                r = read_ಸಾಲು(proc_self_mountinfo, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (sscanf(ಸಾಲು, "%i", &mid) != 1)
                        ಅನುವರ್ತ;

                ಯದಿ (mid != mount_id)
                        ಅನುವರ್ತ;

                e = strstrafter(ಸಾಲು, " - ");
                ಯದಿ (!e)
                        ಅನುವರ್ತ;

                /* accept any name that starts with the currently expected type */
                ಯದಿ (startswith(e, "devtmpfs"))
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mount_fd(
                ಧ್ರುವ ವರ್ಣ *source,
                ಪೂರ್ಣಾಙ್ಕ target_fd,
                ಧ್ರುವ ವರ್ಣ *filesystemtype,
                unsigned long mountflags,
                ಧ್ರುವ ಶೂನ್ಯ *data) {

        ಪ್ರತಿಜ್ಞಾ(target_fd >= 0);

        ಯದಿ (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ಯದಿ (proc_mounted() == 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOSYS;

                ಪ್ರತ್ಯಾಗಮ -ENOENT;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ mount_nofollow(
                ಧ್ರುವ ವರ್ಣ *source,
                ಧ್ರುವ ವರ್ಣ *target,
                ಧ್ರುವ ವರ್ಣ *filesystemtype,
                unsigned long mountflags,
                ಧ್ರುವ ಶೂನ್ಯ *data) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ mount_fd(source, fd, filesystemtype, mountflags, data);
}

ಧ್ರುವ ವರ್ಣ* mount_propagation_flag_to_string(unsigned long flags) {

        ವಿಕಲ್ಪ (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        ಪಕ್ಷ 0:
                ಪ್ರತ್ಯಾಗಮ "";
        ಪಕ್ಷ MS_SHARED:
                ಪ್ರತ್ಯಾಗಮ "shared";
        ಪಕ್ಷ MS_SLAVE:
                ಪ್ರತ್ಯಾಗಮ "slave";
        ಪಕ್ಷ MS_PRIVATE:
                ಪ್ರತ್ಯಾಗಮ "private";
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಪೂರ್ಣಾಙ್ಕ mount_propagation_flag_from_string(ಧ್ರುವ ವರ್ಣ *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (isempty(name))
                *ret = 0;
        ಅನ್ಯಥಾ ಯದಿ (streq(name, "shared"))
                *ret = MS_SHARED;
        ಅನ್ಯಥಾ ಯದಿ (streq(name, "slave"))
                *ret = MS_SLAVE;
        ಅನ್ಯಥಾ ಯದಿ (streq(name, "private"))
                *ret = MS_PRIVATE;
        ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        ಪ್ರತ್ಯಾಗಮ IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cache = -1;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (cache >= 0)
                ಪ್ರತ್ಯಾಗಮ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, ರಿಕ್ತ, 0, ರಿಕ್ತ, 0));
        ಯದಿ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ಪ್ರತ್ಯಾಗಮ (cache = false);

        ಪ್ರತ್ಯಾಗಮ (cache = true);
}

ಪೂರ್ಣಾಙ್ಕ mount_option_supported(ಧ್ರುವ ವರ್ಣ *fstype, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        ಪ್ರತಿಜ್ಞಾ(fstype);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ಯದಿ (fd < 0) {
                ಯದಿ (errno == ENODEV)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ಯದಿ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", ರಿಕ್ತ, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ಯದಿ (errno == EOPNOTSUPP)
                        ಪ್ರತ್ಯಾಗಮ -EAGAIN; /* fs not converted to new mount API → don't know */
                ಯದಿ (errno != EINVAL)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ಯದಿ (ಮೌಲ್ಯ)
                r = fsconfig(fd, FSCONFIG_SET_STRING, ಕೀಲಿ, ಮೌಲ್ಯ, 0);
        ಅನ್ಯಥಾ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, ಕೀಲಿ, ರಿಕ್ತ, 0);
        ಯದಿ (r < 0) {
                ಯದಿ (errno == EINVAL)
                        ಪ್ರತ್ಯಾಗಮ false; /* EINVAL means option not supported. */

                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       ಕೀಲಿ, ಮೌಲ್ಯ ? "=" : "", strempty(ಮೌಲ್ಯ), fstype);
        }

        ಪ್ರತ್ಯಾಗಮ true; /* works! */
}

bool path_below_api_vfs(ಧ್ರುವ ವರ್ಣ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ಪ್ರತ್ಯಾಗಮ PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
