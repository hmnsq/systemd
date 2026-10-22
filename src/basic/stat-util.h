/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/stat.h>           /* IWYU pragma: export */
#अन्तर्भाव <sys/statfs.h>         /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क stat_verify_regular(ध्रुव संरचना stat *st);
पूर्णाङ्क statx_verify_regular(ध्रुव संरचना statx *stx);
पूर्णाङ्क verify_regular_at(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, bool follow);
पूर्णाङ्क fd_verify_regular(पूर्णाङ्क fd);

पूर्णाङ्क stat_verify_directory(ध्रुव संरचना stat *st);
पूर्णाङ्क statx_verify_directory(ध्रुव संरचना statx *stx);
पूर्णाङ्क fd_verify_directory(पूर्णाङ्क fd);
पूर्णाङ्क is_dir_at(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, bool follow);
पूर्णाङ्क is_dir(ध्रुव वर्ण *मार्गः, bool follow);

पूर्णाङ्क stat_verify_symlink(ध्रुव संरचना stat *st);
पूर्णाङ्क fd_verify_symlink(पूर्णाङ्क fd);
पूर्णाङ्क is_symlink(ध्रुव वर्ण *मार्गः);

पूर्णाङ्क stat_verify_socket(ध्रुव संरचना stat *st);
पूर्णाङ्क statx_verify_socket(ध्रुव संरचना statx *stx);
पूर्णाङ्क fd_verify_socket(पूर्णाङ्क fd);
पूर्णाङ्क is_socket(ध्रुव वर्ण *मार्गः);

पूर्णाङ्क stat_verify_linked(ध्रुव संरचना stat *st);
पूर्णाङ्क fd_verify_linked(पूर्णाङ्क fd);

पूर्णाङ्क stat_verify_खण्डम्(ध्रुव संरचना stat *st);
पूर्णाङ्क fd_verify_खण्डम्(पूर्णाङ्क fd);

पूर्णाङ्क stat_verify_char(ध्रुव संरचना stat *st);

पूर्णाङ्क stat_verify_device_ग्रन्थिः(ध्रुव संरचना stat *st);
पूर्णाङ्क is_device_ग्रन्थिः(ध्रुव वर्ण *मार्गः);

पूर्णाङ्क stat_verify_regular_or_खण्डम्(ध्रुव संरचना stat *st);
पूर्णाङ्क fd_verify_regular_or_खण्डम्(पूर्णाङ्क fd);

पूर्णाङ्क dir_is_रिक्तानि_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, bool ignore_hidden_or_backup);
स्थिर अन्तःस्थ पूर्णाङ्क dir_is_रिक्तानि(ध्रुव वर्ण *मार्गः, bool ignore_hidden_or_backup) {
        प्रत्यागम dir_is_रिक्तानि_at(AT_FDCWD, मार्गः, ignore_hidden_or_backup);
}

bool stat_may_be_dev_null(संरचना stat *st) विशुद्धम्;
bool stat_is_रिक्तानि(संरचना stat *st) विशुद्धम्;
स्थिर अन्तःस्थ bool null_or_रिक्तानि(संरचना stat *st) {
        प्रत्यागम stat_may_be_dev_null(st) || stat_is_रिक्तानि(st);
}
पूर्णाङ्क null_or_रिक्तानि_मार्गः_with_मूलम्(ध्रुव वर्ण *fn, ध्रुव वर्ण *मूलम्);

स्थिर अन्तःस्थ पूर्णाङ्क null_or_रिक्तानि_मार्गः(ध्रुव वर्ण *fn) {
        प्रत्यागम null_or_रिक्तानि_मार्गः_with_मूलम्(fn, रिक्त);
}

प्रकारनिर्देश गणना XStatXFlags {
        XSTATX_MNT_ID_BEST = 1 << 0, /* Like STATX_MNT_ID_UNIQUE if available, STATX_MNT_ID otherwise */
} XStatXFlags;

पूर्णाङ्क xstatx_पूर्णम्(पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क statx_flags,
                XStatXFlags xstatx_flags,
                अचिह्नित mandatory_आवरणम्,
                अचिह्नित optional_आवरणम्,
                uint64_t mandatory_गुणाः,
                संरचना statx *ret);

स्थिर अन्तःस्थ पूर्णाङ्क xstatx(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *मार्गः,
                पूर्णाङ्क statx_flags,
                अचिह्नित mandatory_आवरणम्,
                संरचना statx *ret) {

        प्रत्यागम xstatx_पूर्णम्(fd, मार्गः, statx_flags, 0, mandatory_आवरणम्, 0, 0, ret);
}

पूर्णाङ्क fd_is_read_only_fs(पूर्णाङ्क fd);
पूर्णाङ्क मार्गः_is_read_only_fs(ध्रुव वर्ण *मार्गः);

पूर्णाङ्क inode_same_at(पूर्णाङ्क fda, ध्रुव वर्ण *filea, पूर्णाङ्क fdb, ध्रुव वर्ण *fileb, पूर्णाङ्क flags);
स्थिर अन्तःस्थ पूर्णाङ्क inode_same(ध्रुव वर्ण *filea, ध्रुव वर्ण *fileb, पूर्णाङ्क flags) {
        प्रत्यागम inode_same_at(AT_FDCWD, filea, AT_FDCWD, fileb, flags);
}
स्थिर अन्तःस्थ पूर्णाङ्क fd_inode_same(पूर्णाङ्क fda, पूर्णाङ्क fdb) {
        प्रत्यागम inode_same_at(fda, रिक्त, fdb, रिक्त, AT_रिक्तानि_मार्गः);
}

/* The .f_type field of struct statfs is really weird defined on
 * different archs. Let's give its type a name. */
प्रकारनिर्देश typeof_क्षेत्रम्(संरचना statfs, f_type) statfs_f_type_t;

bool is_fs_type(ध्रुव संरचना statfs *s, statfs_f_type_t magic_मानम्) विशुद्धम्;
पूर्णाङ्क is_fs_type_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, statfs_f_type_t magic_मानम्);
स्थिर अन्तःस्थ पूर्णाङ्क fd_is_fs_type(पूर्णाङ्क fd, statfs_f_type_t magic_मानम्) {
        प्रत्यागम is_fs_type_at(fd, रिक्त, magic_मानम्);
}
स्थिर अन्तःस्थ पूर्णाङ्क मार्गः_is_fs_type(ध्रुव वर्ण *मार्गः, statfs_f_type_t magic_मानम्) {
        प्रत्यागम is_fs_type_at(AT_FDCWD, मार्गः, magic_मानम्);
}

bool is_temporary_fs(ध्रुव संरचना statfs *s) विशुद्धम्;
bool is_जालम्_fs(ध्रुव संरचना statfs *s) विशुद्धम्;

पूर्णाङ्क fd_is_temporary_fs(पूर्णाङ्क fd);
पूर्णाङ्क fd_is_जालम्_fs(पूर्णाङ्क fd);

पूर्णाङ्क मार्गः_is_temporary_fs(ध्रुव वर्ण *मार्गः);
पूर्णाङ्क मार्गः_is_जालम्_fs(ध्रुव वर्ण *मार्गः);

/* Because statfs.t_type can be int on some architectures, we have to cast
 * the const magic to the type, otherwise the compiler warns about
 * signed/unsigned comparison, because the magic can be 32 bit unsigned.
 */
#परिभाषय F_TYPE_EQUAL(a, b) (a == (typeof(a)) b)

पूर्णाङ्क proc_mounted(शून्य);

bool stat_inode_same(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b);
bool stat_inode_unmodified(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b);

bool statx_inode_same(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b);
पूर्णाङ्क statx_mount_same(ध्रुव संरचना statx *a, ध्रुव संरचना statx *b);

पूर्णाङ्क xstatfsat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, संरचना statfs *ret);

usec_t statx_timestamp_आरोपय(ध्रुव संरचना statx_timestamp *ts) विशुद्धम्;
nsec_t statx_timestamp_आरोपय_nsec(ध्रुव संरचना statx_timestamp *ts) विशुद्धम्;

/* This compares inode number, backing device and inode type, but not modification info */
शून्य inode_hash_func(ध्रुव संरचना stat *q, संरचना siphash *state);
पूर्णाङ्क inode_compare_func(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b);
बाह्य ध्रुव संरचना hash_ops inode_hash_ops;

/* This is a more thorough version of the above, and also checks the mtimes, the size, and the rdev. It does
 * not check "external" attributes such as access mode or ownership. */
शून्य inode_unmodified_hash_func(ध्रुव संरचना stat *q, संरचना siphash *state);
पूर्णाङ्क inode_unmodified_compare_func(ध्रुव संरचना stat *a, ध्रुव संरचना stat *b);
बाह्य ध्रुव संरचना hash_ops inode_unmodified_hash_ops;

DECLARE_STRING_सारणी_LOOKUP(inode_type, mode_t);

/* Macros that check whether the stat/statx structures have been initialized already. For "struct stat" we
 * use a check for .st_dev being non-zero, since the kernel unconditionally fills that in, mapping the file
 * to its originating superblock, regardless if the fs is block based or virtual (we also check for .st_mode
 * being MODE_INVALID, since we use that as an invalid marker for separate mode_t fields). For "struct statx"
 * we use the .stx_mask field, which must be non-zero if any of the fields have already been initialized. */
स्थिर अन्तःस्थ bool stat_is_समुच्चयः(ध्रुव संरचना stat *st) {
        प्रत्यागम st && st->st_dev != 0 && st->st_mode != बहुलकम्_INVALID;
}
स्थिर अन्तःस्थ bool statx_is_समुच्चयः(ध्रुव संरचना statx *sx) {
        प्रत्यागम sx && sx->stx_आवरणम् != 0;
}
स्थिर अन्तःस्थ bool statfs_is_समुच्चयः(ध्रुव संरचना statfs *sfs) {
        प्रत्यागम sfs && sfs->f_type != 0;
}

स्थिर अन्तःस्थ bool inode_type_can_hardlink(mode_t m) {
        /* returns true for all inode types that support hardlinks on linux. Note this is effectively all
         * inode types except for directories (and those weird misc fds such as eventfds() that have no inode
         * type). */
        प्रत्यागम अन्तर्_समुच्चयः(m & S_IFMT, S_IFSOCK, S_IFLNK, S_IFREG, S_IFBLK, S_IFCHR, S_IFIFO);
}

पूर्णाङ्क vfs_free_अष्टकानि(पूर्णाङ्क fd, uint64_t *ret);
