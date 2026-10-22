/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mount.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "filesystems.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mountpoint-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "nulstr-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_ਮੰਜ਼ਿਲ_handle_at_fatal_error(ਪੂਰਨ err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        ਪ੍ਰਤਿਜ੍ਞਾ(err < 0);

        ਜੇ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ਵਾਪਸ false;
        ਜੇ (ERRNO_IS_NEG_PRIVILEGE(err))
                ਵਾਪਸ false;

        ਵਾਪਸ !ਵਿੱਚ_ਸੈੱਟ(err, -EOVERFLOW, -EINVAL);
}

ਪੂਰਨ name_ਮੰਜ਼ਿਲ_handle_at_loop(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਬਣਤਰ file_handle **ret_handle,
                ਪੂਰਨ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ਪੂਰਨ flags) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~(AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ|AT_HANDLE_FID)) == 0);

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

        ਜੇ (isempty(ਰਸਤਾ)) {
                flags |= AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ;
                ਰਸਤਾ = "";
        }

        ਲਈ (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ ਬਣਤਰ file_handle *h = ਨਿਰਾ;

                h = malloc0(offsetof(ਬਣਤਰ file_handle, f_handle) + n);
                ਜੇ (!h)
                        ਵਾਪਸ -ENOMEM;

                h->handle_ਬਾਈਟਾਂ = n;

                ਜੇ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_ਮੰਜ਼ਿਲ_handle_at(fd, ਰਸਤਾ, h, (ਪੂਰਨ *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ਜੇ (r >= 0) {
                                ਜੇ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ਜੇ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ਵਾਪਸ 1;
                        }
                        ਜੇ (errno == EOVERFLOW)
                                ਜਾਓ grow;
                        ਜੇ (errno != EINVAL)
                                ਵਾਪਸ -errno;
                }

                ਪੂਰਨ mnt_id;
                r = name_ਮੰਜ਼ਿਲ_handle_at(fd, ਰਸਤਾ, h, &mnt_id, flags);
                ਜੇ (r >= 0) {
                        ਜੇ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                ਬਣਤਰ statx sx;
                                r = xstatx(fd, ਰਸਤਾ,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ਜੇ (r >= 0) {
                                        ਜੇ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ਜੇ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ਵਾਪਸ 1;
                                }
                                ਜੇ (r != -EUNATCH || !ret_mnt_id)
                                        ਵਾਪਸ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ਜੇ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ਜੇ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ਵਾਪਸ 0;
                }
                ਜੇ (errno != EOVERFLOW)
                        ਵਾਪਸ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ਜੇ (h->handle_ਬਾਈਟਾਂ <= n)
                        ਵਾਪਸ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_ਬਾਈਟਾਂ;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ਜੇ (n > UINT_MAX - offsetof(ਬਣਤਰ file_handle, f_handle))
                        ਵਾਪਸ -EOVERFLOW;
        }
}

ਪੂਰਨ name_ਮੰਜ਼ਿਲ_handle_at_try_fid(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਬਣਤਰ file_handle **ret_handle,
                ਪੂਰਨ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ਪੂਰਨ flags) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_ਮੰਜ਼ਿਲ_handle_at_loop(fd, ਰਸਤਾ, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ਜੇ (r >= 0 || is_name_ਮੰਜ਼ਿਲ_handle_at_fatal_error(r))
                ਵਾਪਸ r;

        ਵਾਪਸ name_ਮੰਜ਼ਿਲ_handle_at_loop(fd, ਰਸਤਾ, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

ਪੂਰਨ name_ਮੰਜ਼ਿਲ_handle_at_u64(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret) {
        _cleanup_free_ ਬਣਤਰ file_handle *h = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_ਮੰਜ਼ਿਲ_handle_at_loop(fd, ਰਸਤਾ, &h, /* ret_mnt_id= */ ਨਿਰਾ, /* ret_unique_mnt_id= */ ਨਿਰਾ, /* flags= */ 0);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (h->handle_ਬਾਈਟਾਂ < ਆਕਾਰ(uint64_t))
                ਵਾਪਸ -EBADMSG;

        ਜੇ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ਵਾਪਸ 0;
}

bool file_handle_equal(ਅਚਲ ਬਣਤਰ file_handle *a, ਅਚਲ ਬਣਤਰ file_handle *b) {
        ਜੇ (a == b)
                ਵਾਪਸ true;
        ਜੇ (!a != !b)
                ਵਾਪਸ false;
        ਜੇ (a->handle_type != b->handle_type)
                ਵਾਪਸ false;

        ਵਾਪਸ memcmp_nn(a->f_handle, a->handle_ਬਾਈਟਾਂ, b->f_handle, b->handle_ਬਾਈਟਾਂ) == 0;
}

ਬਣਤਰ file_handle* file_handle_dup(ਅਚਲ ਬਣਤਰ file_handle *fh) {
        _cleanup_free_ ਬਣਤਰ file_handle *fh_copy = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(fh);

        fh_copy = malloc0(offsetof(ਬਣਤਰ file_handle, f_handle) + fh->handle_ਬਾਈਟਾਂ);
        ਜੇ (!fh_copy)
                ਵਾਪਸ ਨਿਰਾ;

        fh_copy->handle_ਬਾਈਟਾਂ = fh->handle_ਬਾਈਟਾਂ;
        fh_copy->handle_type = fh->handle_type;
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(fh_copy->f_handle, fh->f_handle, fh->handle_ਬਾਈਟਾਂ);

        ਵਾਪਸ TAKE_PTR(fh_copy);
}

ਪੂਰਨ is_mount_ਬਿੰਦੂ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ਜੇ (ਰਸਤਾ_equal(ਰਸਤਾ, "/"))
                ਵਾਪਸ true;

        ਜੇ (dir_fd == XAT_FDROOT && isempty(ਰਸਤਾ))
                ਵਾਪਸ true;

        ਬਣਤਰ statx sx;
        r = xstatx_ਭਰਿਆ(dir_fd, ਰਸਤਾ,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ਜੜ੍ਹ,
                        &sx);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (FLAGS_ਸੈੱਟ(sx.stx_ਗੁਣ, STATX_ATTR_MOUNT_ਜੜ੍ਹ))
                ਵਾਪਸ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        ਬਣਤਰ statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
ਪੂਰਨ ਰਸਤਾ_is_mount_ਬਿੰਦੂ_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਪੂਰਨ flags) {
        _cleanup_close_ ਪੂਰਨ dir_fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਜੜ੍ਹ))
                ਵਾਪਸ is_mount_ਬਿੰਦੂ_at(AT_FDCWD, ਰਸਤਾ, flags);

        r = chase(ਰਸਤਾ, ਜੜ੍ਹ,
                  FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ ਨਿਰਾ, &dir_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ is_mount_ਬਿੰਦੂ_at(dir_fd, /* path= */ ਨਿਰਾ, flags);
}

ਸਥਿਰ ਪੂਰਨ ਰਸਤਾ_get_mnt_id_at_ਅੰਦਰੂਨੀ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool unique, uint64_t *ret) {
        ਬਣਤਰ statx sx;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(wildcard_fd_is_valid(dir_fd));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = xstatx(dir_fd, ਰਸਤਾ,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = sx.stx_mnt_id;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਰਸਤਾ_get_mnt_id_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ *ret) {
        uint64_t mnt_id;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ਰਸਤਾ_get_mnt_id_at_ਅੰਦਰੂਨੀ(dir_fd, ਰਸਤਾ, /* unique = */ false, &mnt_id);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(mnt_id <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ);
        *ret = (ਪੂਰਨ) mnt_id;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਰਸਤਾ_get_unique_mnt_id_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, uint64_t *ret) {
        ਵਾਪਸ ਰਸਤਾ_get_mnt_id_at_ਅੰਦਰੂਨੀ(dir_fd, ਰਸਤਾ, /* unique = */ true, ret);
}

bool fstype_is_ਜਾਲ(ਅਚਲ ਅੱਖਰ *fstype) {
        ਅਚਲ ਅੱਖਰ *x;

        x = startswith(fstype, "fuse.");
        ਜੇ (x)
                fstype = x;

        ਜੇ (nulstr_ਸ਼ਾਮਲ_ਹੈ(filesystem_sets[FILESYSTEM_ਸੈੱਟ_ਜਾਲ].ਮੁੱਲ, fstype))
                ਵਾਪਸ true;

        /* Filesystems not present in the internal database */
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ਅਚਲ ਅੱਖਰ *fstype) {
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
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_ਅੰਦਰੂਨੀ_quota(ਅਚਲ ਅੱਖਰ *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ਅਚਲ ਅੱਖਰ *fstype) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fstype);

        ਅਚਲ Filesystem_ਸੈੱਟ *fs;
        FOREACH_ਦਲੀਲ(fs,
                         filesystem_sets + FILESYSTEM_ਸੈੱਟ_BASIC_API,
                         filesystem_sets + FILESYSTEM_ਸੈੱਟ_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_ਸੈੱਟ_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_ਸੈੱਟ_TEMPORARY)
                ਜੇ (nulstr_ਸ਼ਾਮਲ_ਹੈ(fs->ਮੁੱਲ, fstype))
                    ਵਾਪਸ true;

        /* Filesystems not present in the internal database */
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ਅਚਲ ਅੱਖਰ *fstype) {
        ਅਚਲ ਅੱਖਰ *x;

        x = startswith(fstype, "fuse.");
        ਜੇ (x)
                fstype = x;

        ਵਾਪਸ !STR_ਵਿੱਚ_ਸੈੱਟ(fstype, "9p", "overlay") && !fstype_is_ਜਾਲ(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ਅਚਲ ਅੱਖਰ *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ਅਚਲ ਅੱਖਰ *fstype) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ਵਾਪਸ true;

        /* On new kernels we can just ask the kernel */
        ਵਾਪਸ mount_option_supported(fstype, "discard", ਨਿਰਾ) > 0;
}

ਅਚਲ ਅੱਖਰ* fstype_norecovery_option(ਅਚਲ ਅੱਖਰ *fstype) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(fstype, "ext3", "ext4", "xfs"))
                ਵਾਪਸ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ਜੇ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", ਨਿਰਾ);
                ਜੇ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ਵਾਪਸ "norecovery";
                }
                ਜੇ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ਜੇ (r > 0)
                        ਵਾਪਸ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ਵਾਪਸ mount_option_supported(fstype, "norecovery", ਨਿਰਾ) > 0 ? "norecovery" : ਨਿਰਾ;
}

bool fstype_can_fmask_dmask(ਅਚਲ ਅੱਖਰ *fstype) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ਵਾਪਸ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ਅਚਲ ਅੱਖਰ *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ਵਾਪਸ !STR_ਵਿੱਚ_ਸੈੱਟ(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ਅਚਲ ਅੱਖਰ *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(fstype,
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

ਪੂਰਨ dev_is_devtmpfs(ਖਾਲੀ) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = ਨਿਰਾ;
        ਪੂਰਨ mount_id, r;
        ਅੱਖਰ *e;

        r = ਰਸਤਾ_get_mnt_id("/dev", &mount_id);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
                ਪੂਰਨ mid;

                r = read_ਸਤਰ(proc_self_mountinfo, LONG_ਸਤਰ_MAX, &ਸਤਰ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ਸਤਰ, "%i", &mid) != 1)
                        ਜਾਰੀ;

                ਜੇ (mid != mount_id)
                        ਜਾਰੀ;

                e = strstrafter(ਸਤਰ, " - ");
                ਜੇ (!e)
                        ਜਾਰੀ;

                /* accept any name that starts with the currently expected type */
                ਜੇ (startswith(e, "devtmpfs"))
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਸਥਿਰ ਪੂਰਨ mount_fd(
                ਅਚਲ ਅੱਖਰ *ਸਰੋਤ,
                ਪੂਰਨ ਨਿਸ਼ਾਨਾ_fd,
                ਅਚਲ ਅੱਖਰ *filesystemtype,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mountflags,
                ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ) {

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ_fd >= 0);

        ਜੇ (mount(ਸਰੋਤ, FORMAT_PROC_FD_ਰਸਤਾ(ਨਿਸ਼ਾਨਾ_fd), filesystemtype, mountflags, ਅੰਕੜੇ) < 0) {
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ਜੇ (proc_mounted() == 0)
                        ਵਾਪਸ -ENOSYS;

                ਵਾਪਸ -ENOENT;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ mount_nofollow(
                ਅਚਲ ਅੱਖਰ *ਸਰੋਤ,
                ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ,
                ਅਚਲ ਅੱਖਰ *filesystemtype,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ mountflags,
                ਅਚਲ ਖਾਲੀ *ਅੰਕੜੇ) {

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(ਨਿਸ਼ਾਨਾ, O_ਰਸਤਾ|O_CLOEXEC|O_NOFOLLOW);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ mount_fd(ਸਰੋਤ, fd, filesystemtype, mountflags, ਅੰਕੜੇ);
}

ਅਚਲ ਅੱਖਰ* mount_ਪ੍ਰਸਾਰ_flag_ਮੰਜ਼ਿਲ_string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flags) {

        ਚੋਣ (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        ਹਾਲਤ 0:
                ਵਾਪਸ "";
        ਹਾਲਤ MS_SHARED:
                ਵਾਪਸ "shared";
        ਹਾਲਤ MS_SLAVE:
                ਵਾਪਸ "slave";
        ਹਾਲਤ MS_PRIVATE:
                ਵਾਪਸ "private";
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ mount_ਪ੍ਰਸਾਰ_flag_from_string(ਅਚਲ ਅੱਖਰ *name, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ *ret) {

        ਸੂਚਕ_MAY_BE_NULL(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (isempty(name))
                *ret = 0;
        ਨਹੀਂਤਾਂ ਜੇ (streq(name, "shared"))
                *ret = MS_SHARED;
        ਨਹੀਂਤਾਂ ਜੇ (streq(name, "slave"))
                *ret = MS_SLAVE;
        ਨਹੀਂਤਾਂ ਜੇ (streq(name, "private"))
                *ret = MS_PRIVATE;
        ਨਹੀਂਤਾਂ
                ਵਾਪਸ -EINVAL;
        ਵਾਪਸ 0;
}

bool mount_ਪ੍ਰਸਾਰ_flag_is_valid(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ flag) {
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ cache = -1;
        ਪੂਰਨ r;

        ਜੇ (cache >= 0)
                ਵਾਪਸ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, ਨਿਰਾ, 0, ਨਿਰਾ, 0));
        ਜੇ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ਵਾਪਸ (cache = false);

        ਵਾਪਸ (cache = true);
}

ਪੂਰਨ mount_option_supported(ਅਚਲ ਅੱਖਰ *fstype, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        ਪ੍ਰਤਿਜ੍ਞਾ(fstype);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ਜੇ (fd < 0) {
                ਜੇ (errno == ENODEV)
                        ਵਾਪਸ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ਵਾਪਸ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ਜੇ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", ਨਿਰਾ, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ਜੇ (errno == EOPNOTSUPP)
                        ਵਾਪਸ -EAGAIN; /* fs not converted to new mount API → don't know */
                ਜੇ (errno != EINVAL)
                        ਵਾਪਸ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ਨਹੀਂਤਾਂ
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ਜੇ (ਮੁੱਲ)
                r = fsconfig(fd, FSCONFIG_SET_STRING, ਕੁੰਜੀ, ਮੁੱਲ, 0);
        ਨਹੀਂਤਾਂ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, ਕੁੰਜੀ, ਨਿਰਾ, 0);
        ਜੇ (r < 0) {
                ਜੇ (errno == EINVAL)
                        ਵਾਪਸ false; /* EINVAL means option not supported. */

                ਵਾਪਸ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       ਕੁੰਜੀ, ਮੁੱਲ ? "=" : "", strempty(ਮੁੱਲ), fstype);
        }

        ਵਾਪਸ true; /* works! */
}

bool ਰਸਤਾ_ਹੇਠਾਂ_api_vfs(ਅਚਲ ਅੱਖਰ *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ਵਾਪਸ ਰਸਤਾ_STARTSWITH_ਸੈੱਟ(p, "/dev", "/sys", "/proc");
}
