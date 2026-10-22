/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        返回 p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        返回 m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

整数 rmdir_parents(const char *path, const char *stop);

整数 rename_noreplace(整数 olddirfd, const char *oldpath, 整数 newdirfd, const char *newpath);

整数 readlinkat_malloc(整数 fd, const char *p, char **ret);
static inline 整数 readlink_malloc(const char *p, char **ret) {
        返回 readlinkat_malloc(AT_FDCWD, p, ret);
}
整数 readlink_value(const char *p, char **ret);
整数 readlink_and_make_absolute(const char *p, char **ret);

整数 chmod_and_chown_at(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline 整数 chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        返回 chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
整数 fchmod_and_chown_with_fallback(整数 fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline 整数 fchmod_and_chown(整数 fd, mode_t mode, uid_t uid, gid_t gid) {
        返回 fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

整数 fchmod_umask(整数 fd, mode_t mode);
整数 fchmod_opath(整数 fd, mode_t m);

整数 futimens_opath(整数 fd, const struct timespec ts[2]);

整数 fd_warn_permissions(const char *path, 整数 fd);
整数 stat_warn_permissions(const char *path, const struct stat *st);

整数 access_nofollow(const char *path, 整数 mode);
整数 access_fd(整数 fd, 整数 mode);

整数 touch_fd(整数 fd, usec_t stamp);
整数 touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
整数 touch(const char *path);

整数 symlinkat_idempotent(const char *target, 整数 atfd, const char *linkpath, bool make_relative);
static inline 整数 symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        返回 symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

整数 symlinkat_atomic_full_label(const char *target, 整数 atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline 整数 symlinkat_atomic_full(const char *target, 整数 atfd, const char *linkpath, SymlinkFlags flags) {
        返回 symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline 整数 symlink_atomic(const char *target, const char *linkpath) {
        返回 symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

整数 mknodat_atomic(整数 atfd, const char *path, mode_t mode, dev_t dev);
static inline 整数 mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        返回 mknodat_atomic(AT_FDCWD, path, mode, dev);
}

整数 mkfifoat_atomic(整数 dir_fd, const char *path, mode_t mode);
static inline 整数 mkfifo_atomic(const char *path, mode_t mode) {
        返回 mkfifoat_atomic(AT_FDCWD, path, mode);
}

整数 get_files_in_directory(const char *path, char ***list);

整数 tmp_dir(const char **ret);
整数 var_tmp_dir(const char **ret);

整数 unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

整数 unlinkat_deallocate(整数 fd, const char *name, UnlinkDeallocateFlags flags);

整数 open_parent_at(整数 dir_fd, const char *path, 整数 flags, mode_t mode);
static inline 整数 open_parent(const char *path, 整数 flags, mode_t mode) {
        返回 open_parent_at(AT_FDCWD, path, flags, mode);
}

整数 conservative_renameat(整数 olddirfd, const char *oldpath, 整数 newdirfd, const char *newpath);
static inline 整数 conservative_rename(const char *oldpath, const char *newpath) {
        返回 conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

整数 posix_fallocate_loop(整数 fd, uint64_t offset, uint64_t size);

整数 parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

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

整数 open_mkdir_at_full_label(整数 dirfd, const char *path, 整数 flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline 整数 open_mkdir_at_full(整数 dirfd, const char *path, 整数 flags, XOpenFlags xopen_flags, mode_t mode) {
        返回 open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline 整数 open_mkdir_at(整数 dirfd, const char *path, 整数 flags, mode_t mode) {
        返回 open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline 整数 open_mkdir(const char *path, 整数 flags, mode_t mode) {
        返回 open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

整数 openat_report_new(整数 dirfd, const char *pathname, 整数 flags, mode_t mode, bool *ret_newly_created);

整数 xopenat_full_label(整数 dir_fd, const char *path, 整数 open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline 整数 xopenat_full(整数 dir_fd, const char *path, 整数 open_flags, XOpenFlags xopen_flags, mode_t mode) {
        返回 xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline 整数 xopenat(整数 dir_fd, const char *path, 整数 open_flags) {
        返回 xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

整数 xopenat_lock_full_label(整数 dir_fd, const char *path, 整数 open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, 整数 operation, LabelContext *label_context);
static inline 整数 xopenat_lock_full(整数 dir_fd, const char *path, 整数 open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, 整数 operation) {
        返回 xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline 整数 xopenat_lock(整数 dir_fd, const char *path, 整数 open_flags, LockType locktype, 整数 operation) {
        返回 xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

整数 link_fd(整数 fd, 整数 newdirfd, const char *newpath);

整数 linkat_replace(整数 olddirfd, const char *oldpath, 整数 newdirfd, const char *newpath);

static inline 整数 at_flags_normalize_nofollow(整数 flags) {
        如果 (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } 否则
                flags |= AT_SYMLINK_NOFOLLOW;
        返回 flags;
}

static inline 整数 at_flags_normalize_follow(整数 flags) {
        如果 (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } 否则
                flags |= AT_SYMLINK_FOLLOW;
        返回 flags;
}
