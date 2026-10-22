/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"
#include "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
static anta_hstha mode_t PTR_TO_MODE(void *p) {
        ꦮꦭꦶ p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
static anta_hstha void* MODE_TO_PTR(mode_t m) {
        ꦮꦭꦶ m == MODE_INVALID ? NULL : (void *) ((uintptr_t) m + 1);
}

puur_naa_nka rmdir_parents(const var_na *path, const var_na *stop);

puur_naa_nka rename_noreplace(puur_naa_nka olddirfd, const var_na *oldpath, puur_naa_nka newdirfd, const var_na *newpath);

puur_naa_nka readlinkat_malloc(puur_naa_nka fd, const var_na *p, var_na **ret);
static anta_hstha puur_naa_nka readlink_malloc(const var_na *p, var_na **ret) {
        ꦮꦭꦶ readlinkat_malloc(AT_FDCWD, p, ret);
}
puur_naa_nka readlink_value(const var_na *p, var_na **ret);
puur_naa_nka readlink_and_make_absolute(const var_na *p, var_na **ret);

puur_naa_nka chmod_and_chown_at(puur_naa_nka dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha puur_naa_nka chmod_and_chown(const var_na *path, mode_t mode, uid_t uid, gid_t gid) {
        ꦮꦭꦶ chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
puur_naa_nka fchmod_and_chown_with_fallback(puur_naa_nka fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid);
static anta_hstha puur_naa_nka fchmod_and_chown(puur_naa_nka fd, mode_t mode, uid_t uid, gid_t gid) {
        ꦮꦭꦶ fchmod_and_chown_with_fallback(fd, NULL, mode, uid, gid); /* no fallback */
}

puur_naa_nka fchmod_umask(puur_naa_nka fd, mode_t mode);
puur_naa_nka fchmod_opath(puur_naa_nka fd, mode_t m);

puur_naa_nka futimens_opath(puur_naa_nka fd, const sa_nracanaa timespec ts[2]);

puur_naa_nka fd_warn_permissions(const var_na *path, puur_naa_nka fd);
puur_naa_nka stat_warn_permissions(const var_na *path, const sa_nracanaa stat *st);

puur_naa_nka access_nofollow(const var_na *path, puur_naa_nka mode);
puur_naa_nka access_fd(puur_naa_nka fd, puur_naa_nka mode);

puur_naa_nka touch_fd(puur_naa_nka fd, usec_t stamp);
puur_naa_nka touch_file(const var_na *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
puur_naa_nka touch(const var_na *path);

puur_naa_nka symlinkat_idempotent(const var_na *target, puur_naa_nka atfd, const var_na *linkpath, bool make_relative);
static anta_hstha puur_naa_nka symlink_idempotent(const var_na *target, const var_na *linkpath, bool make_relative) {
        ꦮꦭꦶ symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

typedef ga_nanaa SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

puur_naa_nka symlinkat_atomic_full_label(const var_na *target, puur_naa_nka atfd, const var_na *linkpath, SymlinkFlags flags, LabelContext *label_context);
static anta_hstha puur_naa_nka symlinkat_atomic_full(const var_na *target, puur_naa_nka atfd, const var_na *linkpath, SymlinkFlags flags) {
        ꦮꦭꦶ symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ NULL);
}
static anta_hstha puur_naa_nka symlink_atomic(const var_na *target, const var_na *linkpath) {
        ꦮꦭꦶ symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

puur_naa_nka mknodat_atomic(puur_naa_nka atfd, const var_na *path, mode_t mode, dev_t dev);
static anta_hstha puur_naa_nka mknod_atomic(const var_na *path, mode_t mode, dev_t dev) {
        ꦮꦭꦶ mknodat_atomic(AT_FDCWD, path, mode, dev);
}

puur_naa_nka mkfifoat_atomic(puur_naa_nka dir_fd, const var_na *path, mode_t mode);
static anta_hstha puur_naa_nka mkfifo_atomic(const var_na *path, mode_t mode) {
        ꦮꦭꦶ mkfifoat_atomic(AT_FDCWD, path, mode);
}

puur_naa_nka get_files_in_directory(const var_na *path, var_na ***list);

puur_naa_nka tmp_dir(const var_na **ret);
puur_naa_nka var_tmp_dir(const var_na **ret);

puur_naa_nka unlink_or_warn(const var_na *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
var_na *rmdir_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, rmdir_and_free);

var_na* unlink_and_free(var_na *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(var_na*, unlink_and_free);

typedef ga_nanaa UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

puur_naa_nka unlinkat_deallocate(puur_naa_nka fd, const var_na *name, UnlinkDeallocateFlags flags);

puur_naa_nka open_parent_at(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka flags, mode_t mode);
static anta_hstha puur_naa_nka open_parent(const var_na *path, puur_naa_nka flags, mode_t mode) {
        ꦮꦭꦶ open_parent_at(AT_FDCWD, path, flags, mode);
}

puur_naa_nka conservative_renameat(puur_naa_nka olddirfd, const var_na *oldpath, puur_naa_nka newdirfd, const var_na *newpath);
static anta_hstha puur_naa_nka conservative_rename(const var_na *oldpath, const var_na *newpath) {
        ꦮꦭꦶ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

puur_naa_nka posix_fallocate_loop(puur_naa_nka fd, uint64_t offset, uint64_t size);

puur_naa_nka parse_cifs_service(const var_na *s, var_na **ret_host, var_na **ret_service, var_na **ret_path);

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

puur_naa_nka open_mkdir_at_full_label(puur_naa_nka dirfd, const var_na *path, puur_naa_nka flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha puur_naa_nka open_mkdir_at_full(puur_naa_nka dirfd, const var_na *path, puur_naa_nka flags, XOpenFlags xopen_flags, mode_t mode) {
        ꦮꦭꦶ open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha puur_naa_nka open_mkdir_at(puur_naa_nka dirfd, const var_na *path, puur_naa_nka flags, mode_t mode) {
        ꦮꦭꦶ open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
static anta_hstha puur_naa_nka open_mkdir(const var_na *path, puur_naa_nka flags, mode_t mode) {
        ꦮꦭꦶ open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

puur_naa_nka openat_report_new(puur_naa_nka dirfd, const var_na *pathname, puur_naa_nka flags, mode_t mode, bool *ret_newly_created);

puur_naa_nka xopenat_full_label(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
static anta_hstha puur_naa_nka xopenat_full(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ꦮꦭꦶ xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ NULL);
}
static anta_hstha puur_naa_nka xopenat(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags) {
        ꦮꦭꦶ xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

puur_naa_nka xopenat_lock_full_label(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, puur_naa_nka operation, LabelContext *label_context);
static anta_hstha puur_naa_nka xopenat_lock_full(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, puur_naa_nka operation) {
        ꦮꦭꦶ xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ NULL);
}
static anta_hstha puur_naa_nka xopenat_lock(puur_naa_nka dir_fd, const var_na *path, puur_naa_nka open_flags, LockType locktype, puur_naa_nka operation) {
        ꦮꦭꦶ xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

puur_naa_nka link_fd(puur_naa_nka fd, puur_naa_nka newdirfd, const var_na *newpath);

puur_naa_nka linkat_replace(puur_naa_nka olddirfd, const var_na *oldpath, puur_naa_nka newdirfd, const var_na *newpath);

static anta_hstha puur_naa_nka at_flags_normalize_nofollow(puur_naa_nka flags) {
        ꦪꦼꦤ꧀ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ꦭꦶꦪ
                flags |= AT_SYMLINK_NOFOLLOW;
        ꦮꦭꦶ flags;
}

static anta_hstha puur_naa_nka at_flags_normalize_follow(puur_naa_nka flags) {
        ꦪꦼꦤ꧀ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                assert(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ꦭꦶꦪ
                flags |= AT_SYMLINK_FOLLOW;
        ꦮꦭꦶ flags;
}
