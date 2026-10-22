/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা fopen_temporary_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, FILE **ret_file, অক্ষর **ret_path);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fopen_temporary(ধ্রুবক অক্ষর *path, FILE **ret_file, অক্ষর **ret_path) {
        ফেরত fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

পূর্ণসংখ্যা fopen_temporary_child_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, FILE **ret_file, অক্ষর **ret_path);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fopen_temporary_child(ধ্রুবক অক্ষর *path, FILE **ret_file, অক্ষর **ret_path) {
        ফেরত fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

পূর্ণসংখ্যা mkostemp_safe(অক্ষর *pattern);
পূর্ণসংখ্যা fmkostemp_safe(অক্ষর *pattern, ধ্রুবক অক্ষর *mode, FILE**_f);

শূন্য unlink_tempfilep(অক্ষর (*p)[]);

পূর্ণসংখ্যা tempfn_xxxxxx(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *extra, অক্ষর **ret);
পূর্ণসংখ্যা tempfn_random(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *extra, অক্ষর **ret);
পূর্ণসংখ্যা tempfn_random_child(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *extra, অক্ষর **ret);

পূর্ণসংখ্যা open_tmpfile_unlinkable(ধ্রুবক অক্ষর *directory, পূর্ণসংখ্যা flags);
পূর্ণসংখ্যা open_tmpfile_linkable_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা flags, অক্ষর **ret_path);
স্থির অন্তঃরেখ পূর্ণসংখ্যা open_tmpfile_linkable(ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা flags, অক্ষর **ret_path) {
        ফেরত open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
পূর্ণসংখ্যা fopen_tmpfile_linkable_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা flags, অক্ষর **ret_path, FILE **ret_file);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fopen_tmpfile_linkable(ধ্রুবক অক্ষর *target, পূর্ণসংখ্যা flags, অক্ষর **ret_path, FILE **ret_file) {
        ফেরত fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

প্রকারলেখ গণনা LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

পূর্ণসংখ্যা link_tmpfile_at(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *target, LinkTmpfileFlags flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা link_tmpfile(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *target, LinkTmpfileFlags flags) {
        ফেরত link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
পূর্ণসংখ্যা flink_tmpfile_at(FILE *f, পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *target, LinkTmpfileFlags flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা flink_tmpfile(FILE *f, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *target, LinkTmpfileFlags flags) {
        ফেরত flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

পূর্ণসংখ্যা mkdtemp_malloc(ধ্রুবক অক্ষর *template, অক্ষর **ret);
পূর্ণসংখ্যা mkdtemp_open(ধ্রুবক অক্ষর *template, পূর্ণসংখ্যা flags, অক্ষর **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
কাঠামো cleanup_tmpfile_তথ্য {
        পূর্ণসংখ্যা *dir_fd;
        অক্ষর **filename;
};
শূন্য cleanup_tmpfile_তথ্য_done(কাঠামো cleanup_tmpfile_তথ্য *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_তথ্য_done) কাঠামো cleanup_tmpfile_তথ্য UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
