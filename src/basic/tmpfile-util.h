/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file, ଅକ୍ଷର **ret_path);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary(ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file, ଅକ୍ଷର **ret_path) {
        ଫେରନ୍ତୁ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary_child_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file, ଅକ୍ଷର **ret_path);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_temporary_child(ଅଚଳ ଅକ୍ଷର *path, FILE **ret_file, ଅକ୍ଷର **ret_path) {
        ଫେରନ୍ତୁ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkostemp_safe(ଅକ୍ଷର *pattern);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fmkostemp_safe(ଅକ୍ଷର *pattern, ଅଚଳ ଅକ୍ଷର *mode, FILE**_f);

ଶୂନ୍ୟ unlink_tempfilep(ଅକ୍ଷର (*p)[]);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_xxxxxx(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *extra, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_random(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *extra, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା tempfn_random_child(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ ଅକ୍ଷର *extra, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_tmpfile_unlinkable(ଅଚଳ ଅକ୍ଷର *directory, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_tmpfile_linkable_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_path);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_tmpfile_linkable(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_path) {
        ଫେରନ୍ତୁ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_tmpfile_linkable_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_path, FILE **ret_file);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fopen_tmpfile_linkable(ଅଚଳ ଅକ୍ଷର *target, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_path, FILE **ret_file) {
        ଫେରନ୍ତୁ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା link_tmpfile_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *target, LinkTmpfileFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା link_tmpfile(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *target, LinkTmpfileFlags flags) {
        ଫେରନ୍ତୁ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା flink_tmpfile_at(FILE *f, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *target, LinkTmpfileFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା flink_tmpfile(FILE *f, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *target, LinkTmpfileFlags flags) {
        ଫେରନ୍ତୁ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdtemp_malloc(ଅଚଳ ଅକ୍ଷର *template, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdtemp_open(ଅଚଳ ଅକ୍ଷର *template, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ସଂରଚନା cleanup_tmpfile_data {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dir_fd;
        ଅକ୍ଷର **filename;
};
ଶୂନ୍ୟ cleanup_tmpfile_data_done(ସଂରଚନା cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) ସଂରଚନା cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
