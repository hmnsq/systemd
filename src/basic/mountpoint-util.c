/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <sys/mount.h>

#समावेश "alloc-util.h"
#समावेश "chase.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "filesystems.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "mountpoint-util.h"
#समावेश "nulstr-util.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "unaligned.h"

/* This is the original MAX_HANDLE_SZ definition from the kernel, when the API was introduced. We use that in place of
 * any more currently defined value to future-proof things: if the size is increased in the API headers, and our code
 * is recompiled then it would cease working on old kernels, as those refuse any sizes larger than this value with
 * EINVAL right-away. Hence, let's disconnect ourselves from any such API changes, and stick to the original definition
 * from when it was introduced. We use it as a start value only anyway (see below), and hence should be able to deal
 * with large file handles anyway. */
#घोषणा ORIGINAL_MAX_HANDLE_SZ 128

bool is_name_to_handle_at_fatal_error(पूर्णांक err) {
        /* name_to_handle_at() can return "acceptable" errors that are due to the context. For example
         * the file system does not support name_to_handle_at() (EOPNOTSUPP), or the syscall was blocked
         * (EACCES/EPERM; maybe through seccomp, because we are running inside of a container), or
         * the mount point is not triggered yet (EOVERFLOW, think autofs+nfs4), or some general name_to_handle_at()
         * flakiness (EINVAL). However other errors are not supposed to happen and therefore are considered
         * fatal ones. */

        निश्चित(err < 0);

        अगर (ERRNO_IS_NEG_NOT_SUPPORTED(err))
                वापस false;
        अगर (ERRNO_IS_NEG_PRIVILEGE(err))
                वापस false;

        वापस !IN_SET(err, -EOVERFLOW, -EINVAL);
}

पूर्णांक name_to_handle_at_loop(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                काष्ठा file_handle **ret_handle,
                पूर्णांक *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                पूर्णांक flags) {

        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);
        निश्चित((flags & ~(AT_SYMLINK_FOLLOW|AT_EMPTY_PATH|AT_HANDLE_FID)) == 0);

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

        अगर (isempty(path)) {
                flags |= AT_EMPTY_PATH;
                path = "";
        }

        क्रम (माप_प्रकार n = ORIGINAL_MAX_HANDLE_SZ;;) {
                _cleanup_free_ काष्ठा file_handle *h = शून्य;

                h = malloc0(दुरत्व(काष्ठा file_handle, f_handle) + n);
                अगर (!h)
                        वापस -ENOMEM;

                h->handle_bytes = n;

                अगर (ret_unique_mnt_id) {
                        /* Here, explicitly initialize mnt_id, otherwise valgrind complains:
                         *
                         * ==175708== Conditional jump or move depends on uninitialised value(s)
                         * ==175708==    at 0x4BC33D1: inode_same_at (stat-util.c:610)
                         * ==175708==    by 0x4BF1972: inode_same (stat-util.h:86)
                         */
                        uint64_t mnt_id = 0;

                        /* The kernel will still use this as uint64_t pointer */
                        r = name_to_handle_at(fd, path, h, (पूर्णांक *) &mnt_id, flags|AT_HANDLE_MNT_ID_UNIQUE);
                        अगर (r >= 0) {
                                अगर (ret_handle)
                                        *ret_handle = TAKE_PTR(h);

                                *ret_unique_mnt_id = mnt_id;

                                अगर (ret_mnt_id)
                                        *ret_mnt_id = -1;

                                वापस 1;
                        }
                        अगर (त्रुटि_सं == EOVERFLOW)
                                जाओ grow;
                        अगर (त्रुटि_सं != EINVAL)
                                वापस -त्रुटि_सं;
                }

                पूर्णांक mnt_id;
                r = name_to_handle_at(fd, path, h, &mnt_id, flags);
                अगर (r >= 0) {
                        अगर (ret_unique_mnt_id) {
                                /* Hmm, AT_HANDLE_MNT_ID_UNIQUE is not supported? Let's try to acquire
                                 * the unique mount id from statx() then, which has a slightly lower
                                 * kernel version requirement (6.8 vs 6.12). */

                                काष्ठा statx sx;
                                r = xstatx(fd, path,
                                           at_flags_normalize_nofollow(flags & (AT_SYMLINK_FOLLOW|AT_EMPTY_PATH))|AT_STATX_DONT_SYNC,
                                           STATX_MNT_ID_UNIQUE,
                                           &sx);
                                अगर (r >= 0) {
                                        अगर (ret_handle)
                                                *ret_handle = TAKE_PTR(h);

                                        *ret_unique_mnt_id = sx.stx_mnt_id;

                                        अगर (ret_mnt_id)
                                                *ret_mnt_id = -1;

                                        वापस 1;
                                }
                                अगर (r != -EUNATCH || !ret_mnt_id)
                                        वापस r;

                                *ret_unique_mnt_id = 0;
                        }

                        अगर (ret_handle)
                                *ret_handle = TAKE_PTR(h);

                        अगर (ret_mnt_id)
                                *ret_mnt_id = mnt_id;

                        वापस 0;
                }
                अगर (त्रुटि_सं != EOVERFLOW)
                        वापस -त्रुटि_सं;

        grow:
                /* If name_to_handle_at() didn't increase the byte size, then this EOVERFLOW is caused by
                 * something else (apparently EOVERFLOW is returned for untriggered nfs4 autofs mounts
                 * sometimes), not by the too small buffer. In that case propagate EOVERFLOW */
                अगर (h->handle_bytes <= n)
                        वापस -EOVERFLOW;

                /* The buffer was too small. Size the new buffer by what name_to_handle_at() returned. */
                n = h->handle_bytes;

                /* paranoia: check for overflow (note that .handle_bytes is unsigned only) */
                अगर (n > अच_पूर्णांक_उच्च - दुरत्व(काष्ठा file_handle, f_handle))
                        वापस -EOVERFLOW;
        }
}

पूर्णांक name_to_handle_at_try_fid(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                काष्ठा file_handle **ret_handle,
                पूर्णांक *ret_mnt_id,
                uint64_t *ret_unique_mnt_id,
                पूर्णांक flags) {

        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);

        /* First issues name_to_handle_at() with AT_HANDLE_FID. If this fails and this is not a fatal error
         * we'll try without the flag, in order to support older kernels that didn't have AT_HANDLE_FID
         * (i.e. older than Linux 6.5). */

        r = name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags | AT_HANDLE_FID);
        अगर (r >= 0 || is_name_to_handle_at_fatal_error(r))
                वापस r;

        वापस name_to_handle_at_loop(fd, path, ret_handle, ret_mnt_id, ret_unique_mnt_id, flags & ~AT_HANDLE_FID);
}

पूर्णांक name_to_handle_at_u64(पूर्णांक fd, स्थिर अक्षर *path, uint64_t *ret) {
        _cleanup_free_ काष्ठा file_handle *h = शून्य;
        पूर्णांक r;

        निश्चित(fd >= 0 || fd == AT_FDCWD);

        /* This provides the first 64bit of the file handle. */

        r = name_to_handle_at_loop(fd, path, &h, /* ret_mnt_id= */ शून्य, /* ret_unique_mnt_id= */ शून्य, /* flags= */ 0);
        अगर (r < 0)
                वापस r;
        अगर (h->handle_bytes < माप(uint64_t))
                वापस -EBADMSG;

        अगर (ret)
                /* Note, "struct file_handle" is 32bit aligned usually, but we need to read a 64bit value from it */
                *ret = unaligned_read_ne64(h->f_handle);

        वापस 0;
}

bool file_handle_equal(स्थिर काष्ठा file_handle *a, स्थिर काष्ठा file_handle *b) {
        अगर (a == b)
                वापस true;
        अगर (!a != !b)
                वापस false;
        अगर (a->handle_type != b->handle_type)
                वापस false;

        वापस memcmp_nn(a->f_handle, a->handle_bytes, b->f_handle, b->handle_bytes) == 0;
}

काष्ठा file_handle* file_handle_dup(स्थिर काष्ठा file_handle *fh) {
        _cleanup_free_ काष्ठा file_handle *fh_copy = शून्य;

        निश्चित(fh);

        fh_copy = malloc0(दुरत्व(काष्ठा file_handle, f_handle) + fh->handle_bytes);
        अगर (!fh_copy)
                वापस शून्य;

        fh_copy->handle_bytes = fh->handle_bytes;
        fh_copy->handle_type = fh->handle_type;
        स_नकल(fh_copy->f_handle, fh->f_handle, fh->handle_bytes);

        वापस TAKE_PTR(fh_copy);
}

पूर्णांक is_mount_point_at(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक flags) {
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित((flags & ~AT_SYMLINK_FOLLOW) == 0);

        अगर (path_equal(path, "/"))
                वापस true;

        अगर (dir_fd == XAT_FDROOT && isempty(path))
                वापस true;

        काष्ठा statx sx;
        r = xstatx_full(dir_fd, path,
                        at_flags_normalize_nofollow(flags) |
                        AT_NO_AUTOMOUNT |            /* don't trigger automounts – mounts are a local concept, hence no need to trigger automounts to determine STATX_ATTR_MOUNT_ROOT */
                        AT_STATX_DONT_SYNC,          /* don't go to the network for this – for similar reasons */
                        /* xstatx_flags = */ 0,
                        STATX_TYPE|STATX_INO,
                        /* optional_mask = */ 0,
                        STATX_ATTR_MOUNT_मूल,
                        &sx);
        अगर (r < 0)
                वापस r;

        अगर (FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_मूल))
                वापस true;

        /* When running on chroot environment, the root may not be a mount point, but we unconditionally
         * return true when the input is "/" in the above, but the shortcut may not work e.g. when the path
         * is relative. */
        काष्ठा statx sx2;
        r = xstatx(AT_FDCWD,
                   "/",
                   AT_STATX_DONT_SYNC,
                   STATX_TYPE|STATX_INO,
                   &sx2);
        अगर (r < 0)
                वापस r;

        वापस statx_inode_same(&sx, &sx2);
}

/* flags can be AT_SYMLINK_FOLLOW or 0 */
पूर्णांक path_is_mount_point_full(स्थिर अक्षर *path, स्थिर अक्षर *मूल, पूर्णांक flags) {
        _cleanup_close_ पूर्णांक dir_fd = -EBADF;
        पूर्णांक r;

        निश्चित(path);
        निश्चित((flags & ~AT_SYMLINK_FOLLOW) == 0);

        अगर (empty_or_मूल(मूल))
                वापस is_mount_point_at(AT_FDCWD, path, flags);

        r = chase(path, मूल,
                  FLAGS_SET(flags, AT_SYMLINK_FOLLOW) ? 0 : CHASE_NOFOLLOW,
                  /* ret_path= */ शून्य, &dir_fd);
        अगर (r < 0)
                वापस r;

        वापस is_mount_point_at(dir_fd, /* path= */ शून्य, flags);
}

अटल पूर्णांक path_get_mnt_id_at_internal(पूर्णांक dir_fd, स्थिर अक्षर *path, bool unique, uint64_t *ret) {
        काष्ठा statx sx;
        पूर्णांक r;

        निश्चित(wildcard_fd_is_valid(dir_fd));
        निश्चित(ret);

        r = xstatx(dir_fd, path,
                   AT_SYMLINK_NOFOLLOW |
                   AT_NO_AUTOMOUNT |    /* don't trigger automounts, mnt_id is a local concept */
                   AT_STATX_DONT_SYNC,  /* don't go to the network, mnt_id is a local concept */
                   unique ? STATX_MNT_ID_UNIQUE : STATX_MNT_ID,
                   &sx);
        अगर (r < 0)
                वापस r;

        *ret = sx.stx_mnt_id;
        वापस 0;
}

पूर्णांक path_get_mnt_id_at(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक *ret) {
        uint64_t mnt_id;
        पूर्णांक r;

        निश्चित(ret);

        r = path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ false, &mnt_id);
        अगर (r < 0)
                वापस r;

        निश्चित(mnt_id <= पूर्णांक_उच्च);
        *ret = (पूर्णांक) mnt_id;
        वापस 0;
}

पूर्णांक path_get_unique_mnt_id_at(पूर्णांक dir_fd, स्थिर अक्षर *path, uint64_t *ret) {
        वापस path_get_mnt_id_at_internal(dir_fd, path, /* unique = */ true, ret);
}

bool fstype_is_network(स्थिर अक्षर *fstype) {
        स्थिर अक्षर *x;

        x = startswith(fstype, "fuse.");
        अगर (x)
                fstype = x;

        अगर (nulstr_contains(filesystem_sets[FILESYSTEM_SET_NETWORK].मूल्य, fstype))
                वापस true;

        /* Filesystems not present in the internal database */
        वापस STR_IN_SET(fstype,
                          "davfs",
                          "glusterfs",
                          "lustre",
                          "sshfs");
}

bool fstype_needs_quota(स्थिर अक्षर *fstype) {
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
        वापस STR_IN_SET(fstype,
                          "ext2",
                          "ext3",
                          "ext4",
                          "reiserfs",
                          "jfs",
                          "f2fs");
}

bool fstype_has_internal_quota(स्थिर अक्षर *fstype) {
        /* These filesystems have built-in quota support and do not need
         * external quotacheck/quotaon services - see the "nothing needed"
         * entries in fstype_needs_quota() above. */
        वापस STR_IN_SET(fstype,
                          "xfs",
                          "gfs2",
                          "ocfs2",
                          "btrfs");
}

bool fstype_is_api_vfs(स्थिर अक्षर *fstype) {
        निश्चित(fstype);

        स्थिर FilesystemSet *fs;
        FOREACH_ARGUMENT(fs,
                         filesystem_sets + FILESYSTEM_SET_BASIC_API,
                         filesystem_sets + FILESYSTEM_SET_AUXILIARY_API,
                         filesystem_sets + FILESYSTEM_SET_PRIVILEGED_API,
                         filesystem_sets + FILESYSTEM_SET_TEMPORARY)
                अगर (nulstr_contains(fs->मूल्य, fstype))
                    वापस true;

        /* Filesystems not present in the internal database */
        वापस STR_IN_SET(fstype,
                          "autofs",
                          "cpuset");
}

bool fstype_is_blockdev_backed(स्थिर अक्षर *fstype) {
        स्थिर अक्षर *x;

        x = startswith(fstype, "fuse.");
        अगर (x)
                fstype = x;

        वापस !STR_IN_SET(fstype, "9p", "overlay") && !fstype_is_network(fstype) && !fstype_is_api_vfs(fstype);
}

bool fstype_is_ro(स्थिर अक्षर *fstype) {
        /* All Linux file systems that are necessarily read-only */
        वापस STR_IN_SET(fstype,
                          "DM_verity_hash",
                          "cramfs",
                          "erofs",
                          "iso9660",
                          "squashfs");
}

bool fstype_can_discard(स्थिर अक्षर *fstype) {
        निश्चित(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        अगर (STR_IN_SET(fstype, "btrfs", "f2fs", "ext4", "vfat", "xfs"))
                वापस true;

        /* On new kernels we can just ask the kernel */
        वापस mount_option_supported(fstype, "discard", शून्य) > 0;
}

स्थिर अक्षर* fstype_norecovery_option(स्थिर अक्षर *fstype) {
        पूर्णांक r;

        निश्चित(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. */
        अगर (STR_IN_SET(fstype, "ext3", "ext4", "xfs"))
                वापस "norecovery";

        /* btrfs dropped support for the "norecovery" option in 6.8
         * (https://github.com/torvalds/linux/commit/a1912f712188291f9d7d434fba155461f1ebef66) and replaced
         * it with rescue=nologreplay, so we check for the new name first and fall back to checking for the
         * old name if the new name doesn't work. */
        अगर (streq(fstype, "btrfs")) {
                r = mount_option_supported(fstype, "rescue=nologreplay", शून्य);
                अगर (r == -EAGAIN) {
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming old kernel with 'norecovery': %m");
                        वापस "norecovery";
                }
                अगर (r < 0)
                        log_debug_errno(r, "Failed to check for btrfs 'rescue=nologreplay' option, assuming it is not supported: %m");
                अगर (r > 0)
                        वापस "rescue=nologreplay";
        }

        /* On new kernels we can just ask the kernel */
        वापस mount_option_supported(fstype, "norecovery", शून्य) > 0 ? "norecovery" : शून्य;
}

bool fstype_can_fmask_dmask(स्थिर अक्षर *fstype) {
        निश्चित(fstype);

        /* Use a curated list as first check, to avoid calling fsopen() which might load kmods, which might
         * not be allowed in our MAC context. If we don't know ourselves, on new kernels we can just ask the
         * kernel. */
        वापस streq(fstype, "vfat") || (mount_option_supported(fstype, "fmask", "0177") > 0 && mount_option_supported(fstype, "dmask", "0077") > 0);
}

bool fstype_can_ownership(स्थिर अक्षर *fstype) {
        /* File systems which are not known to not support uid/gid ownership.
         * For some types, this can be a bit murky. So just exclude the ones that for sure
         * don't support with the current implementations in Linux. */

        वापस !STR_IN_SET(ASSERT_PTR(fstype),
                           "adfs",
                           "exfat",
                           "fat",
                           "hfs",
                           "hpfs",
                           "msdos",
                           "ntfs",
                           "vfat");
}

bool fstype_can_uid_gid(स्थिर अक्षर *fstype) {
        /* All file systems that have a uid=/gid= mount option that fixates the owners of all files and
         * directories, current and future. Note that this does *not* ask the kernel via
         * mount_option_supported() here because the uid=/gid= setting of various file systems mean different
         * things: some apply it only to the root dir inode, others to all inodes in the file system. Thus we
         * maintain the curated list below. 😢 */

        वापस STR_IN_SET(fstype,
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

पूर्णांक dev_is_devtmpfs(व्योम) {
        _cleanup_fclose_ खाता *proc_self_mountinfo = शून्य;
        पूर्णांक mount_id, r;
        अक्षर *e;

        r = path_get_mnt_id("/dev", &mount_id);
        अगर (r < 0)
                वापस r;

        r = fopen_unlocked("/proc/self/mountinfo", "re", &proc_self_mountinfo);
        अगर (r == -ENOENT)
                वापस proc_mounted() > 0 ? -ENOENT : -ENOSYS;
        अगर (r < 0)
                वापस r;

        क्रम (;;) {
                _cleanup_free_ अक्षर *पंक्ति = शून्य;
                पूर्णांक mid;

                r = read_पंक्ति(proc_self_mountinfo, LONG_पंक्ति_MAX, &पंक्ति);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                अगर (माला_पूछो(पंक्ति, "%i", &mid) != 1)
                        जारी;

                अगर (mid != mount_id)
                        जारी;

                e = strstrafter(पंक्ति, " - ");
                अगर (!e)
                        जारी;

                /* accept any name that starts with the currently expected type */
                अगर (startswith(e, "devtmpfs"))
                        वापस true;
        }

        वापस false;
}

अटल पूर्णांक mount_fd(
                स्थिर अक्षर *source,
                पूर्णांक target_fd,
                स्थिर अक्षर *filesystemtype,
                अचिन्हित दीर्घ mountflags,
                स्थिर व्योम *तथ्य) {

        निश्चित(target_fd >= 0);

        अगर (mount(source, FORMAT_PROC_FD_PATH(target_fd), filesystemtype, mountflags, तथ्य) < 0) {
                अगर (त्रुटि_सं != ENOENT)
                        वापस -त्रुटि_सं;

                /* ENOENT can mean two things: either that the source is missing, or that /proc/ isn't
                 * mounted. Check for the latter to generate better error messages. */
                अगर (proc_mounted() == 0)
                        वापस -ENOSYS;

                वापस -ENOENT;
        }

        वापस 0;
}

पूर्णांक mount_nofollow(
                स्थिर अक्षर *source,
                स्थिर अक्षर *target,
                स्थिर अक्षर *filesystemtype,
                अचिन्हित दीर्घ mountflags,
                स्थिर व्योम *तथ्य) {

        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(target);

        /* In almost all cases we want to manipulate the mount table without following symlinks, hence
         * mount_nofollow() is usually the way to go. The only exceptions are environments where /proc/ is
         * not available yet, since we need /proc/self/fd/ for this logic to work. i.e. during the early
         * initialization of namespacing/container stuff where /proc is not yet mounted (and maybe even the
         * fs to mount) we can only use traditional mount() directly.
         *
         * Note that this disables following only for the final component of the target, i.e symlinks within
         * the path of the target are honoured, as are symlinks in the source path everywhere. */

        fd = खोलो(target, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        वापस mount_fd(source, fd, filesystemtype, mountflags, तथ्य);
}

स्थिर अक्षर* mount_propagation_flag_to_string(अचिन्हित दीर्घ flags) {

        चयन (flags & (MS_SHARED|MS_SLAVE|MS_PRIVATE)) {
        हाल 0:
                वापस "";
        हाल MS_SHARED:
                वापस "shared";
        हाल MS_SLAVE:
                वापस "slave";
        हाल MS_PRIVATE:
                वापस "private";
        }

        वापस शून्य;
}

पूर्णांक mount_propagation_flag_from_string(स्थिर अक्षर *name, अचिन्हित दीर्घ *ret) {

        POINTER_MAY_BE_NULL(name);
        निश्चित(ret);

        अगर (isempty(name))
                *ret = 0;
        अन्यथा अगर (streq(name, "shared"))
                *ret = MS_SHARED;
        अन्यथा अगर (streq(name, "slave"))
                *ret = MS_SLAVE;
        अन्यथा अगर (streq(name, "private"))
                *ret = MS_PRIVATE;
        अन्यथा
                वापस -EINVAL;
        वापस 0;
}

bool mount_propagation_flag_is_valid(अचिन्हित दीर्घ flag) {
        वापस IN_SET(flag, 0, MS_SHARED, MS_PRIVATE, MS_SLAVE);
}

bool mount_new_api_supported(व्योम) {
        अटल पूर्णांक cache = -1;
        पूर्णांक r;

        अगर (cache >= 0)
                वापस cache;

        /* This is the newest API among the ones we use, so use it as boundary */
        r = RET_NERRNO(mount_setattr(-EBADF, शून्य, 0, शून्य, 0));
        अगर (r == 0 || ERRNO_IS_NOT_SUPPORTED(r)) /* This should return an error if it is working properly */
                वापस (cache = false);

        वापस (cache = true);
}

पूर्णांक mount_option_supported(स्थिर अक्षर *fstype, स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        /* Checks if the specified file system supports a mount option. Returns > 0 if it supports it, == 0 if
         * it does not. Return -EAGAIN if we can't determine it. And any other error otherwise. */

        निश्चित(fstype);
        निश्चित(कुंजी);

        fd = fsopen(fstype, FSOPEN_CLOEXEC);
        अगर (fd < 0) {
                अगर (त्रुटि_सं == ENODEV)
                        वापस log_debug_errno(त्रुटि_सं, "Kernel does not support file system type '%s'.", fstype);

                वापस log_debug_errno(त्रुटि_सं, "Failed to open superblock context for '%s': %m", fstype);
        }

        /* Various file systems support fs context only in recent kernels (e.g. btrfs). For older kernels
         * fsconfig() with FSCONFIG_SET_STRING/FSCONFIG_SET_FLAG never fail. Which sucks, because we want to
         * use it for testing support, after all. Let's hence do a check if the file system got converted yet
         * first. */
        अगर (fsconfig(fd, FSCONFIG_SET_FD, "adefinitelynotexistingmountoption", शून्य, fd) < 0) {
                /* If FSCONFIG_SET_FD is not supported for the fs, then the file system was not converted to
                 * the new mount API yet. If it returns EINVAL the mount option doesn't exist, but the fstype
                 * is converted. */
                अगर (त्रुटि_सं == EOPNOTSUPP)
                        वापस -EAGAIN; /* fs not converted to new mount API → don't know */
                अगर (त्रुटि_सं != EINVAL)
                        वापस log_debug_errno(त्रुटि_सं, "Failed to check if file system '%s' has been converted to new mount API: %m", fstype);

                /* So FSCONFIG_SET_FD worked, but the option didn't exist (we got EINVAL), this means the fs
                 * is converted. Let's now ask the actual question we wonder about. */
        } अन्यथा
                वापस log_debug_errno(SYNTHETIC_ERRNO(EAGAIN), "FSCONFIG_SET_FD worked unexpectedly for '%s', whoa!", fstype);

        अगर (मूल्य)
                r = fsconfig(fd, FSCONFIG_SET_STRING, कुंजी, मूल्य, 0);
        अन्यथा
                r = fsconfig(fd, FSCONFIG_SET_FLAG, कुंजी, शून्य, 0);
        अगर (r < 0) {
                अगर (त्रुटि_सं == EINVAL)
                        वापस false; /* EINVAL means option not supported. */

                वापस log_debug_errno(त्रुटि_सं, "Failed to set '%s%s%s' on '%s' superblock context: %m",
                                       कुंजी, मूल्य ? "=" : "", strempty(मूल्य), fstype);
        }

        वापस true; /* works! */
}

bool path_below_api_vfs(स्थिर अक्षर *p) {
        निश्चित(p);

        /* API VFS are either directly mounted on any of these three paths, or below it. */
        वापस PATH_STARTSWITH_SET(p, "/dev", "/sys", "/proc");
}
