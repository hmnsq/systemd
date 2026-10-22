/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#وصف_ڪريو LONG_سٽ_MAX (1U*1024U*1024U)

تعريفنوع تعداد {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_بفر             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_گهڻائي_0600                  = 1 << 10,
        WRITE_STRING_FILE_گهڻائي_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

تعريفنوع تعداد {
        READ_ڀريل_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_ڀريل_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_ڀريل_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_ڀريل_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_ڀريل_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_ڀريل_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_ڀريل_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_ڀريل_File_Flags;

صحيح fdopen_unlocked(صحيح fd, ثابت حرف *options, FILE **ret);
صحيح take_fdopen_unlocked(صحيح *fd, ثابت حرف *options, FILE **ret);
FILE* take_fdopen(صحيح *fd, ثابت حرف *options);
DIR* take_fdopendir(صحيح *dfd);
FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(فراغ *buf, size_t سائيز, ثابت حرف *گهڻائي);

صحيح write_string_وهڪرو_ڀريل(FILE *f, ثابت حرف *سٽ, WriteStringFileFlags flags, ثابت بنية timespec *ts);
ساكن مضمن صحيح write_string_وهڪرو(FILE *f, ثابت حرف *سٽ, WriteStringFileFlags flags) {
        أرجع write_string_وهڪرو_ڀريل(f, سٽ, flags, عدم);
}

صحيح write_string_file_ڀريل_label(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *سٽ, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn, LabelContext *label_context);
ساكن مضمن صحيح write_string_file_ڀريل(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *سٽ, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn) {
        أرجع write_string_file_ڀريل_label(dir_fd, fn, سٽ, flags, ts, label_fn, /* label_context= */ عدم);
}
ساكن مضمن صحيح write_string_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *سٽ, WriteStringFileFlags flags) {
        أرجع write_string_file_ڀريل(dir_fd, fn, سٽ, flags, عدم, عدم);
}
ساكن مضمن صحيح write_string_file_fd(صحيح dir_fd, ثابت حرف *سٽ, WriteStringFileFlags flags) {
        أرجع write_string_file_at(dir_fd, عدم, سٽ, flags);
}
ساكن مضمن صحيح write_string_file(ثابت حرف *fn, ثابت حرف *سٽ, WriteStringFileFlags flags) {
        أرجع write_string_file_at(AT_FDCWD, fn, سٽ, flags);
}
صحيح write_string_filef(ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(3, 4);
صحيح write_string_filef_at(صحيح dir_fd, ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(4, 5);

صحيح write_base64_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت بنية iovec *معلومات, WriteStringFileFlags flags);

صحيح read_one_سٽ_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret);
ساكن مضمن صحيح read_one_سٽ_file(ثابت حرف *filename, حرف **ret) {
        أرجع read_one_سٽ_file_at(AT_FDCWD, filename, ret);
}
صحيح read_boolean_file_at(صحيح dir_fd, ثابت حرف *filename);
ساكن مضمن صحيح read_boolean_file(ثابت حرف *filename) {
        أرجع read_boolean_file_at(AT_FDCWD, filename);
}
صحيح read_ڀريل_file_ڀريل(صحيح dir_fd, ثابت حرف *filename, uint64_t وٿي, size_t سائيز, Read_ڀريل_File_Flags flags, ثابت حرف *bind_name, حرف **ret_contents, size_t *ret_سائيز);
ساكن مضمن صحيح read_ڀريل_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret_contents, size_t *ret_سائيز) {
        أرجع read_ڀريل_file_ڀريل(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_سائيز);
}
ساكن مضمن صحيح read_ڀريل_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_سائيز) {
        أرجع read_ڀريل_file_ڀريل(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_سائيز);
}

صحيح read_virtual_file_at(صحيح dir_fd, ثابت حرف *filename, size_t max_سائيز, حرف **ret_contents, size_t *ret_سائيز);
ساكن مضمن صحيح read_virtual_file_fd(صحيح fd, size_t max_سائيز, حرف **ret_contents, size_t *ret_سائيز) {
        أرجع read_virtual_file_at(fd, عدم, max_سائيز, ret_contents, ret_سائيز);
}
ساكن مضمن صحيح read_virtual_file(ثابت حرف *filename, size_t max_سائيز, حرف **ret_contents, size_t *ret_سائيز) {
        أرجع read_virtual_file_at(AT_FDCWD, filename, max_سائيز, ret_contents, ret_سائيز);
}
ساكن مضمن صحيح read_ڀريل_virtual_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_سائيز) {
        أرجع read_virtual_file(filename, SIZE_MAX, ret_contents, ret_سائيز);
}

صحيح read_ڀريل_وهڪرو_ڀريل(FILE *f, ثابت حرف *filename, uint64_t وٿي, size_t سائيز, Read_ڀريل_File_Flags flags, حرف **ret_contents, size_t *ret_سائيز);
ساكن مضمن صحيح read_ڀريل_وهڪرو(FILE *f, حرف **ret_contents, size_t *ret_سائيز) {
        أرجع read_ڀريل_وهڪرو_ڀريل(f, عدم, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_سائيز);
}

صحيح verify_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl);

صحيح script_get_shebang_interpreter(ثابت حرف *رستو, حرف **ret);

صحيح get_proc_ميدان(ثابت حرف *رستو, ثابت حرف *ڪنجي, حرف **ret);

DIR* xopendirat(صحيح dir_fd, ثابت حرف *رستو, صحيح flags);

تعريفنوع تعداد XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

صحيح xfopenat_ڀريل(
                صحيح dir_fd,
                ثابت حرف *رستو,
                ثابت حرف *گهڻائي,
                صحيح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret);
ساكن مضمن صحيح xfopenat(صحيح dir_fd, ثابت حرف *رستو, ثابت حرف *گهڻائي, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_ڀريل(dir_fd, رستو, گهڻائي, open_flags, 0, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked_at(صحيح dir_fd, ثابت حرف *رستو, ثابت حرف *گهڻائي, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_ڀريل(dir_fd, رستو, گهڻائي, open_flags, XFOPEN_UNLOCKED, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked(ثابت حرف *رستو, ثابت حرف *گهڻائي, FILE **ret) {
        أرجع fopen_unlocked_at(AT_FDCWD, رستو, گهڻائي, 0, ret);
}

صحيح fdopen_independent(صحيح fd, ثابت حرف *گهڻائي, FILE **ret);

صحيح ڳولا_and_open(ثابت حرف *رستو, صحيح گهڻائي, ثابت حرف *پاڙ, حرف **ڳولا, صحيح *ret_fd, حرف **ret_رستو);
ساكن مضمن صحيح ڳولا_and_access(ثابت حرف *رستو, صحيح گهڻائي, ثابت حرف *پاڙ, حرف**ڳولا, حرف **ret_رستو) {
        أرجع ڳولا_and_open(رستو, گهڻائي, پاڙ, ڳولا, عدم, ret_رستو);
}
صحيح ڳولا_and_fopen(ثابت حرف *رستو, ثابت حرف *گهڻائي, ثابت حرف *پاڙ, ثابت حرف **ڳولا, FILE **ret_file, حرف **ret_رستو);
صحيح ڳولا_and_fopen_nulstr(ثابت حرف *رستو, ثابت حرف *گهڻائي, ثابت حرف *پاڙ, ثابت حرف *ڳولا, FILE **ret_file, حرف **ret_رستو);

صحيح fflush_and_check(FILE *f);
صحيح fflush_sync_and_check(FILE *f);

صحيح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n);
صحيح read_timestamp_file(ثابت حرف *fn, usec_t *ret);

صحيح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space);
صحيح fputs_with_newline(FILE *f, ثابت حرف *s);

تعريفنوع تعداد Read_سٽ_Flags {
        READ_سٽ_ONLY_NUL  = 1 << 0,
        READ_سٽ_IS_A_TTY  = 1 << 1,
        READ_سٽ_NOT_A_TTY = 1 << 2,
} Read_سٽ_Flags;

صحيح read_سٽ_ڀريل(FILE *f, size_t limit, Read_سٽ_Flags flags, حرف **ret);
ساكن مضمن صحيح read_سٽ(FILE *f, size_t limit, حرف **ret) {
        أرجع read_سٽ_ڀريل(f, limit, 0, ret);
}
ساكن مضمن صحيح read_nul_string(FILE *f, size_t limit, حرف **ret) {
        أرجع read_سٽ_ڀريل(f, limit, READ_سٽ_ONLY_NUL, ret);
}

صحيح read_stripped_سٽ(FILE *f, size_t limit, حرف **ret);

ساكن مضمن bool file_وٿي_beyond_يادگيري_سائيز(off_t x) {
        إذا (x < 0) /* off_t is signed, filter that out */
                أرجع false;
        أرجع (uint64_t) x > (uint64_t) SIZE_MAX;
}

صحيح محفوظ_fgetc(FILE *f, حرف *ret);

صحيح warn_file_is_world_accessible(ثابت حرف *filename, بنية stat *st, ثابت حرف *unit, بنانشان سٽ);

صحيح fopen_گهڻائي_منزل_flags(ثابت حرف *گهڻائي);

تعريفنوع تعداد Write_معلومات_File_Flags {
        WRITE_معلومات_FILE_MKDIR_0755 = 1 << 0,
        WRITE_معلومات_FILE_گهڻائي_0400  = 1 << 1,
} Write_معلومات_File_Flags;

صحيح write_معلومات_file_atomic_at(صحيح dir_fd, ثابت حرف *رستو, ثابت بنية iovec *iovec, Write_معلومات_File_Flags flags);
