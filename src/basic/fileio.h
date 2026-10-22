/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

#define LONG_سطر_MAX (1U*1024U*1024U)

تعریفنوع شمارش {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_BUFFER             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_MODE_0600                  = 1 << 10,
        WRITE_STRING_FILE_MODE_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

تعریفنوع شمارش {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

صحیح fdopen_unlocked(صحیح fd, ثابت نویسه *options, FILE **ret);
صحیح take_fdopen_unlocked(صحیح *fd, ثابت نویسه *options, FILE **ret);
FILE* take_fdopen(صحیح *fd, ثابت نویسه *options);
DIR* take_fdopendir(صحیح *dfd);
FILE* open_memstream_unlocked(نویسه **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(تهی *buf, size_t size, ثابت نویسه *mode);

صحیح write_string_stream_full(FILE *f, ثابت نویسه *سطر, WriteStringFileFlags flags, ثابت ساختار timespec *ts);
ایستا درونخطی صحیح write_string_stream(FILE *f, ثابت نویسه *سطر, WriteStringFileFlags flags) {
        بازگشت write_string_stream_full(f, سطر, flags, پوچ);
}

صحیح write_string_file_full_label(صحیح dir_fd, ثابت نویسه *fn, ثابت نویسه *سطر, WriteStringFileFlags flags, ثابت ساختار timespec *ts, ثابت نویسه *label_fn, LabelContext *label_context);
ایستا درونخطی صحیح write_string_file_full(صحیح dir_fd, ثابت نویسه *fn, ثابت نویسه *سطر, WriteStringFileFlags flags, ثابت ساختار timespec *ts, ثابت نویسه *label_fn) {
        بازگشت write_string_file_full_label(dir_fd, fn, سطر, flags, ts, label_fn, /* label_context= */ پوچ);
}
ایستا درونخطی صحیح write_string_file_at(صحیح dir_fd, ثابت نویسه *fn, ثابت نویسه *سطر, WriteStringFileFlags flags) {
        بازگشت write_string_file_full(dir_fd, fn, سطر, flags, پوچ, پوچ);
}
ایستا درونخطی صحیح write_string_file_fd(صحیح dir_fd, ثابت نویسه *سطر, WriteStringFileFlags flags) {
        بازگشت write_string_file_at(dir_fd, پوچ, سطر, flags);
}
ایستا درونخطی صحیح write_string_file(ثابت نویسه *fn, ثابت نویسه *سطر, WriteStringFileFlags flags) {
        بازگشت write_string_file_at(AT_FDCWD, fn, سطر, flags);
}
صحیح write_string_filef(ثابت نویسه *fn, WriteStringFileFlags flags, ثابت نویسه *format, ...) _printf_(3, 4);
صحیح write_string_filef_at(صحیح dir_fd, ثابت نویسه *fn, WriteStringFileFlags flags, ثابت نویسه *format, ...) _printf_(4, 5);

صحیح write_base64_file_at(صحیح dir_fd, ثابت نویسه *fn, ثابت ساختار iovec *data, WriteStringFileFlags flags);

صحیح read_one_سطر_file_at(صحیح dir_fd, ثابت نویسه *filename, نویسه **ret);
ایستا درونخطی صحیح read_one_سطر_file(ثابت نویسه *filename, نویسه **ret) {
        بازگشت read_one_سطر_file_at(AT_FDCWD, filename, ret);
}
صحیح read_boolean_file_at(صحیح dir_fd, ثابت نویسه *filename);
ایستا درونخطی صحیح read_boolean_file(ثابت نویسه *filename) {
        بازگشت read_boolean_file_at(AT_FDCWD, filename);
}
صحیح read_full_file_full(صحیح dir_fd, ثابت نویسه *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, ثابت نویسه *bind_name, نویسه **ret_contents, size_t *ret_size);
ایستا درونخطی صحیح read_full_file_at(صحیح dir_fd, ثابت نویسه *filename, نویسه **ret_contents, size_t *ret_size) {
        بازگشت read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, پوچ, ret_contents, ret_size);
}
ایستا درونخطی صحیح read_full_file(ثابت نویسه *filename, نویسه **ret_contents, size_t *ret_size) {
        بازگشت read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, پوچ, ret_contents, ret_size);
}

صحیح read_virtual_file_at(صحیح dir_fd, ثابت نویسه *filename, size_t max_size, نویسه **ret_contents, size_t *ret_size);
ایستا درونخطی صحیح read_virtual_file_fd(صحیح fd, size_t max_size, نویسه **ret_contents, size_t *ret_size) {
        بازگشت read_virtual_file_at(fd, پوچ, max_size, ret_contents, ret_size);
}
ایستا درونخطی صحیح read_virtual_file(ثابت نویسه *filename, size_t max_size, نویسه **ret_contents, size_t *ret_size) {
        بازگشت read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
ایستا درونخطی صحیح read_full_virtual_file(ثابت نویسه *filename, نویسه **ret_contents, size_t *ret_size) {
        بازگشت read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

صحیح read_full_stream_full(FILE *f, ثابت نویسه *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, نویسه **ret_contents, size_t *ret_size);
ایستا درونخطی صحیح read_full_stream(FILE *f, نویسه **ret_contents, size_t *ret_size) {
        بازگشت read_full_stream_full(f, پوچ, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

صحیح verify_file_at(صحیح dir_fd, ثابت نویسه *fn, ثابت نویسه *blob, bool accept_extra_nl);

صحیح script_get_shebang_interpreter(ثابت نویسه *path, نویسه **ret);

صحیح get_proc_field(ثابت نویسه *path, ثابت نویسه *کلید, نویسه **ret);

DIR* xopendirat(صحیح dir_fd, ثابت نویسه *path, صحیح flags);

تعریفنوع شمارش XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

صحیح xfopenat_full(
                صحیح dir_fd,
                ثابت نویسه *path,
                ثابت نویسه *mode,
                صحیح open_flags,
                XfopenFlags flags,
                ثابت نویسه *bind_name,
                FILE **ret);
ایستا درونخطی صحیح xfopenat(صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *mode, صحیح open_flags, FILE **ret) {
        بازگشت xfopenat_full(dir_fd, path, mode, open_flags, 0, پوچ, ret);
}
ایستا درونخطی صحیح fopen_unlocked_at(صحیح dir_fd, ثابت نویسه *path, ثابت نویسه *mode, صحیح open_flags, FILE **ret) {
        بازگشت xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, پوچ, ret);
}
ایستا درونخطی صحیح fopen_unlocked(ثابت نویسه *path, ثابت نویسه *mode, FILE **ret) {
        بازگشت fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

صحیح fdopen_independent(صحیح fd, ثابت نویسه *mode, FILE **ret);

صحیح جستجو_and_open(ثابت نویسه *path, صحیح mode, ثابت نویسه *ریشه, نویسه **جستجو, صحیح *ret_fd, نویسه **ret_path);
ایستا درونخطی صحیح جستجو_and_access(ثابت نویسه *path, صحیح mode, ثابت نویسه *ریشه, نویسه**جستجو, نویسه **ret_path) {
        بازگشت جستجو_and_open(path, mode, ریشه, جستجو, پوچ, ret_path);
}
صحیح جستجو_and_fopen(ثابت نویسه *path, ثابت نویسه *mode, ثابت نویسه *ریشه, ثابت نویسه **جستجو, FILE **ret_file, نویسه **ret_path);
صحیح جستجو_and_fopen_nulstr(ثابت نویسه *path, ثابت نویسه *mode, ثابت نویسه *ریشه, ثابت نویسه *جستجو, FILE **ret_file, نویسه **ret_path);

صحیح fflush_and_check(FILE *f);
صحیح fflush_sync_and_check(FILE *f);

صحیح write_timestamp_file_atomic(ثابت نویسه *fn, usec_t n);
صحیح read_timestamp_file(ثابت نویسه *fn, usec_t *ret);

صحیح fputs_with_separator(FILE *f, ثابت نویسه *s, ثابت نویسه *separator, bool *space);
صحیح fputs_with_newline(FILE *f, ثابت نویسه *s);

تعریفنوع شمارش Read_سطر_Flags {
        READ_سطر_ONLY_NUL  = 1 << 0,
        READ_سطر_IS_A_TTY  = 1 << 1,
        READ_سطر_NOT_A_TTY = 1 << 2,
} Read_سطر_Flags;

صحیح read_سطر_full(FILE *f, size_t limit, Read_سطر_Flags flags, نویسه **ret);
ایستا درونخطی صحیح read_سطر(FILE *f, size_t limit, نویسه **ret) {
        بازگشت read_سطر_full(f, limit, 0, ret);
}
ایستا درونخطی صحیح read_nul_string(FILE *f, size_t limit, نویسه **ret) {
        بازگشت read_سطر_full(f, limit, READ_سطر_ONLY_NUL, ret);
}

صحیح read_stripped_سطر(FILE *f, size_t limit, نویسه **ret);

ایستا درونخطی bool file_offset_beyond_memory_size(off_t x) {
        اگر (x < 0) /* off_t is signed, filter that out */
                بازگشت false;
        بازگشت (uint64_t) x > (uint64_t) SIZE_MAX;
}

صحیح safe_fgetc(FILE *f, نویسه *ret);

صحیح warn_file_is_world_accessible(ثابت نویسه *filename, ساختار stat *st, ثابت نویسه *unit, unsigned سطر);

صحیح fopen_mode_to_flags(ثابت نویسه *mode);

تعریفنوع شمارش WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

صحیح write_data_file_atomic_at(صحیح dir_fd, ثابت نویسه *path, ثابت ساختار iovec *iovec, WriteDataFileFlags flags);
