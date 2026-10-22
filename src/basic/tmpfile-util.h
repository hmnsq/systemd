/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ fopen_temporary_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file, ವರ್ಣ **ret_path);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fopen_temporary(ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file, ವರ್ಣ **ret_path) {
        ಪ್ರತ್ಯಾಗಮ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

ಪೂರ್ಣಾಙ್ಕ fopen_temporary_child_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file, ವರ್ಣ **ret_path);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fopen_temporary_child(ಧ್ರುವ ವರ್ಣ *path, FILE **ret_file, ವರ್ಣ **ret_path) {
        ಪ್ರತ್ಯಾಗಮ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

ಪೂರ್ಣಾಙ್ಕ mkostemp_safe(ವರ್ಣ *pattern);
ಪೂರ್ಣಾಙ್ಕ fmkostemp_safe(ವರ್ಣ *pattern, ಧ್ರುವ ವರ್ಣ *mode, FILE**_f);

ಶೂನ್ಯ unlink_tempfilep(ವರ್ಣ (*p)[]);

ಪೂರ್ಣಾಙ್ಕ tempfn_xxxxxx(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *extra, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ tempfn_random(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *extra, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ tempfn_random_child(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *extra, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ open_tmpfile_unlinkable(ಧ್ರುವ ವರ್ಣ *directory, ಪೂರ್ಣಾಙ್ಕ flags);
ಪೂರ್ಣಾಙ್ಕ open_tmpfile_linkable_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_path);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ open_tmpfile_linkable(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_path) {
        ಪ್ರತ್ಯಾಗಮ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
ಪೂರ್ಣಾಙ್ಕ fopen_tmpfile_linkable_at(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_path, FILE **ret_file);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ fopen_tmpfile_linkable(ಧ್ರುವ ವರ್ಣ *target, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_path, FILE **ret_file) {
        ಪ್ರತ್ಯಾಗಮ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ಪೂರ್ಣಾಙ್ಕ link_tmpfile_at(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *target, LinkTmpfileFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ link_tmpfile(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *target, LinkTmpfileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
ಪೂರ್ಣಾಙ್ಕ flink_tmpfile_at(FILE *f, ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *target, LinkTmpfileFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ flink_tmpfile(FILE *f, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *target, LinkTmpfileFlags flags) {
        ಪ್ರತ್ಯಾಗಮ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

ಪೂರ್ಣಾಙ್ಕ mkdtemp_malloc(ಧ್ರುವ ವರ್ಣ *template, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ mkdtemp_open(ಧ್ರುವ ವರ್ಣ *template, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ಸಂರಚನಾ cleanup_tmpfile_data {
        ಪೂರ್ಣಾಙ್ಕ *dir_fd;
        ವರ್ಣ **filename;
};
ಶೂನ್ಯ cleanup_tmpfile_data_done(ಸಂರಚನಾ cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) ಸಂರಚನಾ cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
