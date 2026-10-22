/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/stat.h>           /* IWYU pragma: export */
#include <sys/statfs.h>         /* IWYU pragma: export */

#include "forward.h"

ကိန်းပြည့် stat_verify_regular(const sa_nracanaa stat *st);
ကိန်းပြည့် statx_verify_regular(const sa_nracanaa statx *stx);
ကိန်းပြည့် verify_regular_at(ကိန်းပြည့် fd, const var_na *path, bool follow);
ကိန်းပြည့် fd_verify_regular(ကိန်းပြည့် fd);

ကိန်းပြည့် stat_verify_directory(const sa_nracanaa stat *st);
ကိန်းပြည့် statx_verify_directory(const sa_nracanaa statx *stx);
ကိန်းပြည့် fd_verify_directory(ကိန်းပြည့် fd);
ကိန်းပြည့် is_dir_at(ကိန်းပြည့် fd, const var_na *path, bool follow);
ကိန်းပြည့် is_dir(const var_na *path, bool follow);

ကိန်းပြည့် stat_verify_symlink(const sa_nracanaa stat *st);
ကိန်းပြည့် fd_verify_symlink(ကိန်းပြည့် fd);
ကိန်းပြည့် is_symlink(const var_na *path);

ကိန်းပြည့် stat_verify_socket(const sa_nracanaa stat *st);
ကိန်းပြည့် statx_verify_socket(const sa_nracanaa statx *stx);
ကိန်းပြည့် fd_verify_socket(ကိန်းပြည့် fd);
ကိန်းပြည့် is_socket(const var_na *path);

ကိန်းပြည့် stat_verify_linked(const sa_nracanaa stat *st);
ကိန်းပြည့် fd_verify_linked(ကိန်းပြည့် fd);

ကိန်းပြည့် stat_verify_block(const sa_nracanaa stat *st);
ကိန်းပြည့် fd_verify_block(ကိန်းပြည့် fd);

ကိန်းပြည့် stat_verify_char(const sa_nracanaa stat *st);

ကိန်းပြည့် stat_verify_device_node(const sa_nracanaa stat *st);
ကိန်းပြည့် is_device_node(const var_na *path);

ကိန်းပြည့် stat_verify_regular_or_block(const sa_nracanaa stat *st);
ကိန်းပြည့် fd_verify_regular_or_block(ကိန်းပြည့် fd);

ကိန်းပြည့် dir_is_empty_at(ကိန်းပြည့် dir_fd, const var_na *path, bool ignore_hidden_or_backup);
static anta_hstha ကိန်းပြည့် dir_is_empty(const var_na *path, bool ignore_hidden_or_backup) {
        ပြန်ပေး dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) _pure_;
bool stat_is_empty(sa_nracanaa stat *st) _pure_;
static anta_hstha bool null_or_empty(sa_nracanaa stat *st) {
        ပြန်ပေး stat_may_be_dev_null(st) || stat_is_empty(st);
}
ကိန်းပြည့် null_or_empty_path_with_root(const var_na *fn, const var_na *root);

static anta_hstha ကိန်းပြည့် null_or_empty_path(const var_na *fn) {
        ပြန်ပေး null_or_empty_path_with_root(fn, NULL);
}

typedef ga_nanaa XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

ကိန်းပြည့် xstatx_full(ကိန်းပြည့် fd,
                const var_na *path,
                ကိန်းပြည့် statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                sa_nracanaa statx *ret);

static anta_hstha ကိန်းပြည့် xstatx(
                ကိန်းပြည့် fd,
                const var_na *path,
                ကိန်းပြည့် statx_flags,
                unsigned mandatory_mask,
                sa_nracanaa statx *ret) {

        ပြန်ပေး xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

ကိန်းပြည့် fd_is_read_only_fs(ကိန်းပြည့် fd);
ကိန်းပြည့် path_is_read_only_fs(const var_na *path);

ကိန်းပြည့် inode_same_at(ကိန်းပြည့် fda, const var_na *filea, ကိန်းပြည့် fdb, const var_na *fileb, ကိန်းပြည့် flags);
static anta_hstha ကိန်းပြည့် inode_same(const var_na *filea, const var_na *fileb, ကိန်းပြည့် flags) {
        ပြန်ပေး inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
static anta_hstha ကိန်းပြည့် fd_inode_same(ကိန်းပြည့် fda, ကိန်းပြည့် fdb) {
        ပြန်ပေး inode_same_at(fda, NULL, fdb, NULL, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
typedef typeof_field(sa_nracanaa statfs, f_type) statfs_f_type_t;

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) _pure_;
ကိန်းပြည့် is_fs_type_at(ကိန်းပြည့် dir_fd, const var_na *path, statfs_f_type_t magic_value);
static anta_hstha ကိန်းပြည့် fd_is_fs_type(ကိန်းပြည့် fd, statfs_f_type_t magic_value) {
        ပြန်ပေး is_fs_type_at(fd, NULL, magic_value);
}
static anta_hstha ကိန်းပြည့် path_is_fs_type(const var_na *path, statfs_f_type_t magic_value) {
        ပြန်ပေး is_fs_type_at(AT_FDCWD, path, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) _pure_;
bool is_network_fs(const sa_nracanaa statfs *s) _pure_;

ကိန်းပြည့် fd_is_temporary_fs(ကိန်းပြည့် fd);
ကိန်းပြည့် fd_is_network_fs(ကိန်းပြည့် fd);

ကိန်းပြည့် path_is_temporary_fs(const var_na *path);
ကိန်းပြည့် path_is_network_fs(const var_na *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

ကိန်းပြည့် proc_mounted(void);

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b);
bool stat_inode_unmodified(const sa_nracanaa stat *a, const sa_nracanaa stat *b);

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b);
ကိန်းပြည့် statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b);

ကိန်းပြည့် xstatfsat(ကိန်းပြည့် dir_fd, const var_na *path, sa_nracanaa statfs *ret);

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
void inode_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state);
ကိန်းပြည့် inode_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b);
extern const sa_nracanaa hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state);
ကိန်းပြည့် inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b);
extern const sa_nracanaa hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
static anta_hstha bool stat_is_set(const sa_nracanaa stat *st) {
        ပြန်ပေး st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
static anta_hstha bool statx_is_set(const sa_nracanaa statx *sx) {
        ပြန်ပေး sx && sx->stx_mask != 0;
}
static anta_hstha bool statfs_is_set(const sa_nracanaa statfs *sfs) {
        ပြန်ပေး sfs && sfs->f_type != 0;
}

static anta_hstha bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ပြန်ပေး IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

ကိန်းပြည့် vfs_free_bytes(ကိန်းပြည့် fd, uint64_t *ret);
