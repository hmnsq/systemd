/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
ਸਥਿਰ ਸਤਰਅੰਦਰ mode_t PTR_ਮੰਜ਼ਿਲ_ਬਹੁਲਕ(ਖਾਲੀ *p) {
        ਵਾਪਸ p ? (mode_t) ((uintptr_t) p - 1) : ਬਹੁਲਕ_INVALID;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_PTR(mode_t m) {
        ਵਾਪਸ m == ਬਹੁਲਕ_INVALID ? ਨਿਰਾ : (ਖਾਲੀ *) ((uintptr_t) m + 1);
}

ਪੂਰਨ rmdir_parents(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *stop);

ਪੂਰਨ rename_noreplace(ਪੂਰਨ olddirfd, ਅਚਲ ਅੱਖਰ *oldpath, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath);

ਪੂਰਨ readlinkat_malloc(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ readlink_malloc(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret) {
        ਵਾਪਸ readlinkat_malloc(AT_FDCWD, p, ret);
}
ਪੂਰਨ readlink_ਮੁੱਲ(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret);
ਪੂਰਨ readlink_and_make_absolute(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret);

ਪੂਰਨ chmod_and_chown_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ chmod_and_chown(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid) {
        ਵਾਪਸ chmod_and_chown_at(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid);
}
ਪੂਰਨ fchmod_and_chown_with_fallback(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fchmod_and_chown(ਪੂਰਨ fd, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid) {
        ਵਾਪਸ fchmod_and_chown_with_fallback(fd, ਨਿਰਾ, ਬਹੁਲਕ, uid, gid); /* no fallback */
}

ਪੂਰਨ fchmod_umask(ਪੂਰਨ fd, mode_t ਬਹੁਲਕ);
ਪੂਰਨ fchmod_opath(ਪੂਰਨ fd, mode_t m);

ਪੂਰਨ futimens_opath(ਪੂਰਨ fd, ਅਚਲ ਬਣਤਰ timespec ts[2]);

ਪੂਰਨ fd_warn_permissions(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ fd);
ਪੂਰਨ stat_warn_permissions(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਬਣਤਰ stat *st);

ਪੂਰਨ access_nofollow(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ ਬਹੁਲਕ);
ਪੂਰਨ access_fd(ਪੂਰਨ fd, ਪੂਰਨ ਬਹੁਲਕ);

ਪੂਰਨ touch_fd(ਪੂਰਨ fd, usec_t stamp);
ਪੂਰਨ touch_file(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t ਬਹੁਲਕ);
ਪੂਰਨ touch(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ symlinkat_idempotent(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *linkpath, bool make_relative);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ symlink_idempotent(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *linkpath, bool make_relative) {
        ਵਾਪਸ symlinkat_idempotent(ਨਿਸ਼ਾਨਾ, AT_FDCWD, linkpath, make_relative);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

ਪੂਰਨ symlinkat_atomic_ਭਰਿਆ_label(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *linkpath, SymlinkFlags flags, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ symlinkat_atomic_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *linkpath, SymlinkFlags flags) {
        ਵਾਪਸ symlinkat_atomic_ਭਰਿਆ_label(ਨਿਸ਼ਾਨਾ, atfd, linkpath, flags, /* label_context= */ ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ symlink_atomic(ਅਚਲ ਅੱਖਰ *ਨਿਸ਼ਾਨਾ, ਅਚਲ ਅੱਖਰ *linkpath) {
        ਵਾਪਸ symlinkat_atomic_ਭਰਿਆ(ਨਿਸ਼ਾਨਾ, AT_FDCWD, linkpath, 0);
}

ਪੂਰਨ mknodat_atomic(ਪੂਰਨ atfd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, dev_t dev);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ mknod_atomic(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, dev_t dev) {
        ਵਾਪਸ mknodat_atomic(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, dev);
}

ਪੂਰਨ mkfifoat_atomic(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ mkfifo_atomic(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ mkfifoat_atomic(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ);
}

ਪੂਰਨ get_files_ਵਿੱਚ_directory(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ ***list);

ਪੂਰਨ tmp_dir(ਅਚਲ ਅੱਖਰ **ret);
ਪੂਰਨ var_tmp_dir(ਅਚਲ ਅੱਖਰ **ret);

ਪੂਰਨ unlink_or_warn(ਅਚਲ ਅੱਖਰ *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
ਅੱਖਰ *rmdir_and_free(ਅੱਖਰ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਅੱਖਰ*, rmdir_and_free);

ਅੱਖਰ* unlink_and_free(ਅੱਖਰ *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(ਅੱਖਰ*, unlink_and_free);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

ਪੂਰਨ unlinkat_deallocate(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *name, UnlinkDeallocateFlags flags);

ਪੂਰਨ open_ਮਾਪਾ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ open_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ open_ਮਾਪਾ_at(AT_FDCWD, ਰਸਤਾ, flags, ਬਹੁਲਕ);
}

ਪੂਰਨ conservative_renameat(ਪੂਰਨ olddirfd, ਅਚਲ ਅੱਖਰ *oldpath, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ conservative_rename(ਅਚਲ ਅੱਖਰ *oldpath, ਅਚਲ ਅੱਖਰ *newpath) {
        ਵਾਪਸ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

ਪੂਰਨ posix_fallocate_loop(ਪੂਰਨ fd, uint64_t ਵਿੱਥ, uint64_t ਮਾਪ);

ਪੂਰਨ parse_cifs_service(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ **ret_host, ਅੱਖਰ **ret_service, ਅੱਖਰ **ret_ਰਸਤਾ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

ਪੂਰਨ open_mkdir_at_ਭਰਿਆ_label(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ open_mkdir_at_ਭਰਿਆ(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ open_mkdir_at_ਭਰਿਆ_label(dirfd, ਰਸਤਾ, flags, xopen_flags, ਬਹੁਲਕ, /* label_context= */ ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ open_mkdir_at(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ open_mkdir_at_ਭਰਿਆ(dirfd, ਰਸਤਾ, flags, 0, ਬਹੁਲਕ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ open_mkdir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ open_mkdir_at_ਭਰਿਆ(AT_FDCWD, ਰਸਤਾ, flags, 0, ਬਹੁਲਕ);
}

ਪੂਰਨ openat_report_new(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *pathname, ਪੂਰਨ flags, mode_t ਬਹੁਲਕ, bool *ret_newly_created);

ਪੂਰਨ xopenat_ਭਰਿਆ_label(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xopenat_ਭਰਿਆ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ xopenat_ਭਰਿਆ_label(dir_fd, ਰਸਤਾ, open_flags, xopen_flags, ਬਹੁਲਕ, /* label_context= */ ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xopenat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags) {
        ਵਾਪਸ xopenat_ਭਰਿਆ(dir_fd, ਰਸਤਾ, open_flags, 0, ਬਹੁਲਕ_INVALID);
}

ਪੂਰਨ xopenat_ਤਾਲਾ_ਭਰਿਆ_label(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ, ਤਾਲਾ_Type locktype, ਪੂਰਨ operation, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xopenat_ਤਾਲਾ_ਭਰਿਆ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, XOpenFlags xopen_flags, mode_t ਬਹੁਲਕ, ਤਾਲਾ_Type locktype, ਪੂਰਨ operation) {
        ਵਾਪਸ xopenat_ਤਾਲਾ_ਭਰਿਆ_label(dir_fd, ਰਸਤਾ, open_flags, xopen_flags, ਬਹੁਲਕ, locktype, operation, /* label_context= */ ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xopenat_ਤਾਲਾ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ open_flags, ਤਾਲਾ_Type locktype, ਪੂਰਨ operation) {
        ਵਾਪਸ xopenat_ਤਾਲਾ_ਭਰਿਆ(dir_fd, ਰਸਤਾ, open_flags, 0, 0, locktype, operation);
}

ਪੂਰਨ link_fd(ਪੂਰਨ fd, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath);

ਪੂਰਨ linkat_replace(ਪੂਰਨ olddirfd, ਅਚਲ ਅੱਖਰ *oldpath, ਪੂਰਨ newdirfd, ਅਚਲ ਅੱਖਰ *newpath);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ at_flags_normalize_nofollow(ਪੂਰਨ flags) {
        ਜੇ (FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_FOLLOW)) {
                ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } ਨਹੀਂਤਾਂ
                flags |= AT_SYMLINK_NOFOLLOW;
        ਵਾਪਸ flags;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ at_flags_normalize_follow(ਪੂਰਨ flags) {
        ਜੇ (FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_NOFOLLOW)) {
                ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } ਨਹੀਂਤਾਂ
                flags |= AT_SYMLINK_FOLLOW;
        ਵਾਪਸ flags;
}
