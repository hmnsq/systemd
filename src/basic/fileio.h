/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define LONG_ओळ_MAX (1U*1024U*1024U)

प्रकारनिर्देश गणना {
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

प्रकारनिर्देश गणना {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

पूर्णाङ्क fdopen_unlocked(पूर्णाङ्क fd, ध्रुव वर्ण *options, FILE **ret);
पूर्णाङ्क take_fdopen_unlocked(पूर्णाङ्क *fd, ध्रुव वर्ण *options, FILE **ret);
FILE* take_fdopen(पूर्णाङ्क *fd, ध्रुव वर्ण *options);
DIR* take_fdopendir(पूर्णाङ्क *dfd);
FILE* open_memstream_unlocked(वर्ण **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(शून्य *buf, size_t size, ध्रुव वर्ण *mode);

पूर्णाङ्क write_string_stream_full(FILE *f, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags, ध्रुव संरचना timespec *ts);
स्थिर अन्तःस्थ पूर्णाङ्क write_string_stream(FILE *f, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags) {
        प्रत्यागम write_string_stream_full(f, ओळ, flags, रिक्त);
}

पूर्णाङ्क write_string_file_full_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags, ध्रुव संरचना timespec *ts, ध्रुव वर्ण *label_fn, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_full(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags, ध्रुव संरचना timespec *ts, ध्रुव वर्ण *label_fn) {
        प्रत्यागम write_string_file_full_label(dir_fd, fn, ओळ, flags, ts, label_fn, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags) {
        प्रत्यागम write_string_file_full(dir_fd, fn, ओळ, flags, रिक्त, रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_fd(पूर्णाङ्क dir_fd, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags) {
        प्रत्यागम write_string_file_at(dir_fd, रिक्त, ओळ, flags);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file(ध्रुव वर्ण *fn, ध्रुव वर्ण *ओळ, WriteStringFileFlags flags) {
        प्रत्यागम write_string_file_at(AT_FDCWD, fn, ओळ, flags);
}
पूर्णाङ्क write_string_filef(ध्रुव वर्ण *fn, WriteStringFileFlags flags, ध्रुव वर्ण *format, ...) _printf_(3, 4);
पूर्णाङ्क write_string_filef_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, WriteStringFileFlags flags, ध्रुव वर्ण *format, ...) _printf_(4, 5);

पूर्णाङ्क write_base64_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव संरचना iovec *data, WriteStringFileFlags flags);

पूर्णाङ्क read_one_ओळ_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क read_one_ओळ_file(ध्रुव वर्ण *filename, वर्ण **ret) {
        प्रत्यागम read_one_ओळ_file_at(AT_FDCWD, filename, ret);
}
पूर्णाङ्क read_boolean_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename);
स्थिर अन्तःस्थ पूर्णाङ्क read_boolean_file(ध्रुव वर्ण *filename) {
        प्रत्यागम read_boolean_file_at(AT_FDCWD, filename);
}
पूर्णाङ्क read_full_file_full(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, ध्रुव वर्ण *bind_name, वर्ण **ret_contents, size_t *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क read_full_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, वर्ण **ret_contents, size_t *ret_size) {
        प्रत्यागम read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_full_file(ध्रुव वर्ण *filename, वर्ण **ret_contents, size_t *ret_size) {
        प्रत्यागम read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_size);
}

पूर्णाङ्क read_virtual_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, size_t max_size, वर्ण **ret_contents, size_t *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क read_virtual_file_fd(पूर्णाङ्क fd, size_t max_size, वर्ण **ret_contents, size_t *ret_size) {
        प्रत्यागम read_virtual_file_at(fd, रिक्त, max_size, ret_contents, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_virtual_file(ध्रुव वर्ण *filename, size_t max_size, वर्ण **ret_contents, size_t *ret_size) {
        प्रत्यागम read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_full_virtual_file(ध्रुव वर्ण *filename, वर्ण **ret_contents, size_t *ret_size) {
        प्रत्यागम read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

पूर्णाङ्क read_full_stream_full(FILE *f, ध्रुव वर्ण *filename, uint64_t offset, size_t size, ReadFullFileFlags flags, वर्ण **ret_contents, size_t *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क read_full_stream(FILE *f, वर्ण **ret_contents, size_t *ret_size) {
        प्रत्यागम read_full_stream_full(f, रिक्त, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

पूर्णाङ्क verify_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *blob, bool accept_extra_nl);

पूर्णाङ्क script_get_shebang_interpreter(ध्रुव वर्ण *path, वर्ण **ret);

पूर्णाङ्क get_proc_field(ध्रुव वर्ण *path, ध्रुव वर्ण *किल्ली, वर्ण **ret);

DIR* xopendirat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, पूर्णाङ्क flags);

प्रकारनिर्देश गणना XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

पूर्णाङ्क xfopenat_full(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *path,
                ध्रुव वर्ण *mode,
                पूर्णाङ्क open_flags,
                XfopenFlags flags,
                ध्रुव वर्ण *bind_name,
                FILE **ret);
स्थिर अन्तःस्थ पूर्णाङ्क xfopenat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *mode, पूर्णाङ्क open_flags, FILE **ret) {
        प्रत्यागम xfopenat_full(dir_fd, path, mode, open_flags, 0, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fopen_unlocked_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *mode, पूर्णाङ्क open_flags, FILE **ret) {
        प्रत्यागम xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fopen_unlocked(ध्रुव वर्ण *path, ध्रुव वर्ण *mode, FILE **ret) {
        प्रत्यागम fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

पूर्णाङ्क fdopen_independent(पूर्णाङ्क fd, ध्रुव वर्ण *mode, FILE **ret);

पूर्णाङ्क शोध_and_open(ध्रुव वर्ण *path, पूर्णाङ्क mode, ध्रुव वर्ण *मूळ, वर्ण **शोध, पूर्णाङ्क *ret_fd, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क शोध_and_access(ध्रुव वर्ण *path, पूर्णाङ्क mode, ध्रुव वर्ण *मूळ, वर्ण**शोध, वर्ण **ret_path) {
        प्रत्यागम शोध_and_open(path, mode, मूळ, शोध, रिक्त, ret_path);
}
पूर्णाङ्क शोध_and_fopen(ध्रुव वर्ण *path, ध्रुव वर्ण *mode, ध्रुव वर्ण *मूळ, ध्रुव वर्ण **शोध, FILE **ret_file, वर्ण **ret_path);
पूर्णाङ्क शोध_and_fopen_nulstr(ध्रुव वर्ण *path, ध्रुव वर्ण *mode, ध्रुव वर्ण *मूळ, ध्रुव वर्ण *शोध, FILE **ret_file, वर्ण **ret_path);

पूर्णाङ्क fflush_and_check(FILE *f);
पूर्णाङ्क fflush_sync_and_check(FILE *f);

पूर्णाङ्क write_timestamp_file_atomic(ध्रुव वर्ण *fn, usec_t n);
पूर्णाङ्क read_timestamp_file(ध्रुव वर्ण *fn, usec_t *ret);

पूर्णाङ्क fputs_with_separator(FILE *f, ध्रुव वर्ण *s, ध्रुव वर्ण *separator, bool *space);
पूर्णाङ्क fputs_with_newline(FILE *f, ध्रुव वर्ण *s);

प्रकारनिर्देश गणना Read_ओळ_Flags {
        READ_ओळ_ONLY_NUL  = 1 << 0,
        READ_ओळ_IS_A_TTY  = 1 << 1,
        READ_ओळ_NOT_A_TTY = 1 << 2,
} Read_ओळ_Flags;

पूर्णाङ्क read_ओळ_full(FILE *f, size_t limit, Read_ओळ_Flags flags, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क read_ओळ(FILE *f, size_t limit, वर्ण **ret) {
        प्रत्यागम read_ओळ_full(f, limit, 0, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_nul_string(FILE *f, size_t limit, वर्ण **ret) {
        प्रत्यागम read_ओळ_full(f, limit, READ_ओळ_ONLY_NUL, ret);
}

पूर्णाङ्क read_stripped_ओळ(FILE *f, size_t limit, वर्ण **ret);

स्थिर अन्तःस्थ bool file_offset_beyond_memory_size(off_t x) {
        यदि (x < 0) /* off_t is signed, filter that out */
                प्रत्यागम false;
        प्रत्यागम (uint64_t) x > (uint64_t) SIZE_MAX;
}

पूर्णाङ्क safe_fgetc(FILE *f, वर्ण *ret);

पूर्णाङ्क warn_file_is_world_accessible(ध्रुव वर्ण *filename, संरचना stat *st, ध्रुव वर्ण *unit, unsigned ओळ);

पूर्णाङ्क fopen_mode_to_flags(ध्रुव वर्ण *mode);

प्रकारनिर्देश गणना WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

पूर्णाङ्क write_data_file_atomic_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव संरचना iovec *iovec, WriteDataFileFlags flags);
