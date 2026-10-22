/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        ສົ່ງຄືນ p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        ສົ່ງຄືນ m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

ຈຳນວນເຕັມ rmdir_parents(const var_na *path, const var_na *stop);

ຈຳນວນເຕັມ rename_noreplace(ຈຳນວນເຕັມ olddirfd, const var_na *oldpath, ຈຳນວນເຕັມ newdirfd, const var_na *newpath);

ຈຳນວນເຕັມ readlinkat_malloc(ຈຳນວນເຕັມ fd, const var_na *p, var_na **ret);
static anta_hstha ຈຳນວນເຕັມ readlink_malloc(const var_na *p, var_na **ret) {
        ສົ່ງຄືນ readlinkat_malloc(AT_FDCWD, p, ret);
}
ຈຳນວນເຕັມ readlink_value(const var_na *p, var_na **ret);
ຈຳນວນເຕັມ readlink_and_make_absolute(const var_na *p, var_na **ret);

ຈຳນວນເຕັມ chmod_and_chown_at(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ຈຳນວນເຕັມ chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        ສົ່ງຄືນ chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ຈຳນວນເຕັມ fchmod_and_chown_with_fallback(ຈຳນວນເຕັມ fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ຈຳນວນເຕັມ fchmod_and_chown(ຈຳນວນເຕັມ fd, mode_t mode, uid_t uid, gid_t gid) {
        ສົ່ງຄືນ fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

ຈຳນວນເຕັມ fchmod_umask(ຈຳນວນເຕັມ fd, mode_t mode);
ຈຳນວນເຕັມ fchmod_opath(ຈຳນວນເຕັມ fd, mode_t m);

ຈຳນວນເຕັມ futimens_opath(ຈຳນວນເຕັມ fd, const sa_nracanaa timespec ts[2]);

ຈຳນວນເຕັມ fd_warn_permissions(const var_na *path, ຈຳນວນເຕັມ fd);
ຈຳນວນເຕັມ stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

ຈຳນວນເຕັມ access_nofollow(const var_na *path, ຈຳນວນເຕັມ mode);
ຈຳນວນເຕັມ access_fd(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ mode);

ຈຳນວນເຕັມ touch_fd(ຈຳນວນເຕັມ fd, usec_t stamp);
ຈຳນວນເຕັມ touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ຈຳນວນເຕັມ touch(const var_na *path);

ຈຳນວນເຕັມ symlinkat_idempotent(const var_na *target, ຈຳນວນເຕັມ atfd, const var_na *linkpath, bool make_relative);
static anta_hstha ຈຳນວນເຕັມ symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        ສົ່ງຄືນ symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ຈຳນວນເຕັມ symlinkat_atomic_full_label(const var_na *target, ຈຳນວນເຕັມ atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha ຈຳນວນເຕັມ symlinkat_atomic_full(const var_na *target, ຈຳນວນເຕັມ atfd, const var_na *linkpath, SymlinkFlags flags) {
        ສົ່ງຄືນ symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha ຈຳນວນເຕັມ symlink_atomic(const var_na *target, const var_na *linkpath) {
        ສົ່ງຄືນ symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ຈຳນວນເຕັມ mknodat_atomic(ຈຳນວນເຕັມ atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha ຈຳນວນເຕັມ mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        ສົ່ງຄືນ mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ຈຳນວນເຕັມ mkfifoat_atomic(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode);
static anta_hstha ຈຳນວນເຕັມ mkfifo_atomic(const var_na *path, mode_t mode) {
        ສົ່ງຄືນ mkfifoat_atomic(AT_FDCWD, path, mode);
}

ຈຳນວນເຕັມ get_files_in_directory(const var_na *path, var_na ***list);

ຈຳນວນເຕັມ tmp_dir(const var_na **ret);
ຈຳນວນເຕັມ var_tmp_dir(const var_na **ret);

ຈຳນວນເຕັມ unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ຈຳນວນເຕັມ unlinkat_deallocate(ຈຳນວນເຕັມ fd, const var_na *name, UnlinkDeallocateFlags flags);

ຈຳນວນເຕັມ open_parent_at(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ flags, mode_t mode);
static anta_hstha ຈຳນວນເຕັມ open_parent(const var_na *path, ຈຳນວນເຕັມ flags, mode_t mode) {
        ສົ່ງຄືນ open_parent_at(AT_FDCWD, path, flags, mode);
}

ຈຳນວນເຕັມ conservative_renameat(ຈຳນວນເຕັມ olddirfd, const var_na *oldpath, ຈຳນວນເຕັມ newdirfd, const var_na *newpath);
static anta_hstha ຈຳນວນເຕັມ conservative_rename(const var_na *oldpath, const var_na *newpath) {
        ສົ່ງຄືນ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ຈຳນວນເຕັມ posix_fallocate_loop(ຈຳນວນເຕັມ fd, uint64_t offset, uint64_t size);

ຈຳນວນເຕັມ parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

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

ຈຳນວນເຕັມ open_mkdir_at_full_label(ຈຳນວນເຕັມ dirfd, const var_na *path, ຈຳນວນເຕັມ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ຈຳນວນເຕັມ open_mkdir_at_full(ຈຳນວນເຕັມ dirfd, const var_na *path, ຈຳນວນເຕັມ flags, XOpenFlags xopen_flags, mode_t mode) {
        ສົ່ງຄືນ open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ຈຳນວນເຕັມ open_mkdir_at(ຈຳນວນເຕັມ dirfd, const var_na *path, ຈຳນວນເຕັມ flags, mode_t mode) {
        ສົ່ງຄືນ open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha ຈຳນວນເຕັມ open_mkdir(const var_na *path, ຈຳນວນເຕັມ flags, mode_t mode) {
        ສົ່ງຄືນ open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ຈຳນວນເຕັມ openat_report_new(ຈຳນວນເຕັມ dirfd, const var_na *pathname, ຈຳນວນເຕັມ flags, mode_t mode, bool *ret_newly_created);

ຈຳນວນເຕັມ xopenat_full_label(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ຈຳນວນເຕັມ xopenat_full(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ສົ່ງຄືນ xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ຈຳນວນເຕັມ xopenat(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ open_flags) {
        ສົ່ງຄືນ xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ຈຳນວນເຕັມ xopenat_lock_full_label(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ຈຳນວນເຕັມ operation, LabelContext *label_context);
static anta_hstha ຈຳນວນເຕັມ xopenat_lock_full(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ຈຳນວນເຕັມ operation) {
        ສົ່ງຄືນ xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha ຈຳນວນເຕັມ xopenat_lock(ຈຳນວນເຕັມ dir_fd, const var_na *path, ຈຳນວນເຕັມ open_flags, LockType locktype, ຈຳນວນເຕັມ operation) {
        ສົ່ງຄືນ xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ຈຳນວນເຕັມ link_fd(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ newdirfd, const var_na *newpath);

ຈຳນວນເຕັມ linkat_replace(ຈຳນວນເຕັມ olddirfd, const var_na *oldpath, ຈຳນວນເຕັມ newdirfd, const var_na *newpath);

static anta_hstha ຈຳນວນເຕັມ at_flags_normalize_nofollow(ຈຳນວນເຕັມ flags) {
        ຖ້າ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ບໍ່ດັ່ງນັ້ນ
                flags |= AT_SYMLINK_NOFOLLOW;
        ສົ່ງຄືນ flags;
}

static anta_hstha ຈຳນວນເຕັມ at_flags_normalize_follow(ຈຳນວນເຕັມ flags) {
        ຖ້າ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ບໍ່ດັ່ງນັ້ນ
                flags |= AT_SYMLINK_FOLLOW;
        ສົ່ງຄືນ flags;
}
