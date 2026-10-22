/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/stat.h>           /* IWYU pragma: export */
#அந்தர்பாவ <sys/statfs.h>         /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

முழு stat_verify_regular(மாறிலி கட்டமைப்பு stat *st);
முழு statx_verify_regular(மாறிலி கட்டமைப்பு statx *stx);
முழு verify_regular_at(முழு fd, மாறிலி எழுத்து *path, bool follow);
முழு fd_verify_regular(முழு fd);

முழு stat_verify_directory(மாறிலி கட்டமைப்பு stat *st);
முழு statx_verify_directory(மாறிலி கட்டமைப்பு statx *stx);
முழு fd_verify_directory(முழு fd);
முழு is_dir_at(முழு fd, மாறிலி எழுத்து *path, bool follow);
முழு is_dir(மாறிலி எழுத்து *path, bool follow);

முழு stat_verify_symlink(மாறிலி கட்டமைப்பு stat *st);
முழு fd_verify_symlink(முழு fd);
முழு is_symlink(மாறிலி எழுத்து *path);

முழு stat_verify_socket(மாறிலி கட்டமைப்பு stat *st);
முழு statx_verify_socket(மாறிலி கட்டமைப்பு statx *stx);
முழு fd_verify_socket(முழு fd);
முழு is_socket(மாறிலி எழுத்து *path);

முழு stat_verify_linked(மாறிலி கட்டமைப்பு stat *st);
முழு fd_verify_linked(முழு fd);

முழு stat_verify_block(மாறிலி கட்டமைப்பு stat *st);
முழு fd_verify_block(முழு fd);

முழு stat_verify_char(மாறிலி கட்டமைப்பு stat *st);

முழு stat_verify_device_முனை(மாறிலி கட்டமைப்பு stat *st);
முழு is_device_முனை(மாறிலி எழுத்து *path);

முழு stat_verify_regular_or_block(மாறிலி கட்டமைப்பு stat *st);
முழு fd_verify_regular_or_block(முழு fd);

முழு dir_is_empty_at(முழு dir_fd, மாறிலி எழுத்து *path, bool ignore_hidden_or_backup);
நிலையான உள்வரி முழு dir_is_empty(மாறிலி எழுத்து *path, bool ignore_hidden_or_backup) {
        திரும்பு dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(கட்டமைப்பு stat *st) _pure_;
bool stat_is_empty(கட்டமைப்பு stat *st) _pure_;
நிலையான உள்வரி bool null_or_empty(கட்டமைப்பு stat *st) {
        திரும்பு stat_may_be_dev_null(st) || stat_is_empty(st);
}
முழு null_or_empty_path_with_வேர்(மாறிலி எழுத்து *fn, மாறிலி எழுத்து *வேர்);

நிலையான உள்வரி முழு null_or_empty_path(மாறிலி எழுத்து *fn) {
        திரும்பு null_or_empty_path_with_வேர்(fn, வெற்று);
}

வகைவரை எண்ணிக்கை XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

முழு xstatx_full(முழு fd,
                மாறிலி எழுத்து *path,
                முழு statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                கட்டமைப்பு statx *ret);

நிலையான உள்வரி முழு xstatx(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு statx_flags,
                unsigned mandatory_mask,
                கட்டமைப்பு statx *ret) {

        திரும்பு xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

முழு fd_is_read_only_fs(முழு fd);
முழு path_is_read_only_fs(மாறிலி எழுத்து *path);

முழு inode_same_at(முழு fda, மாறிலி எழுத்து *filea, முழு fdb, மாறிலி எழுத்து *fileb, முழு flags);
நிலையான உள்வரி முழு inode_same(மாறிலி எழுத்து *filea, மாறிலி எழுத்து *fileb, முழு flags) {
        திரும்பு inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
நிலையான உள்வரி முழு fd_inode_same(முழு fda, முழு fdb) {
        திரும்பு inode_same_at(fda, வெற்று, fdb, வெற்று, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
வகைவரை typeof_field(கட்டமைப்பு statfs, f_type) statfs_f_type_t;

bool is_fs_type(மாறிலி கட்டமைப்பு statfs *s, statfs_f_type_t magic_மதிப்பு) _pure_;
முழு is_fs_type_at(முழு dir_fd, மாறிலி எழுத்து *path, statfs_f_type_t magic_மதிப்பு);
நிலையான உள்வரி முழு fd_is_fs_type(முழு fd, statfs_f_type_t magic_மதிப்பு) {
        திரும்பு is_fs_type_at(fd, வெற்று, magic_மதிப்பு);
}
நிலையான உள்வரி முழு path_is_fs_type(மாறிலி எழுத்து *path, statfs_f_type_t magic_மதிப்பு) {
        திரும்பு is_fs_type_at(AT_FDCWD, path, magic_மதிப்பு);
}

bool is_temporary_fs(மாறிலி கட்டமைப்பு statfs *s) _pure_;
bool is_network_fs(மாறிலி கட்டமைப்பு statfs *s) _pure_;

முழு fd_is_temporary_fs(முழு fd);
முழு fd_is_network_fs(முழு fd);

முழு path_is_temporary_fs(மாறிலி எழுத்து *path);
முழு path_is_network_fs(மாறிலி எழுத்து *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

முழு proc_mounted(வெறுமை);

bool stat_inode_same(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b);
bool stat_inode_unmodified(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b);

bool statx_inode_same(மாறிலி கட்டமைப்பு statx *a, மாறிலி கட்டமைப்பு statx *b);
முழு statx_mount_same(மாறிலி கட்டமைப்பு statx *a, மாறிலி கட்டமைப்பு statx *b);

முழு xstatfsat(முழு dir_fd, மாறிலி எழுத்து *path, கட்டமைப்பு statfs *ret);

usec_t statx_timestamp_load(மாறிலி கட்டமைப்பு statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(மாறிலி கட்டமைப்பு statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
வெறுமை inode_hash_func(மாறிலி கட்டமைப்பு stat *q, கட்டமைப்பு siphash *state);
முழு inode_compare_func(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b);
வெளி மாறிலி கட்டமைப்பு hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
வெறுமை inode_unmodified_hash_func(மாறிலி கட்டமைப்பு stat *q, கட்டமைப்பு siphash *state);
முழு inode_unmodified_compare_func(மாறிலி கட்டமைப்பு stat *a, மாறிலி கட்டமைப்பு stat *b);
வெளி மாறிலி கட்டமைப்பு hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
நிலையான உள்வரி bool stat_is_set(மாறிலி கட்டமைப்பு stat *st) {
        திரும்பு st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
நிலையான உள்வரி bool statx_is_set(மாறிலி கட்டமைப்பு statx *sx) {
        திரும்பு sx && sx->stx_mask != 0;
}
நிலையான உள்வரி bool statfs_is_set(மாறிலி கட்டமைப்பு statfs *sfs) {
        திரும்பு sfs && sfs->f_type != 0;
}

நிலையான உள்வரி bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        திரும்பு IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

முழு vfs_free_bytes(முழு fd, uint64_t *ret);
