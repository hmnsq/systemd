/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/stat.h>           /* IWYU pragma: export */
#ଅନ୍ତର୍ଭାଵ <sys/statfs.h>         /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_regular(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_verify_regular(ଅଚଳ ସଂରଚନା statx *stx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା verify_regular_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, bool follow);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_regular(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_directory(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_verify_directory(ଅଚଳ ସଂରଚନା statx *stx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_directory(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_dir_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, bool follow);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_dir(ଅଚଳ ଅକ୍ଷର *path, bool follow);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_symlink(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_symlink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_symlink(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_socket(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_verify_socket(ଅଚଳ ସଂରଚନା statx *stx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_socket(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_socket(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_linked(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_linked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_block(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_block(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_char(ଅଚଳ ସଂରଚନା stat *st);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_device_ଗଣ୍ଠି(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_device_ଗଣ୍ଠି(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା stat_verify_regular_or_block(ଅଚଳ ସଂରଚନା stat *st);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify_regular_or_block(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_is_empty_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, bool ignore_hidden_or_backup);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_is_empty(ଅଚଳ ଅକ୍ଷର *path, bool ignore_hidden_or_backup) {
        ଫେରନ୍ତୁ dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ସଂରଚନା stat *st) _pure_;
bool stat_is_empty(ସଂରଚନା stat *st) _pure_;
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool null_or_empty(ସଂରଚନା stat *st) {
        ଫେରନ୍ତୁ stat_may_be_dev_null(st) || stat_is_empty(st);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା null_or_empty_path_with_ମୂଳ(ଅଚଳ ଅକ୍ଷର *fn, ଅଚଳ ଅକ୍ଷର *ମୂଳ);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା null_or_empty_path(ଅଚଳ ଅକ୍ଷର *fn) {
        ଫେରନ୍ତୁ null_or_empty_path_with_ମୂଳ(fn, ଖାଲି);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatx_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ସଂରଚନା statx *ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatx(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_flags,
                unsigned mandatory_mask,
                ସଂରଚନା statx *ret) {

        ଫେରନ୍ତୁ xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_read_only_fs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_read_only_fs(ଅଚଳ ଅକ୍ଷର *path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_same_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fda, ଅଚଳ ଅକ୍ଷର *filea, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdb, ଅଚଳ ଅକ୍ଷର *fileb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_same(ଅଚଳ ଅକ୍ଷର *filea, ଅଚଳ ଅକ୍ଷର *fileb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) {
        ଫେରନ୍ତୁ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_inode_same(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fda, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fdb) {
        ଫେରନ୍ତୁ inode_same_at(fda, ଖାଲି, fdb, ଖାଲି, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା typeof_field(ସଂରଚନା statfs, f_type) statfs_f_type_t;

bool is_fs_type(ଅଚଳ ସଂରଚନା statfs *s, statfs_f_type_t magic_ମୂଲ୍ୟ) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_fs_type_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, statfs_f_type_t magic_ମୂଲ୍ୟ);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_fs_type(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, statfs_f_type_t magic_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ is_fs_type_at(fd, ଖାଲି, magic_ମୂଲ୍ୟ);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_fs_type(ଅଚଳ ଅକ୍ଷର *path, statfs_f_type_t magic_ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ is_fs_type_at(AT_FDCWD, path, magic_ମୂଲ୍ୟ);
}

bool is_temporary_fs(ଅଚଳ ସଂରଚନା statfs *s) _pure_;
bool is_network_fs(ଅଚଳ ସଂରଚନା statfs *s) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_temporary_fs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_network_fs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_temporary_fs(ଅଚଳ ଅକ୍ଷର *path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା path_is_network_fs(ଅଚଳ ଅକ୍ଷର *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_mounted(ଶୂନ୍ୟ);

bool stat_inode_same(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b);
bool stat_inode_unmodified(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b);

bool statx_inode_same(ଅଚଳ ସଂରଚନା statx *a, ଅଚଳ ସଂରଚନା statx *b);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା statx_mount_same(ଅଚଳ ସଂରଚନା statx *a, ଅଚଳ ସଂରଚନା statx *b);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xstatfsat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ସଂରଚନା statfs *ret);

usec_t statx_timestamp_load(ଅଚଳ ସଂରଚନା statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(ଅଚଳ ସଂରଚନା statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
ଶୂନ୍ୟ inode_hash_func(ଅଚଳ ସଂରଚନା stat *q, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_compare_func(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b);
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
ଶୂନ୍ୟ inode_unmodified_hash_func(ଅଚଳ ସଂରଚନା stat *q, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_unmodified_compare_func(ଅଚଳ ସଂରଚନା stat *a, ଅଚଳ ସଂରଚନା stat *b);
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool stat_is_set(ଅଚଳ ସଂରଚନା stat *st) {
        ଫେରନ୍ତୁ st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool statx_is_set(ଅଚଳ ସଂରଚନା statx *sx) {
        ଫେରନ୍ତୁ sx && sx->stx_mask != 0;
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool statfs_is_set(ଅଚଳ ସଂରଚନା statfs *sfs) {
        ଫେରନ୍ତୁ sfs && sfs->f_type != 0;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ଫେରନ୍ତୁ IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vfs_free_bytes(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);
