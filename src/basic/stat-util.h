/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/stat.h>           /* IWYU pragma: export */
#include <sys/statfs.h>         /* IWYU pragma: export */

#include "forward.h"

整数 stat_verify_regular(const struct stat *st);
整数 statx_verify_regular(const struct statx *stx);
整数 verify_regular_at(整数 fd, const char *path, bool follow);
整数 fd_verify_regular(整数 fd);

整数 stat_verify_directory(const struct stat *st);
整数 statx_verify_directory(const struct statx *stx);
整数 fd_verify_directory(整数 fd);
整数 is_dir_at(整数 fd, const char *path, bool follow);
整数 is_dir(const char *path, bool follow);

整数 stat_verify_symlink(const struct stat *st);
整数 fd_verify_symlink(整数 fd);
整数 is_symlink(const char *path);

整数 stat_verify_socket(const struct stat *st);
整数 statx_verify_socket(const struct statx *stx);
整数 fd_verify_socket(整数 fd);
整数 is_socket(const char *path);

整数 stat_verify_linked(const struct stat *st);
整数 fd_verify_linked(整数 fd);

整数 stat_verify_block(const struct stat *st);
整数 fd_verify_block(整数 fd);

整数 stat_verify_char(const struct stat *st);

整数 stat_verify_device_node(const struct stat *st);
整数 is_device_node(const char *path);

整数 stat_verify_regular_or_block(const struct stat *st);
整数 fd_verify_regular_or_block(整数 fd);

整数 dir_is_empty_at(整数 dir_fd, const char *path, bool ignore_hidden_or_backup);
static inline 整数 dir_is_empty(const char *path, bool ignore_hidden_or_backup) {
        返回 dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(struct stat *st) _pure_;
bool stat_is_empty(struct stat *st) _pure_;
static inline bool null_or_empty(struct stat *st) {
        返回 stat_may_be_dev_null(st) || stat_is_empty(st);
}
整数 null_or_empty_path_with_root(const char *fn, const char *root);

static inline 整数 null_or_empty_path(const char *fn) {
        返回 null_or_empty_path_with_root(fn, NULL);
}

typedef enum XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

整数 xstatx_full(整数 fd,
                const char *path,
                整数 statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                struct statx *ret);

static inline 整数 xstatx(
                整数 fd,
                const char *path,
                整数 statx_flags,
                unsigned mandatory_mask,
                struct statx *ret) {

        返回 xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

整数 fd_is_read_only_fs(整数 fd);
整数 path_is_read_only_fs(const char *path);

整数 inode_same_at(整数 fda, const char *filea, 整数 fdb, const char *fileb, 整数 flags);
static inline 整数 inode_same(const char *filea, const char *fileb, 整数 flags) {
        返回 inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
static inline 整数 fd_inode_same(整数 fda, 整数 fdb) {
        返回 inode_same_at(fda, NULL, fdb, NULL, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
typedef typeof_field(struct statfs, f_type) statfs_f_type_t;

bool is_fs_type(const struct statfs *s, statfs_f_type_t magic_value) _pure_;
整数 is_fs_type_at(整数 dir_fd, const char *path, statfs_f_type_t magic_value);
static inline 整数 fd_is_fs_type(整数 fd, statfs_f_type_t magic_value) {
        返回 is_fs_type_at(fd, NULL, magic_value);
}
static inline 整数 path_is_fs_type(const char *path, statfs_f_type_t magic_value) {
        返回 is_fs_type_at(AT_FDCWD, path, magic_value);
}

bool is_temporary_fs(const struct statfs *s) _pure_;
bool is_network_fs(const struct statfs *s) _pure_;

整数 fd_is_temporary_fs(整数 fd);
整数 fd_is_network_fs(整数 fd);

整数 path_is_temporary_fs(const char *path);
整数 path_is_network_fs(const char *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

整数 proc_mounted(void);

bool stat_inode_same(const struct stat *a, const struct stat *b);
bool stat_inode_unmodified(const struct stat *a, const struct stat *b);

bool statx_inode_same(const struct statx *a, const struct statx *b);
整数 statx_mount_same(const struct statx *a, const struct statx *b);

整数 xstatfsat(整数 dir_fd, const char *path, struct statfs *ret);

usec_t statx_timestamp_load(const struct statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(const struct statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
void inode_hash_func(const struct stat *q, struct siphash *state);
整数 inode_compare_func(const struct stat *a, const struct stat *b);
extern const struct hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
void inode_unmodified_hash_func(const struct stat *q, struct siphash *state);
整数 inode_unmodified_compare_func(const struct stat *a, const struct stat *b);
extern const struct hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
static inline bool stat_is_set(const struct stat *st) {
        返回 st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
static inline bool statx_is_set(const struct statx *sx) {
        返回 sx && sx->stx_mask != 0;
}
static inline bool statfs_is_set(const struct statfs *sfs) {
        返回 sfs && sfs->f_type != 0;
}

static inline bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        返回 IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

整数 vfs_free_bytes(整数 fd, uint64_t *ret);
