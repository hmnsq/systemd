/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा LONG_पंक्ति_MAX (1U*1024U*1024U)

प्रकार क्रमागत {
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
        WRITE_STRING_FILE_MODE_0600                  = 1 << 10,
        WRITE_STRING_FILE_MODE_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

प्रकार क्रमागत {
        READ_FULL_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_FULL_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_FULL_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_FULL_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_FULL_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_FULL_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_FULL_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} ReadFullFileFlags;

पूर्णांक fdopen_unlocked(पूर्णांक fd, स्थिर अक्षर *options, खाता **ret);
पूर्णांक take_fdopen_unlocked(पूर्णांक *fd, स्थिर अक्षर *options, खाता **ret);
खाता* take_fdopen(पूर्णांक *fd, स्थिर अक्षर *options);
सूची* take_fdopendir(पूर्णांक *dfd);
खाता* open_memstream_unlocked(अक्षर **ptr, माप_प्रकार *sizeloc);
खाता* fmemopen_unlocked(व्योम *buf, माप_प्रकार आकार, स्थिर अक्षर *mode);

पूर्णांक write_string_stream_full(खाता *f, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags, स्थिर काष्ठा timespec *ts);
अटल अंतरभूत पूर्णांक write_string_stream(खाता *f, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags) {
        वापस write_string_stream_full(f, पंक्ति, flags, शून्य);
}

पूर्णांक write_string_file_full_label(पूर्णांक dir_fd, स्थिर अक्षर *fn, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags, स्थिर काष्ठा timespec *ts, स्थिर अक्षर *label_fn, LabelContext *label_context);
अटल अंतरभूत पूर्णांक write_string_file_full(पूर्णांक dir_fd, स्थिर अक्षर *fn, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags, स्थिर काष्ठा timespec *ts, स्थिर अक्षर *label_fn) {
        वापस write_string_file_full_label(dir_fd, fn, पंक्ति, flags, ts, label_fn, /* label_context= */ शून्य);
}
अटल अंतरभूत पूर्णांक write_string_file_at(पूर्णांक dir_fd, स्थिर अक्षर *fn, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags) {
        वापस write_string_file_full(dir_fd, fn, पंक्ति, flags, शून्य, शून्य);
}
अटल अंतरभूत पूर्णांक write_string_file_fd(पूर्णांक dir_fd, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags) {
        वापस write_string_file_at(dir_fd, शून्य, पंक्ति, flags);
}
अटल अंतरभूत पूर्णांक write_string_file(स्थिर अक्षर *fn, स्थिर अक्षर *पंक्ति, WriteStringFileFlags flags) {
        वापस write_string_file_at(AT_FDCWD, fn, पंक्ति, flags);
}
पूर्णांक write_string_filef(स्थिर अक्षर *fn, WriteStringFileFlags flags, स्थिर अक्षर *format, ...) _printf_(3, 4);
पूर्णांक write_string_filef_at(पूर्णांक dir_fd, स्थिर अक्षर *fn, WriteStringFileFlags flags, स्थिर अक्षर *format, ...) _printf_(4, 5);

पूर्णांक write_base64_file_at(पूर्णांक dir_fd, स्थिर अक्षर *fn, स्थिर काष्ठा iovec *तथ्य, WriteStringFileFlags flags);

पूर्णांक read_one_पंक्ति_file_at(पूर्णांक dir_fd, स्थिर अक्षर *filename, अक्षर **ret);
अटल अंतरभूत पूर्णांक read_one_पंक्ति_file(स्थिर अक्षर *filename, अक्षर **ret) {
        वापस read_one_पंक्ति_file_at(AT_FDCWD, filename, ret);
}
पूर्णांक read_boolean_file_at(पूर्णांक dir_fd, स्थिर अक्षर *filename);
अटल अंतरभूत पूर्णांक read_boolean_file(स्थिर अक्षर *filename) {
        वापस read_boolean_file_at(AT_FDCWD, filename);
}
पूर्णांक read_full_file_full(पूर्णांक dir_fd, स्थिर अक्षर *filename, uint64_t offset, माप_प्रकार आकार, ReadFullFileFlags flags, स्थिर अक्षर *bind_name, अक्षर **ret_contents, माप_प्रकार *ret_आकार);
अटल अंतरभूत पूर्णांक read_full_file_at(पूर्णांक dir_fd, स्थिर अक्षर *filename, अक्षर **ret_contents, माप_प्रकार *ret_आकार) {
        वापस read_full_file_full(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, शून्य, ret_contents, ret_आकार);
}
अटल अंतरभूत पूर्णांक read_full_file(स्थिर अक्षर *filename, अक्षर **ret_contents, माप_प्रकार *ret_आकार) {
        वापस read_full_file_full(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, शून्य, ret_contents, ret_आकार);
}

पूर्णांक read_virtual_file_at(पूर्णांक dir_fd, स्थिर अक्षर *filename, माप_प्रकार max_आकार, अक्षर **ret_contents, माप_प्रकार *ret_आकार);
अटल अंतरभूत पूर्णांक read_virtual_file_fd(पूर्णांक fd, माप_प्रकार max_आकार, अक्षर **ret_contents, माप_प्रकार *ret_आकार) {
        वापस read_virtual_file_at(fd, शून्य, max_आकार, ret_contents, ret_आकार);
}
अटल अंतरभूत पूर्णांक read_virtual_file(स्थिर अक्षर *filename, माप_प्रकार max_आकार, अक्षर **ret_contents, माप_प्रकार *ret_आकार) {
        वापस read_virtual_file_at(AT_FDCWD, filename, max_आकार, ret_contents, ret_आकार);
}
अटल अंतरभूत पूर्णांक read_full_virtual_file(स्थिर अक्षर *filename, अक्षर **ret_contents, माप_प्रकार *ret_आकार) {
        वापस read_virtual_file(filename, SIZE_MAX, ret_contents, ret_आकार);
}

पूर्णांक read_full_stream_full(खाता *f, स्थिर अक्षर *filename, uint64_t offset, माप_प्रकार आकार, ReadFullFileFlags flags, अक्षर **ret_contents, माप_प्रकार *ret_आकार);
अटल अंतरभूत पूर्णांक read_full_stream(खाता *f, अक्षर **ret_contents, माप_प्रकार *ret_आकार) {
        वापस read_full_stream_full(f, शून्य, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_आकार);
}

पूर्णांक verify_file_at(पूर्णांक dir_fd, स्थिर अक्षर *fn, स्थिर अक्षर *blob, bool accept_extra_nl);

पूर्णांक script_get_shebang_interpreter(स्थिर अक्षर *path, अक्षर **ret);

पूर्णांक get_proc_field(स्थिर अक्षर *path, स्थिर अक्षर *कुंजी, अक्षर **ret);

सूची* xopendirat(पूर्णांक dir_fd, स्थिर अक्षर *path, पूर्णांक flags);

प्रकार क्रमागत XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

पूर्णांक xfopenat_full(
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                स्थिर अक्षर *mode,
                पूर्णांक open_flags,
                XfopenFlags flags,
                स्थिर अक्षर *bind_name,
                खाता **ret);
अटल अंतरभूत पूर्णांक xfopenat(पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *mode, पूर्णांक open_flags, खाता **ret) {
        वापस xfopenat_full(dir_fd, path, mode, open_flags, 0, शून्य, ret);
}
अटल अंतरभूत पूर्णांक fopen_unlocked_at(पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *mode, पूर्णांक open_flags, खाता **ret) {
        वापस xfopenat_full(dir_fd, path, mode, open_flags, XFOPEN_UNLOCKED, शून्य, ret);
}
अटल अंतरभूत पूर्णांक fopen_unlocked(स्थिर अक्षर *path, स्थिर अक्षर *mode, खाता **ret) {
        वापस fopen_unlocked_at(AT_FDCWD, path, mode, 0, ret);
}

पूर्णांक fdopen_independent(पूर्णांक fd, स्थिर अक्षर *mode, खाता **ret);

पूर्णांक खोज_and_open(स्थिर अक्षर *path, पूर्णांक mode, स्थिर अक्षर *मूल, अक्षर **खोज, पूर्णांक *ret_fd, अक्षर **ret_path);
अटल अंतरभूत पूर्णांक खोज_and_access(स्थिर अक्षर *path, पूर्णांक mode, स्थिर अक्षर *मूल, अक्षर**खोज, अक्षर **ret_path) {
        वापस खोज_and_open(path, mode, मूल, खोज, शून्य, ret_path);
}
पूर्णांक खोज_and_fopen(स्थिर अक्षर *path, स्थिर अक्षर *mode, स्थिर अक्षर *मूल, स्थिर अक्षर **खोज, खाता **ret_file, अक्षर **ret_path);
पूर्णांक खोज_and_fopen_nulstr(स्थिर अक्षर *path, स्थिर अक्षर *mode, स्थिर अक्षर *मूल, स्थिर अक्षर *खोज, खाता **ret_file, अक्षर **ret_path);

पूर्णांक fflush_and_check(खाता *f);
पूर्णांक fflush_sync_and_check(खाता *f);

पूर्णांक write_timestamp_file_atomic(स्थिर अक्षर *fn, usec_t n);
पूर्णांक read_timestamp_file(स्थिर अक्षर *fn, usec_t *ret);

पूर्णांक fputs_with_separator(खाता *f, स्थिर अक्षर *s, स्थिर अक्षर *separator, bool *space);
पूर्णांक fputs_with_newline(खाता *f, स्थिर अक्षर *s);

प्रकार क्रमागत Read_पंक्ति_Flags {
        READ_पंक्ति_ONLY_NUL  = 1 << 0,
        READ_पंक्ति_IS_A_TTY  = 1 << 1,
        READ_पंक्ति_NOT_A_TTY = 1 << 2,
} Read_पंक्ति_Flags;

पूर्णांक read_पंक्ति_full(खाता *f, माप_प्रकार limit, Read_पंक्ति_Flags flags, अक्षर **ret);
अटल अंतरभूत पूर्णांक read_पंक्ति(खाता *f, माप_प्रकार limit, अक्षर **ret) {
        वापस read_पंक्ति_full(f, limit, 0, ret);
}
अटल अंतरभूत पूर्णांक read_nul_string(खाता *f, माप_प्रकार limit, अक्षर **ret) {
        वापस read_पंक्ति_full(f, limit, READ_पंक्ति_ONLY_NUL, ret);
}

पूर्णांक read_stripped_पंक्ति(खाता *f, माप_प्रकार limit, अक्षर **ret);

अटल अंतरभूत bool file_offset_beyond_memory_आकार(off_t x) {
        अगर (x < 0) /* off_t is signed, filter that out */
                वापस false;
        वापस (uint64_t) x > (uint64_t) SIZE_MAX;
}

पूर्णांक safe_fgetc(खाता *f, अक्षर *ret);

पूर्णांक warn_file_is_world_accessible(स्थिर अक्षर *filename, काष्ठा stat *st, स्थिर अक्षर *unit, अचिन्हित पंक्ति);

पूर्णांक fopen_mode_to_flags(स्थिर अक्षर *mode);

प्रकार क्रमागत Write_तथ्य_File_Flags {
        WRITE_तथ्य_FILE_MKDIR_0755 = 1 << 0,
        WRITE_तथ्य_FILE_MODE_0400  = 1 << 1,
} Write_तथ्य_File_Flags;

पूर्णांक write_तथ्य_file_atomic_at(पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर काष्ठा iovec *iovec, Write_तथ्य_File_Flags flags);
