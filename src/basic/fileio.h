/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو LONG_سطر_MAX (1U*1024U*1024U)

قسمتعریف شمار {
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

قسمتعریف شمار {
        READ_بھرا_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_بھرا_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_بھرا_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_بھرا_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_بھرا_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_بھرا_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_بھرا_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_بھرا_File_Flags;

صحیح fdopen_unlocked(صحیح fd, ثابت حرف *options, FILE **ret);
صحیح take_fdopen_unlocked(صحیح *fd, ثابت حرف *options, FILE **ret);
FILE* take_fdopen(صحیح *fd, ثابت حرف *options);
DIR* take_fdopendir(صحیح *dfd);
FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(خالی *buf, size_t سائز, ثابت حرف *منوال);

صحیح write_string_دھارا_بھرا(FILE *f, ثابت حرف *سطر, WriteStringFileFlags flags, ثابت ساخت timespec *ts);
ساکن اندرونی صحیح write_string_دھارا(FILE *f, ثابت حرف *سطر, WriteStringFileFlags flags) {
        واپس write_string_دھارا_بھرا(f, سطر, flags, تہی);
}

صحیح write_string_file_بھرا_label(صحیح dir_fd, ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags, ثابت ساخت timespec *ts, ثابت حرف *label_fn, LabelContext *label_context);
ساکن اندرونی صحیح write_string_file_بھرا(صحیح dir_fd, ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags, ثابت ساخت timespec *ts, ثابت حرف *label_fn) {
        واپس write_string_file_بھرا_label(dir_fd, fn, سطر, flags, ts, label_fn, /* label_context= */ تہی);
}
ساکن اندرونی صحیح write_string_file_at(صحیح dir_fd, ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags) {
        واپس write_string_file_بھرا(dir_fd, fn, سطر, flags, تہی, تہی);
}
ساکن اندرونی صحیح write_string_file_fd(صحیح dir_fd, ثابت حرف *سطر, WriteStringFileFlags flags) {
        واپس write_string_file_at(dir_fd, تہی, سطر, flags);
}
ساکن اندرونی صحیح write_string_file(ثابت حرف *fn, ثابت حرف *سطر, WriteStringFileFlags flags) {
        واپس write_string_file_at(AT_FDCWD, fn, سطر, flags);
}
صحیح write_string_filef(ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(3, 4);
صحیح write_string_filef_at(صحیح dir_fd, ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(4, 5);

صحیح write_base64_file_at(صحیح dir_fd, ثابت حرف *fn, ثابت ساخت iovec *ڈیٹا, WriteStringFileFlags flags);

صحیح read_one_سطر_file_at(صحیح dir_fd, ثابت حرف *filename, حرف **ret);
ساکن اندرونی صحیح read_one_سطر_file(ثابت حرف *filename, حرف **ret) {
        واپس read_one_سطر_file_at(AT_FDCWD, filename, ret);
}
صحیح read_boolean_file_at(صحیح dir_fd, ثابت حرف *filename);
ساکن اندرونی صحیح read_boolean_file(ثابت حرف *filename) {
        واپس read_boolean_file_at(AT_FDCWD, filename);
}
صحیح read_بھرا_file_بھرا(صحیح dir_fd, ثابت حرف *filename, uint64_t انحراف, size_t سائز, Read_بھرا_File_Flags flags, ثابت حرف *bind_name, حرف **ret_contents, size_t *ret_سائز);
ساکن اندرونی صحیح read_بھرا_file_at(صحیح dir_fd, ثابت حرف *filename, حرف **ret_contents, size_t *ret_سائز) {
        واپس read_بھرا_file_بھرا(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, تہی, ret_contents, ret_سائز);
}
ساکن اندرونی صحیح read_بھرا_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_سائز) {
        واپس read_بھرا_file_بھرا(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, تہی, ret_contents, ret_سائز);
}

صحیح read_virtual_file_at(صحیح dir_fd, ثابت حرف *filename, size_t max_سائز, حرف **ret_contents, size_t *ret_سائز);
ساکن اندرونی صحیح read_virtual_file_fd(صحیح fd, size_t max_سائز, حرف **ret_contents, size_t *ret_سائز) {
        واپس read_virtual_file_at(fd, تہی, max_سائز, ret_contents, ret_سائز);
}
ساکن اندرونی صحیح read_virtual_file(ثابت حرف *filename, size_t max_سائز, حرف **ret_contents, size_t *ret_سائز) {
        واپس read_virtual_file_at(AT_FDCWD, filename, max_سائز, ret_contents, ret_سائز);
}
ساکن اندرونی صحیح read_بھرا_virtual_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_سائز) {
        واپس read_virtual_file(filename, SIZE_MAX, ret_contents, ret_سائز);
}

صحیح read_بھرا_دھارا_بھرا(FILE *f, ثابت حرف *filename, uint64_t انحراف, size_t سائز, Read_بھرا_File_Flags flags, حرف **ret_contents, size_t *ret_سائز);
ساکن اندرونی صحیح read_بھرا_دھارا(FILE *f, حرف **ret_contents, size_t *ret_سائز) {
        واپس read_بھرا_دھارا_بھرا(f, تہی, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_سائز);
}

صحیح verify_file_at(صحیح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl);

صحیح script_get_shebang_interpreter(ثابت حرف *راستہ, حرف **ret);

صحیح get_proc_میدان(ثابت حرف *راستہ, ثابت حرف *کلید, حرف **ret);

DIR* xopendirat(صحیح dir_fd, ثابت حرف *راستہ, صحیح flags);

قسمتعریف شمار XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

صحیح xfopenat_بھرا(
                صحیح dir_fd,
                ثابت حرف *راستہ,
                ثابت حرف *منوال,
                صحیح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret);
ساکن اندرونی صحیح xfopenat(صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *منوال, صحیح open_flags, FILE **ret) {
        واپس xfopenat_بھرا(dir_fd, راستہ, منوال, open_flags, 0, تہی, ret);
}
ساکن اندرونی صحیح fopen_unlocked_at(صحیح dir_fd, ثابت حرف *راستہ, ثابت حرف *منوال, صحیح open_flags, FILE **ret) {
        واپس xfopenat_بھرا(dir_fd, راستہ, منوال, open_flags, XFOPEN_UNLOCKED, تہی, ret);
}
ساکن اندرونی صحیح fopen_unlocked(ثابت حرف *راستہ, ثابت حرف *منوال, FILE **ret) {
        واپس fopen_unlocked_at(AT_FDCWD, راستہ, منوال, 0, ret);
}

صحیح fdopen_independent(صحیح fd, ثابت حرف *منوال, FILE **ret);

صحیح تلاش_and_open(ثابت حرف *راستہ, صحیح منوال, ثابت حرف *جڑ, حرف **تلاش, صحیح *ret_fd, حرف **ret_راستہ);
ساکن اندرونی صحیح تلاش_and_access(ثابت حرف *راستہ, صحیح منوال, ثابت حرف *جڑ, حرف**تلاش, حرف **ret_راستہ) {
        واپس تلاش_and_open(راستہ, منوال, جڑ, تلاش, تہی, ret_راستہ);
}
صحیح تلاش_and_fopen(ثابت حرف *راستہ, ثابت حرف *منوال, ثابت حرف *جڑ, ثابت حرف **تلاش, FILE **ret_file, حرف **ret_راستہ);
صحیح تلاش_and_fopen_nulstr(ثابت حرف *راستہ, ثابت حرف *منوال, ثابت حرف *جڑ, ثابت حرف *تلاش, FILE **ret_file, حرف **ret_راستہ);

صحیح fflush_and_check(FILE *f);
صحیح fflush_sync_and_check(FILE *f);

صحیح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n);
صحیح read_timestamp_file(ثابت حرف *fn, usec_t *ret);

صحیح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space);
صحیح fputs_with_newline(FILE *f, ثابت حرف *s);

قسمتعریف شمار Read_سطر_Flags {
        READ_سطر_ONLY_NUL  = 1 << 0,
        READ_سطر_IS_A_TTY  = 1 << 1,
        READ_سطر_NOT_A_TTY = 1 << 2,
} Read_سطر_Flags;

صحیح read_سطر_بھرا(FILE *f, size_t limit, Read_سطر_Flags flags, حرف **ret);
ساکن اندرونی صحیح read_سطر(FILE *f, size_t limit, حرف **ret) {
        واپس read_سطر_بھرا(f, limit, 0, ret);
}
ساکن اندرونی صحیح read_nul_string(FILE *f, size_t limit, حرف **ret) {
        واپس read_سطر_بھرا(f, limit, READ_سطر_ONLY_NUL, ret);
}

صحیح read_stripped_سطر(FILE *f, size_t limit, حرف **ret);

ساکن اندرونی bool file_انحراف_beyond_حافظہ_سائز(off_t x) {
        اگر (x < 0) /* off_t is signed, filter that out */
                واپس false;
        واپس (uint64_t) x > (uint64_t) SIZE_MAX;
}

صحیح محفوظ_fgetc(FILE *f, حرف *ret);

صحیح warn_file_is_world_accessible(ثابت حرف *filename, ساخت stat *st, ثابت حرف *unit, غیرعلامتی سطر);

صحیح fopen_منوال_مقصد_flags(ثابت حرف *منوال);

قسمتعریف شمار Write_ڈیٹا_File_Flags {
        WRITE_ڈیٹا_FILE_MKDIR_0755 = 1 << 0,
        WRITE_ڈیٹا_FILE_منوال_0400  = 1 << 1,
} Write_ڈیٹا_File_Flags;

صحیح write_ڈیٹا_file_atomic_at(صحیح dir_fd, ثابت حرف *راستہ, ثابت ساخت iovec *iovec, Write_ڈیٹا_File_Flags flags);
