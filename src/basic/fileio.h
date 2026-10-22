/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు LONG_పంక్తి_MAX (1U*1024U*1024U)

రకనిర్వచనం గణన {
        WRITE_STRING_FILE_CREATE                     = 1 << 0,
        WRITE_STRING_FILE_TRUNCATE                   = 1 << 1,
        WRITE_STRING_FILE_ATOMIC                     = 1 << 2,
        WRITE_STRING_FILE_AVOID_NEWLINE              = 1 << 3,
        WRITE_STRING_FILE_VERIFY_ON_FAILURE          = 1 << 4,
        WRITE_STRING_FILE_VERIFY_IGNORE_NEWLINE      = 1 << 5,
        WRITE_STRING_FILE_SYNC                       = 1 << 6,
        WRITE_STRING_FILE_DISABLE_బఫరు             = 1 << 7,
        WRITE_STRING_FILE_NOFOLLOW                   = 1 << 8,
        WRITE_STRING_FILE_MKDIR_0755                 = 1 << 9,
        WRITE_STRING_FILE_బహుళకం_0600                  = 1 << 10,
        WRITE_STRING_FILE_బహుళకం_0444                  = 1 << 11,
        WRITE_STRING_FILE_SUPPRESS_REDUNDANT_VIRTUAL = 1 << 12,
        WRITE_STRING_FILE_LABEL                      = 1 << 13,
        WRITE_STRING_FILE_OPEN_NONBLOCKING           = 1 << 14,
} WriteStringFileFlags;

రకనిర్వచనం గణన {
        READ_నిండిన_FILE_SECURE              = 1 << 0, /* erase any buffers we employ internally, after use */
        READ_నిండిన_FILE_UNBASE64            = 1 << 1, /* base64 decode what we read */
        READ_నిండిన_FILE_UNHEX               = 1 << 2, /* hex decode what we read */
        READ_నిండిన_FILE_WARN_WORLD_READABLE = 1 << 3, /* if regular file, log at LOG_WARNING level if access mode above 0700 */
        READ_నిండిన_FILE_CONNECT_SOCKET      = 1 << 4, /* if socket inode, connect to it and read off it */
        READ_నిండిన_FILE_FAIL_WHEN_LARGER    = 1 << 5, /* fail loading if file is larger than specified size */
        READ_నిండిన_FILE_VERIFY_REGULAR      = 1 << 6, /* before reading, verify this is a regular file */
} Read_నిండిన_File_Flags;

పూర్ణాంకం fdopen_unlocked(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *options, FILE **ret);
పూర్ణాంకం take_fdopen_unlocked(పూర్ణాంకం *fd, స్థిరరాశి అక్షరం *options, FILE **ret);
FILE* take_fdopen(పూర్ణాంకం *fd, స్థిరరాశి అక్షరం *options);
DIR* take_fdopendir(పూర్ణాంకం *dfd);
FILE* open_memstream_unlocked(అక్షరం **ptr, size_t *sizeloc);
FILE* fmemopen_unlocked(శూన్య *buf, size_t కొలత, స్థిరరాశి అక్షరం *బహుళకం);

పూర్ణాంకం write_string_ప్రవాహం_నిండిన(FILE *f, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags, స్థిరరాశి నిర్మాణం timespec *ts);
స్థిర అంతఃవరుస పూర్ణాంకం write_string_ప్రవాహం(FILE *f, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags) {
        తిరిగి write_string_ప్రవాహం_నిండిన(f, పంక్తి, flags, ఖాళీ);
}

పూర్ణాంకం write_string_file_నిండిన_label(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags, స్థిరరాశి నిర్మాణం timespec *ts, స్థిరరాశి అక్షరం *label_fn, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం write_string_file_నిండిన(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags, స్థిరరాశి నిర్మాణం timespec *ts, స్థిరరాశి అక్షరం *label_fn) {
        తిరిగి write_string_file_నిండిన_label(dir_fd, fn, పంక్తి, flags, ts, label_fn, /* label_context= */ ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం write_string_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags) {
        తిరిగి write_string_file_నిండిన(dir_fd, fn, పంక్తి, flags, ఖాళీ, ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం write_string_file_fd(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags) {
        తిరిగి write_string_file_at(dir_fd, ఖాళీ, పంక్తి, flags);
}
స్థిర అంతఃవరుస పూర్ణాంకం write_string_file(స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *పంక్తి, WriteStringFileFlags flags) {
        తిరిగి write_string_file_at(AT_FDCWD, fn, పంక్తి, flags);
}
పూర్ణాంకం write_string_filef(స్థిరరాశి అక్షరం *fn, WriteStringFileFlags flags, స్థిరరాశి అక్షరం *format, ...) _printf_(3, 4);
పూర్ణాంకం write_string_filef_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, WriteStringFileFlags flags, స్థిరరాశి అక్షరం *format, ...) _printf_(4, 5);

పూర్ణాంకం write_base64_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, స్థిరరాశి నిర్మాణం iovec *దత్తాంశం, WriteStringFileFlags flags);

పూర్ణాంకం read_one_పంక్తి_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename, అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం read_one_పంక్తి_file(స్థిరరాశి అక్షరం *filename, అక్షరం **ret) {
        తిరిగి read_one_పంక్తి_file_at(AT_FDCWD, filename, ret);
}
పూర్ణాంకం read_boolean_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename);
స్థిర అంతఃవరుస పూర్ణాంకం read_boolean_file(స్థిరరాశి అక్షరం *filename) {
        తిరిగి read_boolean_file_at(AT_FDCWD, filename);
}
పూర్ణాంకం read_నిండిన_file_నిండిన(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename, uint64_t స్థానభ్రంశం, size_t కొలత, Read_నిండిన_File_Flags flags, స్థిరరాశి అక్షరం *bind_name, అక్షరం **ret_contents, size_t *ret_కొలత);
స్థిర అంతఃవరుస పూర్ణాంకం read_నిండిన_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename, అక్షరం **ret_contents, size_t *ret_కొలత) {
        తిరిగి read_నిండిన_file_నిండిన(dir_fd, filename, UINT64_MAX, SIZE_MAX, 0, ఖాళీ, ret_contents, ret_కొలత);
}
స్థిర అంతఃవరుస పూర్ణాంకం read_నిండిన_file(స్థిరరాశి అక్షరం *filename, అక్షరం **ret_contents, size_t *ret_కొలత) {
        తిరిగి read_నిండిన_file_నిండిన(AT_FDCWD, filename, UINT64_MAX, SIZE_MAX, 0, ఖాళీ, ret_contents, ret_కొలత);
}

పూర్ణాంకం read_virtual_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *filename, size_t max_కొలత, అక్షరం **ret_contents, size_t *ret_కొలత);
స్థిర అంతఃవరుస పూర్ణాంకం read_virtual_file_fd(పూర్ణాంకం fd, size_t max_కొలత, అక్షరం **ret_contents, size_t *ret_కొలత) {
        తిరిగి read_virtual_file_at(fd, ఖాళీ, max_కొలత, ret_contents, ret_కొలత);
}
స్థిర అంతఃవరుస పూర్ణాంకం read_virtual_file(స్థిరరాశి అక్షరం *filename, size_t max_కొలత, అక్షరం **ret_contents, size_t *ret_కొలత) {
        తిరిగి read_virtual_file_at(AT_FDCWD, filename, max_కొలత, ret_contents, ret_కొలత);
}
స్థిర అంతఃవరుస పూర్ణాంకం read_నిండిన_virtual_file(స్థిరరాశి అక్షరం *filename, అక్షరం **ret_contents, size_t *ret_కొలత) {
        తిరిగి read_virtual_file(filename, SIZE_MAX, ret_contents, ret_కొలత);
}

పూర్ణాంకం read_నిండిన_ప్రవాహం_నిండిన(FILE *f, స్థిరరాశి అక్షరం *filename, uint64_t స్థానభ్రంశం, size_t కొలత, Read_నిండిన_File_Flags flags, అక్షరం **ret_contents, size_t *ret_కొలత);
స్థిర అంతఃవరుస పూర్ణాంకం read_నిండిన_ప్రవాహం(FILE *f, అక్షరం **ret_contents, size_t *ret_కొలత) {
        తిరిగి read_నిండిన_ప్రవాహం_నిండిన(f, ఖాళీ, UINT64_MAX, SIZE_MAX, 0, ret_contents, ret_కొలత);
}

పూర్ణాంకం verify_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం *blob, bool accept_extra_nl);

పూర్ణాంకం script_get_shebang_interpreter(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret);

పూర్ణాంకం get_proc_క్షేత్రం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *కీలకం, అక్షరం **ret);

DIR* xopendirat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags);

రకనిర్వచనం గణన XfopenFlags {
        XFOPEN_UNLOCKED = 1 << 0, /* call __fsetlocking(FSETLOCKING_BYCALLER) after opened */
        XFOPEN_SOCKET   = 1 << 1, /* also try to open unix socket */
} XfopenFlags;

పూర్ణాంకం xfopenat_నిండిన(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                స్థిరరాశి అక్షరం *బహుళకం,
                పూర్ణాంకం open_flags,
                XfopenFlags flags,
                స్థిరరాశి అక్షరం *bind_name,
                FILE **ret);
స్థిర అంతఃవరుస పూర్ణాంకం xfopenat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *బహుళకం, పూర్ణాంకం open_flags, FILE **ret) {
        తిరిగి xfopenat_నిండిన(dir_fd, మార్గం, బహుళకం, open_flags, 0, ఖాళీ, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం fopen_unlocked_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *బహుళకం, పూర్ణాంకం open_flags, FILE **ret) {
        తిరిగి xfopenat_నిండిన(dir_fd, మార్గం, బహుళకం, open_flags, XFOPEN_UNLOCKED, ఖాళీ, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం fopen_unlocked(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *బహుళకం, FILE **ret) {
        తిరిగి fopen_unlocked_at(AT_FDCWD, మార్గం, బహుళకం, 0, ret);
}

పూర్ణాంకం fdopen_independent(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *బహుళకం, FILE **ret);

పూర్ణాంకం శోధన_and_open(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం బహుళకం, స్థిరరాశి అక్షరం *మూలం, అక్షరం **శోధన, పూర్ణాంకం *ret_fd, అక్షరం **ret_మార్గం);
స్థిర అంతఃవరుస పూర్ణాంకం శోధన_and_access(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం బహుళకం, స్థిరరాశి అక్షరం *మూలం, అక్షరం**శోధన, అక్షరం **ret_మార్గం) {
        తిరిగి శోధన_and_open(మార్గం, బహుళకం, మూలం, శోధన, ఖాళీ, ret_మార్గం);
}
పూర్ణాంకం శోధన_and_fopen(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *బహుళకం, స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం **శోధన, FILE **ret_file, అక్షరం **ret_మార్గం);
పూర్ణాంకం శోధన_and_fopen_nulstr(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *బహుళకం, స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం *శోధన, FILE **ret_file, అక్షరం **ret_మార్గం);

పూర్ణాంకం fflush_and_check(FILE *f);
పూర్ణాంకం fflush_sync_and_check(FILE *f);

పూర్ణాంకం write_timestamp_file_atomic(స్థిరరాశి అక్షరం *fn, usec_t n);
పూర్ణాంకం read_timestamp_file(స్థిరరాశి అక్షరం *fn, usec_t *ret);

పూర్ణాంకం fputs_with_separator(FILE *f, స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *separator, bool *space);
పూర్ణాంకం fputs_with_newline(FILE *f, స్థిరరాశి అక్షరం *s);

రకనిర్వచనం గణన Read_పంక్తి_Flags {
        READ_పంక్తి_ONLY_NUL  = 1 << 0,
        READ_పంక్తి_IS_A_TTY  = 1 << 1,
        READ_పంక్తి_NOT_A_TTY = 1 << 2,
} Read_పంక్తి_Flags;

పూర్ణాంకం read_పంక్తి_నిండిన(FILE *f, size_t limit, Read_పంక్తి_Flags flags, అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం read_పంక్తి(FILE *f, size_t limit, అక్షరం **ret) {
        తిరిగి read_పంక్తి_నిండిన(f, limit, 0, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం read_nul_string(FILE *f, size_t limit, అక్షరం **ret) {
        తిరిగి read_పంక్తి_నిండిన(f, limit, READ_పంక్తి_ONLY_NUL, ret);
}

పూర్ణాంకం read_stripped_పంక్తి(FILE *f, size_t limit, అక్షరం **ret);

స్థిర అంతఃవరుస bool file_స్థానభ్రంశం_beyond_జ్ఞాపకం_కొలత(off_t x) {
        ఒకవేళ (x < 0) /* off_t is signed, filter that out */
                తిరిగి false;
        తిరిగి (uint64_t) x > (uint64_t) SIZE_MAX;
}

పూర్ణాంకం సురక్షిత_fgetc(FILE *f, అక్షరం *ret);

పూర్ణాంకం warn_file_is_world_accessible(స్థిరరాశి అక్షరం *filename, నిర్మాణం stat *st, స్థిరరాశి అక్షరం *unit, గుర్తులేని పంక్తి);

పూర్ణాంకం fopen_బహుళకం_గమ్యం_flags(స్థిరరాశి అక్షరం *బహుళకం);

రకనిర్వచనం గణన Write_దత్తాంశం_File_Flags {
        WRITE_దత్తాంశం_FILE_MKDIR_0755 = 1 << 0,
        WRITE_దత్తాంశం_FILE_బహుళకం_0400  = 1 << 1,
} Write_దత్తాంశం_File_Flags;

పూర్ణాంకం write_దత్తాంశం_file_atomic_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి నిర్మాణం iovec *iovec, Write_దత్తాంశం_File_Flags flags);
