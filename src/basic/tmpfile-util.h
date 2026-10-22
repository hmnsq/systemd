/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

puur_naa_nka fopen_temporary_at(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha puur_naa_nka fopen_temporary(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ᬯᬮᬶ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

puur_naa_nka fopen_temporary_child_at(puur_naa_nka dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha puur_naa_nka fopen_temporary_child(const var_na *path, FILE **ret_file, var_na **ret_path) {
        ᬯᬮᬶ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

puur_naa_nka mkostemp_safe(var_na *pattern);
puur_naa_nka fmkostemp_safe(var_na *pattern, const var_na *mode, FILE**_f);

void unlink_tempfilep(var_na (*p)[]);

puur_naa_nka tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret);
puur_naa_nka tempfn_random(const var_na *p, const var_na *extra, var_na **ret);
puur_naa_nka tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret);

puur_naa_nka open_tmpfile_unlinkable(const var_na *directory, puur_naa_nka flags);
puur_naa_nka open_tmpfile_linkable_at(puur_naa_nka dir_fd, const var_na *target, puur_naa_nka flags, var_na **ret_path);
static anta_hstha puur_naa_nka open_tmpfile_linkable(const var_na *target, puur_naa_nka flags, var_na **ret_path) {
        ᬯᬮᬶ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
puur_naa_nka fopen_tmpfile_linkable_at(puur_naa_nka dir_fd, const var_na *target, puur_naa_nka flags, var_na **ret_path, FILE **ret_file);
static anta_hstha puur_naa_nka fopen_tmpfile_linkable(const var_na *target, puur_naa_nka flags, var_na **ret_path, FILE **ret_file) {
        ᬯᬮᬶ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef ga_nanaa LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

puur_naa_nka link_tmpfile_at(puur_naa_nka fd, puur_naa_nka dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha puur_naa_nka link_tmpfile(puur_naa_nka fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ᬯᬮᬶ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
puur_naa_nka flink_tmpfile_at(FILE *f, puur_naa_nka dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha puur_naa_nka flink_tmpfile(FILE *f, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        ᬯᬮᬶ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

puur_naa_nka mkdtemp_malloc(const var_na *template, var_na **ret);
puur_naa_nka mkdtemp_open(const var_na *template, puur_naa_nka flags, var_na **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
sa_nracanaa cleanup_tmpfile_data {
        puur_naa_nka *dir_fd;
        var_na **filename;
};
void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) sa_nracanaa cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
