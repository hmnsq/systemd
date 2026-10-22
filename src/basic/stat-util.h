/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/stat.h>           /* IWYU pragma: export */
#अन्तर्भाव <sys/statfs.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णांक stat_verify_regular(स्थिरांक संरचना stat *st);
पूर्णांक statx_verify_regular(स्थिरांक संरचना statx *stx);
पूर्णांक verify_regular_at(पूर्णांक fd, स्थिरांक अक्षर *path, bool follow);
पूर्णांक fd_verify_regular(पूर्णांक fd);

पूर्णांक stat_verify_directory(स्थिरांक संरचना stat *st);
पूर्णांक statx_verify_directory(स्थिरांक संरचना statx *stx);
पूर्णांक fd_verify_directory(पूर्णांक fd);
पूर्णांक is_dir_at(पूर्णांक fd, स्थिरांक अक्षर *path, bool follow);
पूर्णांक is_dir(स्थिरांक अक्षर *path, bool follow);

पूर्णांक stat_verify_symlink(स्थिरांक संरचना stat *st);
पूर्णांक fd_verify_symlink(पूर्णांक fd);
पूर्णांक is_symlink(स्थिरांक अक्षर *path);

पूर्णांक stat_verify_socket(स्थिरांक संरचना stat *st);
पूर्णांक statx_verify_socket(स्थिरांक संरचना statx *stx);
पूर्णांक fd_verify_socket(पूर्णांक fd);
पूर्णांक is_socket(स्थिरांक अक्षर *path);

पूर्णांक stat_verify_linked(स्थिरांक संरचना stat *st);
पूर्णांक fd_verify_linked(पूर्णांक fd);

पूर्णांक stat_verify_block(स्थिरांक संरचना stat *st);
पूर्णांक fd_verify_block(पूर्णांक fd);

पूर्णांक stat_verify_char(स्थिरांक संरचना stat *st);

पूर्णांक stat_verify_device_गाठ(स्थिरांक संरचना stat *st);
पूर्णांक is_device_गाठ(स्थिरांक अक्षर *path);

पूर्णांक stat_verify_regular_or_block(स्थिरांक संरचना stat *st);
पूर्णांक fd_verify_regular_or_block(पूर्णांक fd);

पूर्णांक dir_is_empty_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, bool ignore_hidden_or_backup);
स्थिर ओळीत पूर्णांक dir_is_empty(स्थिरांक अक्षर *path, bool ignore_hidden_or_backup) {
        परत dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(संरचना stat *st) _pure_;
bool stat_is_empty(संरचना stat *st) _pure_;
स्थिर ओळीत bool null_or_empty(संरचना stat *st) {
        परत stat_may_be_dev_null(st) || stat_is_empty(st);
}
पूर्णांक null_or_empty_path_with_मूळ(स्थिरांक अक्षर *fn, स्थिरांक अक्षर *मूळ);

स्थिर ओळीत पूर्णांक null_or_empty_path(स्थिरांक अक्षर *fn) {
        परत null_or_empty_path_with_मूळ(fn, रिक्त);
}

प्रकारव्याख्या गणना XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

पूर्णांक xstatx_full(पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक statx_flags,
                XStatXFlags xstatx_flags,
                अचिन्हित mandatory_mask,
                अचिन्हित optional_mask,
                uint64_t mandatory_attributes,
                संरचना statx *ret);

स्थिर ओळीत पूर्णांक xstatx(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक statx_flags,
                अचिन्हित mandatory_mask,
                संरचना statx *ret) {

        परत xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

पूर्णांक fd_is_read_only_fs(पूर्णांक fd);
पूर्णांक path_is_read_only_fs(स्थिरांक अक्षर *path);

पूर्णांक inode_same_at(पूर्णांक fda, स्थिरांक अक्षर *filea, पूर्णांक fdb, स्थिरांक अक्षर *fileb, पूर्णांक flags);
स्थिर ओळीत पूर्णांक inode_same(स्थिरांक अक्षर *filea, स्थिरांक अक्षर *fileb, पूर्णांक flags) {
        परत inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
स्थिर ओळीत पूर्णांक fd_inode_same(पूर्णांक fda, पूर्णांक fdb) {
        परत inode_same_at(fda, रिक्त, fdb, रिक्त, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
प्रकारव्याख्या typeof_field(संरचना statfs, f_type) statfs_f_type_t;

bool is_fs_type(स्थिरांक संरचना statfs *s, statfs_f_type_t magic_मूल्य) _pure_;
पूर्णांक is_fs_type_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, statfs_f_type_t magic_मूल्य);
स्थिर ओळीत पूर्णांक fd_is_fs_type(पूर्णांक fd, statfs_f_type_t magic_मूल्य) {
        परत is_fs_type_at(fd, रिक्त, magic_मूल्य);
}
स्थिर ओळीत पूर्णांक path_is_fs_type(स्थिरांक अक्षर *path, statfs_f_type_t magic_मूल्य) {
        परत is_fs_type_at(AT_FDCWD, path, magic_मूल्य);
}

bool is_temporary_fs(स्थिरांक संरचना statfs *s) _pure_;
bool is_network_fs(स्थिरांक संरचना statfs *s) _pure_;

पूर्णांक fd_is_temporary_fs(पूर्णांक fd);
पूर्णांक fd_is_network_fs(पूर्णांक fd);

पूर्णांक path_is_temporary_fs(स्थिरांक अक्षर *path);
पूर्णांक path_is_network_fs(स्थिरांक अक्षर *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

पूर्णांक proc_mounted(शून्य);

bool stat_inode_same(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b);
bool stat_inode_unmodified(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b);

bool statx_inode_same(स्थिरांक संरचना statx *a, स्थिरांक संरचना statx *b);
पूर्णांक statx_mount_same(स्थिरांक संरचना statx *a, स्थिरांक संरचना statx *b);

पूर्णांक xstatfsat(पूर्णांक dir_fd, स्थिरांक अक्षर *path, संरचना statfs *ret);

usec_t statx_timestamp_load(स्थिरांक संरचना statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(स्थिरांक संरचना statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
शून्य inode_hash_func(स्थिरांक संरचना stat *q, संरचना siphash *state);
पूर्णांक inode_compare_func(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b);
बाह्य स्थिरांक संरचना hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
शून्य inode_unmodified_hash_func(स्थिरांक संरचना stat *q, संरचना siphash *state);
पूर्णांक inode_unmodified_compare_func(स्थिरांक संरचना stat *a, स्थिरांक संरचना stat *b);
बाह्य स्थिरांक संरचना hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
स्थिर ओळीत bool stat_is_set(स्थिरांक संरचना stat *st) {
        परत st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
स्थिर ओळीत bool statx_is_set(स्थिरांक संरचना statx *sx) {
        परत sx && sx->stx_mask != 0;
}
स्थिर ओळीत bool statfs_is_set(स्थिरांक संरचना statfs *sfs) {
        परत sfs && sfs->f_type != 0;
}

स्थिर ओळीत bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        परत IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

पूर्णांक vfs_free_bytes(पूर्णांक fd, uint64_t *ret);
