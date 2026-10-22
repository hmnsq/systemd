/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define LONG_LINE_MAX (1U*1024U*1024U)

typedef enum {
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

typedef enum {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

ціле fdopen_unlocked(ціле fd, const char *options, FILE **ret);
ціле take_fdopen_unlocked(ціле *fd, const char *options, FILE **ret);
FILE* take_fdopen(ціле *fd, const char *options);
DIR* take_fdopendir(ціле *dfd);
FILE* open_memstream_unlocked(char **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(void *buf, size_t size, const char *mode);

ціле write_string_stream_full(FILE *f, const char *line, WriteStringFileFlags flags, const struct timespec *ts);
static inline ціле write_string_stream(FILE *f, const char *line, WriteStringFileFlags flags) {
        повернути write_string_stream_full(f, line, flags, NULL);
}

ціле write_string_file_full_label(ціле dir_fd, const char *fn, const char *line, WriteStringFileFlags flags, const struct timespec *ts, const char *label_fn, LabelContext *label_context);
static inline ціле write_string_file_full(ціле dir_fd, const char *fn, const char *line, WriteStringFileFlags flags, const struct timespec *ts, const char *label_fn) {
        повернути write_string_file_full_label(dir_fd, fn, line, flags, ts, label_fn, /* label_context= */ NULL);
}
static inline ціле write_string_file_at(ціле dir_fd, const char *fn, const char *line, WriteStringFileFlags flags) {
        повернути write_string_file_full(dir_fd, fn, line, flags, NULL, NULL);
}
static inline ціле write_string_file_fd(ціле dir_fd, const char *line, WriteStringFileFlags flags) {
        повернути write_string_file_at(dir_fd, NULL, line, flags);
}
static inline ціле write_string_file(const char *fn, const char *line, WriteStringFileFlags flags) {
        повернути write_string_file_at(AT_FDCWD, fn, line, flags);
}
ціле write_string_filef(const char *fn, WriteStringFileFlags flags, const char *format, ...) _printf_(3, 4);
ціле write_string_filef_at(ціле dir_fd, const char *fn, WriteStringFileFlags flags, const char *format, ...) _printf_(4, 5);

ціле write_base64_file_at(ціле dir_fd, const char *fn, const struct iovec *data, WriteStringFileFlags flags);

ціле read_one_line_file_at(ціле dir_fd, const char *filename, char **ret);
static inline ціле read_one_line_file(const char *filename, char **ret) {
        повернути read_one_line_file_at(AT_FDCWD, filename, ret);
}
ціле read_boolean_file_at(ціле dir_fd, const char *filename);
static inline ціле read_boolean_file(const char *filename) {
        повернути read_boolean_file_at(AT_FDCWD, filename);
}
ціле read_full_file_full(ціле dir_fd, const char *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, const char *bind_name, char **ret_contents, size_t *ret_size);
static inline ціле read_full_file_at(ціле dir_fd, const char *filename, char **ret_contents, size_t *ret_size) {
        повернути read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}
static inline ціле read_full_file(const char *filename, char **ret_contents, size_t *ret_size) {
        повернути read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}

ціле read_virtual_file_at(ціле dir_fd, const char *filename, size_t max_size, char **ret_contents, size_t *ret_size);
static inline ціле read_virtual_file_fd(ціле fd, size_t max_size, char **ret_contents, size_t *ret_size) {
        повернути read_virtual_file_at(fd, NULL, max_size, ret_contents, ret_size);
}
static inline ціле read_virtual_file(const char *filename, size_t max_size, char **ret_contents, size_t *ret_size) {
        повернути read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
static inline ціле read_full_virtual_file(const char *filename, char **ret_contents, size_t *ret_size) {
        повернути read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

ціле read_full_stream_full(FILE *f, const char *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, char **ret_contents, size_t *ret_size);
static inline ціле read_full_stream(FILE *f, char **ret_contents, size_t *ret_size) {
        повернути read_full_stream_full(f, NULL, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

ціле verify_file_at(ціле dir_fd, const char *fn, const char *blob, bool accept_extra_nl);

ціле script_get_shebang_interpreter(const char *path, char **ret);

ціле get_proc_field(const char *path, const char *key, char **ret);

DIR* xopendirat(ціле dir_fd, const char *path, ціле flags);

typedef enum XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

ціле xfopenat_full(
                ціле dir_fd,
                const char *path,
                const char *mode,
                ціле open_flags,
                XfopenFlags flags,
                const char *bind_name,
                FILE **ret);
static inline ціле xfopenat(ціле dir_fd, const char *path, const char *mode, ціле open_flags, FILE **ret) {
        повернути xfopenat_full(dir_fd, path, mode, open_flags, 0, NULL, ret);
}
static inline ціле fopen_unlocked_at(ціле dir_fd, const char *path, const char *mode, ціле open_flags, FILE **ret) {
        повернути xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, NULL, ret);
}
static inline ціле fopen_unlocked(const char *path, const char *mode, FILE **ret) {
        повернути fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

ціле fdopen_independent(ціле fd, const char *mode, FILE **ret);

ціле search_and_open(const char *path, ціле mode, const char *root, char **search, ціле *ret_fd, char **ret_path);
static inline ціле search_and_access(const char *path, ціле mode, const char *root, char**search, char **ret_path) {
        повернути search_and_open(path, mode, root, search, NULL, ret_path);
}
ціле search_and_fopen(const char *path, const char *mode, const char *root, const char **search, FILE **ret_file, char **ret_path);
ціле search_and_fopen_nulstr(const char *path, const char *mode, const char *root, const char *search, FILE **ret_file, char **ret_path);

ціле fflush_and_check(FILE *f);
ціле fflush_sync_and_check(FILE *f);

ціле write_timestamp_file_atomic(const char *fn, usec_t n);
ціле read_timestamp_file(const char *fn, usec_t *ret);

ціле fputs_with_separator(FILE *f, const char *s, const char *separator, bool *space);
ціле fputs_with_newline(FILE *f, const char *s);

typedef enum ReadLineFlags {
        READ_LINE_ONLY_NUL  = 1 << 0,
        READ_LINE_IS_A_TTY  = 1 << 1,
        READ_LINE_NOT_A_TTY = 1 << 2,
} ReadLineFlags;

ціле read_line_full(FILE *f, size_t limit, ReadLineFlags flags, char **ret);
static inline ціле read_line(FILE *f, size_t limit, char **ret) {
        повернути read_line_full(f, limit, 0, ret);
}
static inline ціле read_nul_string(FILE *f, size_t limit, char **ret) {
        повернути read_line_full(f, limit, READ_LINE_ONLY_NUL, ret);
}

ціле read_stripped_line(FILE *f, size_t limit, char **ret);

static inline bool file_offset_beyond_memory_size(off_t x) {
        якщо (x < 0) /* off_t is signed, filter that out */
                повернути false;
        повернути (uint64_t) x > (uint64_t) SIZE_MAX;
}

ціле safe_fgetc(FILE *f, char *ret);

ціле warn_file_is_world_accessible(const char *filename, struct stat *st, const char *unit, unsigned line);

ціле fopen_mode_to_flags(const char *mode);

typedef enum WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

ціле write_data_file_atomic_at(ціле dir_fd, const char *path, const struct iovec *iovec, WriteDataFileFlags flags);
