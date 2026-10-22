/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/stat.h>           /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <sys/statfs.h>         /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ stat_verify_regular(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ statx_verify_regular(ಧ್ರುವ ಸಂರಚನಾ statx *stx);
ಪೂರ್ಣಾಙ್ಕ verify_regular_at(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, bool follow);
ಪೂರ್ಣಾಙ್ಕ fd_verify_regular(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ stat_verify_directory(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ statx_verify_directory(ಧ್ರುವ ಸಂರಚನಾ statx *stx);
ಪೂರ್ಣಾಙ್ಕ fd_verify_directory(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ is_dir_at(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, bool follow);
ಪೂರ್ಣಾಙ್ಕ is_dir(ಧ್ರುವ ವರ್ಣ *path, bool follow);

ಪೂರ್ಣಾಙ್ಕ stat_verify_symlink(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ fd_verify_symlink(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ is_symlink(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ stat_verify_socket(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ statx_verify_socket(ಧ್ರುವ ಸಂರಚನಾ statx *stx);
ಪೂರ್ಣಾಙ್ಕ fd_verify_socket(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ is_socket(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ stat_verify_linked(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ fd_verify_linked(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ stat_verify_block(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ fd_verify_block(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ stat_verify_char(ಧ್ರುವ ಸಂರಚನಾ stat *st);

ಪೂರ್ಣಾಙ್ಕ stat_verify_device_ಗಂಟು(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ is_device_ಗಂಟು(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ stat_verify_regular_or_block(ಧ್ರುವ ಸಂರಚನಾ stat *st);
ಪೂರ್ಣಾಙ್ಕ fd_verify_regular_or_block(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ dir_is_empty_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, bool ignore_hidden_or_backup);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ dir_is_empty(ಧ್ರುವ ವರ್ಣ *path, bool ignore_hidden_or_backup) {
        ಪ್ರತ್ಯಾಗಮ dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ಸಂರಚನಾ stat *st) _pure_;
bool stat_is_empty(ಸಂರಚನಾ stat *st) _pure_;
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool null_or_empty(ಸಂರಚನಾ stat *st) {
        ಪ್ರತ್ಯಾಗಮ stat_may_be_dev_null(st) || stat_is_empty(st);
}
ಪೂರ್ಣಾಙ್ಕ null_or_empty_path_with_ಬೇರು(ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *ಬೇರು);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ null_or_empty_path(ಧ್ರುವ ವರ್ಣ *fn) {
        ಪ್ರತ್ಯಾಗಮ null_or_empty_path_with_ಬೇರು(fn, ರಿಕ್ತ);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

ಪೂರ್ಣಾಙ್ಕ xstatx_full(ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ಸಂರಚನಾ statx *ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xstatx(
                ಪೂರ್ಣಾಙ್ಕ fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಪೂರ್ಣಾಙ್ಕ statx_flags,
                unsigned mandatory_mask,
                ಸಂರಚನಾ statx *ret) {

        ಪ್ರತ್ಯಾಗಮ xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

ಪೂರ್ಣಾಙ್ಕ fd_is_read_only_fs(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ path_is_read_only_fs(ಧ್ರುವ ವರ್ಣ *path);

ಪೂರ್ಣಾಙ್ಕ inode_same_at(ಪೂರ್ಣಾಙ್ಕ fda, ಧ್ರುವ ವರ್ಣ *filea, ಪೂರ್ಣಾಙ್ಕ fdb, ಧ್ರುವ ವರ್ಣ *fileb, ಪೂರ್ಣಾಙ್ಕ flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ inode_same(ಧ್ರುವ ವರ್ಣ *filea, ಧ್ರುವ ವರ್ಣ *fileb, ಪೂರ್ಣಾಙ್ಕ flags) {
        ಪ್ರತ್ಯಾಗಮ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fd_inode_same(ಪೂರ್ಣಾಙ್ಕ fda, ಪೂರ್ಣಾಙ್ಕ fdb) {
        ಪ್ರತ್ಯಾಗಮ inode_same_at(fda, ರಿಕ್ತ, fdb, ರಿಕ್ತ, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
ಪ್ರಕಾರನಿರ್ದೇಶ typeof_field(ಸಂರಚನಾ statfs, f_type) statfs_f_type_t;

bool is_fs_type(ಧ್ರುವ ಸಂರಚನಾ statfs *s, statfs_f_type_t magic_ಮೌಲ್ಯ) _pure_;
ಪೂರ್ಣಾಙ್ಕ is_fs_type_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, statfs_f_type_t magic_ಮೌಲ್ಯ);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fd_is_fs_type(ಪೂರ್ಣಾಙ್ಕ fd, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ is_fs_type_at(fd, ರಿಕ್ತ, magic_ಮೌಲ್ಯ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ path_is_fs_type(ಧ್ರುವ ವರ್ಣ *path, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ is_fs_type_at(AT_FDCWD, path, magic_ಮೌಲ್ಯ);
}

bool is_temporary_fs(ಧ್ರುವ ಸಂರಚನಾ statfs *s) _pure_;
bool is_network_fs(ಧ್ರುವ ಸಂರಚನಾ statfs *s) _pure_;

ಪೂರ್ಣಾಙ್ಕ fd_is_temporary_fs(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ fd_is_network_fs(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ path_is_temporary_fs(ಧ್ರುವ ವರ್ಣ *path);
ಪೂರ್ಣಾಙ್ಕ path_is_network_fs(ಧ್ರುವ ವರ್ಣ *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

ಪೂರ್ಣಾಙ್ಕ proc_mounted(ಶೂನ್ಯ);

bool stat_inode_same(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b);
bool stat_inode_unmodified(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b);

bool statx_inode_same(ಧ್ರುವ ಸಂರಚನಾ statx *a, ಧ್ರುವ ಸಂರಚನಾ statx *b);
ಪೂರ್ಣಾಙ್ಕ statx_mount_same(ಧ್ರುವ ಸಂರಚನಾ statx *a, ಧ್ರುವ ಸಂರಚನಾ statx *b);

ಪೂರ್ಣಾಙ್ಕ xstatfsat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಸಂರಚನಾ statfs *ret);

usec_t statx_timestamp_load(ಧ್ರುವ ಸಂರಚನಾ statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(ಧ್ರುವ ಸಂರಚನಾ statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
ಶೂನ್ಯ inode_hash_func(ಧ್ರುವ ಸಂರಚನಾ stat *q, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ inode_compare_func(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b);
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
ಶೂನ್ಯ inode_unmodified_hash_func(ಧ್ರುವ ಸಂರಚನಾ stat *q, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ inode_unmodified_compare_func(ಧ್ರುವ ಸಂರಚನಾ stat *a, ಧ್ರುವ ಸಂರಚನಾ stat *b);
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool stat_is_set(ಧ್ರುವ ಸಂರಚನಾ stat *st) {
        ಪ್ರತ್ಯಾಗಮ st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool statx_is_set(ಧ್ರುವ ಸಂರಚನಾ statx *sx) {
        ಪ್ರತ್ಯಾಗಮ sx && sx->stx_mask != 0;
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool statfs_is_set(ಧ್ರುವ ಸಂರಚನಾ statfs *sfs) {
        ಪ್ರತ್ಯಾಗಮ sfs && sfs->f_type != 0;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ಪ್ರತ್ಯಾಗಮ IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

ಪೂರ್ಣಾಙ್ಕ vfs_free_bytes(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);
