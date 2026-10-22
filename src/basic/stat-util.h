/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/stat.h>           /* IWYU pragma: export */
#include <sys/statfs.h>         /* IWYU pragma: export */

#include "forward.h"

цео stat_verify_regular(const struct stat *st);
цео statx_verify_regular(const struct statx *stx);
цео verify_regular_at(цео fd, const char *path, bool follow);
цео fd_verify_regular(цео fd);

цео stat_verify_directory(const struct stat *st);
цео statx_verify_directory(const struct statx *stx);
цео fd_verify_directory(цео fd);
цео is_dir_at(цео fd, const char *path, bool follow);
цео is_dir(const char *path, bool follow);

цео stat_verify_symlink(const struct stat *st);
цео fd_verify_symlink(цео fd);
цео is_symlink(const char *path);

цео stat_verify_socket(const struct stat *st);
цео statx_verify_socket(const struct statx *stx);
цео fd_verify_socket(цео fd);
цео is_socket(const char *path);

цео stat_verify_linked(const struct stat *st);
цео fd_verify_linked(цео fd);

цео stat_verify_block(const struct stat *st);
цео fd_verify_block(цео fd);

цео stat_verify_char(const struct stat *st);

цео stat_verify_device_node(const struct stat *st);
цео is_device_node(const char *path);

цео stat_verify_regular_or_block(const struct stat *st);
цео fd_verify_regular_or_block(цео fd);

цео dir_is_empty_at(цео dir_fd, const char *path, bool ignore_hidden_or_backup);
static inline цео dir_is_empty(const char *path, bool ignore_hidden_or_backup) {
        врати dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(struct stat *st) _pure_;
bool stat_is_empty(struct stat *st) _pure_;
static inline bool null_or_empty(struct stat *st) {
        врати stat_may_be_dev_null(st) || stat_is_empty(st);
}
цео null_or_empty_path_with_root(const char *fn, const char *root);

static inline цео null_or_empty_path(const char *fn) {
        врати null_or_empty_path_with_root(fn, NULL);
}

typedef enum XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

цео xstatx_full(цео fd,
                const char *path,
                цео statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                struct statx *ret);

static inline цео xstatx(
                цео fd,
                const char *path,
                цео statx_flags,
                unsigned mandatory_mask,
                struct statx *ret) {

        врати xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

цео fd_is_read_only_fs(цео fd);
цео path_is_read_only_fs(const char *path);

цео inode_same_at(цео fda, const char *filea, цео fdb, const char *fileb, цео flags);
static inline цео inode_same(const char *filea, const char *fileb, цео flags) {
        врати inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
static inline цео fd_inode_same(цео fda, цео fdb) {
        врати inode_same_at(fda, NULL, fdb, NULL, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
typedef typeof_field(struct statfs, f_type) statfs_f_type_t;

bool is_fs_type(const struct statfs *s, statfs_f_type_t magic_value) _pure_;
цео is_fs_type_at(цео dir_fd, const char *path, statfs_f_type_t magic_value);
static inline цео fd_is_fs_type(цео fd, statfs_f_type_t magic_value) {
        врати is_fs_type_at(fd, NULL, magic_value);
}
static inline цео path_is_fs_type(const char *path, statfs_f_type_t magic_value) {
        врати is_fs_type_at(AT_FDCWD, path, magic_value);
}

bool is_temporary_fs(const struct statfs *s) _pure_;
bool is_network_fs(const struct statfs *s) _pure_;

цео fd_is_temporary_fs(цео fd);
цео fd_is_network_fs(цео fd);

цео path_is_temporary_fs(const char *path);
цео path_is_network_fs(const char *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

цео proc_mounted(void);

bool stat_inode_same(const struct stat *a, const struct stat *b);
bool stat_inode_unmodified(const struct stat *a, const struct stat *b);

bool statx_inode_same(const struct statx *a, const struct statx *b);
цео statx_mount_same(const struct statx *a, const struct statx *b);

цео xstatfsat(цео dir_fd, const char *path, struct statfs *ret);

usec_t statx_timestamp_load(const struct statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(const struct statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
void inode_hash_func(const struct stat *q, struct siphash *state);
цео inode_compare_func(const struct stat *a, const struct stat *b);
extern const struct hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
void inode_unmodified_hash_func(const struct stat *q, struct siphash *state);
цео inode_unmodified_compare_func(const struct stat *a, const struct stat *b);
extern const struct hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
static inline bool stat_is_set(const struct stat *st) {
        врати st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
static inline bool statx_is_set(const struct statx *sx) {
        врати sx && sx->stx_mask != 0;
}
static inline bool statfs_is_set(const struct statfs *sfs) {
        врати sfs && sfs->f_type != 0;
}

static inline bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        врати IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

цео vfs_free_bytes(цео fd, uint64_t *ret);
