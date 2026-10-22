/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/mount.h>

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fileio.h"
#include "filesystems.h"
#include "fs-util.h"
#include "log.h"
#include "mountpoint-util.h"
#include "nulstr-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"
#include "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#define ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(puur_naa_nka err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        assert(err < 0);

        ꦪꦼꦤ꧀ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (ERRNO_IS_NEG_PRIVILEGE(err))
                ꦮꦭꦶ false;

        ꦮꦭꦶ !IN_SET(err, -EOVERFLOW, -EINVAL);
}

puur_naa_nka name_to_handle_at_loop(
                puur_naa_nka fd,
                const var_na *path,
                sa_nracanaa file_handle **ret_handle,
                puur_naa_nka *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                puur_naa_nka flags) {

        puur_naa_nka r;

        assert(fd >= 0 || fd == AT_FDCWD);
        assert((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        ꦪꦼꦤ꧀ (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        ꦏꦁꦒꦺꦴ (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ sa_nracanaa file_handle *h = NULL;

                h = malloc0(offsetof(sa_nracanaa file_handle, f_handle) + n);
                ꦪꦼꦤ꧀ (!h)
                        ꦮꦭꦶ -ENOMEM;

                h->handle_bytes = n;

                ꦪꦼꦤ꧀ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (puur_naa_nka *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ꦪꦼꦤ꧀ (r >= 0) {
                                ꦪꦼꦤ꧀ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ꦪꦼꦤ꧀ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ꦮꦭꦶ 1;
                        }
                        ꦪꦼꦤ꧀ (errno == EOVERFLOW)
                                goto grow;
                        ꦪꦼꦤ꧀ (errno != EINVAL)
                                ꦮꦭꦶ -errno;
                }

                puur_naa_nka mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                ꦪꦼꦤ꧀ (r >= 0) {
                        ꦪꦼꦤ꧀ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                sa_nracanaa statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ꦪꦼꦤ꧀ (r >= 0) {
                                        ꦪꦼꦤ꧀ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ꦪꦼꦤ꧀ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ꦮꦭꦶ 1;
                                }
                                ꦪꦼꦤ꧀ (r != -EUNATCH || !ret_mnt_id)
                                        ꦮꦭꦶ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ꦪꦼꦤ꧀ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ꦪꦼꦤ꧀ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ꦮꦭꦶ 0;
                }
                ꦪꦼꦤ꧀ (errno != EOVERFLOW)
                        ꦮꦭꦶ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ꦪꦼꦤ꧀ (h->handle_bytes <= n)
                        ꦮꦭꦶ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ꦪꦼꦤ꧀ (n > UINT_MAX - offsetof(sa_nracanaa file_handle, f_handle))
                        ꦮꦭꦶ -EOVERFLOW;
        }
}

puur_naa_nka name_to_handle_at_try_fid(
                puur_naa_nka fd,
                const var_na *path,
                sa_nracanaa file_handle **ret_handle,
                puur_naa_nka *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                puur_naa_nka flags) {

        puur_naa_nka r;

        assert(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ꦪꦼꦤ꧀ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                ꦮꦭꦶ r;

        ꦮꦭꦶ name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

puur_naa_nka name_to_handle_at_u64(puur_naa_nka fd, const var_na *path, uint64_t *ret) {
        _cleanup_free_ sa_nracanaa file_handle *h = NULL;
        puur_naa_nka r;

        assert(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ NULL, /* ret_unique_mnt_id= */ NULL, /* flags= */ 0);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;
        ꦪꦼꦤ꧀ (h->handle_bytes < parimaa_na(uint64_t))
                ꦮꦭꦶ -EBADMSG;

        ꦪꦼꦤ꧀ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ꦮꦭꦶ 0;
}

bool file_handle_equal(const sa_nracanaa file_handle *a, const sa_nracanaa file_handle *b) {
        ꦪꦼꦤ꧀ (a == b)
                ꦮꦭꦶ true;
        ꦪꦼꦤ꧀ (!a != !b)
                ꦮꦭꦶ false;
        ꦪꦼꦤ꧀ (a->handle_type != b->handle_type)
                ꦮꦭꦶ false;

        ꦮꦭꦶ memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

sa_nracanaa file_handle* file_handle_dup(const sa_nracanaa file_handle *fh) {
        _cleanup_free_ sa_nracanaa file_handle *fh_copy = NULL;

        assert(fh);

        fh_copy = malloc0(offsetof(sa_nracanaa file_handle, f_handle) + fh->handle_bytes);
        ꦪꦼꦤ꧀ (!fh_copy)
                ꦮꦭꦶ NULL;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        ꦮꦭꦶ TAKE_PTR(fh_copy);
}

puur_naa_nka is_mount_point_at(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka flags) {
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ꦪꦼꦤ꧀ (path_equal(path, "/"))
                ꦮꦭꦶ true;

        ꦪꦼꦤ꧀ (dir_fd == XAT_FDROOT && isempty(path))
                ꦮꦭꦶ true;

        sa_nracanaa statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ROOT,
                        &sx);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT))
                ꦮꦭꦶ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        sa_nracanaa statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
puur_naa_nka path_is_mount_point_full(const var_na *path, const var_na *root, puur_naa_nka flags) {
        _cleanup_close_ puur_naa_nka dir_fd = -EBADF;
        puur_naa_nka r;

        assert(path);
        assert((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ꦪꦼꦤ꧀ (empty_or_root(root))
                ꦮꦭꦶ is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, root,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ NULL, &dir_fd);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦮꦭꦶ is_mount_point_at(dir_fd, /* path= */ NULL, flags);
}

static puur_naa_nka path_get_mnt_id_at_internal(puur_naa_nka dir_fd, const var_na *path, bool unique, uint64_t *ret) {
        sa_nracanaa statx sx;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        *ret = sx.stx_mnt_id;
        ꦮꦭꦶ 0;
}

puur_naa_nka path_get_mnt_id_at(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka *ret) {
        uint64_t mnt_id;
        puur_naa_nka r;

        assert(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        assert(mnt_id <= INT_MAX);
        *ret = (puur_naa_nka) mnt_id;
        ꦮꦭꦶ 0;
}

puur_naa_nka path_get_unique_mnt_id_at(puur_naa_nka dir_fd, const var_na *path, uint64_t *ret) {
        ꦮꦭꦶ path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(const var_na *fstype) {
        const var_na *x;

        x = startswith(fstype, "fuse.");
        ꦪꦼꦤ꧀ (x)
                fstype = x;

        ꦪꦼꦤ꧀ (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].value, fstype))
                ꦮꦭꦶ true;

        /* Filesystems not present in the internal database */
        ꦮꦭꦶ STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(const var_na *fstype) {
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
        ꦮꦭꦶ STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(const var_na *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        ꦮꦭꦶ STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(const var_na *fstype) {
        assert(fstype);

        const FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                ꦪꦼꦤ꧀ (nulstr_contains(fs->value, fstype))
                    ꦮꦭꦶ true;

        /* Filesystems not present in the internal database */
        ꦮꦭꦶ STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(const var_na *fstype) {
        const var_na *x;

        x = startswith(fstype, "fuse.");
        ꦪꦼꦤ꧀ (x)
                fstype = x;

        ꦮꦭꦶ !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(const var_na *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ꦮꦭꦶ STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(const var_na *fstype) {
        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ꦪꦼꦤ꧀ (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ꦮꦭꦶ true;

        /* On new kernels we can just ask the kernel */
        ꦮꦭꦶ mount_option_supported(fstype, "discard", NULL) > 0;
}

const var_na* fstype_norecovery_option(const var_na *fstype) {
        puur_naa_nka r;

        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ꦪꦼꦤ꧀ (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                ꦮꦭꦶ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ꦪꦼꦤ꧀ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", NULL);
                ꦪꦼꦤ꧀ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ꦮꦭꦶ "norecovery";
                }
                ꦪꦼꦤ꧀ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ꦪꦼꦤ꧀ (r > 0)
                        ꦮꦭꦶ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ꦮꦭꦶ mount_option_supported(fstype, "norecovery", NULL) > 0 ? "norecovery" : NULL;
}

bool fstype_can_fmask_dmask(const var_na *fstype) {
        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ꦮꦭꦶ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(const var_na *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ꦮꦭꦶ !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(const var_na *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        ꦮꦭꦶ STR_IN_SET(fstype,
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

puur_naa_nka dev_is_devtmpfs(void) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = NULL;
        puur_naa_nka mount_id, r;
        var_na *e;

        r = path_get_mnt_id("/dev", &mount_id);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ꦪꦼꦤ꧀ (r == -ENOENT)
                ꦮꦭꦶ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦏꦁꦒꦺꦴ (;;) {
                _cleanup_free_ var_na *line = NULL;
                puur_naa_nka mid;

                r = read_line(proc_self_mountinfo, LONG_LINE_MAX, &line);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;
                ꦪꦼꦤ꧀ (r == 0)
                        bha_nga;

                ꦪꦼꦤ꧀ (sscanf(line, "%i", &mid) != 1)
                        continue;

                ꦪꦼꦤ꧀ (mid != mount_id)
                        continue;

                e = strstrafter(line, " - ");
                ꦪꦼꦤ꧀ (!e)
                        continue;

                /* accept any name that starts with the currently expected type */
                ꦪꦼꦤ꧀ (startswith(e, "devtmpfs"))
                        ꦮꦭꦶ true;
        }

        ꦮꦭꦶ false;
}

static puur_naa_nka mount_fd(
                const var_na *source,
                puur_naa_nka target_fd,
                const var_na *filesystemtype,
                unsigned long mountflags,
                const void *data) {

        assert(target_fd >= 0);

        ꦪꦼꦤ꧀ (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                ꦪꦼꦤ꧀ (errno != ENOENT)
                        ꦮꦭꦶ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ꦪꦼꦤ꧀ (proc_mounted() == 0)
                        ꦮꦭꦶ -ENOSYS;

                ꦮꦭꦶ -ENOENT;
        }

        ꦮꦭꦶ 0;
}

puur_naa_nka mount_nofollow(
                const var_na *source,
                const var_na *target,
                const var_na *filesystemtype,
                unsigned long mountflags,
                const void *data) {

        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = open(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ꦪꦼꦤ꧀ (fd < 0)
                ꦮꦭꦶ -errno;

        ꦮꦭꦶ mount_fd(source, fd, filesystemtype, mountflags, data);
}

const var_na* mount_propagation_flag_to_string(unsigned long flags) {

        switch (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        pak_sa 0:
                ꦮꦭꦶ "";
        pak_sa MS_SHARED:
                ꦮꦭꦶ "shared";
        pak_sa MS_SLAVE:
                ꦮꦭꦶ "slave";
        pak_sa MS_PRIVATE:
                ꦮꦭꦶ "private";
        }

        ꦮꦭꦶ NULL;
}

puur_naa_nka mount_propagation_flag_from_string(const var_na *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        assert(ret);

        ꦪꦼꦤ꧀ (isempty(name))
                *ret = 0;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (streq(name, "shared"))
                *ret = MS_SHARED;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (streq(name, "slave"))
                *ret = MS_SLAVE;
        ꦭꦶꦪ ꦪꦼꦤ꧀ (streq(name, "private"))
                *ret = MS_PRIVATE;
        ꦭꦶꦪ
                ꦮꦭꦶ -EINVAL;
        ꦮꦭꦶ 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        ꦮꦭꦶ IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(void) {
        static puur_naa_nka cache = -1;
        puur_naa_nka r;

        ꦪꦼꦤ꧀ (cache >= 0)
                ꦮꦭꦶ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, NULL, 0, NULL, 0));
        ꦪꦼꦤ꧀ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ꦮꦭꦶ (cache = false);

        ꦮꦭꦶ (cache = true);
}

puur_naa_nka mount_option_supported(const var_na *fstype, const var_na *key, const var_na *value) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        assert(fstype);
        assert(key);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ꦪꦼꦤ꧀ (fd < 0) {
                ꦪꦼꦤ꧀ (errno == ENODEV)
                        ꦮꦭꦶ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ꦮꦭꦶ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ꦪꦼꦤ꧀ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", NULL, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ꦪꦼꦤ꧀ (errno == EOPNOTSUPP)
                        ꦮꦭꦶ -EAGAIN; /* fs not converted to new mount API → don't know */
                ꦪꦼꦤ꧀ (errno != EINVAL)
                        ꦮꦭꦶ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } ꦭꦶꦪ
                ꦮꦭꦶ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ꦪꦼꦤ꧀ (value)
                r = fsconfig(fd, FSCONFIG_SET_STRING, key, value, 0);
        ꦭꦶꦪ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, key, NULL, 0);
        ꦪꦼꦤ꧀ (r < 0) {
                ꦪꦼꦤ꧀ (errno == EINVAL)
                        ꦮꦭꦶ false; /* EINVAL means option not supported. */

                ꦮꦭꦶ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       key, value ? "=" : "", strempty(value), fstype);
        }

        ꦮꦭꦶ true; /* works! */
}

bool path_below_api_vfs(const var_na *p) {
        assert(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ꦮꦭꦶ PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
