/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        повернути p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        повернути m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

ціле rmdir_parents(const char *path, const char *stop);

ціле rename_noreplace(ціле olddirfd, const char *oldpath, ціле newdirfd, const char *newpath);

ціле readlinkat_malloc(ціле fd, const char *p, char **ret);
static inline ціле readlink_malloc(const char *p, char **ret) {
        повернути readlinkat_malloc(AT_FDCWD, p, ret);
}
ціле readlink_value(const char *p, char **ret);
ціле readlink_and_make_absolute(const char *p, char **ret);

ціле chmod_and_chown_at(ціле dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline ціле chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        повернути chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ціле fchmod_and_chown_with_fallback(ціле fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline ціле fchmod_and_chown(ціле fd, mode_t mode, uid_t uid, gid_t gid) {
        повернути fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

ціле fchmod_umask(ціле fd, mode_t mode);
ціле fchmod_opath(ціле fd, mode_t m);

ціле futimens_opath(ціле fd, const struct timespec ts[2]);

ціле fd_warn_permissions(const char *path, ціле fd);
ціле stat_warn_permissions(const char *path, const struct stat *st);

ціле access_nofollow(const char *path, ціле mode);
ціле access_fd(ціле fd, ціле mode);

ціле touch_fd(ціле fd, usec_t stamp);
ціле touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ціле touch(const char *path);

ціле symlinkat_idempotent(const char *target, ціле atfd, const char *linkpath, bool make_relative);
static inline ціле symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        повернути symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ціле symlinkat_atomic_full_label(const char *target, ціле atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline ціле symlinkat_atomic_full(const char *target, ціле atfd, const char *linkpath, SymlinkFlags flags) {
        повернути symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline ціле symlink_atomic(const char *target, const char *linkpath) {
        повернути symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ціле mknodat_atomic(ціле atfd, const char *path, mode_t mode, dev_t dev);
static inline ціле mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        повернути mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ціле mkfifoat_atomic(ціле dir_fd, const char *path, mode_t mode);
static inline ціле mkfifo_atomic(const char *path, mode_t mode) {
        повернути mkfifoat_atomic(AT_FDCWD, path, mode);
}

ціле get_files_in_directory(const char *path, char ***list);

ціле tmp_dir(const char **ret);
ціле var_tmp_dir(const char **ret);

ціле unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ціле unlinkat_deallocate(ціле fd, const char *name, UnlinkDeallocateFlags flags);

ціле open_parent_at(ціле dir_fd, const char *path, ціле flags, mode_t mode);
static inline ціле open_parent(const char *path, ціле flags, mode_t mode) {
        повернути open_parent_at(AT_FDCWD, path, flags, mode);
}

ціле conservative_renameat(ціле olddirfd, const char *oldpath, ціле newdirfd, const char *newpath);
static inline ціле conservative_rename(const char *oldpath, const char *newpath) {
        повернути conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ціле posix_fallocate_loop(ціле fd, uint64_t offset, uint64_t size);

ціле parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

typedef enum XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

ціле open_mkdir_at_full_label(ціле dirfd, const char *path, ціле flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline ціле open_mkdir_at_full(ціле dirfd, const char *path, ціле flags, XOpenFlags xopen_flags, mode_t mode) {
        повернути open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline ціле open_mkdir_at(ціле dirfd, const char *path, ціле flags, mode_t mode) {
        повернути open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline ціле open_mkdir(const char *path, ціле flags, mode_t mode) {
        повернути open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ціле openat_report_new(ціле dirfd, const char *pathname, ціле flags, mode_t mode, bool *ret_newly_created);

ціле xopenat_full_label(ціле dir_fd, const char *path, ціле open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline ціле xopenat_full(ціле dir_fd, const char *path, ціле open_flags, XOpenFlags xopen_flags, mode_t mode) {
        повернути xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline ціле xopenat(ціле dir_fd, const char *path, ціле open_flags) {
        повернути xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ціле xopenat_lock_full_label(ціле dir_fd, const char *path, ціле open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ціле operation, LabelContext *label_context);
static inline ціле xopenat_lock_full(ціле dir_fd, const char *path, ціле open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ціле operation) {
        повернути xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline ціле xopenat_lock(ціле dir_fd, const char *path, ціле open_flags, LockType locktype, ціле operation) {
        повернути xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ціле link_fd(ціле fd, ціле newdirfd, const char *newpath);

ціле linkat_replace(ціле olddirfd, const char *oldpath, ціле newdirfd, const char *newpath);

static inline ціле at_flags_normalize_nofollow(ціле flags) {
        якщо (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } інакше
                flags |= AT_SYMLINK_NOFOLLOW;
        повернути flags;
}

static inline ціле at_flags_normalize_follow(ціле flags) {
        якщо (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } інакше
                flags |= AT_SYMLINK_FOLLOW;
        повернути flags;
}
