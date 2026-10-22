/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/stat.h>           /* IWYU pragma: export */
#अन्तर्भाव <sys/statfs.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क stat_verify_regular(अचर संरचना stat *st);
पूर्णाङ्क statx_verify_regular(अचर संरचना statx *stx);
पूर्णाङ्क verify_regular_at(पूर्णाङ्क fd, अचर वर्ण *path, bool follow);
पूर्णाङ्क fd_verify_regular(पूर्णाङ्क fd);

पूर्णाङ्क stat_verify_directory(अचर संरचना stat *st);
पूर्णाङ्क statx_verify_directory(अचर संरचना statx *stx);
पूर्णाङ्क fd_verify_directory(पूर्णाङ्क fd);
पूर्णाङ्क is_dir_at(पूर्णाङ्क fd, अचर वर्ण *path, bool follow);
पूर्णाङ्क is_dir(अचर वर्ण *path, bool follow);

पूर्णाङ्क stat_verify_symlink(अचर संरचना stat *st);
पूर्णाङ्क fd_verify_symlink(पूर्णाङ्क fd);
पूर्णाङ्क is_symlink(अचर वर्ण *path);

पूर्णाङ्क stat_verify_socket(अचर संरचना stat *st);
पूर्णाङ्क statx_verify_socket(अचर संरचना statx *stx);
पूर्णाङ्क fd_verify_socket(पूर्णाङ्क fd);
पूर्णाङ्क is_socket(अचर वर्ण *path);

पूर्णाङ्क stat_verify_linked(अचर संरचना stat *st);
पूर्णाङ्क fd_verify_linked(पूर्णाङ्क fd);

पूर्णाङ्क stat_verify_block(अचर संरचना stat *st);
पूर्णाङ्क fd_verify_block(पूर्णाङ्क fd);

पूर्णाङ्क stat_verify_char(अचर संरचना stat *st);

पूर्णाङ्क stat_verify_device_गाँठो(अचर संरचना stat *st);
पूर्णाङ्क is_device_गाँठो(अचर वर्ण *path);

पूर्णाङ्क stat_verify_regular_or_block(अचर संरचना stat *st);
पूर्णाङ्क fd_verify_regular_or_block(पूर्णाङ्क fd);

पूर्णाङ्क dir_is_empty_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, bool ignore_hidden_or_backup);
स्थिर अन्तःस्थ पूर्णाङ्क dir_is_empty(अचर वर्ण *path, bool ignore_hidden_or_backup) {
        फर्काऊ dir_is_empty_at(AT_FDCWD, path, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(संरचना stat *st) _pure_;
bool stat_is_empty(संरचना stat *st) _pure_;
स्थिर अन्तःस्थ bool null_or_empty(संरचना stat *st) {
        फर्काऊ stat_may_be_dev_null(st) || stat_is_empty(st);
}
पूर्णाङ्क null_or_empty_path_with_जरा(अचर वर्ण *fn, अचर वर्ण *जरा);

स्थिर अन्तःस्थ पूर्णाङ्क null_or_empty_path(अचर वर्ण *fn) {
        फर्काऊ null_or_empty_path_with_जरा(fn, रिक्त);
}

प्रकारनिर्देश गणना XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

पूर्णाङ्क xstatx_full(पूर्णाङ्क fd,
                अचर वर्ण *path,
                पूर्णाङ्क statx_flags,
                XStatXFlags xstatx_flags,
                अचिन्हित mandatory_mask,
                अचिन्हित optional_mask,
                uint64_t mandatory_attributes,
                संरचना statx *ret);

स्थिर अन्तःस्थ पूर्णाङ्क xstatx(
                पूर्णाङ्क fd,
                अचर वर्ण *path,
                पूर्णाङ्क statx_flags,
                अचिन्हित mandatory_mask,
                संरचना statx *ret) {

        फर्काऊ xstatx_full(fd, path, statx_flags, 0, mandatory_mask, 0, 0, ret);
}

पूर्णाङ्क fd_is_read_only_fs(पूर्णाङ्क fd);
पूर्णाङ्क path_is_read_only_fs(अचर वर्ण *path);

पूर्णाङ्क inode_same_at(पूर्णाङ्क fda, अचर वर्ण *filea, पूर्णाङ्क fdb, अचर वर्ण *fileb, पूर्णाङ्क flags);
स्थिर अन्तःस्थ पूर्णाङ्क inode_same(अचर वर्ण *filea, अचर वर्ण *fileb, पूर्णाङ्क flags) {
        फर्काऊ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
स्थिर अन्तःस्थ पूर्णाङ्क fd_inode_same(पूर्णाङ्क fda, पूर्णाङ्क fdb) {
        फर्काऊ inode_same_at(fda, रिक्त, fdb, रिक्त, AT_EMPTY_PATH);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
प्रकारनिर्देश typeof_field(संरचना statfs, f_type) statfs_f_type_t;

bool is_fs_type(अचर संरचना statfs *s, statfs_f_type_t magic_मान) _pure_;
पूर्णाङ्क is_fs_type_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, statfs_f_type_t magic_मान);
स्थिर अन्तःस्थ पूर्णाङ्क fd_is_fs_type(पूर्णाङ्क fd, statfs_f_type_t magic_मान) {
        फर्काऊ is_fs_type_at(fd, रिक्त, magic_मान);
}
स्थिर अन्तःस्थ पूर्णाङ्क path_is_fs_type(अचर वर्ण *path, statfs_f_type_t magic_मान) {
        फर्काऊ is_fs_type_at(AT_FDCWD, path, magic_मान);
}

bool is_temporary_fs(अचर संरचना statfs *s) _pure_;
bool is_network_fs(अचर संरचना statfs *s) _pure_;

पूर्णाङ्क fd_is_temporary_fs(पूर्णाङ्क fd);
पूर्णाङ्क fd_is_network_fs(पूर्णाङ्क fd);

पूर्णाङ्क path_is_temporary_fs(अचर वर्ण *path);
पूर्णाङ्क path_is_network_fs(अचर वर्ण *path);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#define F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

पूर्णाङ्क proc_mounted(शून्य);

bool stat_inode_same(अचर संरचना stat *a, अचर संरचना stat *b);
bool stat_inode_unmodified(अचर संरचना stat *a, अचर संरचना stat *b);

bool statx_inode_same(अचर संरचना statx *a, अचर संरचना statx *b);
पूर्णाङ्क statx_mount_same(अचर संरचना statx *a, अचर संरचना statx *b);

पूर्णाङ्क xstatfsat(पूर्णाङ्क dir_fd, अचर वर्ण *path, संरचना statfs *ret);

usec_t statx_timestamp_load(अचर संरचना statx_timestamp *ts) _pure_;
nsec_t statx_timestamp_load_nsec(अचर संरचना statx_timestamp *ts) _pure_;

/* This compares inode number, backing device and inode type, but not modification info */
शून्य inode_hash_func(अचर संरचना stat *q, संरचना siphash *state);
पूर्णाङ्क inode_compare_func(अचर संरचना stat *a, अचर संरचना stat *b);
बाह्य अचर संरचना hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
शून्य inode_unmodified_hash_func(अचर संरचना stat *q, संरचना siphash *state);
पूर्णाङ्क inode_unmodified_compare_func(अचर संरचना stat *a, अचर संरचना stat *b);
बाह्य अचर संरचना hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_TABLE_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
स्थिर अन्तःस्थ bool stat_is_set(अचर संरचना stat *st) {
        फर्काऊ st && st->st_dev != 0 && st->st_mode != MODE_INVALID;
}
स्थिर अन्तःस्थ bool statx_is_set(अचर संरचना statx *sx) {
        फर्काऊ sx && sx->stx_mask != 0;
}
स्थिर अन्तःस्थ bool statfs_is_set(अचर संरचना statfs *sfs) {
        फर्काऊ sfs && sfs->f_type != 0;
}

स्थिर अन्तःस्थ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        फर्काऊ IN_SET(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

पूर्णाङ्क vfs_free_bytes(पूर्णाङ्क fd, uint64_t *ret);
