/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"
#ଅନ୍ତର୍ଭାଵ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ mode_t PTR_TO_MODE(ଶୂନ୍ୟ *p) {
        ଫେରନ୍ତୁ p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* MODE_TO_PTR(mode_t m) {
        ଫେରନ୍ତୁ m == MODE_INVALID ? ଖାଲି : (ଶୂନ୍ୟ *) ((uintptr_t) m + 1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rmdir_parents(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *stop);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା rename_noreplace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା olddirfd, ଅଚଳ ଅକ୍ଷର *oldpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlinkat_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlink_malloc(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ readlinkat_malloc(AT_FDCWD, p, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlink_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା readlink_and_make_absolute(ଅଚଳ ଅକ୍ଷର *p, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chmod_and_chown_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା chmod_and_chown(ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid) {
        ଫେରନ୍ତୁ chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_and_chown_with_fallback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_and_chown(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, mode_t mode, uid_t uid, gid_t gid) {
        ଫେରନ୍ତୁ fchmod_and_chown_with_fallback(fd, ଖାଲି, mode, uid, gid); /* no fallback */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_umask(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, mode_t mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fchmod_opath(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, mode_t m);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା futimens_opath(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ସଂରଚନା timespec ts[2]);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_warn_permissions(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_warn_permissions(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ସଂରଚନା stat *st);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_nofollow(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା access_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା touch_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, usec_t stamp);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା touch_file(ଅଚଳ ଅକ୍ଷର *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା touch(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlinkat_idempotent(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *linkpath, bool make_relative);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlink_idempotent(ଅଚଳ ଅକ୍ଷର *target, ଅଚଳ ଅକ୍ଷର *linkpath, bool make_relative) {
        ଫେରନ୍ତୁ symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlinkat_atomic_full_label(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *linkpath, SymlinkFlags flags, LabelContext *label_context);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlinkat_atomic_full(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *linkpath, SymlinkFlags flags) {
        ଫେରନ୍ତୁ symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା symlink_atomic(ଅଚଳ ଅକ୍ଷର *target, ଅଚଳ ଅକ୍ଷର *linkpath) {
        ଫେରନ୍ତୁ symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mknodat_atomic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, dev_t dev);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mknod_atomic(ଅଚଳ ଅକ୍ଷର *path, mode_t mode, dev_t dev) {
        ଫେରନ୍ତୁ mknodat_atomic(AT_FDCWD, path, mode, dev);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkfifoat_atomic(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkfifo_atomic(ଅଚଳ ଅକ୍ଷର *path, mode_t mode) {
        ଫେରନ୍ତୁ mkfifoat_atomic(AT_FDCWD, path, mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_files_in_directory(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର ***list);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp_dir(ଅଚଳ ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_tmp_dir(ଅଚଳ ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlink_or_warn(ଅଚଳ ଅକ୍ଷର *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
ଅକ୍ଷର *rmdir_and_free(ଅକ୍ଷର *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ଅକ୍ଷର*, rmdir_and_free);

ଅକ୍ଷର* unlink_and_free(ଅକ୍ଷର *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ଅକ୍ଷର*, unlink_and_free);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlinkat_deallocate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *name, UnlinkDeallocateFlags flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_parent(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode) {
        ଫେରନ୍ତୁ open_parent_at(AT_FDCWD, path, flags, mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conservative_renameat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା olddirfd, ଅଚଳ ଅକ୍ଷର *oldpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା conservative_rename(ଅଚଳ ଅକ୍ଷର *oldpath, ଅଚଳ ଅକ୍ଷର *newpath) {
        ଫେରନ୍ତୁ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_fallocate_loop(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t offset, uint64_t size);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_cifs_service(ଅଚଳ ଅକ୍ଷର *s, ଅକ୍ଷର **ret_host, ଅକ୍ଷର **ret_service, ଅକ୍ଷର **ret_path);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_mkdir_at_full_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_mkdir_at_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, XOpenFlags xopen_flags, mode_t mode) {
        ଫେରନ୍ତୁ open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_mkdir_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode) {
        ଫେରନ୍ତୁ open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_mkdir(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode) {
        ଫେରନ୍ତୁ open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା openat_report_new(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *pathname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, mode_t mode, bool *ret_newly_created);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_full_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, XOpenFlags xopen_flags, mode_t mode) {
        ଫେରନ୍ତୁ xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags) {
        ଫେରନ୍ତୁ xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_lock_full_label(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation, LabelContext *label_context);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_lock_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation) {
        ଫେରନ୍ତୁ xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ ଖାଲି);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା xopenat_lock(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_flags, LockType locktype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା operation) {
        ଫେରନ୍ତୁ xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା link_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା linkat_replace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା olddirfd, ଅଚଳ ଅକ୍ଷର *oldpath, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newdirfd, ଅଚଳ ଅକ୍ଷର *newpath);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags_normalize_nofollow(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ଯଦି (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ଅନ୍ୟଥା
                flags |= AT_SYMLINK_NOFOLLOW;
        ଫେରନ୍ତୁ flags;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags_normalize_follow(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ଯଦି (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ଅନ୍ୟଥା
                flags |= AT_SYMLINK_FOLLOW;
        ଫେରନ୍ତୁ flags;
}
