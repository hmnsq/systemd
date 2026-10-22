/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णाङ्क fopen_temporary_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, FILE **ret_file, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_temporary(ध्रुव वर्ण *path, FILE **ret_file, वर्ण **ret_path) {
        पटिनिवत्त fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णाङ्क fopen_temporary_child_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, FILE **ret_file, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_temporary_child(ध्रुव वर्ण *path, FILE **ret_file, वर्ण **ret_path) {
        पटिनिवत्त fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णाङ्क mkostemp_safe(वर्ण *pattern);
पूर्णाङ्क fmkostemp_safe(वर्ण *pattern, ध्रुव वर्ण *mode, FILE**_f);

शून्य unlink_tempfilep(वर्ण (*p)[]);

पूर्णाङ्क tempfn_xxxxxx(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret);
पूर्णाङ्क tempfn_random(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret);
पूर्णाङ्क tempfn_random_child(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret);

पूर्णाङ्क open_tmpfile_unlinkable(ध्रुव वर्ण *directory, पूर्णाङ्क flags);
पूर्णाङ्क open_tmpfile_linkable_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क open_tmpfile_linkable(ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path) {
        पटिनिवत्त open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
पूर्णाङ्क fopen_tmpfile_linkable_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path, FILE **ret_file);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_tmpfile_linkable(ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path, FILE **ret_file) {
        पटिनिवत्त fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

प्रकारनिर्देश गणना LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

पूर्णाङ्क link_tmpfile_at(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *target, LinkTmpfileFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क link_tmpfile(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *target, LinkTmpfileFlags flags) {
        पटिनिवत्त link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
पूर्णाङ्क flink_tmpfile_at(FILE *f, पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, ध्रुव वर्ण *target, LinkTmpfileFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क flink_tmpfile(FILE *f, ध्रुव वर्ण *path, ध्रुव वर्ण *target, LinkTmpfileFlags flags) {
        पटिनिवत्त flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

पूर्णाङ्क mkdtemp_malloc(ध्रुव वर्ण *template, वर्ण **ret);
पूर्णाङ्क mkdtemp_open(ध्रुव वर्ण *template, पूर्णाङ्क flags, वर्ण **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
संरचना cleanup_tmpfile_data {
        पूर्णाङ्क *dir_fd;
        वर्ण **filename;
};
शून्य cleanup_tmpfile_data_done(संरचना cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) संरचना cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
