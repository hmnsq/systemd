/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

صحیح fopen_temporary_at(صحیح dir_fd, ثابت حرف *راستہ, FILE **ret_file, حرف **ret_راستہ);
ساکن اندرونی صحیح fopen_temporary(ثابت حرف *راستہ, FILE **ret_file, حرف **ret_راستہ) {
        واپس fopen_temporary_at(AT_FDCWD, راستہ, ret_file, ret_راستہ);
}

صحیح fopen_temporary_بچہ_at(صحیح dir_fd, ثابت حرف *راستہ, FILE **ret_file, حرف **ret_راستہ);
ساکن اندرونی صحیح fopen_temporary_بچہ(ثابت حرف *راستہ, FILE **ret_file, حرف **ret_راستہ) {
        واپس fopen_temporary_بچہ_at(AT_FDCWD, راستہ, ret_file, ret_راستہ);
}

صحیح mkostemp_محفوظ(حرف *نمونہ);
صحیح fmkostemp_محفوظ(حرف *نمونہ, ثابت حرف *منوال, FILE**_f);

خالی unlink_tempfilep(حرف (*p)[]);

صحیح tempfn_xxxxxx(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحیح tempfn_random(ثابت حرف *p, ثابت حرف *extra, حرف **ret);
صحیح tempfn_random_بچہ(ثابت حرف *p, ثابت حرف *extra, حرف **ret);

صحیح open_tmpfile_unlinkable(ثابت حرف *directory, صحیح flags);
صحیح open_tmpfile_linkable_at(صحیح dir_fd, ثابت حرف *ہدف, صحیح flags, حرف **ret_راستہ);
ساکن اندرونی صحیح open_tmpfile_linkable(ثابت حرف *ہدف, صحیح flags, حرف **ret_راستہ) {
        واپس open_tmpfile_linkable_at(AT_FDCWD, ہدف, flags, ret_راستہ);
}
صحیح fopen_tmpfile_linkable_at(صحیح dir_fd, ثابت حرف *ہدف, صحیح flags, حرف **ret_راستہ, FILE **ret_file);
ساکن اندرونی صحیح fopen_tmpfile_linkable(ثابت حرف *ہدف, صحیح flags, حرف **ret_راستہ, FILE **ret_file) {
        واپس fopen_tmpfile_linkable_at(AT_FDCWD, ہدف, flags, ret_راستہ, ret_file);
}

قسمتعریف شمار LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

صحیح link_tmpfile_at(صحیح fd, صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *ہدف, LinkTmpfileFlags flags);
ساکن اندرونی صحیح link_tmpfile(صحیح fd, ثابت حرف *راستہ, ثابت حرف *ہدف, LinkTmpfileFlags flags) {
        واپس link_tmpfile_at(fd, AT_FDCWD, راستہ, ہدف, flags);
}
صحیح flink_tmpfile_at(FILE *f, صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *ہدف, LinkTmpfileFlags flags);
ساکن اندرونی صحیح flink_tmpfile(FILE *f, ثابت حرف *راستہ, ثابت حرف *ہدف, LinkTmpfileFlags flags) {
        واپس flink_tmpfile_at(f, AT_FDCWD, راستہ, ہدف, flags);
}

صحیح mkdtemp_malloc(ثابت حرف *سانچہ, حرف **ret);
صحیح mkdtemp_open(ثابت حرف *سانچہ, صحیح flags, حرف **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ساخت cleanup_tmpfile_ڈیٹا {
        صحیح *dir_fd;
        حرف **filename;
};
خالی cleanup_tmpfile_ڈیٹا_مکمل(ساخت cleanup_tmpfile_ڈیٹا *d);
#تعریف_کرو _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        غیر_مستعمل _cleanup_(cleanup_tmpfile_ڈیٹا_مکمل) ساخت cleanup_tmpfile_ڈیٹا UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#تعریف_کرو CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
