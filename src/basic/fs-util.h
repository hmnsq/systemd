/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        ត្រឡប់ p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        ត្រឡប់ m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

ចំនួនគត់ rmdir_parents(const var_na *path, const var_na *stop);

ចំនួនគត់ rename_noreplace(ចំនួនគត់ olddirfd, const var_na *oldpath, ចំនួនគត់ newdirfd, const var_na *newpath);

ចំនួនគត់ readlinkat_malloc(ចំនួនគត់ fd, const var_na *p, var_na **ret);
static anta_hstha ចំនួនគត់ readlink_malloc(const var_na *p, var_na **ret) {
        ត្រឡប់ readlinkat_malloc(AT_FDCWD, p, ret);
}
ចំនួនគត់ readlink_value(const var_na *p, var_na **ret);
ចំនួនគត់ readlink_and_make_absolute(const var_na *p, var_na **ret);

ចំនួនគត់ chmod_and_chown_at(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ចំនួនគត់ chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        ត្រឡប់ chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ចំនួនគត់ fchmod_and_chown_with_fallback(ចំនួនគត់ fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ចំនួនគត់ fchmod_and_chown(ចំនួនគត់ fd, mode_t mode, uid_t uid, gid_t gid) {
        ត្រឡប់ fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

ចំនួនគត់ fchmod_umask(ចំនួនគត់ fd, mode_t mode);
ចំនួនគត់ fchmod_opath(ចំនួនគត់ fd, mode_t m);

ចំនួនគត់ futimens_opath(ចំនួនគត់ fd, const sa_nracanaa timespec ts[2]);

ចំនួនគត់ fd_warn_permissions(const var_na *path, ចំនួនគត់ fd);
ចំនួនគត់ stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

ចំនួនគត់ access_nofollow(const var_na *path, ចំនួនគត់ mode);
ចំនួនគត់ access_fd(ចំនួនគត់ fd, ចំនួនគត់ mode);

ចំនួនគត់ touch_fd(ចំនួនគត់ fd, usec_t stamp);
ចំនួនគត់ touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ចំនួនគត់ touch(const var_na *path);

ចំនួនគត់ symlinkat_idempotent(const var_na *target, ចំនួនគត់ atfd, const var_na *linkpath, bool make_relative);
static anta_hstha ចំនួនគត់ symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        ត្រឡប់ symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ចំនួនគត់ symlinkat_atomic_full_label(const var_na *target, ចំនួនគត់ atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha ចំនួនគត់ symlinkat_atomic_full(const var_na *target, ចំនួនគត់ atfd, const var_na *linkpath, SymlinkFlags flags) {
        ត្រឡប់ symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha ចំនួនគត់ symlink_atomic(const var_na *target, const var_na *linkpath) {
        ត្រឡប់ symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ចំនួនគត់ mknodat_atomic(ចំនួនគត់ atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha ចំនួនគត់ mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        ត្រឡប់ mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ចំនួនគត់ mkfifoat_atomic(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode);
static anta_hstha ចំនួនគត់ mkfifo_atomic(const var_na *path, mode_t mode) {
        ត្រឡប់ mkfifoat_atomic(AT_FDCWD, path, mode);
}

ចំនួនគត់ get_files_in_directory(const var_na *path, var_na ***list);

ចំនួនគត់ tmp_dir(const var_na **ret);
ចំនួនគត់ var_tmp_dir(const var_na **ret);

ចំនួនគត់ unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ចំនួនគត់ unlinkat_deallocate(ចំនួនគត់ fd, const var_na *name, UnlinkDeallocateFlags flags);

ចំនួនគត់ open_parent_at(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ flags, mode_t mode);
static anta_hstha ចំនួនគត់ open_parent(const var_na *path, ចំនួនគត់ flags, mode_t mode) {
        ត្រឡប់ open_parent_at(AT_FDCWD, path, flags, mode);
}

ចំនួនគត់ conservative_renameat(ចំនួនគត់ olddirfd, const var_na *oldpath, ចំនួនគត់ newdirfd, const var_na *newpath);
static anta_hstha ចំនួនគត់ conservative_rename(const var_na *oldpath, const var_na *newpath) {
        ត្រឡប់ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ចំនួនគត់ posix_fallocate_loop(ចំនួនគត់ fd, uint64_t offset, uint64_t size);

ចំនួនគត់ parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

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

ចំនួនគត់ open_mkdir_at_full_label(ចំនួនគត់ dirfd, const var_na *path, ចំនួនគត់ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ចំនួនគត់ open_mkdir_at_full(ចំនួនគត់ dirfd, const var_na *path, ចំនួនគត់ flags, XOpenFlags xopen_flags, mode_t mode) {
        ត្រឡប់ open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ចំនួនគត់ open_mkdir_at(ចំនួនគត់ dirfd, const var_na *path, ចំនួនគត់ flags, mode_t mode) {
        ត្រឡប់ open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha ចំនួនគត់ open_mkdir(const var_na *path, ចំនួនគត់ flags, mode_t mode) {
        ត្រឡប់ open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ចំនួនគត់ openat_report_new(ចំនួនគត់ dirfd, const var_na *pathname, ចំនួនគត់ flags, mode_t mode, bool *ret_newly_created);

ចំនួនគត់ xopenat_full_label(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ចំនួនគត់ xopenat_full(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ត្រឡប់ xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ចំនួនគត់ xopenat(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ open_flags) {
        ត្រឡប់ xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ចំនួនគត់ xopenat_lock_full_label(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ចំនួនគត់ operation, LabelContext *label_context);
static anta_hstha ចំនួនគត់ xopenat_lock_full(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ចំនួនគត់ operation) {
        ត្រឡប់ xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha ចំនួនគត់ xopenat_lock(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ open_flags, LockType locktype, ចំនួនគត់ operation) {
        ត្រឡប់ xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ចំនួនគត់ link_fd(ចំនួនគត់ fd, ចំនួនគត់ newdirfd, const var_na *newpath);

ចំនួនគត់ linkat_replace(ចំនួនគត់ olddirfd, const var_na *oldpath, ចំនួនគត់ newdirfd, const var_na *newpath);

static anta_hstha ចំនួនគត់ at_flags_normalize_nofollow(ចំនួនគត់ flags) {
        ប្រសិនបើ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } បើមិនដូច្នេះ
                flags |= AT_SYMLINK_NOFOLLOW;
        ត្រឡប់ flags;
}

static anta_hstha ចំនួនគត់ at_flags_normalize_follow(ចំនួនគត់ flags) {
        ប្រសិនបើ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } បើមិនដូច្នេះ
                flags |= AT_SYMLINK_FOLLOW;
        ត្រឡប់ flags;
}
