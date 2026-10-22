/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цяло fopen_temporary_at(цяло dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline цяло fopen_temporary(const char *path, FILE **ret_file, char **ret_path) {
        върни fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

цяло fopen_temporary_child_at(цяло dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline цяло fopen_temporary_child(const char *path, FILE **ret_file, char **ret_path) {
        върни fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

цяло mkostemp_safe(char *pattern);
цяло fmkostemp_safe(char *pattern, const char *mode, FILE**_f);

void unlink_tempfilep(char (*p)[]);

цяло tempfn_xxxxxx(const char *p, const char *extra, char **ret);
цяло tempfn_random(const char *p, const char *extra, char **ret);
цяло tempfn_random_child(const char *p, const char *extra, char **ret);

цяло open_tmpfile_unlinkable(const char *directory, цяло flags);
цяло open_tmpfile_linkable_at(цяло dir_fd, const char *target, цяло flags, char **ret_path);
static inline цяло open_tmpfile_linkable(const char *target, цяло flags, char **ret_path) {
        върни open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
цяло fopen_tmpfile_linkable_at(цяло dir_fd, const char *target, цяло flags, char **ret_path, FILE **ret_file);
static inline цяло fopen_tmpfile_linkable(const char *target, цяло flags, char **ret_path, FILE **ret_file) {
        върни fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef enum LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

цяло link_tmpfile_at(цяло fd, цяло dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline цяло link_tmpfile(цяло fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        върни link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
цяло flink_tmpfile_at(FILE *f, цяло dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline цяло flink_tmpfile(FILE *f, const char *path, const char *target, LinkTmpfileFlags flags) {
        върни flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

цяло mkdtemp_malloc(const char *template, char **ret);
цяло mkdtemp_open(const char *template, цяло flags, char **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
struct cleanup_tmpfile_data {
        цяло *dir_fd;
        char **filename;
};
void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) struct cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
