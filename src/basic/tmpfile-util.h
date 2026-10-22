/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ຈຳນວນເຕັມ fopen_temporary_at(ຈຳນວນເຕັມ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha ຈຳນວນເຕັມ fopen_temporary(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ສົ່ງຄືນ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

ຈຳນວນເຕັມ fopen_temporary_child_at(ຈຳນວນເຕັມ dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha ຈຳນວນເຕັມ fopen_temporary_child(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ສົ່ງຄືນ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

ຈຳນວນເຕັມ mkostemp_safe(var_na *pattern);
ຈຳນວນເຕັມ fmkostemp_safe(var_na *pattern, const var_na *mode, FILE**_f);

void unlink_tempfilep(var_na (*p)[]);

ຈຳນວນເຕັມ tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret);
ຈຳນວນເຕັມ tempfn_random(const var_na *p, const var_na *extra, var_na **ret);
ຈຳນວນເຕັມ tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret);

ຈຳນວນເຕັມ open_tmpfile_unlinkable(const var_na *directory, ຈຳນວນເຕັມ flags);
ຈຳນວນເຕັມ open_tmpfile_linkable_at(ຈຳນວນເຕັມ dir_fd, const var_na *target, ຈຳນວນເຕັມ flags, var_na **ret_path);
static anta_hstha ຈຳນວນເຕັມ open_tmpfile_linkable(const var_na *target, ຈຳນວນເຕັມ flags, var_na **ret_path) {
        ສົ່ງຄືນ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
ຈຳນວນເຕັມ fopen_tmpfile_linkable_at(ຈຳນວນເຕັມ dir_fd, const var_na *target, ຈຳນວນເຕັມ flags, var_na **ret_path, FILE **ret_file);
static anta_hstha ຈຳນວນເຕັມ fopen_tmpfile_linkable(const var_na *target, ຈຳນວນເຕັມ flags, var_na **ret_path, FILE **ret_file) {
        ສົ່ງຄືນ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef ga_nanaa LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ຈຳນວນເຕັມ link_tmpfile_at(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha ຈຳນວນເຕັມ link_tmpfile(ຈຳນວນເຕັມ fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ສົ່ງຄືນ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
ຈຳນວນເຕັມ flink_tmpfile_at(FILE *f, ຈຳນວນເຕັມ dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha ຈຳນວນເຕັມ flink_tmpfile(FILE *f, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ສົ່ງຄືນ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

ຈຳນວນເຕັມ mkdtemp_malloc(const var_na *template, var_na **ret);
ຈຳນວນເຕັມ mkdtemp_open(const var_na *template, ຈຳນວນເຕັມ flags, var_na **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
sa_nracanaa cleanup_tmpfile_data {
        ຈຳນວນເຕັມ *dir_fd;
        var_na **filename;
};
void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) sa_nracanaa cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
