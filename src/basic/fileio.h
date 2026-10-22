/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು LONG_ಸಾಲು_MAX (1U*1024U*1024U)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_ಬಫರ್             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_ಬಹುಲಕ_0600                  = 1 << 10,
        WRITE_STRING_FILE_ಬಹುಲಕ_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
        READ_ತುಂಬಿದ_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_ತುಂಬಿದ_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_ತುಂಬಿದ_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_ತುಂಬಿದ_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_ತುಂಬಿದ_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_ತುಂಬಿದ_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_ತುಂಬಿದ_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_ತುಂಬಿದ_File_Flags;

ಪೂರ್ಣಾಂಕ fdopen_unlocked(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options, FILE **ret);
ಪೂರ್ಣಾಂಕ take_fdopen_unlocked(ಪೂರ್ಣಾಂಕ *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options, FILE **ret);
FILE* take_fdopen(ಪೂರ್ಣಾಂಕ *fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *options);
DIR* take_fdopendir(ಪೂರ್ಣಾಂಕ *dfd);
FILE* open_memstream_unlocked(ಅಕ್ಷರ **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(ಶೂನ್ಯ *buf, size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ);

ಪೂರ್ಣಾಂಕ write_string_ಹರಿವು_ತುಂಬಿದ(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_string_ಹರಿವು(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags) {
        ಹಿಂತಿರುಗಿ write_string_ಹರಿವು_ತುಂಬಿದ(f, ಸಾಲು, flags, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ write_string_file_ತುಂಬಿದ_label(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label_fn, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_string_file_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags, ಸ್ಥಿರಾಂಕ ರಚನೆ timespec *ts, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label_fn) {
        ಹಿಂತಿರುಗಿ write_string_file_ತುಂಬಿದ_label(dir_fd, fn, ಸಾಲು, flags, ts, label_fn, /* label_context= */ ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_string_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags) {
        ಹಿಂತಿರುಗಿ write_string_file_ತುಂಬಿದ(dir_fd, fn, ಸಾಲು, flags, ಖಾಲಿ, ಖಾಲಿ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_string_file_fd(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags) {
        ಹಿಂತಿರುಗಿ write_string_file_at(dir_fd, ಖಾಲಿ, ಸಾಲು, flags);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ write_string_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು, WriteStringFileFlags flags) {
        ಹಿಂತಿರುಗಿ write_string_file_at(AT_FDCWD, fn, ಸಾಲು, flags);
}
ಪೂರ್ಣಾಂಕ write_string_filef(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, WriteStringFileFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(3, 4);
ಪೂರ್ಣಾಂಕ write_string_filef_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, WriteStringFileFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(4, 5);

ಪೂರ್ಣಾಂಕ write_base64_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ದತ್ತಾಂಶ, WriteStringFileFlags flags);

ಪೂರ್ಣಾಂಕ read_one_ಸಾಲು_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_one_ಸಾಲು_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ read_one_ಸಾಲು_file_at(AT_FDCWD, filename, ret);
}
ಪೂರ್ಣಾಂಕ read_boolean_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_boolean_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename) {
        ಹಿಂತಿರುಗಿ read_boolean_file_at(AT_FDCWD, filename);
}
ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_file_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, uint64_t ಸ್ಥಾನಾಂತರ, size_t ಗಾತ್ರ_2, Read_ತುಂಬಿದ_File_Flags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bind_name, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ read_ತುಂಬಿದ_file_ತುಂಬಿದ(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ಖಾಲಿ, ret_contents, ret_ಗಾತ್ರ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ read_ತುಂಬಿದ_file_ತುಂಬಿದ(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ಖಾಲಿ, ret_contents, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ read_virtual_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, size_t max_ಗಾತ್ರ, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_virtual_file_fd(ಪೂರ್ಣಾಂಕ fd, size_t max_ಗಾತ್ರ, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ read_virtual_file_at(fd, ಖಾಲಿ, max_ಗಾತ್ರ, ret_contents, ret_ಗಾತ್ರ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_virtual_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, size_t max_ಗಾತ್ರ, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ read_virtual_file_at(AT_FDCWD, filename, max_ಗಾತ್ರ, ret_contents, ret_ಗಾತ್ರ);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_virtual_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ read_virtual_file(filename, SIZE_MAX, ret_contents, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_ಹರಿವು_ತುಂಬಿದ(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, uint64_t ಸ್ಥಾನಾಂತರ, size_t ಗಾತ್ರ_2, Read_ತುಂಬಿದ_File_Flags flags, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_ತುಂಬಿದ_ಹರಿವು(FILE *f, ಅಕ್ಷರ **ret_contents, size_t *ret_ಗಾತ್ರ) {
        ಹಿಂತಿರುಗಿ read_ತುಂಬಿದ_ಹರಿವು_ತುಂಬಿದ(f, ಖಾಲಿ, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_ಗಾತ್ರ);
}

ಪೂರ್ಣಾಂಕ verify_file_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *blob, bool accept_extra_nl);

ಪೂರ್ಣಾಂಕ script_get_shebang_interpreter(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ get_proc_ಕ್ಷೇತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಅಕ್ಷರ **ret);

DIR* xopendirat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

ಪೂರ್ಣಾಂಕ xfopenat_ತುಂಬಿದ(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ,
                ಪೂರ್ಣಾಂಕ open_flags,
                XfopenFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *bind_name,
                FILE **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ xfopenat(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ open_flags, FILE **ret) {
        ಹಿಂತಿರುಗಿ xfopenat_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, open_flags, 0, ಖಾಲಿ, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fopen_unlocked_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, ಪೂರ್ಣಾಂಕ open_flags, FILE **ret) {
        ಹಿಂತಿರುಗಿ xfopenat_ತುಂಬಿದ(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, open_flags, XFOPEN_UNLOCKED, ಖಾಲಿ, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ fopen_unlocked(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE **ret) {
        ಹಿಂತಿರುಗಿ fopen_unlocked_at(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, 0, ret);
}

ಪೂರ್ಣಾಂಕ fdopen_independent(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, FILE **ret);

ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ **ಹುಡುಕು, ಪೂರ್ಣಾಂಕ *ret_fd, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_access(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಅಕ್ಷರ**ಹುಡುಕು, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಹಿಂತಿರುಗಿ ಹುಡುಕು_and_open(ಮಾರ್ಗ, ಬಹುಲಕ, ಬೇರು, ಹುಡುಕು, ಖಾಲಿ, ret_ಮಾರ್ಗ);
}
ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_fopen(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಹುಡುಕು, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);
ಪೂರ್ಣಾಂಕ ಹುಡುಕು_and_fopen_nulstr(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಹುಡುಕು, FILE **ret_file, ಅಕ್ಷರ **ret_ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ fflush_and_check(FILE *f);
ಪೂರ್ಣಾಂಕ fflush_sync_and_check(FILE *f);

ಪೂರ್ಣಾಂಕ write_timestamp_file_atomic(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, usec_t n);
ಪೂರ್ಣಾಂಕ read_timestamp_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, usec_t *ret);

ಪೂರ್ಣಾಂಕ fputs_with_separator(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, bool *space);
ಪೂರ್ಣಾಂಕ fputs_with_newline(FILE *f, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Read_ಸಾಲು_Flags {
        READ_ಸಾಲು_ONLY_NUL  = 1 << 0,
        READ_ಸಾಲು_IS_A_TTY  = 1 << 1,
        READ_ಸಾಲು_NOT_A_TTY = 1 << 2,
} Read_ಸಾಲು_Flags;

ಪೂರ್ಣಾಂಕ read_ಸಾಲು_ತುಂಬಿದ(FILE *f, size_t limit, Read_ಸಾಲು_Flags flags, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_ಸಾಲು(FILE *f, size_t limit, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ read_ಸಾಲು_ತುಂಬಿದ(f, limit, 0, ret);
}
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ read_nul_string(FILE *f, size_t limit, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ read_ಸಾಲು_ತುಂಬಿದ(f, limit, READ_ಸಾಲು_ONLY_NUL, ret);
}

ಪೂರ್ಣಾಂಕ read_stripped_ಸಾಲು(FILE *f, size_t limit, ಅಕ್ಷರ **ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool file_ಸ್ಥಾನಾಂತರ_beyond_ಸ್ಮರಣೆ_ಗಾತ್ರ(off_t x) {
        ಆದರೆ (x < 0) /* off_t is signed, filter that out */
                ಹಿಂತಿರುಗಿ false;
        ಹಿಂತಿರುಗಿ (uint64_t) x > (uint64_t) SIZE_MAX;
}

ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_fgetc(FILE *f, ಅಕ್ಷರ *ret);

ಪೂರ್ಣಾಂಕ warn_file_is_world_accessible(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ರಚನೆ stat *st, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit, ಚಿಹ್ನೆರಹಿತ ಸಾಲು);

ಪೂರ್ಣಾಂಕ fopen_ಬಹುಲಕ_ಗಮ್ಯ_flags(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Write_ದತ್ತಾಂಶ_File_Flags {
        WRITE_ದತ್ತಾಂಶ_FILE_MKDIR_0755 = 1 << 0,
        WRITE_ದತ್ತಾಂಶ_FILE_ಬಹುಲಕ_0400  = 1 << 1,
} Write_ದತ್ತಾಂಶ_File_Flags;

ಪೂರ್ಣಾಂಕ write_ದತ್ತಾಂಶ_file_atomic_at(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *iovec, Write_ದತ್ತಾಂಶ_File_Flags flags);
