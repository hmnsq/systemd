/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"
#समावेश "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
अटल अंतरभूत mode_t PTR_TO_MODE(व्योम *p) {
        वापस p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
अटल अंतरभूत व्योम* MODE_TO_PTR(mode_t m) {
        वापस m == MODE_INVALID ? शून्य : (व्योम *) ((uintptr_t) m + 1);
}

पूर्णांक rmdir_parents(स्थिर अक्षर *path, स्थिर अक्षर *stop);

पूर्णांक rename_noreplace(पूर्णांक olddirfd, स्थिर अक्षर *oldpath, पूर्णांक newdirfd, स्थिर अक्षर *newpath);

पूर्णांक readlinkat_malloc(पूर्णांक fd, स्थिर अक्षर *p, अक्षर **ret);
अटल अंतरभूत पूर्णांक readlink_malloc(स्थिर अक्षर *p, अक्षर **ret) {
        वापस readlinkat_malloc(AT_FDCWD, p, ret);
}
पूर्णांक readlink_मूल्य(स्थिर अक्षर *p, अक्षर **ret);
पूर्णांक readlink_and_make_absolute(स्थिर अक्षर *p, अक्षर **ret);

पूर्णांक chmod_and_chown_at(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid);
अटल अंतरभूत पूर्णांक chmod_and_chown(स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid) {
        वापस chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
पूर्णांक fchmod_and_chown_with_fallback(पूर्णांक fd, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid);
अटल अंतरभूत पूर्णांक fchmod_and_chown(पूर्णांक fd, mode_t mode, uid_t uid, gid_t gid) {
        वापस fchmod_and_chown_with_fallback(fd, शून्य, mode, uid, gid); /* no fallback */
}

पूर्णांक fchmod_umask(पूर्णांक fd, mode_t mode);
पूर्णांक fchmod_opath(पूर्णांक fd, mode_t m);

पूर्णांक futimens_opath(पूर्णांक fd, स्थिर काष्ठा timespec ts[2]);

पूर्णांक fd_warn_permissions(स्थिर अक्षर *path, पूर्णांक fd);
पूर्णांक stat_warn_permissions(स्थिर अक्षर *path, स्थिर काष्ठा stat *st);

पूर्णांक access_nofollow(स्थिर अक्षर *path, पूर्णांक mode);
पूर्णांक access_fd(पूर्णांक fd, पूर्णांक mode);

पूर्णांक touch_fd(पूर्णांक fd, usec_t stamp);
पूर्णांक touch_file(स्थिर अक्षर *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
पूर्णांक touch(स्थिर अक्षर *path);

पूर्णांक symlinkat_idempotent(स्थिर अक्षर *target, पूर्णांक atfd, स्थिर अक्षर *linkpath, bool make_relative);
अटल अंतरभूत पूर्णांक symlink_idempotent(स्थिर अक्षर *target, स्थिर अक्षर *linkpath, bool make_relative) {
        वापस symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

प्रकार क्रमागत SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

पूर्णांक symlinkat_atomic_full_label(स्थिर अक्षर *target, पूर्णांक atfd, स्थिर अक्षर *linkpath, SymlinkFlags flags, LabelContext *label_context);
अटल अंतरभूत पूर्णांक symlinkat_atomic_full(स्थिर अक्षर *target, पूर्णांक atfd, स्थिर अक्षर *linkpath, SymlinkFlags flags) {
        वापस symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ शून्य);
}
अटल अंतरभूत पूर्णांक symlink_atomic(स्थिर अक्षर *target, स्थिर अक्षर *linkpath) {
        वापस symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

पूर्णांक mknodat_atomic(पूर्णांक atfd, स्थिर अक्षर *path, mode_t mode, dev_t dev);
अटल अंतरभूत पूर्णांक mknod_atomic(स्थिर अक्षर *path, mode_t mode, dev_t dev) {
        वापस mknodat_atomic(AT_FDCWD, path, mode, dev);
}

पूर्णांक mkfifoat_atomic(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode);
अटल अंतरभूत पूर्णांक mkfifo_atomic(स्थिर अक्षर *path, mode_t mode) {
        वापस mkfifoat_atomic(AT_FDCWD, path, mode);
}

पूर्णांक get_files_in_directory(स्थिर अक्षर *path, अक्षर ***list);

पूर्णांक tmp_dir(स्थिर अक्षर **ret);
पूर्णांक var_tmp_dir(स्थिर अक्षर **ret);

पूर्णांक unlink_or_warn(स्थिर अक्षर *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
अक्षर *rmdir_and_free(अक्षर *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(अक्षर*, rmdir_and_free);

अक्षर* unlink_and_free(अक्षर *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(अक्षर*, unlink_and_free);

प्रकार क्रमागत UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

पूर्णांक unlinkat_deallocate(पूर्णांक fd, स्थिर अक्षर *name, UnlinkDeallocateFlags flags);

पूर्णांक open_parent_at(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक flags, mode_t mode);
अटल अंतरभूत पूर्णांक open_parent(स्थिर अक्षर *path, पूर्णांक flags, mode_t mode) {
        वापस open_parent_at(AT_FDCWD, path, flags, mode);
}

पूर्णांक conservative_renameat(पूर्णांक olddirfd, स्थिर अक्षर *oldpath, पूर्णांक newdirfd, स्थिर अक्षर *newpath);
अटल अंतरभूत पूर्णांक conservative_rename(स्थिर अक्षर *oldpath, स्थिर अक्षर *newpath) {
        वापस conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

पूर्णांक posix_fallocate_loop(पूर्णांक fd, uint64_t offset, uint64_t आकार);

पूर्णांक parse_cifs_service(स्थिर अक्षर *s, अक्षर **ret_host, अक्षर **ret_service, अक्षर **ret_path);

प्रकार क्रमागत XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

पूर्णांक open_mkdir_at_full_label(पूर्णांक dirfd, स्थिर अक्षर *path, पूर्णांक flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
अटल अंतरभूत पूर्णांक open_mkdir_at_full(पूर्णांक dirfd, स्थिर अक्षर *path, पूर्णांक flags, XOpenFlags xopen_flags, mode_t mode) {
        वापस open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ शून्य);
}
अटल अंतरभूत पूर्णांक open_mkdir_at(पूर्णांक dirfd, स्थिर अक्षर *path, पूर्णांक flags, mode_t mode) {
        वापस open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
अटल अंतरभूत पूर्णांक open_mkdir(स्थिर अक्षर *path, पूर्णांक flags, mode_t mode) {
        वापस open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

पूर्णांक openat_report_new(पूर्णांक dirfd, स्थिर अक्षर *pathname, पूर्णांक flags, mode_t mode, bool *ret_newly_created);

पूर्णांक xopenat_full_label(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
अटल अंतरभूत पूर्णांक xopenat_full(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक open_flags, XOpenFlags xopen_flags, mode_t mode) {
        वापस xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ शून्य);
}
अटल अंतरभूत पूर्णांक xopenat(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक open_flags) {
        वापस xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

पूर्णांक xopenat_lock_full_label(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, पूर्णांक operation, LabelContext *label_context);
अटल अंतरभूत पूर्णांक xopenat_lock_full(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, पूर्णांक operation) {
        वापस xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ शून्य);
}
अटल अंतरभूत पूर्णांक xopenat_lock(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक open_flags, LockType locktype, पूर्णांक operation) {
        वापस xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

पूर्णांक link_fd(पूर्णांक fd, पूर्णांक newdirfd, स्थिर अक्षर *newpath);

पूर्णांक linkat_replace(पूर्णांक olddirfd, स्थिर अक्षर *oldpath, पूर्णांक newdirfd, स्थिर अक्षर *newpath);

अटल अंतरभूत पूर्णांक at_flags_normalize_nofollow(पूर्णांक flags) {
        अगर (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                निश्चित(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } अन्यथा
                flags |= AT_SYMLINK_NOFOLLOW;
        वापस flags;
}

अटल अंतरभूत पूर्णांक at_flags_normalize_follow(पूर्णांक flags) {
        अगर (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                निश्चित(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } अन्यथा
                flags |= AT_SYMLINK_FOLLOW;
        वापस flags;
}
