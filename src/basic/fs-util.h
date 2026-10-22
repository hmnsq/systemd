/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        върни p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        върни m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

цяло rmdir_parents(const char *path, const char *stop);

цяло rename_noreplace(цяло olddirfd, const char *oldpath, цяло newdirfd, const char *newpath);

цяло readlinkat_malloc(цяло fd, const char *p, char **ret);
static inline цяло readlink_malloc(const char *p, char **ret) {
        върни readlinkat_malloc(AT_FDCWD, p, ret);
}
цяло readlink_value(const char *p, char **ret);
цяло readlink_and_make_absolute(const char *p, char **ret);

цяло chmod_and_chown_at(цяло dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline цяло chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        върни chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
цяло fchmod_and_chown_with_fallback(цяло fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline цяло fchmod_and_chown(цяло fd, mode_t mode, uid_t uid, gid_t gid) {
        върни fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

цяло fchmod_umask(цяло fd, mode_t mode);
цяло fchmod_opath(цяло fd, mode_t m);

цяло futimens_opath(цяло fd, const struct timespec ts[2]);

цяло fd_warn_permissions(const char *path, цяло fd);
цяло stat_warn_permissions(const char *path, const struct stat *st);

цяло access_nofollow(const char *path, цяло mode);
цяло access_fd(цяло fd, цяло mode);

цяло touch_fd(цяло fd, usec_t stamp);
цяло touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
цяло touch(const char *path);

цяло symlinkat_idempotent(const char *target, цяло atfd, const char *linkpath, bool make_relative);
static inline цяло symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        върни symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

цяло symlinkat_atomic_full_label(const char *target, цяло atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline цяло symlinkat_atomic_full(const char *target, цяло atfd, const char *linkpath, SymlinkFlags flags) {
        върни symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline цяло symlink_atomic(const char *target, const char *linkpath) {
        върни symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

цяло mknodat_atomic(цяло atfd, const char *path, mode_t mode, dev_t dev);
static inline цяло mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        върни mknodat_atomic(AT_FDCWD, path, mode, dev);
}

цяло mkfifoat_atomic(цяло dir_fd, const char *path, mode_t mode);
static inline цяло mkfifo_atomic(const char *path, mode_t mode) {
        върни mkfifoat_atomic(AT_FDCWD, path, mode);
}

цяло get_files_in_directory(const char *path, char ***list);

цяло tmp_dir(const char **ret);
цяло var_tmp_dir(const char **ret);

цяло unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

цяло unlinkat_deallocate(цяло fd, const char *name, UnlinkDeallocateFlags flags);

цяло open_parent_at(цяло dir_fd, const char *path, цяло flags, mode_t mode);
static inline цяло open_parent(const char *path, цяло flags, mode_t mode) {
        върни open_parent_at(AT_FDCWD, path, flags, mode);
}

цяло conservative_renameat(цяло olddirfd, const char *oldpath, цяло newdirfd, const char *newpath);
static inline цяло conservative_rename(const char *oldpath, const char *newpath) {
        върни conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

цяло posix_fallocate_loop(цяло fd, uint64_t offset, uint64_t size);

цяло parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

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

цяло open_mkdir_at_full_label(цяло dirfd, const char *path, цяло flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline цяло open_mkdir_at_full(цяло dirfd, const char *path, цяло flags, XOpenFlags xopen_flags, mode_t mode) {
        върни open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline цяло open_mkdir_at(цяло dirfd, const char *path, цяло flags, mode_t mode) {
        върни open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline цяло open_mkdir(const char *path, цяло flags, mode_t mode) {
        върни open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

цяло openat_report_new(цяло dirfd, const char *pathname, цяло flags, mode_t mode, bool *ret_newly_created);

цяло xopenat_full_label(цяло dir_fd, const char *path, цяло open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline цяло xopenat_full(цяло dir_fd, const char *path, цяло open_flags, XOpenFlags xopen_flags, mode_t mode) {
        върни xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline цяло xopenat(цяло dir_fd, const char *path, цяло open_flags) {
        върни xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

цяло xopenat_lock_full_label(цяло dir_fd, const char *path, цяло open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, цяло operation, LabelContext *label_context);
static inline цяло xopenat_lock_full(цяло dir_fd, const char *path, цяло open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, цяло operation) {
        върни xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline цяло xopenat_lock(цяло dir_fd, const char *path, цяло open_flags, LockType locktype, цяло operation) {
        върни xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

цяло link_fd(цяло fd, цяло newdirfd, const char *newpath);

цяло linkat_replace(цяло olddirfd, const char *oldpath, цяло newdirfd, const char *newpath);

static inline цяло at_flags_normalize_nofollow(цяло flags) {
        ако (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } иначе
                flags |= AT_SYMLINK_NOFOLLOW;
        върни flags;
}

static inline цяло at_flags_normalize_follow(цяло flags) {
        ако (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } иначе
                flags |= AT_SYMLINK_FOLLOW;
        върни flags;
}
