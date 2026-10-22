/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"
#شامل "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ایستا درونخطی mode_t PTR_TO_MODE(تهی *p) {
        بازگشت p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
ایستا درونخطی تهی* MODE_TO_PTR(mode_t m) {
        بازگشت m == MODE_INVALID ? پوچ : (تهی *) ((uintptr_t) m + 1);
}

صحیح rmdir_parents(ثابت نویسه *path, ثابت نویسه *stop);

صحیح rename_noreplace(صحیح olddirfd, ثابت نویسه *oldpath, صحیح newdirfd, ثابت نویسه *newpath);

صحیح readlinkat_malloc(صحیح fd, ثابت نویسه *p, نویسه **ret);
ایستا درونخطی صحیح readlink_malloc(ثابت نویسه *p, نویسه **ret) {
        بازگشت readlinkat_malloc(AT_FDCWD, p, ret);
}
صحیح readlink_مقدار(ثابت نویسه *p, نویسه **ret);
صحیح readlink_and_make_absolute(ثابت نویسه *p, نویسه **ret);

صحیح chmod_and_chown_at(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid);
ایستا درونخطی صحیح chmod_and_chown(ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid) {
        بازگشت chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
صحیح fchmod_and_chown_with_fallback(صحیح fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid);
ایستا درونخطی صحیح fchmod_and_chown(صحیح fd, mode_t mode, uid_t uid, gid_t gid) {
        بازگشت fchmod_and_chown_with_fallback(fd, پوچ, mode, uid, gid); /* no fallback */
}

صحیح fchmod_umask(صحیح fd, mode_t mode);
صحیح fchmod_opath(صحیح fd, mode_t m);

صحیح futimens_opath(صحیح fd, ثابت ساختار timespec ts[2]);

صحیح fd_warn_permissions(ثابت نویسه *path, صحیح fd);
صحیح stat_warn_permissions(ثابت نویسه *path, ثابت ساختار stat *st);

صحیح access_nofollow(ثابت نویسه *path, صحیح mode);
صحیح access_fd(صحیح fd, صحیح mode);

صحیح touch_fd(صحیح fd, usec_t stamp);
صحیح touch_file(ثابت نویسه *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
صحیح touch(ثابت نویسه *path);

صحیح symlinkat_idempotent(ثابت نویسه *target, صحیح atfd, ثابت نویسه *linkpath, bool make_relative);
ایستا درونخطی صحیح symlink_idempotent(ثابت نویسه *target, ثابت نویسه *linkpath, bool make_relative) {
        بازگشت symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

تعریفنوع شمارش SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

صحیح symlinkat_atomic_full_label(ثابت نویسه *target, صحیح atfd, ثابت نویسه *linkpath, SymlinkFlags flags, LabelContext *label_context);
ایستا درونخطی صحیح symlinkat_atomic_full(ثابت نویسه *target, صحیح atfd, ثابت نویسه *linkpath, SymlinkFlags flags) {
        بازگشت symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ پوچ);
}
ایستا درونخطی صحیح symlink_atomic(ثابت نویسه *target, ثابت نویسه *linkpath) {
        بازگشت symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

صحیح mknodat_atomic(صحیح atfd, ثابت نویسه *path, mode_t mode, dev_t dev);
ایستا درونخطی صحیح mknod_atomic(ثابت نویسه *path, mode_t mode, dev_t dev) {
        بازگشت mknodat_atomic(AT_FDCWD, path, mode, dev);
}

صحیح mkfifoat_atomic(صحیح dir_fd, ثابت نویسه *path, mode_t mode);
ایستا درونخطی صحیح mkfifo_atomic(ثابت نویسه *path, mode_t mode) {
        بازگشت mkfifoat_atomic(AT_FDCWD, path, mode);
}

صحیح get_files_in_directory(ثابت نویسه *path, نویسه ***list);

صحیح tmp_dir(ثابت نویسه **ret);
صحیح var_tmp_dir(ثابت نویسه **ret);

صحیح unlink_or_warn(ثابت نویسه *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
نویسه *rmdir_and_free(نویسه *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(نویسه*, rmdir_and_free);

نویسه* unlink_and_free(نویسه *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(نویسه*, unlink_and_free);

تعریفنوع شمارش UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

صحیح unlinkat_deallocate(صحیح fd, ثابت نویسه *name, UnlinkDeallocateFlags flags);

صحیح open_parent_at(صحیح dir_fd, ثابت نویسه *path, صحیح flags, mode_t mode);
ایستا درونخطی صحیح open_parent(ثابت نویسه *path, صحیح flags, mode_t mode) {
        بازگشت open_parent_at(AT_FDCWD, path, flags, mode);
}

صحیح conservative_renameat(صحیح olddirfd, ثابت نویسه *oldpath, صحیح newdirfd, ثابت نویسه *newpath);
ایستا درونخطی صحیح conservative_rename(ثابت نویسه *oldpath, ثابت نویسه *newpath) {
        بازگشت conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

صحیح posix_fallocate_loop(صحیح fd, uint64_t offset, uint64_t size);

صحیح parse_cifs_service(ثابت نویسه *s, نویسه **ret_host, نویسه **ret_service, نویسه **ret_path);

تعریفنوع شمارش XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

صحیح open_mkdir_at_full_label(صحیح dirfd, ثابت نویسه *path, صحیح flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ایستا درونخطی صحیح open_mkdir_at_full(صحیح dirfd, ثابت نویسه *path, صحیح flags, XOpenFlags xopen_flags, mode_t mode) {
        بازگشت open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ پوچ);
}
ایستا درونخطی صحیح open_mkdir_at(صحیح dirfd, ثابت نویسه *path, صحیح flags, mode_t mode) {
        بازگشت open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
ایستا درونخطی صحیح open_mkdir(ثابت نویسه *path, صحیح flags, mode_t mode) {
        بازگشت open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

صحیح openat_report_new(صحیح dirfd, ثابت نویسه *pathname, صحیح flags, mode_t mode, bool *ret_newly_created);

صحیح xopenat_full_label(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ایستا درونخطی صحیح xopenat_full(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, XOpenFlags xopen_flags, mode_t mode) {
        بازگشت xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ پوچ);
}
ایستا درونخطی صحیح xopenat(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags) {
        بازگشت xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

صحیح xopenat_lock_full_label(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, صحیح operation, LabelContext *label_context);
ایستا درونخطی صحیح xopenat_lock_full(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, صحیح operation) {
        بازگشت xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ پوچ);
}
ایستا درونخطی صحیح xopenat_lock(صحیح dir_fd, ثابت نویسه *path, صحیح open_flags, LockType locktype, صحیح operation) {
        بازگشت xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

صحیح link_fd(صحیح fd, صحیح newdirfd, ثابت نویسه *newpath);

صحیح linkat_replace(صحیح olddirfd, ثابت نویسه *oldpath, صحیح newdirfd, ثابت نویسه *newpath);

ایستا درونخطی صحیح at_flags_normalize_nofollow(صحیح flags) {
        اگر (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                أكد(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } وگرنه
                flags |= AT_SYMLINK_NOFOLLOW;
        بازگشت flags;
}

ایستا درونخطی صحیح at_flags_normalize_follow(صحیح flags) {
        اگر (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                أكد(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } وگرنه
                flags |= AT_SYMLINK_FOLLOW;
        بازگشت flags;
}
