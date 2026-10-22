/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static inline mode_t PTR_TO_MODE(void *p) {
        trả_về p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static inline void* MODE_TO_PTR(mode_t m) {
        trả_về m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

số_nguyên rmdir_parents(const char *path, const char *stop);

số_nguyên rename_noreplace(số_nguyên olddirfd, const char *oldpath, số_nguyên newdirfd, const char *newpath);

số_nguyên readlinkat_malloc(số_nguyên fd, const char *p, char **ret);
static inline số_nguyên readlink_malloc(const char *p, char **ret) {
        trả_về readlinkat_malloc(AT_FDCWD, p, ret);
}
số_nguyên readlink_value(const char *p, char **ret);
số_nguyên readlink_and_make_absolute(const char *p, char **ret);

số_nguyên chmod_and_chown_at(số_nguyên dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline số_nguyên chmod_and_chown(const char *path, mode_t mode, uid_t uid, gid_t gid) {
        trả_về chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
số_nguyên fchmod_and_chown_with_fallback(số_nguyên fd, const char *path, mode_t mode, uid_t uid, gid_t gid);
static inline số_nguyên fchmod_and_chown(số_nguyên fd, mode_t mode, uid_t uid, gid_t gid) {
        trả_về fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

số_nguyên fchmod_umask(số_nguyên fd, mode_t mode);
số_nguyên fchmod_opath(số_nguyên fd, mode_t m);

số_nguyên futimens_opath(số_nguyên fd, const struct timespec ts[2]);

số_nguyên fd_warn_permissions(const char *path, số_nguyên fd);
số_nguyên stat_warn_permissions(const char *path, const struct stat *st);

số_nguyên access_nofollow(const char *path, số_nguyên mode);
số_nguyên access_fd(số_nguyên fd, số_nguyên mode);

số_nguyên touch_fd(số_nguyên fd, usec_t stamp);
số_nguyên touch_file(const char *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
số_nguyên touch(const char *path);

số_nguyên symlinkat_idempotent(const char *target, số_nguyên atfd, const char *linkpath, bool make_relative);
static inline số_nguyên symlink_idempotent(const char *target, const char *linkpath, bool make_relative) {
        trả_về symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef enum SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

số_nguyên symlinkat_atomic_full_label(const char *target, số_nguyên atfd, const char *linkpath, SymlinkFlags flags, LabelContext *label_context);
static inline số_nguyên symlinkat_atomic_full(const char *target, số_nguyên atfd, const char *linkpath, SymlinkFlags flags) {
        trả_về symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static inline số_nguyên symlink_atomic(const char *target, const char *linkpath) {
        trả_về symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

số_nguyên mknodat_atomic(số_nguyên atfd, const char *path, mode_t mode, dev_t dev);
static inline số_nguyên mknod_atomic(const char *path, mode_t mode, dev_t dev) {
        trả_về mknodat_atomic(AT_FDCWD, path, mode, dev);
}

số_nguyên mkfifoat_atomic(số_nguyên dir_fd, const char *path, mode_t mode);
static inline số_nguyên mkfifo_atomic(const char *path, mode_t mode) {
        trả_về mkfifoat_atomic(AT_FDCWD, path, mode);
}

số_nguyên get_files_in_directory(const char *path, char ***list);

số_nguyên tmp_dir(const char **ret);
số_nguyên var_tmp_dir(const char **ret);

số_nguyên unlink_or_warn(const char *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
char *rmdir_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, rmdir_and_free);

char* unlink_and_free(char *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(char*, unlink_and_free);

typedef enum UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

số_nguyên unlinkat_deallocate(số_nguyên fd, const char *name, UnlinkDeallocateFlags flags);

số_nguyên open_parent_at(số_nguyên dir_fd, const char *path, số_nguyên flags, mode_t mode);
static inline số_nguyên open_parent(const char *path, số_nguyên flags, mode_t mode) {
        trả_về open_parent_at(AT_FDCWD, path, flags, mode);
}

số_nguyên conservative_renameat(số_nguyên olddirfd, const char *oldpath, số_nguyên newdirfd, const char *newpath);
static inline số_nguyên conservative_rename(const char *oldpath, const char *newpath) {
        trả_về conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

số_nguyên posix_fallocate_loop(số_nguyên fd, uint64_t offset, uint64_t size);

số_nguyên parse_cifs_service(const char *s, char **ret_host, char **ret_service, char **ret_path);

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

số_nguyên open_mkdir_at_full_label(số_nguyên dirfd, const char *path, số_nguyên flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline số_nguyên open_mkdir_at_full(số_nguyên dirfd, const char *path, số_nguyên flags, XOpenFlags xopen_flags, mode_t mode) {
        trả_về open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline số_nguyên open_mkdir_at(số_nguyên dirfd, const char *path, số_nguyên flags, mode_t mode) {
        trả_về open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static inline số_nguyên open_mkdir(const char *path, số_nguyên flags, mode_t mode) {
        trả_về open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

số_nguyên openat_report_new(số_nguyên dirfd, const char *pathname, số_nguyên flags, mode_t mode, bool *ret_newly_created);

số_nguyên xopenat_full_label(số_nguyên dir_fd, const char *path, số_nguyên open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static inline số_nguyên xopenat_full(số_nguyên dir_fd, const char *path, số_nguyên open_flags, XOpenFlags xopen_flags, mode_t mode) {
        trả_về xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static inline số_nguyên xopenat(số_nguyên dir_fd, const char *path, số_nguyên open_flags) {
        trả_về xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

số_nguyên xopenat_lock_full_label(số_nguyên dir_fd, const char *path, số_nguyên open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, số_nguyên operation, LabelContext *label_context);
static inline số_nguyên xopenat_lock_full(số_nguyên dir_fd, const char *path, số_nguyên open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, số_nguyên operation) {
        trả_về xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static inline số_nguyên xopenat_lock(số_nguyên dir_fd, const char *path, số_nguyên open_flags, LockType locktype, số_nguyên operation) {
        trả_về xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

số_nguyên link_fd(số_nguyên fd, số_nguyên newdirfd, const char *newpath);

số_nguyên linkat_replace(số_nguyên olddirfd, const char *oldpath, số_nguyên newdirfd, const char *newpath);

static inline số_nguyên at_flags_normalize_nofollow(số_nguyên flags) {
        nếu (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } không_thì
                flags |= AT_SYMLINK_NOFOLLOW;
        trả_về flags;
}

static inline số_nguyên at_flags_normalize_follow(số_nguyên flags) {
        nếu (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } không_thì
                flags |= AT_SYMLINK_FOLLOW;
        trả_về flags;
}
