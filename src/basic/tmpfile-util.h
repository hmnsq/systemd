/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ціле fopen_temporary_at(ціле dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline ціле fopen_temporary(const char *path, FILE **ret_file, char **ret_path) {
        повернути fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

ціле fopen_temporary_child_at(ціле dir_fd, const char *path, FILE **ret_file, char **ret_path);
static inline ціле fopen_temporary_child(const char *path, FILE **ret_file, char **ret_path) {
        повернути fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

ціле mkostemp_safe(char *pattern);
ціле fmkostemp_safe(char *pattern, const char *mode, FILE**_f);

void unlink_tempfilep(char (*p)[]);

ціле tempfn_xxxxxx(const char *p, const char *extra, char **ret);
ціле tempfn_random(const char *p, const char *extra, char **ret);
ціле tempfn_random_child(const char *p, const char *extra, char **ret);

ціле open_tmpfile_unlinkable(const char *directory, ціле flags);
ціле open_tmpfile_linkable_at(ціле dir_fd, const char *target, ціле flags, char **ret_path);
static inline ціле open_tmpfile_linkable(const char *target, ціле flags, char **ret_path) {
        повернути open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
ціле fopen_tmpfile_linkable_at(ціле dir_fd, const char *target, ціле flags, char **ret_path, FILE **ret_file);
static inline ціле fopen_tmpfile_linkable(const char *target, ціле flags, char **ret_path, FILE **ret_file) {
        повернути fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef enum LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ціле link_tmpfile_at(ціле fd, ціле dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline ціле link_tmpfile(ціле fd, const char *path, const char *target, LinkTmpfileFlags flags) {
        повернути link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
ціле flink_tmpfile_at(FILE *f, ціле dir_fd, const char *path, const char *target, LinkTmpfileFlags flags);
static inline ціле flink_tmpfile(FILE *f, const char *path, const char *target, LinkTmpfileFlags flags) {
        повернути flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

ціле mkdtemp_malloc(const char *template, char **ret);
ціле mkdtemp_open(const char *template, ціле flags, char **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
struct cleanup_tmpfile_data {
        ціле *dir_fd;
        char **filename;
};
void cleanup_tmpfile_data_done(struct cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) struct cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
