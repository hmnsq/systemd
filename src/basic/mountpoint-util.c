/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sys/mount.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "filesystems.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mountpoint-util.h"
#அந்தர்பாவ "nulstr-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(முழு err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        ப்ரதிஜ்ஞா(err < 0);

        எனில் (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                திரும்பு false;
        எனில் (ERRNO_IS_NEG_PRIVILEGE(err))
                திரும்பு false;

        திரும்பு !IN_SET(err, -EOVERFLOW, -EINVAL);
}

முழு name_to_handle_at_loop(
                முழு fd,
                மாறிலி எழுத்து *path,
                கட்டமைப்பு file_handle **ret_handle,
                முழு *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                முழு flags) {

        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);
        ப்ரதிஜ்ஞா((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        எனில் (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        க்கு (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ கட்டமைப்பு file_handle *h = வெற்று;

                h = malloc0(offsetof(கட்டமைப்பு file_handle, f_handle) + n);
                எனில் (!h)
                        திரும்பு -ENOMEM;

                h->handle_bytes = n;

                எனில் (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (முழு *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        எனில் (r >= 0) {
                                எனில் (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                எனில் (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                திரும்பு 1;
                        }
                        எனில் (errno == EOVERFLOW)
                                செல் grow;
                        எனில் (errno != EINVAL)
                                திரும்பு -errno;
                }

                முழு mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                எனில் (r >= 0) {
                        எனில் (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                கட்டமைப்பு statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                எனில் (r >= 0) {
                                        எனில் (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        எனில் (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        திரும்பு 1;
                                }
                                எனில் (r != -EUNATCH || !ret_mnt_id)
                                        திரும்பு r;

                                *ret_unique_mnt_id = 0;
                        }

                        எனில் (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        எனில் (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        திரும்பு 0;
                }
                எனில் (errno != EOVERFLOW)
                        திரும்பு -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                எனில் (h->handle_bytes <= n)
                        திரும்பு -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                எனில் (n > UINT_MAX - offsetof(கட்டமைப்பு file_handle, f_handle))
                        திரும்பு -EOVERFLOW;
        }
}

முழு name_to_handle_at_try_fid(
                முழு fd,
                மாறிலி எழுத்து *path,
                கட்டமைப்பு file_handle **ret_handle,
                முழு *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                முழு flags) {

        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        எனில் (r >= 0 || is_name_to_handle_at_fatal_error(r))
                திரும்பு r;

        திரும்பு name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

முழு name_to_handle_at_u64(முழு fd, மாறிலி எழுத்து *path, uint64_t *ret) {
        _cleanup_free_ கட்டமைப்பு file_handle *h = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ வெற்று, /* ret_unique_mnt_id= */ வெற்று, /* flags= */ 0);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (h->handle_bytes < அளவு(uint64_t))
                திரும்பு -EBADMSG;

        எனில் (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        திரும்பு 0;
}

bool file_handle_equal(மாறிலி கட்டமைப்பு file_handle *a, மாறிலி கட்டமைப்பு file_handle *b) {
        எனில் (a == b)
                திரும்பு true;
        எனில் (!a != !b)
                திரும்பு false;
        எனில் (a->handle_type != b->handle_type)
                திரும்பு false;

        திரும்பு memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

கட்டமைப்பு file_handle* file_handle_dup(மாறிலி கட்டமைப்பு file_handle *fh) {
        _cleanup_free_ கட்டமைப்பு file_handle *fh_copy = வெற்று;

        ப்ரதிஜ்ஞா(fh);

        fh_copy = malloc0(offsetof(கட்டமைப்பு file_handle, f_handle) + fh->handle_bytes);
        எனில் (!fh_copy)
                திரும்பு வெற்று;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        திரும்பு TAKE_PTR(fh_copy);
}

முழு is_mount_point_at(முழு dir_fd, மாறிலி எழுத்து *path, முழு flags) {
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா((flags & ~AT_SYMLINK_FOLLOW) == 0);

        எனில் (path_equal(path, "/"))
                திரும்பு true;

        எனில் (dir_fd == XAT_FDROOT && isempty(path))
                திரும்பு true;

        கட்டமைப்பு statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_வேர்,
                        &sx);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_வேர்))
                திரும்பு true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        கட்டமைப்பு statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
முழு path_is_mount_point_full(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், முழு flags) {
        _cleanup_close_ முழு dir_fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா((flags & ~AT_SYMLINK_FOLLOW) == 0);

        எனில் (empty_or_வேர்(வேர்))
                திரும்பு is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, வேர்,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ வெற்று, &dir_fd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு is_mount_point_at(dir_fd, /* path= */ வெற்று, flags);
}

நிலையான முழு path_get_mnt_id_at_internal(முழு dir_fd, மாறிலி எழுத்து *path, bool unique, uint64_t *ret) {
        கட்டமைப்பு statx sx;
        முழு r;

        ப்ரதிஜ்ஞா(wildcard_fd_is_valid(dir_fd));
        ப்ரதிஜ்ஞா(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        எனில் (r < 0)
                திரும்பு r;

        *ret = sx.stx_mnt_id;
        திரும்பு 0;
}

முழு path_get_mnt_id_at(முழு dir_fd, மாறிலி எழுத்து *path, முழு *ret) {
        uint64_t mnt_id;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(mnt_id <= INT_MAX);
        *ret = (முழு) mnt_id;
        திரும்பு 0;
}

முழு path_get_unique_mnt_id_at(முழு dir_fd, மாறிலி எழுத்து *path, uint64_t *ret) {
        திரும்பு path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(மாறிலி எழுத்து *fstype) {
        மாறிலி எழுத்து *x;

        x = startswith(fstype, "fuse.");
        எனில் (x)
                fstype = x;

        எனில் (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].மதிப்பு, fstype))
                திரும்பு true;

        /* Filesystems not present in the internal database */
        திரும்பு STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(மாறிலி எழுத்து *fstype) {
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
        திரும்பு STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(மாறிலி எழுத்து *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        திரும்பு STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(மாறிலி எழுத்து *fstype) {
        ப்ரதிஜ்ஞா(fstype);

        மாறிலி FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                எனில் (nulstr_contains(fs->மதிப்பு, fstype))
                    திரும்பு true;

        /* Filesystems not present in the internal database */
        திரும்பு STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(மாறிலி எழுத்து *fstype) {
        மாறிலி எழுத்து *x;

        x = startswith(fstype, "fuse.");
        எனில் (x)
                fstype = x;

        திரும்பு !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(மாறிலி எழுத்து *fstype) {
        /* All Linux file systems that are necessarily read-only */
        திரும்பு STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(மாறிலி எழுத்து *fstype) {
        ப்ரதிஜ்ஞா(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        எனில் (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                திரும்பு true;

        /* On new kernels we can just ask the kernel */
        திரும்பு mount_option_supported(fstype, "discard", வெற்று) > 0;
}

மாறிலி எழுத்து* fstype_norecovery_option(மாறிலி எழுத்து *fstype) {
        முழு r;

        ப்ரதிஜ்ஞா(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        எனில் (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                திரும்பு "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        எனில் (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", வெற்று);
                எனில் (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        திரும்பு "norecovery";
                }
                எனில் (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                எனில் (r > 0)
                        திரும்பு "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        திரும்பு mount_option_supported(fstype, "norecovery", வெற்று) > 0 ? "norecovery" : வெற்று;
}

bool fstype_can_fmask_dmask(மாறிலி எழுத்து *fstype) {
        ப்ரதிஜ்ஞா(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        திரும்பு streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(மாறிலி எழுத்து *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        திரும்பு !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(மாறிலி எழுத்து *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        திரும்பு STR_IN_SET(fstype,
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

முழு dev_is_devtmpfs(வெறுமை) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = வெற்று;
        முழு mount_id, r;
        எழுத்து *e;

        r = path_get_mnt_id("/dev", &mount_id);
        எனில் (r < 0)
                திரும்பு r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        எனில் (r == -ENOENT)
                திரும்பு proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        எனில் (r < 0)
                திரும்பு r;

        க்கு (;;) {
                _cleanup_free_ எழுத்து *வரி = வெற்று;
                முழு mid;

                r = read_வரி(proc_self_mountinfo, LONG_வரி_MAX, &வரி);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                எனில் (sscanf(வரி, "%i", &mid) != 1)
                        தொடர்;

                எனில் (mid != mount_id)
                        தொடர்;

                e = strstrafter(வரி, " - ");
                எனில் (!e)
                        தொடர்;

                /* accept any name that starts with the currently expected type */
                எனில் (startswith(e, "devtmpfs"))
                        திரும்பு true;
        }

        திரும்பு false;
}

நிலையான முழு mount_fd(
                மாறிலி எழுத்து *source,
                முழு target_fd,
                மாறிலி எழுத்து *filesystemtype,
                unsigned long mountflags,
                மாறிலி வெறுமை *data) {

        ப்ரதிஜ்ஞா(target_fd >= 0);

        எனில் (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                எனில் (proc_mounted() == 0)
                        திரும்பு -ENOSYS;

                திரும்பு -ENOENT;
        }

        திரும்பு 0;
}

முழு mount_nofollow(
                மாறிலி எழுத்து *source,
                மாறிலி எழுத்து *target,
                மாறிலி எழுத்து *filesystemtype,
                unsigned long mountflags,
                மாறிலி வெறுமை *data) {

        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        எனில் (fd < 0)
                திரும்பு -errno;

        திரும்பு mount_fd(source, fd, filesystemtype, mountflags, data);
}

மாறிலி எழுத்து* mount_propagation_flag_to_string(unsigned long flags) {

        தேர்வு (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        நிலை 0:
                திரும்பு "";
        நிலை MS_SHARED:
                திரும்பு "shared";
        நிலை MS_SLAVE:
                திரும்பு "slave";
        நிலை MS_PRIVATE:
                திரும்பு "private";
        }

        திரும்பு வெற்று;
}

முழு mount_propagation_flag_from_string(மாறிலி எழுத்து *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        ப்ரதிஜ்ஞா(ret);

        எனில் (isempty(name))
                *ret = 0;
        இல்லையெனில் எனில் (streq(name, "shared"))
                *ret = MS_SHARED;
        இல்லையெனில் எனில் (streq(name, "slave"))
                *ret = MS_SLAVE;
        இல்லையெனில் எனில் (streq(name, "private"))
                *ret = MS_PRIVATE;
        இல்லையெனில்
                திரும்பு -EINVAL;
        திரும்பு 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        திரும்பு IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(வெறுமை) {
        நிலையான முழு cache = -1;
        முழு r;

        எனில் (cache >= 0)
                திரும்பு cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, வெற்று, 0, வெற்று, 0));
        எனில் (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                திரும்பு (cache = false);

        திரும்பு (cache = true);
}

முழு mount_option_supported(மாறிலி எழுத்து *fstype, மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        ப்ரதிஜ்ஞா(fstype);
        ப்ரதிஜ்ஞா(சாவி);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        எனில் (fd < 0) {
                எனில் (errno == ENODEV)
                        திரும்பு log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                திரும்பு log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        எனில் (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", வெற்று, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                எனில் (errno == EOPNOTSUPP)
                        திரும்பு -EAGAIN; /* fs not converted to new mount API → don't know */
                எனில் (errno != EINVAL)
                        திரும்பு log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } இல்லையெனில்
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        எனில் (மதிப்பு)
                r = fsconfig(fd, FSCONFIG_SET_STRING, சாவி, மதிப்பு, 0);
        இல்லையெனில்
                r = fsconfig(fd, FSCONFIG_SET_FLAG, சாவி, வெற்று, 0);
        எனில் (r < 0) {
                எனில் (errno == EINVAL)
                        திரும்பு false; /* EINVAL means option not supported. */

                திரும்பு log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       சாவி, மதிப்பு ? "=" : "", strempty(மதிப்பு), fstype);
        }

        திரும்பு true; /* works! */
}

bool path_below_api_vfs(மாறிலி எழுத்து *p) {
        ப்ரதிஜ்ஞா(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        திரும்பு PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
