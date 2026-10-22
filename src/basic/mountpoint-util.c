/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sys/mount.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "filesystems.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mountpoint-util.h"
#ଅନ୍ତର୍ଭାଵ "nulstr-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        ପ୍ରତିଜ୍ଞା(err < 0);

        ଯଦି (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ଫେରନ୍ତୁ false;
        ଯଦି (ERRNO_IS_NEG_PRIVILEGE(err))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ !IN_SET(err, -EOVERFLOW, -EINVAL);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_to_handle_at_loop(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ସଂରଚନା file_handle **ret_handle,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        ଯଦି (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        ପାଇଁ (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ ସଂରଚନା file_handle *h = ଖାଲି;

                h = malloc0(offsetof(ସଂରଚନା file_handle, f_handle) + n);
                ଯଦି (!h)
                        ଫେରନ୍ତୁ -ENOMEM;

                h->handle_bytes = n;

                ଯଦି (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ଯଦି (r >= 0) {
                                ଯଦି (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ଯଦି (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ଫେରନ୍ତୁ 1;
                        }
                        ଯଦି (errno == EOVERFLOW)
                                ଯାଅ grow;
                        ଯଦି (errno != EINVAL)
                                ଫେରନ୍ତୁ -errno;
                }

                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                ଯଦି (r >= 0) {
                        ଯଦି (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                ସଂରଚନା statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ଯଦି (r >= 0) {
                                        ଯଦି (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ଯଦି (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ଫେରନ୍ତୁ 1;
                                }
                                ଯଦି (r != -EUNATCH || !ret_mnt_id)
                                        ଫେରନ୍ତୁ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ଯଦି (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ଯଦି (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (errno != EOVERFLOW)
                        ଫେରନ୍ତୁ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ଯଦି (h->handle_bytes <= n)
                        ଫେରନ୍ତୁ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ଯଦି (n > UINT_MAX - offsetof(ସଂରଚନା file_handle, f_handle))
                        ଫେରନ୍ତୁ -EOVERFLOW;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_to_handle_at_try_fid(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ସଂରଚନା file_handle **ret_handle,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ଯଦି (r >= 0 || is_name_to_handle_at_fatal_error(r))
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା name_to_handle_at_u64(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret) {
        _cleanup_free_ ସଂରଚନା file_handle *h = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ ଖାଲି, /* ret_unique_mnt_id= */ ଖାଲି, /* flags= */ 0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (h->handle_bytes < ଆକାର(uint64_t))
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ଫେରନ୍ତୁ 0;
}

bool file_handle_equal(ଅଚଳ ସଂରଚନା file_handle *a, ଅଚଳ ସଂରଚନା file_handle *b) {
        ଯଦି (a == b)
                ଫେରନ୍ତୁ true;
        ଯଦି (!a != !b)
                ଫେରନ୍ତୁ false;
        ଯଦି (a->handle_type != b->handle_type)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

ସଂରଚନା file_handle* file_handle_dup(ଅଚଳ ସଂରଚନା file_handle *fh) {
        _cleanup_free_ ସଂରଚନା file_handle *fh_copy = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(fh);

        fh_copy = malloc0(offsetof(ସଂରଚନା file_handle, f_handle) + fh->handle_bytes);
        ଯଦି (!fh_copy)
                ଫେରନ୍ତୁ ଖାଲି;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        ଫେରନ୍ତୁ TAKE_PTR(fh_copy);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_mount_point_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ଯଦି (path_equal(path, "/"))
                ଫେରନ୍ତୁ true;

        ଯଦି (dir_fd == XAT_FDROOT && isempty(path))
                ଫେରନ୍ତୁ true;

        ସଂରଚନା statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ମୂଳ,
                        &sx);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ମୂଳ))
                ଫେରନ୍ତୁ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        ସଂରଚନା statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_mount_point_full(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ଯଦି (empty_or_ମୂଳ(ମୂଳ))
                ଫେରନ୍ତୁ is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, ମୂଳ,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ ଖାଲି, &dir_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ is_mount_point_at(dir_fd, /* path= */ ଖାଲି, flags);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_get_mnt_id_at_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, bool unique, uint64_t *ret) {
        ସଂରଚନା statx sx;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(wildcard_fd_is_valid(dir_fd));
        ପ୍ରତିଜ୍ଞା(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = sx.stx_mnt_id;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_get_mnt_id_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret) {
        uint64_t mnt_id;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(mnt_id <= INT_MAX);
        *ret = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) mnt_id;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_get_unique_mnt_id_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, uint64_t *ret) {
        ଫେରନ୍ତୁ path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(ଅଚଳ ଅକ୍ଷର *fstype) {
        ଅଚଳ ଅକ୍ଷର *x;

        x = startswith(fstype, "fuse.");
        ଯଦି (x)
                fstype = x;

        ଯଦି (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].ମୂଲ୍ୟ, fstype))
                ଫେରନ୍ତୁ true;

        /* Filesystems not present in the internal database */
        ଫେରନ୍ତୁ STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(ଅଚଳ ଅକ୍ଷର *fstype) {
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
        ଫେରନ୍ତୁ STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(ଅଚଳ ଅକ୍ଷର *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        ଫେରନ୍ତୁ STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(ଅଚଳ ଅକ୍ଷର *fstype) {
        ପ୍ରତିଜ୍ଞା(fstype);

        ଅଚଳ FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                ଯଦି (nulstr_contains(fs->ମୂଲ୍ୟ, fstype))
                    ଫେରନ୍ତୁ true;

        /* Filesystems not present in the internal database */
        ଫେରନ୍ତୁ STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(ଅଚଳ ଅକ୍ଷର *fstype) {
        ଅଚଳ ଅକ୍ଷର *x;

        x = startswith(fstype, "fuse.");
        ଯଦି (x)
                fstype = x;

        ଫେରନ୍ତୁ !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(ଅଚଳ ଅକ୍ଷର *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ଫେରନ୍ତୁ STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(ଅଚଳ ଅକ୍ଷର *fstype) {
        ପ୍ରତିଜ୍ଞା(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ଯଦି (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ଫେରନ୍ତୁ true;

        /* On new kernels we can just ask the kernel */
        ଫେରନ୍ତୁ mount_option_supported(fstype, "discard", ଖାଲି) > 0;
}

ଅଚଳ ଅକ୍ଷର* fstype_norecovery_option(ଅଚଳ ଅକ୍ଷର *fstype) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ଯଦି (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                ଫେରନ୍ତୁ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ଯଦି (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", ଖାଲି);
                ଯଦି (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ଫେରନ୍ତୁ "norecovery";
                }
                ଯଦି (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ଯଦି (r > 0)
                        ଫେରନ୍ତୁ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ଫେରନ୍ତୁ mount_option_supported(fstype, "norecovery", ଖାଲି) > 0 ? "norecovery" : ଖାଲି;
}

bool fstype_can_fmask_dmask(ଅଚଳ ଅକ୍ଷର *fstype) {
        ପ୍ରତିଜ୍ଞା(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ଫେରନ୍ତୁ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(ଅଚଳ ଅକ୍ଷର *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ଫେରନ୍ତୁ !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(ଅଚଳ ଅକ୍ଷର *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        ଫେରନ୍ତୁ STR_IN_SET(fstype,
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dev_is_devtmpfs(ଶୂନ୍ୟ) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_id, r;
        ଅକ୍ଷର *e;

        r = path_get_mnt_id("/dev", &mount_id);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା mid;

                r = read_ଧାଡ଼ି(proc_self_mountinfo, LONG_ଧାଡ଼ି_MAX, &ଧାଡ଼ି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (sscanf(ଧାଡ଼ି, "%i", &mid) != 1)
                        ଜାରି;

                ଯଦି (mid != mount_id)
                        ଜାରି;

                e = strstrafter(ଧାଡ଼ି, " - ");
                ଯଦି (!e)
                        ଜାରି;

                /* accept any name that starts with the currently expected type */
                ଯଦି (startswith(e, "devtmpfs"))
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_fd(
                ଅଚଳ ଅକ୍ଷର *source,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା target_fd,
                ଅଚଳ ଅକ୍ଷର *filesystemtype,
                unsigned long mountflags,
                ଅଚଳ ଶୂନ୍ୟ *data) {

        ପ୍ରତିଜ୍ଞା(target_fd >= 0);

        ଯଦି (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ଯଦି (proc_mounted() == 0)
                        ଫେରନ୍ତୁ -ENOSYS;

                ଫେରନ୍ତୁ -ENOENT;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_nofollow(
                ଅଚଳ ଅକ୍ଷର *source,
                ଅଚଳ ଅକ୍ଷର *target,
                ଅଚଳ ଅକ୍ଷର *filesystemtype,
                unsigned long mountflags,
                ଅଚଳ ଶୂନ୍ୟ *data) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ mount_fd(source, fd, filesystemtype, mountflags, data);
}

ଅଚଳ ଅକ୍ଷର* mount_propagation_flag_to_string(unsigned long flags) {

        ବାଛ (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        ସ୍ଥିତି 0:
                ଫେରନ୍ତୁ "";
        ସ୍ଥିତି MS_SHARED:
                ଫେରନ୍ତୁ "shared";
        ସ୍ଥିତି MS_SLAVE:
                ଫେରନ୍ତୁ "slave";
        ସ୍ଥିତି MS_PRIVATE:
                ଫେରନ୍ତୁ "private";
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_propagation_flag_from_string(ଅଚଳ ଅକ୍ଷର *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (isempty(name))
                *ret = 0;
        ଅନ୍ୟଥା ଯଦି (streq(name, "shared"))
                *ret = MS_SHARED;
        ଅନ୍ୟଥା ଯଦି (streq(name, "slave"))
                *ret = MS_SLAVE;
        ଅନ୍ୟଥା ଯଦି (streq(name, "private"))
                *ret = MS_PRIVATE;
        ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -EINVAL;
        ଫେରନ୍ତୁ 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        ଫେରନ୍ତୁ IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(ଶୂନ୍ୟ) {
        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache = -1;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (cache >= 0)
                ଫେରନ୍ତୁ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, ଖାଲି, 0, ଖାଲି, 0));
        ଯଦି (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ଫେରନ୍ତୁ (cache = false);

        ଫେରନ୍ତୁ (cache = true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mount_option_supported(ଅଚଳ ଅକ୍ଷର *fstype, ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        ପ୍ରତିଜ୍ଞା(fstype);
        ପ୍ରତିଜ୍ଞା(ଚାବି);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ଯଦି (fd < 0) {
                ଯଦି (errno == ENODEV)
                        ଫେରନ୍ତୁ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ଯଦି (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", ଖାଲି, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ଯଦି (errno == EOPNOTSUPP)
                        ଫେରନ୍ତୁ -EAGAIN; /* fs not converted to new mount API → don't know */
                ଯଦି (errno != EINVAL)
                        ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ଅନ୍ୟଥା
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ଯଦି (ମୂଲ୍ୟ)
                r = fsconfig(fd, FSCONFIG_SET_STRING, ଚାବି, ମୂଲ୍ୟ, 0);
        ଅନ୍ୟଥା
                r = fsconfig(fd, FSCONFIG_SET_FLAG, ଚାବି, ଖାଲି, 0);
        ଯଦି (r < 0) {
                ଯଦି (errno == EINVAL)
                        ଫେରନ୍ତୁ false; /* EINVAL means option not supported. */

                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       ଚାବି, ମୂଲ୍ୟ ? "=" : "", strempty(ମୂଲ୍ୟ), fstype);
        }

        ଫେରନ୍ତୁ true; /* works! */
}

bool path_below_api_vfs(ଅଚଳ ଅକ୍ଷର *p) {
        ପ୍ରତିଜ୍ଞା(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ଫେରନ୍ତୁ PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
