/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

صحيح fopen_temporary_at(صحيح dir_fd, ثابت حرف *رستہ, FILE **ret_file, حرف **ret_رستہ);
ساكن مضمن صحيح fopen_temporary(ثابت حرف *رستہ, FILE **ret_file, حرف **ret_رستہ) {
        أرجع fopen_temporary_at(AT_FDCWD, رستہ, ret_file, ret_رستہ);
}

صحيح fopen_temporary_بچہ_at(صحيح dir_fd, ثابت حرف *رستہ, FILE **ret_file, حرف **ret_رستہ);
ساكن مضمن صحيح fopen_temporary_بچہ(ثابت حرف *رستہ, FILE **ret_file, حرف **ret_رستہ) {
        أرجع fopen_temporary_بچہ_at(AT_FDCWD, رستہ, ret_file, ret_رستہ);
}

صحيح mkostemp_محفوظ(حرف *نمونہ);
صحيح fmkostemp_محفوظ(حرف *نمونہ, ثابت حرف *منوال, FILE**_f);

فراغ unlink_tempfilep(حرف (*p)[]);

صحيح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحيح tempfn_random_بچہ(ثابت حرف *p, ثابت حرف *extra, حرف **ret);

صحيح open_tmpfile_unlinkable(ثابت حرف *directory, صحيح flags);
صحيح open_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *نشانہ, صحيح flags, حرف **ret_رستہ);
ساكن مضمن صحيح open_tmpfile_linkable(ثابت حرف *نشانہ, صحيح flags, حرف **ret_رستہ) {
        أرجع open_tmpfile_linkable_at(AT_FDCWD, نشانہ, flags, ret_رستہ);
}
صحيح fopen_tmpfile_linkable_at(صحيح dir_fd, ثابت حرف *نشانہ, صحيح flags, حرف **ret_رستہ, FILE **ret_file);
ساكن مضمن صحيح fopen_tmpfile_linkable(ثابت حرف *نشانہ, صحيح flags, حرف **ret_رستہ, FILE **ret_file) {
        أرجع fopen_tmpfile_linkable_at(AT_FDCWD, نشانہ, flags, ret_رستہ, ret_file);
}

تعريفنوع تعداد LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحيح link_tmpfile_at(صحيح fd, صحيح dir_fd, ثابت حرف *رستہ, ثابت حرف *نشانہ, LinkTmpfileFlags flags);
ساكن مضمن صحيح link_tmpfile(صحيح fd, ثابت حرف *رستہ, ثابت حرف *نشانہ, LinkTmpfileFlags flags) {
        أرجع link_tmpfile_at(fd, AT_FDCWD, رستہ, نشانہ, flags);
}
صحيح flink_tmpfile_at(FILE *f, صحيح dir_fd, ثابت حرف *رستہ, ثابت حرف *نشانہ, LinkTmpfileFlags flags);
ساكن مضمن صحيح flink_tmpfile(FILE *f, ثابت حرف *رستہ, ثابت حرف *نشانہ, LinkTmpfileFlags flags) {
        أرجع flink_tmpfile_at(f, AT_FDCWD, رستہ, نشانہ, flags);
}

صحيح mkdtemp_malloc(ثابت حرف *سانچا, حرف **ret);
صحيح mkdtemp_open(ثابت حرف *سانچا, صحيح flags, حرف **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
بنية cleanup_tmpfile_انکڑے {
        صحيح *dir_fd;
        حرف **filename;
};
فراغ cleanup_tmpfile_انکڑے_پورا(بنية cleanup_tmpfile_انکڑے *d);
#تعریف_کرو _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        بن_ورتیا _cleanup_(cleanup_tmpfile_انکڑے_پورا) بنية cleanup_tmpfile_انکڑے UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#تعریف_کرو CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
