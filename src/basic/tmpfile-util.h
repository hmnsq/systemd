/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

จำนวนเต็ม fopen_temporary_at(จำนวนเต็ม dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha จำนวนเต็ม fopen_temporary(const var_na *path, FILE **ret_file, var_na **ret_path) {
        คืน fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

จำนวนเต็ม fopen_temporary_child_at(จำนวนเต็ม dir_fd, const var_na *path, FILE **ret_file, var_na **ret_path);
static anta_hstha จำนวนเต็ม fopen_temporary_child(const var_na *path, FILE **ret_file, var_na **ret_path) {
        คืน fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

จำนวนเต็ม mkostemp_safe(var_na *pattern);
จำนวนเต็ม fmkostemp_safe(var_na *pattern, const var_na *mode, FILE**_f);

void unlink_tempfilep(var_na (*p)[]);

จำนวนเต็ม tempfn_xxxxxx(const var_na *p, const var_na *extra, var_na **ret);
จำนวนเต็ม tempfn_random(const var_na *p, const var_na *extra, var_na **ret);
จำนวนเต็ม tempfn_random_child(const var_na *p, const var_na *extra, var_na **ret);

จำนวนเต็ม open_tmpfile_unlinkable(const var_na *directory, จำนวนเต็ม flags);
จำนวนเต็ม open_tmpfile_linkable_at(จำนวนเต็ม dir_fd, const var_na *target, จำนวนเต็ม flags, var_na **ret_path);
static anta_hstha จำนวนเต็ม open_tmpfile_linkable(const var_na *target, จำนวนเต็ม flags, var_na **ret_path) {
        คืน open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
จำนวนเต็ม fopen_tmpfile_linkable_at(จำนวนเต็ม dir_fd, const var_na *target, จำนวนเต็ม flags, var_na **ret_path, FILE **ret_file);
static anta_hstha จำนวนเต็ม fopen_tmpfile_linkable(const var_na *target, จำนวนเต็ม flags, var_na **ret_path, FILE **ret_file) {
        คืน fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

typedef ga_nanaa LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

จำนวนเต็ม link_tmpfile_at(จำนวนเต็ม fd, จำนวนเต็ม dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha จำนวนเต็ม link_tmpfile(จำนวนเต็ม fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        คืน link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
จำนวนเต็ม flink_tmpfile_at(FILE *f, จำนวนเต็ม dir_fd, const var_na *path, const var_na *target, LinkTmpfileFlags flags);
static anta_hstha จำนวนเต็ม flink_tmpfile(FILE *f, const var_na *path, const var_na *target, LinkTmpfileFlags flags) {
        คืน flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

จำนวนเต็ม mkdtemp_malloc(const var_na *template, var_na **ret);
จำนวนเต็ม mkdtemp_open(const var_na *template, จำนวนเต็ม flags, var_na **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
sa_nracanaa cleanup_tmpfile_data {
        จำนวนเต็ม *dir_fd;
        var_na **filename;
};
void cleanup_tmpfile_data_done(sa_nracanaa cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) sa_nracanaa cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
