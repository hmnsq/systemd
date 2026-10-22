/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णाङ्क fopen_temporary_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, खाता **ret_file, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_temporary(अचर वर्ण *path, खाता **ret_file, वर्ण **ret_path) {
        फर्काऊ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णाङ्क fopen_temporary_child_at(पूर्णाङ्क dir_fd, अचर वर्ण *path, खाता **ret_file, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_temporary_child(अचर वर्ण *path, खाता **ret_file, वर्ण **ret_path) {
        फर्काऊ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णाङ्क mkostemp_safe(वर्ण *pattern);
पूर्णाङ्क fmkostemp_safe(वर्ण *pattern, अचर वर्ण *mode, खाता**_f);

शून्य unlink_tempfilep(वर्ण (*p)[]);

पूर्णाङ्क tempfn_xxxxxx(अचर वर्ण *p, अचर वर्ण *extra, वर्ण **ret);
पूर्णाङ्क tempfn_random(अचर वर्ण *p, अचर वर्ण *extra, वर्ण **ret);
पूर्णाङ्क tempfn_random_child(अचर वर्ण *p, अचर वर्ण *extra, वर्ण **ret);

पूर्णाङ्क open_tmpfile_unlinkable(अचर वर्ण *directory, पूर्णाङ्क flags);
पूर्णाङ्क open_tmpfile_linkable_at(पूर्णाङ्क dir_fd, अचर वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path);
स्थिर अन्तःस्थ पूर्णाङ्क open_tmpfile_linkable(अचर वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path) {
        फर्काऊ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
पूर्णाङ्क fopen_tmpfile_linkable_at(पूर्णाङ्क dir_fd, अचर वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path, खाता **ret_file);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_tmpfile_linkable(अचर वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_path, खाता **ret_file) {
        फर्काऊ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

प्रकारनिर्देश गणना LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

पूर्णाङ्क link_tmpfile_at(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर वर्ण *target, LinkTmpfileFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क link_tmpfile(पूर्णाङ्क fd, अचर वर्ण *path, अचर वर्ण *target, LinkTmpfileFlags flags) {
        फर्काऊ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
पूर्णाङ्क flink_tmpfile_at(खाता *f, पूर्णाङ्क dir_fd, अचर वर्ण *path, अचर वर्ण *target, LinkTmpfileFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क flink_tmpfile(खाता *f, अचर वर्ण *path, अचर वर्ण *target, LinkTmpfileFlags flags) {
        फर्काऊ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

पूर्णाङ्क mkdtemp_malloc(अचर वर्ण *ढाँचा, वर्ण **ret);
पूर्णाङ्क mkdtemp_open(अचर वर्ण *ढाँचा, पूर्णाङ्क flags, वर्ण **ret);

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
