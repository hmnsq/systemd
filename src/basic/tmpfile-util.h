/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

целое fopen_temporary_at(целое dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline целое fopen_temporary(const char *path, FILE **ret_file, char **ret_path) {
        вернуть fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

целое fopen_temporary_child_at(целое dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline целое fopen_temporary_child(const char *path, FILE **ret_file, char **ret_path) {
        вернуть fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

целое mkostemp_safe(char *pattern);
целое fmkostemp_safe(char *pattern, const char *mode, FILE**_f);

void unlink_tempfilep(char (*p)[]);

целое tempfn_xxxxxx(const char *p, const char *extra, char **ret);
целое tempfn_random(const char *p, const char *extra, char **ret);
целое tempfn_random_child(const char *p, const char *extra, char **ret);

целое open_tmpfile_unlinkable(const char *directory, целое flags);
целое open_tmpfile_linkable_at(целое dir_fd, const char *target, целое flags, char **ret_path);
static inline целое open_tmpfile_linkable(const char *target, целое flags, char **ret_path) {
        вернуть open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
целое fopen_tmpfile_linkable_at(целое dir_fd, const char *target, целое flags, char **ret_path, FILE **ret_file);
static inline целое fopen_tmpfile_linkable(const char *target, целое flags, char **ret_path, FILE **ret_file) {
        вернуть fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef enum LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

целое link_tmpfile_at(целое fd, целое dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline целое link_tmpfile(целое fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        вернуть link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
целое flink_tmpfile_at(FILE *f, целое dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline целое flink_tmpfile(FILE *f, const char *path, const char *target, LinkTmpfileFlags flags) {
        вернуть flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

целое mkdtemp_malloc(const char *template, char **ret);
целое mkdtemp_open(const char *template, целое flags, char **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
struct cleanup_tmpfile_data {
        целое *dir_fd;
        char **filename;
};
void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) struct cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
