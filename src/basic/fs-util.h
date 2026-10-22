/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
স্থির অন্তঃস্থ mode_t PTR_TO_MODE(শূন্য *p) {
        ঘূৰাই p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
স্থির অন্তঃস্থ শূন্য* MODE_TO_PTR(mode_t m) {
        ঘূৰাই m == MODE_INVALID ? রিক্ত : (শূন্য *) ((uintptr_t) m + 1);
}

পূর্ণাঙ্ক rmdir_parents(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *stop);

পূর্ণাঙ্ক rename_noreplace(পূর্ণাঙ্ক olddirfd, ধ্রুৱ ৱর্ণ *oldpath, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath);

পূর্ণাঙ্ক readlinkat_malloc(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক readlink_malloc(ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret) {
        ঘূৰাই readlinkat_malloc(AT_FDCWD, p, ret);
}
পূর্ণাঙ্ক readlink_মান(ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret);
পূর্ণাঙ্ক readlink_and_make_absolute(ধ্রুৱ ৱর্ণ *p, ৱর্ণ **ret);

পূর্ণাঙ্ক chmod_and_chown_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid);
স্থির অন্তঃস্থ পূর্ণাঙ্ক chmod_and_chown(ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid) {
        ঘূৰাই chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
পূর্ণাঙ্ক fchmod_and_chown_with_fallback(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fchmod_and_chown(পূর্ণাঙ্ক fd, mode_t mode, uid_t uid, gid_t gid) {
        ঘূৰাই fchmod_and_chown_with_fallback(fd, রিক্ত, mode, uid, gid); /* no fallback */
}

পূর্ণাঙ্ক fchmod_umask(পূর্ণাঙ্ক fd, mode_t mode);
পূর্ণাঙ্ক fchmod_opath(পূর্ণাঙ্ক fd, mode_t m);

পূর্ণাঙ্ক futimens_opath(পূর্ণাঙ্ক fd, ধ্রুৱ সংরচনা timespec ts[2]);

পূর্ণাঙ্ক fd_warn_permissions(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক stat_warn_permissions(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ সংরচনা stat *st);

পূর্ণাঙ্ক access_nofollow(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক mode);
পূর্ণাঙ্ক access_fd(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক mode);

পূর্ণাঙ্ক touch_fd(পূর্ণাঙ্ক fd, usec_t stamp);
পূর্ণাঙ্ক touch_file(ধ্রুৱ ৱর্ণ *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
পূর্ণাঙ্ক touch(ধ্রুৱ ৱর্ণ *path);

পূর্ণাঙ্ক symlinkat_idempotent(ধ্রুৱ ৱর্ণ *target, পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *linkpath, bool make_relative);
স্থির অন্তঃস্থ পূর্ণাঙ্ক symlink_idempotent(ধ্রুৱ ৱর্ণ *target, ধ্রুৱ ৱর্ণ *linkpath, bool make_relative) {
        ঘূৰাই symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

প্রকারনির্দেশ গণনা SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

পূর্ণাঙ্ক symlinkat_atomic_full_label(ধ্রুৱ ৱর্ণ *target, পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *linkpath, SymlinkFlags flags, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক symlinkat_atomic_full(ধ্রুৱ ৱর্ণ *target, পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *linkpath, SymlinkFlags flags) {
        ঘূৰাই symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ রিক্ত);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক symlink_atomic(ধ্রুৱ ৱর্ণ *target, ধ্রুৱ ৱর্ণ *linkpath) {
        ঘূৰাই symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

পূর্ণাঙ্ক mknodat_atomic(পূর্ণাঙ্ক atfd, ধ্রুৱ ৱর্ণ *path, mode_t mode, dev_t dev);
স্থির অন্তঃস্থ পূর্ণাঙ্ক mknod_atomic(ধ্রুৱ ৱর্ণ *path, mode_t mode, dev_t dev) {
        ঘূৰাই mknodat_atomic(AT_FDCWD, path, mode, dev);
}

পূর্ণাঙ্ক mkfifoat_atomic(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode);
স্থির অন্তঃস্থ পূর্ণাঙ্ক mkfifo_atomic(ধ্রুৱ ৱর্ণ *path, mode_t mode) {
        ঘূৰাই mkfifoat_atomic(AT_FDCWD, path, mode);
}

পূর্ণাঙ্ক get_files_in_directory(ধ্রুৱ ৱর্ণ *path, ৱর্ণ ***list);

পূর্ণাঙ্ক tmp_dir(ধ্রুৱ ৱর্ণ **ret);
পূর্ণাঙ্ক var_tmp_dir(ধ্রুৱ ৱর্ণ **ret);

পূর্ণাঙ্ক unlink_or_warn(ধ্রুৱ ৱর্ণ *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
ৱর্ণ *rmdir_and_free(ৱর্ণ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ৱর্ণ*, rmdir_and_free);

ৱর্ণ* unlink_and_free(ৱর্ণ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ৱর্ণ*, unlink_and_free);

প্রকারনির্দেশ গণনা UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

পূর্ণাঙ্ক unlinkat_deallocate(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *name, UnlinkDeallocateFlags flags);

পূর্ণাঙ্ক open_parent_at(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, mode_t mode);
স্থির অন্তঃস্থ পূর্ণাঙ্ক open_parent(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, mode_t mode) {
        ঘূৰাই open_parent_at(AT_FDCWD, path, flags, mode);
}

পূর্ণাঙ্ক conservative_renameat(পূর্ণাঙ্ক olddirfd, ধ্রুৱ ৱর্ণ *oldpath, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath);
স্থির অন্তঃস্থ পূর্ণাঙ্ক conservative_rename(ধ্রুৱ ৱর্ণ *oldpath, ধ্রুৱ ৱর্ণ *newpath) {
        ঘূৰাই conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

পূর্ণাঙ্ক posix_fallocate_loop(পূর্ণাঙ্ক fd, uint64_t offset, uint64_t size);

পূর্ণাঙ্ক parse_cifs_service(ধ্রুৱ ৱর্ণ *s, ৱর্ণ **ret_host, ৱর্ণ **ret_service, ৱর্ণ **ret_path);

প্রকারনির্দেশ গণনা XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

পূর্ণাঙ্ক open_mkdir_at_full_label(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক open_mkdir_at_full(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, XOpenFlags xopen_flags, mode_t mode) {
        ঘূৰাই open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ রিক্ত);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক open_mkdir_at(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, mode_t mode) {
        ঘূৰাই open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক open_mkdir(ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক flags, mode_t mode) {
        ঘূৰাই open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

পূর্ণাঙ্ক openat_report_new(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *pathname, পূর্ণাঙ্ক flags, mode_t mode, bool *ret_newly_created);

পূর্ণাঙ্ক xopenat_full_label(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক xopenat_full(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ঘূৰাই xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ রিক্ত);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক xopenat(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags) {
        ঘূৰাই xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

পূর্ণাঙ্ক xopenat_lock_full_label(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, পূর্ণাঙ্ক operation, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক xopenat_lock_full(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, পূর্ণাঙ্ক operation) {
        ঘূৰাই xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ রিক্ত);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক xopenat_lock(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, পূর্ণাঙ্ক open_flags, LockType locktype, পূর্ণাঙ্ক operation) {
        ঘূৰাই xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

পূর্ণাঙ্ক link_fd(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath);

পূর্ণাঙ্ক linkat_replace(পূর্ণাঙ্ক olddirfd, ধ্রুৱ ৱর্ণ *oldpath, পূর্ণাঙ্ক newdirfd, ধ্রুৱ ৱর্ণ *newpath);

স্থির অন্তঃস্থ পূর্ণাঙ্ক at_flags_normalize_nofollow(পূর্ণাঙ্ক flags) {
        যদি (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                প্রতিজ্ঞা(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } অন্যথা
                flags |= AT_SYMLINK_NOFOLLOW;
        ঘূৰাই flags;
}

স্থির অন্তঃস্থ পূর্ণাঙ্ক at_flags_normalize_follow(পূর্ণাঙ্ক flags) {
        যদি (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                প্রতিজ্ঞা(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } অন্যথা
                flags |= AT_SYMLINK_FOLLOW;
        ঘূৰাই flags;
}
