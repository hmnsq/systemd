/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
स्थिर अन्तःस्थ mode_t PTR_TO_MODE(शून्य *p) {
        प्रत्यागम p ? (mode_t) ((uintptr_t) p - 1) : MODE_INVALID;
}
स्थिर अन्तःस्थ शून्य* MODE_TO_PTR(mode_t m) {
        प्रत्यागम m == MODE_INVALID ? रिक्त : (शून्य *) ((uintptr_t) m + 1);
}

पूर्णाङ्क rmdir_parents(ध्रुव वर्ण *path, ध्रुव वर्ण *stop);

पूर्णाङ्क rename_noreplace(पूर्णाङ्क olddirfd, ध्रुव वर्ण *oldpath, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath);

पूर्णाङ्क readlinkat_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *p, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क readlink_malloc(ध्रुव वर्ण *p, वर्ण **ret) {
        प्रत्यागम readlinkat_malloc(AT_FDCWD, p, ret);
}
पूर्णाङ्क readlink_मानम्(ध्रुव वर्ण *p, वर्ण **ret);
पूर्णाङ्क readlink_and_make_absolute(ध्रुव वर्ण *p, वर्ण **ret);

पूर्णाङ्क chmod_and_chown_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid);
स्थिर अन्तःस्थ पूर्णाङ्क chmod_and_chown(ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid) {
        प्रत्यागम chmod_and_chown_at(AT_FDCWD, path, mode, uid, gid);
}
पूर्णाङ्क fchmod_and_chown_with_fallback(पूर्णाङ्क fd, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid);
स्थिर अन्तःस्थ पूर्णाङ्क fchmod_and_chown(पूर्णाङ्क fd, mode_t mode, uid_t uid, gid_t gid) {
        प्रत्यागम fchmod_and_chown_with_fallback(fd, रिक्त, mode, uid, gid); /* no fallback */
}

पूर्णाङ्क fchmod_umask(पूर्णाङ्क fd, mode_t mode);
पूर्णाङ्क fchmod_opath(पूर्णाङ्क fd, mode_t m);

पूर्णाङ्क futimens_opath(पूर्णाङ्क fd, ध्रुव संरचना timespec ts[2]);

पूर्णाङ्क fd_warn_permissions(ध्रुव वर्ण *path, पूर्णाङ्क fd);
पूर्णाङ्क stat_warn_permissions(ध्रुव वर्ण *path, ध्रुव संरचना stat *st);

पूर्णाङ्क access_nofollow(ध्रुव वर्ण *path, पूर्णाङ्क mode);
पूर्णाङ्क access_fd(पूर्णाङ्क fd, पूर्णाङ्क mode);

पूर्णाङ्क touch_fd(पूर्णाङ्क fd, usec_t stamp);
पूर्णाङ्क touch_file(ध्रुव वर्ण *path, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t mode);
पूर्णाङ्क touch(ध्रुव वर्ण *path);

पूर्णाङ्क symlinkat_idempotent(ध्रुव वर्ण *target, पूर्णाङ्क atfd, ध्रुव वर्ण *linkpath, bool make_relative);
स्थिर अन्तःस्थ पूर्णाङ्क symlink_idempotent(ध्रुव वर्ण *target, ध्रुव वर्ण *linkpath, bool make_relative) {
        प्रत्यागम symlinkat_idempotent(target, AT_FDCWD, linkpath, make_relative);
}

प्रकारनिर्देश गणना SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

पूर्णाङ्क symlinkat_atomic_full_label(ध्रुव वर्ण *target, पूर्णाङ्क atfd, ध्रुव वर्ण *linkpath, SymlinkFlags flags, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क symlinkat_atomic_full(ध्रुव वर्ण *target, पूर्णाङ्क atfd, ध्रुव वर्ण *linkpath, SymlinkFlags flags) {
        प्रत्यागम symlinkat_atomic_full_label(target, atfd, linkpath, flags, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क symlink_atomic(ध्रुव वर्ण *target, ध्रुव वर्ण *linkpath) {
        प्रत्यागम symlinkat_atomic_full(target, AT_FDCWD, linkpath, 0);
}

पूर्णाङ्क mknodat_atomic(पूर्णाङ्क atfd, ध्रुव वर्ण *path, mode_t mode, dev_t dev);
स्थिर अन्तःस्थ पूर्णाङ्क mknod_atomic(ध्रुव वर्ण *path, mode_t mode, dev_t dev) {
        प्रत्यागम mknodat_atomic(AT_FDCWD, path, mode, dev);
}

पूर्णाङ्क mkfifoat_atomic(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode);
स्थिर अन्तःस्थ पूर्णाङ्क mkfifo_atomic(ध्रुव वर्ण *path, mode_t mode) {
        प्रत्यागम mkfifoat_atomic(AT_FDCWD, path, mode);
}

पूर्णाङ्क get_files_in_directory(ध्रुव वर्ण *path, वर्ण ***list);

पूर्णाङ्क tmp_dir(ध्रुव वर्ण **ret);
पूर्णाङ्क var_tmp_dir(ध्रुव वर्ण **ret);

पूर्णाङ्क unlink_or_warn(ध्रुव वर्ण *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
वर्ण *rmdir_and_free(वर्ण *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण*, rmdir_and_free);

वर्ण* unlink_and_free(वर्ण *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(वर्ण*, unlink_and_free);

प्रकारनिर्देश गणना UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

पूर्णाङ्क unlinkat_deallocate(पूर्णाङ्क fd, ध्रुव वर्ण *name, UnlinkDeallocateFlags flags);

पूर्णाङ्क open_parent_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क flags, mode_t mode);
स्थिर अन्तःस्थ पूर्णाङ्क open_parent(ध्रुव वर्ण *path, पूर्णाङ्क flags, mode_t mode) {
        प्रत्यागम open_parent_at(AT_FDCWD, path, flags, mode);
}

पूर्णाङ्क conservative_renameat(पूर्णाङ्क olddirfd, ध्रुव वर्ण *oldpath, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath);
स्थिर अन्तःस्थ पूर्णाङ्क conservative_rename(ध्रुव वर्ण *oldpath, ध्रुव वर्ण *newpath) {
        प्रत्यागम conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

पूर्णाङ्क posix_fallocate_loop(पूर्णाङ्क fd, uint64_t offset, uint64_t size);

पूर्णाङ्क parse_cifs_service(ध्रुव वर्ण *s, वर्ण **ret_host, वर्ण **ret_service, वर्ण **ret_path);

प्रकारनिर्देश गणना XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

पूर्णाङ्क open_mkdir_at_full_label(पूर्णाङ्क dirfd, ध्रुव वर्ण *path, पूर्णाङ्क flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क open_mkdir_at_full(पूर्णाङ्क dirfd, ध्रुव वर्ण *path, पूर्णाङ्क flags, XOpenFlags xopen_flags, mode_t mode) {
        प्रत्यागम open_mkdir_at_full_label(dirfd, path, flags, xopen_flags, mode, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क open_mkdir_at(पूर्णाङ्क dirfd, ध्रुव वर्ण *path, पूर्णाङ्क flags, mode_t mode) {
        प्रत्यागम open_mkdir_at_full(dirfd, path, flags, 0, mode);
}
स्थिर अन्तःस्थ पूर्णाङ्क open_mkdir(ध्रुव वर्ण *path, पूर्णाङ्क flags, mode_t mode) {
        प्रत्यागम open_mkdir_at_full(AT_FDCWD, path, flags, 0, mode);
}

पूर्णाङ्क openat_report_new(पूर्णाङ्क dirfd, ध्रुव वर्ण *pathname, पूर्णाङ्क flags, mode_t mode, bool *ret_newly_created);

पूर्णाङ्क xopenat_full_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क open_flags, XOpenFlags xopen_flags, mode_t mode, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क xopenat_full(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क open_flags, XOpenFlags xopen_flags, mode_t mode) {
        प्रत्यागम xopenat_full_label(dir_fd, path, open_flags, xopen_flags, mode, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क xopenat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क open_flags) {
        प्रत्यागम xopenat_full(dir_fd, path, open_flags, 0, MODE_INVALID);
}

पूर्णाङ्क xopenat_lock_full_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, पूर्णाङ्क operation, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क xopenat_lock_full(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क open_flags, XOpenFlags xopen_flags, mode_t mode, LockType locktype, पूर्णाङ्क operation) {
        प्रत्यागम xopenat_lock_full_label(dir_fd, path, open_flags, xopen_flags, mode, locktype, operation, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क xopenat_lock(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क open_flags, LockType locktype, पूर्णाङ्क operation) {
        प्रत्यागम xopenat_lock_full(dir_fd, path, open_flags, 0, 0, locktype, operation);
}

पूर्णाङ्क link_fd(पूर्णाङ्क fd, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath);

पूर्णाङ्क linkat_replace(पूर्णाङ्क olddirfd, ध्रुव वर्ण *oldpath, पूर्णाङ्क newdirfd, ध्रुव वर्ण *newpath);

स्थिर अन्तःस्थ पूर्णाङ्क at_flags_normalize_nofollow(पूर्णाङ्क flags) {
        जइ (FLAGS_SET(flags, AT_SYMLINK_FOLLOW)) {
                प्रतिज्ञा(!FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } अण्णहा
                flags |= AT_SYMLINK_NOFOLLOW;
        प्रत्यागम flags;
}

स्थिर अन्तःस्थ पूर्णाङ्क at_flags_normalize_follow(पूर्णाङ्क flags) {
        जइ (FLAGS_SET(flags, AT_SYMLINK_NOFOLLOW)) {
                प्रतिज्ञा(!FLAGS_SET(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } अण्णहा
                flags |= AT_SYMLINK_FOLLOW;
        प्रत्यागम flags;
}
