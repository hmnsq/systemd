/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/stat.h>           /* IWYU pragma: export */
#include <sys/statfs.h>         /* IWYU pragma: export */

#include "forward.h"

puur_naa_nka stat_verify_regular(const sa_nracanaa stat *st);
puur_naa_nka statx_verify_regular(const sa_nracanaa statx *stx);
puur_naa_nka verify_regular_at(puur_naa_nka fd, const var_na *path, bool follow);
puur_naa_nka fd_verify_regular(puur_naa_nka fd);

puur_naa_nka stat_verify_directory(const sa_nracanaa stat *st);
puur_naa_nka statx_verify_directory(const sa_nracanaa statx *stx);
puur_naa_nka fd_verify_directory(puur_naa_nka fd);
puur_naa_nka is_dir_at(puur_naa_nka fd, const var_na *path, bool follow);
puur_naa_nka is_dir(const var_na *path, bool follow);

puur_naa_nka stat_verify_symlink(const sa_nracanaa stat *st);
puur_naa_nka fd_verify_symlink(puur_naa_nka fd);
puur_naa_nka is_symlink(const var_na *path);

puur_naa_nka stat_verify_socket(const sa_nracanaa stat *st);
puur_naa_nka statx_verify_socket(const sa_nracanaa statx *stx);
puur_naa_nka fd_verify_socket(puur_naa_nka fd);
puur_naa_nka is_socket(const var_na *path);

puur_naa_nka stat_verify_linked(const sa_nracanaa stat *st);
puur_naa_nka fd_verify_linked(puur_naa_nka fd);

puur_naa_nka stat_verify_block(const sa_nracanaa stat *st);
puur_naa_nka fd_verify_block(puur_naa_nka fd);

puur_naa_nka stat_verify_char(const sa_nracanaa stat *st);

puur_naa_nka stat_verify_device_node(const sa_nracanaa stat *st);
puur_naa_nka is_device_node(const var_na *path);

puur_naa_nka stat_verify_regular_or_block(const sa_nracanaa stat *st);
puur_naa_nka fd_verify_regular_or_block(puur_naa_nka fd);

puur_naa_nka dir_is_empty_at(puur_naa_nka dir_fd, const var_na *path, bool ignore_hidden_or_backup);
static anta_hstha puur_naa_nka dir_is_empty(const var_na *path, bool ignore_hidden_or_backup) {
        ᬯᬮᬶ dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(sa_nracanaa stat *st) _pure_;
bool stat_is_empty(sa_nracanaa stat *st) _pure_;
static anta_hstha bool null_or_empty(sa_nracanaa stat *st) {
        ᬯᬮᬶ stat_may_be_dev_null(st) || stat_is_empty(st);
}
puur_naa_nka null_or_empty_path_with_root(const var_na *fn, const var_na *root);

static anta_hstha puur_naa_nka null_or_empty_path(const var_na *fn) {
        ᬯᬮᬶ null_or_empty_path_with_root(fn, NULL);
}

typedef ga_nanaa XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

puur_naa_nka xstatx_full(puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                sa_nracanaa statx *ret);

static anta_hstha puur_naa_nka xstatx(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka statx_flags,
                unsigned mandatory_mask,
                sa_nracanaa statx *ret) {

        ᬯᬮᬶ xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

puur_naa_nka fd_is_read_only_fs(puur_naa_nka fd);
puur_naa_nka path_is_read_only_fs(const var_na *path);

puur_naa_nka inode_same_at(puur_naa_nka fda, const var_na *filea, puur_naa_nka fdb, const var_na *fileb, puur_naa_nka flags);
static anta_hstha puur_naa_nka inode_same(const var_na *filea, const var_na *fileb, puur_naa_nka flags) {
        ᬯᬮᬶ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
static anta_hstha puur_naa_nka fd_inode_same(puur_naa_nka fda, puur_naa_nka fdb) {
        ᬯᬮᬶ inode_same_at(fda, NULL, fdb, NULL, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
typedef typeof_field(sa_nracanaa statfs, f_type) statfs_f_type_t;

bool is_fs_type(const sa_nracanaa statfs *s, statfs_f_type_t magic_value) _pure_;
puur_naa_nka is_fs_type_at(puur_naa_nka dir_fd, const var_na *path, statfs_f_type_t magic_value);
static anta_hstha puur_naa_nka fd_is_fs_type(puur_naa_nka fd, statfs_f_type_t magic_value) {
        ᬯᬮᬶ is_fs_type_at(fd, NULL, magic_value);
}
static anta_hstha puur_naa_nka path_is_fs_type(const var_na *path, statfs_f_type_t magic_value) {
        ᬯᬮᬶ is_fs_type_at(AT_FDCWD, path, magic_value);
}

bool is_temporary_fs(const sa_nracanaa statfs *s) _pure_;
bool is_network_fs(const sa_nracanaa statfs *s) _pure_;

puur_naa_nka fd_is_temporary_fs(puur_naa_nka fd);
puur_naa_nka fd_is_network_fs(puur_naa_nka fd);

puur_naa_nka path_is_temporary_fs(const var_na *path);
puur_naa_nka path_is_network_fs(const var_na *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

puur_naa_nka proc_mounted(void);

bool stat_inode_same(const sa_nracanaa stat *a, const sa_nracanaa stat *b);
bool stat_inode_unmodified(const sa_nracanaa stat *a, const sa_nracanaa stat *b);

bool statx_inode_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b);
puur_naa_nka statx_mount_same(const sa_nracanaa statx *a, const sa_nracanaa statx *b);

puur_naa_nka xstatfsat(puur_naa_nka dir_fd, const var_na *path, sa_nracanaa statfs *ret);

usec_t statx_timestamp_load(const sa_nracanaa statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(const sa_nracanaa statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
void inode_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state);
puur_naa_nka inode_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b);
extern const sa_nracanaa hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
void inode_unmodified_hash_func(const sa_nracanaa stat *q, sa_nracanaa siphash *state);
puur_naa_nka inode_unmodified_compare_func(const sa_nracanaa stat *a, const sa_nracanaa stat *b);
extern const sa_nracanaa hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
static anta_hstha bool stat_is_set(const sa_nracanaa stat *st) {
        ᬯᬮᬶ st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
static anta_hstha bool statx_is_set(const sa_nracanaa statx *sx) {
        ᬯᬮᬶ sx && sx->stx_mask != 0;
}
static anta_hstha bool statfs_is_set(const sa_nracanaa statfs *sfs) {
        ᬯᬮᬶ sfs && sfs->f_type != 0;
}

static anta_hstha bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ᬯᬮᬶ IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

puur_naa_nka vfs_free_bytes(puur_naa_nka fd, uint64_t *ret);
