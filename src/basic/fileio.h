/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define LONG_ओळ_MAX (1U*1024U*1024U)

प्रकारव्याख्या गणना {
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

प्रकारव्याख्या गणना {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

पूर्णांक fdopen_unlocked(पूर्णांक fd, स्थिरांक अक्षर *options, खाता **ret);
पूर्णांक take_fdopen_unlocked(पूर्णांक *fd, स्थिरांक अक्षर *options, खाता **ret);
खाता* take_fdopen(पूर्णांक *fd, स्थिरांक अक्षर *options);
सूची* take_fdopendir(पूर्णांक *dfd);
खाता* open_memstream_unlocked(अक्षर **ptr, माप_प्रकार *sizeloc);
खाता* fmemopen_unlocked(शून्य *buf, माप_प्रकार size, स्थिरांक अक्षर *mode);

पूर्णांक write_string_stream_full(खाता *f, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags, स्थिरांक संरचना timespec *ts);
स्थिर ओळीत पूर्णांक write_string_stream(खाता *f, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags) {
        परत write_string_stream_full(f, ओळ, flags, रिक्त);
}

पूर्णांक write_string_file_full_label(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags, स्थिरांक संरचना timespec *ts, स्थिरांक अक्षर *label_fn, LabelContext *label_context);
स्थिर ओळीत पूर्णांक write_string_file_full(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags, स्थिरांक संरचना timespec *ts, स्थिरांक अक्षर *label_fn) {
        परत write_string_file_full_label(dir_fd, fn, ओळ, flags, ts, label_fn, /* label_context= */ रिक्त);
}
स्थिर ओळीत पूर्णांक write_string_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags) {
        परत write_string_file_full(dir_fd, fn, ओळ, flags, रिक्त, रिक्त);
}
स्थिर ओळीत पूर्णांक write_string_file_fd(पूर्णांक dir_fd, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags) {
        परत write_string_file_at(dir_fd, रिक्त, ओळ, flags);
}
स्थिर ओळीत पूर्णांक write_string_file(स्थिरांक अक्षर *fn, स्थिरांक अक्षर *ओळ, WriteStringFileFlags flags) {
        परत write_string_file_at(AT_FDCWD, fn, ओळ, flags);
}
पूर्णांक write_string_filef(स्थिरांक अक्षर *fn, WriteStringFileFlags flags, स्थिरांक अक्षर *format, ...) _printf_(3, 4);
पूर्णांक write_string_filef_at(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, WriteStringFileFlags flags, स्थिरांक अक्षर *format, ...) _printf_(4, 5);

पूर्णांक write_base64_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, स्थिरांक संरचना iovec *data, WriteStringFileFlags flags);

पूर्णांक read_one_ओळ_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *filename, अक्षर **ret);
स्थिर ओळीत पूर्णांक read_one_ओळ_file(स्थिरांक अक्षर *filename, अक्षर **ret) {
        परत read_one_ओळ_file_at(AT_FDCWD, filename, ret);
}
पूर्णांक read_boolean_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *filename);
स्थिर ओळीत पूर्णांक read_boolean_file(स्थिरांक अक्षर *filename) {
        परत read_boolean_file_at(AT_FDCWD, filename);
}
पूर्णांक read_full_file_full(पूर्णांक dir_fd, स्थिरांक अक्षर *filename, uint64_t offset, माप_प्रकार size, ReadFullFileFlags flags, स्थिरांक अक्षर *bind_name, अक्षर **ret_contents, माप_प्रकार *ret_size);
स्थिर ओळीत पूर्णांक read_full_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *filename, अक्षर **ret_contents, माप_प्रकार *ret_size) {
        परत read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_size);
}
स्थिर ओळीत पूर्णांक read_full_file(स्थिरांक अक्षर *filename, अक्षर **ret_contents, माप_प्रकार *ret_size) {
        परत read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_size);
}

पूर्णांक read_virtual_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *filename, माप_प्रकार max_size, अक्षर **ret_contents, माप_प्रकार *ret_size);
स्थिर ओळीत पूर्णांक read_virtual_file_fd(पूर्णांक fd, माप_प्रकार max_size, अक्षर **ret_contents, माप_प्रकार *ret_size) {
        परत read_virtual_file_at(fd, रिक्त, max_size, ret_contents, ret_size);
}
स्थिर ओळीत पूर्णांक read_virtual_file(स्थिरांक अक्षर *filename, माप_प्रकार max_size, अक्षर **ret_contents, माप_प्रकार *ret_size) {
        परत read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
स्थिर ओळीत पूर्णांक read_full_virtual_file(स्थिरांक अक्षर *filename, अक्षर **ret_contents, माप_प्रकार *ret_size) {
        परत read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

पूर्णांक read_full_stream_full(खाता *f, स्थिरांक अक्षर *filename, uint64_t offset, माप_प्रकार size, ReadFullFileFlags flags, अक्षर **ret_contents, माप_प्रकार *ret_size);
स्थिर ओळीत पूर्णांक read_full_stream(खाता *f, अक्षर **ret_contents, माप_प्रकार *ret_size) {
        परत read_full_stream_full(f, रिक्त, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

पूर्णांक verify_file_at(पूर्णांक dir_fd, स्थिरांक अक्षर *fn, स्थिरांक अक्षर *blob, bool accept_extra_nl);

पूर्णांक script_get_shebang_interpreter(स्थिरांक अक्षर *path, अक्षर **ret);

पूर्णांक get_proc_field(स्थिरांक अक्षर *path, स्थिरांक अक्षर *किल्ली, अक्षर **ret);

सूची* xopendirat(पूर्णांक dir_fd, स्थिरांक अक्षर *path, पूर्णांक flags);

प्रकारव्याख्या गणना XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

पूर्णांक xfopenat_full(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *path,
                स्थिरांक अक्षर *mode,
                पूर्णांक open_flags,
                XfopenFlags flags,
                स्थिरांक अक्षर *bind_name,
                खाता **ret);
स्थिर ओळीत पूर्णांक xfopenat(पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *mode, पूर्णांक open_flags, खाता **ret) {
        परत xfopenat_full(dir_fd, path, mode, open_flags, 0, रिक्त, ret);
}
स्थिर ओळीत पूर्णांक fopen_unlocked_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *mode, पूर्णांक open_flags, खाता **ret) {
        परत xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, रिक्त, ret);
}
स्थिर ओळीत पूर्णांक fopen_unlocked(स्थिरांक अक्षर *path, स्थिरांक अक्षर *mode, खाता **ret) {
        परत fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

पूर्णांक fdopen_independent(पूर्णांक fd, स्थिरांक अक्षर *mode, खाता **ret);

पूर्णांक शोध_and_open(स्थिरांक अक्षर *path, पूर्णांक mode, स्थिरांक अक्षर *मूळ, अक्षर **शोध, पूर्णांक *ret_fd, अक्षर **ret_path);
स्थिर ओळीत पूर्णांक शोध_and_access(स्थिरांक अक्षर *path, पूर्णांक mode, स्थिरांक अक्षर *मूळ, अक्षर**शोध, अक्षर **ret_path) {
        परत शोध_and_open(path, mode, मूळ, शोध, रिक्त, ret_path);
}
पूर्णांक शोध_and_fopen(स्थिरांक अक्षर *path, स्थिरांक अक्षर *mode, स्थिरांक अक्षर *मूळ, स्थिरांक अक्षर **शोध, खाता **ret_file, अक्षर **ret_path);
पूर्णांक शोध_and_fopen_nulstr(स्थिरांक अक्षर *path, स्थिरांक अक्षर *mode, स्थिरांक अक्षर *मूळ, स्थिरांक अक्षर *शोध, खाता **ret_file, अक्षर **ret_path);

पूर्णांक fflush_and_check(खाता *f);
पूर्णांक fflush_sync_and_check(खाता *f);

पूर्णांक write_timestamp_file_atomic(स्थिरांक अक्षर *fn, usec_t n);
पूर्णांक read_timestamp_file(स्थिरांक अक्षर *fn, usec_t *ret);

पूर्णांक fputs_with_separator(खाता *f, स्थिरांक अक्षर *s, स्थिरांक अक्षर *separator, bool *space);
पूर्णांक fputs_with_newline(खाता *f, स्थिरांक अक्षर *s);

प्रकारव्याख्या गणना Read_ओळ_Flags {
        READ_ओळ_ONLY_NUL  = 1 << 0,
        READ_ओळ_IS_A_TTY  = 1 << 1,
        READ_ओळ_NOT_A_TTY = 1 << 2,
} Read_ओळ_Flags;

पूर्णांक read_ओळ_full(खाता *f, माप_प्रकार limit, Read_ओळ_Flags flags, अक्षर **ret);
स्थिर ओळीत पूर्णांक read_ओळ(खाता *f, माप_प्रकार limit, अक्षर **ret) {
        परत read_ओळ_full(f, limit, 0, ret);
}
स्थिर ओळीत पूर्णांक read_nul_string(खाता *f, माप_प्रकार limit, अक्षर **ret) {
        परत read_ओळ_full(f, limit, READ_ओळ_ONLY_NUL, ret);
}

पूर्णांक read_stripped_ओळ(खाता *f, माप_प्रकार limit, अक्षर **ret);

स्थिर ओळीत bool file_offset_beyond_memory_size(off_t x) {
        जर (x < 0) /* off_t is signed, filter that out */
                परत false;
        परत (uint64_t) x > (uint64_t) SIZE_MAX;
}

पूर्णांक safe_fgetc(खाता *f, अक्षर *ret);

पूर्णांक warn_file_is_world_accessible(स्थिरांक अक्षर *filename, संरचना stat *st, स्थिरांक अक्षर *unit, अचिन्हित ओळ);

पूर्णांक fopen_mode_to_flags(स्थिरांक अक्षर *mode);

प्रकारव्याख्या गणना WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

पूर्णांक write_data_file_atomic_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक संरचना iovec *iovec, WriteDataFileFlags flags);
