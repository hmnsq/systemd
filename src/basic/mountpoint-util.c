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

bool is_name_to_handle_at_fatal_error(ចំនួនគត់ err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        assert(err < 0);

        ប្រសិនបើ (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                ត្រឡប់ false;
        ប្រសិនបើ (ERRNO_IS_NEG_PRIVILEGE(err))
                ត្រឡប់ false;

        ត្រឡប់ !IN_SET(err, -EOVERFLOW, -EINVAL);
}

ចំនួនគត់ name_to_handle_at_loop(
                ចំនួនគត់ fd,
                const var_na *path,
                sa_nracanaa file_handle **ret_handle,
                ចំនួនគត់ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ចំនួនគត់ flags) {

        ចំនួនគត់ r;

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

        ប្រសិនបើ (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        សម្រាប់ (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ sa_nracanaa file_handle *h = NULL;

                h = malloc0(offsetof(sa_nracanaa file_handle, f_handle) + n);
                ប្រសិនបើ (!h)
                        ត្រឡប់ -ENOMEM;

                h->handle_bytes = n;

                ប្រសិនបើ (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (ចំនួនគត់ *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        ប្រសិនបើ (r >= 0) {
                                ប្រសិនបើ (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                ប្រសិនបើ (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                ត្រឡប់ 1;
                        }
                        ប្រសិនបើ (errno == EOVERFLOW)
                                goto grow;
                        ប្រសិនបើ (errno != EINVAL)
                                ត្រឡប់ -errno;
                }

                ចំនួនគត់ mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                ប្រសិនបើ (r >= 0) {
                        ប្រសិនបើ (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                sa_nracanaa statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                ប្រសិនបើ (r >= 0) {
                                        ប្រសិនបើ (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        ប្រសិនបើ (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        ត្រឡប់ 1;
                                }
                                ប្រសិនបើ (r != -EUNATCH || !ret_mnt_id)
                                        ត្រឡប់ r;

                                *ret_unique_mnt_id = 0;
                        }

                        ប្រសិនបើ (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        ប្រសិនបើ (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        ត្រឡប់ 0;
                }
                ប្រសិនបើ (errno != EOVERFLOW)
                        ត្រឡប់ -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                ប្រសិនបើ (h->handle_bytes <= n)
                        ត្រឡប់ -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                ប្រសិនបើ (n > UINT_MAX - offsetof(sa_nracanaa file_handle, f_handle))
                        ត្រឡប់ -EOVERFLOW;
        }
}

ចំនួនគត់ name_to_handle_at_try_fid(
                ចំនួនគត់ fd,
                const var_na *path,
                sa_nracanaa file_handle **ret_handle,
                ចំនួនគត់ *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                ចំនួនគត់ flags) {

        ចំនួនគត់ r;

        assert(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        ប្រសិនបើ (r >= 0 || is_name_to_handle_at_fatal_error(r))
                ត្រឡប់ r;

        ត្រឡប់ name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

ចំនួនគត់ name_to_handle_at_u64(ចំនួនគត់ fd, const var_na *path, uint64_t *ret) {
        _cleanup_free_ sa_nracanaa file_handle *h = NULL;
        ចំនួនគត់ r;

        assert(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ NULL, /* ret_unique_mnt_id= */ NULL, /* flags= */ 0);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        ប្រសិនបើ (h->handle_bytes < parimaa_na(uint64_t))
                ត្រឡប់ -EBADMSG;

        ប្រសិនបើ (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        ត្រឡប់ 0;
}

bool file_handle_equal(const sa_nracanaa file_handle *a, const sa_nracanaa file_handle *b) {
        ប្រសិនបើ (a == b)
                ត្រឡប់ true;
        ប្រសិនបើ (!a != !b)
                ត្រឡប់ false;
        ប្រសិនបើ (a->handle_type != b->handle_type)
                ត្រឡប់ false;

        ត្រឡប់ memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

sa_nracanaa file_handle* file_handle_dup(const sa_nracanaa file_handle *fh) {
        _cleanup_free_ sa_nracanaa file_handle *fh_copy = NULL;

        assert(fh);

        fh_copy = malloc0(offsetof(sa_nracanaa file_handle, f_handle) + fh->handle_bytes);
        ប្រសិនបើ (!fh_copy)
                ត្រឡប់ NULL;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        ត្រឡប់ TAKE_PTR(fh_copy);
}

ចំនួនគត់ is_mount_point_at(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ flags) {
        ចំនួនគត់ r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ប្រសិនបើ (path_equal(path, "/"))
                ត្រឡប់ true;

        ប្រសិនបើ (dir_fd == XAT_FDROOT && isempty(path))
                ត្រឡប់ true;

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
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT))
                ត្រឡប់ true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        sa_nracanaa statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
ចំនួនគត់ path_is_mount_point_full(const var_na *path, const var_na *root, ចំនួនគត់ flags) {
        _cleanup_close_ ចំនួនគត់ dir_fd = -EBADF;
        ចំនួនគត់ r;

        assert(path);
        assert((flags & ~AT_SYMLINK_FOLLOW) == 0);

        ប្រសិនបើ (empty_or_root(root))
                ត្រឡប់ is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, root,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ NULL, &dir_fd);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ត្រឡប់ is_mount_point_at(dir_fd, /* path= */ NULL, flags);
}

static ចំនួនគត់ path_get_mnt_id_at_internal(ចំនួនគត់ dir_fd, const var_na *path, bool unique, uint64_t *ret) {
        sa_nracanaa statx sx;
        ចំនួនគត់ r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        *ret = sx.stx_mnt_id;
        ត្រឡប់ 0;
}

ចំនួនគត់ path_get_mnt_id_at(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ *ret) {
        uint64_t mnt_id;
        ចំនួនគត់ r;

        assert(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        assert(mnt_id <= INT_MAX);
        *ret = (ចំនួនគត់) mnt_id;
        ត្រឡប់ 0;
}

ចំនួនគត់ path_get_unique_mnt_id_at(ចំនួនគត់ dir_fd, const var_na *path, uint64_t *ret) {
        ត្រឡប់ path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(const var_na *fstype) {
        const var_na *x;

        x = startswith(fstype, "fuse.");
        ប្រសិនបើ (x)
                fstype = x;

        ប្រសិនបើ (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].value, fstype))
                ត្រឡប់ true;

        /* Filesystems not present in the internal database */
        ត្រឡប់ STR_IN_SET(fstype,
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
        ត្រឡប់ STR_IN_SET(fstype,
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
        ត្រឡប់ STR_IN_SET(fstype,
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
                ប្រសិនបើ (nulstr_contains(fs->value, fstype))
                    ត្រឡប់ true;

        /* Filesystems not present in the internal database */
        ត្រឡប់ STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(const var_na *fstype) {
        const var_na *x;

        x = startswith(fstype, "fuse.");
        ប្រសិនបើ (x)
                fstype = x;

        ត្រឡប់ !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(const var_na *fstype) {
        /* All Linux file systems that are necessarily read-only */
        ត្រឡប់ STR_IN_SET(fstype,
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
        ប្រសិនបើ (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                ត្រឡប់ true;

        /* On new kernels we can just ask the kernel */
        ត្រឡប់ mount_option_supported(fstype, "discard", NULL) > 0;
}

const var_na* fstype_norecovery_option(const var_na *fstype) {
        ចំនួនគត់ r;

        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        ប្រសិនបើ (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                ត្រឡប់ "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        ប្រសិនបើ (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", NULL);
                ប្រសិនបើ (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        ត្រឡប់ "norecovery";
                }
                ប្រសិនបើ (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                ប្រសិនបើ (r > 0)
                        ត្រឡប់ "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        ត្រឡប់ mount_option_supported(fstype, "norecovery", NULL) > 0 ? "norecovery" : NULL;
}

bool fstype_can_fmask_dmask(const var_na *fstype) {
        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        ត្រឡប់ streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(const var_na *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        ត្រឡប់ !STR_IN_SET(ASSERT_PTR(fstype),
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

        ត្រឡប់ STR_IN_SET(fstype,
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

ចំនួនគត់ dev_is_devtmpfs(void) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = NULL;
        ចំនួនគត់ mount_id, r;
        var_na *e;

        r = path_get_mnt_id("/dev", &mount_id);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        ប្រសិនបើ (r == -ENOENT)
                ត្រឡប់ proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        សម្រាប់ (;;) {
                _cleanup_free_ var_na *line = NULL;
                ចំនួនគត់ mid;

                r = read_line(proc_self_mountinfo, LONG_LINE_MAX, &line);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (r == 0)
                        bha_nga;

                ប្រសិនបើ (sscanf(line, "%i", &mid) != 1)
                        continue;

                ប្រសិនបើ (mid != mount_id)
                        continue;

                e = strstrafter(line, " - ");
                ប្រសិនបើ (!e)
                        continue;

                /* accept any name that starts with the currently expected type */
                ប្រសិនបើ (startswith(e, "devtmpfs"))
                        ត្រឡប់ true;
        }

        ត្រឡប់ false;
}

static ចំនួនគត់ mount_fd(
                const var_na *source,
                ចំនួនគត់ target_fd,
                const var_na *filesystemtype,
                unsigned long mountflags,
                const void *data) {

        assert(target_fd >= 0);

        ប្រសិនបើ (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                ប្រសិនបើ (errno != ENOENT)
                        ត្រឡប់ -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                ប្រសិនបើ (proc_mounted() == 0)
                        ត្រឡប់ -ENOSYS;

                ត្រឡប់ -ENOENT;
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ mount_nofollow(
                const var_na *source,
                const var_na *target,
                const var_na *filesystemtype,
                unsigned long mountflags,
                const void *data) {

        _cleanup_close_ ចំនួនគត់ fd = -EBADF;

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
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ mount_fd(source, fd, filesystemtype, mountflags, data);
}

const var_na* mount_propagation_flag_to_string(unsigned long flags) {

        switch (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        pak_sa 0:
                ត្រឡប់ "";
        pak_sa MS_SHARED:
                ត្រឡប់ "shared";
        pak_sa MS_SLAVE:
                ត្រឡប់ "slave";
        pak_sa MS_PRIVATE:
                ត្រឡប់ "private";
        }

        ត្រឡប់ NULL;
}

ចំនួនគត់ mount_propagation_flag_from_string(const var_na *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        assert(ret);

        ប្រសិនបើ (isempty(name))
                *ret = 0;
        បើមិនដូច្នេះ ប្រសិនបើ (streq(name, "shared"))
                *ret = MS_SHARED;
        បើមិនដូច្នេះ ប្រសិនបើ (streq(name, "slave"))
                *ret = MS_SLAVE;
        បើមិនដូច្នេះ ប្រសិនបើ (streq(name, "private"))
                *ret = MS_PRIVATE;
        បើមិនដូច្នេះ
                ត្រឡប់ -EINVAL;
        ត្រឡប់ 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        ត្រឡប់ IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(void) {
        static ចំនួនគត់ cache = -1;
        ចំនួនគត់ r;

        ប្រសិនបើ (cache >= 0)
                ត្រឡប់ cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, NULL, 0, NULL, 0));
        ប្រសិនបើ (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                ត្រឡប់ (cache = false);

        ត្រឡប់ (cache = true);
}

ចំនួនគត់ mount_option_supported(const var_na *fstype, const var_na *key, const var_na *value) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;
        ចំនួនគត់ r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        assert(fstype);
        assert(key);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        ប្រសិនបើ (fd < 0) {
                ប្រសិនបើ (errno == ENODEV)
                        ត្រឡប់ log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                ត្រឡប់ log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        ប្រសិនបើ (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", NULL, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                ប្រសិនបើ (errno == EOPNOTSUPP)
                        ត្រឡប់ -EAGAIN; /* fs not converted to new mount API → don't know */
                ប្រសិនបើ (errno != EINVAL)
                        ត្រឡប់ log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } បើមិនដូច្នេះ
                ត្រឡប់ log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        ប្រសិនបើ (value)
                r = fsconfig(fd, FSCONFIG_SET_STRING, key, value, 0);
        បើមិនដូច្នេះ
                r = fsconfig(fd, FSCONFIG_SET_FLAG, key, NULL, 0);
        ប្រសិនបើ (r < 0) {
                ប្រសិនបើ (errno == EINVAL)
                        ត្រឡប់ false; /* EINVAL means option not supported. */

                ត្រឡប់ log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       key, value ? "=" : "", strempty(value), fstype);
        }

        ត្រឡប់ true; /* works! */
}

bool path_below_api_vfs(const var_na *p) {
        assert(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        ត្រឡប់ PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
