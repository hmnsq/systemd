/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define LONG_সারি_MAX (1U*1024U*1024U)

প্রকারনির্দেশ গণনা {
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

প্রকারনির্দেশ গণনা {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

পূর্ণাঙ্ক fdopen_unlocked(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *options, FILE **ret);
পূর্ণাঙ্ক take_fdopen_unlocked(পূর্ণাঙ্ক *fd, ধ্রুব বর্ণ *options, FILE **ret);
FILE* take_fdopen(পূর্ণাঙ্ক *fd, ধ্রুব বর্ণ *options);
DIR* take_fdopendir(পূর্ণাঙ্ক *dfd);
FILE* open_memstream_unlocked(বর্ণ **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(শূন্য *buf, size_t size, ধ্রুব বর্ণ *mode);

পূর্ণাঙ্ক write_string_stream_full(FILE *f, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags, ধ্রুব সংরচনা timespec *ts);
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_string_stream(FILE *f, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags) {
        প্রত্যাগম write_string_stream_full(f, সারি, flags, রিক্ত);
}

পূর্ণাঙ্ক write_string_file_full_label(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags, ধ্রুব সংরচনা timespec *ts, ধ্রুব বর্ণ *label_fn, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_string_file_full(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags, ধ্রুব সংরচনা timespec *ts, ধ্রুব বর্ণ *label_fn) {
        প্রত্যাগম write_string_file_full_label(dir_fd, fn, সারি, flags, ts, label_fn, /* label_context= */ রিক্ত);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_string_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags) {
        প্রত্যাগম write_string_file_full(dir_fd, fn, সারি, flags, রিক্ত, রিক্ত);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_string_file_fd(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags) {
        প্রত্যাগম write_string_file_at(dir_fd, রিক্ত, সারি, flags);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক write_string_file(ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *সারি, WriteStringFileFlags flags) {
        প্রত্যাগম write_string_file_at(AT_FDCWD, fn, সারি, flags);
}
পূর্ণাঙ্ক write_string_filef(ধ্রুব বর্ণ *fn, WriteStringFileFlags flags, ধ্রুব বর্ণ *format, ...) _printf_(3, 4);
পূর্ণাঙ্ক write_string_filef_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fn, WriteStringFileFlags flags, ধ্রুব বর্ণ *format, ...) _printf_(4, 5);

পূর্ণাঙ্ক write_base64_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fn, ধ্রুব সংরচনা iovec *data, WriteStringFileFlags flags);

পূর্ণাঙ্ক read_one_সারি_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *filename, বর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_one_সারি_file(ধ্রুব বর্ণ *filename, বর্ণ **ret) {
        প্রত্যাগম read_one_সারি_file_at(AT_FDCWD, filename, ret);
}
পূর্ণাঙ্ক read_boolean_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *filename);
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_boolean_file(ধ্রুব বর্ণ *filename) {
        প্রত্যাগম read_boolean_file_at(AT_FDCWD, filename);
}
পূর্ণাঙ্ক read_full_file_full(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, ধ্রুব বর্ণ *bind_name, বর্ণ **ret_contents, size_t *ret_size);
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_full_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *filename, বর্ণ **ret_contents, size_t *ret_size) {
        প্রত্যাগম read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, রিক্ত, ret_contents, ret_size);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_full_file(ধ্রুব বর্ণ *filename, বর্ণ **ret_contents, size_t *ret_size) {
        প্রত্যাগম read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, রিক্ত, ret_contents, ret_size);
}

পূর্ণাঙ্ক read_virtual_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *filename, size_t max_size, বর্ণ **ret_contents, size_t *ret_size);
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_virtual_file_fd(পূর্ণাঙ্ক fd, size_t max_size, বর্ণ **ret_contents, size_t *ret_size) {
        প্রত্যাগম read_virtual_file_at(fd, রিক্ত, max_size, ret_contents, ret_size);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_virtual_file(ধ্রুব বর্ণ *filename, size_t max_size, বর্ণ **ret_contents, size_t *ret_size) {
        প্রত্যাগম read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_full_virtual_file(ধ্রুব বর্ণ *filename, বর্ণ **ret_contents, size_t *ret_size) {
        প্রত্যাগম read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

পূর্ণাঙ্ক read_full_stream_full(FILE *f, ধ্রুব বর্ণ *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, বর্ণ **ret_contents, size_t *ret_size);
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_full_stream(FILE *f, বর্ণ **ret_contents, size_t *ret_size) {
        প্রত্যাগম read_full_stream_full(f, রিক্ত, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

পূর্ণাঙ্ক verify_file_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *fn, ধ্রুব বর্ণ *blob, bool accept_extra_nl);

পূর্ণাঙ্ক script_get_shebang_interpreter(ধ্রুব বর্ণ *path, বর্ণ **ret);

পূর্ণাঙ্ক get_proc_field(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *চাবি, বর্ণ **ret);

DIR* xopendirat(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক flags);

প্রকারনির্দেশ গণনা XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

পূর্ণাঙ্ক xfopenat_full(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুব বর্ণ *path,
                ধ্রুব বর্ণ *mode,
                পূর্ণাঙ্ক open_flags,
                XfopenFlags flags,
                ধ্রুব বর্ণ *bind_name,
                FILE **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক xfopenat(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *mode, পূর্ণাঙ্ক open_flags, FILE **ret) {
        প্রত্যাগম xfopenat_full(dir_fd, path, mode, open_flags, 0, রিক্ত, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক fopen_unlocked_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *mode, পূর্ণাঙ্ক open_flags, FILE **ret) {
        প্রত্যাগম xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, রিক্ত, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক fopen_unlocked(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *mode, FILE **ret) {
        প্রত্যাগম fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

পূর্ণাঙ্ক fdopen_independent(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *mode, FILE **ret);

পূর্ণাঙ্ক অনুসন্ধান_and_open(ধ্রুব বর্ণ *path, পূর্ণাঙ্ক mode, ধ্রুব বর্ণ *মূল, বর্ণ **অনুসন্ধান, পূর্ণাঙ্ক *ret_fd, বর্ণ **ret_path);
স্থির অন্তঃস্থ পূর্ণাঙ্ক অনুসন্ধান_and_access(ধ্রুব বর্ণ *path, পূর্ণাঙ্ক mode, ধ্রুব বর্ণ *মূল, বর্ণ**অনুসন্ধান, বর্ণ **ret_path) {
        প্রত্যাগম অনুসন্ধান_and_open(path, mode, মূল, অনুসন্ধান, রিক্ত, ret_path);
}
পূর্ণাঙ্ক অনুসন্ধান_and_fopen(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *mode, ধ্রুব বর্ণ *মূল, ধ্রুব বর্ণ **অনুসন্ধান, FILE **ret_file, বর্ণ **ret_path);
পূর্ণাঙ্ক অনুসন্ধান_and_fopen_nulstr(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *mode, ধ্রুব বর্ণ *মূল, ধ্রুব বর্ণ *অনুসন্ধান, FILE **ret_file, বর্ণ **ret_path);

পূর্ণাঙ্ক fflush_and_check(FILE *f);
পূর্ণাঙ্ক fflush_sync_and_check(FILE *f);

পূর্ণাঙ্ক write_timestamp_file_atomic(ধ্রুব বর্ণ *fn, usec_t n);
পূর্ণাঙ্ক read_timestamp_file(ধ্রুব বর্ণ *fn, usec_t *ret);

পূর্ণাঙ্ক fputs_with_separator(FILE *f, ধ্রুব বর্ণ *s, ধ্রুব বর্ণ *separator, bool *space);
পূর্ণাঙ্ক fputs_with_newline(FILE *f, ধ্রুব বর্ণ *s);

প্রকারনির্দেশ গণনা Read_সারি_Flags {
        READ_সারি_ONLY_NUL  = 1 << 0,
        READ_সারি_IS_A_TTY  = 1 << 1,
        READ_সারি_NOT_A_TTY = 1 << 2,
} Read_সারি_Flags;

পূর্ণাঙ্ক read_সারি_full(FILE *f, size_t limit, Read_সারি_Flags flags, বর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_সারি(FILE *f, size_t limit, বর্ণ **ret) {
        প্রত্যাগম read_সারি_full(f, limit, 0, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক read_nul_string(FILE *f, size_t limit, বর্ণ **ret) {
        প্রত্যাগম read_সারি_full(f, limit, READ_সারি_ONLY_NUL, ret);
}

পূর্ণাঙ্ক read_stripped_সারি(FILE *f, size_t limit, বর্ণ **ret);

স্থির অন্তঃস্থ bool file_offset_beyond_memory_size(off_t x) {
        যদি (x < 0) /* off_t is signed, filter that out */
                প্রত্যাগম false;
        প্রত্যাগম (uint64_t) x > (uint64_t) SIZE_MAX;
}

পূর্ণাঙ্ক safe_fgetc(FILE *f, বর্ণ *ret);

পূর্ণাঙ্ক warn_file_is_world_accessible(ধ্রুব বর্ণ *filename, সংরচনা stat *st, ধ্রুব বর্ণ *unit, unsigned সারি);

পূর্ণাঙ্ক fopen_mode_to_flags(ধ্রুব বর্ণ *mode);

প্রকারনির্দেশ গণনা WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

পূর্ণাঙ্ক write_data_file_atomic_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব সংরচনা iovec *iovec, WriteDataFileFlags flags);
