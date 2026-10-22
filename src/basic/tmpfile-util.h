/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণাঙ্ক fopen_temporary_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, FILE **ret_file, বর্ণ **ret_path);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fopen_temporary(ধ্রুব বর্ণ *path, FILE **ret_file, বর্ণ **ret_path) {
        প্রত্যাগম fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

পূর্ণাঙ্ক fopen_temporary_child_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, FILE **ret_file, বর্ণ **ret_path);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fopen_temporary_child(ধ্রুব বর্ণ *path, FILE **ret_file, বর্ণ **ret_path) {
        প্রত্যাগম fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

পূর্ণাঙ্ক mkostemp_safe(বর্ণ *pattern);
পূর্ণাঙ্ক fmkostemp_safe(বর্ণ *pattern, ধ্রুব বর্ণ *mode, FILE**_f);

শূন্য unlink_tempfilep(বর্ণ (*p)[]);

পূর্ণাঙ্ক tempfn_xxxxxx(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *extra, বর্ণ **ret);
পূর্ণাঙ্ক tempfn_random(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *extra, বর্ণ **ret);
পূর্ণাঙ্ক tempfn_random_child(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *extra, বর্ণ **ret);

পূর্ণাঙ্ক open_tmpfile_unlinkable(ধ্রুব বর্ণ *directory, পূর্ণাঙ্ক flags);
পূর্ণাঙ্ক open_tmpfile_linkable_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *target, পূর্ণাঙ্ক flags, বর্ণ **ret_path);
স্থির অন্তঃস্থ পূর্ণাঙ্ক open_tmpfile_linkable(ধ্রুব বর্ণ *target, পূর্ণাঙ্ক flags, বর্ণ **ret_path) {
        প্রত্যাগম open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
পূর্ণাঙ্ক fopen_tmpfile_linkable_at(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *target, পূর্ণাঙ্ক flags, বর্ণ **ret_path, FILE **ret_file);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fopen_tmpfile_linkable(ধ্রুব বর্ণ *target, পূর্ণাঙ্ক flags, বর্ণ **ret_path, FILE **ret_file) {
        প্রত্যাগম fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

প্রকারনির্দেশ গণনা LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

পূর্ণাঙ্ক link_tmpfile_at(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *target, LinkTmpfileFlags flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক link_tmpfile(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *target, LinkTmpfileFlags flags) {
        প্রত্যাগম link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
পূর্ণাঙ্ক flink_tmpfile_at(FILE *f, পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *target, LinkTmpfileFlags flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক flink_tmpfile(FILE *f, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *target, LinkTmpfileFlags flags) {
        প্রত্যাগম flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

পূর্ণাঙ্ক mkdtemp_malloc(ধ্রুব বর্ণ *template, বর্ণ **ret);
পূর্ণাঙ্ক mkdtemp_open(ধ্রুব বর্ণ *template, পূর্ণাঙ্ক flags, বর্ণ **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
সংরচনা cleanup_tmpfile_data {
        পূর্ণাঙ্ক *dir_fd;
        বর্ণ **filename;
};
শূন্য cleanup_tmpfile_data_done(সংরচনা cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) সংরচনা cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
