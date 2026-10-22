/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        ཕྱིར p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        ཕྱིར m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

ཧྲིལ rmdir_parents(const var_na *path, const var_na *stop);

ཧྲིལ rename_noreplace(ཧྲིལ olddirfd, const var_na *oldpath, ཧྲིལ newdirfd, const var_na *newpath);

ཧྲིལ readlinkat_malloc(ཧྲིལ fd, const var_na *p, var_na **ret);
static anta_hstha ཧྲིལ readlink_malloc(const var_na *p, var_na **ret) {
        ཕྱིར readlinkat_malloc(AT_FDCWD, p, ret);
}
ཧྲིལ readlink_value(const var_na *p, var_na **ret);
ཧྲིལ readlink_and_make_absolute(const var_na *p, var_na **ret);

ཧྲིལ chmod_and_chown_at(ཧྲིལ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ཧྲིལ chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        ཕྱིར chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ཧྲིལ fchmod_and_chown_with_fallback(ཧྲིལ fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ཧྲིལ fchmod_and_chown(ཧྲིལ fd, mode_t mode, uid_t uid, gid_t gid) {
        ཕྱིར fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

ཧྲིལ fchmod_umask(ཧྲིལ fd, mode_t mode);
ཧྲིལ fchmod_opath(ཧྲིལ fd, mode_t m);

ཧྲིལ futimens_opath(ཧྲིལ fd, const sa_nracanaa timespec ts[2]);

ཧྲིལ fd_warn_permissions(const var_na *path, ཧྲིལ fd);
ཧྲིལ stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

ཧྲིལ access_nofollow(const var_na *path, ཧྲིལ mode);
ཧྲིལ access_fd(ཧྲིལ fd, ཧྲིལ mode);

ཧྲིལ touch_fd(ཧྲིལ fd, usec_t stamp);
ཧྲིལ touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ཧྲིལ touch(const var_na *path);

ཧྲིལ symlinkat_idempotent(const var_na *target, ཧྲིལ atfd, const var_na *linkpath, bool make_relative);
static anta_hstha ཧྲིལ symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        ཕྱིར symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ཧྲིལ symlinkat_atomic_full_label(const var_na *target, ཧྲིལ atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha ཧྲིལ symlinkat_atomic_full(const var_na *target, ཧྲིལ atfd, const var_na *linkpath, SymlinkFlags flags) {
        ཕྱིར symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha ཧྲིལ symlink_atomic(const var_na *target, const var_na *linkpath) {
        ཕྱིར symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ཧྲིལ mknodat_atomic(ཧྲིལ atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha ཧྲིལ mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        ཕྱིར mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ཧྲིལ mkfifoat_atomic(ཧྲིལ dir_fd, const var_na *path, mode_t mode);
static anta_hstha ཧྲིལ mkfifo_atomic(const var_na *path, mode_t mode) {
        ཕྱིར mkfifoat_atomic(AT_FDCWD, path, mode);
}

ཧྲིལ get_files_in_directory(const var_na *path, var_na ***list);

ཧྲིལ tmp_dir(const var_na **ret);
ཧྲིལ var_tmp_dir(const var_na **ret);

ཧྲིལ unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ཧྲིལ unlinkat_deallocate(ཧྲིལ fd, const var_na *name, UnlinkDeallocateFlags flags);

ཧྲིལ open_parent_at(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ flags, mode_t mode);
static anta_hstha ཧྲིལ open_parent(const var_na *path, ཧྲིལ flags, mode_t mode) {
        ཕྱིར open_parent_at(AT_FDCWD, path, flags, mode);
}

ཧྲིལ conservative_renameat(ཧྲིལ olddirfd, const var_na *oldpath, ཧྲིལ newdirfd, const var_na *newpath);
static anta_hstha ཧྲིལ conservative_rename(const var_na *oldpath, const var_na *newpath) {
        ཕྱིར conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ཧྲིལ posix_fallocate_loop(ཧྲིལ fd, uint64_t offset, uint64_t size);

ཧྲིལ parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

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

ཧྲིལ open_mkdir_at_full_label(ཧྲིལ dirfd, const var_na *path, ཧྲིལ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ཧྲིལ open_mkdir_at_full(ཧྲིལ dirfd, const var_na *path, ཧྲིལ flags, XOpenFlags xopen_flags, mode_t mode) {
        ཕྱིར open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ཧྲིལ open_mkdir_at(ཧྲིལ dirfd, const var_na *path, ཧྲིལ flags, mode_t mode) {
        ཕྱིར open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha ཧྲིལ open_mkdir(const var_na *path, ཧྲིལ flags, mode_t mode) {
        ཕྱིར open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ཧྲིལ openat_report_new(ཧྲིལ dirfd, const var_na *pathname, ཧྲིལ flags, mode_t mode, bool *ret_newly_created);

ཧྲིལ xopenat_full_label(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ཧྲིལ xopenat_full(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ཕྱིར xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ཧྲིལ xopenat(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ open_flags) {
        ཕྱིར xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ཧྲིལ xopenat_lock_full_label(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ཧྲིལ operation, LabelContext *label_context);
static anta_hstha ཧྲིལ xopenat_lock_full(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ཧྲིལ operation) {
        ཕྱིར xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha ཧྲིལ xopenat_lock(ཧྲིལ dir_fd, const var_na *path, ཧྲིལ open_flags, LockType locktype, ཧྲིལ operation) {
        ཕྱིར xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ཧྲིལ link_fd(ཧྲིལ fd, ཧྲིལ newdirfd, const var_na *newpath);

ཧྲིལ linkat_replace(ཧྲིལ olddirfd, const var_na *oldpath, ཧྲིལ newdirfd, const var_na *newpath);

static anta_hstha ཧྲིལ at_flags_normalize_nofollow(ཧྲིལ flags) {
        གལ་ཏེ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } དེ་མིན
                flags |= AT_SYMLINK_NOFOLLOW;
        ཕྱིར flags;
}

static anta_hstha ཧྲིལ at_flags_normalize_follow(ཧྲིལ flags) {
        གལ་ཏེ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } དེ་མིན
                flags |= AT_SYMLINK_FOLLOW;
        ཕྱིར flags;
}
