/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/stat.h>           /* IWYU pragma: export */
#ಅನ್ತರ್ಭಾವ <sys/statfs.h>         /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ stat_verify_regular(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ statx_verify_regular(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *stx);
ಪೂರ್ಣಾಂಕ verify_regular_at(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool follow);
ಪೂರ್ಣಾಂಕ fd_verify_regular(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ stat_verify_directory(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ statx_verify_directory(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *stx);
ಪೂರ್ಣಾಂಕ fd_verify_directory(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ is_dir_at(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool follow);
ಪೂರ್ಣಾಂಕ is_dir(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool follow);

ಪೂರ್ಣಾಂಕ stat_verify_symlink(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ fd_verify_symlink(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ is_symlink(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ stat_verify_socket(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ statx_verify_socket(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *stx);
ಪೂರ್ಣಾಂಕ fd_verify_socket(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ is_socket(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ stat_verify_linked(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ fd_verify_linked(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ stat_verify_ಖಂಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ fd_verify_ಖಂಡ(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ stat_verify_char(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);

ಪೂರ್ಣಾಂಕ stat_verify_device_ಗಂಟು(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ is_device_ಗಂಟು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ stat_verify_regular_or_ಖಂಡ(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st);
ಪೂರ್ಣಾಂಕ fd_verify_regular_or_ಖಂಡ(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ dir_is_ಖಾಲಿಗಳು_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool ignore_hidden_or_backup);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ dir_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, bool ignore_hidden_or_backup) {
        ಹಿಂತಿರುಗಿ dir_is_ಖಾಲಿಗಳು_at(AT_FDCWD, ಮಾರ್ಗ, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ರಚನೆ stat *st) ಶುದ್ಧ;
bool stat_is_ಖಾಲಿಗಳು(ರಚನೆ stat *st) ಶುದ್ಧ;
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool null_or_ಖಾಲಿಗಳು(ರಚನೆ stat *st) {
        ಹಿಂತಿರುಗಿ stat_may_be_dev_null(st) || stat_is_ಖಾಲಿಗಳು(st);
}
ಪೂರ್ಣಾಂಕ null_or_ಖಾಲಿಗಳು_ಮಾರ್ಗ_with_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ null_or_ಖಾಲಿಗಳು_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn) {
        ಹಿಂತಿರುಗಿ null_or_ಖಾಲಿಗಳು_ಮಾರ್ಗ_with_ಬೇರು(fn, ಖಾಲಿ);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ XStatXFlags {
        XSTATX_MNT_ID_ಉತ್ತಮ = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

ಪೂರ್ಣಾಂಕ xstatx_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ statx_flags,
                XStatXFlags xstatx_flags,
                ಚಿಹ್ನೆರಹಿತ mandatory_ಮುಖವಾಡ,
                ಚಿಹ್ನೆರಹಿತ optional_ಮುಖವಾಡ,
                uint64_t mandatory_ಗುಣಲಕ್ಷಣಗಳು,
                ರಚನೆ statx *ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xstatx(
                ಪೂರ್ಣಾಂಕ fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಪೂರ್ಣಾಂಕ statx_flags,
                ಚಿಹ್ನೆರಹಿತ mandatory_ಮುಖವಾಡ,
                ರಚನೆ statx *ret) {

        ಹಿಂತಿರುಗಿ xstatx_ತುಂಬಿದ(fd, ಮಾರ್ಗ, statx_flags, 0, mandatory_ಮುಖವಾಡ, 0, 0, ret);
}

ಪೂರ್ಣಾಂಕ fd_is_read_only_fs(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_read_only_fs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ inode_same_at(ಪೂರ್ಣಾಂಕ fda, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filea, ಪೂರ್ಣಾಂಕ fdb, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fileb, ಪೂರ್ಣಾಂಕ flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ inode_same(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filea, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fileb, ಪೂರ್ಣಾಂಕ flags) {
        ಹಿಂತಿರುಗಿ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_inode_same(ಪೂರ್ಣಾಂಕ fda, ಪೂರ್ಣಾಂಕ fdb) {
        ಹಿಂತಿರುಗಿ inode_same_at(fda, ಖಾಲಿ, fdb, ಖಾಲಿ, AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ typeof_ಕ್ಷೇತ್ರ(ರಚನೆ statfs, f_type) statfs_f_type_t;

bool is_fs_type(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s, statfs_f_type_t magic_ಮೌಲ್ಯ) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ is_fs_type_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, statfs_f_type_t magic_ಮೌಲ್ಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fd_is_fs_type(ಪೂರ್ಣಾಂಕ fd, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ is_fs_type_at(fd, ಖಾಲಿ, magic_ಮೌಲ್ಯ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_fs_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, statfs_f_type_t magic_ಮೌಲ್ಯ) {
        ಹಿಂತಿರುಗಿ is_fs_type_at(AT_FDCWD, ಮಾರ್ಗ, magic_ಮೌಲ್ಯ);
}

bool is_temporary_fs(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s) ಶುದ್ಧ;
bool is_ಜಾಲ_fs(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *s) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ fd_is_temporary_fs(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ fd_is_ಜಾಲ_fs(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_temporary_fs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);
ಪೂರ್ಣಾಂಕ ಮಾರ್ಗ_is_ಜಾಲ_fs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

ಪೂರ್ಣಾಂಕ proc_mounted(ಶೂನ್ಯ);

bool stat_inode_same(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b);
bool stat_inode_unmodified(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b);

bool statx_inode_same(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *a, ಸ್ಥಿರಾಂಕ ರಚನೆ statx *b);
ಪೂರ್ಣಾಂಕ statx_mount_same(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *a, ಸ್ಥಿರಾಂಕ ರಚನೆ statx *b);

ಪೂರ್ಣಾಂಕ xstatfsat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ರಚನೆ statfs *ret);

usec_t statx_timestamp_ತುಂಬು(ಸ್ಥಿರಾಂಕ ರಚನೆ statx_timestamp *ts) ಶುದ್ಧ;
nsec_t statx_timestamp_ತುಂಬು_nsec(ಸ್ಥಿರಾಂಕ ರಚನೆ statx_timestamp *ts) ಶುದ್ಧ;

/* This compares inode number, backing device and inode type, but not modification info */
ಶೂನ್ಯ inode_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *q, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ inode_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b);
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
ಶೂನ್ಯ inode_unmodified_hash_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *q, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ inode_unmodified_compare_func(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *a, ಸ್ಥಿರಾಂಕ ರಚನೆ stat *b);
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool stat_is_ಗಣ(ಸ್ಥಿರಾಂಕ ರಚನೆ stat *st) {
        ಹಿಂತಿರುಗಿ st && st->st_dev != 0 && st->st_mode != ಬಹುಲಕ_INVALID;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool statx_is_ಗಣ(ಸ್ಥಿರಾಂಕ ರಚನೆ statx *sx) {
        ಹಿಂತಿರುಗಿ sx && sx->stx_ಮುಖವಾಡ != 0;
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool statfs_is_ಗಣ(ಸ್ಥಿರಾಂಕ ರಚನೆ statfs *sfs) {
        ಹಿಂತಿರುಗಿ sfs && sfs->f_type != 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

ಪೂರ್ಣಾಂಕ vfs_free_ಬೈಟುಗಳು(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);
