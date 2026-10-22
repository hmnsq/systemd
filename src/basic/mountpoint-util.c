/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sys/mount.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "filesystems.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mountpoint-util.h"
#అన్తర్భావ "nulstr-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#నిర్వచించు ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_గమ్యం_handle_at_fatal_error(పూర్ణాంకం err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        ప్రతిజ్ఞా(err < 0);

        ఒకవేళ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                తిరిగి false;
        ఒకవేళ (ERRNO_IS_NEG_PRIVILEGE(err))
                తిరిగి false;

        తిరిగి !లోపల_సమితి(err, -EOVERFLOW, -EINVAL);
}

పూర్ణాంకం name_గమ్యం_handle_at_loop(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                నిర్మాణం file_handle **ret_handle,
                పూర్ణాంకం *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                పూర్ణాంకం flags) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);
        ప్రతిజ్ఞా((flags & ~(AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం|AT_HANDLE_FID)) == 0);

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

        ఒకవేళ (isempty(మార్గం)) {
                flags |= AT_ఖాళీలు_మార్గం;
                మార్గం = "";
        }

        కొరకు (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ నిర్మాణం file_handle *h = ఖాళీ;

                h = malloc0(offsetof(నిర్మాణం file_handle, f_handle) + n);
                ఒకవేళ (!h)
                        తిరిగి -ENOMEM;

                h->handle_బైట్లు = n;

                ఒకవేళ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_గమ్యం_handle_at(fd, మార్గం, h, (పూర్ణాంకం *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ఒకవేళ (r >= 0) {
                                ఒకవేళ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ఒకవేళ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                తిరిగి 1;
                        }
                        ఒకవేళ (errno == EOVERFLOW)
                                వెళ్ళు grow;
                        ఒకవేళ (errno != EINVAL)
                                తిరిగి -errno;
                }

                పూర్ణాంకం mnt_id;
                r = name_గమ్యం_handle_at(fd, మార్గం, h, &mnt_id, flags);
                ఒకవేళ (r >= 0) {
                        ఒకవేళ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                నిర్మాణం statx sx;
                                r = xstatx(fd, మార్గం,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_ఖాళీలు_మార్గం))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ఒకవేళ (r >= 0) {
                                        ఒకవేళ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ఒకవేళ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        తిరిగి 1;
                                }
                                ఒకవేళ (r != -EUNATCH || !ret_mnt_id)
                                        తిరిగి r;

                                *ret_unique_mnt_id = 0;
                        }

                        ఒకవేళ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ఒకవేళ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        తిరిగి 0;
                }
                ఒకవేళ (errno != EOVERFLOW)
                        తిరిగి -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ఒకవేళ (h->handle_బైట్లు <= n)
                        తిరిగి -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_బైట్లు;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ఒకవేళ (n > UINT_MAX - offsetof(నిర్మాణం file_handle, f_handle))
                        తిరిగి -EOVERFLOW;
        }
}

పూర్ణాంకం name_గమ్యం_handle_at_try_fid(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                నిర్మాణం file_handle **ret_handle,
                పూర్ణాంకం *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                పూర్ణాంకం flags) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_గమ్యం_handle_at_loop(fd, మార్గం, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ఒకవేళ (r >= 0 || is_name_గమ్యం_handle_at_fatal_error(r))
                తిరిగి r;

        తిరిగి name_గమ్యం_handle_at_loop(fd, మార్గం, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

పూర్ణాంకం name_గమ్యం_handle_at_u64(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret) {
        _cleanup_free_ నిర్మాణం file_handle *h = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_గమ్యం_handle_at_loop(fd, మార్గం, &h, /* ret_mnt_id= */ ఖాళీ, /* ret_unique_mnt_id= */ ఖాళీ, /* flags= */ 0);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (h->handle_బైట్లు < పరిమాణం(uint64_t))
                తిరిగి -EBADMSG;

        ఒకవేళ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        తిరిగి 0;
}

bool file_handle_equal(స్థిరరాశి నిర్మాణం file_handle *a, స్థిరరాశి నిర్మాణం file_handle *b) {
        ఒకవేళ (a == b)
                తిరిగి true;
        ఒకవేళ (!a != !b)
                తిరిగి false;
        ఒకవేళ (a->handle_type != b->handle_type)
                తిరిగి false;

        తిరిగి memcmp_nn(a->f_handle, a->handle_బైట్లు, b->f_handle, b->handle_బైట్లు) == 0;
}

నిర్మాణం file_handle* file_handle_dup(స్థిరరాశి నిర్మాణం file_handle *fh) {
        _cleanup_free_ నిర్మాణం file_handle *fh_copy = ఖాళీ;

        ప్రతిజ్ఞా(fh);

        fh_copy = malloc0(offsetof(నిర్మాణం file_handle, f_handle) + fh->handle_బైట్లు);
        ఒకవేళ (!fh_copy)
                తిరిగి ఖాళీ;

        fh_copy->handle_బైట్లు = fh->handle_బైట్లు;
        fh_copy->handle_type = fh->handle_type;
        జ్ఞాపకం_నకలు(fh_copy->f_handle, fh->f_handle, fh->handle_బైట్లు);

        తిరిగి TAKE_PTR(fh_copy);
}

పూర్ణాంకం is_mount_బిందువు_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ఒకవేళ (మార్గం_equal(మార్గం, "/"))
                తిరిగి true;

        ఒకవేళ (dir_fd == XAT_FDROOT && isempty(మార్గం))
                తిరిగి true;

        నిర్మాణం statx sx;
        r = xstatx_నిండిన(dir_fd, మార్గం,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_మూలం,
                        &sx);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (FLAGS_సమితి(sx.stx_లక్షణాలు, STATX_ATTR_MOUNT_మూలం))
                తిరిగి true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        నిర్మాణం statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
పూర్ణాంకం మార్గం_is_mount_బిందువు_నిండిన(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, పూర్ణాంకం flags) {
        _cleanup_close_ పూర్ణాంకం dir_fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ఒకవేళ (ఖాళీలు_or_మూలం(మూలం))
                తిరిగి is_mount_బిందువు_at(AT_FDCWD, మార్గం, flags);

        r = chase(మార్గం, మూలం,
                  FLAGS_సమితి(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ ఖాళీ, &dir_fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి is_mount_బిందువు_at(dir_fd, /* path= */ ఖాళీ, flags);
}

స్థిర పూర్ణాంకం మార్గం_get_mnt_id_at_అంతర్గత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, bool unique, uint64_t *ret) {
        నిర్మాణం statx sx;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(wildcard_fd_is_valid(dir_fd));
        ప్రతిజ్ఞా(ret);

        r = xstatx(dir_fd, మార్గం,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = sx.stx_mnt_id;
        తిరిగి 0;
}

పూర్ణాంకం మార్గం_get_mnt_id_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం *ret) {
        uint64_t mnt_id;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = మార్గం_get_mnt_id_at_అంతర్గత(dir_fd, మార్గం, /* unique = */ false, &mnt_id);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(mnt_id <= గరిష్ఠ_పూర్ణాంకం);
        *ret = (పూర్ణాంకం) mnt_id;
        తిరిగి 0;
}

పూర్ణాంకం మార్గం_get_unique_mnt_id_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, uint64_t *ret) {
        తిరిగి మార్గం_get_mnt_id_at_అంతర్గత(dir_fd, మార్గం, /* unique = */ true, ret);
}

bool fstype_is_జాలం(స్థిరరాశి అక్షరం *fstype) {
        స్థిరరాశి అక్షరం *x;

        x = startswith(fstype, "fuse.");
        ఒకవేళ (x)
                fstype = x;

        ఒకవేళ (nulstr_కలిగియుంది(filesystem_sets[FILESYSTEM_సమితి_జాలం].విలువ, fstype))
                తిరిగి true;

        /* Filesystems not present in the internal database */
        తిరిగి STR_లోపల_సమితి(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(స్థిరరాశి అక్షరం *fstype) {
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
        తిరిగి STR_లోపల_సమితి(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_అంతర్గత_quota(స్థిరరాశి అక్షరం *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        తిరిగి STR_లోపల_సమితి(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(స్థిరరాశి అక్షరం *fstype) {
        ప్రతిజ్ఞా(fstype);

        స్థిరరాశి Filesystem_సమితి *fs;
        FOREACH_వాదం(fs,
                         filesystem_sets + FILESYSTEM_సమితి_BASIC_API,
                         filesystem_sets + FILESYSTEM_సమితి_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_సమితి_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_సమితి_TEMPORARY)
                ఒకవేళ (nulstr_కలిగియుంది(fs->విలువ, fstype))
                    తిరిగి true;

        /* Filesystems not present in the internal database */
        తిరిగి STR_లోపల_సమితి(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(స్థిరరాశి అక్షరం *fstype) {
        స్థిరరాశి అక్షరం *x;

        x = startswith(fstype, "fuse.");
        ఒకవేళ (x)
                fstype = x;

        తిరిగి !STR_లోపల_సమితి(fstype, "9p", "overlay") && !fstype_is_జాలం(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(స్థిరరాశి అక్షరం *fstype) {
        /* All Linux file systems that are necessarily read-only */
        తిరిగి STR_లోపల_సమితి(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(స్థిరరాశి అక్షరం *fstype) {
        ప్రతిజ్ఞా(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ఒకవేళ (STR_లోపల_సమితి(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                తిరిగి true;

        /* On new kernels we can just ask the kernel */
        తిరిగి mount_option_supported(fstype, "discard", ఖాళీ) > 0;
}

స్థిరరాశి అక్షరం* fstype_norecovery_option(స్థిరరాశి అక్షరం *fstype) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ఒకవేళ (STR_లోపల_సమితి(fstype, "ext3", "ext4", "xfs"))
                తిరిగి "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ఒకవేళ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", ఖాళీ);
                ఒకవేళ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        తిరిగి "norecovery";
                }
                ఒకవేళ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ఒకవేళ (r > 0)
                        తిరిగి "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        తిరిగి mount_option_supported(fstype, "norecovery", ఖాళీ) > 0 ? "norecovery" : ఖాళీ;
}

bool fstype_can_fmask_dmask(స్థిరరాశి అక్షరం *fstype) {
        ప్రతిజ్ఞా(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        తిరిగి streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(స్థిరరాశి అక్షరం *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        తిరిగి !STR_లోపల_సమితి(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(స్థిరరాశి అక్షరం *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        తిరిగి STR_లోపల_సమితి(fstype,
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

పూర్ణాంకం dev_is_devtmpfs(శూన్య) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = ఖాళీ;
        పూర్ణాంకం mount_id, r;
        అక్షరం *e;

        r = మార్గం_get_mnt_id("/dev", &mount_id);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ఒకవేళ (r == -ENOENT)
                తిరిగి proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (;;) {
                _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
                పూర్ణాంకం mid;

                r = read_పంక్తి(proc_self_mountinfo, LONG_పంక్తి_MAX, &పంక్తి);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (అక్షరశ్రేణి_చదువు(పంక్తి, "%i", &mid) != 1)
                        కొనసాగు;

                ఒకవేళ (mid != mount_id)
                        కొనసాగు;

                e = strstrafter(పంక్తి, " - ");
                ఒకవేళ (!e)
                        కొనసాగు;

                /* accept any name that starts with the currently expected type */
                ఒకవేళ (startswith(e, "devtmpfs"))
                        తిరిగి true;
        }

        తిరిగి false;
}

స్థిర పూర్ణాంకం mount_fd(
                స్థిరరాశి అక్షరం *ఆధారం,
                పూర్ణాంకం లక్ష్యం_fd,
                స్థిరరాశి అక్షరం *filesystemtype,
                గుర్తులేని దీర్ఘ mountflags,
                స్థిరరాశి శూన్య *దత్తాంశం) {

        ప్రతిజ్ఞా(లక్ష్యం_fd >= 0);

        ఒకవేళ (mount(ఆధారం, FORMAT_PROC_FD_మార్గం(లక్ష్యం_fd), filesystemtype, mountflags, దత్తాంశం) < 0) {
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ఒకవేళ (proc_mounted() == 0)
                        తిరిగి -ENOSYS;

                తిరిగి -ENOENT;
        }

        తిరిగి 0;
}

పూర్ణాంకం mount_nofollow(
                స్థిరరాశి అక్షరం *ఆధారం,
                స్థిరరాశి అక్షరం *లక్ష్యం,
                స్థిరరాశి అక్షరం *filesystemtype,
                గుర్తులేని దీర్ఘ mountflags,
                స్థిరరాశి శూన్య *దత్తాంశం) {

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(లక్ష్యం);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(లక్ష్యం, O_మార్గం|O_CLOEXEC|O_NOFOLLOW);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        తిరిగి mount_fd(ఆధారం, fd, filesystemtype, mountflags, దత్తాంశం);
}

స్థిరరాశి అక్షరం* mount_ప్రసరణ_flag_గమ్యం_string(గుర్తులేని దీర్ఘ flags) {

        ఎంపిక (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        స్థితి 0:
                తిరిగి "";
        స్థితి MS_SHARED:
                తిరిగి "shared";
        స్థితి MS_SLAVE:
                తిరిగి "slave";
        స్థితి MS_PRIVATE:
                తిరిగి "private";
        }

        తిరిగి ఖాళీ;
}

పూర్ణాంకం mount_ప్రసరణ_flag_from_string(స్థిరరాశి అక్షరం *name, గుర్తులేని దీర్ఘ *ret) {

        సూచిక_MAY_BE_NULL(name);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (isempty(name))
                *ret = 0;
        లేకపోతే ఒకవేళ (streq(name, "shared"))
                *ret = MS_SHARED;
        లేకపోతే ఒకవేళ (streq(name, "slave"))
                *ret = MS_SLAVE;
        లేకపోతే ఒకవేళ (streq(name, "private"))
                *ret = MS_PRIVATE;
        లేకపోతే
                తిరిగి -EINVAL;
        తిరిగి 0;
}

bool mount_ప్రసరణ_flag_is_valid(గుర్తులేని దీర్ఘ flag) {
        తిరిగి లోపల_సమితి(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(శూన్య) {
        స్థిర పూర్ణాంకం cache = -1;
        పూర్ణాంకం r;

        ఒకవేళ (cache >= 0)
                తిరిగి cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, ఖాళీ, 0, ఖాళీ, 0));
        ఒకవేళ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                తిరిగి (cache = false);

        తిరిగి (cache = true);
}

పూర్ణాంకం mount_option_supported(స్థిరరాశి అక్షరం *fstype, స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        ప్రతిజ్ఞా(fstype);
        ప్రతిజ్ఞా(కీలకం);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ఒకవేళ (fd < 0) {
                ఒకవేళ (errno == ENODEV)
                        తిరిగి log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                తిరిగి log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ఒకవేళ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", ఖాళీ, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ఒకవేళ (errno == EOPNOTSUPP)
                        తిరిగి -EAGAIN; /* fs not converted to new mount API → don't know */
                ఒకవేళ (errno != EINVAL)
                        తిరిగి log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } లేకపోతే
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ఒకవేళ (విలువ)
                r = fsconfig(fd, FSCONFIG_SET_STRING, కీలకం, విలువ, 0);
        లేకపోతే
                r = fsconfig(fd, FSCONFIG_SET_FLAG, కీలకం, ఖాళీ, 0);
        ఒకవేళ (r < 0) {
                ఒకవేళ (errno == EINVAL)
                        తిరిగి false; /* EINVAL means option not supported. */

                తిరిగి log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       కీలకం, విలువ ? "=" : "", strempty(విలువ), fstype);
        }

        తిరిగి true; /* works! */
}

bool మార్గం_క్రింద_api_vfs(స్థిరరాశి అక్షరం *p) {
        ప్రతిజ్ఞా(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        తిరిగి మార్గం_STARTSWITH_సమితి(p, "/dev", "/sys", "/proc");
}
