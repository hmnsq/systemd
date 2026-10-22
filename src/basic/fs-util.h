/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ mode_t PTR_TO_MODE(ಶೂನ್ಯ *p) {
        ಪ್ರತ್ಯಾಗಮ p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* MODE_TO_PTR(mode_t m) {
        ಪ್ರತ್ಯಾಗಮ m == MODE_INVALID ? ರಿಕ್ತ : (ಶೂನ್ಯ *) ((uintptr_t) m + 1);
}

ಪೂರ್ಣಾಙ್ಕ rmdir_parents(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *stop);

ಪೂರ್ಣಾಙ್ಕ rename_noreplace(ಪೂರ್ಣಾಙ್ಕ olddirfd, ಧ್ರುವ ವರ್ಣ *oldpath, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath);

ಪೂರ್ಣಾಙ್ಕ readlinkat_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ readlink_malloc(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ readlinkat_malloc(AT_FDCWD, p, ret);
}
ಪೂರ್ಣಾಙ್ಕ readlink_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ readlink_and_make_absolute(ಧ್ರುವ ವರ್ಣ *p, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ chmod_and_chown_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ chmod_and_chown(ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid) {
        ಪ್ರತ್ಯಾಗಮ chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ಪೂರ್ಣಾಙ್ಕ fchmod_and_chown_with_fallback(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fchmod_and_chown(ಪೂರ್ಣಾಙ್ಕ fd, mode_t mode, uid_t uid, gid_t gid) {
        ಪ್ರತ್ಯಾಗಮ fchmod_and_chown_with_fallback(fd, ರಿಕ್ತ, mode, uid, gid); /* no fallback */
}

ಪೂರ್ಣಾಙ್ಕ fchmod_umask(ಪೂರ್ಣಾಙ್ಕ fd, mode_t mode);
ಪೂರ್ಣಾಙ್ಕ fchmod_opath(ಪೂರ್ಣಾಙ್ಕ fd, mode_t m);

ಪೂರ್ಣಾಙ್ಕ futimens_opath(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಸಂರಚನಾ timespec ts[2]);

ಪೂರ್ಣಾಙ್ಕ fd_warn_permissions(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ stat_warn_permissions(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ಸಂರಚನಾ stat *st);

ಪೂರ್ಣಾಙ್ಕ access_nofollow(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ mode);
ಪೂರ್ಣಾಙ್ಕ access_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ mode);

ಪೂರ್ಣಾಙ್ಕ touch_fd(ಪೂರ್ಣಾಙ್ಕ fd, usec_t stamp);
ಪೂರ್ಣಾಙ್ಕ touch_file(ಧ್ರುವ ವರ್ಣ *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ಪೂರ್ಣಾಙ್ಕ touch(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ symlinkat_idempotent(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *linkpath, bool make_relative);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ symlink_idempotent(ಧ್ರುವ ವರ್ಣ *target, ಧ್ರುವ ವರ್ಣ *linkpath, bool make_relative) {
        ಪ್ರತ್ಯಾಗಮ symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ಪೂರ್ಣಾಙ್ಕ symlinkat_atomic_full_label(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *linkpath, SymlinkFlags flags, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ symlinkat_atomic_full(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *linkpath, SymlinkFlags flags) {
        ಪ್ರತ್ಯಾಗಮ symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ symlink_atomic(ಧ್ರುವ ವರ್ಣ *target, ಧ್ರುವ ವರ್ಣ *linkpath) {
        ಪ್ರತ್ಯಾಗಮ symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ಪೂರ್ಣಾಙ್ಕ mknodat_atomic(ಪೂರ್ಣಾಙ್ಕ atfd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, dev_t dev);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ mknod_atomic(ಧ್ರುವ ವರ್ಣ *path, mode_t mode, dev_t dev) {
        ಪ್ರತ್ಯಾಗಮ mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ಪೂರ್ಣಾಙ್ಕ mkfifoat_atomic(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ mkfifo_atomic(ಧ್ರುವ ವರ್ಣ *path, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ mkfifoat_atomic(AT_FDCWD, path, mode);
}

ಪೂರ್ಣಾಙ್ಕ get_files_in_directory(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ ***list);

ಪೂರ್ಣಾಙ್ಕ tmp_dir(ಧ್ರುವ ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ var_tmp_dir(ಧ್ರುವ ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ unlink_or_warn(ಧ್ರುವ ವರ್ಣ *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
ವರ್ಣ *rmdir_and_free(ವರ್ಣ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ವರ್ಣ*, rmdir_and_free);

ವರ್ಣ* unlink_and_free(ವರ್ಣ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ವರ್ಣ*, unlink_and_free);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ಪೂರ್ಣಾಙ್ಕ unlinkat_deallocate(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *name, UnlinkDeallocateFlags flags);

ಪೂರ್ಣಾಙ್ಕ open_parent_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ open_parent(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ open_parent_at(AT_FDCWD, path, flags, mode);
}

ಪೂರ್ಣಾಙ್ಕ conservative_renameat(ಪೂರ್ಣಾಙ್ಕ olddirfd, ಧ್ರುವ ವರ್ಣ *oldpath, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ conservative_rename(ಧ್ರುವ ವರ್ಣ *oldpath, ಧ್ರುವ ವರ್ಣ *newpath) {
        ಪ್ರತ್ಯಾಗಮ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ಪೂರ್ಣಾಙ್ಕ posix_fallocate_loop(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t offset, uint64_t size);

ಪೂರ್ಣಾಙ್ಕ parse_cifs_service(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ **ret_host, ವರ್ಣ **ret_service, ವರ್ಣ **ret_path);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

ಪೂರ್ಣಾಙ್ಕ open_mkdir_at_full_label(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ open_mkdir_at_full(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, XOpenFlags xopen_flags, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ open_mkdir_at(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ open_mkdir(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ಪೂರ್ಣಾಙ್ಕ openat_report_new(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *pathname, ಪೂರ್ಣಾಙ್ಕ flags, mode_t mode, bool *ret_newly_created);

ಪೂರ್ಣಾಙ್ಕ xopenat_full_label(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xopenat_full(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xopenat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags) {
        ಪ್ರತ್ಯಾಗಮ xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ಪೂರ್ಣಾಙ್ಕ xopenat_lock_full_label(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ಪೂರ್ಣಾಙ್ಕ operation, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xopenat_lock_full(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ಪೂರ್ಣಾಙ್ಕ operation) {
        ಪ್ರತ್ಯಾಗಮ xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xopenat_lock(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ open_flags, LockType locktype, ಪೂರ್ಣಾಙ್ಕ operation) {
        ಪ್ರತ್ಯಾಗಮ xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ಪೂರ್ಣಾಙ್ಕ link_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath);

ಪೂರ್ಣಾಙ್ಕ linkat_replace(ಪೂರ್ಣಾಙ್ಕ olddirfd, ಧ್ರುವ ವರ್ಣ *oldpath, ಪೂರ್ಣಾಙ್ಕ newdirfd, ಧ್ರುವ ವರ್ಣ *newpath);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ at_flags_normalize_nofollow(ಪೂರ್ಣಾಙ್ಕ flags) {
        ಯದಿ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ಅನ್ಯಥಾ
                flags |= AT_SYMLINK_NOFOLLOW;
        ಪ್ರತ್ಯಾಗಮ flags;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ at_flags_normalize_follow(ಪೂರ್ಣಾಙ್ಕ flags) {
        ಯದಿ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ಅನ್ಯಥಾ
                flags |= AT_SYMLINK_FOLLOW;
        ಪ್ರತ್ಯಾಗಮ flags;
}
