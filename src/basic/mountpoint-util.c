/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sys/mount.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "filesystems.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mountpoint-util.h"
#અન્તર્ભાવ "nulstr-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(પૂર્ણાંક err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        પ્રતિજ્ઞા(err < 0);

        જો (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                પાછા false;
        જો (ERRNO_IS_NEG_PRIVILEGE(err))
                પાછા false;

        પાછા !IN_SET(err, -EOVERFLOW, -EINVAL);
}

પૂર્ણાંક name_to_handle_at_loop(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                સંરચના file_handle **ret_handle,
                પૂર્ણાંક *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                પૂર્ણાંક flags) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);
        પ્રતિજ્ઞા((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        જો (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        માટે (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ સંરચના file_handle *h = ખાલી;

                h = malloc0(offsetof(સંરચના file_handle, f_handle) + n);
                જો (!h)
                        પાછા -ENOMEM;

                h->handle_bytes = n;

                જો (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (પૂર્ણાંક *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        જો (r >= 0) {
                                જો (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                જો (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                પાછા 1;
                        }
                        જો (errno == EOVERFLOW)
                                જાઓ grow;
                        જો (errno != EINVAL)
                                પાછા -errno;
                }

                પૂર્ણાંક mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                જો (r >= 0) {
                        જો (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                સંરચના statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                જો (r >= 0) {
                                        જો (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        જો (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        પાછા 1;
                                }
                                જો (r != -EUNATCH || !ret_mnt_id)
                                        પાછા r;

                                *ret_unique_mnt_id = 0;
                        }

                        જો (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        જો (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        પાછા 0;
                }
                જો (errno != EOVERFLOW)
                        પાછા -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                જો (h->handle_bytes <= n)
                        પાછા -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                જો (n > UINT_MAX - offsetof(સંરચના file_handle, f_handle))
                        પાછા -EOVERFLOW;
        }
}

પૂર્ણાંક name_to_handle_at_try_fid(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                સંરચના file_handle **ret_handle,
                પૂર્ણાંક *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                પૂર્ણાંક flags) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        જો (r >= 0 || is_name_to_handle_at_fatal_error(r))
                પાછા r;

        પાછા name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

પૂર્ણાંક name_to_handle_at_u64(પૂર્ણાંક fd, અચલ અક્ષર *path, uint64_t *ret) {
        _cleanup_free_ સંરચના file_handle *h = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ ખાલી, /* ret_unique_mnt_id= */ ખાલી, /* flags= */ 0);
        જો (r < 0)
                પાછા r;
        જો (h->handle_bytes < કદ(uint64_t))
                પાછા -EBADMSG;

        જો (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        પાછા 0;
}

bool file_handle_equal(અચલ સંરચના file_handle *a, અચલ સંરચના file_handle *b) {
        જો (a == b)
                પાછા true;
        જો (!a != !b)
                પાછા false;
        જો (a->handle_type != b->handle_type)
                પાછા false;

        પાછા memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

સંરચના file_handle* file_handle_dup(અચલ સંરચના file_handle *fh) {
        _cleanup_free_ સંરચના file_handle *fh_copy = ખાલી;

        પ્રતિજ્ઞા(fh);

        fh_copy = malloc0(offsetof(સંરચના file_handle, f_handle) + fh->handle_bytes);
        જો (!fh_copy)
                પાછા ખાલી;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        પાછા TAKE_PTR(fh_copy);
}

પૂર્ણાંક is_mount_point_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક flags) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા((flags & ~AT_SYMLINK_FOLLOW) == 0);

        જો (path_equal(path, "/"))
                પાછા true;

        જો (dir_fd == XAT_FDROOT && isempty(path))
                પાછા true;

        સંરચના statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_મૂળ,
                        &sx);
        જો (r < 0)
                પાછા r;

        જો (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_મૂળ))
                પાછા true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        સંરચના statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        જો (r < 0)
                પાછા r;

        પાછા statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
પૂર્ણાંક path_is_mount_point_full(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, પૂર્ણાંક flags) {
        _cleanup_close_ પૂર્ણાંક dir_fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા((flags & ~AT_SYMLINK_FOLLOW) == 0);

        જો (empty_or_મૂળ(મૂળ))
                પાછા is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, મૂળ,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ ખાલી, &dir_fd);
        જો (r < 0)
                પાછા r;

        પાછા is_mount_point_at(dir_fd, /* path= */ ખાલી, flags);
}

સ્થિર પૂર્ણાંક path_get_mnt_id_at_internal(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, bool unique, uint64_t *ret) {
        સંરચના statx sx;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(wildcard_fd_is_valid(dir_fd));
        પ્રતિજ્ઞા(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        જો (r < 0)
                પાછા r;

        *ret = sx.stx_mnt_id;
        પાછા 0;
}

પૂર્ણાંક path_get_mnt_id_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક *ret) {
        uint64_t mnt_id;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(mnt_id <= INT_MAX);
        *ret = (પૂર્ણાંક) mnt_id;
        પાછા 0;
}

પૂર્ણાંક path_get_unique_mnt_id_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, uint64_t *ret) {
        પાછા path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(અચલ અક્ષર *fstype) {
        અચલ અક્ષર *x;

        x = startswith(fstype, "fuse.");
        જો (x)
                fstype = x;

        જો (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].કિંમત, fstype))
                પાછા true;

        /* Filesystems not present in the internal database */
        પાછા STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(અચલ અક્ષર *fstype) {
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
        પાછા STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(અચલ અક્ષર *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        પાછા STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(અચલ અક્ષર *fstype) {
        પ્રતિજ્ઞા(fstype);

        અચલ FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                જો (nulstr_contains(fs->કિંમત, fstype))
                    પાછા true;

        /* Filesystems not present in the internal database */
        પાછા STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(અચલ અક્ષર *fstype) {
        અચલ અક્ષર *x;

        x = startswith(fstype, "fuse.");
        જો (x)
                fstype = x;

        પાછા !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(અચલ અક્ષર *fstype) {
        /* All Linux file systems that are necessarily read-only */
        પાછા STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(અચલ અક્ષર *fstype) {
        પ્રતિજ્ઞા(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        જો (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                પાછા true;

        /* On new kernels we can just ask the kernel */
        પાછા mount_option_supported(fstype, "discard", ખાલી) > 0;
}

અચલ અક્ષર* fstype_norecovery_option(અચલ અક્ષર *fstype) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        જો (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                પાછા "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        જો (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", ખાલી);
                જો (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        પાછા "norecovery";
                }
                જો (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                જો (r > 0)
                        પાછા "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        પાછા mount_option_supported(fstype, "norecovery", ખાલી) > 0 ? "norecovery" : ખાલી;
}

bool fstype_can_fmask_dmask(અચલ અક્ષર *fstype) {
        પ્રતિજ્ઞા(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        પાછા streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(અચલ અક્ષર *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        પાછા !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(અચલ અક્ષર *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        પાછા STR_IN_SET(fstype,
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

પૂર્ણાંક dev_is_devtmpfs(શૂન્ય) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = ખાલી;
        પૂર્ણાંક mount_id, r;
        અક્ષર *e;

        r = path_get_mnt_id("/dev", &mount_id);
        જો (r < 0)
                પાછા r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        જો (r == -ENOENT)
                પાછા proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        જો (r < 0)
                પાછા r;

        માટે (;;) {
                _cleanup_free_ અક્ષર *લીટી = ખાલી;
                પૂર્ણાંક mid;

                r = read_લીટી(proc_self_mountinfo, LONG_લીટી_MAX, &લીટી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                જો (sscanf(લીટી, "%i", &mid) != 1)
                        ચાલુ;

                જો (mid != mount_id)
                        ચાલુ;

                e = strstrafter(લીટી, " - ");
                જો (!e)
                        ચાલુ;

                /* accept any name that starts with the currently expected type */
                જો (startswith(e, "devtmpfs"))
                        પાછા true;
        }

        પાછા false;
}

સ્થિર પૂર્ણાંક mount_fd(
                અચલ અક્ષર *source,
                પૂર્ણાંક target_fd,
                અચલ અક્ષર *filesystemtype,
                unsigned long mountflags,
                અચલ શૂન્ય *data) {

        પ્રતિજ્ઞા(target_fd >= 0);

        જો (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                જો (errno != ENOENT)
                        પાછા -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                જો (proc_mounted() == 0)
                        પાછા -ENOSYS;

                પાછા -ENOENT;
        }

        પાછા 0;
}

પૂર્ણાંક mount_nofollow(
                અચલ અક્ષર *source,
                અચલ અક્ષર *target,
                અચલ અક્ષર *filesystemtype,
                unsigned long mountflags,
                અચલ શૂન્ય *data) {

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        જો (fd < 0)
                પાછા -errno;

        પાછા mount_fd(source, fd, filesystemtype, mountflags, data);
}

અચલ અક્ષર* mount_propagation_flag_to_string(unsigned long flags) {

        પસંદગી (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        સ્થિતિ 0:
                પાછા "";
        સ્થિતિ MS_SHARED:
                પાછા "shared";
        સ્થિતિ MS_SLAVE:
                પાછા "slave";
        સ્થિતિ MS_PRIVATE:
                પાછા "private";
        }

        પાછા ખાલી;
}

પૂર્ણાંક mount_propagation_flag_from_string(અચલ અક્ષર *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        પ્રતિજ્ઞા(ret);

        જો (isempty(name))
                *ret = 0;
        નહીંતર જો (streq(name, "shared"))
                *ret = MS_SHARED;
        નહીંતર જો (streq(name, "slave"))
                *ret = MS_SLAVE;
        નહીંતર જો (streq(name, "private"))
                *ret = MS_PRIVATE;
        નહીંતર
                પાછા -EINVAL;
        પાછા 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        પાછા IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(શૂન્ય) {
        સ્થિર પૂર્ણાંક cache = -1;
        પૂર્ણાંક r;

        જો (cache >= 0)
                પાછા cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, ખાલી, 0, ખાલી, 0));
        જો (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                પાછા (cache = false);

        પાછા (cache = true);
}

પૂર્ણાંક mount_option_supported(અચલ અક્ષર *fstype, અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        પ્રતિજ્ઞા(fstype);
        પ્રતિજ્ઞા(ચાવી);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        જો (fd < 0) {
                જો (errno == ENODEV)
                        પાછા log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                પાછા log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        જો (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", ખાલી, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                જો (errno == EOPNOTSUPP)
                        પાછા -EAGAIN; /* fs not converted to new mount API → don't know */
                જો (errno != EINVAL)
                        પાછા log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } નહીંતર
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        જો (કિંમત)
                r = fsconfig(fd, FSCONFIG_SET_STRING, ચાવી, કિંમત, 0);
        નહીંતર
                r = fsconfig(fd, FSCONFIG_SET_FLAG, ચાવી, ખાલી, 0);
        જો (r < 0) {
                જો (errno == EINVAL)
                        પાછા false; /* EINVAL means option not supported. */

                પાછા log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       ચાવી, કિંમત ? "=" : "", strempty(કિંમત), fstype);
        }

        પાછા true; /* works! */
}

bool path_below_api_vfs(અચલ અક્ષર *p) {
        પ્રતિજ્ઞા(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        પાછા PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
