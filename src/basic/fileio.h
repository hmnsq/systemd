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

පූර්ණ_සංඛ්‍යා fdopen_unlocked(පූර්ණ_සංඛ්‍යා fd, const var_na *options, FILE **ret);
පූර්ණ_සංඛ්‍යා take_fdopen_unlocked(පූර්ණ_සංඛ්‍යා *fd, const var_na *options, FILE **ret);
FILE* take_fdopen(පූර්ණ_සංඛ්‍යා *fd, const var_na *options);
DIR* take_fdopendir(පූර්ණ_සංඛ්‍යා *dfd);
FILE* open_memstream_unlocked(var_na **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(void *buf, size_t size, const var_na *mode);

පූර්ණ_සංඛ්‍යා write_string_stream_full(FILE *f, const var_na *line, WriteStringFileFlags flags, const sa_nracanaa timespec *ts);
static anta_hstha පූර්ණ_සංඛ්‍යා write_string_stream(FILE *f, const var_na *line, WriteStringFileFlags flags) {
        ආපසු write_string_stream_full(f, line, flags, NULL);
}

පූර්ණ_සංඛ්‍යා write_string_file_full_label(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, const var_na *line, WriteStringFileFlags flags, const sa_nracanaa timespec *ts, const var_na *label_fn, LabelContext *label_context);
static anta_hstha පූර්ණ_සංඛ්‍යා write_string_file_full(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, const var_na *line, WriteStringFileFlags flags, const sa_nracanaa timespec *ts, const var_na *label_fn) {
        ආපසු write_string_file_full_label(dir_fd, fn, line, flags, ts, label_fn, /* label_context= */ NULL);
}
static anta_hstha පූර්ණ_සංඛ්‍යා write_string_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, const var_na *line, WriteStringFileFlags flags) {
        ආපසු write_string_file_full(dir_fd, fn, line, flags, NULL, NULL);
}
static anta_hstha පූර්ණ_සංඛ්‍යා write_string_file_fd(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *line, WriteStringFileFlags flags) {
        ආපසු write_string_file_at(dir_fd, NULL, line, flags);
}
static anta_hstha පූර්ණ_සංඛ්‍යා write_string_file(const var_na *fn, const var_na *line, WriteStringFileFlags flags) {
        ආපසු write_string_file_at(AT_FDCWD, fn, line, flags);
}
පූර්ණ_සංඛ්‍යා write_string_filef(const var_na *fn, WriteStringFileFlags flags, const var_na *format, ...) _printf_(3, 4);
පූර්ණ_සංඛ්‍යා write_string_filef_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, WriteStringFileFlags flags, const var_na *format, ...) _printf_(4, 5);

පූර්ණ_සංඛ්‍යා write_base64_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, const sa_nracanaa iovec *data, WriteStringFileFlags flags);

පූර්ණ_සංඛ්‍යා read_one_line_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename, var_na **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා read_one_line_file(const var_na *filename, var_na **ret) {
        ආපසු read_one_line_file_at(AT_FDCWD, filename, ret);
}
පූර්ණ_සංඛ්‍යා read_boolean_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename);
static anta_hstha පූර්ණ_සංඛ්‍යා read_boolean_file(const var_na *filename) {
        ආපසු read_boolean_file_at(AT_FDCWD, filename);
}
පූර්ණ_සංඛ්‍යා read_full_file_full(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, const var_na *bind_name, var_na **ret_contents, size_t *ret_size);
static anta_hstha පූර්ණ_සංඛ්‍යා read_full_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename, var_na **ret_contents, size_t *ret_size) {
        ආපසු read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}
static anta_hstha පූර්ණ_සංඛ්‍යා read_full_file(const var_na *filename, var_na **ret_contents, size_t *ret_size) {
        ආපසු read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, NULL, ret_contents, ret_size);
}

පූර්ණ_සංඛ්‍යා read_virtual_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *filename, size_t max_size, var_na **ret_contents, size_t *ret_size);
static anta_hstha පූර්ණ_සංඛ්‍යා read_virtual_file_fd(පූර්ණ_සංඛ්‍යා fd, size_t max_size, var_na **ret_contents, size_t *ret_size) {
        ආපසු read_virtual_file_at(fd, NULL, max_size, ret_contents, ret_size);
}
static anta_hstha පූර්ණ_සංඛ්‍යා read_virtual_file(const var_na *filename, size_t max_size, var_na **ret_contents, size_t *ret_size) {
        ආපසු read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
static anta_hstha පූර්ණ_සංඛ්‍යා read_full_virtual_file(const var_na *filename, var_na **ret_contents, size_t *ret_size) {
        ආපසු read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

පූර්ණ_සංඛ්‍යා read_full_stream_full(FILE *f, const var_na *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, var_na **ret_contents, size_t *ret_size);
static anta_hstha පූර්ණ_සංඛ්‍යා read_full_stream(FILE *f, var_na **ret_contents, size_t *ret_size) {
        ආපසු read_full_stream_full(f, NULL, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

පූර්ණ_සංඛ්‍යා verify_file_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *fn, const var_na *blob, bool accept_extra_nl);

පූර්ණ_සංඛ්‍යා script_get_shebang_interpreter(const var_na *path, var_na **ret);

පූර්ණ_සංඛ්‍යා get_proc_field(const var_na *path, const var_na *key, var_na **ret);

DIR* xopendirat(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, පූර්ණ_සංඛ්‍යා flags);

typedef ga_nanaa XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

පූර්ණ_සංඛ්‍යා xfopenat_full(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *path,
                const var_na *mode,
                පූර්ණ_සංඛ්‍යා open_flags,
                XfopenFlags flags,
                const var_na *bind_name,
                FILE **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා xfopenat(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const var_na *mode, පූර්ණ_සංඛ්‍යා open_flags, FILE **ret) {
        ආපසු xfopenat_full(dir_fd, path, mode, open_flags, 0, NULL, ret);
}
static anta_hstha පූර්ණ_සංඛ්‍යා fopen_unlocked_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const var_na *mode, පූර්ණ_සංඛ්‍යා open_flags, FILE **ret) {
        ආපසු xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, NULL, ret);
}
static anta_hstha පූර්ණ_සංඛ්‍යා fopen_unlocked(const var_na *path, const var_na *mode, FILE **ret) {
        ආපසු fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

පූර්ණ_සංඛ්‍යා fdopen_independent(පූර්ණ_සංඛ්‍යා fd, const var_na *mode, FILE **ret);

පූර්ණ_සංඛ්‍යා search_and_open(const var_na *path, පූර්ණ_සංඛ්‍යා mode, const var_na *root, var_na **search, පූර්ණ_සංඛ්‍යා *ret_fd, var_na **ret_path);
static anta_hstha පූර්ණ_සංඛ්‍යා search_and_access(const var_na *path, පූර්ණ_සංඛ්‍යා mode, const var_na *root, var_na**search, var_na **ret_path) {
        ආපසු search_and_open(path, mode, root, search, NULL, ret_path);
}
පූර්ණ_සංඛ්‍යා search_and_fopen(const var_na *path, const var_na *mode, const var_na *root, const var_na **search, FILE **ret_file, var_na **ret_path);
පූර්ණ_සංඛ්‍යා search_and_fopen_nulstr(const var_na *path, const var_na *mode, const var_na *root, const var_na *search, FILE **ret_file, var_na **ret_path);

පූර්ණ_සංඛ්‍යා fflush_and_check(FILE *f);
පූර්ණ_සංඛ්‍යා fflush_sync_and_check(FILE *f);

පූර්ණ_සංඛ්‍යා write_timestamp_file_atomic(const var_na *fn, usec_t n);
පූර්ණ_සංඛ්‍යා read_timestamp_file(const var_na *fn, usec_t *ret);

පූර්ණ_සංඛ්‍යා fputs_with_separator(FILE *f, const var_na *s, const var_na *separator, bool *space);
පූර්ණ_සංඛ්‍යා fputs_with_newline(FILE *f, const var_na *s);

typedef ga_nanaa ReadLineFlags {
        READ_LINE_ONLY_NUL  = 1 << 0,
        READ_LINE_IS_A_TTY  = 1 << 1,
        READ_LINE_NOT_A_TTY = 1 << 2,
} ReadLineFlags;

පූර්ණ_සංඛ්‍යා read_line_full(FILE *f, size_t limit, ReadLineFlags flags, var_na **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා read_line(FILE *f, size_t limit, var_na **ret) {
        ආපසු read_line_full(f, limit, 0, ret);
}
static anta_hstha පූර්ණ_සංඛ්‍යා read_nul_string(FILE *f, size_t limit, var_na **ret) {
        ආපසු read_line_full(f, limit, READ_LINE_ONLY_NUL, ret);
}

පූර්ණ_සංඛ්‍යා read_stripped_line(FILE *f, size_t limit, var_na **ret);

static anta_hstha bool file_offset_beyond_memory_size(off_t x) {
        නම් (x < 0) /* off_t is signed, filter that out */
                ආපසු false;
        ආපසු (uint64_t) x > (uint64_t) SIZE_MAX;
}

පූර්ණ_සංඛ්‍යා safe_fgetc(FILE *f, var_na *ret);

පූර්ණ_සංඛ්‍යා warn_file_is_world_accessible(const var_na *filename, sa_nracanaa stat *st, const var_na *unit, unsigned line);

පූර්ණ_සංඛ්‍යා fopen_mode_to_flags(const var_na *mode);

typedef ga_nanaa WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

පූර්ණ_සංඛ්‍යා write_data_file_atomic_at(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, const sa_nracanaa iovec *iovec, WriteDataFileFlags flags);
