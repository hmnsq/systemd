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

bool is_name_to_handle_at_fatal_error(整数 err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        assert(err < 0);

        もし (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                戻る false;
        もし (ERRNO_IS_NEG_PRIVILEGE(err))
                戻る false;

        戻る !IN_SET(err, -EOVERFLOW, -EINVAL);
}

整数 name_to_handle_at_loop(
                整数 fd,
                const char *path,
                struct file_handle **ret_handle,
                整数 *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                整数 flags) {

        整数 r;

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

        もし (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        ために (size_t n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ struct file_handle *h = NULL;

                h = malloc0(offsetof(struct file_handle, f_handle) + n);
                もし (!h)
                        戻る -ENOMEM;

                h->handle_bytes = n;

                もし (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (整数 *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        もし (r >= 0) {
                                もし (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                もし (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                戻る 1;
                        }
                        もし (errno == EOVERFLOW)
                                goto grow;
                        もし (errno != EINVAL)
                                戻る -errno;
                }

                整数 mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                もし (r >= 0) {
                        もし (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                struct statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                もし (r >= 0) {
                                        もし (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        もし (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        戻る 1;
                                }
                                もし (r != -EUNATCH || !ret_mnt_id)
                                        戻る r;

                                *ret_unique_mnt_id = 0;
                        }

                        もし (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        もし (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        戻る 0;
                }
                もし (errno != EOVERFLOW)
                        戻る -errno;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                もし (h->handle_bytes <= n)
                        戻る -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                もし (n > UINT_MAX - offsetof(struct file_handle, f_handle))
                        戻る -EOVERFLOW;
        }
}

整数 name_to_handle_at_try_fid(
                整数 fd,
                const char *path,
                struct file_handle **ret_handle,
                整数 *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                整数 flags) {

        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        もし (r >= 0 || is_name_to_handle_at_fatal_error(r))
                戻る r;

        戻る name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

整数 name_to_handle_at_u64(整数 fd, const char *path, uint64_t *ret) {
        _cleanup_free_ struct file_handle *h = NULL;
        整数 r;

        assert(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ NULL, /* ret_unique_mnt_id= */ NULL, /* flags= */ 0);
        もし (r < 0)
                戻る r;
        もし (h->handle_bytes < sizeof(uint64_t))
                戻る -EBADMSG;

        もし (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        戻る 0;
}

bool file_handle_equal(const struct file_handle *a, const struct file_handle *b) {
        もし (a == b)
                戻る true;
        もし (!a != !b)
                戻る false;
        もし (a->handle_type != b->handle_type)
                戻る false;

        戻る memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

struct file_handle* file_handle_dup(const struct file_handle *fh) {
        _cleanup_free_ struct file_handle *fh_copy = NULL;

        assert(fh);

        fh_copy = malloc0(offsetof(struct file_handle, f_handle) + fh->handle_bytes);
        もし (!fh_copy)
                戻る NULL;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        memcpy(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        戻る TAKE_PTR(fh_copy);
}

整数 is_mount_point_at(整数 dir_fd, const char *path, 整数 flags) {
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert((flags & ~AT_SYMLINK_FOLLOW) == 0);

        もし (path_equal(path, "/"))
                戻る true;

        もし (dir_fd == XAT_FDROOT && isempty(path))
                戻る true;

        struct statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_ROOT,
                        &sx);
        もし (r < 0)
                戻る r;

        もし (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT))
                戻る true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        struct statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        もし (r < 0)
                戻る r;

        戻る statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
整数 path_is_mount_point_full(const char *path, const char *root, 整数 flags) {
        _cleanup_close_ 整数 dir_fd = -EBADF;
        整数 r;

        assert(path);
        assert((flags & ~AT_SYMLINK_FOLLOW) == 0);

        もし (empty_or_root(root))
                戻る is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, root,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ NULL, &dir_fd);
        もし (r < 0)
                戻る r;

        戻る is_mount_point_at(dir_fd, /* path= */ NULL, flags);
}

static 整数 path_get_mnt_id_at_internal(整数 dir_fd, const char *path, bool unique, uint64_t *ret) {
        struct statx sx;
        整数 r;

        assert(wildcard_fd_is_valid(dir_fd));
        assert(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        もし (r < 0)
                戻る r;

        *ret = sx.stx_mnt_id;
        戻る 0;
}

整数 path_get_mnt_id_at(整数 dir_fd, const char *path, 整数 *ret) {
        uint64_t mnt_id;
        整数 r;

        assert(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        もし (r < 0)
                戻る r;

        assert(mnt_id <= INT_MAX);
        *ret = (整数) mnt_id;
        戻る 0;
}

整数 path_get_unique_mnt_id_at(整数 dir_fd, const char *path, uint64_t *ret) {
        戻る path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(const char *fstype) {
        const char *x;

        x = startswith(fstype, "fuse.");
        もし (x)
                fstype = x;

        もし (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].value, fstype))
                戻る true;

        /* Filesystems not present in the internal database */
        戻る STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(const char *fstype) {
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
        戻る STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(const char *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        戻る STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(const char *fstype) {
        assert(fstype);

        const FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                もし (nulstr_contains(fs->value, fstype))
                    戻る true;

        /* Filesystems not present in the internal database */
        戻る STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(const char *fstype) {
        const char *x;

        x = startswith(fstype, "fuse.");
        もし (x)
                fstype = x;

        戻る !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(const char *fstype) {
        /* All Linux file systems that are necessarily read-only */
        戻る STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(const char *fstype) {
        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        もし (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                戻る true;

        /* On new kernels we can just ask the kernel */
        戻る mount_option_supported(fstype, "discard", NULL) > 0;
}

const char* fstype_norecovery_option(const char *fstype) {
        整数 r;

        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        もし (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                戻る "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        もし (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", NULL);
                もし (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        戻る "norecovery";
                }
                もし (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                もし (r > 0)
                        戻る "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        戻る mount_option_supported(fstype, "norecovery", NULL) > 0 ? "norecovery" : NULL;
}

bool fstype_can_fmask_dmask(const char *fstype) {
        assert(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        戻る streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(const char *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        戻る !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(const char *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        戻る STR_IN_SET(fstype,
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

整数 dev_is_devtmpfs(void) {
        _cleanup_fclose_ FILE *proc_self_mountinfo = NULL;
        整数 mount_id, r;
        char *e;

        r = path_get_mnt_id("/dev", &mount_id);
        もし (r < 0)
                戻る r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        もし (r == -ENOENT)
                戻る proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        もし (r < 0)
                戻る r;

        ために (;;) {
                _cleanup_free_ char *line = NULL;
                整数 mid;

                r = read_line(proc_self_mountinfo, LONG_LINE_MAX, &line);
                もし (r < 0)
                        戻る r;
                もし (r == 0)
                        break;

                もし (sscanf(line, "%i", &mid) != 1)
                        continue;

                もし (mid != mount_id)
                        continue;

                e = strstrafter(line, " - ");
                もし (!e)
                        continue;

                /* accept any name that starts with the currently expected type */
                もし (startswith(e, "devtmpfs"))
                        戻る true;
        }

        戻る false;
}

static 整数 mount_fd(
                const char *source,
                整数 target_fd,
                const char *filesystemtype,
                unsigned long mountflags,
                const void *data) {

        assert(target_fd >= 0);

        もし (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, data) < 0) {
                もし (errno != ENOENT)
                        戻る -errno;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                もし (proc_mounted() == 0)
                        戻る -ENOSYS;

                戻る -ENOENT;
        }

        戻る 0;
}

整数 mount_nofollow(
                const char *source,
                const char *target,
                const char *filesystemtype,
                unsigned long mountflags,
                const void *data) {

        _cleanup_close_ 整数 fd = -EBADF;

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
        もし (fd < 0)
                戻る -errno;

        戻る mount_fd(source, fd, filesystemtype, mountflags, data);
}

const char* mount_propagation_flag_to_string(unsigned long flags) {

        switch (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        case 0:
                戻る "";
        case MS_SHARED:
                戻る "shared";
        case MS_SLAVE:
                戻る "slave";
        case MS_PRIVATE:
                戻る "private";
        }

        戻る NULL;
}

整数 mount_propagation_flag_from_string(const char *name, unsigned long *ret) {

        POINTER_MAY_BE_NULL(name);
        assert(ret);

        もし (isempty(name))
                *ret = 0;
        否则 もし (streq(name, "shared"))
                *ret = MS_SHARED;
        否则 もし (streq(name, "slave"))
                *ret = MS_SLAVE;
        否则 もし (streq(name, "private"))
                *ret = MS_PRIVATE;
        否则
                戻る -EINVAL;
        戻る 0;
}

bool mount_propagation_flag_is_valid(unsigned long flag) {
        戻る IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(void) {
        static 整数 cache = -1;
        整数 r;

        もし (cache >= 0)
                戻る cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, NULL, 0, NULL, 0));
        もし (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                戻る (cache = false);

        戻る (cache = true);
}

整数 mount_option_supported(const char *fstype, const char *key, const char *value) {
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        assert(fstype);
        assert(key);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        もし (fd < 0) {
                もし (errno == ENODEV)
                        戻る log_debug_errno(errno, "Kernel does not support file system type '%s'.", fstype);

                戻る log_debug_errno(errno, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        もし (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", NULL, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                もし (errno == EOPNOTSUPP)
                        戻る -EAGAIN; /* fs not converted to new mount API → don't know */
                もし (errno != EINVAL)
                        戻る log_debug_errno(errno, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } 否则
                戻る log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        もし (value)
                r = fsconfig(fd, FSCONFIG_SET_STRING, key, value, 0);
        否则
                r = fsconfig(fd, FSCONFIG_SET_FLAG, key, NULL, 0);
        もし (r < 0) {
                もし (errno == EINVAL)
                        戻る false; /* EINVAL means option not supported. */

                戻る log_debug_errno(errno, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       key, value ? "=" : "", strempty(value), fstype);
        }

        戻る true; /* works! */
}

bool path_below_api_vfs(const char *p) {
        assert(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        戻る PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
