/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/stat.h>           /* IWYU pragma: export */
#അന്തര്ഭാവ <sys/statfs.h>         /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ stat_verify_regular(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ statx_verify_regular(സ്ഥിരം ഘടന statx *stx);
പൂർണ്ണസംഖ്യ verify_regular_at(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, bool follow);
പൂർണ്ണസംഖ്യ fd_verify_regular(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ stat_verify_directory(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ statx_verify_directory(സ്ഥിരം ഘടന statx *stx);
പൂർണ്ണസംഖ്യ fd_verify_directory(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ is_dir_at(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, bool follow);
പൂർണ്ണസംഖ്യ is_dir(സ്ഥിരം അക്ഷരം *path, bool follow);

പൂർണ്ണസംഖ്യ stat_verify_symlink(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ fd_verify_symlink(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ is_symlink(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ stat_verify_socket(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ statx_verify_socket(സ്ഥിരം ഘടന statx *stx);
പൂർണ്ണസംഖ്യ fd_verify_socket(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ is_socket(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ stat_verify_linked(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ fd_verify_linked(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ stat_verify_block(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ fd_verify_block(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ stat_verify_char(സ്ഥിരം ഘടന stat *st);

പൂർണ്ണസംഖ്യ stat_verify_device_കണ്ണി(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ is_device_കണ്ണി(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ stat_verify_regular_or_block(സ്ഥിരം ഘടന stat *st);
പൂർണ്ണസംഖ്യ fd_verify_regular_or_block(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ dir_is_empty_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, bool ignore_hidden_or_backup);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ dir_is_empty(സ്ഥിരം അക്ഷരം *path, bool ignore_hidden_or_backup) {
        തിരികെ dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ഘടന stat *st) _pure_;
bool stat_is_empty(ഘടന stat *st) _pure_;
സ്ഥിതം വരിയിൽ bool null_or_empty(ഘടന stat *st) {
        തിരികെ stat_may_be_dev_null(st) || stat_is_empty(st);
}
പൂർണ്ണസംഖ്യ null_or_empty_path_with_വേര്(സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *വേര്);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ null_or_empty_path(സ്ഥിരം അക്ഷരം *fn) {
        തിരികെ null_or_empty_path_with_വേര്(fn, ശൂന്യ);
}

തരനിർവചനം എണ്ണൽ XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

പൂർണ്ണസംഖ്യ xstatx_full(പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ statx_flags,
                XStatXFlags xstatx_flags,
                unsigned mandatory_mask,
                unsigned optional_mask,
                uint64_t mandatory_attributes,
                ഘടന statx *ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xstatx(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ statx_flags,
                unsigned mandatory_mask,
                ഘടന statx *ret) {

        തിരികെ xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

പൂർണ്ണസംഖ്യ fd_is_read_only_fs(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ path_is_read_only_fs(സ്ഥിരം അക്ഷരം *path);

പൂർണ്ണസംഖ്യ inode_same_at(പൂർണ്ണസംഖ്യ fda, സ്ഥിരം അക്ഷരം *filea, പൂർണ്ണസംഖ്യ fdb, സ്ഥിരം അക്ഷരം *fileb, പൂർണ്ണസംഖ്യ flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ inode_same(സ്ഥിരം അക്ഷരം *filea, സ്ഥിരം അക്ഷരം *fileb, പൂർണ്ണസംഖ്യ flags) {
        തിരികെ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_inode_same(പൂർണ്ണസംഖ്യ fda, പൂർണ്ണസംഖ്യ fdb) {
        തിരികെ inode_same_at(fda, ശൂന്യ, fdb, ശൂന്യ, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
തരനിർവചനം typeof_field(ഘടന statfs, f_type) statfs_f_type_t;

bool is_fs_type(സ്ഥിരം ഘടന statfs *s, statfs_f_type_t magic_മൂല്യം) _pure_;
പൂർണ്ണസംഖ്യ is_fs_type_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, statfs_f_type_t magic_മൂല്യം);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_is_fs_type(പൂർണ്ണസംഖ്യ fd, statfs_f_type_t magic_മൂല്യം) {
        തിരികെ is_fs_type_at(fd, ശൂന്യ, magic_മൂല്യം);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ path_is_fs_type(സ്ഥിരം അക്ഷരം *path, statfs_f_type_t magic_മൂല്യം) {
        തിരികെ is_fs_type_at(AT_FDCWD, path, magic_മൂല്യം);
}

bool is_temporary_fs(സ്ഥിരം ഘടന statfs *s) _pure_;
bool is_network_fs(സ്ഥിരം ഘടന statfs *s) _pure_;

പൂർണ്ണസംഖ്യ fd_is_temporary_fs(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ fd_is_network_fs(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ path_is_temporary_fs(സ്ഥിരം അക്ഷരം *path);
പൂർണ്ണസംഖ്യ path_is_network_fs(സ്ഥിരം അക്ഷരം *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

പൂർണ്ണസംഖ്യ proc_mounted(ശൂന്യം);

bool stat_inode_same(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b);
bool stat_inode_unmodified(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b);

bool statx_inode_same(സ്ഥിരം ഘടന statx *a, സ്ഥിരം ഘടന statx *b);
പൂർണ്ണസംഖ്യ statx_mount_same(സ്ഥിരം ഘടന statx *a, സ്ഥിരം ഘടന statx *b);

പൂർണ്ണസംഖ്യ xstatfsat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ഘടന statfs *ret);

usec_t statx_timestamp_load(സ്ഥിരം ഘടന statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(സ്ഥിരം ഘടന statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
ശൂന്യം inode_hash_func(സ്ഥിരം ഘടന stat *q, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ inode_compare_func(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b);
ബാഹ്യ സ്ഥിരം ഘടന hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
ശൂന്യം inode_unmodified_hash_func(സ്ഥിരം ഘടന stat *q, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ inode_unmodified_compare_func(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b);
ബാഹ്യ സ്ഥിരം ഘടന hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
സ്ഥിതം വരിയിൽ bool stat_is_set(സ്ഥിരം ഘടന stat *st) {
        തിരികെ st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
സ്ഥിതം വരിയിൽ bool statx_is_set(സ്ഥിരം ഘടന statx *sx) {
        തിരികെ sx && sx->stx_mask != 0;
}
സ്ഥിതം വരിയിൽ bool statfs_is_set(സ്ഥിരം ഘടന statfs *sfs) {
        തിരികെ sfs && sfs->f_type != 0;
}

സ്ഥിതം വരിയിൽ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        തിരികെ IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

പൂർണ്ണസംഖ്യ vfs_free_bytes(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);
