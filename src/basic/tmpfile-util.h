/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *لار, FILE **ret_file, حرف **ret_لار);
ساكن مضمن صحيح fopen_temporary(ثابت حرف *لار, FILE **ret_file, حرف **ret_لار) {
        أرجع fopen_temporary_at(AT_FDCWD, لار, ret_file, ret_لار);
}

صحيح fopen_temporary_ماشوم_at(صحيح dir_fd, ثابت حرف *لار, FILE **ret_file, حرف **ret_لار);
ساكن مضمن صحيح fopen_temporary_ماشوم(ثابت حرف *لار, FILE **ret_file, حرف **ret_لار) {
        أرجع fopen_temporary_ماشوم_at(AT_FDCWD, لار, ret_file, ret_لار);
}

صحيح mkostemp_خوندي(حرف *نمونه);
صحيح fmkostemp_خوندي(حرف *نمونه, ثابت حرف *ډېرپېښ, FILE**_f);

فراغ unlink_tempfilep(حرف (*p)[]);

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random_ماشوم(ثابت حرف *p, ثابت حرف *extra, حرف **ret);

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags);
صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *موخه, صحيح flags, حرف **ret_لار);
ساكن مضمن صحيح open_tmpfile_linkable(ثابت حرف *موخه, صحيح flags, حرف **ret_لار) {
        أرجع open_tmpfile_linkable_at(AT_FDCWD, موخه, flags, ret_لار);
}
صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *موخه, صحيح flags, حرف **ret_لار, FILE **ret_file);
ساكن مضمن صحيح fopen_tmpfile_linkable(ثابت حرف *موخه, صحيح flags, حرف **ret_لار, FILE **ret_file) {
        أرجع fopen_tmpfile_linkable_at(AT_FDCWD, موخه, flags, ret_لار, ret_file);
}

تعريفنوع تعداد LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *لار, ثابت حرف *موخه, LinkTmpfileFlags flags);
ساكن مضمن صحيح link_tmpfile(صحيح fd, ثابت حرف *لار, ثابت حرف *موخه, LinkTmpfileFlags flags) {
        أرجع link_tmpfile_at(fd, AT_FDCWD, لار, موخه, flags);
}
صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *لار, ثابت حرف *موخه, LinkTmpfileFlags flags);
ساكن مضمن صحيح flink_tmpfile(FILE *f, ثابت حرف *لار, ثابت حرف *موخه, LinkTmpfileFlags flags) {
        أرجع flink_tmpfile_at(f, AT_FDCWD, لار, موخه, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *کينډۍ, حرف **ret);
صحيح mkdtemp_open(ثابت حرف *کينډۍ, صحيح flags, حرف **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
بنية cleanup_tmpfile_معلومات {
        صحيح *dir_fd;
        حرف **filename;
};
فراغ cleanup_tmpfile_معلومات_بشپړ(بنية cleanup_tmpfile_معلومات *d);
#تعریف_کړه _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        ناکارول_شوی _cleanup_(cleanup_tmpfile_معلومات_بشپړ) بنية cleanup_tmpfile_معلومات UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#تعریف_کړه CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
