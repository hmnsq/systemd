/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/stat.h>           /* IWYU pragma: export */
#অন্তর্ভাব <sys/statfs.h>         /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা stat_verify_regular(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা statx_verify_regular(ধ্রুবক কাঠামো statx *stx);
পূর্ণসংখ্যা verify_regular_at(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, bool follow);
পূর্ণসংখ্যা fd_verify_regular(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা stat_verify_directory(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা statx_verify_directory(ধ্রুবক কাঠামো statx *stx);
পূর্ণসংখ্যা fd_verify_directory(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা is_dir_at(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, bool follow);
পূর্ণসংখ্যা is_dir(ধ্রুবক অক্ষর *path, bool follow);

পূর্ণসংখ্যা stat_verify_symlink(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা fd_verify_symlink(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা is_symlink(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা stat_verify_socket(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা statx_verify_socket(ধ্রুবক কাঠামো statx *stx);
পূর্ণসংখ্যা fd_verify_socket(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা is_socket(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা stat_verify_linked(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা fd_verify_linked(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা stat_verify_block(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা fd_verify_block(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা stat_verify_char(ধ্রুবক কাঠামো stat *st);

পূর্ণসংখ্যা stat_verify_device_গ্রন্থি(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা is_device_গ্রন্থি(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা stat_verify_regular_or_block(ধ্রুবক কাঠামো stat *st);
পূর্ণসংখ্যা fd_verify_regular_or_block(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা dir_is_empty_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, bool ignore_hidden_or_backup);
স্থির অন্তঃরেখ পূর্ণসংখ্যা dir_is_empty(ধ্রুবক অক্ষর *path, bool ignore_hidden_or_backup) {
        ফেরত dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(কাঠামো stat *st) _pure_;
bool stat_is_empty(কাঠামো stat *st) _pure_;
স্থির অন্তঃরেখ bool null_or_empty(কাঠামো stat *st) {
        ফেরত stat_may_be_dev_null(st) || stat_is_empty(st);
}
পূর্ণসংখ্যা null_or_empty_path_with_মূল(ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *মূল);

স্থির অন্তঃরেখ পূর্ণসংখ্যা null_or_empty_path(ধ্রুবক অক্ষর *fn) {
        ফেরত null_or_empty_path_with_মূল(fn, ফাঁকা);
}

প্রকারলেখ গণনা XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

পূর্ণসংখ্যা xstatx_full(পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                কাঠামো statx *ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা xstatx(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা statx_flags,
                unsigned mandatory_mask,
                কাঠামো statx *ret) {

        ফেরত xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

পূর্ণসংখ্যা fd_is_read_only_fs(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা path_is_read_only_fs(ধ্রুবক অক্ষর *path);

পূর্ণসংখ্যা inode_same_at(পূর্ণসংখ্যা fda, ধ্রুবক অক্ষর *filea, পূর্ণসংখ্যা fdb, ধ্রুবক অক্ষর *fileb, পূর্ণসংখ্যা flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা inode_same(ধ্রুবক অক্ষর *filea, ধ্রুবক অক্ষর *fileb, পূর্ণসংখ্যা flags) {
        ফেরত inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_inode_same(পূর্ণসংখ্যা fda, পূর্ণসংখ্যা fdb) {
        ফেরত inode_same_at(fda, ফাঁকা, fdb, ফাঁকা, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
প্রকারলেখ typeof_field(কাঠামো statfs, f_type) statfs_f_type_t;

bool is_fs_type(ধ্রুবক কাঠামো statfs *s, statfs_f_type_t magic_মান) _pure_;
পূর্ণসংখ্যা is_fs_type_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, statfs_f_type_t magic_মান);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_is_fs_type(পূর্ণসংখ্যা fd, statfs_f_type_t magic_মান) {
        ফেরত is_fs_type_at(fd, ফাঁকা, magic_মান);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা path_is_fs_type(ধ্রুবক অক্ষর *path, statfs_f_type_t magic_মান) {
        ফেরত is_fs_type_at(AT_FDCWD, path, magic_মান);
}

bool is_temporary_fs(ধ্রুবক কাঠামো statfs *s) _pure_;
bool is_network_fs(ধ্রুবক কাঠামো statfs *s) _pure_;

পূর্ণসংখ্যা fd_is_temporary_fs(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা fd_is_network_fs(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা path_is_temporary_fs(ধ্রুবক অক্ষর *path);
পূর্ণসংখ্যা path_is_network_fs(ধ্রুবক অক্ষর *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

পূর্ণসংখ্যা proc_mounted(শূন্য);

bool stat_inode_same(ধ্রুবক কাঠামো stat *a, ধ্রুবক কাঠামো stat *b);
bool stat_inode_unmodified(ধ্রুবক কাঠামো stat *a, ধ্রুবক কাঠামো stat *b);

bool statx_inode_same(ধ্রুবক কাঠামো statx *a, ধ্রুবক কাঠামো statx *b);
পূর্ণসংখ্যা statx_mount_same(ধ্রুবক কাঠামো statx *a, ধ্রুবক কাঠামো statx *b);

পূর্ণসংখ্যা xstatfsat(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, কাঠামো statfs *ret);

usec_t statx_timestamp_load(ধ্রুবক কাঠামো statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(ধ্রুবক কাঠামো statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
শূন্য inode_hash_func(ধ্রুবক কাঠামো stat *q, কাঠামো siphash *state);
পূর্ণসংখ্যা inode_compare_func(ধ্রুবক কাঠামো stat *a, ধ্রুবক কাঠামো stat *b);
বাহ্যিক ধ্রুবক কাঠামো hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
শূন্য inode_unmodified_hash_func(ধ্রুবক কাঠামো stat *q, কাঠামো siphash *state);
পূর্ণসংখ্যা inode_unmodified_compare_func(ধ্রুবক কাঠামো stat *a, ধ্রুবক কাঠামো stat *b);
বাহ্যিক ধ্রুবক কাঠামো hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
স্থির অন্তঃরেখ bool stat_is_set(ধ্রুবক কাঠামো stat *st) {
        ফেরত st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
স্থির অন্তঃরেখ bool statx_is_set(ধ্রুবক কাঠামো statx *sx) {
        ফেরত sx && sx->stx_mask != 0;
}
স্থির অন্তঃরেখ bool statfs_is_set(ধ্রুবক কাঠামো statfs *sfs) {
        ফেরত sfs && sfs->f_type != 0;
}

স্থির অন্তঃরেখ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ফেরত IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

পূর্ণসংখ্যা vfs_free_bytes(পূর্ণসংখ্যা fd, uint64_t *ret);
