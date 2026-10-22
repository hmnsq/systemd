/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णांक fopen_temporary_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, खाता **ret_file, अक्षर **ret_path);
स्थिर ओळीत पूर्णांक fopen_temporary(स्थिरांक अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        परत fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णांक fopen_temporary_child_at(पूर्णांक dir_fd, स्थिरांक अक्षर *path, खाता **ret_file, अक्षर **ret_path);
स्थिर ओळीत पूर्णांक fopen_temporary_child(स्थिरांक अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        परत fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णांक mkostemp_safe(अक्षर *pattern);
पूर्णांक fmkostemp_safe(अक्षर *pattern, स्थिरांक अक्षर *mode, खाता**_f);

शून्य unlink_tempfilep(अक्षर (*p)[]);

पूर्णांक tempfn_xxxxxx(स्थिरांक अक्षर *p, स्थिरांक अक्षर *extra, अक्षर **ret);
पूर्णांक tempfn_random(स्थिरांक अक्षर *p, स्थिरांक अक्षर *extra, अक्षर **ret);
पूर्णांक tempfn_random_child(स्थिरांक अक्षर *p, स्थिरांक अक्षर *extra, अक्षर **ret);

पूर्णांक open_tmpfile_unlinkable(स्थिरांक अक्षर *directory, पूर्णांक flags);
पूर्णांक open_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिरांक अक्षर *target, पूर्णांक flags, अक्षर **ret_path);
स्थिर ओळीत पूर्णांक open_tmpfile_linkable(स्थिरांक अक्षर *target, पूर्णांक flags, अक्षर **ret_path) {
        परत open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
पूर्णांक fopen_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिरांक अक्षर *target, पूर्णांक flags, अक्षर **ret_path, खाता **ret_file);
स्थिर ओळीत पूर्णांक fopen_tmpfile_linkable(स्थिरांक अक्षर *target, पूर्णांक flags, अक्षर **ret_path, खाता **ret_file) {
        परत fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

प्रकारव्याख्या गणना LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

पूर्णांक link_tmpfile_at(पूर्णांक fd, पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *target, LinkTmpfileFlags flags);
स्थिर ओळीत पूर्णांक link_tmpfile(पूर्णांक fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *target, LinkTmpfileFlags flags) {
        परत link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
पूर्णांक flink_tmpfile_at(खाता *f, पूर्णांक dir_fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *target, LinkTmpfileFlags flags);
स्थिर ओळीत पूर्णांक flink_tmpfile(खाता *f, स्थिरांक अक्षर *path, स्थिरांक अक्षर *target, LinkTmpfileFlags flags) {
        परत flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

पूर्णांक mkdtemp_malloc(स्थिरांक अक्षर *ढाँचा, अक्षर **ret);
पूर्णांक mkdtemp_open(स्थिरांक अक्षर *ढाँचा, पूर्णांक flags, अक्षर **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
संरचना cleanup_tmpfile_data {
        पूर्णांक *dir_fd;
        अक्षर **filename;
};
शून्य cleanup_tmpfile_data_done(संरचना cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) संरचना cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
