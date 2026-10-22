/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LONG_ਸਤਰ_MAX (1U*1024U*1024U)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_ਬਫਰ             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_ਬਹੁਲਕ_0600                  = 1 << 10,
        WRITE_STRING_FILE_ਬਹੁਲਕ_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
        READ_ਭਰਿਆ_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_ਭਰਿਆ_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_ਭਰਿਆ_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_ਭਰਿਆ_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_ਭਰਿਆ_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_ਭਰਿਆ_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_ਭਰਿਆ_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_ਭਰਿਆ_File_Flags;

ਪੂਰਨ fdopen_unlocked(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *options, FILE **ret);
ਪੂਰਨ take_fdopen_unlocked(ਪੂਰਨ *fd, ਅਚਲ ਅੱਖਰ *options, FILE **ret);
FILE* take_fdopen(ਪੂਰਨ *fd, ਅਚਲ ਅੱਖਰ *options);
DIR* take_fdopendir(ਪੂਰਨ *dfd);
FILE* open_memstream_unlocked(ਅੱਖਰ **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(ਖਾਲੀ *buf, size_t ਮਾਪ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ);

ਪੂਰਨ write_string_ਧਾਰਾ_ਭਰਿਆ(FILE *f, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags, ਅਚਲ ਬਣਤਰ timespec *ts);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_string_ਧਾਰਾ(FILE *f, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags) {
        ਵਾਪਸ write_string_ਧਾਰਾ_ਭਰਿਆ(f, ਸਤਰ, flags, ਨਿਰਾ);
}

ਪੂਰਨ write_string_file_ਭਰਿਆ_label(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags, ਅਚਲ ਬਣਤਰ timespec *ts, ਅਚਲ ਅੱਖਰ *label_fn, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_string_file_ਭਰਿਆ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags, ਅਚਲ ਬਣਤਰ timespec *ts, ਅਚਲ ਅੱਖਰ *label_fn) {
        ਵਾਪਸ write_string_file_ਭਰਿਆ_label(dir_fd, fn, ਸਤਰ, flags, ts, label_fn, /* label_context= */ ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_string_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags) {
        ਵਾਪਸ write_string_file_ਭਰਿਆ(dir_fd, fn, ਸਤਰ, flags, ਨਿਰਾ, ਨਿਰਾ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_string_file_fd(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags) {
        ਵਾਪਸ write_string_file_at(dir_fd, ਨਿਰਾ, ਸਤਰ, flags);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ write_string_file(ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *ਸਤਰ, WriteStringFileFlags flags) {
        ਵਾਪਸ write_string_file_at(AT_FDCWD, fn, ਸਤਰ, flags);
}
ਪੂਰਨ write_string_filef(ਅਚਲ ਅੱਖਰ *fn, WriteStringFileFlags flags, ਅਚਲ ਅੱਖਰ *format, ...) _printf_(3, 4);
ਪੂਰਨ write_string_filef_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, WriteStringFileFlags flags, ਅਚਲ ਅੱਖਰ *format, ...) _printf_(4, 5);

ਪੂਰਨ write_base64_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਬਣਤਰ iovec *ਅੰਕੜੇ, WriteStringFileFlags flags);

ਪੂਰਨ read_one_ਸਤਰ_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_one_ਸਤਰ_file(ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret) {
        ਵਾਪਸ read_one_ਸਤਰ_file_at(AT_FDCWD, filename, ret);
}
ਪੂਰਨ read_boolean_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_boolean_file(ਅਚਲ ਅੱਖਰ *filename) {
        ਵਾਪਸ read_boolean_file_at(AT_FDCWD, filename);
}
ਪੂਰਨ read_ਭਰਿਆ_file_ਭਰਿਆ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename, uint64_t ਵਿੱਥ, size_t ਮਾਪ, Read_ਭਰਿਆ_File_Flags flags, ਅਚਲ ਅੱਖਰ *bind_name, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_ਭਰਿਆ_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ read_ਭਰਿਆ_file_ਭਰਿਆ(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ਨਿਰਾ, ret_contents, ret_ਮਾਪ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_ਭਰਿਆ_file(ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ read_ਭਰਿਆ_file_ਭਰਿਆ(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ਨਿਰਾ, ret_contents, ret_ਮਾਪ);
}

ਪੂਰਨ read_virtual_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *filename, size_t max_ਮਾਪ, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_virtual_file_fd(ਪੂਰਨ fd, size_t max_ਮਾਪ, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ read_virtual_file_at(fd, ਨਿਰਾ, max_ਮਾਪ, ret_contents, ret_ਮਾਪ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_virtual_file(ਅਚਲ ਅੱਖਰ *filename, size_t max_ਮਾਪ, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ read_virtual_file_at(AT_FDCWD, filename, max_ਮਾਪ, ret_contents, ret_ਮਾਪ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_ਭਰਿਆ_virtual_file(ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ read_virtual_file(filename, SIZE_MAX, ret_contents, ret_ਮਾਪ);
}

ਪੂਰਨ read_ਭਰਿਆ_ਧਾਰਾ_ਭਰਿਆ(FILE *f, ਅਚਲ ਅੱਖਰ *filename, uint64_t ਵਿੱਥ, size_t ਮਾਪ, Read_ਭਰਿਆ_File_Flags flags, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_ਭਰਿਆ_ਧਾਰਾ(FILE *f, ਅੱਖਰ **ret_contents, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ read_ਭਰਿਆ_ਧਾਰਾ_ਭਰਿਆ(f, ਨਿਰਾ, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_ਮਾਪ);
}

ਪੂਰਨ verify_file_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ *blob, bool accept_extra_nl);

ਪੂਰਨ script_get_shebang_interpreter(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret);

ਪੂਰਨ get_proc_ਖੇਤਰ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅੱਖਰ **ret);

DIR* xopendirat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

ਪੂਰਨ xfopenat_ਭਰਿਆ(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ,
                ਪੂਰਨ open_flags,
                XfopenFlags flags,
                ਅਚਲ ਅੱਖਰ *bind_name,
                FILE **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xfopenat(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, ਪੂਰਨ open_flags, FILE **ret) {
        ਵਾਪਸ xfopenat_ਭਰਿਆ(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, open_flags, 0, ਨਿਰਾ, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fopen_unlocked_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, ਪੂਰਨ open_flags, FILE **ret) {
        ਵਾਪਸ xfopenat_ਭਰਿਆ(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, open_flags, XFOPEN_UNLOCKED, ਨਿਰਾ, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fopen_unlocked(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE **ret) {
        ਵਾਪਸ fopen_unlocked_at(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, 0, ret);
}

ਪੂਰਨ fdopen_independent(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, FILE **ret);

ਪੂਰਨ ਖੋਜ_and_open(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ ਬਹੁਲਕ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ਖੋਜ, ਪੂਰਨ *ret_fd, ਅੱਖਰ **ret_ਰਸਤਾ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਖੋਜ_and_access(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ ਬਹੁਲਕ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ**ਖੋਜ, ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਵਾਪਸ ਖੋਜ_and_open(ਰਸਤਾ, ਬਹੁਲਕ, ਜੜ੍ਹ, ਖੋਜ, ਨਿਰਾ, ret_ਰਸਤਾ);
}
ਪੂਰਨ ਖੋਜ_and_fopen(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ **ਖੋਜ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ);
ਪੂਰਨ ਖੋਜ_and_fopen_nulstr(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ *ਖੋਜ, FILE **ret_file, ਅੱਖਰ **ret_ਰਸਤਾ);

ਪੂਰਨ fflush_and_check(FILE *f);
ਪੂਰਨ fflush_sync_and_check(FILE *f);

ਪੂਰਨ write_timestamp_file_atomic(ਅਚਲ ਅੱਖਰ *fn, usec_t n);
ਪੂਰਨ read_timestamp_file(ਅਚਲ ਅੱਖਰ *fn, usec_t *ret);

ਪੂਰਨ fputs_with_separator(FILE *f, ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *separator, bool *space);
ਪੂਰਨ fputs_with_newline(FILE *f, ਅਚਲ ਅੱਖਰ *s);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Read_ਸਤਰ_Flags {
        READ_ਸਤਰ_ONLY_NUL  = 1 << 0,
        READ_ਸਤਰ_IS_A_TTY  = 1 << 1,
        READ_ਸਤਰ_NOT_A_TTY = 1 << 2,
} Read_ਸਤਰ_Flags;

ਪੂਰਨ read_ਸਤਰ_ਭਰਿਆ(FILE *f, size_t limit, Read_ਸਤਰ_Flags flags, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_ਸਤਰ(FILE *f, size_t limit, ਅੱਖਰ **ret) {
        ਵਾਪਸ read_ਸਤਰ_ਭਰਿਆ(f, limit, 0, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ read_nul_string(FILE *f, size_t limit, ਅੱਖਰ **ret) {
        ਵਾਪਸ read_ਸਤਰ_ਭਰਿਆ(f, limit, READ_ਸਤਰ_ONLY_NUL, ret);
}

ਪੂਰਨ read_stripped_ਸਤਰ(FILE *f, size_t limit, ਅੱਖਰ **ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool file_ਵਿੱਥ_beyond_ਸਮ੍ਰਿਤੀ_ਮਾਪ(off_t x) {
        ਜੇ (x < 0) /* off_t is signed, filter that out */
                ਵਾਪਸ false;
        ਵਾਪਸ (uint64_t) x > (uint64_t) SIZE_MAX;
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_fgetc(FILE *f, ਅੱਖਰ *ret);

ਪੂਰਨ warn_file_is_world_accessible(ਅਚਲ ਅੱਖਰ *filename, ਬਣਤਰ stat *st, ਅਚਲ ਅੱਖਰ *unit, ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ);

ਪੂਰਨ fopen_ਬਹੁਲਕ_ਮੰਜ਼ਿਲ_flags(ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Write_ਅੰਕੜੇ_File_Flags {
        WRITE_ਅੰਕੜੇ_FILE_MKDIR_0755 = 1 << 0,
        WRITE_ਅੰਕੜੇ_FILE_ਬਹੁਲਕ_0400  = 1 << 1,
} Write_ਅੰਕੜੇ_File_Flags;

ਪੂਰਨ write_ਅੰਕੜੇ_file_atomic_at(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਬਣਤਰ iovec *iovec, Write_ਅੰਕੜੇ_File_Flags flags);
