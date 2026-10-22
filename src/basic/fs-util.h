/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ mode_t PTR_ಗಮ್ಯ_ಬಹುಲಕ(ಶೂನ್ಯ *p) {
        ಹಿಂತಿರುಗಿ p ? (mode_t) ((uintptr_t) p - 1) : ಬಹುಲಕ_INVALID;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ಬಹುಲಕ_ಗಮ್ಯ_PTR(mode_t m) {
        ಹಿಂತಿರುಗಿ m == ಬಹುಲಕ_INVALID ? ಖಾಲಿ : (ಶೂನ್ಯ *) ((uintptr_t) m + 1);
}

ಪೂರ್ಣಾಂಕ rmdir_parents(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *stop);

ಪೂರ್ಣಾಂಕ rename_noreplace(ಪೂರ್ಣಾಂಕ olddirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath);

ಪೂರ್ಣಾಂಕ readlinkat_malloc(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ readlink_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ readlinkat_malloc(AT_FDCWD, p, ret);
}
ಪೂರ್ಣಾಂಕ readlink_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ readlink_and_make_absolute(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ chmod_and_chown_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ chmod_and_chown(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid) {
        ಹಿಂತಿರುಗಿ chmod_and_chown_at(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid);
}
ಪೂರ್ಣಾಂಕ fchmod_and_chown_with_fallback(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fchmod_and_chown(ಪೂರ್ಣಾಂಕ fd, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid) {
        ಹಿಂತಿರುಗಿ fchmod_and_chown_with_fallback(fd, ಖಾಲಿ, ಬಹುಲಕ, uid, gid); /* no fallback */
}

ಪೂರ್ಣಾಂಕ fchmod_umask(ಪೂರ್ಣಾಂಕ fd, mode_t ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ fchmod_opath(ಪೂರ್ಣಾಂಕ fd, mode_t m);

ಪೂರ್ಣಾಂಕ futimens_opath(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec ts[2]);

ಪೂರ್ಣಾಂಕ fd_warn_permissions(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ stat_warn_permissions(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);

ಪೂರ್ಣಾಂಕ access_nofollow(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ access_fd(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);

ಪೂರ್ಣಾಂಕ touch_fd(ಪೂರ್ಣಾಂಕ fd, usec_t stamp);
ಪೂರ್ಣಾಂಕ touch_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ touch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ symlinkat_idempotent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath, bool make_relative);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ symlink_idempotent(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath, bool make_relative) {
        ಹಿಂತಿರುಗಿ symlinkat_idempotent(ಗುರಿ, AT_FDCWD, linkpath, make_relative);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ಪೂರ್ಣಾಂಕ symlinkat_atomic_ತುಂಬಿದ_label(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath, SymlinkFlags flags, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ symlinkat_atomic_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath, SymlinkFlags flags) {
        ಹಿಂತಿರುಗಿ symlinkat_atomic_ತುಂಬಿದ_label(ಗುರಿ, atfd, linkpath, flags, /* label_context= */ ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ symlink_atomic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಗುರಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *linkpath) {
        ಹಿಂತಿರುಗಿ symlinkat_atomic_ತುಂಬಿದ(ಗುರಿ, AT_FDCWD, linkpath, 0);
}

ಪೂರ್ಣಾಂಕ mknodat_atomic(ಪೂರ್ಣಾಂಕ atfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, dev_t dev);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ mknod_atomic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, dev_t dev) {
        ಹಿಂತಿರುಗಿ mknodat_atomic(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, dev);
}

ಪೂರ್ಣಾಂಕ mkfifoat_atomic(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ mkfifo_atomic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ mkfifoat_atomic(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ);
}

ಪೂರ್ಣಾಂಕ get_files_ಒಳ_directory(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ ***list);

ಪೂರ್ಣಾಂಕ tmp_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ var_tmp_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ unlink_or_warn(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
ಅಕ್ಷರ *rmdir_and_free(ಅಕ್ಷರ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಅಕ್ಷರ*, rmdir_and_free);

ಅಕ್ಷರ* unlink_and_free(ಅಕ್ಷರ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ಅಕ್ಷರ*, unlink_and_free);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ಪೂರ್ಣಾಂಕ unlinkat_deallocate(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UnlinkDeallocateFlags flags);

ಪೂರ್ಣಾಂಕ open_ಪೋಷಕ_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ open_ಪೋಷಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ open_ಪೋಷಕ_at(AT_FDCWD, ಮಾರ್ಗ, flags, ಬಹುಲಕ);
}

ಪೂರ್ಣಾಂಕ conservative_renameat(ಪೂರ್ಣಾಂಕ olddirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ conservative_rename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath) {
        ಹಿಂತಿರುಗಿ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ಪೂರ್ಣಾಂಕ posix_fallocate_loop(ಪೂರ್ಣಾಂಕ fd, uint64_t ಸ್ಥಾನಾಂತರ, uint64_t ಗಾತ್ರ_2);

ಪೂರ್ಣಾಂಕ parse_cifs_service(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಅಕ್ಷರ **ret_host, ಅಕ್ಷರ **ret_service, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

ಪೂರ್ಣಾಂಕ open_mkdir_at_ತುಂಬಿದ_label(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ open_mkdir_at_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ open_mkdir_at_ತುಂಬಿದ_label(dirfd, ಮಾರ್ಗ, flags, xopen_flags, ಬಹುಲಕ, /* label_context= */ ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ open_mkdir_at(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ open_mkdir_at_ತುಂಬಿದ(dirfd, ಮಾರ್ಗ, flags, 0, ಬಹುಲಕ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ open_mkdir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ open_mkdir_at_ತುಂಬಿದ(AT_FDCWD, ಮಾರ್ಗ, flags, 0, ಬಹುಲಕ);
}

ಪೂರ್ಣಾಂಕ openat_report_new(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, ಪೂರ್ಣಾಂಕ flags, mode_t ಬಹುಲಕ, bool *ret_newly_created);

ಪೂರ್ಣಾಂಕ xopenat_ತುಂಬಿದ_label(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xopenat_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ xopenat_ತುಂಬಿದ_label(dir_fd, ಮಾರ್ಗ, open_flags, xopen_flags, ಬಹುಲಕ, /* label_context= */ ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xopenat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags) {
        ಹಿಂತಿರುಗಿ xopenat_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ, open_flags, 0, ಬಹುಲಕ_INVALID);
}

ಪೂರ್ಣಾಂಕ xopenat_ಬೀಗ_ತುಂಬಿದ_label(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ, ಬೀಗ_Type locktype, ಪೂರ್ಣಾಂಕ operation, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xopenat_ಬೀಗ_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, XOpenFlags xopen_flags, mode_t ಬಹುಲಕ, ಬೀಗ_Type locktype, ಪೂರ್ಣಾಂಕ operation) {
        ಹಿಂತಿರುಗಿ xopenat_ಬೀಗ_ತುಂಬಿದ_label(dir_fd, ಮಾರ್ಗ, open_flags, xopen_flags, ಬಹುಲಕ, locktype, operation, /* label_context= */ ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xopenat_ಬೀಗ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ open_flags, ಬೀಗ_Type locktype, ಪೂರ್ಣಾಂಕ operation) {
        ಹಿಂತಿರುಗಿ xopenat_ಬೀಗ_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ, open_flags, 0, 0, locktype, operation);
}

ಪೂರ್ಣಾಂಕ link_fd(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath);

ಪೂರ್ಣಾಂಕ linkat_replace(ಪೂರ್ಣಾಂಕ olddirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *oldpath, ಪೂರ್ಣಾಂಕ newdirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newpath);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ at_flags_normalize_nofollow(ಪೂರ್ಣಾಂಕ flags) {
        ಆದರೆ (FLAGS_ಗಣ(flags, AT_SYMLINK_FOLLOW)) {
                ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ಇಲ್ಲದಿದ್ದರೆ
                flags |= AT_SYMLINK_NOFOLLOW;
        ಹಿಂತಿರುಗಿ flags;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ at_flags_normalize_follow(ಪೂರ್ಣಾಂಕ flags) {
        ಆದರೆ (FLAGS_ಗಣ(flags, AT_SYMLINK_NOFOLLOW)) {
                ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ಇಲ್ಲದಿದ್ದರೆ
                flags |= AT_SYMLINK_FOLLOW;
        ಹಿಂತಿರುಗಿ flags;
}
