/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

#تعریف_کړه LONG_کرښه_MAX (1U*1024U*1024U)

تعريفنوع تعداد {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_بفر             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_ډېرپېښ_0600                  = 1 << 10,
        WRITE_STRING_FILE_ډېرپېښ_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

تعريفنوع تعداد {
        READ_ډک_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_ډک_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_ډک_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_ډک_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_ډک_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_ډک_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_ډک_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_ډک_File_Flags;

صحيح fdopen_unlocked(صحيح fd, ثابت حرف *options, FILE **ret);
صحيح take_fdopen_unlocked(صحيح *fd, ثابت حرف *options, FILE **ret);
FILE* take_fdopen(صحيح *fd, ثابت حرف *options);
DIR* take_fdopendir(صحيح *dfd);
FILE* open_memstream_unlocked(حرف **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(فراغ *buf, size_t اندازه, ثابت حرف *ډېرپېښ);

صحيح write_string_بهیر_ډک(FILE *f, ثابت حرف *کرښه, WriteStringFileFlags flags, ثابت بنية timespec *ts);
ساكن مضمن صحيح write_string_بهیر(FILE *f, ثابت حرف *کرښه, WriteStringFileFlags flags) {
        أرجع write_string_بهیر_ډک(f, کرښه, flags, عدم);
}

صحيح write_string_file_ډک_label(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *کرښه, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn, LabelContext *label_context);
ساكن مضمن صحيح write_string_file_ډک(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *کرښه, WriteStringFileFlags flags, ثابت بنية timespec *ts, ثابت حرف *label_fn) {
        أرجع write_string_file_ډک_label(dir_fd, fn, کرښه, flags, ts, label_fn, /* label_context= */ عدم);
}
ساكن مضمن صحيح write_string_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *کرښه, WriteStringFileFlags flags) {
        أرجع write_string_file_ډک(dir_fd, fn, کرښه, flags, عدم, عدم);
}
ساكن مضمن صحيح write_string_file_fd(صحيح dir_fd, ثابت حرف *کرښه, WriteStringFileFlags flags) {
        أرجع write_string_file_at(dir_fd, عدم, کرښه, flags);
}
ساكن مضمن صحيح write_string_file(ثابت حرف *fn, ثابت حرف *کرښه, WriteStringFileFlags flags) {
        أرجع write_string_file_at(AT_FDCWD, fn, کرښه, flags);
}
صحيح write_string_filef(ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(3, 4);
صحيح write_string_filef_at(صحيح dir_fd, ثابت حرف *fn, WriteStringFileFlags flags, ثابت حرف *format, ...) _printf_(4, 5);

صحيح write_base64_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت بنية iovec *معلومات, WriteStringFileFlags flags);

صحيح read_one_کرښه_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret);
ساكن مضمن صحيح read_one_کرښه_file(ثابت حرف *filename, حرف **ret) {
        أرجع read_one_کرښه_file_at(AT_FDCWD, filename, ret);
}
صحيح read_boolean_file_at(صحيح dir_fd, ثابت حرف *filename);
ساكن مضمن صحيح read_boolean_file(ثابت حرف *filename) {
        أرجع read_boolean_file_at(AT_FDCWD, filename);
}
صحيح read_ډک_file_ډک(صحيح dir_fd, ثابت حرف *filename, uint64_t دننه_واټن, size_t اندازه, Read_ډک_File_Flags flags, ثابت حرف *bind_name, حرف **ret_contents, size_t *ret_اندازه);
ساكن مضمن صحيح read_ډک_file_at(صحيح dir_fd, ثابت حرف *filename, حرف **ret_contents, size_t *ret_اندازه) {
        أرجع read_ډک_file_ډک(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_اندازه);
}
ساكن مضمن صحيح read_ډک_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_اندازه) {
        أرجع read_ډک_file_ډک(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, عدم, ret_contents, ret_اندازه);
}

صحيح read_virtual_file_at(صحيح dir_fd, ثابت حرف *filename, size_t max_اندازه, حرف **ret_contents, size_t *ret_اندازه);
ساكن مضمن صحيح read_virtual_file_fd(صحيح fd, size_t max_اندازه, حرف **ret_contents, size_t *ret_اندازه) {
        أرجع read_virtual_file_at(fd, عدم, max_اندازه, ret_contents, ret_اندازه);
}
ساكن مضمن صحيح read_virtual_file(ثابت حرف *filename, size_t max_اندازه, حرف **ret_contents, size_t *ret_اندازه) {
        أرجع read_virtual_file_at(AT_FDCWD, filename, max_اندازه, ret_contents, ret_اندازه);
}
ساكن مضمن صحيح read_ډک_virtual_file(ثابت حرف *filename, حرف **ret_contents, size_t *ret_اندازه) {
        أرجع read_virtual_file(filename, SIZE_MAX, ret_contents, ret_اندازه);
}

صحيح read_ډک_بهیر_ډک(FILE *f, ثابت حرف *filename, uint64_t دننه_واټن, size_t اندازه, Read_ډک_File_Flags flags, حرف **ret_contents, size_t *ret_اندازه);
ساكن مضمن صحيح read_ډک_بهیر(FILE *f, حرف **ret_contents, size_t *ret_اندازه) {
        أرجع read_ډک_بهیر_ډک(f, عدم, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_اندازه);
}

صحيح verify_file_at(صحيح dir_fd, ثابت حرف *fn, ثابت حرف *blob, bool accept_extra_nl);

صحيح script_get_shebang_interpreter(ثابت حرف *لار, حرف **ret);

صحيح get_proc_ډګر(ثابت حرف *لار, ثابت حرف *کلی, حرف **ret);

DIR* xopendirat(صحيح dir_fd, ثابت حرف *لار, صحيح flags);

تعريفنوع تعداد XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

صحيح xfopenat_ډک(
                صحيح dir_fd,
                ثابت حرف *لار,
                ثابت حرف *ډېرپېښ,
                صحيح open_flags,
                XfopenFlags flags,
                ثابت حرف *bind_name,
                FILE **ret);
ساكن مضمن صحيح xfopenat(صحيح dir_fd, ثابت حرف *لار, ثابت حرف *ډېرپېښ, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_ډک(dir_fd, لار, ډېرپېښ, open_flags, 0, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked_at(صحيح dir_fd, ثابت حرف *لار, ثابت حرف *ډېرپېښ, صحيح open_flags, FILE **ret) {
        أرجع xfopenat_ډک(dir_fd, لار, ډېرپېښ, open_flags, XFOPEN_UNLOCKED, عدم, ret);
}
ساكن مضمن صحيح fopen_unlocked(ثابت حرف *لار, ثابت حرف *ډېرپېښ, FILE **ret) {
        أرجع fopen_unlocked_at(AT_FDCWD, لار, ډېرپېښ, 0, ret);
}

صحيح fdopen_independent(صحيح fd, ثابت حرف *ډېرپېښ, FILE **ret);

صحيح پلټنه_and_open(ثابت حرف *لار, صحيح ډېرپېښ, ثابت حرف *ریښه, حرف **پلټنه, صحيح *ret_fd, حرف **ret_لار);
ساكن مضمن صحيح پلټنه_and_access(ثابت حرف *لار, صحيح ډېرپېښ, ثابت حرف *ریښه, حرف**پلټنه, حرف **ret_لار) {
        أرجع پلټنه_and_open(لار, ډېرپېښ, ریښه, پلټنه, عدم, ret_لار);
}
صحيح پلټنه_and_fopen(ثابت حرف *لار, ثابت حرف *ډېرپېښ, ثابت حرف *ریښه, ثابت حرف **پلټنه, FILE **ret_file, حرف **ret_لار);
صحيح پلټنه_and_fopen_nulstr(ثابت حرف *لار, ثابت حرف *ډېرپېښ, ثابت حرف *ریښه, ثابت حرف *پلټنه, FILE **ret_file, حرف **ret_لار);

صحيح fflush_and_check(FILE *f);
صحيح fflush_sync_and_check(FILE *f);

صحيح write_timestamp_file_atomic(ثابت حرف *fn, usec_t n);
صحيح read_timestamp_file(ثابت حرف *fn, usec_t *ret);

صحيح fputs_with_separator(FILE *f, ثابت حرف *s, ثابت حرف *separator, bool *space);
صحيح fputs_with_newline(FILE *f, ثابت حرف *s);

تعريفنوع تعداد Read_کرښه_Flags {
        READ_کرښه_ONLY_NUL  = 1 << 0,
        READ_کرښه_IS_A_TTY  = 1 << 1,
        READ_کرښه_NOT_A_TTY = 1 << 2,
} Read_کرښه_Flags;

صحيح read_کرښه_ډک(FILE *f, size_t limit, Read_کرښه_Flags flags, حرف **ret);
ساكن مضمن صحيح read_کرښه(FILE *f, size_t limit, حرف **ret) {
        أرجع read_کرښه_ډک(f, limit, 0, ret);
}
ساكن مضمن صحيح read_nul_string(FILE *f, size_t limit, حرف **ret) {
        أرجع read_کرښه_ډک(f, limit, READ_کرښه_ONLY_NUL, ret);
}

صحيح read_stripped_کرښه(FILE *f, size_t limit, حرف **ret);

ساكن مضمن bool file_دننه_واټن_beyond_حافظه_اندازه(off_t x) {
        إذا (x < 0) /* off_t is signed, filter that out */
                أرجع false;
        أرجع (uint64_t) x > (uint64_t) SIZE_MAX;
}

صحيح خوندي_fgetc(FILE *f, حرف *ret);

صحيح warn_file_is_world_accessible(ثابت حرف *filename, بنية stat *st, ثابت حرف *unit, بینښه کرښه);

صحيح fopen_ډېرپېښ_منزل_flags(ثابت حرف *ډېرپېښ);

تعريفنوع تعداد Write_معلومات_File_Flags {
        WRITE_معلومات_FILE_MKDIR_0755 = 1 << 0,
        WRITE_معلومات_FILE_ډېرپېښ_0400  = 1 << 1,
} Write_معلومات_File_Flags;

صحيح write_معلومات_file_atomic_at(صحيح dir_fd, ثابت حرف *لار, ثابت بنية iovec *iovec, Write_معلومات_File_Flags flags);
