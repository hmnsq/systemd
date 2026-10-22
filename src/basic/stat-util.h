/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/stat.h>           /* IWYU pragma: export */
#شامل <sys/statfs.h>         /* IWYU pragma: export */

#شامل "forward.h"

صحیح stat_verify_regular(ثابت ساختار stat *st);
صحیح statx_verify_regular(ثابت ساختار statx *stx);
صحیح verify_regular_at(صحیح fd, ثابت نویسه *path, bool follow);
صحیح fd_verify_regular(صحیح fd);

صحیح stat_verify_directory(ثابت ساختار stat *st);
صحیح statx_verify_directory(ثابت ساختار statx *stx);
صحیح fd_verify_directory(صحیح fd);
صحیح is_dir_at(صحیح fd, ثابت نویسه *path, bool follow);
صحیح is_dir(ثابت نویسه *path, bool follow);

صحیح stat_verify_symlink(ثابت ساختار stat *st);
صحیح fd_verify_symlink(صحیح fd);
صحیح is_symlink(ثابت نویسه *path);

صحیح stat_verify_socket(ثابت ساختار stat *st);
صحیح statx_verify_socket(ثابت ساختار statx *stx);
صحیح fd_verify_socket(صحیح fd);
صحیح is_socket(ثابت نویسه *path);

صحیح stat_verify_linked(ثابت ساختار stat *st);
صحیح fd_verify_linked(صحیح fd);

صحیح stat_verify_block(ثابت ساختار stat *st);
صحیح fd_verify_block(صحیح fd);

صحیح stat_verify_char(ثابت ساختار stat *st);

صحیح stat_verify_device_گره(ثابت ساختار stat *st);
صحیح is_device_گره(ثابت نویسه *path);

صحیح stat_verify_regular_or_block(ثابت ساختار stat *st);
صحیح fd_verify_regular_or_block(صحیح fd);

صحیح dir_is_empty_at(صحیح dir_fd, ثابت نویسه *path, bool ignore_hidden_or_backup);
ایستا درونخطی صحیح dir_is_empty(ثابت نویسه *path, bool ignore_hidden_or_backup) {
        بازگشت dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ساختار stat *st) _pure_;
bool stat_is_empty(ساختار stat *st) _pure_;
ایستا درونخطی bool null_or_empty(ساختار stat *st) {
        بازگشت stat_may_be_dev_null(st) || stat_is_empty(st);
}
صحیح null_or_empty_path_with_ریشه(ثابت نویسه *fn, ثابت نویسه *ریشه);

ایستا درونخطی صحیح null_or_empty_path(ثابت نویسه *fn) {
        بازگشت null_or_empty_path_with_ریشه(fn, پوچ);
}

تعریفنوع شمارش XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

صحیح xstatx_full(صحیح fd,
                ثابت نویسه *path,
                صحیح statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ساختار statx *ret);

ایستا درونخطی صحیح xstatx(
                صحیح fd,
                ثابت نویسه *path,
                صحیح statx_flags,
                unsigned mandatory_mask,
                ساختار statx *ret) {

        بازگشت xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

صحیح fd_is_read_only_fs(صحیح fd);
صحیح path_is_read_only_fs(ثابت نویسه *path);

صحیح inode_same_at(صحیح fda, ثابت نویسه *filea, صحیح fdb, ثابت نویسه *fileb, صحیح flags);
ایستا درونخطی صحیح inode_same(ثابت نویسه *filea, ثابت نویسه *fileb, صحیح flags) {
        بازگشت inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ایستا درونخطی صحیح fd_inode_same(صحیح fda, صحیح fdb) {
        بازگشت inode_same_at(fda, پوچ, fdb, پوچ, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
تعریفنوع typeof_field(ساختار statfs, f_type) statfs_f_type_t;

bool is_fs_type(ثابت ساختار statfs *s, statfs_f_type_t magic_مقدار) _pure_;
صحیح is_fs_type_at(صحیح dir_fd, ثابت نویسه *path, statfs_f_type_t magic_مقدار);
ایستا درونخطی صحیح fd_is_fs_type(صحیح fd, statfs_f_type_t magic_مقدار) {
        بازگشت is_fs_type_at(fd, پوچ, magic_مقدار);
}
ایستا درونخطی صحیح path_is_fs_type(ثابت نویسه *path, statfs_f_type_t magic_مقدار) {
        بازگشت is_fs_type_at(AT_FDCWD, path, magic_مقدار);
}

bool is_temporary_fs(ثابت ساختار statfs *s) _pure_;
bool is_network_fs(ثابت ساختار statfs *s) _pure_;

صحیح fd_is_temporary_fs(صحیح fd);
صحیح fd_is_network_fs(صحیح fd);

صحیح path_is_temporary_fs(ثابت نویسه *path);
صحیح path_is_network_fs(ثابت نویسه *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

صحیح proc_mounted(تهی);

bool stat_inode_same(ثابت ساختار stat *a, ثابت ساختار stat *b);
bool stat_inode_unmodified(ثابت ساختار stat *a, ثابت ساختار stat *b);

bool statx_inode_same(ثابت ساختار statx *a, ثابت ساختار statx *b);
صحیح statx_mount_same(ثابت ساختار statx *a, ثابت ساختار statx *b);

صحیح xstatfsat(صحیح dir_fd, ثابت نویسه *path, ساختار statfs *ret);

usec_t statx_timestamp_load(ثابت ساختار statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(ثابت ساختار statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
تهی inode_hash_func(ثابت ساختار stat *q, ساختار siphash *state);
صحیح inode_compare_func(ثابت ساختار stat *a, ثابت ساختار stat *b);
بیرونی ثابت ساختار hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
تهی inode_unmodified_hash_func(ثابت ساختار stat *q, ساختار siphash *state);
صحیح inode_unmodified_compare_func(ثابت ساختار stat *a, ثابت ساختار stat *b);
بیرونی ثابت ساختار hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ایستا درونخطی bool stat_is_set(ثابت ساختار stat *st) {
        بازگشت st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
ایستا درونخطی bool statx_is_set(ثابت ساختار statx *sx) {
        بازگشت sx && sx->stx_mask != 0;
}
ایستا درونخطی bool statfs_is_set(ثابت ساختار statfs *sfs) {
        بازگشت sfs && sfs->f_type != 0;
}

ایستا درونخطی bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        بازگشت IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

صحیح vfs_free_bytes(صحیح fd, uint64_t *ret);
