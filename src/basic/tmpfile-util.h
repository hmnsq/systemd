/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цео fopen_temporary_at(цео dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline цео fopen_temporary(const char *path, FILE **ret_file, char **ret_path) {
        врати fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

цео fopen_temporary_child_at(цео dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline цео fopen_temporary_child(const char *path, FILE **ret_file, char **ret_path) {
        врати fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

цео mkostemp_safe(char *pattern);
цео fmkostemp_safe(char *pattern, const char *mode, FILE**_f);

void unlink_tempfilep(char (*p)[]);

цео tempfn_xxxxxx(const char *p, const char *extra, char **ret);
цео tempfn_random(const char *p, const char *extra, char **ret);
цео tempfn_random_child(const char *p, const char *extra, char **ret);

цео open_tmpfile_unlinkable(const char *directory, цео flags);
цео open_tmpfile_linkable_at(цео dir_fd, const char *target, цео flags, char **ret_path);
static inline цео open_tmpfile_linkable(const char *target, цео flags, char **ret_path) {
        врати open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
цео fopen_tmpfile_linkable_at(цео dir_fd, const char *target, цео flags, char **ret_path, FILE **ret_file);
static inline цео fopen_tmpfile_linkable(const char *target, цео flags, char **ret_path, FILE **ret_file) {
        врати fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef enum LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

цео link_tmpfile_at(цео fd, цео dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline цео link_tmpfile(цео fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        врати link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
цео flink_tmpfile_at(FILE *f, цео dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline цео flink_tmpfile(FILE *f, const char *path, const char *target, LinkTmpfileFlags flags) {
        врати flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

цео mkdtemp_malloc(const char *template, char **ret);
цео mkdtemp_open(const char *template, цео flags, char **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
struct cleanup_tmpfile_data {
        цео *dir_fd;
        char **filename;
};
void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) struct cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
