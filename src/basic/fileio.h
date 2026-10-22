/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define LONG_पङ्क्ति_MAX (1U*1024U*1024U)

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

पूर्णाङ्क fdopen_unlocked(पूर्णाङ्क fd, अचर वर्ण *options, खाता **ret);
पूर्णाङ्क take_fdopen_unlocked(पूर्णाङ्क *fd, अचर वर्ण *options, खाता **ret);
खाता* take_fdopen(पूर्णाङ्क *fd, अचर वर्ण *options);
सूची* take_fdopendir(पूर्णाङ्क *dfd);
खाता* open_memstream_unlocked(वर्ण **ptr, माप_प्रकार *sizeloc);
खाता* fmemopen_unlocked(शून्य *buf, माप_प्रकार size, अचर वर्ण *mode);

पूर्णाङ्क write_string_stream_full(खाता *f, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags, अचर संरचना timespec *ts);
स्थिर अन्तःस्थ पूर्णाङ्क write_string_stream(खाता *f, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags) {
        फर्काऊ write_string_stream_full(f, पङ्क्ति, flags, रिक्त);
}

पूर्णाङ्क write_string_file_full_label(पूर्णाङ्क dir_fd, अचर वर्ण *fn, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags, अचर संरचना timespec *ts, अचर वर्ण *label_fn, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_full(पूर्णाङ्क dir_fd, अचर वर्ण *fn, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags, अचर संरचना timespec *ts, अचर वर्ण *label_fn) {
        फर्काऊ write_string_file_full_label(dir_fd, fn, पङ्क्ति, flags, ts, label_fn, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *fn, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags) {
        फर्काऊ write_string_file_full(dir_fd, fn, पङ्क्ति, flags, रिक्त, रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_fd(पूर्णाङ्क dir_fd, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags) {
        फर्काऊ write_string_file_at(dir_fd, रिक्त, पङ्क्ति, flags);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file(अचर वर्ण *fn, अचर वर्ण *पङ्क्ति, WriteStringFileFlags flags) {
        फर्काऊ write_string_file_at(AT_FDCWD, fn, पङ्क्ति, flags);
}
पूर्णाङ्क write_string_filef(अचर वर्ण *fn, WriteStringFileFlags flags, अचर वर्ण *format, ...) _printf_(3, 4);
पूर्णाङ्क write_string_filef_at(पूर्णाङ्क dir_fd, अचर वर्ण *fn, WriteStringFileFlags flags, अचर वर्ण *format, ...) _printf_(4, 5);

पूर्णाङ्क write_base64_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *fn, अचर संरचना iovec *data, WriteStringFileFlags flags);

पूर्णाङ्क read_one_पङ्क्ति_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *filename, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क read_one_पङ्क्ति_file(अचर वर्ण *filename, वर्ण **ret) {
        फर्काऊ read_one_पङ्क्ति_file_at(AT_FDCWD, filename, ret);
}
पूर्णाङ्क read_boolean_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *filename);
स्थिर अन्तःस्थ पूर्णाङ्क read_boolean_file(अचर वर्ण *filename) {
        फर्काऊ read_boolean_file_at(AT_FDCWD, filename);
}
पूर्णाङ्क read_full_file_full(पूर्णाङ्क dir_fd, अचर वर्ण *filename, uint64_t offset, माप_प्रकार size, ReadFullFileFlags flags, अचर वर्ण *bind_name, वर्ण **ret_contents, माप_प्रकार *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क read_full_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *filename, वर्ण **ret_contents, माप_प्रकार *ret_size) {
        फर्काऊ read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_full_file(अचर वर्ण *filename, वर्ण **ret_contents, माप_प्रकार *ret_size) {
        फर्काऊ read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_size);
}

पूर्णाङ्क read_virtual_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *filename, माप_प्रकार max_size, वर्ण **ret_contents, माप_प्रकार *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क read_virtual_file_fd(पूर्णाङ्क fd, माप_प्रकार max_size, वर्ण **ret_contents, माप_प्रकार *ret_size) {
        फर्काऊ read_virtual_file_at(fd, रिक्त, max_size, ret_contents, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_virtual_file(अचर वर्ण *filename, माप_प्रकार max_size, वर्ण **ret_contents, माप_प्रकार *ret_size) {
        फर्काऊ read_virtual_file_at(AT_FDCWD, filename, max_size, ret_contents, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_full_virtual_file(अचर वर्ण *filename, वर्ण **ret_contents, माप_प्रकार *ret_size) {
        फर्काऊ read_virtual_file(filename, SIZE_MAX, ret_contents, ret_size);
}

पूर्णाङ्क read_full_stream_full(खाता *f, अचर वर्ण *filename, uint64_t offset, माप_प्रकार size, ReadFullFileFlags flags, वर्ण **ret_contents, माप_प्रकार *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क read_full_stream(खाता *f, वर्ण **ret_contents, माप_प्रकार *ret_size) {
        फर्काऊ read_full_stream_full(f, रिक्त, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_size);
}

पूर्णाङ्क verify_file_at(पूर्णाङ्क dir_fd, अचर वर्ण *fn, अचर वर्ण *blob, bool accept_extra_nl);

पूर्णाङ्क script_get_shebang_interpreter(अचर वर्ण *path, वर्ण **ret);

पूर्णाङ्क get_proc_field(अचर वर्ण *path, अचर वर्ण *साँचो, वर्ण **ret);

सूची* xopendirat(पूर्णाङ्क dir_fd, अचर वर्ण *path, पूर्णाङ्क flags);

प्रकारनिर्देश गणना XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

पूर्णाङ्क xfopenat_full(
                पूर्णाङ्क dir_fd,
                अचर वर्ण *path,
                अचर वर्ण *mode,
                पूर्णाङ्क open_flags,
                XfopenFlags flags,
                अचर वर्ण *bind_name,
                खाता **ret);
स्थिर अन्तःस्थ पूर्णाङ्क xfopenat(पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर वर्ण *mode, पूर्णाङ्क open_flags, खाता **ret) {
        फर्काऊ xfopenat_full(dir_fd, path, mode, open_flags, 0, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fopen_unlocked_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर वर्ण *mode, पूर्णाङ्क open_flags, खाता **ret) {
        फर्काऊ xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fopen_unlocked(अचर वर्ण *path, अचर वर्ण *mode, खाता **ret) {
        फर्काऊ fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

पूर्णाङ्क fdopen_independent(पूर्णाङ्क fd, अचर वर्ण *mode, खाता **ret);

पूर्णाङ्क खोज_and_open(अचर वर्ण *path, पूर्णाङ्क mode, अचर वर्ण *जरा, वर्ण **खोज, पूर्णाङ्क *ret_fd, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क खोज_and_access(अचर वर्ण *path, पूर्णाङ्क mode, अचर वर्ण *जरा, वर्ण**खोज, वर्ण **ret_path) {
        फर्काऊ खोज_and_open(path, mode, जरा, खोज, रिक्त, ret_path);
}
पूर्णाङ्क खोज_and_fopen(अचर वर्ण *path, अचर वर्ण *mode, अचर वर्ण *जरा, अचर वर्ण **खोज, खाता **ret_file, वर्ण **ret_path);
पूर्णाङ्क खोज_and_fopen_nulstr(अचर वर्ण *path, अचर वर्ण *mode, अचर वर्ण *जरा, अचर वर्ण *खोज, खाता **ret_file, वर्ण **ret_path);

पूर्णाङ्क fflush_and_check(खाता *f);
पूर्णाङ्क fflush_sync_and_check(खाता *f);

पूर्णाङ्क write_timestamp_file_atomic(अचर वर्ण *fn, usec_t n);
पूर्णाङ्क read_timestamp_file(अचर वर्ण *fn, usec_t *ret);

पूर्णाङ्क fputs_with_separator(खाता *f, अचर वर्ण *s, अचर वर्ण *separator, bool *space);
पूर्णाङ्क fputs_with_newline(खाता *f, अचर वर्ण *s);

प्रकारनिर्देश गणना Read_पङ्क्ति_Flags {
        READ_पङ्क्ति_ONLY_NUL  = 1 << 0,
        READ_पङ्क्ति_IS_A_TTY  = 1 << 1,
        READ_पङ्क्ति_NOT_A_TTY = 1 << 2,
} Read_पङ्क्ति_Flags;

पूर्णाङ्क read_पङ्क्ति_full(खाता *f, माप_प्रकार limit, Read_पङ्क्ति_Flags flags, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क read_पङ्क्ति(खाता *f, माप_प्रकार limit, वर्ण **ret) {
        फर्काऊ read_पङ्क्ति_full(f, limit, 0, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_nul_string(खाता *f, माप_प्रकार limit, वर्ण **ret) {
        फर्काऊ read_पङ्क्ति_full(f, limit, READ_पङ्क्ति_ONLY_NUL, ret);
}

पूर्णाङ्क read_stripped_पङ्क्ति(खाता *f, माप_प्रकार limit, वर्ण **ret);

स्थिर अन्तःस्थ bool file_offset_beyond_memory_size(off_t x) {
        यदि (x < 0) /* off_t is signed, filter that out */
                फर्काऊ false;
        फर्काऊ (uint64_t) x > (uint64_t) SIZE_MAX;
}

पूर्णाङ्क safe_fgetc(खाता *f, वर्ण *ret);

पूर्णाङ्क warn_file_is_world_accessible(अचर वर्ण *filename, संरचना stat *st, अचर वर्ण *unit, अचिन्हित पङ्क्ति);

पूर्णाङ्क fopen_mode_to_flags(अचर वर्ण *mode);

प्रकारनिर्देश गणना WriteDataFileFlags {
        WRITE_DATA_FILE_MKDIR_0755 = 1 << 0,
        WRITE_DATA_FILE_MODE_0400  = 1 << 1,
} WriteDataFileFlags;

पूर्णाङ्क write_data_file_atomic_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर संरचना iovec *iovec, WriteDataFileFlags flags);
