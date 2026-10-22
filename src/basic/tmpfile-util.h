/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ཧྲིལ fopen_temporary_at(ཧྲིལ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha ཧྲིལ fopen_temporary(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ཕྱིར fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

ཧྲིལ fopen_temporary_child_at(ཧྲིལ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha ཧྲིལ fopen_temporary_child(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ཕྱིར fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

ཧྲིལ mkostemp_safe(var_na *pattern);
ཧྲིལ fmkostemp_safe(var_na *pattern, const var_na *mode, FILE**_f);

void unlink_tempfilep(var_na (*p)[]);

ཧྲིལ tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret);
ཧྲིལ tempfn_random(const var_na *p, const var_na *extra, var_na **ret);
ཧྲིལ tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret);

ཧྲིལ open_tmpfile_unlinkable(const var_na *directory, ཧྲིལ flags);
ཧྲིལ open_tmpfile_linkable_at(ཧྲིལ dir_fd, const var_na *target, ཧྲིལ flags, var_na **ret_path);
static anta_hstha ཧྲིལ open_tmpfile_linkable(const var_na *target, ཧྲིལ flags, var_na **ret_path) {
        ཕྱིར open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
ཧྲིལ fopen_tmpfile_linkable_at(ཧྲིལ dir_fd, const var_na *target, ཧྲིལ flags, var_na **ret_path, FILE **ret_file);
static anta_hstha ཧྲིལ fopen_tmpfile_linkable(const var_na *target, ཧྲིལ flags, var_na **ret_path, FILE **ret_file) {
        ཕྱིར fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef ga_nanaa LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ཧྲིལ link_tmpfile_at(ཧྲིལ fd, ཧྲིལ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha ཧྲིལ link_tmpfile(ཧྲིལ fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ཕྱིར link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
ཧྲིལ flink_tmpfile_at(FILE *f, ཧྲིལ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha ཧྲིལ flink_tmpfile(FILE *f, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ཕྱིར flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

ཧྲིལ mkdtemp_malloc(const var_na *template, var_na **ret);
ཧྲིལ mkdtemp_open(const var_na *template, ཧྲིལ flags, var_na **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
sa_nracanaa cleanup_tmpfile_data {
        ཧྲིལ *dir_fd;
        var_na **filename;
};
void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) sa_nracanaa cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
