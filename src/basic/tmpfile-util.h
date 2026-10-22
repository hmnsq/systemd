/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

صحیح fopen_temporary_at(صحیح dir_fd, ثابت نویسه *path, FILE **ret_file, نویسه **ret_path);
ایستا درونخطی صحیح fopen_temporary(ثابت نویسه *path, FILE **ret_file, نویسه **ret_path) {
        بازگشت fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

صحیح fopen_temporary_child_at(صحیح dir_fd, ثابت نویسه *path, FILE **ret_file, نویسه **ret_path);
ایستا درونخطی صحیح fopen_temporary_child(ثابت نویسه *path, FILE **ret_file, نویسه **ret_path) {
        بازگشت fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

صحیح mkostemp_safe(نویسه *pattern);
صحیح fmkostemp_safe(نویسه *pattern, ثابت نویسه *mode, FILE**_f);

تهی unlink_tempfilep(نویسه (*p)[]);

صحیح tempfn_xxxxxx(ثابت نویسه *p, ثابت نویسه *extra, نویسه **ret);
صحیح tempfn_random(ثابت نویسه *p, ثابت نویسه *extra, نویسه **ret);
صحیح tempfn_random_child(ثابت نویسه *p, ثابت نویسه *extra, نویسه **ret);

صحیح open_tmpfile_unlinkable(ثابت نویسه *directory, صحیح flags);
صحیح open_tmpfile_linkable_at(صحیح dir_fd, ثابت نویسه *target, صحیح flags, نویسه **ret_path);
ایستا درونخطی صحیح open_tmpfile_linkable(ثابت نویسه *target, صحیح flags, نویسه **ret_path) {
        بازگشت open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
صحیح fopen_tmpfile_linkable_at(صحیح dir_fd, ثابت نویسه *target, صحیح flags, نویسه **ret_path, FILE **ret_file);
ایستا درونخطی صحیح fopen_tmpfile_linkable(ثابت نویسه *target, صحیح flags, نویسه **ret_path, FILE **ret_file) {
        بازگشت fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

تعریفنوع شمارش LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحیح link_tmpfile_at(صحیح fd, صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *target, LinkTmpfileFlags flags);
ایستا درونخطی صحیح link_tmpfile(صحیح fd, ثابت نویسه *path, ثابت نویسه *target, LinkTmpfileFlags flags) {
        بازگشت link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
صحیح flink_tmpfile_at(FILE *f, صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *target, LinkTmpfileFlags flags);
ایستا درونخطی صحیح flink_tmpfile(FILE *f, ثابت نویسه *path, ثابت نویسه *target, LinkTmpfileFlags flags) {
        بازگشت flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

صحیح mkdtemp_malloc(ثابت نویسه *template, نویسه **ret);
صحیح mkdtemp_open(ثابت نویسه *template, صحیح flags, نویسه **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ساختار cleanup_tmpfile_data {
        صحیح *dir_fd;
        نویسه **filename;
};
تهی cleanup_tmpfile_data_done(ساختار cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) ساختار cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
