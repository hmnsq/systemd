/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define LONG_വരി_MAX (1U*1024U*1024U)

തരനിർവചനം എണ്ണൽ {
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

തരനിർവചനം എണ്ണൽ {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

പൂർണ്ണസംഖ്യ fdopen_unlocked(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *options, FILE **ret);
പൂർണ്ണസംഖ്യ take_fdopen_unlocked(പൂർണ്ണസംഖ്യ *fd, സ്ഥിരം അക്ഷരം *options, FILE **ret);
FILE* take_fdopen(പൂർണ്ണസംഖ്യ *fd, സ്ഥിരം അക്ഷരം *options);
DIR* take_fdopendir(പൂർണ്ണസംഖ്യ *dfd);
FILE* open_memstream_unlocked(അക്ഷരം **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(ശൂന്യം *buf, size_t size, സ്ഥിരം അക്ഷരം *mode);

പൂർണ്ണസംഖ്യ write_string_stream_full(FILE *f, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags, സ്ഥിരം ഘടന timespec *ts);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_string_stream(FILE *f, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags) {
        തിരികെ write_string_stream_full(f, വരി, flags, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ write_string_file_full_label(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags, സ്ഥിരം ഘടന timespec *ts, സ്ഥിരം അക്ഷരം *label_fn, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_string_file_full(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags, സ്ഥിരം ഘടന timespec *ts, സ്ഥിരം അക്ഷരം *label_fn) {
        തിരികെ write_string_file_full_label(dir_fd, fn, വരി, flags, ts, label_fn, /* label_context= */ ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_string_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags) {
        തിരികെ write_string_file_full(dir_fd, fn, വരി, flags, ശൂന്യ, ശൂന്യ);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_string_file_fd(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags) {
        തിരികെ write_string_file_at(dir_fd, ശൂന്യ, വരി, flags);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ write_string_file(സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *വരി, WriteStringFileFlags flags) {
        തിരികെ write_string_file_at(AT_FDCWD, fn, വരി, flags);
}
പൂർണ്ണസംഖ്യ write_string_filef(സ്ഥിരം അക്ഷരം *fn, WriteStringFileFlags flags, സ്ഥിരം അക്ഷരം *format, ...) _printf_(3, 4);
പൂർണ്ണസംഖ്യ write_string_filef_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, WriteStringFileFlags flags, സ്ഥിരം അക്ഷരം *format, ...) _printf_(4, 5);

പൂർണ്ണസംഖ്യ write_base64_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം ഘടന iovec *data, WriteStringFileFlags flags);

പൂർണ്ണസംഖ്യ read_one_വരി_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_one_വരി_file(സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret) {
        തിരികെ read_one_വരി_file_at(AT_FDCWD, filename, ret);
}
പൂർണ്ണസംഖ്യ read_boolean_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_boolean_file(സ്ഥിരം അക്ഷരം *filename) {
        തിരികെ read_boolean_file_at(AT_FDCWD, filename);
}
പൂർണ്ണസംഖ്യ read_full_file_full(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, സ്ഥിരം അക്ഷരം *bind_name, അക്ഷരം **ret_contents, size_t *ret_size);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_full_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret_contents, size_t *ret_size) {
        തിരികെ read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ശൂന്യ, ret_contents, ret_size);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_full_file(സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret_contents, size_t *ret_size) {
        തിരികെ read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ശൂന്യ, ret_contents, ret_size);
}

പൂർണ്ണസംഖ്യ read_virtual_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *filename, size_t max_size, അക്ഷരം **ret_contents, size_t *ret_size);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_virtual_file_fd(പൂർണ്ണസംഖ്യ fd, size_t max_size, അക്ഷരം **ret_contents, size_t *ret_size) {
        തിരികെ read_virtual_file_at(fd, ശൂന്യ, max_size, ret_contents, ret_size);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_virtual_file(സ്ഥിരം അക്ഷരം *filename, size_t max_size, അക്ഷരം **ret_contents, size_t *ret_size) {
        തിരികെ read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_full_virtual_file(സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret_contents, size_t *ret_size) {
        തിരികെ read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

പൂർണ്ണസംഖ്യ read_full_stream_full(FILE *f, സ്ഥിരം അക്ഷരം *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, അക്ഷരം **ret_contents, size_t *ret_size);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_full_stream(FILE *f, അക്ഷരം **ret_contents, size_t *ret_size) {
        തിരികെ read_full_stream_full(f, ശൂന്യ, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

പൂർണ്ണസംഖ്യ verify_file_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം *blob, bool accept_extra_nl);

പൂർണ്ണസംഖ്യ script_get_shebang_interpreter(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ get_proc_field(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *താക്കോൽ, അക്ഷരം **ret);

DIR* xopendirat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags);

തരനിർവചനം എണ്ണൽ XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

പൂർണ്ണസംഖ്യ xfopenat_full(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                സ്ഥിരം അക്ഷരം *mode,
                പൂർണ്ണസംഖ്യ open_flags,
                XfopenFlags flags,
                സ്ഥിരം അക്ഷരം *bind_name,
                FILE **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xfopenat(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *mode, പൂർണ്ണസംഖ്യ open_flags, FILE **ret) {
        തിരികെ xfopenat_full(dir_fd, path, mode, open_flags, 0, ശൂന്യ, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fopen_unlocked_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *mode, പൂർണ്ണസംഖ്യ open_flags, FILE **ret) {
        തിരികെ xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, ശൂന്യ, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fopen_unlocked(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *mode, FILE **ret) {
        തിരികെ fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

പൂർണ്ണസംഖ്യ fdopen_independent(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *mode, FILE **ret);

പൂർണ്ണസംഖ്യ തിരയൽ_and_open(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ mode, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **തിരയൽ, പൂർണ്ണസംഖ്യ *ret_fd, അക്ഷരം **ret_path);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ തിരയൽ_and_access(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ mode, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം**തിരയൽ, അക്ഷരം **ret_path) {
        തിരികെ തിരയൽ_and_open(path, mode, വേര്, തിരയൽ, ശൂന്യ, ret_path);
}
പൂർണ്ണസംഖ്യ തിരയൽ_and_fopen(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *mode, സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം **തിരയൽ, FILE **ret_file, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ തിരയൽ_and_fopen_nulstr(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *mode, സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം *തിരയൽ, FILE **ret_file, അക്ഷരം **ret_path);

പൂർണ്ണസംഖ്യ fflush_and_check(FILE *f);
പൂർണ്ണസംഖ്യ fflush_sync_and_check(FILE *f);

പൂർണ്ണസംഖ്യ write_timestamp_file_atomic(സ്ഥിരം അക്ഷരം *fn, usec_t n);
പൂർണ്ണസംഖ്യ read_timestamp_file(സ്ഥിരം അക്ഷരം *fn, usec_t *ret);

പൂർണ്ണസംഖ്യ fputs_with_separator(FILE *f, സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *separator, bool *space);
പൂർണ്ണസംഖ്യ fputs_with_newline(FILE *f, സ്ഥിരം അക്ഷരം *s);

തരനിർവചനം എണ്ണൽ Read_വരി_Flags {
        READ_വരി_ONLY_NUL  = 1 << 0,
        READ_വരി_IS_A_TTY  = 1 << 1,
        READ_വരി_NOT_A_TTY = 1 << 2,
} Read_വരി_Flags;

പൂർണ്ണസംഖ്യ read_വരി_full(FILE *f, size_t limit, Read_വരി_Flags flags, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_വരി(FILE *f, size_t limit, അക്ഷരം **ret) {
        തിരികെ read_വരി_full(f, limit, 0, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ read_nul_string(FILE *f, size_t limit, അക്ഷരം **ret) {
        തിരികെ read_വരി_full(f, limit, READ_വരി_ONLY_NUL, ret);
}

പൂർണ്ണസംഖ്യ read_stripped_വരി(FILE *f, size_t limit, അക്ഷരം **ret);

സ്ഥിതം വരിയിൽ bool file_offset_beyond_memory_size(off_t x) {
        എങ്കിൽ (x < 0) /* off_t is signed, filter that out */
                തിരികെ false;
        തിരികെ (uint64_t) x > (uint64_t) SIZE_MAX;
}

പൂർണ്ണസംഖ്യ safe_fgetc(FILE *f, അക്ഷരം *ret);

പൂർണ്ണസംഖ്യ warn_file_is_world_accessible(സ്ഥിരം അക്ഷരം *filename, ഘടന stat *st, സ്ഥിരം അക്ഷരം *unit, unsigned വരി);

പൂർണ്ണസംഖ്യ fopen_mode_to_flags(സ്ഥിരം അക്ഷരം *mode);

തരനിർവചനം എണ്ണൽ WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

പൂർണ്ണസംഖ്യ write_data_file_atomic_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം ഘടന iovec *iovec, WriteDataFileFlags flags);
