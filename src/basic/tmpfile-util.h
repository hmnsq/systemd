/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

정수 fopen_temporary_at(정수 dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline 정수 fopen_temporary(const char *path, FILE **ret_file, char **ret_path) {
        반환 fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

정수 fopen_temporary_child_at(정수 dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline 정수 fopen_temporary_child(const char *path, FILE **ret_file, char **ret_path) {
        반환 fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

정수 mkostemp_safe(char *pattern);
정수 fmkostemp_safe(char *pattern, const char *mode, FILE**_f);

void unlink_tempfilep(char (*p)[]);

정수 tempfn_xxxxxx(const char *p, const char *extra, char **ret);
정수 tempfn_random(const char *p, const char *extra, char **ret);
정수 tempfn_random_child(const char *p, const char *extra, char **ret);

정수 open_tmpfile_unlinkable(const char *directory, 정수 flags);
정수 open_tmpfile_linkable_at(정수 dir_fd, const char *target, 정수 flags, char **ret_path);
static inline 정수 open_tmpfile_linkable(const char *target, 정수 flags, char **ret_path) {
        반환 open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
정수 fopen_tmpfile_linkable_at(정수 dir_fd, const char *target, 정수 flags, char **ret_path, FILE **ret_file);
static inline 정수 fopen_tmpfile_linkable(const char *target, 정수 flags, char **ret_path, FILE **ret_file) {
        반환 fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef enum LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

정수 link_tmpfile_at(정수 fd, 정수 dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline 정수 link_tmpfile(정수 fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        반환 link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
정수 flink_tmpfile_at(FILE *f, 정수 dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline 정수 flink_tmpfile(FILE *f, const char *path, const char *target, LinkTmpfileFlags flags) {
        반환 flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

정수 mkdtemp_malloc(const char *template, char **ret);
정수 mkdtemp_open(const char *template, 정수 flags, char **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
struct cleanup_tmpfile_data {
        정수 *dir_fd;
        char **filename;
};
void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) struct cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
