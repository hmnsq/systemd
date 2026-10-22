/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        คืน p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        คืน m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

จำนวนเต็ม rmdir_parents(const var_na *path, const var_na *stop);

จำนวนเต็ม rename_noreplace(จำนวนเต็ม olddirfd, const var_na *oldpath, จำนวนเต็ม newdirfd, const var_na *newpath);

จำนวนเต็ม readlinkat_malloc(จำนวนเต็ม fd, const var_na *p, var_na **ret);
static anta_hstha จำนวนเต็ม readlink_malloc(const var_na *p, var_na **ret) {
        คืน readlinkat_malloc(AT_FDCWD, p, ret);
}
จำนวนเต็ม readlink_value(const var_na *p, var_na **ret);
จำนวนเต็ม readlink_and_make_absolute(const var_na *p, var_na **ret);

จำนวนเต็ม chmod_and_chown_at(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha จำนวนเต็ม chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        คืน chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
จำนวนเต็ม fchmod_and_chown_with_fallback(จำนวนเต็ม fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha จำนวนเต็ม fchmod_and_chown(จำนวนเต็ม fd, mode_t mode, uid_t uid, gid_t gid) {
        คืน fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

จำนวนเต็ม fchmod_umask(จำนวนเต็ม fd, mode_t mode);
จำนวนเต็ม fchmod_opath(จำนวนเต็ม fd, mode_t m);

จำนวนเต็ม futimens_opath(จำนวนเต็ม fd, const sa_nracanaa timespec ts[2]);

จำนวนเต็ม fd_warn_permissions(const var_na *path, จำนวนเต็ม fd);
จำนวนเต็ม stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

จำนวนเต็ม access_nofollow(const var_na *path, จำนวนเต็ม mode);
จำนวนเต็ม access_fd(จำนวนเต็ม fd, จำนวนเต็ม mode);

จำนวนเต็ม touch_fd(จำนวนเต็ม fd, usec_t stamp);
จำนวนเต็ม touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
จำนวนเต็ม touch(const var_na *path);

จำนวนเต็ม symlinkat_idempotent(const var_na *target, จำนวนเต็ม atfd, const var_na *linkpath, bool make_relative);
static anta_hstha จำนวนเต็ม symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        คืน symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

จำนวนเต็ม symlinkat_atomic_full_label(const var_na *target, จำนวนเต็ม atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha จำนวนเต็ม symlinkat_atomic_full(const var_na *target, จำนวนเต็ม atfd, const var_na *linkpath, SymlinkFlags flags) {
        คืน symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha จำนวนเต็ม symlink_atomic(const var_na *target, const var_na *linkpath) {
        คืน symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

จำนวนเต็ม mknodat_atomic(จำนวนเต็ม atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha จำนวนเต็ม mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        คืน mknodat_atomic(AT_FDCWD, path, mode, dev);
}

จำนวนเต็ม mkfifoat_atomic(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode);
static anta_hstha จำนวนเต็ม mkfifo_atomic(const var_na *path, mode_t mode) {
        คืน mkfifoat_atomic(AT_FDCWD, path, mode);
}

จำนวนเต็ม get_files_in_directory(const var_na *path, var_na ***list);

จำนวนเต็ม tmp_dir(const var_na **ret);
จำนวนเต็ม var_tmp_dir(const var_na **ret);

จำนวนเต็ม unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

จำนวนเต็ม unlinkat_deallocate(จำนวนเต็ม fd, const var_na *name, UnlinkDeallocateFlags flags);

จำนวนเต็ม open_parent_at(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม flags, mode_t mode);
static anta_hstha จำนวนเต็ม open_parent(const var_na *path, จำนวนเต็ม flags, mode_t mode) {
        คืน open_parent_at(AT_FDCWD, path, flags, mode);
}

จำนวนเต็ม conservative_renameat(จำนวนเต็ม olddirfd, const var_na *oldpath, จำนวนเต็ม newdirfd, const var_na *newpath);
static anta_hstha จำนวนเต็ม conservative_rename(const var_na *oldpath, const var_na *newpath) {
        คืน conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

จำนวนเต็ม posix_fallocate_loop(จำนวนเต็ม fd, uint64_t offset, uint64_t size);

จำนวนเต็ม parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

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

จำนวนเต็ม open_mkdir_at_full_label(จำนวนเต็ม dirfd, const var_na *path, จำนวนเต็ม flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha จำนวนเต็ม open_mkdir_at_full(จำนวนเต็ม dirfd, const var_na *path, จำนวนเต็ม flags, XOpenFlags xopen_flags, mode_t mode) {
        คืน open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha จำนวนเต็ม open_mkdir_at(จำนวนเต็ม dirfd, const var_na *path, จำนวนเต็ม flags, mode_t mode) {
        คืน open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha จำนวนเต็ม open_mkdir(const var_na *path, จำนวนเต็ม flags, mode_t mode) {
        คืน open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

จำนวนเต็ม openat_report_new(จำนวนเต็ม dirfd, const var_na *pathname, จำนวนเต็ม flags, mode_t mode, bool *ret_newly_created);

จำนวนเต็ม xopenat_full_label(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha จำนวนเต็ม xopenat_full(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม open_flags, XOpenFlags xopen_flags, mode_t mode) {
        คืน xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha จำนวนเต็ม xopenat(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม open_flags) {
        คืน xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

จำนวนเต็ม xopenat_lock_full_label(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, จำนวนเต็ม operation, LabelContext *label_context);
static anta_hstha จำนวนเต็ม xopenat_lock_full(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, จำนวนเต็ม operation) {
        คืน xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha จำนวนเต็ม xopenat_lock(จำนวนเต็ม dir_fd, const var_na *path, จำนวนเต็ม open_flags, LockType locktype, จำนวนเต็ม operation) {
        คืน xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

จำนวนเต็ม link_fd(จำนวนเต็ม fd, จำนวนเต็ม newdirfd, const var_na *newpath);

จำนวนเต็ม linkat_replace(จำนวนเต็ม olddirfd, const var_na *oldpath, จำนวนเต็ม newdirfd, const var_na *newpath);

static anta_hstha จำนวนเต็ม at_flags_normalize_nofollow(จำนวนเต็ม flags) {
        ถ้า (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } มิฉะนั้น
                flags |= AT_SYMLINK_NOFOLLOW;
        คืน flags;
}

static anta_hstha จำนวนเต็ม at_flags_normalize_follow(จำนวนเต็ม flags) {
        ถ้า (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } มิฉะนั้น
                flags |= AT_SYMLINK_FOLLOW;
        คืน flags;
}
