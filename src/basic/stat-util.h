/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/stat.h>           /* IWYU pragma: export */
#شامل <sys/statfs.h>         /* IWYU pragma: export */

#شامل "forward.h"

صحیح stat_verify_regular(ثابت ساخت stat *st);
صحیح statx_verify_regular(ثابت ساخت statx *stx);
صحیح verify_regular_at(صحیح fd, ثابت حرف *راستہ, bool follow);
صحیح fd_verify_regular(صحیح fd);

صحیح stat_verify_directory(ثابت ساخت stat *st);
صحیح statx_verify_directory(ثابت ساخت statx *stx);
صحیح fd_verify_directory(صحیح fd);
صحیح is_dir_at(صحیح fd, ثابت حرف *راستہ, bool follow);
صحیح is_dir(ثابت حرف *راستہ, bool follow);

صحیح stat_verify_symlink(ثابت ساخت stat *st);
صحیح fd_verify_symlink(صحیح fd);
صحیح is_symlink(ثابت حرف *راستہ);

صحیح stat_verify_socket(ثابت ساخت stat *st);
صحیح statx_verify_socket(ثابت ساخت statx *stx);
صحیح fd_verify_socket(صحیح fd);
صحیح is_socket(ثابت حرف *راستہ);

صحیح stat_verify_linked(ثابت ساخت stat *st);
صحیح fd_verify_linked(صحیح fd);

صحیح stat_verify_بلاک(ثابت ساخت stat *st);
صحیح fd_verify_بلاک(صحیح fd);

صحیح stat_verify_char(ثابت ساخت stat *st);

صحیح stat_verify_device_گرہ(ثابت ساخت stat *st);
صحیح is_device_گرہ(ثابت حرف *راستہ);

صحیح stat_verify_regular_or_بلاک(ثابت ساخت stat *st);
صحیح fd_verify_regular_or_بلاک(صحیح fd);

صحیح dir_is_خالی_خانے_at(صحیح dir_fd, ثابت حرف *راستہ, bool ignore_hidden_or_backup);
ساکن اندرونی صحیح dir_is_خالی_خانے(ثابت حرف *راستہ, bool ignore_hidden_or_backup) {
        واپس dir_is_خالی_خانے_at(AT_FDCWD, راستہ, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(ساخت stat *st) خالص;
bool stat_is_خالی_خانے(ساخت stat *st) خالص;
ساکن اندرونی bool null_or_خالی_خانے(ساخت stat *st) {
        واپس stat_may_be_dev_null(st) || stat_is_خالی_خانے(st);
}
صحیح null_or_خالی_خانے_راستہ_with_جڑ(ثابت حرف *fn, ثابت حرف *جڑ);

ساکن اندرونی صحیح null_or_خالی_خانے_راستہ(ثابت حرف *fn) {
        واپس null_or_خالی_خانے_راستہ_with_جڑ(fn, تہی);
}

قسمتعریف شمار XStatXFlags {
        XSTATX_MNT_ID_بہترین = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

صحیح xstatx_بھرا(صحیح fd,
                ثابت حرف *راستہ,
                صحیح statx_flags,
                XStatXFlags xstatx_flags,
                غیرعلامتی mandatory_نقاب,
                غیرعلامتی optional_نقاب,
                uint64_t mandatory_اوصاف,
                ساخت statx *ret);

ساکن اندرونی صحیح xstatx(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح statx_flags,
                غیرعلامتی mandatory_نقاب,
                ساخت statx *ret) {

        واپس xstatx_بھرا(fd, راستہ, statx_flags, 0, mandatory_نقاب, 0, 0, ret);
}

صحیح fd_is_read_only_fs(صحیح fd);
صحیح راستہ_is_read_only_fs(ثابت حرف *راستہ);

صحیح inode_same_at(صحیح fda, ثابت حرف *filea, صحیح fdb, ثابت حرف *fileb, صحیح flags);
ساکن اندرونی صحیح inode_same(ثابت حرف *filea, ثابت حرف *fileb, صحیح flags) {
        واپس inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
ساکن اندرونی صحیح fd_inode_same(صحیح fda, صحیح fdb) {
        واپس inode_same_at(fda, تہی, fdb, تہی, AT_خالی_خانے_راستہ);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
قسمتعریف typeof_میدان(ساخت statfs, f_type) statfs_f_type_t;

bool is_fs_type(ثابت ساخت statfs *s, statfs_f_type_t magic_قدر) خالص;
صحیح is_fs_type_at(صحیح dir_fd, ثابت حرف *راستہ, statfs_f_type_t magic_قدر);
ساکن اندرونی صحیح fd_is_fs_type(صحیح fd, statfs_f_type_t magic_قدر) {
        واپس is_fs_type_at(fd, تہی, magic_قدر);
}
ساکن اندرونی صحیح راستہ_is_fs_type(ثابت حرف *راستہ, statfs_f_type_t magic_قدر) {
        واپس is_fs_type_at(AT_FDCWD, راستہ, magic_قدر);
}

bool is_temporary_fs(ثابت ساخت statfs *s) خالص;
bool is_جال_fs(ثابت ساخت statfs *s) خالص;

صحیح fd_is_temporary_fs(صحیح fd);
صحیح fd_is_جال_fs(صحیح fd);

صحیح راستہ_is_temporary_fs(ثابت حرف *راستہ);
صحیح راستہ_is_جال_fs(ثابت حرف *راستہ);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#تعریف_کرو F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

صحیح proc_mounted(خالی);

bool stat_inode_same(ثابت ساخت stat *a, ثابت ساخت stat *b);
bool stat_inode_unmodified(ثابت ساخت stat *a, ثابت ساخت stat *b);

bool statx_inode_same(ثابت ساخت statx *a, ثابت ساخت statx *b);
صحیح statx_mount_same(ثابت ساخت statx *a, ثابت ساخت statx *b);

صحیح xstatfsat(صحیح dir_fd, ثابت حرف *راستہ, ساخت statfs *ret);

usec_t statx_timestamp_بھرو(ثابت ساخت statx_timestamp *ts) خالص;
nsec_t statx_timestamp_بھرو_nsec(ثابت ساخت statx_timestamp *ts) خالص;

/* This compares inode number, backing device and inode type, but not modification info */
خالی inode_hash_func(ثابت ساخت stat *q, ساخت siphash *state);
صحیح inode_compare_func(ثابت ساخت stat *a, ثابت ساخت stat *b);
خارجی ثابت ساخت hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
خالی inode_unmodified_hash_func(ثابت ساخت stat *q, ساخت siphash *state);
صحیح inode_unmodified_compare_func(ثابت ساخت stat *a, ثابت ساخت stat *b);
خارجی ثابت ساخت hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_جدول_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
ساکن اندرونی bool stat_is_سیٹ(ثابت ساخت stat *st) {
        واپس st && st->st_dev != 0 && st->st_mode != منوال_INVALID;
}
ساکن اندرونی bool statx_is_سیٹ(ثابت ساخت statx *sx) {
        واپس sx && sx->stx_نقاب != 0;
}
ساکن اندرونی bool statfs_is_سیٹ(ثابت ساخت statfs *sfs) {
        واپس sfs && sfs->f_type != 0;
}

ساکن اندرونی bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        واپس اندر_سیٹ(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

صحیح vfs_free_بائٹیں(صحیح fd, uint64_t *ret);
