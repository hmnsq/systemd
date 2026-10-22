/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

số_nguyên fopen_temporary_at(số_nguyên dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline số_nguyên fopen_temporary(const char *path, FILE **ret_file, char **ret_path) {
        trả_về fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

số_nguyên fopen_temporary_child_at(số_nguyên dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline số_nguyên fopen_temporary_child(const char *path, FILE **ret_file, char **ret_path) {
        trả_về fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

số_nguyên mkostemp_safe(char *pattern);
số_nguyên fmkostemp_safe(char *pattern, const char *mode, FILE**_f);

void unlink_tempfilep(char (*p)[]);

số_nguyên tempfn_xxxxxx(const char *p, const char *extra, char **ret);
số_nguyên tempfn_random(const char *p, const char *extra, char **ret);
số_nguyên tempfn_random_child(const char *p, const char *extra, char **ret);

số_nguyên open_tmpfile_unlinkable(const char *directory, số_nguyên flags);
số_nguyên open_tmpfile_linkable_at(số_nguyên dir_fd, const char *target, số_nguyên flags, char **ret_path);
static inline số_nguyên open_tmpfile_linkable(const char *target, số_nguyên flags, char **ret_path) {
        trả_về open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
số_nguyên fopen_tmpfile_linkable_at(số_nguyên dir_fd, const char *target, số_nguyên flags, char **ret_path, FILE **ret_file);
static inline số_nguyên fopen_tmpfile_linkable(const char *target, số_nguyên flags, char **ret_path, FILE **ret_file) {
        trả_về fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef enum LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

số_nguyên link_tmpfile_at(số_nguyên fd, số_nguyên dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline số_nguyên link_tmpfile(số_nguyên fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        trả_về link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
số_nguyên flink_tmpfile_at(FILE *f, số_nguyên dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline số_nguyên flink_tmpfile(FILE *f, const char *path, const char *target, LinkTmpfileFlags flags) {
        trả_về flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

số_nguyên mkdtemp_malloc(const char *template, char **ret);
số_nguyên mkdtemp_open(const char *template, số_nguyên flags, char **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
struct cleanup_tmpfile_data {
        số_nguyên *dir_fd;
        char **filename;
};
void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) struct cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
