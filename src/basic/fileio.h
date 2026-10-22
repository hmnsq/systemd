/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define LONG_সারি_MAX (1U*1024U*1024U)

প্রকারলেখ গণনা {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_বাফার             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_MODE_0600                  = 1 << 10,
        WRITE_STRING_FILE_MODE_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

প্রকারলেখ গণনা {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

পূর্ণসংখ্যা fdopen_unlocked(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *options, FILE **ret);
পূর্ণসংখ্যা take_fdopen_unlocked(পূর্ণসংখ্যা *fd, ধ্রুবক অক্ষর *options, FILE **ret);
FILE* take_fdopen(পূর্ণসংখ্যা *fd, ধ্রুবক অক্ষর *options);
DIR* take_fdopendir(পূর্ণসংখ্যা *dfd);
FILE* open_memstream_unlocked(অক্ষর **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(শূন্য *buf, size_t আকার_2, ধ্রুবক অক্ষর *mode);

পূর্ণসংখ্যা write_string_stream_full(FILE *f, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags, ধ্রুবক কাঠামো timespec *ts);
স্থির অন্তঃরেখ পূর্ণসংখ্যা write_string_stream(FILE *f, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags) {
        ফেরত write_string_stream_full(f, সারি, flags, ফাঁকা);
}

পূর্ণসংখ্যা write_string_file_full_label(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags, ধ্রুবক কাঠামো timespec *ts, ধ্রুবক অক্ষর *label_fn, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা write_string_file_full(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags, ধ্রুবক কাঠামো timespec *ts, ধ্রুবক অক্ষর *label_fn) {
        ফেরত write_string_file_full_label(dir_fd, fn, সারি, flags, ts, label_fn, /* label_context= */ ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা write_string_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags) {
        ফেরত write_string_file_full(dir_fd, fn, সারি, flags, ফাঁকা, ফাঁকা);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা write_string_file_fd(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags) {
        ফেরত write_string_file_at(dir_fd, ফাঁকা, সারি, flags);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা write_string_file(ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *সারি, WriteStringFileFlags flags) {
        ফেরত write_string_file_at(AT_FDCWD, fn, সারি, flags);
}
পূর্ণসংখ্যা write_string_filef(ধ্রুবক অক্ষর *fn, WriteStringFileFlags flags, ধ্রুবক অক্ষর *format, ...) _printf_(3, 4);
পূর্ণসংখ্যা write_string_filef_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, WriteStringFileFlags flags, ধ্রুবক অক্ষর *format, ...) _printf_(4, 5);

পূর্ণসংখ্যা write_base64_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, ধ্রুবক কাঠামো iovec *তথ্য, WriteStringFileFlags flags);

পূর্ণসংখ্যা read_one_সারি_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename, অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_one_সারি_file(ধ্রুবক অক্ষর *filename, অক্ষর **ret) {
        ফেরত read_one_সারি_file_at(AT_FDCWD, filename, ret);
}
পূর্ণসংখ্যা read_boolean_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename);
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_boolean_file(ধ্রুবক অক্ষর *filename) {
        ফেরত read_boolean_file_at(AT_FDCWD, filename);
}
পূর্ণসংখ্যা read_full_file_full(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename, uint64_t offset, size_t আকার_2, ReadFullFileFlags flags, ধ্রুবক অক্ষর *bind_name, অক্ষর **ret_contents, size_t *ret_আকার);
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_full_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename, অক্ষর **ret_contents, size_t *ret_আকার) {
        ফেরত read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ফাঁকা, ret_contents, ret_আকার);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_full_file(ধ্রুবক অক্ষর *filename, অক্ষর **ret_contents, size_t *ret_আকার) {
        ফেরত read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ফাঁকা, ret_contents, ret_আকার);
}

পূর্ণসংখ্যা read_virtual_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *filename, size_t max_আকার, অক্ষর **ret_contents, size_t *ret_আকার);
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_virtual_file_fd(পূর্ণসংখ্যা fd, size_t max_আকার, অক্ষর **ret_contents, size_t *ret_আকার) {
        ফেরত read_virtual_file_at(fd, ফাঁকা, max_আকার, ret_contents, ret_আকার);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_virtual_file(ধ্রুবক অক্ষর *filename, size_t max_আকার, অক্ষর **ret_contents, size_t *ret_আকার) {
        ফেরত read_virtual_file_at(AT_FDCWD, filename, max_আকার, ret_contents, ret_আকার);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_full_virtual_file(ধ্রুবক অক্ষর *filename, অক্ষর **ret_contents, size_t *ret_আকার) {
        ফেরত read_virtual_file(filename, SIZE_MAX, ret_contents, ret_আকার);
}

পূর্ণসংখ্যা read_full_stream_full(FILE *f, ধ্রুবক অক্ষর *filename, uint64_t offset, size_t আকার_2, ReadFullFileFlags flags, অক্ষর **ret_contents, size_t *ret_আকার);
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_full_stream(FILE *f, অক্ষর **ret_contents, size_t *ret_আকার) {
        ফেরত read_full_stream_full(f, ফাঁকা, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_আকার);
}

পূর্ণসংখ্যা verify_file_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর *blob, bool accept_extra_nl);

পূর্ণসংখ্যা script_get_shebang_interpreter(ধ্রুবক অক্ষর *path, অক্ষর **ret);

পূর্ণসংখ্যা get_proc_field(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *চাবি, অক্ষর **ret);

DIR* xopendirat(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags);

প্রকারলেখ গণনা XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

পূর্ণসংখ্যা xfopenat_full(
                পূর্ণসংখ্যা dir_fd,
                ধ্রুবক অক্ষর *path,
                ধ্রুবক অক্ষর *mode,
                পূর্ণসংখ্যা open_flags,
                XfopenFlags flags,
                ধ্রুবক অক্ষর *bind_name,
                FILE **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা xfopenat(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *mode, পূর্ণসংখ্যা open_flags, FILE **ret) {
        ফেরত xfopenat_full(dir_fd, path, mode, open_flags, 0, ফাঁকা, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা fopen_unlocked_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *mode, পূর্ণসংখ্যা open_flags, FILE **ret) {
        ফেরত xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, ফাঁকা, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা fopen_unlocked(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *mode, FILE **ret) {
        ফেরত fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

পূর্ণসংখ্যা fdopen_independent(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *mode, FILE **ret);

পূর্ণসংখ্যা অনুসন্ধান_and_open(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা mode, ধ্রুবক অক্ষর *মূল, অক্ষর **অনুসন্ধান, পূর্ণসংখ্যা *ret_fd, অক্ষর **ret_path);
স্থির অন্তঃরেখ পূর্ণসংখ্যা অনুসন্ধান_and_access(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা mode, ধ্রুবক অক্ষর *মূল, অক্ষর**অনুসন্ধান, অক্ষর **ret_path) {
        ফেরত অনুসন্ধান_and_open(path, mode, মূল, অনুসন্ধান, ফাঁকা, ret_path);
}
পূর্ণসংখ্যা অনুসন্ধান_and_fopen(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *mode, ধ্রুবক অক্ষর *মূল, ধ্রুবক অক্ষর **অনুসন্ধান, FILE **ret_file, অক্ষর **ret_path);
পূর্ণসংখ্যা অনুসন্ধান_and_fopen_nulstr(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *mode, ধ্রুবক অক্ষর *মূল, ধ্রুবক অক্ষর *অনুসন্ধান, FILE **ret_file, অক্ষর **ret_path);

পূর্ণসংখ্যা fflush_and_check(FILE *f);
পূর্ণসংখ্যা fflush_sync_and_check(FILE *f);

পূর্ণসংখ্যা write_timestamp_file_atomic(ধ্রুবক অক্ষর *fn, usec_t n);
পূর্ণসংখ্যা read_timestamp_file(ধ্রুবক অক্ষর *fn, usec_t *ret);

পূর্ণসংখ্যা fputs_with_separator(FILE *f, ধ্রুবক অক্ষর *s, ধ্রুবক অক্ষর *separator, bool *space);
পূর্ণসংখ্যা fputs_with_newline(FILE *f, ধ্রুবক অক্ষর *s);

প্রকারলেখ গণনা Read_সারি_Flags {
        READ_সারি_ONLY_NUL  = 1 << 0,
        READ_সারি_IS_A_TTY  = 1 << 1,
        READ_সারি_NOT_A_TTY = 1 << 2,
} Read_সারি_Flags;

পূর্ণসংখ্যা read_সারি_full(FILE *f, size_t limit, Read_সারি_Flags flags, অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_সারি(FILE *f, size_t limit, অক্ষর **ret) {
        ফেরত read_সারি_full(f, limit, 0, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা read_nul_string(FILE *f, size_t limit, অক্ষর **ret) {
        ফেরত read_সারি_full(f, limit, READ_সারি_ONLY_NUL, ret);
}

পূর্ণসংখ্যা read_stripped_সারি(FILE *f, size_t limit, অক্ষর **ret);

স্থির অন্তঃরেখ bool file_offset_beyond_memory_আকার(off_t x) {
        যদি (x < 0) /* off_t is signed, filter that out */
                ফেরত false;
        ফেরত (uint64_t) x > (uint64_t) SIZE_MAX;
}

পূর্ণসংখ্যা safe_fgetc(FILE *f, অক্ষর *ret);

পূর্ণসংখ্যা warn_file_is_world_accessible(ধ্রুবক অক্ষর *filename, কাঠামো stat *st, ধ্রুবক অক্ষর *unit, unsigned সারি);

পূর্ণসংখ্যা fopen_mode_to_flags(ধ্রুবক অক্ষর *mode);

প্রকারলেখ গণনা Write_তথ্য_File_Flags {
        WRITE_তথ্য_FILE_MKDIR_0755 = 1 << 0,
        WRITE_তথ্য_FILE_MODE_0400  = 1 << 1,
} Write_তথ্য_File_Flags;

পূর্ণসংখ্যা write_তথ্য_file_atomic_at(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, ধ্রুবক কাঠামো iovec *iovec, Write_তথ্য_File_Flags flags);
