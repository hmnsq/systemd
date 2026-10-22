/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#define LONG_LINE_MAX (1U*1024U*1024U)

تعريفنوع تعداد {
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

تعريفنوع تعداد {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

صحيح fdopen_unlocked(صحيح fd, ثابت حرف *options, FILE **ret);
صحيح take_fdopen_unlocked(صحيح *fd, ثابت حرف *options, FILE **ret);
FILE* take_fdopen(صحيح *fd, ثابت حرف *options);
DIR* take_fdopendir(صحيح *dfd);
FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(فراغ *buf, size_t size, ثابت حرف *mode);

صحيح write_string_stream_full(FILE *f, ثابت حرف *line, WriteStringFileFlags flags, ثابت بنية timespec *ts);
ساكن مضمن صحيح write_string_stream(FILE *f, ثابت حرف *line, WriteStringFileFlags flags) {
        أرجع write_string_stream_full(f, line, flags, عدم);
}

صحيح write_string_file_full_label(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *line, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn, LabelContext *label_context);
ساكن مضمن صحيح write_string_file_full(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *line, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn) {
        أرجع write_string_file_full_label(dir_fd, fn, line, flags, ts, label_fn, /* label_context= */ عدم);
}
ساكن مضمن صحيح write_string_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *line, WriteStringFileFlags flags) {
        أرجع write_string_file_full(dir_fd, fn, line, flags, عدم, عدم);
}
ساكن مضمن صحيح write_string_file_fd(صحيح dir_fd, ثابت حرف *line, WriteStringFileFlags flags) {
        أرجع write_string_file_at(dir_fd, عدم, line, flags);
}
ساكن مضمن صحيح write_string_file(ثابت حرف *fn, ثابت حرف *line, WriteStringFileFlags flags) {
        أرجع write_string_file_at(AT_FDCWD, fn, line, flags);
}
صحيح write_string_filef(ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(3, 4);
صحيح write_string_filef_at(صحيح dir_fd, ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(4, 5);

صحيح write_base64_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت بنية iovec *data, WriteStringFileFlags flags);

صحيح read_one_line_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret);
ساكن مضمن صحيح read_one_line_file(ثابت حرف *filename, حرف **ret) {
        أرجع read_one_line_file_at(AT_FDCWD, filename, ret);
}
صحيح read_boolean_file_at(صحيح dir_fd, ثابت حرف *filename);
ساكن مضمن صحيح read_boolean_file(ثابت حرف *filename) {
        أرجع read_boolean_file_at(AT_FDCWD, filename);
}
صحيح read_full_file_full(صحيح dir_fd, ثابت حرف *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, ثابت حرف *bind_name, حرف **ret_contents, size_t *ret_size);
ساكن مضمن صحيح read_full_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret_contents, size_t *ret_size) {
        أرجع read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_size);
}
ساكن مضمن صحيح read_full_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_size) {
        أرجع read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_size);
}

صحيح read_virtual_file_at(صحيح dir_fd, ثابت حرف *filename, size_t max_size, حرف **ret_contents, size_t *ret_size);
ساكن مضمن صحيح read_virtual_file_fd(صحيح fd, size_t max_size, حرف **ret_contents, size_t *ret_size) {
        أرجع read_virtual_file_at(fd, عدم, max_size, ret_contents, ret_size);
}
ساكن مضمن صحيح read_virtual_file(ثابت حرف *filename, size_t max_size, حرف **ret_contents, size_t *ret_size) {
        أرجع read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
ساكن مضمن صحيح read_full_virtual_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_size) {
        أرجع read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

صحيح read_full_stream_full(FILE *f, ثابت حرف *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, حرف **ret_contents, size_t *ret_size);
ساكن مضمن صحيح read_full_stream(FILE *f, حرف **ret_contents, size_t *ret_size) {
        أرجع read_full_stream_full(f, عدم, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

صحيح verify_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl);

صحيح script_get_shebang_interpreter(ثابت حرف *path, حرف **ret);

صحيح get_proc_field(ثابت حرف *path, ثابت حرف *key, حرف **ret);

DIR* xopendirat(صحيح dir_fd, ثابت حرف *path, صحيح flags);

تعريفنوع تعداد XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

صحيح xfopenat_full(
                صحيح dir_fd,
                ثابت حرف *path,
                ثابت حرف *mode,
                صحيح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret);
ساكن مضمن صحيح xfopenat(صحيح dir_fd, ثابت حرف *path, ثابت حرف *mode, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_full(dir_fd, path, mode, open_flags, 0, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked_at(صحيح dir_fd, ثابت حرف *path, ثابت حرف *mode, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked(ثابت حرف *path, ثابت حرف *mode, FILE **ret) {
        أرجع fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

صحيح fdopen_independent(صحيح fd, ثابت حرف *mode, FILE **ret);

صحيح search_and_open(ثابت حرف *path, صحيح mode, ثابت حرف *root, حرف **search, صحيح *ret_fd, حرف **ret_path);
ساكن مضمن صحيح search_and_access(ثابت حرف *path, صحيح mode, ثابت حرف *root, حرف**search, حرف **ret_path) {
        أرجع search_and_open(path, mode, root, search, عدم, ret_path);
}
صحيح search_and_fopen(ثابت حرف *path, ثابت حرف *mode, ثابت حرف *root, ثابت حرف **search, FILE **ret_file, حرف **ret_path);
صحيح search_and_fopen_nulstr(ثابت حرف *path, ثابت حرف *mode, ثابت حرف *root, ثابت حرف *search, FILE **ret_file, حرف **ret_path);

صحيح fflush_and_check(FILE *f);
صحيح fflush_sync_and_check(FILE *f);

صحيح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n);
صحيح read_timestamp_file(ثابت حرف *fn, usec_t *ret);

صحيح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space);
صحيح fputs_with_newline(FILE *f, ثابت حرف *s);

تعريفنوع تعداد ReadLineFlags {
        READ_LINE_ONLY_NUL  = 1 << 0,
        READ_LINE_IS_A_TTY  = 1 << 1,
        READ_LINE_NOT_A_TTY = 1 << 2,
} ReadLineFlags;

صحيح read_line_full(FILE *f, size_t limit, ReadLineFlags flags, حرف **ret);
ساكن مضمن صحيح read_line(FILE *f, size_t limit, حرف **ret) {
        أرجع read_line_full(f, limit, 0, ret);
}
ساكن مضمن صحيح read_nul_string(FILE *f, size_t limit, حرف **ret) {
        أرجع read_line_full(f, limit, READ_LINE_ONLY_NUL, ret);
}

صحيح read_stripped_line(FILE *f, size_t limit, حرف **ret);

ساكن مضمن bool file_offset_beyond_memory_size(off_t x) {
        𐤀𐤌 (x < 0) /* off_t is signed, filter that out */
                أرجع false;
        أرجع (uint64_t) x > (uint64_t) SIZE_MAX;
}

صحيح safe_fgetc(FILE *f, حرف *ret);

صحيح warn_file_is_world_accessible(ثابت حرف *filename, بنية stat *st, ثابت حرف *unit, unsigned line);

صحيح fopen_mode_to_flags(ثابت حرف *mode);

تعريفنوع تعداد WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

صحيح write_data_file_atomic_at(صحيح dir_fd, ثابت حرف *path, ثابت بنية iovec *iovec, WriteDataFileFlags flags);
