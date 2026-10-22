/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"
#ضمن "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ساكن مضمن mode_t PTR_TO_MODE(فراغ *p) {
        إرجاع p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
ساكن مضمن فراغ* MODE_TO_PTR(mode_t m) {
        إرجاع m == MODE_INVALID ? عدم : (فراغ *) ((uintptr_t) m + 1);
}

صحيح rmdir_parents(ثابت حرف *path, ثابت حرف *stop);

صحيح rename_noreplace(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath);

صحيح readlinkat_malloc(صحيح fd, ثابت حرف *p, حرف **ret);
ساكن مضمن صحيح readlink_malloc(ثابت حرف *p, حرف **ret) {
        إرجاع readlinkat_malloc(AT_FDCWD, p, ret);
}
صحيح readlink_قيمة(ثابت حرف *p, حرف **ret);
صحيح readlink_and_make_absolute(ثابت حرف *p, حرف **ret);

صحيح chmod_and_chown_at(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid);
ساكن مضمن صحيح chmod_and_chown(ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid) {
        إرجاع chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
صحيح fchmod_and_chown_with_fallback(صحيح fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid);
ساكن مضمن صحيح fchmod_and_chown(صحيح fd, mode_t mode, uid_t uid, gid_t gid) {
        إرجاع fchmod_and_chown_with_fallback(fd, عدم, mode, uid, gid); /* no fallback */
}

صحيح fchmod_umask(صحيح fd, mode_t mode);
صحيح fchmod_opath(صحيح fd, mode_t m);

صحيح futimens_opath(صحيح fd, ثابت بنية timespec ts[2]);

صحيح fd_warn_permissions(ثابت حرف *path, صحيح fd);
صحيح stat_warn_permissions(ثابت حرف *path, ثابت بنية stat *st);

صحيح access_nofollow(ثابت حرف *path, صحيح mode);
صحيح access_fd(صحيح fd, صحيح mode);

صحيح touch_fd(صحيح fd, usec_t stamp);
صحيح touch_file(ثابت حرف *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
صحيح touch(ثابت حرف *path);

صحيح symlinkat_idempotent(ثابت حرف *target, صحيح atfd, ثابت حرف *linkpath, bool make_relative);
ساكن مضمن صحيح symlink_idempotent(ثابت حرف *target, ثابت حرف *linkpath, bool make_relative) {
        إرجاع symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

تعريفنوع تعداد SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

صحيح symlinkat_atomic_full_label(ثابت حرف *target, صحيح atfd, ثابت حرف *linkpath, SymlinkFlags flags, LabelContext *label_context);
ساكن مضمن صحيح symlinkat_atomic_full(ثابت حرف *target, صحيح atfd, ثابت حرف *linkpath, SymlinkFlags flags) {
        إرجاع symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ عدم);
}
ساكن مضمن صحيح symlink_atomic(ثابت حرف *target, ثابت حرف *linkpath) {
        إرجاع symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

صحيح mknodat_atomic(صحيح atfd, ثابت حرف *path, mode_t mode, dev_t dev);
ساكن مضمن صحيح mknod_atomic(ثابت حرف *path, mode_t mode, dev_t dev) {
        إرجاع mknodat_atomic(AT_FDCWD, path, mode, dev);
}

صحيح mkfifoat_atomic(صحيح dir_fd, ثابت حرف *path, mode_t mode);
ساكن مضمن صحيح mkfifo_atomic(ثابت حرف *path, mode_t mode) {
        إرجاع mkfifoat_atomic(AT_FDCWD, path, mode);
}

صحيح get_files_in_directory(ثابت حرف *path, حرف ***list);

صحيح tmp_dir(ثابت حرف **ret);
صحيح var_tmp_dir(ثابت حرف **ret);

صحيح unlink_or_warn(ثابت حرف *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
حرف *rmdir_and_free(حرف *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف*, rmdir_and_free);

حرف* unlink_and_free(حرف *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف*, unlink_and_free);

تعريفنوع تعداد UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

صحيح unlinkat_deallocate(صحيح fd, ثابت حرف *name, UnlinkDeallocateFlags flags);

صحيح open_parent_at(صحيح dir_fd, ثابت حرف *path, صحيح flags, mode_t mode);
ساكن مضمن صحيح open_parent(ثابت حرف *path, صحيح flags, mode_t mode) {
        إرجاع open_parent_at(AT_FDCWD, path, flags, mode);
}

صحيح conservative_renameat(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath);
ساكن مضمن صحيح conservative_rename(ثابت حرف *oldpath, ثابت حرف *newpath) {
        إرجاع conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

صحيح posix_fallocate_loop(صحيح fd, uint64_t offset, uint64_t size);

صحيح parse_cifs_service(ثابت حرف *s, حرف **ret_host, حرف **ret_service, حرف **ret_path);

تعريفنوع تعداد XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

صحيح open_mkdir_at_full_label(صحيح dirfd, ثابت حرف *path, صحيح flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ساكن مضمن صحيح open_mkdir_at_full(صحيح dirfd, ثابت حرف *path, صحيح flags, XOpenFlags xopen_flags, mode_t mode) {
        إرجاع open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ عدم);
}
ساكن مضمن صحيح open_mkdir_at(صحيح dirfd, ثابت حرف *path, صحيح flags, mode_t mode) {
        إرجاع open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
ساكن مضمن صحيح open_mkdir(ثابت حرف *path, صحيح flags, mode_t mode) {
        إرجاع open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

صحيح openat_report_new(صحيح dirfd, ثابت حرف *pathname, صحيح flags, mode_t mode, bool *ret_newly_created);

صحيح xopenat_full_label(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ساكن مضمن صحيح xopenat_full(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, XOpenFlags xopen_flags, mode_t mode) {
        إرجاع xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ عدم);
}
ساكن مضمن صحيح xopenat(صحيح dir_fd, ثابت حرف *path, صحيح open_flags) {
        إرجاع xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

صحيح xopenat_lock_full_label(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, صحيح operation, LabelContext *label_context);
ساكن مضمن صحيح xopenat_lock_full(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, صحيح operation) {
        إرجاع xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ عدم);
}
ساكن مضمن صحيح xopenat_lock(صحيح dir_fd, ثابت حرف *path, صحيح open_flags, LockType locktype, صحيح operation) {
        إرجاع xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

صحيح link_fd(صحيح fd, صحيح newdirfd, ثابت حرف *newpath);

صحيح linkat_replace(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath);

ساكن مضمن صحيح at_flags_normalize_nofollow(صحيح flags) {
        إذا (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                أكد(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } وإلا
                flags |= AT_SYMLINK_NOFOLLOW;
        إرجاع flags;
}

ساكن مضمن صحيح at_flags_normalize_follow(صحيح flags) {
        إذا (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                أكد(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } وإلا
                flags |= AT_SYMLINK_FOLLOW;
        إرجاع flags;
}
