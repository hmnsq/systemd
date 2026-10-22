/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define LONG_લીટી_MAX (1U*1024U*1024U)

પ્રકારવ્યાખ્યા ગણના {
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

પ્રકારવ્યાખ્યા ગણના {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

પૂર્ણાંક fdopen_unlocked(પૂર્ણાંક fd, અચલ અક્ષર *options, FILE **ret);
પૂર્ણાંક take_fdopen_unlocked(પૂર્ણાંક *fd, અચલ અક્ષર *options, FILE **ret);
FILE* take_fdopen(પૂર્ણાંક *fd, અચલ અક્ષર *options);
DIR* take_fdopendir(પૂર્ણાંક *dfd);
FILE* open_memstream_unlocked(અક્ષર **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(શૂન્ય *buf, size_t size, અચલ અક્ષર *mode);

પૂર્ણાંક write_string_stream_full(FILE *f, અચલ અક્ષર *લીટી, WriteStringFileFlags flags, અચલ સંરચના timespec *ts);
સ્થિર અંતઃરેખ પૂર્ણાંક write_string_stream(FILE *f, અચલ અક્ષર *લીટી, WriteStringFileFlags flags) {
        પાછા write_string_stream_full(f, લીટી, flags, ખાલી);
}

પૂર્ણાંક write_string_file_full_label(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, અચલ અક્ષર *લીટી, WriteStringFileFlags flags, અચલ સંરચના timespec *ts, અચલ અક્ષર *label_fn, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક write_string_file_full(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, અચલ અક્ષર *લીટી, WriteStringFileFlags flags, અચલ સંરચના timespec *ts, અચલ અક્ષર *label_fn) {
        પાછા write_string_file_full_label(dir_fd, fn, લીટી, flags, ts, label_fn, /* label_context= */ ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક write_string_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, અચલ અક્ષર *લીટી, WriteStringFileFlags flags) {
        પાછા write_string_file_full(dir_fd, fn, લીટી, flags, ખાલી, ખાલી);
}
સ્થિર અંતઃરેખ પૂર્ણાંક write_string_file_fd(પૂર્ણાંક dir_fd, અચલ અક્ષર *લીટી, WriteStringFileFlags flags) {
        પાછા write_string_file_at(dir_fd, ખાલી, લીટી, flags);
}
સ્થિર અંતઃરેખ પૂર્ણાંક write_string_file(અચલ અક્ષર *fn, અચલ અક્ષર *લીટી, WriteStringFileFlags flags) {
        પાછા write_string_file_at(AT_FDCWD, fn, લીટી, flags);
}
પૂર્ણાંક write_string_filef(અચલ અક્ષર *fn, WriteStringFileFlags flags, અચલ અક્ષર *format, ...) _printf_(3, 4);
પૂર્ણાંક write_string_filef_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, WriteStringFileFlags flags, અચલ અક્ષર *format, ...) _printf_(4, 5);

પૂર્ણાંક write_base64_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, અચલ સંરચના iovec *data, WriteStringFileFlags flags);

પૂર્ણાંક read_one_લીટી_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename, અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક read_one_લીટી_file(અચલ અક્ષર *filename, અક્ષર **ret) {
        પાછા read_one_લીટી_file_at(AT_FDCWD, filename, ret);
}
પૂર્ણાંક read_boolean_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename);
સ્થિર અંતઃરેખ પૂર્ણાંક read_boolean_file(અચલ અક્ષર *filename) {
        પાછા read_boolean_file_at(AT_FDCWD, filename);
}
પૂર્ણાંક read_full_file_full(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, અચલ અક્ષર *bind_name, અક્ષર **ret_contents, size_t *ret_size);
સ્થિર અંતઃરેખ પૂર્ણાંક read_full_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename, અક્ષર **ret_contents, size_t *ret_size) {
        પાછા read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ખાલી, ret_contents, ret_size);
}
સ્થિર અંતઃરેખ પૂર્ણાંક read_full_file(અચલ અક્ષર *filename, અક્ષર **ret_contents, size_t *ret_size) {
        પાછા read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ખાલી, ret_contents, ret_size);
}

પૂર્ણાંક read_virtual_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *filename, size_t max_size, અક્ષર **ret_contents, size_t *ret_size);
સ્થિર અંતઃરેખ પૂર્ણાંક read_virtual_file_fd(પૂર્ણાંક fd, size_t max_size, અક્ષર **ret_contents, size_t *ret_size) {
        પાછા read_virtual_file_at(fd, ખાલી, max_size, ret_contents, ret_size);
}
સ્થિર અંતઃરેખ પૂર્ણાંક read_virtual_file(અચલ અક્ષર *filename, size_t max_size, અક્ષર **ret_contents, size_t *ret_size) {
        પાછા read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
સ્થિર અંતઃરેખ પૂર્ણાંક read_full_virtual_file(અચલ અક્ષર *filename, અક્ષર **ret_contents, size_t *ret_size) {
        પાછા read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

પૂર્ણાંક read_full_stream_full(FILE *f, અચલ અક્ષર *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, અક્ષર **ret_contents, size_t *ret_size);
સ્થિર અંતઃરેખ પૂર્ણાંક read_full_stream(FILE *f, અક્ષર **ret_contents, size_t *ret_size) {
        પાછા read_full_stream_full(f, ખાલી, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

પૂર્ણાંક verify_file_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *fn, અચલ અક્ષર *blob, bool accept_extra_nl);

પૂર્ણાંક script_get_shebang_interpreter(અચલ અક્ષર *path, અક્ષર **ret);

પૂર્ણાંક get_proc_field(અચલ અક્ષર *path, અચલ અક્ષર *ચાવી, અક્ષર **ret);

DIR* xopendirat(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, પૂર્ણાંક flags);

પ્રકારવ્યાખ્યા ગણના XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

પૂર્ણાંક xfopenat_full(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *path,
                અચલ અક્ષર *mode,
                પૂર્ણાંક open_flags,
                XfopenFlags flags,
                અચલ અક્ષર *bind_name,
                FILE **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક xfopenat(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *mode, પૂર્ણાંક open_flags, FILE **ret) {
        પાછા xfopenat_full(dir_fd, path, mode, open_flags, 0, ખાલી, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક fopen_unlocked_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *mode, પૂર્ણાંક open_flags, FILE **ret) {
        પાછા xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, ખાલી, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક fopen_unlocked(અચલ અક્ષર *path, અચલ અક્ષર *mode, FILE **ret) {
        પાછા fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

પૂર્ણાંક fdopen_independent(પૂર્ણાંક fd, અચલ અક્ષર *mode, FILE **ret);

પૂર્ણાંક શોધ_and_open(અચલ અક્ષર *path, પૂર્ણાંક mode, અચલ અક્ષર *મૂળ, અક્ષર **શોધ, પૂર્ણાંક *ret_fd, અક્ષર **ret_path);
સ્થિર અંતઃરેખ પૂર્ણાંક શોધ_and_access(અચલ અક્ષર *path, પૂર્ણાંક mode, અચલ અક્ષર *મૂળ, અક્ષર**શોધ, અક્ષર **ret_path) {
        પાછા શોધ_and_open(path, mode, મૂળ, શોધ, ખાલી, ret_path);
}
પૂર્ણાંક શોધ_and_fopen(અચલ અક્ષર *path, અચલ અક્ષર *mode, અચલ અક્ષર *મૂળ, અચલ અક્ષર **શોધ, FILE **ret_file, અક્ષર **ret_path);
પૂર્ણાંક શોધ_and_fopen_nulstr(અચલ અક્ષર *path, અચલ અક્ષર *mode, અચલ અક્ષર *મૂળ, અચલ અક્ષર *શોધ, FILE **ret_file, અક્ષર **ret_path);

પૂર્ણાંક fflush_and_check(FILE *f);
પૂર્ણાંક fflush_sync_and_check(FILE *f);

પૂર્ણાંક write_timestamp_file_atomic(અચલ અક્ષર *fn, usec_t n);
પૂર્ણાંક read_timestamp_file(અચલ અક્ષર *fn, usec_t *ret);

પૂર્ણાંક fputs_with_separator(FILE *f, અચલ અક્ષર *s, અચલ અક્ષર *separator, bool *space);
પૂર્ણાંક fputs_with_newline(FILE *f, અચલ અક્ષર *s);

પ્રકારવ્યાખ્યા ગણના Read_લીટી_Flags {
        READ_લીટી_ONLY_NUL  = 1 << 0,
        READ_લીટી_IS_A_TTY  = 1 << 1,
        READ_લીટી_NOT_A_TTY = 1 << 2,
} Read_લીટી_Flags;

પૂર્ણાંક read_લીટી_full(FILE *f, size_t limit, Read_લીટી_Flags flags, અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક read_લીટી(FILE *f, size_t limit, અક્ષર **ret) {
        પાછા read_લીટી_full(f, limit, 0, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક read_nul_string(FILE *f, size_t limit, અક્ષર **ret) {
        પાછા read_લીટી_full(f, limit, READ_લીટી_ONLY_NUL, ret);
}

પૂર્ણાંક read_stripped_લીટી(FILE *f, size_t limit, અક્ષર **ret);

સ્થિર અંતઃરેખ bool file_offset_beyond_memory_size(off_t x) {
        જો (x < 0) /* off_t is signed, filter that out */
                પાછા false;
        પાછા (uint64_t) x > (uint64_t) SIZE_MAX;
}

પૂર્ણાંક safe_fgetc(FILE *f, અક્ષર *ret);

પૂર્ણાંક warn_file_is_world_accessible(અચલ અક્ષર *filename, સંરચના stat *st, અચલ અક્ષર *unit, unsigned લીટી);

પૂર્ણાંક fopen_mode_to_flags(અચલ અક્ષર *mode);

પ્રકારવ્યાખ્યા ગણના WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

પૂર્ણાંક write_data_file_atomic_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ સંરચના iovec *iovec, WriteDataFileFlags flags);
