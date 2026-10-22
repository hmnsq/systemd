/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define LONG_வரி_MAX (1U*1024U*1024U)

வகைவரை எண்ணிக்கை {
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

வகைவரை எண்ணிக்கை {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

முழு fdopen_unlocked(முழு fd, மாறிலி எழுத்து *options, FILE **ret);
முழு take_fdopen_unlocked(முழு *fd, மாறிலி எழுத்து *options, FILE **ret);
FILE* take_fdopen(முழு *fd, மாறிலி எழுத்து *options);
DIR* take_fdopendir(முழு *dfd);
FILE* open_memstream_unlocked(எழுத்து **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(வெறுமை *buf, size_t size, மாறிலி எழுத்து *mode);

முழு write_string_stream_full(FILE *f, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags, மாறிலி கட்டமைப்பு timespec *ts);
நிலையான உள்வரி முழு write_string_stream(FILE *f, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags) {
        திரும்பு write_string_stream_full(f, வரி, flags, வெற்று);
}

முழு write_string_file_full_label(முழு dir_fd, மாறிலி எழுத்து *fn, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags, மாறிலி கட்டமைப்பு timespec *ts, மாறிலி எழுத்து *label_fn, LabelContext *label_context);
நிலையான உள்வரி முழு write_string_file_full(முழு dir_fd, மாறிலி எழுத்து *fn, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags, மாறிலி கட்டமைப்பு timespec *ts, மாறிலி எழுத்து *label_fn) {
        திரும்பு write_string_file_full_label(dir_fd, fn, வரி, flags, ts, label_fn, /* label_context= */ வெற்று);
}
நிலையான உள்வரி முழு write_string_file_at(முழு dir_fd, மாறிலி எழுத்து *fn, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags) {
        திரும்பு write_string_file_full(dir_fd, fn, வரி, flags, வெற்று, வெற்று);
}
நிலையான உள்வரி முழு write_string_file_fd(முழு dir_fd, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags) {
        திரும்பு write_string_file_at(dir_fd, வெற்று, வரி, flags);
}
நிலையான உள்வரி முழு write_string_file(மாறிலி எழுத்து *fn, மாறிலி எழுத்து *வரி, WriteStringFileFlags flags) {
        திரும்பு write_string_file_at(AT_FDCWD, fn, வரி, flags);
}
முழு write_string_filef(மாறிலி எழுத்து *fn, WriteStringFileFlags flags, மாறிலி எழுத்து *format, ...) _printf_(3, 4);
முழு write_string_filef_at(முழு dir_fd, மாறிலி எழுத்து *fn, WriteStringFileFlags flags, மாறிலி எழுத்து *format, ...) _printf_(4, 5);

முழு write_base64_file_at(முழு dir_fd, மாறிலி எழுத்து *fn, மாறிலி கட்டமைப்பு iovec *data, WriteStringFileFlags flags);

முழு read_one_வரி_file_at(முழு dir_fd, மாறிலி எழுத்து *filename, எழுத்து **ret);
நிலையான உள்வரி முழு read_one_வரி_file(மாறிலி எழுத்து *filename, எழுத்து **ret) {
        திரும்பு read_one_வரி_file_at(AT_FDCWD, filename, ret);
}
முழு read_boolean_file_at(முழு dir_fd, மாறிலி எழுத்து *filename);
நிலையான உள்வரி முழு read_boolean_file(மாறிலி எழுத்து *filename) {
        திரும்பு read_boolean_file_at(AT_FDCWD, filename);
}
முழு read_full_file_full(முழு dir_fd, மாறிலி எழுத்து *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, மாறிலி எழுத்து *bind_name, எழுத்து **ret_contents, size_t *ret_size);
நிலையான உள்வரி முழு read_full_file_at(முழு dir_fd, மாறிலி எழுத்து *filename, எழுத்து **ret_contents, size_t *ret_size) {
        திரும்பு read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, வெற்று, ret_contents, ret_size);
}
நிலையான உள்வரி முழு read_full_file(மாறிலி எழுத்து *filename, எழுத்து **ret_contents, size_t *ret_size) {
        திரும்பு read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, வெற்று, ret_contents, ret_size);
}

முழு read_virtual_file_at(முழு dir_fd, மாறிலி எழுத்து *filename, size_t max_size, எழுத்து **ret_contents, size_t *ret_size);
நிலையான உள்வரி முழு read_virtual_file_fd(முழு fd, size_t max_size, எழுத்து **ret_contents, size_t *ret_size) {
        திரும்பு read_virtual_file_at(fd, வெற்று, max_size, ret_contents, ret_size);
}
நிலையான உள்வரி முழு read_virtual_file(மாறிலி எழுத்து *filename, size_t max_size, எழுத்து **ret_contents, size_t *ret_size) {
        திரும்பு read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
நிலையான உள்வரி முழு read_full_virtual_file(மாறிலி எழுத்து *filename, எழுத்து **ret_contents, size_t *ret_size) {
        திரும்பு read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

முழு read_full_stream_full(FILE *f, மாறிலி எழுத்து *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, எழுத்து **ret_contents, size_t *ret_size);
நிலையான உள்வரி முழு read_full_stream(FILE *f, எழுத்து **ret_contents, size_t *ret_size) {
        திரும்பு read_full_stream_full(f, வெற்று, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

முழு verify_file_at(முழு dir_fd, மாறிலி எழுத்து *fn, மாறிலி எழுத்து *blob, bool accept_extra_nl);

முழு script_get_shebang_interpreter(மாறிலி எழுத்து *path, எழுத்து **ret);

முழு get_proc_field(மாறிலி எழுத்து *path, மாறிலி எழுத்து *சாவி, எழுத்து **ret);

DIR* xopendirat(முழு dir_fd, மாறிலி எழுத்து *path, முழு flags);

வகைவரை எண்ணிக்கை XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

முழு xfopenat_full(
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                மாறிலி எழுத்து *mode,
                முழு open_flags,
                XfopenFlags flags,
                மாறிலி எழுத்து *bind_name,
                FILE **ret);
நிலையான உள்வரி முழு xfopenat(முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *mode, முழு open_flags, FILE **ret) {
        திரும்பு xfopenat_full(dir_fd, path, mode, open_flags, 0, வெற்று, ret);
}
நிலையான உள்வரி முழு fopen_unlocked_at(முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *mode, முழு open_flags, FILE **ret) {
        திரும்பு xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, வெற்று, ret);
}
நிலையான உள்வரி முழு fopen_unlocked(மாறிலி எழுத்து *path, மாறிலி எழுத்து *mode, FILE **ret) {
        திரும்பு fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

முழு fdopen_independent(முழு fd, மாறிலி எழுத்து *mode, FILE **ret);

முழு தேடல்_and_open(மாறிலி எழுத்து *path, முழு mode, மாறிலி எழுத்து *வேர், எழுத்து **தேடல், முழு *ret_fd, எழுத்து **ret_path);
நிலையான உள்வரி முழு தேடல்_and_access(மாறிலி எழுத்து *path, முழு mode, மாறிலி எழுத்து *வேர், எழுத்து**தேடல், எழுத்து **ret_path) {
        திரும்பு தேடல்_and_open(path, mode, வேர், தேடல், வெற்று, ret_path);
}
முழு தேடல்_and_fopen(மாறிலி எழுத்து *path, மாறிலி எழுத்து *mode, மாறிலி எழுத்து *வேர், மாறிலி எழுத்து **தேடல், FILE **ret_file, எழுத்து **ret_path);
முழு தேடல்_and_fopen_nulstr(மாறிலி எழுத்து *path, மாறிலி எழுத்து *mode, மாறிலி எழுத்து *வேர், மாறிலி எழுத்து *தேடல், FILE **ret_file, எழுத்து **ret_path);

முழு fflush_and_check(FILE *f);
முழு fflush_sync_and_check(FILE *f);

முழு write_timestamp_file_atomic(மாறிலி எழுத்து *fn, usec_t n);
முழு read_timestamp_file(மாறிலி எழுத்து *fn, usec_t *ret);

முழு fputs_with_separator(FILE *f, மாறிலி எழுத்து *s, மாறிலி எழுத்து *separator, bool *space);
முழு fputs_with_newline(FILE *f, மாறிலி எழுத்து *s);

வகைவரை எண்ணிக்கை Read_வரி_Flags {
        READ_வரி_ONLY_NUL  = 1 << 0,
        READ_வரி_IS_A_TTY  = 1 << 1,
        READ_வரி_NOT_A_TTY = 1 << 2,
} Read_வரி_Flags;

முழு read_வரி_full(FILE *f, size_t limit, Read_வரி_Flags flags, எழுத்து **ret);
நிலையான உள்வரி முழு read_வரி(FILE *f, size_t limit, எழுத்து **ret) {
        திரும்பு read_வரி_full(f, limit, 0, ret);
}
நிலையான உள்வரி முழு read_nul_string(FILE *f, size_t limit, எழுத்து **ret) {
        திரும்பு read_வரி_full(f, limit, READ_வரி_ONLY_NUL, ret);
}

முழு read_stripped_வரி(FILE *f, size_t limit, எழுத்து **ret);

நிலையான உள்வரி bool file_offset_beyond_memory_size(off_t x) {
        எனில் (x < 0) /* off_t is signed, filter that out */
                திரும்பு false;
        திரும்பு (uint64_t) x > (uint64_t) SIZE_MAX;
}

முழு safe_fgetc(FILE *f, எழுத்து *ret);

முழு warn_file_is_world_accessible(மாறிலி எழுத்து *filename, கட்டமைப்பு stat *st, மாறிலி எழுத்து *unit, unsigned வரி);

முழு fopen_mode_to_flags(மாறிலி எழுத்து *mode);

வகைவரை எண்ணிக்கை WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

முழு write_data_file_atomic_at(முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி கட்டமைப்பு iovec *iovec, WriteDataFileFlags flags);
