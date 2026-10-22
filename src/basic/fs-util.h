/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"
#شامل "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ساکن اندرونی mode_t PTR_مقصد_منوال(خالی *p) {
        واپس p ? (mode_t) ((uintptr_t) p - 1) : منوال_INVALID;
}
ساکن اندرونی خالی* منوال_مقصد_PTR(mode_t m) {
        واپس m == منوال_INVALID ? تہی : (خالی *) ((uintptr_t) m + 1);
}

صحیح rmdir_parents(ثابت حرف *راستہ, ثابت حرف *stop);

صحیح rename_noreplace(صحیح olddirfd, ثابت حرف *oldpath, صحیح newdirfd, ثابت حرف *newpath);

صحیح readlinkat_malloc(صحیح fd, ثابت حرف *p, حرف **ret);
ساکن اندرونی صحیح readlink_malloc(ثابت حرف *p, حرف **ret) {
        واپس readlinkat_malloc(AT_FDCWD, p, ret);
}
صحیح readlink_قدر(ثابت حرف *p, حرف **ret);
صحیح readlink_and_make_absolute(ثابت حرف *p, حرف **ret);

صحیح chmod_and_chown_at(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid);
ساکن اندرونی صحیح chmod_and_chown(ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid) {
        واپس chmod_and_chown_at(AT_FDCWD, راستہ, منوال, uid, gid);
}
صحیح fchmod_and_chown_with_fallback(صحیح fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid);
ساکن اندرونی صحیح fchmod_and_chown(صحیح fd, mode_t منوال, uid_t uid, gid_t gid) {
        واپس fchmod_and_chown_with_fallback(fd, تہی, منوال, uid, gid); /* no fallback */
}

صحیح fchmod_umask(صحیح fd, mode_t منوال);
صحیح fchmod_opath(صحیح fd, mode_t m);

صحیح futimens_opath(صحیح fd, ثابت ساخت timespec ts[2]);

صحیح fd_warn_permissions(ثابت حرف *راستہ, صحیح fd);
صحیح stat_warn_permissions(ثابت حرف *راستہ, ثابت ساخت stat *st);

صحیح access_nofollow(ثابت حرف *راستہ, صحیح منوال);
صحیح access_fd(صحیح fd, صحیح منوال);

صحیح touch_fd(صحیح fd, usec_t stamp);
صحیح touch_file(ثابت حرف *راستہ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t منوال);
صحیح touch(ثابت حرف *راستہ);

صحیح symlinkat_idempotent(ثابت حرف *ہدف, صحیح atfd, ثابت حرف *linkpath, bool make_relative);
ساکن اندرونی صحیح symlink_idempotent(ثابت حرف *ہدف, ثابت حرف *linkpath, bool make_relative) {
        واپس symlinkat_idempotent(ہدف, AT_FDCWD, linkpath, make_relative);
}

قسمتعریف شمار SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

صحیح symlinkat_atomic_بھرا_label(ثابت حرف *ہدف, صحیح atfd, ثابت حرف *linkpath, SymlinkFlags flags, LabelContext *label_context);
ساکن اندرونی صحیح symlinkat_atomic_بھرا(ثابت حرف *ہدف, صحیح atfd, ثابت حرف *linkpath, SymlinkFlags flags) {
        واپس symlinkat_atomic_بھرا_label(ہدف, atfd, linkpath, flags, /* label_context= */ تہی);
}
ساکن اندرونی صحیح symlink_atomic(ثابت حرف *ہدف, ثابت حرف *linkpath) {
        واپس symlinkat_atomic_بھرا(ہدف, AT_FDCWD, linkpath, 0);
}

صحیح mknodat_atomic(صحیح atfd, ثابت حرف *راستہ, mode_t منوال, dev_t dev);
ساکن اندرونی صحیح mknod_atomic(ثابت حرف *راستہ, mode_t منوال, dev_t dev) {
        واپس mknodat_atomic(AT_FDCWD, راستہ, منوال, dev);
}

صحیح mkfifoat_atomic(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال);
ساکن اندرونی صحیح mkfifo_atomic(ثابت حرف *راستہ, mode_t منوال) {
        واپس mkfifoat_atomic(AT_FDCWD, راستہ, منوال);
}

صحیح get_files_اندر_directory(ثابت حرف *راستہ, حرف ***list);

صحیح tmp_dir(ثابت حرف **ret);
صحیح var_tmp_dir(ثابت حرف **ret);

صحیح unlink_or_warn(ثابت حرف *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
حرف *rmdir_and_free(حرف *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف*, rmdir_and_free);

حرف* unlink_and_free(حرف *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(حرف*, unlink_and_free);

قسمتعریف شمار UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

صحیح unlinkat_deallocate(صحیح fd, ثابت حرف *name, UnlinkDeallocateFlags flags);

صحیح open_والد_at(صحیح dir_fd, ثابت حرف *راستہ, صحیح flags, mode_t منوال);
ساکن اندرونی صحیح open_والد(ثابت حرف *راستہ, صحیح flags, mode_t منوال) {
        واپس open_والد_at(AT_FDCWD, راستہ, flags, منوال);
}

صحیح conservative_renameat(صحیح olddirfd, ثابت حرف *oldpath, صحیح newdirfd, ثابت حرف *newpath);
ساکن اندرونی صحیح conservative_rename(ثابت حرف *oldpath, ثابت حرف *newpath) {
        واپس conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

صحیح posix_fallocate_loop(صحیح fd, uint64_t انحراف, uint64_t سائز);

صحیح parse_cifs_service(ثابت حرف *s, حرف **ret_host, حرف **ret_service, حرف **ret_راستہ);

قسمتعریف شمار XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

صحیح open_mkdir_at_بھرا_label(صحیح dirfd, ثابت حرف *راستہ, صحیح flags, XOpenFlags xopen_flags, mode_t منوال, LabelContext *label_context);
ساکن اندرونی صحیح open_mkdir_at_بھرا(صحیح dirfd, ثابت حرف *راستہ, صحیح flags, XOpenFlags xopen_flags, mode_t منوال) {
        واپس open_mkdir_at_بھرا_label(dirfd, راستہ, flags, xopen_flags, منوال, /* label_context= */ تہی);
}
ساکن اندرونی صحیح open_mkdir_at(صحیح dirfd, ثابت حرف *راستہ, صحیح flags, mode_t منوال) {
        واپس open_mkdir_at_بھرا(dirfd, راستہ, flags, 0, منوال);
}
ساکن اندرونی صحیح open_mkdir(ثابت حرف *راستہ, صحیح flags, mode_t منوال) {
        واپس open_mkdir_at_بھرا(AT_FDCWD, راستہ, flags, 0, منوال);
}

صحیح openat_report_new(صحیح dirfd, ثابت حرف *pathname, صحیح flags, mode_t منوال, bool *ret_newly_created);

صحیح xopenat_بھرا_label(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, XOpenFlags xopen_flags, mode_t منوال, LabelContext *label_context);
ساکن اندرونی صحیح xopenat_بھرا(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, XOpenFlags xopen_flags, mode_t منوال) {
        واپس xopenat_بھرا_label(dir_fd, راستہ, open_flags, xopen_flags, منوال, /* label_context= */ تہی);
}
ساکن اندرونی صحیح xopenat(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags) {
        واپس xopenat_بھرا(dir_fd, راستہ, open_flags, 0, منوال_INVALID);
}

صحیح xopenat_تالا_بھرا_label(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, XOpenFlags xopen_flags, mode_t منوال, تالا_Type locktype, صحیح operation, LabelContext *label_context);
ساکن اندرونی صحیح xopenat_تالا_بھرا(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, XOpenFlags xopen_flags, mode_t منوال, تالا_Type locktype, صحیح operation) {
        واپس xopenat_تالا_بھرا_label(dir_fd, راستہ, open_flags, xopen_flags, منوال, locktype, operation, /* label_context= */ تہی);
}
ساکن اندرونی صحیح xopenat_تالا(صحیح dir_fd, ثابت حرف *راستہ, صحیح open_flags, تالا_Type locktype, صحیح operation) {
        واپس xopenat_تالا_بھرا(dir_fd, راستہ, open_flags, 0, 0, locktype, operation);
}

صحیح link_fd(صحیح fd, صحیح newdirfd, ثابت حرف *newpath);

صحیح linkat_replace(صحیح olddirfd, ثابت حرف *oldpath, صحیح newdirfd, ثابت حرف *newpath);

ساکن اندرونی صحیح at_flags_normalize_nofollow(صحیح flags) {
        اگر (FLAGS_سیٹ(flags, AT_SYMLINK_FOLLOW)) {
                أكد(!FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ورنہ
                flags |= AT_SYMLINK_NOFOLLOW;
        واپس flags;
}

ساکن اندرونی صحیح at_flags_normalize_follow(صحیح flags) {
        اگر (FLAGS_سیٹ(flags, AT_SYMLINK_NOFOLLOW)) {
                أكد(!FLAGS_سیٹ(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ورنہ
                flags |= AT_SYMLINK_FOLLOW;
        واپس flags;
}
