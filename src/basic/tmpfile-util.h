/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

முழு fopen_temporary_at(முழு dir_fd, மாறிலி எழுத்து *path, FILE **ret_file, எழுத்து **ret_path);
நிலையான உள்வரி முழு fopen_temporary(மாறிலி எழுத்து *path, FILE **ret_file, எழுத்து **ret_path) {
        திரும்பு fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

முழு fopen_temporary_child_at(முழு dir_fd, மாறிலி எழுத்து *path, FILE **ret_file, எழுத்து **ret_path);
நிலையான உள்வரி முழு fopen_temporary_child(மாறிலி எழுத்து *path, FILE **ret_file, எழுத்து **ret_path) {
        திரும்பு fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

முழு mkostemp_safe(எழுத்து *pattern);
முழு fmkostemp_safe(எழுத்து *pattern, மாறிலி எழுத்து *mode, FILE**_f);

வெறுமை unlink_tempfilep(எழுத்து (*p)[]);

முழு tempfn_xxxxxx(மாறிலி எழுத்து *p, மாறிலி எழுத்து *extra, எழுத்து **ret);
முழு tempfn_random(மாறிலி எழுத்து *p, மாறிலி எழுத்து *extra, எழுத்து **ret);
முழு tempfn_random_child(மாறிலி எழுத்து *p, மாறிலி எழுத்து *extra, எழுத்து **ret);

முழு open_tmpfile_unlinkable(மாறிலி எழுத்து *directory, முழு flags);
முழு open_tmpfile_linkable_at(முழு dir_fd, மாறிலி எழுத்து *target, முழு flags, எழுத்து **ret_path);
நிலையான உள்வரி முழு open_tmpfile_linkable(மாறிலி எழுத்து *target, முழு flags, எழுத்து **ret_path) {
        திரும்பு open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
முழு fopen_tmpfile_linkable_at(முழு dir_fd, மாறிலி எழுத்து *target, முழு flags, எழுத்து **ret_path, FILE **ret_file);
நிலையான உள்வரி முழு fopen_tmpfile_linkable(மாறிலி எழுத்து *target, முழு flags, எழுத்து **ret_path, FILE **ret_file) {
        திரும்பு fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

வகைவரை எண்ணிக்கை LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

முழு link_tmpfile_at(முழு fd, முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *target, LinkTmpfileFlags flags);
நிலையான உள்வரி முழு link_tmpfile(முழு fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *target, LinkTmpfileFlags flags) {
        திரும்பு link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
முழு flink_tmpfile_at(FILE *f, முழு dir_fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *target, LinkTmpfileFlags flags);
நிலையான உள்வரி முழு flink_tmpfile(FILE *f, மாறிலி எழுத்து *path, மாறிலி எழுத்து *target, LinkTmpfileFlags flags) {
        திரும்பு flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

முழு mkdtemp_malloc(மாறிலி எழுத்து *template, எழுத்து **ret);
முழு mkdtemp_open(மாறிலி எழுத்து *template, முழு flags, எழுத்து **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
கட்டமைப்பு cleanup_tmpfile_data {
        முழு *dir_fd;
        எழுத்து **filename;
};
வெறுமை cleanup_tmpfile_data_done(கட்டமைப்பு cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) கட்டமைப்பு cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
