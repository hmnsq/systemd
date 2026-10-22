/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        ပြန်ပေး p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        ပြန်ပေး m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

ကိန်းပြည့် rmdir_parents(const var_na *path, const var_na *stop);

ကိန်းပြည့် rename_noreplace(ကိန်းပြည့် olddirfd, const var_na *oldpath, ကိန်းပြည့် newdirfd, const var_na *newpath);

ကိန်းပြည့် readlinkat_malloc(ကိန်းပြည့် fd, const var_na *p, var_na **ret);
static anta_hstha ကိန်းပြည့် readlink_malloc(const var_na *p, var_na **ret) {
        ပြန်ပေး readlinkat_malloc(AT_FDCWD, p, ret);
}
ကိန်းပြည့် readlink_value(const var_na *p, var_na **ret);
ကိန်းပြည့် readlink_and_make_absolute(const var_na *p, var_na **ret);

ကိန်းပြည့် chmod_and_chown_at(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ကိန်းပြည့် chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        ပြန်ပေး chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ကိန်းပြည့် fchmod_and_chown_with_fallback(ကိန်းပြည့် fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha ကိန်းပြည့် fchmod_and_chown(ကိန်းပြည့် fd, mode_t mode, uid_t uid, gid_t gid) {
        ပြန်ပေး fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

ကိန်းပြည့် fchmod_umask(ကိန်းပြည့် fd, mode_t mode);
ကိန်းပြည့် fchmod_opath(ကိန်းပြည့် fd, mode_t m);

ကိန်းပြည့် futimens_opath(ကိန်းပြည့် fd, const sa_nracanaa timespec ts[2]);

ကိန်းပြည့် fd_warn_permissions(const var_na *path, ကိန်းပြည့် fd);
ကိန်းပြည့် stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

ကိန်းပြည့် access_nofollow(const var_na *path, ကိန်းပြည့် mode);
ကိန်းပြည့် access_fd(ကိန်းပြည့် fd, ကိန်းပြည့် mode);

ကိန်းပြည့် touch_fd(ကိန်းပြည့် fd, usec_t stamp);
ကိန်းပြည့် touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ကိန်းပြည့် touch(const var_na *path);

ကိန်းပြည့် symlinkat_idempotent(const var_na *target, ကိန်းပြည့် atfd, const var_na *linkpath, bool make_relative);
static anta_hstha ကိန်းပြည့် symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        ပြန်ပေး symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ကိန်းပြည့် symlinkat_atomic_full_label(const var_na *target, ကိန်းပြည့် atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha ကိန်းပြည့် symlinkat_atomic_full(const var_na *target, ကိန်းပြည့် atfd, const var_na *linkpath, SymlinkFlags flags) {
        ပြန်ပေး symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha ကိန်းပြည့် symlink_atomic(const var_na *target, const var_na *linkpath) {
        ပြန်ပေး symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ကိန်းပြည့် mknodat_atomic(ကိန်းပြည့် atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha ကိန်းပြည့် mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        ပြန်ပေး mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ကိန်းပြည့် mkfifoat_atomic(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode);
static anta_hstha ကိန်းပြည့် mkfifo_atomic(const var_na *path, mode_t mode) {
        ပြန်ပေး mkfifoat_atomic(AT_FDCWD, path, mode);
}

ကိန်းပြည့် get_files_in_directory(const var_na *path, var_na ***list);

ကိန်းပြည့် tmp_dir(const var_na **ret);
ကိန်းပြည့် var_tmp_dir(const var_na **ret);

ကိန်းပြည့် unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ကိန်းပြည့် unlinkat_deallocate(ကိန်းပြည့် fd, const var_na *name, UnlinkDeallocateFlags flags);

ကိန်းပြည့် open_parent_at(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် flags, mode_t mode);
static anta_hstha ကိန်းပြည့် open_parent(const var_na *path, ကိန်းပြည့် flags, mode_t mode) {
        ပြန်ပေး open_parent_at(AT_FDCWD, path, flags, mode);
}

ကိန်းပြည့် conservative_renameat(ကိန်းပြည့် olddirfd, const var_na *oldpath, ကိန်းပြည့် newdirfd, const var_na *newpath);
static anta_hstha ကိန်းပြည့် conservative_rename(const var_na *oldpath, const var_na *newpath) {
        ပြန်ပေး conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ကိန်းပြည့် posix_fallocate_loop(ကိန်းပြည့် fd, uint64_t offset, uint64_t size);

ကိန်းပြည့် parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

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

ကိန်းပြည့် open_mkdir_at_full_label(ကိန်းပြည့် dirfd, const var_na *path, ကိန်းပြည့် flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ကိန်းပြည့် open_mkdir_at_full(ကိန်းပြည့် dirfd, const var_na *path, ကိန်းပြည့် flags, XOpenFlags xopen_flags, mode_t mode) {
        ပြန်ပေး open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ကိန်းပြည့် open_mkdir_at(ကိန်းပြည့် dirfd, const var_na *path, ကိန်းပြည့် flags, mode_t mode) {
        ပြန်ပေး open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha ကိန်းပြည့် open_mkdir(const var_na *path, ကိန်းပြည့် flags, mode_t mode) {
        ပြန်ပေး open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ကိန်းပြည့် openat_report_new(ကိန်းပြည့် dirfd, const var_na *pathname, ကိန်းပြည့် flags, mode_t mode, bool *ret_newly_created);

ကိန်းပြည့် xopenat_full_label(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha ကိန်းပြည့် xopenat_full(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ပြန်ပေး xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha ကိန်းပြည့် xopenat(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် open_flags) {
        ပြန်ပေး xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ကိန်းပြည့် xopenat_lock_full_label(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ကိန်းပြည့် operation, LabelContext *label_context);
static anta_hstha ကိန်းပြည့် xopenat_lock_full(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ကိန်းပြည့် operation) {
        ပြန်ပေး xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha ကိန်းပြည့် xopenat_lock(ကိန်းပြည့် dir_fd, const var_na *path, ကိန်းပြည့် open_flags, LockType locktype, ကိန်းပြည့် operation) {
        ပြန်ပေး xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ကိန်းပြည့် link_fd(ကိန်းပြည့် fd, ကိန်းပြည့် newdirfd, const var_na *newpath);

ကိန်းပြည့် linkat_replace(ကိန်းပြည့် olddirfd, const var_na *oldpath, ကိန်းပြည့် newdirfd, const var_na *newpath);

static anta_hstha ကိန်းပြည့် at_flags_normalize_nofollow(ကိန်းပြည့် flags) {
        အကယ်၍ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } မဟုတ်ပါက
                flags |= AT_SYMLINK_NOFOLLOW;
        ပြန်ပေး flags;
}

static anta_hstha ကိန်းပြည့် at_flags_normalize_follow(ကိန်းပြည့် flags) {
        အကယ်၍ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } မဟုတ်ပါက
                flags |= AT_SYMLINK_FOLLOW;
        ပြန်ပေး flags;
}
