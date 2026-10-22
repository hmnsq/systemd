/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        врати p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        врати m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

цео rmdir_parents(const char *path, const char *stop);

цео rename_noreplace(цео olddirfd, const char *oldpath, цео newdirfd, const char *newpath);

цео readlinkat_malloc(цео fd, const char *p, char **ret);
static inline цео readlink_malloc(const char *p, char **ret) {
        врати readlinkat_malloc(AT_FDCWD, p, ret);
}
цео readlink_value(const char *p, char **ret);
цео readlink_and_make_absolute(const char *p, char **ret);

цео chmod_and_chown_at(цео dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline цео chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        врати chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
цео fchmod_and_chown_with_fallback(цео fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline цео fchmod_and_chown(цео fd, mode_t mode, uid_t uid, gid_t gid) {
        врати fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

цео fchmod_umask(цео fd, mode_t mode);
цео fchmod_opath(цео fd, mode_t m);

цео futimens_opath(цео fd, const struct timespec ts[2]);

цео fd_warn_permissions(const char *path, цео fd);
цео stat_warn_permissions(const char *path, const struct stat *st);

цео access_nofollow(const char *path, цео mode);
цео access_fd(цео fd, цео mode);

цео touch_fd(цео fd, usec_t stamp);
цео touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
цео touch(const char *path);

цео symlinkat_idempotent(const char *target, цео atfd, const char *linkpath, bool make_relative);
static inline цео symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        врати symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

цео symlinkat_atomic_full_label(const char *target, цео atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline цео symlinkat_atomic_full(const char *target, цео atfd, const char *linkpath, SymlinkFlags flags) {
        врати symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline цео symlink_atomic(const char *target, const char *linkpath) {
        врати symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

цео mknodat_atomic(цео atfd, const char *path, mode_t mode, dev_t dev);
static inline цео mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        врати mknodat_atomic(AT_FDCWD, path, mode, dev);
}

цео mkfifoat_atomic(цео dir_fd, const char *path, mode_t mode);
static inline цео mkfifo_atomic(const char *path, mode_t mode) {
        врати mkfifoat_atomic(AT_FDCWD, path, mode);
}

цео get_files_in_directory(const char *path, char ***list);

цео tmp_dir(const char **ret);
цео var_tmp_dir(const char **ret);

цео unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

цео unlinkat_deallocate(цео fd, const char *name, UnlinkDeallocateFlags flags);

цео open_parent_at(цео dir_fd, const char *path, цео flags, mode_t mode);
static inline цео open_parent(const char *path, цео flags, mode_t mode) {
        врати open_parent_at(AT_FDCWD, path, flags, mode);
}

цео conservative_renameat(цео olddirfd, const char *oldpath, цео newdirfd, const char *newpath);
static inline цео conservative_rename(const char *oldpath, const char *newpath) {
        врати conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

цео posix_fallocate_loop(цео fd, uint64_t offset, uint64_t size);

цео parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

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

цео open_mkdir_at_full_label(цео dirfd, const char *path, цео flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline цео open_mkdir_at_full(цео dirfd, const char *path, цео flags, XOpenFlags xopen_flags, mode_t mode) {
        врати open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline цео open_mkdir_at(цео dirfd, const char *path, цео flags, mode_t mode) {
        врати open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline цео open_mkdir(const char *path, цео flags, mode_t mode) {
        врати open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

цео openat_report_new(цео dirfd, const char *pathname, цео flags, mode_t mode, bool *ret_newly_created);

цео xopenat_full_label(цео dir_fd, const char *path, цео open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline цео xopenat_full(цео dir_fd, const char *path, цео open_flags, XOpenFlags xopen_flags, mode_t mode) {
        врати xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline цео xopenat(цео dir_fd, const char *path, цео open_flags) {
        врати xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

цео xopenat_lock_full_label(цео dir_fd, const char *path, цео open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, цео operation, LabelContext *label_context);
static inline цео xopenat_lock_full(цео dir_fd, const char *path, цео open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, цео operation) {
        врати xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline цео xopenat_lock(цео dir_fd, const char *path, цео open_flags, LockType locktype, цео operation) {
        врати xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

цео link_fd(цео fd, цео newdirfd, const char *newpath);

цео linkat_replace(цео olddirfd, const char *oldpath, цео newdirfd, const char *newpath);

static inline цео at_flags_normalize_nofollow(цео flags) {
        ако (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } инако
                flags |= AT_SYMLINK_NOFOLLOW;
        врати flags;
}

static inline цео at_flags_normalize_follow(цео flags) {
        ако (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } инако
                flags |= AT_SYMLINK_FOLLOW;
        врати flags;
}
