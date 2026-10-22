/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"
#ضمن "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ساكن مضمن mode_t PTR_منزل_منوال(فراغ *p) {
        أرجع p ? (mode_t) ((uintptr_t) p - 1) : منوال_INVALID;
}
ساكن مضمن فراغ* منوال_منزل_PTR(mode_t m) {
        أرجع m == منوال_INVALID ? عدم : (فراغ *) ((uintptr_t) m + 1);
}

صحيح rmdir_parents(ثابت حرف *رستہ, ثابت حرف *stop);

صحيح rename_noreplace(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath);

صحيح readlinkat_malloc(صحيح fd, ثابت حرف *p, حرف **ret);
ساكن مضمن صحيح readlink_malloc(ثابت حرف *p, حرف **ret) {
        أرجع readlinkat_malloc(AT_FDCWD, p, ret);
}
صحيح readlink_قدر(ثابت حرف *p, حرف **ret);
صحيح readlink_and_make_absolute(ثابت حرف *p, حرف **ret);

صحيح chmod_and_chown_at(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid);
ساكن مضمن صحيح chmod_and_chown(ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid) {
        أرجع chmod_and_chown_at(AT_FDCWD, رستہ, منوال, uid, gid);
}
صحيح fchmod_and_chown_with_fallback(صحيح fd, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid);
ساكن مضمن صحيح fchmod_and_chown(صحيح fd, mode_t منوال, uid_t uid, gid_t gid) {
        أرجع fchmod_and_chown_with_fallback(fd, عدم, منوال, uid, gid); /* no fallback */
}

صحيح fchmod_umask(صحيح fd, mode_t منوال);
صحيح fchmod_opath(صحيح fd, mode_t m);

صحيح futimens_opath(صحيح fd, ثابت بنية timespec ts[2]);

صحيح fd_warn_permissions(ثابت حرف *رستہ, صحيح fd);
صحيح stat_warn_permissions(ثابت حرف *رستہ, ثابت بنية stat *st);

صحيح access_nofollow(ثابت حرف *رستہ, صحيح منوال);
صحيح access_fd(صحيح fd, صحيح منوال);

صحيح touch_fd(صحيح fd, usec_t stamp);
صحيح touch_file(ثابت حرف *رستہ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t منوال);
صحيح touch(ثابت حرف *رستہ);

صحيح symlinkat_idempotent(ثابت حرف *نشانہ, صحيح atfd, ثابت حرف *linkpath, bool make_relative);
ساكن مضمن صحيح symlink_idempotent(ثابت حرف *نشانہ, ثابت حرف *linkpath, bool make_relative) {
        أرجع symlinkat_idempotent(نشانہ, AT_FDCWD, linkpath, make_relative);
}

تعريفنوع تعداد SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

صحيح symlinkat_atomic_بھریا_label(ثابت حرف *نشانہ, صحيح atfd, ثابت حرف *linkpath, SymlinkFlags flags, LabelContext *label_context);
ساكن مضمن صحيح symlinkat_atomic_بھریا(ثابت حرف *نشانہ, صحيح atfd, ثابت حرف *linkpath, SymlinkFlags flags) {
        أرجع symlinkat_atomic_بھریا_label(نشانہ, atfd, linkpath, flags, /* label_context= */ عدم);
}
ساكن مضمن صحيح symlink_atomic(ثابت حرف *نشانہ, ثابت حرف *linkpath) {
        أرجع symlinkat_atomic_بھریا(نشانہ, AT_FDCWD, linkpath, 0);
}

صحيح mknodat_atomic(صحيح atfd, ثابت حرف *رستہ, mode_t منوال, dev_t dev);
ساكن مضمن صحيح mknod_atomic(ثابت حرف *رستہ, mode_t منوال, dev_t dev) {
        أرجع mknodat_atomic(AT_FDCWD, رستہ, منوال, dev);
}

صحيح mkfifoat_atomic(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال);
ساكن مضمن صحيح mkfifo_atomic(ثابت حرف *رستہ, mode_t منوال) {
        أرجع mkfifoat_atomic(AT_FDCWD, رستہ, منوال);
}

صحيح get_files_وچ_directory(ثابت حرف *رستہ, حرف ***list);

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

صحيح open_ماپا_at(صحيح dir_fd, ثابت حرف *رستہ, صحيح flags, mode_t منوال);
ساكن مضمن صحيح open_ماپا(ثابت حرف *رستہ, صحيح flags, mode_t منوال) {
        أرجع open_ماپا_at(AT_FDCWD, رستہ, flags, منوال);
}

صحيح conservative_renameat(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath);
ساكن مضمن صحيح conservative_rename(ثابت حرف *oldpath, ثابت حرف *newpath) {
        أرجع conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

صحيح posix_fallocate_loop(صحيح fd, uint64_t وتھ, uint64_t ناپ);

صحيح parse_cifs_service(ثابت حرف *s, حرف **ret_host, حرف **ret_service, حرف **ret_رستہ);

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

صحيح open_mkdir_at_بھریا_label(صحيح dirfd, ثابت حرف *رستہ, صحيح flags, XOpenFlags xopen_flags, mode_t منوال, LabelContext *label_context);
ساكن مضمن صحيح open_mkdir_at_بھریا(صحيح dirfd, ثابت حرف *رستہ, صحيح flags, XOpenFlags xopen_flags, mode_t منوال) {
        أرجع open_mkdir_at_بھریا_label(dirfd, رستہ, flags, xopen_flags, منوال, /* label_context= */ عدم);
}
ساكن مضمن صحيح open_mkdir_at(صحيح dirfd, ثابت حرف *رستہ, صحيح flags, mode_t منوال) {
        أرجع open_mkdir_at_بھریا(dirfd, رستہ, flags, 0, منوال);
}
ساكن مضمن صحيح open_mkdir(ثابت حرف *رستہ, صحيح flags, mode_t منوال) {
        أرجع open_mkdir_at_بھریا(AT_FDCWD, رستہ, flags, 0, منوال);
}

صحيح openat_report_new(صحيح dirfd, ثابت حرف *pathname, صحيح flags, mode_t منوال, bool *ret_newly_created);

صحيح xopenat_بھریا_label(صحيح dir_fd, ثابت حرف *رستہ, صحيح open_flags, XOpenFlags xopen_flags, mode_t منوال, LabelContext *label_context);
ساكن مضمن صحيح xopenat_بھریا(صحيح dir_fd, ثابت حرف *رستہ, صحيح open_flags, XOpenFlags xopen_flags, mode_t منوال) {
        أرجع xopenat_بھریا_label(dir_fd, رستہ, open_flags, xopen_flags, منوال, /* label_context= */ عدم);
}
ساكن مضمن صحيح xopenat(صحيح dir_fd, ثابت حرف *رستہ, صحيح open_flags) {
        أرجع xopenat_بھریا(dir_fd, رستہ, open_flags, 0, منوال_INVALID);
}

صحيح xopenat_جندرا_بھریا_label(صحيح dir_fd, ثابت حرف *رستہ, صحيح open_flags, XOpenFlags xopen_flags, mode_t منوال, جندرا_Type locktype, صحيح operation, LabelContext *label_context);
ساكن مضمن صحيح xopenat_جندرا_بھریا(صحيح dir_fd, ثابت حرف *رستہ, صحيح open_flags, XOpenFlags xopen_flags, mode_t منوال, جندرا_Type locktype, صحيح operation) {
        أرجع xopenat_جندرا_بھریا_label(dir_fd, رستہ, open_flags, xopen_flags, منوال, locktype, operation, /* label_context= */ عدم);
}
ساكن مضمن صحيح xopenat_جندرا(صحيح dir_fd, ثابت حرف *رستہ, صحيح open_flags, جندرا_Type locktype, صحيح operation) {
        أرجع xopenat_جندرا_بھریا(dir_fd, رستہ, open_flags, 0, 0, locktype, operation);
}

صحيح link_fd(صحيح fd, صحيح newdirfd, ثابت حرف *newpath);

صحيح linkat_replace(صحيح olddirfd, ثابت حرف *oldpath, صحيح newdirfd, ثابت حرف *newpath);

ساكن مضمن صحيح at_flags_normalize_nofollow(صحيح flags) {
        إذا (FLAGS_سیٹ(flags, AT_SYMLINK_FOLLOW)) {
                أكد(!FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } وإلا
                flags |= AT_SYMLINK_NOFOLLOW;
        أرجع flags;
}

ساكن مضمن صحيح at_flags_normalize_follow(صحيح flags) {
        إذا (FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW)) {
                أكد(!FLAGS_سیٹ(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } وإلا
                flags |= AT_SYMLINK_FOLLOW;
        أرجع flags;
}
