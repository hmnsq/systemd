/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/stat.h>           /* IWYU pragma: export */
#అన్తర్భావ <sys/statfs.h>         /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

పూర్ణాంకం stat_verify_regular(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం statx_verify_regular(స్థిరరాశి నిర్మాణం statx *stx);
పూర్ణాంకం verify_regular_at(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, bool follow);
పూర్ణాంకం fd_verify_regular(పూర్ణాంకం fd);

పూర్ణాంకం stat_verify_directory(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం statx_verify_directory(స్థిరరాశి నిర్మాణం statx *stx);
పూర్ణాంకం fd_verify_directory(పూర్ణాంకం fd);
పూర్ణాంకం is_dir_at(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, bool follow);
పూర్ణాంకం is_dir(స్థిరరాశి అక్షరం *మార్గం, bool follow);

పూర్ణాంకం stat_verify_symlink(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం fd_verify_symlink(పూర్ణాంకం fd);
పూర్ణాంకం is_symlink(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం stat_verify_socket(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం statx_verify_socket(స్థిరరాశి నిర్మాణం statx *stx);
పూర్ణాంకం fd_verify_socket(పూర్ణాంకం fd);
పూర్ణాంకం is_socket(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం stat_verify_linked(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం fd_verify_linked(పూర్ణాంకం fd);

పూర్ణాంకం stat_verify_ఖండం(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం fd_verify_ఖండం(పూర్ణాంకం fd);

పూర్ణాంకం stat_verify_char(స్థిరరాశి నిర్మాణం stat *st);

పూర్ణాంకం stat_verify_device_కణుపు(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం is_device_కణుపు(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం stat_verify_regular_or_ఖండం(స్థిరరాశి నిర్మాణం stat *st);
పూర్ణాంకం fd_verify_regular_or_ఖండం(పూర్ణాంకం fd);

పూర్ణాంకం dir_is_ఖాళీలు_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, bool ignore_hidden_or_backup);
స్థిర అంతఃవరుస పూర్ణాంకం dir_is_ఖాళీలు(స్థిరరాశి అక్షరం *మార్గం, bool ignore_hidden_or_backup) {
        తిరిగి dir_is_ఖాళీలు_at(AT_FDCWD, మార్గం, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(నిర్మాణం stat *st) శుద్ధ;
bool stat_is_ఖాళీలు(నిర్మాణం stat *st) శుద్ధ;
స్థిర అంతఃవరుస bool null_or_ఖాళీలు(నిర్మాణం stat *st) {
        తిరిగి stat_may_be_dev_null(st) || stat_is_ఖాళీలు(st);
}
పూర్ణాంకం null_or_ఖాళీలు_మార్గం_with_మూలం(స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *మూలం);

స్థిర అంతఃవరుస పూర్ణాంకం null_or_ఖాళీలు_మార్గం(స్థిరరాశి అక్షరం *fn) {
        తిరిగి null_or_ఖాళీలు_మార్గం_with_మూలం(fn, ఖాళీ);
}

రకనిర్వచనం గణన XStatXFlags {
        XSTATX_MNT_ID_ఉత్తమ = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

పూర్ణాంకం xstatx_నిండిన(పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం statx_flags,
                XStatXFlags xstatx_flags,
                గుర్తులేని mandatory_ముసుగు,
                గుర్తులేని optional_ముసుగు,
                uint64_t mandatory_లక్షణాలు,
                నిర్మాణం statx *ret);

స్థిర అంతఃవరుస పూర్ణాంకం xstatx(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం statx_flags,
                గుర్తులేని mandatory_ముసుగు,
                నిర్మాణం statx *ret) {

        తిరిగి xstatx_నిండిన(fd, మార్గం, statx_flags, 0, mandatory_ముసుగు, 0, 0, ret);
}

పూర్ణాంకం fd_is_read_only_fs(పూర్ణాంకం fd);
పూర్ణాంకం మార్గం_is_read_only_fs(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం inode_same_at(పూర్ణాంకం fda, స్థిరరాశి అక్షరం *filea, పూర్ణాంకం fdb, స్థిరరాశి అక్షరం *fileb, పూర్ణాంకం flags);
స్థిర అంతఃవరుస పూర్ణాంకం inode_same(స్థిరరాశి అక్షరం *filea, స్థిరరాశి అక్షరం *fileb, పూర్ణాంకం flags) {
        తిరిగి inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
స్థిర అంతఃవరుస పూర్ణాంకం fd_inode_same(పూర్ణాంకం fda, పూర్ణాంకం fdb) {
        తిరిగి inode_same_at(fda, ఖాళీ, fdb, ఖాళీ, AT_ఖాళీలు_మార్గం);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
రకనిర్వచనం typeof_క్షేత్రం(నిర్మాణం statfs, f_type) statfs_f_type_t;

bool is_fs_type(స్థిరరాశి నిర్మాణం statfs *s, statfs_f_type_t magic_విలువ) శుద్ధ;
పూర్ణాంకం is_fs_type_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, statfs_f_type_t magic_విలువ);
స్థిర అంతఃవరుస పూర్ణాంకం fd_is_fs_type(పూర్ణాంకం fd, statfs_f_type_t magic_విలువ) {
        తిరిగి is_fs_type_at(fd, ఖాళీ, magic_విలువ);
}
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_is_fs_type(స్థిరరాశి అక్షరం *మార్గం, statfs_f_type_t magic_విలువ) {
        తిరిగి is_fs_type_at(AT_FDCWD, మార్గం, magic_విలువ);
}

bool is_temporary_fs(స్థిరరాశి నిర్మాణం statfs *s) శుద్ధ;
bool is_జాలం_fs(స్థిరరాశి నిర్మాణం statfs *s) శుద్ధ;

పూర్ణాంకం fd_is_temporary_fs(పూర్ణాంకం fd);
పూర్ణాంకం fd_is_జాలం_fs(పూర్ణాంకం fd);

పూర్ణాంకం మార్గం_is_temporary_fs(స్థిరరాశి అక్షరం *మార్గం);
పూర్ణాంకం మార్గం_is_జాలం_fs(స్థిరరాశి అక్షరం *మార్గం);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#నిర్వచించు F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

పూర్ణాంకం proc_mounted(శూన్య);

bool stat_inode_same(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b);
bool stat_inode_unmodified(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b);

bool statx_inode_same(స్థిరరాశి నిర్మాణం statx *a, స్థిరరాశి నిర్మాణం statx *b);
పూర్ణాంకం statx_mount_same(స్థిరరాశి నిర్మాణం statx *a, స్థిరరాశి నిర్మాణం statx *b);

పూర్ణాంకం xstatfsat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, నిర్మాణం statfs *ret);

usec_t statx_timestamp_ఎక్కించు(స్థిరరాశి నిర్మాణం statx_timestamp *ts) శుద్ధ;
nsec_t statx_timestamp_ఎక్కించు_nsec(స్థిరరాశి నిర్మాణం statx_timestamp *ts) శుద్ధ;

/* This compares inode number, backing device and inode type, but not modification info */
శూన్య inode_hash_func(స్థిరరాశి నిర్మాణం stat *q, నిర్మాణం siphash *state);
పూర్ణాంకం inode_compare_func(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b);
బాహ్య స్థిరరాశి నిర్మాణం hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
శూన్య inode_unmodified_hash_func(స్థిరరాశి నిర్మాణం stat *q, నిర్మాణం siphash *state);
పూర్ణాంకం inode_unmodified_compare_func(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b);
బాహ్య స్థిరరాశి నిర్మాణం hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_పట్టిక_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
స్థిర అంతఃవరుస bool stat_is_సమితి(స్థిరరాశి నిర్మాణం stat *st) {
        తిరిగి st && st->st_dev != 0 && st->st_mode != బహుళకం_INVALID;
}
స్థిర అంతఃవరుస bool statx_is_సమితి(స్థిరరాశి నిర్మాణం statx *sx) {
        తిరిగి sx && sx->stx_ముసుగు != 0;
}
స్థిర అంతఃవరుస bool statfs_is_సమితి(స్థిరరాశి నిర్మాణం statfs *sfs) {
        తిరిగి sfs && sfs->f_type != 0;
}

స్థిర అంతఃవరుస bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        తిరిగి లోపల_సమితి(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

పూర్ణాంకం vfs_free_బైట్లు(పూర్ణాంకం fd, uint64_t *ret);
