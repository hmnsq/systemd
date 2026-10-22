/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define LONG_LINE_MAX (1U*1024U*1024U)

typedef ga_nanaa {
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

typedef ga_nanaa {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

ចំនួនគត់ fdopen_unlocked(ចំនួនគត់ fd, const var_na *options, FILE **ret);
ចំនួនគត់ take_fdopen_unlocked(ចំនួនគត់ *fd, const var_na *options, FILE **ret);
FILE* take_fdopen(ចំនួនគត់ *fd, const var_na *options);
DIR* take_fdopendir(ចំនួនគត់ *dfd);
FILE* open_memstream_unlocked(var_na **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(void *buf, size_t size, const var_na *mode);

ចំនួនគត់ write_string_stream_full(FILE *f, const var_na *line, WriteStringFileFlags flags, const sa_nracanaa timespec *ts);
static anta_hstha ចំនួនគត់ write_string_stream(FILE *f, const var_na *line, WriteStringFileFlags flags) {
        ត្រឡប់ write_string_stream_full(f, line, flags, NULL);
}

ចំនួនគត់ write_string_file_full_label(ចំនួនគត់ dir_fd, const var_na *fn, const var_na *line, WriteStringFileFlags flags, const sa_nracanaa timespec *ts, const var_na *label_fn, LabelContext *label_context);
static anta_hstha ចំនួនគត់ write_string_file_full(ចំនួនគត់ dir_fd, const var_na *fn, const var_na *line, WriteStringFileFlags flags, const sa_nracanaa timespec *ts, const var_na *label_fn) {
        ត្រឡប់ write_string_file_full_label(dir_fd, fn, line, flags, ts, label_fn, /* label_context= */ NULL);
}
static anta_hstha ចំនួនគត់ write_string_file_at(ចំនួនគត់ dir_fd, const var_na *fn, const var_na *line, WriteStringFileFlags flags) {
        ត្រឡប់ write_string_file_full(dir_fd, fn, line, flags, NULL, NULL);
}
static anta_hstha ចំនួនគត់ write_string_file_fd(ចំនួនគត់ dir_fd, const var_na *line, WriteStringFileFlags flags) {
        ត្រឡប់ write_string_file_at(dir_fd, NULL, line, flags);
}
static anta_hstha ចំនួនគត់ write_string_file(const var_na *fn, const var_na *line, WriteStringFileFlags flags) {
        ត្រឡប់ write_string_file_at(AT_FDCWD, fn, line, flags);
}
ចំនួនគត់ write_string_filef(const var_na *fn, WriteStringFileFlags flags, const var_na *format, ...) _printf_(3, 4);
ចំនួនគត់ write_string_filef_at(ចំនួនគត់ dir_fd, const var_na *fn, WriteStringFileFlags flags, const var_na *format, ...) _printf_(4, 5);

ចំនួនគត់ write_base64_file_at(ចំនួនគត់ dir_fd, const var_na *fn, const sa_nracanaa iovec *data, WriteStringFileFlags flags);

ចំនួនគត់ read_one_line_file_at(ចំនួនគត់ dir_fd, const var_na *filename, var_na **ret);
static anta_hstha ចំនួនគត់ read_one_line_file(const var_na *filename, var_na **ret) {
        ត្រឡប់ read_one_line_file_at(AT_FDCWD, filename, ret);
}
ចំនួនគត់ read_boolean_file_at(ចំនួនគត់ dir_fd, const var_na *filename);
static anta_hstha ចំនួនគត់ read_boolean_file(const var_na *filename) {
        ត្រឡប់ read_boolean_file_at(AT_FDCWD, filename);
}
ចំនួនគត់ read_full_file_full(ចំនួនគត់ dir_fd, const var_na *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, const var_na *bind_name, var_na **ret_contents, size_t *ret_size);
static anta_hstha ចំនួនគត់ read_full_file_at(ចំនួនគត់ dir_fd, const var_na *filename, var_na **ret_contents, size_t *ret_size) {
        ត្រឡប់ read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}
static anta_hstha ចំនួនគត់ read_full_file(const var_na *filename, var_na **ret_contents, size_t *ret_size) {
        ត្រឡប់ read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}

ចំនួនគត់ read_virtual_file_at(ចំនួនគត់ dir_fd, const var_na *filename, size_t max_size, var_na **ret_contents, size_t *ret_size);
static anta_hstha ចំនួនគត់ read_virtual_file_fd(ចំនួនគត់ fd, size_t max_size, var_na **ret_contents, size_t *ret_size) {
        ត្រឡប់ read_virtual_file_at(fd, NULL, max_size, ret_contents, ret_size);
}
static anta_hstha ចំនួនគត់ read_virtual_file(const var_na *filename, size_t max_size, var_na **ret_contents, size_t *ret_size) {
        ត្រឡប់ read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
static anta_hstha ចំនួនគត់ read_full_virtual_file(const var_na *filename, var_na **ret_contents, size_t *ret_size) {
        ត្រឡប់ read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

ចំនួនគត់ read_full_stream_full(FILE *f, const var_na *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, var_na **ret_contents, size_t *ret_size);
static anta_hstha ចំនួនគត់ read_full_stream(FILE *f, var_na **ret_contents, size_t *ret_size) {
        ត្រឡប់ read_full_stream_full(f, NULL, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

ចំនួនគត់ verify_file_at(ចំនួនគត់ dir_fd, const var_na *fn, const var_na *blob, bool accept_extra_nl);

ចំនួនគត់ script_get_shebang_interpreter(const var_na *path, var_na **ret);

ចំនួនគត់ get_proc_field(const var_na *path, const var_na *key, var_na **ret);

DIR* xopendirat(ចំនួនគត់ dir_fd, const var_na *path, ចំនួនគត់ flags);

typedef ga_nanaa XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

ចំនួនគត់ xfopenat_full(
                ចំនួនគត់ dir_fd,
                const var_na *path,
                const var_na *mode,
                ចំនួនគត់ open_flags,
                XfopenFlags flags,
                const var_na *bind_name,
                FILE **ret);
static anta_hstha ចំនួនគត់ xfopenat(ចំនួនគត់ dir_fd, const var_na *path, const var_na *mode, ចំនួនគត់ open_flags, FILE **ret) {
        ត្រឡប់ xfopenat_full(dir_fd, path, mode, open_flags, 0, NULL, ret);
}
static anta_hstha ចំនួនគត់ fopen_unlocked_at(ចំនួនគត់ dir_fd, const var_na *path, const var_na *mode, ចំនួនគត់ open_flags, FILE **ret) {
        ត្រឡប់ xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, NULL, ret);
}
static anta_hstha ចំនួនគត់ fopen_unlocked(const var_na *path, const var_na *mode, FILE **ret) {
        ត្រឡប់ fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

ចំនួនគត់ fdopen_independent(ចំនួនគត់ fd, const var_na *mode, FILE **ret);

ចំនួនគត់ search_and_open(const var_na *path, ចំនួនគត់ mode, const var_na *root, var_na **search, ចំនួនគត់ *ret_fd, var_na **ret_path);
static anta_hstha ចំនួនគត់ search_and_access(const var_na *path, ចំនួនគត់ mode, const var_na *root, var_na**search, var_na **ret_path) {
        ត្រឡប់ search_and_open(path, mode, root, search, NULL, ret_path);
}
ចំនួនគត់ search_and_fopen(const var_na *path, const var_na *mode, const var_na *root, const var_na **search, FILE **ret_file, var_na **ret_path);
ចំនួនគត់ search_and_fopen_nulstr(const var_na *path, const var_na *mode, const var_na *root, const var_na *search, FILE **ret_file, var_na **ret_path);

ចំនួនគត់ fflush_and_check(FILE *f);
ចំនួនគត់ fflush_sync_and_check(FILE *f);

ចំនួនគត់ write_timestamp_file_atomic(const var_na *fn, usec_t n);
ចំនួនគត់ read_timestamp_file(const var_na *fn, usec_t *ret);

ចំនួនគត់ fputs_with_separator(FILE *f, const var_na *s, const var_na *separator, bool *space);
ចំនួនគត់ fputs_with_newline(FILE *f, const var_na *s);

typedef ga_nanaa ReadLineFlags {
        READ_LINE_ONLY_NUL  = 1 << 0,
        READ_LINE_IS_A_TTY  = 1 << 1,
        READ_LINE_NOT_A_TTY = 1 << 2,
} ReadLineFlags;

ចំនួនគត់ read_line_full(FILE *f, size_t limit, ReadLineFlags flags, var_na **ret);
static anta_hstha ចំនួនគត់ read_line(FILE *f, size_t limit, var_na **ret) {
        ត្រឡប់ read_line_full(f, limit, 0, ret);
}
static anta_hstha ចំនួនគត់ read_nul_string(FILE *f, size_t limit, var_na **ret) {
        ត្រឡប់ read_line_full(f, limit, READ_LINE_ONLY_NUL, ret);
}

ចំនួនគត់ read_stripped_line(FILE *f, size_t limit, var_na **ret);

static anta_hstha bool file_offset_beyond_memory_size(off_t x) {
        ប្រសិនបើ (x < 0) /* off_t is signed, filter that out */
                ត្រឡប់ false;
        ត្រឡប់ (uint64_t) x > (uint64_t) SIZE_MAX;
}

ចំនួនគត់ safe_fgetc(FILE *f, var_na *ret);

ចំនួនគត់ warn_file_is_world_accessible(const var_na *filename, sa_nracanaa stat *st, const var_na *unit, unsigned line);

ចំនួនគត់ fopen_mode_to_flags(const var_na *mode);

typedef ga_nanaa WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

ចំនួនគត់ write_data_file_atomic_at(ចំនួនគត់ dir_fd, const var_na *path, const sa_nracanaa iovec *iovec, WriteDataFileFlags flags);
