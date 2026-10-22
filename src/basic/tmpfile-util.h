/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *مسیر, FILE **ret_file, حرف **ret_مسیر);
ساكن مضمن صحيح fopen_temporary(ثابت حرف *مسیر, FILE **ret_file, حرف **ret_مسیر) {
        أرجع fopen_temporary_at(AT_FDCWD, مسیر, ret_file, ret_مسیر);
}

صحيح fopen_temporary_فرزند_at(صحيح dir_fd, ثابت حرف *مسیر, FILE **ret_file, حرف **ret_مسیر);
ساكن مضمن صحيح fopen_temporary_فرزند(ثابت حرف *مسیر, FILE **ret_file, حرف **ret_مسیر) {
        أرجع fopen_temporary_فرزند_at(AT_FDCWD, مسیر, ret_file, ret_مسیر);
}

صحيح mkostemp_امن(حرف *الگو);
صحيح fmkostemp_امن(حرف *الگو, ثابت حرف *نما, FILE**_f);

فراغ unlink_tempfilep(حرف (*p)[]);

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random_فرزند(ثابت حرف *p, ثابت حرف *extra, حرف **ret);

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags);
صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *هدف, صحيح flags, حرف **ret_مسیر);
ساكن مضمن صحيح open_tmpfile_linkable(ثابت حرف *هدف, صحيح flags, حرف **ret_مسیر) {
        أرجع open_tmpfile_linkable_at(AT_FDCWD, هدف, flags, ret_مسیر);
}
صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *هدف, صحيح flags, حرف **ret_مسیر, FILE **ret_file);
ساكن مضمن صحيح fopen_tmpfile_linkable(ثابت حرف *هدف, صحيح flags, حرف **ret_مسیر, FILE **ret_file) {
        أرجع fopen_tmpfile_linkable_at(AT_FDCWD, هدف, flags, ret_مسیر, ret_file);
}

تعريفنوع تعداد LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *مسیر, ثابت حرف *هدف, LinkTmpfileFlags flags);
ساكن مضمن صحيح link_tmpfile(صحيح fd, ثابت حرف *مسیر, ثابت حرف *هدف, LinkTmpfileFlags flags) {
        أرجع link_tmpfile_at(fd, AT_FDCWD, مسیر, هدف, flags);
}
صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *مسیر, ثابت حرف *هدف, LinkTmpfileFlags flags);
ساكن مضمن صحيح flink_tmpfile(FILE *f, ثابت حرف *مسیر, ثابت حرف *هدف, LinkTmpfileFlags flags) {
        أرجع flink_tmpfile_at(f, AT_FDCWD, مسیر, هدف, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *قالب, حرف **ret);
صحيح mkdtemp_open(ثابت حرف *قالب, صحيح flags, حرف **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
بنية cleanup_tmpfile_داده {
        صحيح *dir_fd;
        حرف **filename;
};
فراغ cleanup_tmpfile_داده_انجامشده(بنية cleanup_tmpfile_داده *d);
#تعریف_کن _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        استفادهنشده _cleanup_(cleanup_tmpfile_داده_انجامشده) بنية cleanup_tmpfile_داده UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#تعریف_کن CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
