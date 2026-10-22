/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

पूर्णाङ्क fopen_temporary_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, FILE **ret_file, वर्ण **ret_मार्गः);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_temporary(ध्रुव वर्ण *मार्गः, FILE **ret_file, वर्ण **ret_मार्गः) {
        प्रत्यागम fopen_temporary_at(AT_FDCWD, मार्गः, ret_file, ret_मार्गः);
}

पूर्णाङ्क fopen_temporary_child_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, FILE **ret_file, वर्ण **ret_मार्गः);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_temporary_child(ध्रुव वर्ण *मार्गः, FILE **ret_file, वर्ण **ret_मार्गः) {
        प्रत्यागम fopen_temporary_child_at(AT_FDCWD, मार्गः, ret_file, ret_मार्गः);
}

पूर्णाङ्क mkostemp_safe(वर्ण *आदर्शः);
पूर्णाङ्क fmkostemp_safe(वर्ण *आदर्शः, ध्रुव वर्ण *बहुलकम्, FILE**_f);

शून्य unlink_tempfilep(वर्ण (*p)[]);

पूर्णाङ्क tempfn_xxxxxx(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret);
पूर्णाङ्क tempfn_random(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret);
पूर्णाङ्क tempfn_random_child(ध्रुव वर्ण *p, ध्रुव वर्ण *extra, वर्ण **ret);

पूर्णाङ्क open_tmpfile_unlinkable(ध्रुव वर्ण *directory, पूर्णाङ्क flags);
पूर्णाङ्क open_tmpfile_linkable_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_मार्गः);
स्थिर अन्तःस्थ पूर्णाङ्क open_tmpfile_linkable(ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_मार्गः) {
        प्रत्यागम open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_मार्गः);
}
पूर्णाङ्क fopen_tmpfile_linkable_at(पूर्णाङ्क dir_fd, ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_मार्गः, FILE **ret_file);
स्थिर अन्तःस्थ पूर्णाङ्क fopen_tmpfile_linkable(ध्रुव वर्ण *target, पूर्णाङ्क flags, वर्ण **ret_मार्गः, FILE **ret_file) {
        प्रत्यागम fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_मार्गः, ret_file);
}

प्रकारनिर्देश गणना LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

पूर्णाङ्क link_tmpfile_at(पूर्णाङ्क fd, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *target, LinkTmpfileFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क link_tmpfile(पूर्णाङ्क fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *target, LinkTmpfileFlags flags) {
        प्रत्यागम link_tmpfile_at(fd, AT_FDCWD, मार्गः, target, flags);
}
पूर्णाङ्क flink_tmpfile_at(FILE *f, पूर्णाङ्क dir_fd, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *target, LinkTmpfileFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क flink_tmpfile(FILE *f, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *target, LinkTmpfileFlags flags) {
        प्रत्यागम flink_tmpfile_at(f, AT_FDCWD, मार्गः, target, flags);
}

पूर्णाङ्क mkdtemp_malloc(ध्रुव वर्ण *प्रतिरूपम्, वर्ण **ret);
पूर्णाङ्क mkdtemp_open(ध्रुव वर्ण *प्रतिरूपम्, पूर्णाङ्क flags, वर्ण **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
संरचना cleanup_tmpfile_दत्त {
        पूर्णाङ्क *dir_fd;
        वर्ण **filename;
};
शून्य cleanup_tmpfile_दत्त_समाप्तम्(संरचना cleanup_tmpfile_दत्त *d);
#परिभाषय _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        अप्रयुक्तम् _cleanup_(cleanup_tmpfile_दत्त_समाप्तम्) संरचना cleanup_tmpfile_दत्त UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#परिभाषय CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
