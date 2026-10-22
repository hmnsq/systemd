/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

#تعریف_کرو LONG_سطر_MAX (1U*1024U*1024U)

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
        WRITE_STRING_FILE_منوال_0600                  = 1 << 10,
        WRITE_STRING_FILE_منوال_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

تعريفنوع تعداد {
        READ_بھریا_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_بھریا_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_بھریا_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_بھریا_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_بھریا_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_بھریا_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_بھریا_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_بھریا_File_Flags;

صحيح fdopen_unlocked(صحيح fd, ثابت حرف *options, FILE **ret);
صحيح take_fdopen_unlocked(صحيح *fd, ثابت حرف *options, FILE **ret);
FILE* take_fdopen(صحيح *fd, ثابت حرف *options);
DIR* take_fdopendir(صحيح *dfd);
FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(فراغ *buf, size_t ناپ, ثابت حرف *منوال);

صحيح write_string_دھارا_بھریا(FILE *f, ثابت حرف *سطر, WriteStringFileFlags flags, ثابت بنية timespec *ts);
ساكن مضمن صحيح write_string_دھارا(FILE *f, ثابت حرف *سطر, WriteStringFileFlags flags) {
        أرجع write_string_دھارا_بھریا(f, سطر, flags, عدم);
}

صحيح write_string_file_بھریا_label(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn, LabelContext *label_context);
ساكن مضمن صحيح write_string_file_بھریا(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn) {
        أرجع write_string_file_بھریا_label(dir_fd, fn, سطر, flags, ts, label_fn, /* label_context= */ عدم);
}
ساكن مضمن صحيح write_string_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags) {
        أرجع write_string_file_بھریا(dir_fd, fn, سطر, flags, عدم, عدم);
}
ساكن مضمن صحيح write_string_file_fd(صحيح dir_fd, ثابت حرف *سطر, WriteStringFileFlags flags) {
        أرجع write_string_file_at(dir_fd, عدم, سطر, flags);
}
ساكن مضمن صحيح write_string_file(ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags) {
        أرجع write_string_file_at(AT_FDCWD, fn, سطر, flags);
}
صحيح write_string_filef(ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(3, 4);
صحيح write_string_filef_at(صحيح dir_fd, ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(4, 5);

صحيح write_base64_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت بنية iovec *انکڑے, WriteStringFileFlags flags);

صحيح read_one_سطر_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret);
ساكن مضمن صحيح read_one_سطر_file(ثابت حرف *filename, حرف **ret) {
        أرجع read_one_سطر_file_at(AT_FDCWD, filename, ret);
}
صحيح read_boolean_file_at(صحيح dir_fd, ثابت حرف *filename);
ساكن مضمن صحيح read_boolean_file(ثابت حرف *filename) {
        أرجع read_boolean_file_at(AT_FDCWD, filename);
}
صحيح read_بھریا_file_بھریا(صحيح dir_fd, ثابت حرف *filename, uint64_t وتھ, size_t ناپ, Read_بھریا_File_Flags flags, ثابت حرف *bind_name, حرف **ret_contents, size_t *ret_ناپ);
ساكن مضمن صحيح read_بھریا_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret_contents, size_t *ret_ناپ) {
        أرجع read_بھریا_file_بھریا(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_ناپ);
}
ساكن مضمن صحيح read_بھریا_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_ناپ) {
        أرجع read_بھریا_file_بھریا(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_ناپ);
}

صحيح read_virtual_file_at(صحيح dir_fd, ثابت حرف *filename, size_t max_ناپ, حرف **ret_contents, size_t *ret_ناپ);
ساكن مضمن صحيح read_virtual_file_fd(صحيح fd, size_t max_ناپ, حرف **ret_contents, size_t *ret_ناپ) {
        أرجع read_virtual_file_at(fd, عدم, max_ناپ, ret_contents, ret_ناپ);
}
ساكن مضمن صحيح read_virtual_file(ثابت حرف *filename, size_t max_ناپ, حرف **ret_contents, size_t *ret_ناپ) {
        أرجع read_virtual_file_at(AT_FDCWD, filename, max_ناپ, ret_contents, ret_ناپ);
}
ساكن مضمن صحيح read_بھریا_virtual_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_ناپ) {
        أرجع read_virtual_file(filename, SIZE_MAX, ret_contents, ret_ناپ);
}

صحيح read_بھریا_دھارا_بھریا(FILE *f, ثابت حرف *filename, uint64_t وتھ, size_t ناپ, Read_بھریا_File_Flags flags, حرف **ret_contents, size_t *ret_ناپ);
ساكن مضمن صحيح read_بھریا_دھارا(FILE *f, حرف **ret_contents, size_t *ret_ناپ) {
        أرجع read_بھریا_دھارا_بھریا(f, عدم, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_ناپ);
}

صحيح verify_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl);

صحيح script_get_shebang_interpreter(ثابت حرف *رستہ, حرف **ret);

صحيح get_proc_میدان(ثابت حرف *رستہ, ثابت حرف *کلید, حرف **ret);

DIR* xopendirat(صحيح dir_fd, ثابت حرف *رستہ, صحيح flags);

تعريفنوع تعداد XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

صحيح xfopenat_بھریا(
                صحيح dir_fd,
                ثابت حرف *رستہ,
                ثابت حرف *منوال,
                صحيح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret);
ساكن مضمن صحيح xfopenat(صحيح dir_fd, ثابت حرف *رستہ, ثابت حرف *منوال, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_بھریا(dir_fd, رستہ, منوال, open_flags, 0, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked_at(صحيح dir_fd, ثابت حرف *رستہ, ثابت حرف *منوال, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_بھریا(dir_fd, رستہ, منوال, open_flags, XFOPEN_UNLOCKED, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked(ثابت حرف *رستہ, ثابت حرف *منوال, FILE **ret) {
        أرجع fopen_unlocked_at(AT_FDCWD, رستہ, منوال, 0, ret);
}

صحيح fdopen_independent(صحيح fd, ثابت حرف *منوال, FILE **ret);

صحيح تلاش_and_open(ثابت حرف *رستہ, صحيح منوال, ثابت حرف *جڑ, حرف **تلاش, صحيح *ret_fd, حرف **ret_رستہ);
ساكن مضمن صحيح تلاش_and_access(ثابت حرف *رستہ, صحيح منوال, ثابت حرف *جڑ, حرف**تلاش, حرف **ret_رستہ) {
        أرجع تلاش_and_open(رستہ, منوال, جڑ, تلاش, عدم, ret_رستہ);
}
صحيح تلاش_and_fopen(ثابت حرف *رستہ, ثابت حرف *منوال, ثابت حرف *جڑ, ثابت حرف **تلاش, FILE **ret_file, حرف **ret_رستہ);
صحيح تلاش_and_fopen_nulstr(ثابت حرف *رستہ, ثابت حرف *منوال, ثابت حرف *جڑ, ثابت حرف *تلاش, FILE **ret_file, حرف **ret_رستہ);

صحيح fflush_and_check(FILE *f);
صحيح fflush_sync_and_check(FILE *f);

صحيح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n);
صحيح read_timestamp_file(ثابت حرف *fn, usec_t *ret);

صحيح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space);
صحيح fputs_with_newline(FILE *f, ثابت حرف *s);

تعريفنوع تعداد Read_سطر_Flags {
        READ_سطر_ONLY_NUL  = 1 << 0,
        READ_سطر_IS_A_TTY  = 1 << 1,
        READ_سطر_NOT_A_TTY = 1 << 2,
} Read_سطر_Flags;

صحيح read_سطر_بھریا(FILE *f, size_t limit, Read_سطر_Flags flags, حرف **ret);
ساكن مضمن صحيح read_سطر(FILE *f, size_t limit, حرف **ret) {
        أرجع read_سطر_بھریا(f, limit, 0, ret);
}
ساكن مضمن صحيح read_nul_string(FILE *f, size_t limit, حرف **ret) {
        أرجع read_سطر_بھریا(f, limit, READ_سطر_ONLY_NUL, ret);
}

صحيح read_stripped_سطر(FILE *f, size_t limit, حرف **ret);

ساكن مضمن bool file_وتھ_beyond_حافظہ_ناپ(off_t x) {
        إذا (x < 0) /* off_t is signed, filter that out */
                أرجع false;
        أرجع (uint64_t) x > (uint64_t) SIZE_MAX;
}

صحيح محفوظ_fgetc(FILE *f, حرف *ret);

صحيح warn_file_is_world_accessible(ثابت حرف *filename, بنية stat *st, ثابت حرف *unit, بنانشانی سطر);

صحيح fopen_منوال_منزل_flags(ثابت حرف *منوال);

تعريفنوع تعداد Write_انکڑے_File_Flags {
        WRITE_انکڑے_FILE_MKDIR_0755 = 1 << 0,
        WRITE_انکڑے_FILE_منوال_0400  = 1 << 1,
} Write_انکڑے_File_Flags;

صحيح write_انکڑے_file_atomic_at(صحيح dir_fd, ثابت حرف *رستہ, ثابت بنية iovec *iovec, Write_انکڑے_File_Flags flags);
