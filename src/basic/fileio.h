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

целое fdopen_unlocked(целое fd, const char *options, FILE **ret);
целое take_fdopen_unlocked(целое *fd, const char *options, FILE **ret);
FILE* take_fdopen(целое *fd, const char *options);
DIR* take_fdopendir(целое *dfd);
FILE* open_memstream_unlocked(char **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(void *buf, size_t size, const char *mode);

целое write_string_stream_full(FILE *f, const char *line, WriteStringFileFlags flags, const struct timespec *ts);
static inline целое write_string_stream(FILE *f, const char *line, WriteStringFileFlags flags) {
        вернуть write_string_stream_full(f, line, flags, NULL);
}

целое write_string_file_full_label(целое dir_fd, const char *fn, const char *line, WriteStringFileFlags flags, const struct timespec *ts, const char *label_fn, LabelContext *label_context);
static inline целое write_string_file_full(целое dir_fd, const char *fn, const char *line, WriteStringFileFlags flags, const struct timespec *ts, const char *label_fn) {
        вернуть write_string_file_full_label(dir_fd, fn, line, flags, ts, label_fn, /* label_context= */ NULL);
}
static inline целое write_string_file_at(целое dir_fd, const char *fn, const char *line, WriteStringFileFlags flags) {
        вернуть write_string_file_full(dir_fd, fn, line, flags, NULL, NULL);
}
static inline целое write_string_file_fd(целое dir_fd, const char *line, WriteStringFileFlags flags) {
        вернуть write_string_file_at(dir_fd, NULL, line, flags);
}
static inline целое write_string_file(const char *fn, const char *line, WriteStringFileFlags flags) {
        вернуть write_string_file_at(AT_FDCWD, fn, line, flags);
}
целое write_string_filef(const char *fn, WriteStringFileFlags flags, const char *format, ...) _printf_(3, 4);
целое write_string_filef_at(целое dir_fd, const char *fn, WriteStringFileFlags flags, const char *format, ...) _printf_(4, 5);

целое write_base64_file_at(целое dir_fd, const char *fn, const struct iovec *data, WriteStringFileFlags flags);

целое read_one_line_file_at(целое dir_fd, const char *filename, char **ret);
static inline целое read_one_line_file(const char *filename, char **ret) {
        вернуть read_one_line_file_at(AT_FDCWD, filename, ret);
}
целое read_boolean_file_at(целое dir_fd, const char *filename);
static inline целое read_boolean_file(const char *filename) {
        вернуть read_boolean_file_at(AT_FDCWD, filename);
}
целое read_full_file_full(целое dir_fd, const char *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, const char *bind_name, char **ret_contents, size_t *ret_size);
static inline целое read_full_file_at(целое dir_fd, const char *filename, char **ret_contents, size_t *ret_size) {
        вернуть read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}
static inline целое read_full_file(const char *filename, char **ret_contents, size_t *ret_size) {
        вернуть read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}

целое read_virtual_file_at(целое dir_fd, const char *filename, size_t max_size, char **ret_contents, size_t *ret_size);
static inline целое read_virtual_file_fd(целое fd, size_t max_size, char **ret_contents, size_t *ret_size) {
        вернуть read_virtual_file_at(fd, NULL, max_size, ret_contents, ret_size);
}
static inline целое read_virtual_file(const char *filename, size_t max_size, char **ret_contents, size_t *ret_size) {
        вернуть read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
static inline целое read_full_virtual_file(const char *filename, char **ret_contents, size_t *ret_size) {
        вернуть read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

целое read_full_stream_full(FILE *f, const char *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, char **ret_contents, size_t *ret_size);
static inline целое read_full_stream(FILE *f, char **ret_contents, size_t *ret_size) {
        вернуть read_full_stream_full(f, NULL, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

целое verify_file_at(целое dir_fd, const char *fn, const char *blob, bool accept_extra_nl);

целое script_get_shebang_interpreter(const char *path, char **ret);

целое get_proc_field(const char *path, const char *key, char **ret);

DIR* xopendirat(целое dir_fd, const char *path, целое flags);

typedef enum XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

целое xfopenat_full(
                целое dir_fd,
                const char *path,
                const char *mode,
                целое open_flags,
                XfopenFlags flags,
                const char *bind_name,
                FILE **ret);
static inline целое xfopenat(целое dir_fd, const char *path, const char *mode, целое open_flags, FILE **ret) {
        вернуть xfopenat_full(dir_fd, path, mode, open_flags, 0, NULL, ret);
}
static inline целое fopen_unlocked_at(целое dir_fd, const char *path, const char *mode, целое open_flags, FILE **ret) {
        вернуть xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, NULL, ret);
}
static inline целое fopen_unlocked(const char *path, const char *mode, FILE **ret) {
        вернуть fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

целое fdopen_independent(целое fd, const char *mode, FILE **ret);

целое search_and_open(const char *path, целое mode, const char *root, char **search, целое *ret_fd, char **ret_path);
static inline целое search_and_access(const char *path, целое mode, const char *root, char**search, char **ret_path) {
        вернуть search_and_open(path, mode, root, search, NULL, ret_path);
}
целое search_and_fopen(const char *path, const char *mode, const char *root, const char **search, FILE **ret_file, char **ret_path);
целое search_and_fopen_nulstr(const char *path, const char *mode, const char *root, const char *search, FILE **ret_file, char **ret_path);

целое fflush_and_check(FILE *f);
целое fflush_sync_and_check(FILE *f);

целое write_timestamp_file_atomic(const char *fn, usec_t n);
целое read_timestamp_file(const char *fn, usec_t *ret);

целое fputs_with_separator(FILE *f, const char *s, const char *separator, bool *space);
целое fputs_with_newline(FILE *f, const char *s);

typedef enum ReadLineFlags {
        READ_LINE_ONLY_NUL  = 1 << 0,
        READ_LINE_IS_A_TTY  = 1 << 1,
        READ_LINE_NOT_A_TTY = 1 << 2,
} ReadLineFlags;

целое read_line_full(FILE *f, size_t limit, ReadLineFlags flags, char **ret);
static inline целое read_line(FILE *f, size_t limit, char **ret) {
        вернуть read_line_full(f, limit, 0, ret);
}
static inline целое read_nul_string(FILE *f, size_t limit, char **ret) {
        вернуть read_line_full(f, limit, READ_LINE_ONLY_NUL, ret);
}

целое read_stripped_line(FILE *f, size_t limit, char **ret);

static inline bool file_offset_beyond_memory_size(off_t x) {
        если (x < 0) /* off_t is signed, filter that out */
                вернуть false;
        вернуть (uint64_t) x > (uint64_t) SIZE_MAX;
}

целое safe_fgetc(FILE *f, char *ret);

целое warn_file_is_world_accessible(const char *filename, struct stat *st, const char *unit, unsigned line);

целое fopen_mode_to_flags(const char *mode);

typedef enum WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

целое write_data_file_atomic_at(целое dir_fd, const char *path, const struct iovec *iovec, WriteDataFileFlags flags);
