/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        вернуть p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        вернуть m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

целое rmdir_parents(const char *path, const char *stop);

целое rename_noreplace(целое olddirfd, const char *oldpath, целое newdirfd, const char *newpath);

целое readlinkat_malloc(целое fd, const char *p, char **ret);
static inline целое readlink_malloc(const char *p, char **ret) {
        вернуть readlinkat_malloc(AT_FDCWD, p, ret);
}
целое readlink_value(const char *p, char **ret);
целое readlink_and_make_absolute(const char *p, char **ret);

целое chmod_and_chown_at(целое dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline целое chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        вернуть chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
целое fchmod_and_chown_with_fallback(целое fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline целое fchmod_and_chown(целое fd, mode_t mode, uid_t uid, gid_t gid) {
        вернуть fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

целое fchmod_umask(целое fd, mode_t mode);
целое fchmod_opath(целое fd, mode_t m);

целое futimens_opath(целое fd, const struct timespec ts[2]);

целое fd_warn_permissions(const char *path, целое fd);
целое stat_warn_permissions(const char *path, const struct stat *st);

целое access_nofollow(const char *path, целое mode);
целое access_fd(целое fd, целое mode);

целое touch_fd(целое fd, usec_t stamp);
целое touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
целое touch(const char *path);

целое symlinkat_idempotent(const char *target, целое atfd, const char *linkpath, bool make_relative);
static inline целое symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        вернуть symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

целое symlinkat_atomic_full_label(const char *target, целое atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline целое symlinkat_atomic_full(const char *target, целое atfd, const char *linkpath, SymlinkFlags flags) {
        вернуть symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline целое symlink_atomic(const char *target, const char *linkpath) {
        вернуть symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

целое mknodat_atomic(целое atfd, const char *path, mode_t mode, dev_t dev);
static inline целое mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        вернуть mknodat_atomic(AT_FDCWD, path, mode, dev);
}

целое mkfifoat_atomic(целое dir_fd, const char *path, mode_t mode);
static inline целое mkfifo_atomic(const char *path, mode_t mode) {
        вернуть mkfifoat_atomic(AT_FDCWD, path, mode);
}

целое get_files_in_directory(const char *path, char ***list);

целое tmp_dir(const char **ret);
целое var_tmp_dir(const char **ret);

целое unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

целое unlinkat_deallocate(целое fd, const char *name, UnlinkDeallocateFlags flags);

целое open_parent_at(целое dir_fd, const char *path, целое flags, mode_t mode);
static inline целое open_parent(const char *path, целое flags, mode_t mode) {
        вернуть open_parent_at(AT_FDCWD, path, flags, mode);
}

целое conservative_renameat(целое olddirfd, const char *oldpath, целое newdirfd, const char *newpath);
static inline целое conservative_rename(const char *oldpath, const char *newpath) {
        вернуть conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

целое posix_fallocate_loop(целое fd, uint64_t offset, uint64_t size);

целое parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

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

целое open_mkdir_at_full_label(целое dirfd, const char *path, целое flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline целое open_mkdir_at_full(целое dirfd, const char *path, целое flags, XOpenFlags xopen_flags, mode_t mode) {
        вернуть open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline целое open_mkdir_at(целое dirfd, const char *path, целое flags, mode_t mode) {
        вернуть open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline целое open_mkdir(const char *path, целое flags, mode_t mode) {
        вернуть open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

целое openat_report_new(целое dirfd, const char *pathname, целое flags, mode_t mode, bool *ret_newly_created);

целое xopenat_full_label(целое dir_fd, const char *path, целое open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline целое xopenat_full(целое dir_fd, const char *path, целое open_flags, XOpenFlags xopen_flags, mode_t mode) {
        вернуть xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline целое xopenat(целое dir_fd, const char *path, целое open_flags) {
        вернуть xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

целое xopenat_lock_full_label(целое dir_fd, const char *path, целое open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, целое operation, LabelContext *label_context);
static inline целое xopenat_lock_full(целое dir_fd, const char *path, целое open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, целое operation) {
        вернуть xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline целое xopenat_lock(целое dir_fd, const char *path, целое open_flags, LockType locktype, целое operation) {
        вернуть xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

целое link_fd(целое fd, целое newdirfd, const char *newpath);

целое linkat_replace(целое olddirfd, const char *oldpath, целое newdirfd, const char *newpath);

static inline целое at_flags_normalize_nofollow(целое flags) {
        если (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } иначе
                flags |= AT_SYMLINK_NOFOLLOW;
        вернуть flags;
}

static inline целое at_flags_normalize_follow(целое flags) {
        если (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } иначе
                flags |= AT_SYMLINK_FOLLOW;
        вернуть flags;
}
