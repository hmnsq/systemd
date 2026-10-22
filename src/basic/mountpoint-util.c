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
#ವ್ಯಾಖ್ಯಾನಿಸು ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_ಗಮ್ಯ_handle_at_fatal_error(ಪೂರ್ಣಾಂಕ err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        ಪ್ರತಿಜ್ಞಾ(err < 0);

        ಆದರೆ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (ERRNO_IS_NEG_PRIVILEGE(err))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ !ಒಳ_ಗಣ(err, -EOVERFLOW, -EINVAL);
}

ಪೂರ್ಣಾಂಕ name_ಗಮ್ಯ_handle_at_loop(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ರಚನೆ file_handle **ret_handle,
                ಪೂರ್ಣಾಂಕ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ಪೂರ್ಣಾಂಕ flags) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ((flags & ~(AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ|AT_HANDLE_FID)) == 0);

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

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                flags |= AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ;
                ಮಾರ್ಗ = "";
        }

        ಗಾಗಿ (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ ರಚನೆ file_handle *h = ಖಾಲಿ;

                h = malloc0(offsetof(ರಚನೆ file_handle, f_handle) + n);
                ಆದರೆ (!h)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                h->handle_ಬೈಟುಗಳು = n;

                ಆದರೆ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_ಗಮ್ಯ_handle_at(fd, ಮಾರ್ಗ, h, (ಪೂರ್ಣಾಂಕ *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ಆದರೆ (r >= 0) {
                                ಆದರೆ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ಆದರೆ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ಹಿಂತಿರುಗಿ 1;
                        }
                        ಆದರೆ (errno == EOVERFLOW)
                                ಹೋಗು grow;
                        ಆದರೆ (errno != EINVAL)
                                ಹಿಂತಿರುಗಿ -errno;
                }

                ಪೂರ್ಣಾಂಕ mnt_id;
                r = name_ಗಮ್ಯ_handle_at(fd, ಮಾರ್ಗ, h, &mnt_id, flags);
                ಆದರೆ (r >= 0) {
                        ಆದರೆ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                ರಚನೆ statx sx;
                                r = xstatx(fd, ಮಾರ್ಗ,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ಆದರೆ (r >= 0) {
                                        ಆದರೆ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ಆದರೆ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ಹಿಂತಿರುಗಿ 1;
                                }
                                ಆದರೆ (r != -EUNATCH || !ret_mnt_id)
                                        ಹಿಂತಿರುಗಿ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ಆದರೆ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ಆದರೆ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (errno != EOVERFLOW)
                        ಹಿಂತಿರುಗಿ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ಆದರೆ (h->handle_ಬೈಟುಗಳು <= n)
                        ಹಿಂತಿರುಗಿ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_ಬೈಟುಗಳು;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ಆದರೆ (n > UINT_MAX - offsetof(ರಚನೆ file_handle, f_handle))
                        ಹಿಂತಿರುಗಿ -EOVERFLOW;
        }
}

ಪೂರ್ಣಾಂಕ name_ಗಮ್ಯ_handle_at_try_fid(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ರಚನೆ file_handle **ret_handle,
                ಪೂರ್ಣಾಂಕ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ಪೂರ್ಣಾಂಕ flags) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_ಗಮ್ಯ_handle_at_loop(fd, ಮಾರ್ಗ, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ಆದರೆ (r >= 0 || is_name_ಗಮ್ಯ_handle_at_fatal_error(r))
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ name_ಗಮ್ಯ_handle_at_loop(fd, ಮಾರ್ಗ, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

ಪೂರ್ಣಾಂಕ name_ಗಮ್ಯ_handle_at_u64(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret) {
        _cleanup_free_ ರಚನೆ file_handle *h = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_ಗಮ್ಯ_handle_at_loop(fd, ಮಾರ್ಗ, &h, /* ret_mnt_id= */ ಖಾಲಿ, /* ret_unique_mnt_id= */ ಖಾಲಿ, /* flags= */ 0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (h->handle_ಬೈಟುಗಳು < ಗಾತ್ರ(uint64_t))
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ಹಿಂತಿರುಗಿ 0;
}

bool file_handle_equal(ಸ್ಥಿರಾಂಕ ರಚನೆ file_handle *a, ಸ್ಥಿರಾಂಕ ರಚನೆ file_handle *b) {
        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ true;
        ಆದರೆ (!a != !b)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (a->handle_type != b->handle_type)
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ memcmp_nn(a->f_handle, a->handle_ಬೈಟುಗಳು, b->f_handle, b->handle_ಬೈಟುಗಳು) == 0;
}

ರಚನೆ file_handle* file_handle_dup(ಸ್ಥಿರಾಂಕ ರಚನೆ file_handle *fh) {
        _cleanup_free_ ರಚನೆ file_handle *fh_copy = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(fh);

        fh_copy = malloc0(offsetof(ರಚನೆ file_handle, f_handle) + fh->handle_ಬೈಟುಗಳು);
        ಆದರೆ (!fh_copy)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        fh_copy->handle_ಬೈಟುಗಳು = fh->handle_ಬೈಟುಗಳು;
        fh_copy->handle_type = fh->handle_type;
        ಸ್ಮರಣೆ_ನಕಲು(fh_copy->f_handle, fh->f_handle, fh->handle_ಬೈಟುಗಳು);

        ಹಿಂತಿರುಗಿ TAKE_PTR(fh_copy);
}

ಪೂರ್ಣಾಂಕ is_mount_ಬಿಂದು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ಆದರೆ (ಮಾರ್ಗ_equal(ಮಾರ್ಗ, "/"))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (dir_fd == XAT_FDROOT && isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ true;

        ರಚನೆ statx sx;
        r = xstatx_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ಬೇರು,
                        &sx);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (FLAGS_ಗಣ(sx.stx_ಗುಣಲಕ್ಷಣಗಳು, STATX_ATTR_MOUNT_ಬೇರು))
                ಹಿಂತಿರುಗಿ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        ರಚನೆ statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_mount_ಬಿಂದು_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಪೂರ್ಣಾಂಕ flags) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ dir_fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ಆದರೆ (ಖಾಲಿಗಳು_or_ಬೇರು(ಬೇರು))
                ಹಿಂತಿರುಗಿ is_mount_ಬಿಂದು_at(AT_FDCWD, ಮಾರ್ಗ, flags);

        r = chase(ಮಾರ್ಗ, ಬೇರು,
                  FLAGS_ಗಣ(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ ಖಾಲಿ, &dir_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ is_mount_ಬಿಂದು_at(dir_fd, /* path= */ ಖಾಲಿ, flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_get_mnt_id_at_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool unique, uint64_t *ret) {
        ರಚನೆ statx sx;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(wildcard_fd_is_valid(dir_fd));
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = xstatx(dir_fd, ಮಾರ್ಗ,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = sx.stx_mnt_id;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_get_mnt_id_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ *ret) {
        uint64_t mnt_id;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ಮಾರ್ಗ_get_mnt_id_at_ಆಂತರಿಕ(dir_fd, ಮಾರ್ಗ, /* unique = */ false, &mnt_id);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(mnt_id <= ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ);
        *ret = (ಪೂರ್ಣಾಂಕ) mnt_id;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_get_unique_mnt_id_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, uint64_t *ret) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_get_mnt_id_at_ಆಂತರಿಕ(dir_fd, ಮಾರ್ಗ, /* unique = */ true, ret);
}

bool fstype_is_ಜಾಲ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x;

        x = startswith(fstype, "fuse.");
        ಆದರೆ (x)
                fstype = x;

        ಆದರೆ (nulstr_ಒಳಗೊಂಡಿದೆ(filesystem_sets[FILESYSTEM_ಗಣ_ಜಾಲ].ಮೌಲ್ಯ, fstype))
                ಹಿಂತಿರುಗಿ true;

        /* Filesystems not present in the internal database */
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
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
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_ಆಂತರಿಕ_quota(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಪ್ರತಿಜ್ಞಾ(fstype);

        ಸ್ಥಿರಾಂಕ Filesystem_ಗಣ *fs;
        FOREACH_ವಾದ(fs,
                         filesystem_sets + FILESYSTEM_ಗಣ_BASIC_API,
                         filesystem_sets + FILESYSTEM_ಗಣ_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_ಗಣ_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_ಗಣ_TEMPORARY)
                ಆದರೆ (nulstr_ಒಳಗೊಂಡಿದೆ(fs->ಮೌಲ್ಯ, fstype))
                    ಹಿಂತಿರುಗಿ true;

        /* Filesystems not present in the internal database */
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *x;

        x = startswith(fstype, "fuse.");
        ಆದರೆ (x)
                fstype = x;

        ಹಿಂತಿರುಗಿ !STR_ಒಳ_ಗಣ(fstype, "9p", "overlay") && !fstype_is_ಜಾಲ(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಪ್ರತಿಜ್ಞಾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ಆದರೆ (STR_ಒಳ_ಗಣ(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ಹಿಂತಿರುಗಿ true;

        /* On new kernels we can just ask the kernel */
        ಹಿಂತಿರುಗಿ mount_option_supported(fstype, "discard", ಖಾಲಿ) > 0;
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* fstype_norecovery_option(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ಆದರೆ (STR_ಒಳ_ಗಣ(fstype, "ext3", "ext4", "xfs"))
                ಹಿಂತಿರುಗಿ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ಆದರೆ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", ಖಾಲಿ);
                ಆದರೆ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ಹಿಂತಿರುಗಿ "norecovery";
                }
                ಆದರೆ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ಆದರೆ (r > 0)
                        ಹಿಂತಿರುಗಿ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ಹಿಂತಿರುಗಿ mount_option_supported(fstype, "norecovery", ಖಾಲಿ) > 0 ? "norecovery" : ಖಾಲಿ;
}

bool fstype_can_fmask_dmask(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        ಪ್ರತಿಜ್ಞಾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ಹಿಂತಿರುಗಿ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ಹಿಂತಿರುಗಿ !STR_ಒಳ_ಗಣ(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        ಹಿಂತಿರುಗಿ STR_ಒಳ_ಗಣ(fstype,
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

ಪೂರ್ಣಾಂಕ dev_is_devtmpfs(ಶೂನ್ಯ) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ mount_id, r;
        ಅಕ್ಷರ *e;

        r = ಮಾರ್ಗ_get_mnt_id("/dev", &mount_id);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
                ಪೂರ್ಣಾಂಕ mid;

                r = read_ಸಾಲು(proc_self_mountinfo, LONG_ಸಾಲು_MAX, &ಸಾಲು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(ಸಾಲು, "%i", &mid) != 1)
                        ಮುಂದುವರಿ;

                ಆದರೆ (mid != mount_id)
                        ಮುಂದುವರಿ;

                e = strstrafter(ಸಾಲು, " - ");
                ಆದರೆ (!e)
                        ಮುಂದುವರಿ;

                /* accept any name that starts with the currently expected type */
                ಆದರೆ (startswith(e, "devtmpfs"))
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mount_fd(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ,
                ಪೂರ್ಣಾಂಕ ಗುರಿ_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filesystemtype,
                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mountflags,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ) {

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ_fd >= 0);

        ಆದರೆ (mount(ಮೂಲ, FORMAT_PROC_FD_ಮಾರ್ಗ(ಗುರಿ_fd), filesystemtype, mountflags, ದತ್ತಾಂಶ) < 0) {
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ಆದರೆ (proc_mounted() == 0)
                        ಹಿಂತಿರುಗಿ -ENOSYS;

                ಹಿಂತಿರುಗಿ -ENOENT;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ mount_nofollow(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೂಲ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filesystemtype,
                ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ mountflags,
                ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(ಗುರಿ);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(ಗುರಿ, O_ಮಾರ್ಗ|O_CLOEXEC|O_NOFOLLOW);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ mount_fd(ಮೂಲ, fd, filesystemtype, mountflags, ದತ್ತಾಂಶ);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* mount_ಪ್ರಸರಣ_flag_ಗಮ್ಯ_string(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flags) {

        ಆಯ್ಕೆ (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        ಸ್ಥಿತಿ 0:
                ಹಿಂತಿರುಗಿ "";
        ಸ್ಥಿತಿ MS_SHARED:
                ಹಿಂತಿರುಗಿ "shared";
        ಸ್ಥಿತಿ MS_SLAVE:
                ಹಿಂತಿರುಗಿ "slave";
        ಸ್ಥಿತಿ MS_PRIVATE:
                ಹಿಂತಿರುಗಿ "private";
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಪೂರ್ಣಾಂಕ mount_ಪ್ರಸರಣ_flag_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ *ret) {

        ಸೂಚಕ_MAY_BE_NULL(name);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (isempty(name))
                *ret = 0;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(name, "shared"))
                *ret = MS_SHARED;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(name, "slave"))
                *ret = MS_SLAVE;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (streq(name, "private"))
                *ret = MS_PRIVATE;
        ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಹಿಂತಿರುಗಿ 0;
}

bool mount_ಪ್ರಸರಣ_flag_is_valid(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ flag) {
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cache = -1;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (cache >= 0)
                ಹಿಂತಿರುಗಿ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, ಖಾಲಿ, 0, ಖಾಲಿ, 0));
        ಆದರೆ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ಹಿಂತಿರುಗಿ (cache = false);

        ಹಿಂತಿರುಗಿ (cache = true);
}

ಪೂರ್ಣಾಂಕ mount_option_supported(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fstype, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        ಪ್ರತಿಜ್ಞಾ(fstype);
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ಆದರೆ (fd < 0) {
                ಆದರೆ (errno == ENODEV)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ಆದರೆ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", ಖಾಲಿ, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ಆದರೆ (errno == EOPNOTSUPP)
                        ಹಿಂತಿರುಗಿ -EAGAIN; /* fs not converted to new mount API → don't know */
                ಆದರೆ (errno != EINVAL)
                        ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ಆದರೆ (ಮೌಲ್ಯ)
                r = fsconfig(fd, FSCONFIG_SET_STRING, ಕೀಲಿ, ಮೌಲ್ಯ, 0);
        ಇಲ್ಲದಿದ್ದರೆ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, ಕೀಲಿ, ಖಾಲಿ, 0);
        ಆದರೆ (r < 0) {
                ಆದರೆ (errno == EINVAL)
                        ಹಿಂತಿರುಗಿ false; /* EINVAL means option not supported. */

                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       ಕೀಲಿ, ಮೌಲ್ಯ ? "=" : "", strempty(ಮೌಲ್ಯ), fstype);
        }

        ಹಿಂತಿರುಗಿ true; /* works! */
}

bool ಮಾರ್ಗ_ಕೆಳಗೆ_api_vfs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        ಪ್ರತಿಜ್ಞಾ(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_STARTSWITH_ಗಣ(p, "/dev", "/sys", "/proc");
}
