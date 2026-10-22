/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/stat.h>           /* IWYU pragma: export */
#অন্তর্ভাব <sys/statfs.h>         /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

পূর্ণাঙ্ক stat_verify_regular(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক statx_verify_regular(ধ্রুব সংরচনা statx *stx);
পূর্ণাঙ্ক verify_regular_at(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, bool follow);
পূর্ণাঙ্ক fd_verify_regular(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক stat_verify_directory(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক statx_verify_directory(ধ্রুব সংরচনা statx *stx);
পূর্ণাঙ্ক fd_verify_directory(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক is_dir_at(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, bool follow);
পূর্ণাঙ্ক is_dir(ধ্রুব বর্ণ *path, bool follow);

পূর্ণাঙ্ক stat_verify_symlink(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক fd_verify_symlink(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক is_symlink(ধ্রুব বর্ণ *path);

পূর্ণাঙ্ক stat_verify_socket(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক statx_verify_socket(ধ্রুব সংরচনা statx *stx);
পূর্ণাঙ্ক fd_verify_socket(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক is_socket(ধ্রুব বর্ণ *path);

পূর্ণাঙ্ক stat_verify_linked(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক fd_verify_linked(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক stat_verify_block(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক fd_verify_block(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক stat_verify_char(ধ্রুব সংরচনা stat *st);

পূর্ণাঙ্ক stat_verify_device_গ্রন্থি(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক is_device_গ্রন্থি(ধ্রুব বর্ণ *path);

পূর্ণাঙ্ক stat_verify_regular_or_block(ধ্রুব সংরচনা stat *st);
পূর্ণাঙ্ক fd_verify_regular_or_block(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক dir_is_empty_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, bool ignore_hidden_or_backup);
স্থির অন্তঃস্থ পূর্ণাঙ্ক dir_is_empty(ধ্রুব বর্ণ *path, bool ignore_hidden_or_backup) {
        প্রত্যাগম dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(সংরচনা stat *st) _pure_;
bool stat_is_empty(সংরচনা stat *st) _pure_;
স্থির অন্তঃস্থ bool null_or_empty(সংরচনা stat *st) {
        প্রত্যাগম stat_may_be_dev_null(st) || stat_is_empty(st);
}
পূর্ণাঙ্ক null_or_empty_path_with_মূল(ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *মূল);

স্থির অন্তঃস্থ পূর্ণাঙ্ক null_or_empty_path(ধ্রুব বর্ণ *fn) {
        প্রত্যাগম null_or_empty_path_with_মূল(fn, রিক্ত);
}

প্রকারনির্দেশ গণনা XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

পূর্ণাঙ্ক xstatx_full(পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                সংরচনা statx *ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক xstatx(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক statx_flags,
                unsigned mandatory_mask,
                সংরচনা statx *ret) {

        প্রত্যাগম xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

পূর্ণাঙ্ক fd_is_read_only_fs(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক path_is_read_only_fs(ধ্রুব বর্ণ *path);

পূর্ণাঙ্ক inode_same_at(পূর্ণাঙ্ক fda, ধ্রুব বর্ণ *filea, পূর্ণাঙ্ক fdb, ধ্রুব বর্ণ *fileb, পূর্ণাঙ্ক flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক inode_same(ধ্রুব বর্ণ *filea, ধ্রুব বর্ণ *fileb, পূর্ণাঙ্ক flags) {
        প্রত্যাগম inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক fd_inode_same(পূর্ণাঙ্ক fda, পূর্ণাঙ্ক fdb) {
        প্রত্যাগম inode_same_at(fda, রিক্ত, fdb, রিক্ত, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
প্রকারনির্দেশ typeof_field(সংরচনা statfs, f_type) statfs_f_type_t;

bool is_fs_type(ধ্রুব সংরচনা statfs *s, statfs_f_type_t magic_মান) _pure_;
পূর্ণাঙ্ক is_fs_type_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, statfs_f_type_t magic_মান);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fd_is_fs_type(পূর্ণাঙ্ক fd, statfs_f_type_t magic_মান) {
        প্রত্যাগম is_fs_type_at(fd, রিক্ত, magic_মান);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক path_is_fs_type(ধ্রুব বর্ণ *path, statfs_f_type_t magic_মান) {
        প্রত্যাগম is_fs_type_at(AT_FDCWD, path, magic_মান);
}

bool is_temporary_fs(ধ্রুব সংরচনা statfs *s) _pure_;
bool is_network_fs(ধ্রুব সংরচনা statfs *s) _pure_;

পূর্ণাঙ্ক fd_is_temporary_fs(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক fd_is_network_fs(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক path_is_temporary_fs(ধ্রুব বর্ণ *path);
পূর্ণাঙ্ক path_is_network_fs(ধ্রুব বর্ণ *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

পূর্ণাঙ্ক proc_mounted(শূন্য);

bool stat_inode_same(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b);
bool stat_inode_unmodified(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b);

bool statx_inode_same(ধ্রুব সংরচনা statx *a, ধ্রুব সংরচনা statx *b);
পূর্ণাঙ্ক statx_mount_same(ধ্রুব সংরচনা statx *a, ধ্রুব সংরচনা statx *b);

পূর্ণাঙ্ক xstatfsat(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, সংরচনা statfs *ret);

usec_t statx_timestamp_load(ধ্রুব সংরচনা statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(ধ্রুব সংরচনা statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
শূন্য inode_hash_func(ধ্রুব সংরচনা stat *q, সংরচনা siphash *state);
পূর্ণাঙ্ক inode_compare_func(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b);
বাহ্য ধ্রুব সংরচনা hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
শূন্য inode_unmodified_hash_func(ধ্রুব সংরচনা stat *q, সংরচনা siphash *state);
পূর্ণাঙ্ক inode_unmodified_compare_func(ধ্রুব সংরচনা stat *a, ধ্রুব সংরচনা stat *b);
বাহ্য ধ্রুব সংরচনা hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
স্থির অন্তঃস্থ bool stat_is_set(ধ্রুব সংরচনা stat *st) {
        প্রত্যাগম st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
স্থির অন্তঃস্থ bool statx_is_set(ধ্রুব সংরচনা statx *sx) {
        প্রত্যাগম sx && sx->stx_mask != 0;
}
স্থির অন্তঃস্থ bool statfs_is_set(ধ্রুব সংরচনা statfs *sfs) {
        প্রত্যাগম sfs && sfs->f_type != 0;
}

স্থির অন্তঃস্থ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        প্রত্যাগম IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

পূর্ণাঙ্ক vfs_free_bytes(পূর্ণাঙ্ক fd, uint64_t *ret);
