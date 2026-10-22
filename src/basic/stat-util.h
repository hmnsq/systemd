/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/stat.h>           /* IWYU pragma: export */
#ضمن <sys/statfs.h>         /* IWYU pragma: export */

#ضمن "forward.h"

صحيح stat_verify_regular(ثابت بنية stat *st);
صحيح statx_verify_regular(ثابت بنية statx *stx);
صحيح verify_regular_at(صحيح fd, ثابت حرف *path, bool follow);
صحيح fd_verify_regular(صحيح fd);

صحيح stat_verify_directory(ثابت بنية stat *st);
صحيح statx_verify_directory(ثابت بنية statx *stx);
صحيح fd_verify_directory(صحيح fd);
صحيح is_dir_at(صحيح fd, ثابت حرف *path, bool follow);
صحيح is_dir(ثابت حرف *path, bool follow);

صحيح stat_verify_symlink(ثابت بنية stat *st);
صحيح fd_verify_symlink(صحيح fd);
صحيح is_symlink(ثابت حرف *path);

صحيح stat_verify_socket(ثابت بنية stat *st);
صحيح statx_verify_socket(ثابت بنية statx *stx);
صحيح fd_verify_socket(صحيح fd);
صحيح is_socket(ثابت حرف *path);

صحيح stat_verify_linked(ثابت بنية stat *st);
صحيح fd_verify_linked(صحيح fd);

صحيح stat_verify_block(ثابت بنية stat *st);
صحيح fd_verify_block(صحيح fd);

صحيح stat_verify_char(ثابت بنية stat *st);

صحيح stat_verify_device_گرہ(ثابت بنية stat *st);
صحيح is_device_گرہ(ثابت حرف *path);

صحيح stat_verify_regular_or_block(ثابت بنية stat *st);
صحيح fd_verify_regular_or_block(صحيح fd);

صحيح dir_is_empty_at(صحيح dir_fd, ثابت حرف *path, bool ignore_hidden_or_backup);
ساكن مضمن صحيح dir_is_empty(ثابت حرف *path, bool ignore_hidden_or_backup) {
        أرجع dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(بنية stat *st) _pure_;
bool stat_is_empty(بنية stat *st) _pure_;
ساكن مضمن bool null_or_empty(بنية stat *st) {
        أرجع stat_may_be_dev_null(st) || stat_is_empty(st);
}
صحيح null_or_empty_path_with_جڑ(ثابت حرف *fn, ثابت حرف *جڑ);

ساكن مضمن صحيح null_or_empty_path(ثابت حرف *fn) {
        أرجع null_or_empty_path_with_جڑ(fn, عدم);
}

تعريفنوع تعداد XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

صحيح xstatx_full(صحيح fd,
                ثابت حرف *path,
                صحيح statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                بنية statx *ret);

ساكن مضمن صحيح xstatx(
                صحيح fd,
                ثابت حرف *path,
                صحيح statx_flags,
                unsigned mandatory_mask,
                بنية statx *ret) {

        أرجع xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

صحيح fd_is_read_only_fs(صحيح fd);
صحيح path_is_read_only_fs(ثابت حرف *path);

صحيح inode_same_at(صحيح fda, ثابت حرف *filea, صحيح fdb, ثابت حرف *fileb, صحيح flags);
ساكن مضمن صحيح inode_same(ثابت حرف *filea, ثابت حرف *fileb, صحيح flags) {
        أرجع inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ساكن مضمن صحيح fd_inode_same(صحيح fda, صحيح fdb) {
        أرجع inode_same_at(fda, عدم, fdb, عدم, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
تعريفنوع typeof_field(بنية statfs, f_type) statfs_f_type_t;

bool is_fs_type(ثابت بنية statfs *s, statfs_f_type_t magic_قدر) _pure_;
صحيح is_fs_type_at(صحيح dir_fd, ثابت حرف *path, statfs_f_type_t magic_قدر);
ساكن مضمن صحيح fd_is_fs_type(صحيح fd, statfs_f_type_t magic_قدر) {
        أرجع is_fs_type_at(fd, عدم, magic_قدر);
}
ساكن مضمن صحيح path_is_fs_type(ثابت حرف *path, statfs_f_type_t magic_قدر) {
        أرجع is_fs_type_at(AT_FDCWD, path, magic_قدر);
}

bool is_temporary_fs(ثابت بنية statfs *s) _pure_;
bool is_network_fs(ثابت بنية statfs *s) _pure_;

صحيح fd_is_temporary_fs(صحيح fd);
صحيح fd_is_network_fs(صحيح fd);

صحيح path_is_temporary_fs(ثابت حرف *path);
صحيح path_is_network_fs(ثابت حرف *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

صحيح proc_mounted(فراغ);

bool stat_inode_same(ثابت بنية stat *a, ثابت بنية stat *b);
bool stat_inode_unmodified(ثابت بنية stat *a, ثابت بنية stat *b);

bool statx_inode_same(ثابت بنية statx *a, ثابت بنية statx *b);
صحيح statx_mount_same(ثابت بنية statx *a, ثابت بنية statx *b);

صحيح xstatfsat(صحيح dir_fd, ثابت حرف *path, بنية statfs *ret);

usec_t statx_timestamp_load(ثابت بنية statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(ثابت بنية statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
فراغ inode_hash_func(ثابت بنية stat *q, بنية siphash *state);
صحيح inode_compare_func(ثابت بنية stat *a, ثابت بنية stat *b);
خارجي ثابت بنية hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
فراغ inode_unmodified_hash_func(ثابت بنية stat *q, بنية siphash *state);
صحيح inode_unmodified_compare_func(ثابت بنية stat *a, ثابت بنية stat *b);
خارجي ثابت بنية hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ساكن مضمن bool stat_is_set(ثابت بنية stat *st) {
        أرجع st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
ساكن مضمن bool statx_is_set(ثابت بنية statx *sx) {
        أرجع sx && sx->stx_mask != 0;
}
ساكن مضمن bool statfs_is_set(ثابت بنية statfs *sfs) {
        أرجع sfs && sfs->f_type != 0;
}

ساكن مضمن bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        أرجع IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

صحيح vfs_free_bytes(صحيح fd, uint64_t *ret);
