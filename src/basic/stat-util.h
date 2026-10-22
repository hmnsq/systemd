/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>           /* IWYU pragma: export */
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/statfs.h>         /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ stat_verify_regular(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ statx_verify_regular(ਅਚਲ ਬਣਤਰ statx *stx);
ਪੂਰਨ verify_regular_at(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool follow);
ਪੂਰਨ fd_verify_regular(ਪੂਰਨ fd);

ਪੂਰਨ stat_verify_directory(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ statx_verify_directory(ਅਚਲ ਬਣਤਰ statx *stx);
ਪੂਰਨ fd_verify_directory(ਪੂਰਨ fd);
ਪੂਰਨ is_dir_at(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool follow);
ਪੂਰਨ is_dir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool follow);

ਪੂਰਨ stat_verify_symlink(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ fd_verify_symlink(ਪੂਰਨ fd);
ਪੂਰਨ is_symlink(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ stat_verify_socket(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ statx_verify_socket(ਅਚਲ ਬਣਤਰ statx *stx);
ਪੂਰਨ fd_verify_socket(ਪੂਰਨ fd);
ਪੂਰਨ is_socket(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ stat_verify_linked(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ fd_verify_linked(ਪੂਰਨ fd);

ਪੂਰਨ stat_verify_ਖੰਡ(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ fd_verify_ਖੰਡ(ਪੂਰਨ fd);

ਪੂਰਨ stat_verify_char(ਅਚਲ ਬਣਤਰ stat *st);

ਪੂਰਨ stat_verify_device_ਗੰਢ(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ is_device_ਗੰਢ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ stat_verify_regular_or_ਖੰਡ(ਅਚਲ ਬਣਤਰ stat *st);
ਪੂਰਨ fd_verify_regular_or_ਖੰਡ(ਪੂਰਨ fd);

ਪੂਰਨ dir_is_ਖਾਲੀ_ਥਾਵਾਂ_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool ignore_hidden_or_backup);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ dir_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool ignore_hidden_or_backup) {
        ਵਾਪਸ dir_is_ਖਾਲੀ_ਥਾਵਾਂ_at(AT_FDCWD, ਰਸਤਾ, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ਬਣਤਰ stat *st) ਸ਼ੁੱਧ;
bool stat_is_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ stat *st) ਸ਼ੁੱਧ;
ਸਥਿਰ ਸਤਰਅੰਦਰ bool null_or_ਖਾਲੀ_ਥਾਵਾਂ(ਬਣਤਰ stat *st) {
        ਵਾਪਸ stat_may_be_dev_null(st) || stat_is_ਖਾਲੀ_ਥਾਵਾਂ(st);
}
ਪੂਰਨ null_or_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ_with_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ null_or_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *fn) {
        ਵਾਪਸ null_or_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ_with_ਜੜ੍ਹ(fn, ਨਿਰਾ);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ XStatXFlags {
        XSTATX_MNT_ID_ਉੱਤਮ = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

ਪੂਰਨ xstatx_ਭਰਿਆ(ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ statx_flags,
                XStatXFlags xstatx_flags,
                ਬਿਨਾਨਿਸ਼ਾਨੀ mandatory_ਨਕਾਬ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ optional_ਨਕਾਬ,
                uint64_t mandatory_ਗੁਣ,
                ਬਣਤਰ statx *ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xstatx(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ statx_flags,
                ਬਿਨਾਨਿਸ਼ਾਨੀ mandatory_ਨਕਾਬ,
                ਬਣਤਰ statx *ret) {

        ਵਾਪਸ xstatx_ਭਰਿਆ(fd, ਰਸਤਾ, statx_flags, 0, mandatory_ਨਕਾਬ, 0, 0, ret);
}

ਪੂਰਨ fd_is_read_only_fs(ਪੂਰਨ fd);
ਪੂਰਨ ਰਸਤਾ_is_read_only_fs(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

ਪੂਰਨ inode_same_at(ਪੂਰਨ fda, ਅਚਲ ਅੱਖਰ *filea, ਪੂਰਨ fdb, ਅਚਲ ਅੱਖਰ *fileb, ਪੂਰਨ flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ inode_same(ਅਚਲ ਅੱਖਰ *filea, ਅਚਲ ਅੱਖਰ *fileb, ਪੂਰਨ flags) {
        ਵਾਪਸ inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_inode_same(ਪੂਰਨ fda, ਪੂਰਨ fdb) {
        ਵਾਪਸ inode_same_at(fda, ਨਿਰਾ, fdb, ਨਿਰਾ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ typeof_ਖੇਤਰ(ਬਣਤਰ statfs, f_type) statfs_f_type_t;

bool is_fs_type(ਅਚਲ ਬਣਤਰ statfs *s, statfs_f_type_t magic_ਮੁੱਲ) ਸ਼ੁੱਧ;
ਪੂਰਨ is_fs_type_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, statfs_f_type_t magic_ਮੁੱਲ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_is_fs_type(ਪੂਰਨ fd, statfs_f_type_t magic_ਮੁੱਲ) {
        ਵਾਪਸ is_fs_type_at(fd, ਨਿਰਾ, magic_ਮੁੱਲ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਰਸਤਾ_is_fs_type(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, statfs_f_type_t magic_ਮੁੱਲ) {
        ਵਾਪਸ is_fs_type_at(AT_FDCWD, ਰਸਤਾ, magic_ਮੁੱਲ);
}

bool is_temporary_fs(ਅਚਲ ਬਣਤਰ statfs *s) ਸ਼ੁੱਧ;
bool is_ਜਾਲ_fs(ਅਚਲ ਬਣਤਰ statfs *s) ਸ਼ੁੱਧ;

ਪੂਰਨ fd_is_temporary_fs(ਪੂਰਨ fd);
ਪੂਰਨ fd_is_ਜਾਲ_fs(ਪੂਰਨ fd);

ਪੂਰਨ ਰਸਤਾ_is_temporary_fs(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);
ਪੂਰਨ ਰਸਤਾ_is_ਜਾਲ_fs(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

ਪੂਰਨ proc_mounted(ਖਾਲੀ);

bool stat_inode_same(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b);
bool stat_inode_unmodified(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b);

bool statx_inode_same(ਅਚਲ ਬਣਤਰ statx *a, ਅਚਲ ਬਣਤਰ statx *b);
ਪੂਰਨ statx_mount_same(ਅਚਲ ਬਣਤਰ statx *a, ਅਚਲ ਬਣਤਰ statx *b);

ਪੂਰਨ xstatfsat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਬਣਤਰ statfs *ret);

usec_t statx_timestamp_ਲੱਦੋ(ਅਚਲ ਬਣਤਰ statx_timestamp *ts) ਸ਼ੁੱਧ;
nsec_t statx_timestamp_ਲੱਦੋ_nsec(ਅਚਲ ਬਣਤਰ statx_timestamp *ts) ਸ਼ੁੱਧ;

/* This compares inode number, backing device and inode type, but not modification info */
ਖਾਲੀ inode_hash_func(ਅਚਲ ਬਣਤਰ stat *q, ਬਣਤਰ siphash *state);
ਪੂਰਨ inode_compare_func(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b);
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
ਖਾਲੀ inode_unmodified_hash_func(ਅਚਲ ਬਣਤਰ stat *q, ਬਣਤਰ siphash *state);
ਪੂਰਨ inode_unmodified_compare_func(ਅਚਲ ਬਣਤਰ stat *a, ਅਚਲ ਬਣਤਰ stat *b);
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool stat_is_ਸੈੱਟ(ਅਚਲ ਬਣਤਰ stat *st) {
        ਵਾਪਸ st && st->st_dev != 0 && st->st_mode != ਬਹੁਲਕ_INVALID;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool statx_is_ਸੈੱਟ(ਅਚਲ ਬਣਤਰ statx *sx) {
        ਵਾਪਸ sx && sx->stx_ਨਕਾਬ != 0;
}
ਸਥਿਰ ਸਤਰਅੰਦਰ bool statfs_is_ਸੈੱਟ(ਅਚਲ ਬਣਤਰ statfs *sfs) {
        ਵਾਪਸ sfs && sfs->f_type != 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

ਪੂਰਨ vfs_free_ਬਾਈਟਾਂ(ਪੂਰਨ fd, uint64_t *ret);
