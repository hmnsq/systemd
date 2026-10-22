/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        반환 p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        반환 m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

정수 rmdir_parents(const char *path, const char *stop);

정수 rename_noreplace(정수 olddirfd, const char *oldpath, 정수 newdirfd, const char *newpath);

정수 readlinkat_malloc(정수 fd, const char *p, char **ret);
static inline 정수 readlink_malloc(const char *p, char **ret) {
        반환 readlinkat_malloc(AT_FDCWD, p, ret);
}
정수 readlink_value(const char *p, char **ret);
정수 readlink_and_make_absolute(const char *p, char **ret);

정수 chmod_and_chown_at(정수 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline 정수 chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        반환 chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
정수 fchmod_and_chown_with_fallback(정수 fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline 정수 fchmod_and_chown(정수 fd, mode_t mode, uid_t uid, gid_t gid) {
        반환 fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

정수 fchmod_umask(정수 fd, mode_t mode);
정수 fchmod_opath(정수 fd, mode_t m);

정수 futimens_opath(정수 fd, const struct timespec ts[2]);

정수 fd_warn_permissions(const char *path, 정수 fd);
정수 stat_warn_permissions(const char *path, const struct stat *st);

정수 access_nofollow(const char *path, 정수 mode);
정수 access_fd(정수 fd, 정수 mode);

정수 touch_fd(정수 fd, usec_t stamp);
정수 touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
정수 touch(const char *path);

정수 symlinkat_idempotent(const char *target, 정수 atfd, const char *linkpath, bool make_relative);
static inline 정수 symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        반환 symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

정수 symlinkat_atomic_full_label(const char *target, 정수 atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline 정수 symlinkat_atomic_full(const char *target, 정수 atfd, const char *linkpath, SymlinkFlags flags) {
        반환 symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline 정수 symlink_atomic(const char *target, const char *linkpath) {
        반환 symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

정수 mknodat_atomic(정수 atfd, const char *path, mode_t mode, dev_t dev);
static inline 정수 mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        반환 mknodat_atomic(AT_FDCWD, path, mode, dev);
}

정수 mkfifoat_atomic(정수 dir_fd, const char *path, mode_t mode);
static inline 정수 mkfifo_atomic(const char *path, mode_t mode) {
        반환 mkfifoat_atomic(AT_FDCWD, path, mode);
}

정수 get_files_in_directory(const char *path, char ***list);

정수 tmp_dir(const char **ret);
정수 var_tmp_dir(const char **ret);

정수 unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

정수 unlinkat_deallocate(정수 fd, const char *name, UnlinkDeallocateFlags flags);

정수 open_parent_at(정수 dir_fd, const char *path, 정수 flags, mode_t mode);
static inline 정수 open_parent(const char *path, 정수 flags, mode_t mode) {
        반환 open_parent_at(AT_FDCWD, path, flags, mode);
}

정수 conservative_renameat(정수 olddirfd, const char *oldpath, 정수 newdirfd, const char *newpath);
static inline 정수 conservative_rename(const char *oldpath, const char *newpath) {
        반환 conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

정수 posix_fallocate_loop(정수 fd, uint64_t offset, uint64_t size);

정수 parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

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

정수 open_mkdir_at_full_label(정수 dirfd, const char *path, 정수 flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline 정수 open_mkdir_at_full(정수 dirfd, const char *path, 정수 flags, XOpenFlags xopen_flags, mode_t mode) {
        반환 open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline 정수 open_mkdir_at(정수 dirfd, const char *path, 정수 flags, mode_t mode) {
        반환 open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline 정수 open_mkdir(const char *path, 정수 flags, mode_t mode) {
        반환 open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

정수 openat_report_new(정수 dirfd, const char *pathname, 정수 flags, mode_t mode, bool *ret_newly_created);

정수 xopenat_full_label(정수 dir_fd, const char *path, 정수 open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline 정수 xopenat_full(정수 dir_fd, const char *path, 정수 open_flags, XOpenFlags xopen_flags, mode_t mode) {
        반환 xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline 정수 xopenat(정수 dir_fd, const char *path, 정수 open_flags) {
        반환 xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

정수 xopenat_lock_full_label(정수 dir_fd, const char *path, 정수 open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, 정수 operation, LabelContext *label_context);
static inline 정수 xopenat_lock_full(정수 dir_fd, const char *path, 정수 open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, 정수 operation) {
        반환 xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline 정수 xopenat_lock(정수 dir_fd, const char *path, 정수 open_flags, LockType locktype, 정수 operation) {
        반환 xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

정수 link_fd(정수 fd, 정수 newdirfd, const char *newpath);

정수 linkat_replace(정수 olddirfd, const char *oldpath, 정수 newdirfd, const char *newpath);

static inline 정수 at_flags_normalize_nofollow(정수 flags) {
        만약 (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } 아니면
                flags |= AT_SYMLINK_NOFOLLOW;
        반환 flags;
}

static inline 정수 at_flags_normalize_follow(정수 flags) {
        만약 (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } 아니면
                flags |= AT_SYMLINK_FOLLOW;
        반환 flags;
}
