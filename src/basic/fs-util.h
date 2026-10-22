/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
സ്ഥിതം വരിയിൽ mode_t PTR_TO_MODE(ശൂന്യം *p) {
        തിരികെ p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
സ്ഥിതം വരിയിൽ ശൂന്യം* MODE_TO_PTR(mode_t m) {
        തിരികെ m == MODE_INVALID ? ശൂന്യ : (ശൂന്യം *) ((uintptr_t) m + 1);
}

പൂർണ്ണസംഖ്യ rmdir_parents(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *stop);

പൂർണ്ണസംഖ്യ rename_noreplace(പൂർണ്ണസംഖ്യ olddirfd, സ്ഥിരം അക്ഷരം *oldpath, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath);

പൂർണ്ണസംഖ്യ readlinkat_malloc(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ readlink_malloc(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret) {
        തിരികെ readlinkat_malloc(AT_FDCWD, p, ret);
}
പൂർണ്ണസംഖ്യ readlink_മൂല്യം(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ readlink_and_make_absolute(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ chmod_and_chown_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ chmod_and_chown(സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid) {
        തിരികെ chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
പൂർണ്ണസംഖ്യ fchmod_and_chown_with_fallback(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fchmod_and_chown(പൂർണ്ണസംഖ്യ fd, mode_t mode, uid_t uid, gid_t gid) {
        തിരികെ fchmod_and_chown_with_fallback(fd, ശൂന്യ, mode, uid, gid); /* no fallback */
}

പൂർണ്ണസംഖ്യ fchmod_umask(പൂർണ്ണസംഖ്യ fd, mode_t mode);
പൂർണ്ണസംഖ്യ fchmod_opath(പൂർണ്ണസംഖ്യ fd, mode_t m);

പൂർണ്ണസംഖ്യ futimens_opath(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ഘടന timespec ts[2]);

പൂർണ്ണസംഖ്യ fd_warn_permissions(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ stat_warn_permissions(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം ഘടന stat *st);

പൂർണ്ണസംഖ്യ access_nofollow(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ mode);
പൂർണ്ണസംഖ്യ access_fd(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ mode);

പൂർണ്ണസംഖ്യ touch_fd(പൂർണ്ണസംഖ്യ fd, usec_t stamp);
പൂർണ്ണസംഖ്യ touch_file(സ്ഥിരം അക്ഷരം *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
പൂർണ്ണസംഖ്യ touch(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ symlinkat_idempotent(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *linkpath, bool make_relative);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ symlink_idempotent(സ്ഥിരം അക്ഷരം *target, സ്ഥിരം അക്ഷരം *linkpath, bool make_relative) {
        തിരികെ symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

തരനിർവചനം എണ്ണൽ SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

പൂർണ്ണസംഖ്യ symlinkat_atomic_full_label(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *linkpath, SymlinkFlags flags, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ symlinkat_atomic_full(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *linkpath, SymlinkFlags flags) {
        തിരികെ symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ symlink_atomic(സ്ഥിരം അക്ഷരം *target, സ്ഥിരം അക്ഷരം *linkpath) {
        തിരികെ symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

പൂർണ്ണസംഖ്യ mknodat_atomic(പൂർണ്ണസംഖ്യ atfd, സ്ഥിരം അക്ഷരം *path, mode_t mode, dev_t dev);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ mknod_atomic(സ്ഥിരം അക്ഷരം *path, mode_t mode, dev_t dev) {
        തിരികെ mknodat_atomic(AT_FDCWD, path, mode, dev);
}

പൂർണ്ണസംഖ്യ mkfifoat_atomic(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ mkfifo_atomic(സ്ഥിരം അക്ഷരം *path, mode_t mode) {
        തിരികെ mkfifoat_atomic(AT_FDCWD, path, mode);
}

പൂർണ്ണസംഖ്യ get_files_in_directory(സ്ഥിരം അക്ഷരം *path, അക്ഷരം ***list);

പൂർണ്ണസംഖ്യ tmp_dir(സ്ഥിരം അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ var_tmp_dir(സ്ഥിരം അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ unlink_or_warn(സ്ഥിരം അക്ഷരം *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
അക്ഷരം *rmdir_and_free(അക്ഷരം *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(അക്ഷരം*, rmdir_and_free);

അക്ഷരം* unlink_and_free(അക്ഷരം *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(അക്ഷരം*, unlink_and_free);

തരനിർവചനം എണ്ണൽ UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

പൂർണ്ണസംഖ്യ unlinkat_deallocate(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *name, UnlinkDeallocateFlags flags);

പൂർണ്ണസംഖ്യ open_parent_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, mode_t mode);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ open_parent(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, mode_t mode) {
        തിരികെ open_parent_at(AT_FDCWD, path, flags, mode);
}

പൂർണ്ണസംഖ്യ conservative_renameat(പൂർണ്ണസംഖ്യ olddirfd, സ്ഥിരം അക്ഷരം *oldpath, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ conservative_rename(സ്ഥിരം അക്ഷരം *oldpath, സ്ഥിരം അക്ഷരം *newpath) {
        തിരികെ conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

പൂർണ്ണസംഖ്യ posix_fallocate_loop(പൂർണ്ണസംഖ്യ fd, uint64_t offset, uint64_t size);

പൂർണ്ണസംഖ്യ parse_cifs_service(സ്ഥിരം അക്ഷരം *s, അക്ഷരം **ret_host, അക്ഷരം **ret_service, അക്ഷരം **ret_path);

തരനിർവചനം എണ്ണൽ XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

പൂർണ്ണസംഖ്യ open_mkdir_at_full_label(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ open_mkdir_at_full(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, XOpenFlags xopen_flags, mode_t mode) {
        തിരികെ open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ open_mkdir_at(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, mode_t mode) {
        തിരികെ open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ open_mkdir(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, mode_t mode) {
        തിരികെ open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

പൂർണ്ണസംഖ്യ openat_report_new(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *pathname, പൂർണ്ണസംഖ്യ flags, mode_t mode, bool *ret_newly_created);

പൂർണ്ണസംഖ്യ xopenat_full_label(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xopenat_full(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, XOpenFlags xopen_flags, mode_t mode) {
        തിരികെ xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xopenat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags) {
        തിരികെ xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

പൂർണ്ണസംഖ്യ xopenat_lock_full_label(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, പൂർണ്ണസംഖ്യ operation, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xopenat_lock_full(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, പൂർണ്ണസംഖ്യ operation) {
        തിരികെ xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xopenat_lock(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ open_flags, LockType locktype, പൂർണ്ണസംഖ്യ operation) {
        തിരികെ xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

പൂർണ്ണസംഖ്യ link_fd(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath);

പൂർണ്ണസംഖ്യ linkat_replace(പൂർണ്ണസംഖ്യ olddirfd, സ്ഥിരം അക്ഷരം *oldpath, പൂർണ്ണസംഖ്യ newdirfd, സ്ഥിരം അക്ഷരം *newpath);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ at_flags_normalize_nofollow(പൂർണ്ണസംഖ്യ flags) {
        എങ്കിൽ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                പ്രതിജ്ഞാ(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } അല്ലെങ്കിൽ
                flags |= AT_SYMLINK_NOFOLLOW;
        തിരികെ flags;
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ at_flags_normalize_follow(പൂർണ്ണസംഖ്യ flags) {
        എങ്കിൽ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                പ്രതിജ്ഞാ(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } അല്ലെങ്കിൽ
                flags |= AT_SYMLINK_FOLLOW;
        തിരികെ flags;
}
