/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#परिभाषय LONG_पङ्क्तिः_MAX (1U*1024U*1024U)

प्रकारनिर्देश गणना {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_बफर             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_बहुलकम्_0600                  = 1 << 10,
        WRITE_STRING_FILE_बहुलकम्_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

प्रकारनिर्देश गणना {
        READ_पूर्णम्_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_पूर्णम्_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_पूर्णम्_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_पूर्णम्_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_पूर्णम्_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_पूर्णम्_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_पूर्णम्_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_पूर्णम्_File_Flags;

पूर्णाङ्क fdopen_unlocked(पूर्णाङ्क fd, ध्रुव वर्ण *options, FILE **ret);
पूर्णाङ्क take_fdopen_unlocked(पूर्णाङ्क *fd, ध्रुव वर्ण *options, FILE **ret);
FILE* take_fdopen(पूर्णाङ्क *fd, ध्रुव वर्ण *options);
DIR* take_fdopendir(पूर्णाङ्क *dfd);
FILE* open_memstream_unlocked(वर्ण **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(शून्य *buf, size_t आकार, ध्रुव वर्ण *बहुलकम्);

पूर्णाङ्क write_string_प्रवाहः_पूर्णम्(FILE *f, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags, ध्रुव संरचना timespec *ts);
स्थिर अन्तःस्थ पूर्णाङ्क write_string_प्रवाहः(FILE *f, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags) {
        प्रत्यागम write_string_प्रवाहः_पूर्णम्(f, पङ्क्तिः, flags, रिक्त);
}

पूर्णाङ्क write_string_file_पूर्णम्_label(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags, ध्रुव संरचना timespec *ts, ध्रुव वर्ण *label_fn, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_पूर्णम्(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags, ध्रुव संरचना timespec *ts, ध्रुव वर्ण *label_fn) {
        प्रत्यागम write_string_file_पूर्णम्_label(dir_fd, fn, पङ्क्तिः, flags, ts, label_fn, /* label_context= */ रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags) {
        प्रत्यागम write_string_file_पूर्णम्(dir_fd, fn, पङ्क्तिः, flags, रिक्त, रिक्त);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file_fd(पूर्णाङ्क dir_fd, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags) {
        प्रत्यागम write_string_file_at(dir_fd, रिक्त, पङ्क्तिः, flags);
}
स्थिर अन्तःस्थ पूर्णाङ्क write_string_file(ध्रुव वर्ण *fn, ध्रुव वर्ण *पङ्क्तिः, WriteStringFileFlags flags) {
        प्रत्यागम write_string_file_at(AT_FDCWD, fn, पङ्क्तिः, flags);
}
पूर्णाङ्क write_string_filef(ध्रुव वर्ण *fn, WriteStringFileFlags flags, ध्रुव वर्ण *format, ...) _printf_(3, 4);
पूर्णाङ्क write_string_filef_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, WriteStringFileFlags flags, ध्रुव वर्ण *format, ...) _printf_(4, 5);

पूर्णाङ्क write_base64_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव संरचना iovec *दत्त, WriteStringFileFlags flags);

पूर्णाङ्क read_one_पङ्क्तिः_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क read_one_पङ्क्तिः_file(ध्रुव वर्ण *filename, वर्ण **ret) {
        प्रत्यागम read_one_पङ्क्तिः_file_at(AT_FDCWD, filename, ret);
}
पूर्णाङ्क read_boolean_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename);
स्थिर अन्तःस्थ पूर्णाङ्क read_boolean_file(ध्रुव वर्ण *filename) {
        प्रत्यागम read_boolean_file_at(AT_FDCWD, filename);
}
पूर्णाङ्क read_पूर्णम्_file_पूर्णम्(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, uint64_t विस्थापनम्, size_t आकार, Read_पूर्णम्_File_Flags flags, ध्रुव वर्ण *bind_name, वर्ण **ret_contents, size_t *ret_आकार);
स्थिर अन्तःस्थ पूर्णाङ्क read_पूर्णम्_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, वर्ण **ret_contents, size_t *ret_आकार) {
        प्रत्यागम read_पूर्णम्_file_पूर्णम्(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_आकार);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_पूर्णम्_file(ध्रुव वर्ण *filename, वर्ण **ret_contents, size_t *ret_आकार) {
        प्रत्यागम read_पूर्णम्_file_पूर्णम्(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, रिक्त, ret_contents, ret_आकार);
}

पूर्णाङ्क read_virtual_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *filename, size_t max_आकार, वर्ण **ret_contents, size_t *ret_आकार);
स्थिर अन्तःस्थ पूर्णाङ्क read_virtual_file_fd(पूर्णाङ्क fd, size_t max_आकार, वर्ण **ret_contents, size_t *ret_आकार) {
        प्रत्यागम read_virtual_file_at(fd, रिक्त, max_आकार, ret_contents, ret_आकार);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_virtual_file(ध्रुव वर्ण *filename, size_t max_आकार, वर्ण **ret_contents, size_t *ret_आकार) {
        प्रत्यागम read_virtual_file_at(AT_FDCWD, filename, max_आकार, ret_contents, ret_आकार);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_पूर्णम्_virtual_file(ध्रुव वर्ण *filename, वर्ण **ret_contents, size_t *ret_आकार) {
        प्रत्यागम read_virtual_file(filename, SIZE_MAX, ret_contents, ret_आकार);
}

पूर्णाङ्क read_पूर्णम्_प्रवाहः_पूर्णम्(FILE *f, ध्रुव वर्ण *filename, uint64_t विस्थापनम्, size_t आकार, Read_पूर्णम्_File_Flags flags, वर्ण **ret_contents, size_t *ret_आकार);
स्थिर अन्तःस्थ पूर्णाङ्क read_पूर्णम्_प्रवाहः(FILE *f, वर्ण **ret_contents, size_t *ret_आकार) {
        प्रत्यागम read_पूर्णम्_प्रवाहः_पूर्णम्(f, रिक्त, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_आकार);
}

पूर्णाङ्क verify_file_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *fn, ध्रुव वर्ण *blob, bool accept_extra_nl);

पूर्णाङ्क script_get_shebang_interpreter(ध्रुव वर्ण *मार्गः, वर्ण **ret);

पूर्णाङ्क get_proc_क्षेत्रम्(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *कुञ्चिका, वर्ण **ret);

DIR* xopendirat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, पूर्णाङ्क flags);

प्रकारनिर्देश गणना XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

पूर्णाङ्क xfopenat_पूर्णम्(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *मार्गः,
                ध्रुव वर्ण *बहुलकम्,
                पूर्णाङ्क open_flags,
                XfopenFlags flags,
                ध्रुव वर्ण *bind_name,
                FILE **ret);
स्थिर अन्तःस्थ पूर्णाङ्क xfopenat(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *बहुलकम्, पूर्णाङ्क open_flags, FILE **ret) {
        प्रत्यागम xfopenat_पूर्णम्(dir_fd, मार्गः, बहुलकम्, open_flags, 0, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fopen_unlocked_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *बहुलकम्, पूर्णाङ्क open_flags, FILE **ret) {
        प्रत्यागम xfopenat_पूर्णम्(dir_fd, मार्गः, बहुलकम्, open_flags, XFOPEN_UNLOCKED, रिक्त, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क fopen_unlocked(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *बहुलकम्, FILE **ret) {
        प्रत्यागम fopen_unlocked_at(AT_FDCWD, मार्गः, बहुलकम्, 0, ret);
}

पूर्णाङ्क fdopen_independent(पूर्णाङ्क fd, ध्रुव वर्ण *बहुलकम्, FILE **ret);

पूर्णाङ्क अन्वेषणम्_and_open(ध्रुव वर्ण *मार्गः, पूर्णाङ्क बहुलकम्, ध्रुव वर्ण *मूलम्, वर्ण **अन्वेषणम्, पूर्णाङ्क *ret_fd, वर्ण **ret_मार्गः);
स्थिर अन्तःस्थ पूर्णाङ्क अन्वेषणम्_and_access(ध्रुव वर्ण *मार्गः, पूर्णाङ्क बहुलकम्, ध्रुव वर्ण *मूलम्, वर्ण**अन्वेषणम्, वर्ण **ret_मार्गः) {
        प्रत्यागम अन्वेषणम्_and_open(मार्गः, बहुलकम्, मूलम्, अन्वेषणम्, रिक्त, ret_मार्गः);
}
पूर्णाङ्क अन्वेषणम्_and_fopen(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *बहुलकम्, ध्रुव वर्ण *मूलम्, ध्रुव वर्ण **अन्वेषणम्, FILE **ret_file, वर्ण **ret_मार्गः);
पूर्णाङ्क अन्वेषणम्_and_fopen_nulstr(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *बहुलकम्, ध्रुव वर्ण *मूलम्, ध्रुव वर्ण *अन्वेषणम्, FILE **ret_file, वर्ण **ret_मार्गः);

पूर्णाङ्क fflush_and_check(FILE *f);
पूर्णाङ्क fflush_sync_and_check(FILE *f);

पूर्णाङ्क write_timestamp_file_atomic(ध्रुव वर्ण *fn, usec_t n);
पूर्णाङ्क read_timestamp_file(ध्रुव वर्ण *fn, usec_t *ret);

पूर्णाङ्क fputs_with_separator(FILE *f, ध्रुव वर्ण *s, ध्रुव वर्ण *separator, bool *space);
पूर्णाङ्क fputs_with_newline(FILE *f, ध्रुव वर्ण *s);

प्रकारनिर्देश गणना Read_पङ्क्तिः_Flags {
        READ_पङ्क्तिः_ONLY_NUL  = 1 << 0,
        READ_पङ्क्तिः_IS_A_TTY  = 1 << 1,
        READ_पङ्क्तिः_NOT_A_TTY = 1 << 2,
} Read_पङ्क्तिः_Flags;

पूर्णाङ्क read_पङ्क्तिः_पूर्णम्(FILE *f, size_t limit, Read_पङ्क्तिः_Flags flags, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क read_पङ्क्तिः(FILE *f, size_t limit, वर्ण **ret) {
        प्रत्यागम read_पङ्क्तिः_पूर्णम्(f, limit, 0, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क read_nul_string(FILE *f, size_t limit, वर्ण **ret) {
        प्रत्यागम read_पङ्क्तिः_पूर्णम्(f, limit, READ_पङ्क्तिः_ONLY_NUL, ret);
}

पूर्णाङ्क read_stripped_पङ्क्तिः(FILE *f, size_t limit, वर्ण **ret);

स्थिर अन्तःस्थ bool file_विस्थापनम्_beyond_स्मृतिः_आकार(off_t x) {
        यदि (x < 0) /* off_t is signed, filter that out */
                प्रत्यागम false;
        प्रत्यागम (uint64_t) x > (uint64_t) SIZE_MAX;
}

पूर्णाङ्क safe_fgetc(FILE *f, वर्ण *ret);

पूर्णाङ्क warn_file_is_world_accessible(ध्रुव वर्ण *filename, संरचना stat *st, ध्रुव वर्ण *unit, अचिह्नित पङ्क्तिः);

पूर्णाङ्क fopen_बहुलकम्_गन्तव्यम्_flags(ध्रुव वर्ण *बहुलकम्);

प्रकारनिर्देश गणना Write_दत्त_File_Flags {
        WRITE_दत्त_FILE_MKDIR_0755 = 1 << 0,
        WRITE_दत्त_FILE_बहुलकम्_0400  = 1 << 1,
} Write_दत्त_File_Flags;

पूर्णाङ्क write_दत्त_file_atomic_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव संरचना iovec *iovec, Write_दत्त_File_Flags flags);
