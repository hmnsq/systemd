/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ចំនួនគត់ fopen_temporary_at(ចំនួនគត់ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha ចំនួនគត់ fopen_temporary(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ត្រឡប់ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

ចំនួនគត់ fopen_temporary_child_at(ចំនួនគត់ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha ចំនួនគត់ fopen_temporary_child(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ត្រឡប់ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

ចំនួនគត់ mkostemp_safe(var_na *pattern);
ចំនួនគត់ fmkostemp_safe(var_na *pattern, const var_na *mode, FILE**_f);

void unlink_tempfilep(var_na (*p)[]);

ចំនួនគត់ tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret);
ចំនួនគត់ tempfn_random(const var_na *p, const var_na *extra, var_na **ret);
ចំនួនគត់ tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret);

ចំនួនគត់ open_tmpfile_unlinkable(const var_na *directory, ចំនួនគត់ flags);
ចំនួនគត់ open_tmpfile_linkable_at(ចំនួនគត់ dir_fd, const var_na *target, ចំនួនគត់ flags, var_na **ret_path);
static anta_hstha ចំនួនគត់ open_tmpfile_linkable(const var_na *target, ចំនួនគត់ flags, var_na **ret_path) {
        ត្រឡប់ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
ចំនួនគត់ fopen_tmpfile_linkable_at(ចំនួនគត់ dir_fd, const var_na *target, ចំនួនគត់ flags, var_na **ret_path, FILE **ret_file);
static anta_hstha ចំនួនគត់ fopen_tmpfile_linkable(const var_na *target, ចំនួនគត់ flags, var_na **ret_path, FILE **ret_file) {
        ត្រឡប់ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef ga_nanaa LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ចំនួនគត់ link_tmpfile_at(ចំនួនគត់ fd, ចំនួនគត់ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha ចំនួនគត់ link_tmpfile(ចំនួនគត់ fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ត្រឡប់ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
ចំនួនគត់ flink_tmpfile_at(FILE *f, ចំនួនគត់ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha ចំនួនគត់ flink_tmpfile(FILE *f, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ត្រឡប់ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

ចំនួនគត់ mkdtemp_malloc(const var_na *template, var_na **ret);
ចំនួនគត់ mkdtemp_open(const var_na *template, ចំនួនគត់ flags, var_na **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
sa_nracanaa cleanup_tmpfile_data {
        ចំនួនគត់ *dir_fd;
        var_na **filename;
};
void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) sa_nracanaa cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
