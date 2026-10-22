/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"
#அந்தர்பாவ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
நிலையான உள்வரி mode_t PTR_TO_MODE(வெறுமை *p) {
        திரும்பு p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
நிலையான உள்வரி வெறுமை* MODE_TO_PTR(mode_t m) {
        திரும்பு m == MODE_INVALID ? வெற்று : (வெறுமை *) ((uintptr_t) m + 1);
}

முழு rmdir_parents(மாறிலி எழுத்து *path, மாறிலி எழுத்து *stop);

முழு rename_noreplace(முழு olddirfd, மாறிலி எழுத்து *oldpath, முழு newdirfd, மாறிலி எழுத்து *newpath);

முழு readlinkat_malloc(முழு fd, மாறிலி எழுத்து *p, எழுத்து **ret);
நிலையான உள்வரி முழு readlink_malloc(மாறிலி எழுத்து *p, எழுத்து **ret) {
        திரும்பு readlinkat_malloc(AT_FDCWD, p, ret);
}
முழு readlink_மதிப்பு(மாறிலி எழுத்து *p, எழுத்து **ret);
முழு readlink_and_make_absolute(மாறிலி எழுத்து *p, எழுத்து **ret);

முழு chmod_and_chown_at(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid);
நிலையான உள்வரி முழு chmod_and_chown(மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid) {
        திரும்பு chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
முழு fchmod_and_chown_with_fallback(முழு fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid);
நிலையான உள்வரி முழு fchmod_and_chown(முழு fd, mode_t mode, uid_t uid, gid_t gid) {
        திரும்பு fchmod_and_chown_with_fallback(fd, வெற்று, mode, uid, gid); /* no fallback */
}

முழு fchmod_umask(முழு fd, mode_t mode);
முழு fchmod_opath(முழு fd, mode_t m);

முழு futimens_opath(முழு fd, மாறிலி கட்டமைப்பு timespec ts[2]);

முழு fd_warn_permissions(மாறிலி எழுத்து *path, முழு fd);
முழு stat_warn_permissions(மாறிலி எழுத்து *path, மாறிலி கட்டமைப்பு stat *st);

முழு access_nofollow(மாறிலி எழுத்து *path, முழு mode);
முழு access_fd(முழு fd, முழு mode);

முழு touch_fd(முழு fd, usec_t stamp);
முழு touch_file(மாறிலி எழுத்து *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
முழு touch(மாறிலி எழுத்து *path);

முழு symlinkat_idempotent(மாறிலி எழுத்து *target, முழு atfd, மாறிலி எழுத்து *linkpath, bool make_relative);
நிலையான உள்வரி முழு symlink_idempotent(மாறிலி எழுத்து *target, மாறிலி எழுத்து *linkpath, bool make_relative) {
        திரும்பு symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

வகைவரை எண்ணிக்கை SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

முழு symlinkat_atomic_full_label(மாறிலி எழுத்து *target, முழு atfd, மாறிலி எழுத்து *linkpath, SymlinkFlags flags, LabelContext *label_context);
நிலையான உள்வரி முழு symlinkat_atomic_full(மாறிலி எழுத்து *target, முழு atfd, மாறிலி எழுத்து *linkpath, SymlinkFlags flags) {
        திரும்பு symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ வெற்று);
}
நிலையான உள்வரி முழு symlink_atomic(மாறிலி எழுத்து *target, மாறிலி எழுத்து *linkpath) {
        திரும்பு symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

முழு mknodat_atomic(முழு atfd, மாறிலி எழுத்து *path, mode_t mode, dev_t dev);
நிலையான உள்வரி முழு mknod_atomic(மாறிலி எழுத்து *path, mode_t mode, dev_t dev) {
        திரும்பு mknodat_atomic(AT_FDCWD, path, mode, dev);
}

முழு mkfifoat_atomic(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode);
நிலையான உள்வரி முழு mkfifo_atomic(மாறிலி எழுத்து *path, mode_t mode) {
        திரும்பு mkfifoat_atomic(AT_FDCWD, path, mode);
}

முழு get_files_in_directory(மாறிலி எழுத்து *path, எழுத்து ***list);

முழு tmp_dir(மாறிலி எழுத்து **ret);
முழு var_tmp_dir(மாறிலி எழுத்து **ret);

முழு unlink_or_warn(மாறிலி எழுத்து *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
எழுத்து *rmdir_and_free(எழுத்து *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(எழுத்து*, rmdir_and_free);

எழுத்து* unlink_and_free(எழுத்து *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(எழுத்து*, unlink_and_free);

வகைவரை எண்ணிக்கை UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

முழு unlinkat_deallocate(முழு fd, மாறிலி எழுத்து *name, UnlinkDeallocateFlags flags);

முழு open_parent_at(முழு dir_fd, மாறிலி எழுத்து *path, முழு flags, mode_t mode);
நிலையான உள்வரி முழு open_parent(மாறிலி எழுத்து *path, முழு flags, mode_t mode) {
        திரும்பு open_parent_at(AT_FDCWD, path, flags, mode);
}

முழு conservative_renameat(முழு olddirfd, மாறிலி எழுத்து *oldpath, முழு newdirfd, மாறிலி எழுத்து *newpath);
நிலையான உள்வரி முழு conservative_rename(மாறிலி எழுத்து *oldpath, மாறிலி எழுத்து *newpath) {
        திரும்பு conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

முழு posix_fallocate_loop(முழு fd, uint64_t offset, uint64_t size);

முழு parse_cifs_service(மாறிலி எழுத்து *s, எழுத்து **ret_host, எழுத்து **ret_service, எழுத்து **ret_path);

வகைவரை எண்ணிக்கை XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

முழு open_mkdir_at_full_label(முழு dirfd, மாறிலி எழுத்து *path, முழு flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
நிலையான உள்வரி முழு open_mkdir_at_full(முழு dirfd, மாறிலி எழுத்து *path, முழு flags, XOpenFlags xopen_flags, mode_t mode) {
        திரும்பு open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ வெற்று);
}
நிலையான உள்வரி முழு open_mkdir_at(முழு dirfd, மாறிலி எழுத்து *path, முழு flags, mode_t mode) {
        திரும்பு open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
நிலையான உள்வரி முழு open_mkdir(மாறிலி எழுத்து *path, முழு flags, mode_t mode) {
        திரும்பு open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

முழு openat_report_new(முழு dirfd, மாறிலி எழுத்து *pathname, முழு flags, mode_t mode, bool *ret_newly_created);

முழு xopenat_full_label(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
நிலையான உள்வரி முழு xopenat_full(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, XOpenFlags xopen_flags, mode_t mode) {
        திரும்பு xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ வெற்று);
}
நிலையான உள்வரி முழு xopenat(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags) {
        திரும்பு xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

முழு xopenat_lock_full_label(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, முழு operation, LabelContext *label_context);
நிலையான உள்வரி முழு xopenat_lock_full(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, முழு operation) {
        திரும்பு xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ வெற்று);
}
நிலையான உள்வரி முழு xopenat_lock(முழு dir_fd, மாறிலி எழுத்து *path, முழு open_flags, LockType locktype, முழு operation) {
        திரும்பு xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

முழு link_fd(முழு fd, முழு newdirfd, மாறிலி எழுத்து *newpath);

முழு linkat_replace(முழு olddirfd, மாறிலி எழுத்து *oldpath, முழு newdirfd, மாறிலி எழுத்து *newpath);

நிலையான உள்வரி முழு at_flags_normalize_nofollow(முழு flags) {
        எனில் (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                ப்ரதிஜ்ஞா(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } இல்லையெனில்
                flags |= AT_SYMLINK_NOFOLLOW;
        திரும்பு flags;
}

நிலையான உள்வரி முழு at_flags_normalize_follow(முழு flags) {
        எனில் (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                ப்ரதிஜ்ஞா(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } இல்லையெனில்
                flags |= AT_SYMLINK_FOLLOW;
        திரும்பு flags;
}
