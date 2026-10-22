/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        ආපසු p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        ආපසු m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

පූර්ණ_සංඛ්‍යා rmdir_parents(const var_na *path, const var_na *stop);

පූර්ණ_සංඛ්‍යා rename_noreplace(පූර්ණ_සංඛ්‍යා olddirfd, const var_na *oldpath, පූර්ණ_සංඛ්‍යා newdirfd, const var_na *newpath);

පූර්ණ_සංඛ්‍යා readlinkat_malloc(පූර්ණ_සංඛ්‍යා fd, const var_na *p, var_na **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා readlink_malloc(const var_na *p, var_na **ret) {
        ආපසු readlinkat_malloc(AT_FDCWD, p, ret);
}
පූර්ණ_සංඛ්‍යා readlink_value(const var_na *p, var_na **ret);
පූර්ණ_සංඛ්‍යා readlink_and_make_absolute(const var_na *p, var_na **ret);

පූර්ණ_සංඛ්‍යා chmod_and_chown_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha පූර්ණ_සංඛ්‍යා chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        ආපසු chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
පූර්ණ_සංඛ්‍යා fchmod_and_chown_with_fallback(පූර්ණ_සංඛ්‍යා fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha පූර්ණ_සංඛ්‍යා fchmod_and_chown(පූර්ණ_සංඛ්‍යා fd, mode_t mode, uid_t uid, gid_t gid) {
        ආපසු fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

පූර්ණ_සංඛ්‍යා fchmod_umask(පූර්ණ_සංඛ්‍යා fd, mode_t mode);
පූර්ණ_සංඛ්‍යා fchmod_opath(පූර්ණ_සංඛ්‍යා fd, mode_t m);

පූර්ණ_සංඛ්‍යා futimens_opath(පූර්ණ_සංඛ්‍යා fd, const sa_nracanaa timespec ts[2]);

පූර්ණ_සංඛ්‍යා fd_warn_permissions(const var_na *path, පූර්ණ_සංඛ්‍යා fd);
පූර්ණ_සංඛ්‍යා stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

පූර්ණ_සංඛ්‍යා access_nofollow(const var_na *path, පූර්ණ_සංඛ්‍යා mode);
පූර්ණ_සංඛ්‍යා access_fd(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා mode);

පූර්ණ_සංඛ්‍යා touch_fd(පූර්ණ_සංඛ්‍යා fd, usec_t stamp);
පූර්ණ_සංඛ්‍යා touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
පූර්ණ_සංඛ්‍යා touch(const var_na *path);

පූර්ණ_සංඛ්‍යා symlinkat_idempotent(const var_na *target, පූර්ණ_සංඛ්‍යා atfd, const var_na *linkpath, bool make_relative);
static anta_hstha පූර්ණ_සංඛ්‍යා symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        ආපසු symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

පූර්ණ_සංඛ්‍යා symlinkat_atomic_full_label(const var_na *target, පූර්ණ_සංඛ්‍යා atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha පූර්ණ_සංඛ්‍යා symlinkat_atomic_full(const var_na *target, පූර්ණ_සංඛ්‍යා atfd, const var_na *linkpath, SymlinkFlags flags) {
        ආපසු symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha පූර්ණ_සංඛ්‍යා symlink_atomic(const var_na *target, const var_na *linkpath) {
        ආපසු symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

පූර්ණ_සංඛ්‍යා mknodat_atomic(පූර්ණ_සංඛ්‍යා atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha පූර්ණ_සංඛ්‍යා mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        ආපසු mknodat_atomic(AT_FDCWD, path, mode, dev);
}

පූර්ණ_සංඛ්‍යා mkfifoat_atomic(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode);
static anta_hstha පූර්ණ_සංඛ්‍යා mkfifo_atomic(const var_na *path, mode_t mode) {
        ආපසු mkfifoat_atomic(AT_FDCWD, path, mode);
}

පූර්ණ_සංඛ්‍යා get_files_in_directory(const var_na *path, var_na ***list);

පූර්ණ_සංඛ්‍යා tmp_dir(const var_na **ret);
පූර්ණ_සංඛ්‍යා var_tmp_dir(const var_na **ret);

පූර්ණ_සංඛ්‍යා unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

පූර්ණ_සංඛ්‍යා unlinkat_deallocate(පූර්ණ_සංඛ්‍යා fd, const var_na *name, UnlinkDeallocateFlags flags);

පූර්ණ_සංඛ්‍යා open_parent_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා flags, mode_t mode);
static anta_hstha පූර්ණ_සංඛ්‍යා open_parent(const var_na *path, පූර්ණ_සංඛ්‍යා flags, mode_t mode) {
        ආපසු open_parent_at(AT_FDCWD, path, flags, mode);
}

පූර්ණ_සංඛ්‍යා conservative_renameat(පූර්ණ_සංඛ්‍යා olddirfd, const var_na *oldpath, පූර්ණ_සංඛ්‍යා newdirfd, const var_na *newpath);
static anta_hstha පූර්ණ_සංඛ්‍යා conservative_rename(const var_na *oldpath, const var_na *newpath) {
        ආපසු conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

පූර්ණ_සංඛ්‍යා posix_fallocate_loop(පූර්ණ_සංඛ්‍යා fd, uint64_t offset, uint64_t size);

පූර්ණ_සංඛ්‍යා parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

typedef ga_nanaa XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

පූර්ණ_සංඛ්‍යා open_mkdir_at_full_label(පූර්ණ_සංඛ්‍යා dirfd, const var_na *path, පූර්ණ_සංඛ්‍යා flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha පූර්ණ_සංඛ්‍යා open_mkdir_at_full(පූර්ණ_සංඛ්‍යා dirfd, const var_na *path, පූර්ණ_සංඛ්‍යා flags, XOpenFlags xopen_flags, mode_t mode) {
        ආපසු open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha පූර්ණ_සංඛ්‍යා open_mkdir_at(පූර්ණ_සංඛ්‍යා dirfd, const var_na *path, පූර්ණ_සංඛ්‍යා flags, mode_t mode) {
        ආපසු open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha පූර්ණ_සංඛ්‍යා open_mkdir(const var_na *path, පූර්ණ_සංඛ්‍යා flags, mode_t mode) {
        ආපසු open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

පූර්ණ_සංඛ්‍යා openat_report_new(පූර්ණ_සංඛ්‍යා dirfd, const var_na *pathname, පූර්ණ_සංඛ්‍යා flags, mode_t mode, bool *ret_newly_created);

පූර්ණ_සංඛ්‍යා xopenat_full_label(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha පූර්ණ_සංඛ්‍යා xopenat_full(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ආපසු xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha පූර්ණ_සංඛ්‍යා xopenat(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා open_flags) {
        ආපසු xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

පූර්ණ_සංඛ්‍යා xopenat_lock_full_label(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, පූර්ණ_සංඛ්‍යා operation, LabelContext *label_context);
static anta_hstha පූර්ණ_සංඛ්‍යා xopenat_lock_full(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, පූර්ණ_සංඛ්‍යා operation) {
        ආපසු xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha පූර්ණ_සංඛ්‍යා xopenat_lock(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා open_flags, LockType locktype, පූර්ණ_සංඛ්‍යා operation) {
        ආපසු xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

පූර්ණ_සංඛ්‍යා link_fd(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා newdirfd, const var_na *newpath);

පූර්ණ_සංඛ්‍යා linkat_replace(පූර්ණ_සංඛ්‍යා olddirfd, const var_na *oldpath, පූර්ණ_සංඛ්‍යා newdirfd, const var_na *newpath);

static anta_hstha පූර්ණ_සංඛ්‍යා at_flags_normalize_nofollow(පූර්ණ_සංඛ්‍යා flags) {
        නම් (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } නැතහොත්
                flags |= AT_SYMLINK_NOFOLLOW;
        ආපසු flags;
}

static anta_hstha පූර්ණ_සංඛ්‍යා at_flags_normalize_follow(පූර්ණ_සංඛ්‍යා flags) {
        නම් (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } නැතහොත්
                flags |= AT_SYMLINK_FOLLOW;
        ආපසු flags;
}
