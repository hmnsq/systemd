/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/stat.h>           /* IWYU pragma: export */
#અન્તર્ભાવ <sys/statfs.h>         /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક stat_verify_regular(અચલ સંરચના stat *st);
પૂર્ણાંક statx_verify_regular(અચલ સંરચના statx *stx);
પૂર્ણાંક verify_regular_at(પૂર્ણાંક fd, અચલ અક્ષર *path, bool follow);
પૂર્ણાંક fd_verify_regular(પૂર્ણાંક fd);

પૂર્ણાંક stat_verify_directory(અચલ સંરચના stat *st);
પૂર્ણાંક statx_verify_directory(અચલ સંરચના statx *stx);
પૂર્ણાંક fd_verify_directory(પૂર્ણાંક fd);
પૂર્ણાંક is_dir_at(પૂર્ણાંક fd, અચલ અક્ષર *path, bool follow);
પૂર્ણાંક is_dir(અચલ અક્ષર *path, bool follow);

પૂર્ણાંક stat_verify_symlink(અચલ સંરચના stat *st);
પૂર્ણાંક fd_verify_symlink(પૂર્ણાંક fd);
પૂર્ણાંક is_symlink(અચલ અક્ષર *path);

પૂર્ણાંક stat_verify_socket(અચલ સંરચના stat *st);
પૂર્ણાંક statx_verify_socket(અચલ સંરચના statx *stx);
પૂર્ણાંક fd_verify_socket(પૂર્ણાંક fd);
પૂર્ણાંક is_socket(અચલ અક્ષર *path);

પૂર્ણાંક stat_verify_linked(અચલ સંરચના stat *st);
પૂર્ણાંક fd_verify_linked(પૂર્ણાંક fd);

પૂર્ણાંક stat_verify_block(અચલ સંરચના stat *st);
પૂર્ણાંક fd_verify_block(પૂર્ણાંક fd);

પૂર્ણાંક stat_verify_char(અચલ સંરચના stat *st);

પૂર્ણાંક stat_verify_device_ગાંઠ(અચલ સંરચના stat *st);
પૂર્ણાંક is_device_ગાંઠ(અચલ અક્ષર *path);

પૂર્ણાંક stat_verify_regular_or_block(અચલ સંરચના stat *st);
પૂર્ણાંક fd_verify_regular_or_block(પૂર્ણાંક fd);

પૂર્ણાંક dir_is_empty_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, bool ignore_hidden_or_backup);
સ્થિર અંતઃરેખ પૂર્ણાંક dir_is_empty(અચલ અક્ષર *path, bool ignore_hidden_or_backup) {
        પાછા dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(સંરચના stat *st) _pure_;
bool stat_is_empty(સંરચના stat *st) _pure_;
સ્થિર અંતઃરેખ bool null_or_empty(સંરચના stat *st) {
        પાછા stat_may_be_dev_null(st) || stat_is_empty(st);
}
પૂર્ણાંક null_or_empty_path_with_મૂળ(અચલ અક્ષર *fn, અચલ અક્ષર *મૂળ);

સ્થિર અંતઃરેખ પૂર્ણાંક null_or_empty_path(અચલ અક્ષર *fn) {
        પાછા null_or_empty_path_with_મૂળ(fn, ખાલી);
}

પ્રકારવ્યાખ્યા ગણના XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

પૂર્ણાંક xstatx_full(પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                સંરચના statx *ret);

સ્થિર અંતઃરેખ પૂર્ણાંક xstatx(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક statx_flags,
                unsigned mandatory_mask,
                સંરચના statx *ret) {

        પાછા xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

પૂર્ણાંક fd_is_read_only_fs(પૂર્ણાંક fd);
પૂર્ણાંક path_is_read_only_fs(અચલ અક્ષર *path);

પૂર્ણાંક inode_same_at(પૂર્ણાંક fda, અચલ અક્ષર *filea, પૂર્ણાંક fdb, અચલ અક્ષર *fileb, પૂર્ણાંક flags);
સ્થિર અંતઃરેખ પૂર્ણાંક inode_same(અચલ અક્ષર *filea, અચલ અક્ષર *fileb, પૂર્ણાંક flags) {
        પાછા inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
સ્થિર અંતઃરેખ પૂર્ણાંક fd_inode_same(પૂર્ણાંક fda, પૂર્ણાંક fdb) {
        પાછા inode_same_at(fda, ખાલી, fdb, ખાલી, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
પ્રકારવ્યાખ્યા typeof_field(સંરચના statfs, f_type) statfs_f_type_t;

bool is_fs_type(અચલ સંરચના statfs *s, statfs_f_type_t magic_કિંમત) _pure_;
પૂર્ણાંક is_fs_type_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, statfs_f_type_t magic_કિંમત);
સ્થિર અંતઃરેખ પૂર્ણાંક fd_is_fs_type(પૂર્ણાંક fd, statfs_f_type_t magic_કિંમત) {
        પાછા is_fs_type_at(fd, ખાલી, magic_કિંમત);
}
સ્થિર અંતઃરેખ પૂર્ણાંક path_is_fs_type(અચલ અક્ષર *path, statfs_f_type_t magic_કિંમત) {
        પાછા is_fs_type_at(AT_FDCWD, path, magic_કિંમત);
}

bool is_temporary_fs(અચલ સંરચના statfs *s) _pure_;
bool is_network_fs(અચલ સંરચના statfs *s) _pure_;

પૂર્ણાંક fd_is_temporary_fs(પૂર્ણાંક fd);
પૂર્ણાંક fd_is_network_fs(પૂર્ણાંક fd);

પૂર્ણાંક path_is_temporary_fs(અચલ અક્ષર *path);
પૂર્ણાંક path_is_network_fs(અચલ અક્ષર *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

પૂર્ણાંક proc_mounted(શૂન્ય);

bool stat_inode_same(અચલ સંરચના stat *a, અચલ સંરચના stat *b);
bool stat_inode_unmodified(અચલ સંરચના stat *a, અચલ સંરચના stat *b);

bool statx_inode_same(અચલ સંરચના statx *a, અચલ સંરચના statx *b);
પૂર્ણાંક statx_mount_same(અચલ સંરચના statx *a, અચલ સંરચના statx *b);

પૂર્ણાંક xstatfsat(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, સંરચના statfs *ret);

usec_t statx_timestamp_load(અચલ સંરચના statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(અચલ સંરચના statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
શૂન્ય inode_hash_func(અચલ સંરચના stat *q, સંરચના siphash *state);
પૂર્ણાંક inode_compare_func(અચલ સંરચના stat *a, અચલ સંરચના stat *b);
બાહ્ય અચલ સંરચના hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
શૂન્ય inode_unmodified_hash_func(અચલ સંરચના stat *q, સંરચના siphash *state);
પૂર્ણાંક inode_unmodified_compare_func(અચલ સંરચના stat *a, અચલ સંરચના stat *b);
બાહ્ય અચલ સંરચના hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
સ્થિર અંતઃરેખ bool stat_is_set(અચલ સંરચના stat *st) {
        પાછા st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
સ્થિર અંતઃરેખ bool statx_is_set(અચલ સંરચના statx *sx) {
        પાછા sx && sx->stx_mask != 0;
}
સ્થિર અંતઃરેખ bool statfs_is_set(અચલ સંરચના statfs *sfs) {
        પાછા sfs && sfs->f_type != 0;
}

સ્થિર અંતઃરેખ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        પાછા IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

પૂર્ણાંક vfs_free_bytes(પૂર્ણાંક fd, uint64_t *ret);
