/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
સ્થિર અંતઃરેખ mode_t PTR_TO_MODE(શૂન્ય *p) {
        પાછા p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
સ્થિર અંતઃરેખ શૂન્ય* MODE_TO_PTR(mode_t m) {
        પાછા m == MODE_INVALID ? ખાલી : (શૂન્ય *) ((uintptr_t) m + 1);
}

પૂર્ણાંક rmdir_parents(અચલ અક્ષર *path, અચલ અક્ષર *stop);

પૂર્ણાંક rename_noreplace(પૂર્ણાંક olddirfd, અચલ અક્ષર *oldpath, પૂર્ણાંક newdirfd, અચલ અક્ષર *newpath);

પૂર્ણાંક readlinkat_malloc(પૂર્ણાંક fd, અચલ અક્ષર *p, અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક readlink_malloc(અચલ અક્ષર *p, અક્ષર **ret) {
        પાછા readlinkat_malloc(AT_FDCWD, p, ret);
}
પૂર્ણાંક readlink_કિંમત(અચલ અક્ષર *p, અક્ષર **ret);
પૂર્ણાંક readlink_and_make_absolute(અચલ અક્ષર *p, અક્ષર **ret);

પૂર્ણાંક chmod_and_chown_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid);
સ્થિર અંતઃરેખ પૂર્ણાંક chmod_and_chown(અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid) {
        પાછા chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
પૂર્ણાંક fchmod_and_chown_with_fallback(પૂર્ણાંક fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid);
સ્થિર અંતઃરેખ પૂર્ણાંક fchmod_and_chown(પૂર્ણાંક fd, mode_t mode, uid_t uid, gid_t gid) {
        પાછા fchmod_and_chown_with_fallback(fd, ખાલી, mode, uid, gid); /* no fallback */
}

પૂર્ણાંક fchmod_umask(પૂર્ણાંક fd, mode_t mode);
પૂર્ણાંક fchmod_opath(પૂર્ણાંક fd, mode_t m);

પૂર્ણાંક futimens_opath(પૂર્ણાંક fd, અચલ સંરચના timespec ts[2]);

પૂર્ણાંક fd_warn_permissions(અચલ અક્ષર *path, પૂર્ણાંક fd);
પૂર્ણાંક stat_warn_permissions(અચલ અક્ષર *path, અચલ સંરચના stat *st);

પૂર્ણાંક access_nofollow(અચલ અક્ષર *path, પૂર્ણાંક mode);
પૂર્ણાંક access_fd(પૂર્ણાંક fd, પૂર્ણાંક mode);

પૂર્ણાંક touch_fd(પૂર્ણાંક fd, usec_t stamp);
પૂર્ણાંક touch_file(અચલ અક્ષર *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
પૂર્ણાંક touch(અચલ અક્ષર *path);

પૂર્ણાંક symlinkat_idempotent(અચલ અક્ષર *target, પૂર્ણાંક atfd, અચલ અક્ષર *linkpath, bool make_relative);
સ્થિર અંતઃરેખ પૂર્ણાંક symlink_idempotent(અચલ અક્ષર *target, અચલ અક્ષર *linkpath, bool make_relative) {
        પાછા symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

પ્રકારવ્યાખ્યા ગણના SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

પૂર્ણાંક symlinkat_atomic_full_label(અચલ અક્ષર *target, પૂર્ણાંક atfd, અચલ અક્ષર *linkpath, SymlinkFlags flags, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક symlinkat_atomic_full(અચલ અક્ષર *target, પૂર્ણાંક atfd, અચલ અક્ષર *linkpath, SymlinkFlags flags) {
        પાછા symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક symlink_atomic(અચલ અક્ષર *target, અચલ અક્ષર *linkpath) {
        પાછા symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

પૂર્ણાંક mknodat_atomic(પૂર્ણાંક atfd, અચલ અક્ષર *path, mode_t mode, dev_t dev);
સ્થિર અંતઃરેખ પૂર્ણાંક mknod_atomic(અચલ અક્ષર *path, mode_t mode, dev_t dev) {
        પાછા mknodat_atomic(AT_FDCWD, path, mode, dev);
}

પૂર્ણાંક mkfifoat_atomic(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode);
સ્થિર અંતઃરેખ પૂર્ણાંક mkfifo_atomic(અચલ અક્ષર *path, mode_t mode) {
        પાછા mkfifoat_atomic(AT_FDCWD, path, mode);
}

પૂર્ણાંક get_files_in_directory(અચલ અક્ષર *path, અક્ષર ***list);

પૂર્ણાંક tmp_dir(અચલ અક્ષર **ret);
પૂર્ણાંક var_tmp_dir(અચલ અક્ષર **ret);

પૂર્ણાંક unlink_or_warn(અચલ અક્ષર *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
અક્ષર *rmdir_and_free(અક્ષર *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(અક્ષર*, rmdir_and_free);

અક્ષર* unlink_and_free(અક્ષર *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(અક્ષર*, unlink_and_free);

પ્રકારવ્યાખ્યા ગણના UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

પૂર્ણાંક unlinkat_deallocate(પૂર્ણાંક fd, અચલ અક્ષર *name, UnlinkDeallocateFlags flags);

પૂર્ણાંક open_parent_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક flags, mode_t mode);
સ્થિર અંતઃરેખ પૂર્ણાંક open_parent(અચલ અક્ષર *path, પૂર્ણાંક flags, mode_t mode) {
        પાછા open_parent_at(AT_FDCWD, path, flags, mode);
}

પૂર્ણાંક conservative_renameat(પૂર્ણાંક olddirfd, અચલ અક્ષર *oldpath, પૂર્ણાંક newdirfd, અચલ અક્ષર *newpath);
સ્થિર અંતઃરેખ પૂર્ણાંક conservative_rename(અચલ અક્ષર *oldpath, અચલ અક્ષર *newpath) {
        પાછા conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

પૂર્ણાંક posix_fallocate_loop(પૂર્ણાંક fd, uint64_t offset, uint64_t size);

પૂર્ણાંક parse_cifs_service(અચલ અક્ષર *s, અક્ષર **ret_host, અક્ષર **ret_service, અક્ષર **ret_path);

પ્રકારવ્યાખ્યા ગણના XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

પૂર્ણાંક open_mkdir_at_full_label(પૂર્ણાંક dirfd, અચલ અક્ષર *path, પૂર્ણાંક flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક open_mkdir_at_full(પૂર્ણાંક dirfd, અચલ અક્ષર *path, પૂર્ણાંક flags, XOpenFlags xopen_flags, mode_t mode) {
        પાછા open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક open_mkdir_at(પૂર્ણાંક dirfd, અચલ અક્ષર *path, પૂર્ણાંક flags, mode_t mode) {
        પાછા open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
સ્થિર અંતઃરેખ પૂર્ણાંક open_mkdir(અચલ અક્ષર *path, પૂર્ણાંક flags, mode_t mode) {
        પાછા open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

પૂર્ણાંક openat_report_new(પૂર્ણાંક dirfd, અચલ અક્ષર *pathname, પૂર્ણાંક flags, mode_t mode, bool *ret_newly_created);

પૂર્ણાંક xopenat_full_label(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક xopenat_full(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક open_flags, XOpenFlags xopen_flags, mode_t mode) {
        પાછા xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક xopenat(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક open_flags) {
        પાછા xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

પૂર્ણાંક xopenat_lock_full_label(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, પૂર્ણાંક operation, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક xopenat_lock_full(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, પૂર્ણાંક operation) {
        પાછા xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક xopenat_lock(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક open_flags, LockType locktype, પૂર્ણાંક operation) {
        પાછા xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

પૂર્ણાંક link_fd(પૂર્ણાંક fd, પૂર્ણાંક newdirfd, અચલ અક્ષર *newpath);

પૂર્ણાંક linkat_replace(પૂર્ણાંક olddirfd, અચલ અક્ષર *oldpath, પૂર્ણાંક newdirfd, અચલ અક્ષર *newpath);

સ્થિર અંતઃરેખ પૂર્ણાંક at_flags_normalize_nofollow(પૂર્ણાંક flags) {
        જો (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                પ્રતિજ્ઞા(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } નહીંતર
                flags |= AT_SYMLINK_NOFOLLOW;
        પાછા flags;
}

સ્થિર અંતઃરેખ પૂર્ણાંક at_flags_normalize_follow(પૂર્ણાંક flags) {
        જો (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                પ્રતિજ્ઞા(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } નહીંતર
                flags |= AT_SYMLINK_FOLLOW;
        પાછા flags;
}
