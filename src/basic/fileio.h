/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define LONG_ಸಾಲು_MAX (1U*1024U*1024U)

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

ಪೂರ್ಣಾಙ್ಕ fdopen_unlocked(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *options, FILE **ret);
ಪೂರ್ಣಾಙ್ಕ take_fdopen_unlocked(ಪೂರ್ಣಾಙ್ಕ *fd, ಧ್ರುವ ವರ್ಣ *options, FILE **ret);
FILE* take_fdopen(ಪೂರ್ಣಾಙ್ಕ *fd, ಧ್ರುವ ವರ್ಣ *options);
DIR* take_fdopendir(ಪೂರ್ಣಾಙ್ಕ *dfd);
FILE* open_memstream_unlocked(ವರ್ಣ **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(ಶೂನ್ಯ *buf, size_t size, ಧ್ರುವ ವರ್ಣ *mode);

ಪೂರ್ಣಾಙ್ಕ write_string_stream_full(FILE *f, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags, ಧ್ರುವ ಸಂರಚನಾ timespec *ts);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_string_stream(FILE *f, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ write_string_stream_full(f, ಸಾಲು, flags, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ write_string_file_full_label(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags, ಧ್ರುವ ಸಂರಚನಾ timespec *ts, ಧ್ರುವ ವರ್ಣ *label_fn, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_string_file_full(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags, ಧ್ರುವ ಸಂರಚನಾ timespec *ts, ಧ್ರುವ ವರ್ಣ *label_fn) {
        ಪ್ರತ್ಯಾಗಮ write_string_file_full_label(dir_fd, fn, ಸಾಲು, flags, ts, label_fn, /* label_context= */ ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_string_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ write_string_file_full(dir_fd, fn, ಸಾಲು, flags, ರಿಕ್ತ, ರಿಕ್ತ);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_string_file_fd(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ write_string_file_at(dir_fd, ರಿಕ್ತ, ಸಾಲು, flags);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ write_string_file(ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *ಸಾಲು, WriteStringFileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ write_string_file_at(AT_FDCWD, fn, ಸಾಲು, flags);
}
ಪೂರ್ಣಾಙ್ಕ write_string_filef(ಧ್ರುವ ವರ್ಣ *fn, WriteStringFileFlags flags, ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(3, 4);
ಪೂರ್ಣಾಙ್ಕ write_string_filef_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, WriteStringFileFlags flags, ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(4, 5);

ಪೂರ್ಣಾಙ್ಕ write_base64_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ಸಂರಚನಾ iovec *data, WriteStringFileFlags flags);

ಪೂರ್ಣಾಙ್ಕ read_one_ಸಾಲು_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_one_ಸಾಲು_file(ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ read_one_ಸಾಲು_file_at(AT_FDCWD, filename, ret);
}
ಪೂರ್ಣಾಙ್ಕ read_boolean_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_boolean_file(ಧ್ರುವ ವರ್ಣ *filename) {
        ಪ್ರತ್ಯಾಗಮ read_boolean_file_at(AT_FDCWD, filename);
}
ಪೂರ್ಣಾಙ್ಕ read_full_file_full(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, ಧ್ರುವ ವರ್ಣ *bind_name, ವರ್ಣ **ret_contents, size_t *ret_size);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_full_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret_contents, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ರಿಕ್ತ, ret_contents, ret_size);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_full_file(ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret_contents, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ರಿಕ್ತ, ret_contents, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ read_virtual_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *filename, size_t max_size, ವರ್ಣ **ret_contents, size_t *ret_size);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_virtual_file_fd(ಪೂರ್ಣಾಙ್ಕ fd, size_t max_size, ವರ್ಣ **ret_contents, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ read_virtual_file_at(fd, ರಿಕ್ತ, max_size, ret_contents, ret_size);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_virtual_file(ಧ್ರುವ ವರ್ಣ *filename, size_t max_size, ವರ್ಣ **ret_contents, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_full_virtual_file(ಧ್ರುವ ವರ್ಣ *filename, ವರ್ಣ **ret_contents, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ read_full_stream_full(FILE *f, ಧ್ರುವ ವರ್ಣ *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, ವರ್ಣ **ret_contents, size_t *ret_size);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_full_stream(FILE *f, ವರ್ಣ **ret_contents, size_t *ret_size) {
        ಪ್ರತ್ಯಾಗಮ read_full_stream_full(f, ರಿಕ್ತ, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

ಪೂರ್ಣಾಙ್ಕ verify_file_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ *blob, bool accept_extra_nl);

ಪೂರ್ಣಾಙ್ಕ script_get_shebang_interpreter(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ get_proc_field(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ವರ್ಣ **ret);

DIR* xopendirat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ flags);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

ಪೂರ್ಣಾಙ್ಕ xfopenat_full(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                ಧ್ರುವ ವರ್ಣ *mode,
                ಪೂರ್ಣಾಙ್ಕ open_flags,
                XfopenFlags flags,
                ಧ್ರುವ ವರ್ಣ *bind_name,
                FILE **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ xfopenat(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *mode, ಪೂರ್ಣಾಙ್ಕ open_flags, FILE **ret) {
        ಪ್ರತ್ಯಾಗಮ xfopenat_full(dir_fd, path, mode, open_flags, 0, ರಿಕ್ತ, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fopen_unlocked_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *mode, ಪೂರ್ಣಾಙ್ಕ open_flags, FILE **ret) {
        ಪ್ರತ್ಯಾಗಮ xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, ರಿಕ್ತ, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fopen_unlocked(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *mode, FILE **ret) {
        ಪ್ರತ್ಯಾಗಮ fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

ಪೂರ್ಣಾಙ್ಕ fdopen_independent(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *mode, FILE **ret);

ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_open(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ mode, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ **ಹುಡುಕು, ಪೂರ್ಣಾಙ್ಕ *ret_fd, ವರ್ಣ **ret_path);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_access(ಧ್ರುವ ವರ್ಣ *path, ಪೂರ್ಣಾಙ್ಕ mode, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ವರ್ಣ**ಹುಡುಕು, ವರ್ಣ **ret_path) {
        ಪ್ರತ್ಯಾಗಮ ಹುಡುಕು_and_open(path, mode, ಬೇರು, ಹುಡುಕು, ರಿಕ್ತ, ret_path);
}
ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_fopen(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *mode, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ **ಹುಡುಕು, FILE **ret_file, ವರ್ಣ **ret_path);
ಪೂರ್ಣಾಙ್ಕ ಹುಡುಕು_and_fopen_nulstr(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *mode, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ *ಹುಡುಕು, FILE **ret_file, ವರ್ಣ **ret_path);

ಪೂರ್ಣಾಙ್ಕ fflush_and_check(FILE *f);
ಪೂರ್ಣಾಙ್ಕ fflush_sync_and_check(FILE *f);

ಪೂರ್ಣಾಙ್ಕ write_timestamp_file_atomic(ಧ್ರುವ ವರ್ಣ *fn, usec_t n);
ಪೂರ್ಣಾಙ್ಕ read_timestamp_file(ಧ್ರುವ ವರ್ಣ *fn, usec_t *ret);

ಪೂರ್ಣಾಙ್ಕ fputs_with_separator(FILE *f, ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *separator, bool *space);
ಪೂರ್ಣಾಙ್ಕ fputs_with_newline(FILE *f, ಧ್ರುವ ವರ್ಣ *s);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ Read_ಸಾಲು_Flags {
        READ_ಸಾಲು_ONLY_NUL  = 1 << 0,
        READ_ಸಾಲು_IS_A_TTY  = 1 << 1,
        READ_ಸಾಲು_NOT_A_TTY = 1 << 2,
} Read_ಸಾಲು_Flags;

ಪೂರ್ಣಾಙ್ಕ read_ಸಾಲು_full(FILE *f, size_t limit, Read_ಸಾಲು_Flags flags, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_ಸಾಲು(FILE *f, size_t limit, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ read_ಸಾಲು_full(f, limit, 0, ret);
}
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ read_nul_string(FILE *f, size_t limit, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ read_ಸಾಲು_full(f, limit, READ_ಸಾಲು_ONLY_NUL, ret);
}

ಪೂರ್ಣಾಙ್ಕ read_stripped_ಸಾಲು(FILE *f, size_t limit, ವರ್ಣ **ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool file_offset_beyond_memory_size(off_t x) {
        ಯದಿ (x < 0) /* off_t is signed, filter that out */
                ಪ್ರತ್ಯಾಗಮ false;
        ಪ್ರತ್ಯಾಗಮ (uint64_t) x > (uint64_t) SIZE_MAX;
}

ಪೂರ್ಣಾಙ್ಕ safe_fgetc(FILE *f, ವರ್ಣ *ret);

ಪೂರ್ಣಾಙ್ಕ warn_file_is_world_accessible(ಧ್ರುವ ವರ್ಣ *filename, ಸಂರಚನಾ stat *st, ಧ್ರುವ ವರ್ಣ *unit, unsigned ಸಾಲು);

ಪೂರ್ಣಾಙ್ಕ fopen_mode_to_flags(ಧ್ರುವ ವರ್ಣ *mode);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

ಪೂರ್ಣಾಙ್ಕ write_data_file_atomic_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ಸಂರಚನಾ iovec *iovec, WriteDataFileFlags flags);
