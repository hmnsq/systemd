/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *path, FILE **ret_file, حرف **ret_path);
ساكن مضمن صحيح fopen_temporary(ثابت حرف *path, FILE **ret_file, حرف **ret_path) {
        إرجاع fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

صحيح fopen_temporary_child_at(صحيح dir_fd, ثابت حرف *path, FILE **ret_file, حرف **ret_path);
ساكن مضمن صحيح fopen_temporary_child(ثابت حرف *path, FILE **ret_file, حرف **ret_path) {
        إرجاع fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

صحيح mkostemp_safe(حرف *pattern);
صحيح fmkostemp_safe(حرف *pattern, ثابت حرف *mode, FILE**_f);

فراغ unlink_tempfilep(حرف (*p)[]);

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random_child(ثابت حرف *p, ثابت حرف *extra, حرف **ret);

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags);
صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *target, صحيح flags, حرف **ret_path);
ساكن مضمن صحيح open_tmpfile_linkable(ثابت حرف *target, صحيح flags, حرف **ret_path) {
        إرجاع open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *target, صحيح flags, حرف **ret_path, FILE **ret_file);
ساكن مضمن صحيح fopen_tmpfile_linkable(ثابت حرف *target, صحيح flags, حرف **ret_path, FILE **ret_file) {
        إرجاع fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

تعريفنوع تعداد LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags);
ساكن مضمن صحيح link_tmpfile(صحيح fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags) {
        إرجاع link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags);
ساكن مضمن صحيح flink_tmpfile(FILE *f, ثابت حرف *path, ثابت حرف *target, LinkTmpfileFlags flags) {
        إرجاع flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *template, حرف **ret);
صحيح mkdtemp_open(ثابت حرف *template, صحيح flags, حرف **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
بنية cleanup_tmpfile_data {
        صحيح *dir_fd;
        حرف **filename;
};
فراغ cleanup_tmpfile_data_done(بنية cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) بنية cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
