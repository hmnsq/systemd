/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *رستو, FILE **ret_file, حرف **ret_رستو);
ساكن مضمن صحيح fopen_temporary(ثابت حرف *رستو, FILE **ret_file, حرف **ret_رستو) {
        أرجع fopen_temporary_at(AT_FDCWD, رستو, ret_file, ret_رستو);
}

صحيح fopen_temporary_ٻار_at(صحيح dir_fd, ثابت حرف *رستو, FILE **ret_file, حرف **ret_رستو);
ساكن مضمن صحيح fopen_temporary_ٻار(ثابت حرف *رستو, FILE **ret_file, حرف **ret_رستو) {
        أرجع fopen_temporary_ٻار_at(AT_FDCWD, رستو, ret_file, ret_رستو);
}

صحيح mkostemp_محفوظ(حرف *نمونو);
صحيح fmkostemp_محفوظ(حرف *نمونو, ثابت حرف *گهڻائي, FILE**_f);

فراغ unlink_tempfilep(حرف (*p)[]);

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random_ٻار(ثابت حرف *p, ثابت حرف *extra, حرف **ret);

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags);
صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *هدف, صحيح flags, حرف **ret_رستو);
ساكن مضمن صحيح open_tmpfile_linkable(ثابت حرف *هدف, صحيح flags, حرف **ret_رستو) {
        أرجع open_tmpfile_linkable_at(AT_FDCWD, هدف, flags, ret_رستو);
}
صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *هدف, صحيح flags, حرف **ret_رستو, FILE **ret_file);
ساكن مضمن صحيح fopen_tmpfile_linkable(ثابت حرف *هدف, صحيح flags, حرف **ret_رستو, FILE **ret_file) {
        أرجع fopen_tmpfile_linkable_at(AT_FDCWD, هدف, flags, ret_رستو, ret_file);
}

تعريفنوع تعداد LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *رستو, ثابت حرف *هدف, LinkTmpfileFlags flags);
ساكن مضمن صحيح link_tmpfile(صحيح fd, ثابت حرف *رستو, ثابت حرف *هدف, LinkTmpfileFlags flags) {
        أرجع link_tmpfile_at(fd, AT_FDCWD, رستو, هدف, flags);
}
صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *رستو, ثابت حرف *هدف, LinkTmpfileFlags flags);
ساكن مضمن صحيح flink_tmpfile(FILE *f, ثابت حرف *رستو, ثابت حرف *هدف, LinkTmpfileFlags flags) {
        أرجع flink_tmpfile_at(f, AT_FDCWD, رستو, هدف, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *سانچو, حرف **ret);
صحيح mkdtemp_open(ثابت حرف *سانچو, صحيح flags, حرف **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
بنية cleanup_tmpfile_معلومات {
        صحيح *dir_fd;
        حرف **filename;
};
فراغ cleanup_tmpfile_معلومات_پورو(بنية cleanup_tmpfile_معلومات *d);
#وصف_ڪريو _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        اڻ_استعمال_ٿيل _cleanup_(cleanup_tmpfile_معلومات_پورو) بنية cleanup_tmpfile_معلومات UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#وصف_ڪريو CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
