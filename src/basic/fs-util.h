/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"
#অন্তর্ভাব "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
স্থির অন্তঃরেখ mode_t PTR_TO_MODE(শূন্য *p) {
        ফেরত p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
স্থির অন্তঃরেখ শূন্য* MODE_TO_PTR(mode_t m) {
        ফেরত m == MODE_INVALID ? ফাঁকা : (শূন্য *) ((uintptr_t) m + 1);
}

পূর্ণসংখ্যা rmdir_parents(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *stop);

পূর্ণসংখ্যা rename_noreplace(পূর্ণসংখ্যা olddirfd, ধ্রুবক অক্ষর *oldpath, পূর্ণসংখ্যা newdirfd, ধ্রুবক অক্ষর *newpath);

পূর্ণসংখ্যা readlinkat_malloc(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *p, অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা readlink_malloc(ধ্রুবক অক্ষর *p, অক্ষর **ret) {
        ফেরত readlinkat_malloc(AT_FDCWD, p, ret);
}
পূর্ণসংখ্যা readlink_মান(ধ্রুবক অক্ষর *p, অক্ষর **ret);
পূর্ণসংখ্যা readlink_and_make_absolute(ধ্রুবক অক্ষর *p, অক্ষর **ret);

পূর্ণসংখ্যা chmod_and_chown_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid);
স্থির অন্তঃরেখ পূর্ণসংখ্যা chmod_and_chown(ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid) {
        ফেরত chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
পূর্ণসংখ্যা fchmod_and_chown_with_fallback(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fchmod_and_chown(পূর্ণসংখ্যা fd, mode_t mode, uid_t uid, gid_t gid) {
        ফেরত fchmod_and_chown_with_fallback(fd, ফাঁকা, mode, uid, gid); /* no fallback */
}

পূর্ণসংখ্যা fchmod_umask(পূর্ণসংখ্যা fd, mode_t mode);
পূর্ণসংখ্যা fchmod_opath(পূর্ণসংখ্যা fd, mode_t m);

পূর্ণসংখ্যা futimens_opath(পূর্ণসংখ্যা fd, ধ্রুবক কাঠামো timespec ts[2]);

পূর্ণসংখ্যা fd_warn_permissions(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা stat_warn_permissions(ধ্রুবক অক্ষর *path, ধ্রুবক কাঠামো stat *st);

পূর্ণসংখ্যা access_nofollow(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা mode);
পূর্ণসংখ্যা access_fd(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা mode);

পূর্ণসংখ্যা touch_fd(পূর্ণসংখ্যা fd, usec_t stamp);
পূর্ণসংখ্যা touch_file(ধ্রুবক অক্ষর *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
পূর্ণসংখ্যা touch(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা symlinkat_idempotent(ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা atfd, ধ্রুবক অক্ষর *linkpath, bool make_relative);
স্থির অন্তঃরেখ পূর্ণসংখ্যা symlink_idempotent(ধ্রুবক অক্ষর *target, ধ্রুবক অক্ষর *linkpath, bool make_relative) {
        ফেরত symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

প্রকারলেখ গণনা SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

পূর্ণসংখ্যা symlinkat_atomic_full_label(ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা atfd, ধ্রুবক অক্ষর *linkpath, SymlinkFlags flags, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা symlinkat_atomic_full(ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা atfd, ধ্রুবক অক্ষর *linkpath, SymlinkFlags flags) {
        ফেরত symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা symlink_atomic(ধ্রুবক অক্ষর *target, ধ্রুবক অক্ষর *linkpath) {
        ফেরত symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

পূর্ণসংখ্যা mknodat_atomic(পূর্ণসংখ্যা atfd, ধ্রুবক অক্ষর *path, mode_t mode, dev_t dev);
স্থির অন্তঃরেখ পূর্ণসংখ্যা mknod_atomic(ধ্রুবক অক্ষর *path, mode_t mode, dev_t dev) {
        ফেরত mknodat_atomic(AT_FDCWD, path, mode, dev);
}

পূর্ণসংখ্যা mkfifoat_atomic(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode);
স্থির অন্তঃরেখ পূর্ণসংখ্যা mkfifo_atomic(ধ্রুবক অক্ষর *path, mode_t mode) {
        ফেরত mkfifoat_atomic(AT_FDCWD, path, mode);
}

পূর্ণসংখ্যা get_files_in_directory(ধ্রুবক অক্ষর *path, অক্ষর ***list);

পূর্ণসংখ্যা tmp_dir(ধ্রুবক অক্ষর **ret);
পূর্ণসংখ্যা var_tmp_dir(ধ্রুবক অক্ষর **ret);

পূর্ণসংখ্যা unlink_or_warn(ধ্রুবক অক্ষর *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
অক্ষর *rmdir_and_free(অক্ষর *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(অক্ষর*, rmdir_and_free);

অক্ষর* unlink_and_free(অক্ষর *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(অক্ষর*, unlink_and_free);

প্রকারলেখ গণনা UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

পূর্ণসংখ্যা unlinkat_deallocate(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *name, UnlinkDeallocateFlags flags);

পূর্ণসংখ্যা open_parent_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, mode_t mode);
স্থির অন্তঃরেখ পূর্ণসংখ্যা open_parent(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, mode_t mode) {
        ফেরত open_parent_at(AT_FDCWD, path, flags, mode);
}

পূর্ণসংখ্যা conservative_renameat(পূর্ণসংখ্যা olddirfd, ধ্রুবক অক্ষর *oldpath, পূর্ণসংখ্যা newdirfd, ধ্রুবক অক্ষর *newpath);
স্থির অন্তঃরেখ পূর্ণসংখ্যা conservative_rename(ধ্রুবক অক্ষর *oldpath, ধ্রুবক অক্ষর *newpath) {
        ফেরত conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

পূর্ণসংখ্যা posix_fallocate_loop(পূর্ণসংখ্যা fd, uint64_t offset, uint64_t আকার_2);

পূর্ণসংখ্যা parse_cifs_service(ধ্রুবক অক্ষর *s, অক্ষর **ret_host, অক্ষর **ret_service, অক্ষর **ret_path);

প্রকারলেখ গণনা XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

পূর্ণসংখ্যা open_mkdir_at_full_label(পূর্ণসংখ্যা dirfd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা open_mkdir_at_full(পূর্ণসংখ্যা dirfd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, XOpenFlags xopen_flags, mode_t mode) {
        ফেরত open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা open_mkdir_at(পূর্ণসংখ্যা dirfd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, mode_t mode) {
        ফেরত open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা open_mkdir(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, mode_t mode) {
        ফেরত open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

পূর্ণসংখ্যা openat_report_new(পূর্ণসংখ্যা dirfd, ধ্রুবক অক্ষর *pathname, পূর্ণসংখ্যা flags, mode_t mode, bool *ret_newly_created);

পূর্ণসংখ্যা xopenat_full_label(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা xopenat_full(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ফেরত xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা xopenat(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা open_flags) {
        ফেরত xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

পূর্ণসংখ্যা xopenat_lock_full_label(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, পূর্ণসংখ্যা operation, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা xopenat_lock_full(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, পূর্ণসংখ্যা operation) {
        ফেরত xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা xopenat_lock(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা open_flags, LockType locktype, পূর্ণসংখ্যা operation) {
        ফেরত xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

পূর্ণসংখ্যা link_fd(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা newdirfd, ধ্রুবক অক্ষর *newpath);

পূর্ণসংখ্যা linkat_replace(পূর্ণসংখ্যা olddirfd, ধ্রুবক অক্ষর *oldpath, পূর্ণসংখ্যা newdirfd, ধ্রুবক অক্ষর *newpath);

স্থির অন্তঃরেখ পূর্ণসংখ্যা at_flags_normalize_nofollow(পূর্ণসংখ্যা flags) {
        যদি (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                প্রতিজ্ঞা(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } অন্যথা
                flags |= AT_SYMLINK_NOFOLLOW;
        ফেরত flags;
}

স্থির অন্তঃরেখ পূর্ণসংখ্যা at_flags_normalize_follow(পূর্ণসংখ্যা flags) {
        যদি (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                প্রতিজ্ঞা(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } অন্যথা
                flags |= AT_SYMLINK_FOLLOW;
        ফেরত flags;
}
