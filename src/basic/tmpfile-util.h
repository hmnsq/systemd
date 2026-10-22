/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

पूर्णांक fopen_temporary_at(पूर्णांक dir_fd, स्थिर अक्षर *path, खाता **ret_file, अक्षर **ret_path);
अटल अंतरभूत पूर्णांक fopen_temporary(स्थिर अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        वापस fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णांक fopen_temporary_child_at(पूर्णांक dir_fd, स्थिर अक्षर *path, खाता **ret_file, अक्षर **ret_path);
अटल अंतरभूत पूर्णांक fopen_temporary_child(स्थिर अक्षर *path, खाता **ret_file, अक्षर **ret_path) {
        वापस fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

पूर्णांक mkostemp_safe(अक्षर *pattern);
पूर्णांक fmkostemp_safe(अक्षर *pattern, स्थिर अक्षर *mode, खाता**_f);

व्योम unlink_tempfilep(अक्षर (*p)[]);

पूर्णांक tempfn_xxxxxx(स्थिर अक्षर *p, स्थिर अक्षर *extra, अक्षर **ret);
पूर्णांक tempfn_random(स्थिर अक्षर *p, स्थिर अक्षर *extra, अक्षर **ret);
पूर्णांक tempfn_random_child(स्थिर अक्षर *p, स्थिर अक्षर *extra, अक्षर **ret);

पूर्णांक open_tmpfile_unlinkable(स्थिर अक्षर *directory, पूर्णांक flags);
पूर्णांक open_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिर अक्षर *target, पूर्णांक flags, अक्षर **ret_path);
अटल अंतरभूत पूर्णांक open_tmpfile_linkable(स्थिर अक्षर *target, पूर्णांक flags, अक्षर **ret_path) {
        वापस open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
पूर्णांक fopen_tmpfile_linkable_at(पूर्णांक dir_fd, स्थिर अक्षर *target, पूर्णांक flags, अक्षर **ret_path, खाता **ret_file);
अटल अंतरभूत पूर्णांक fopen_tmpfile_linkable(स्थिर अक्षर *target, पूर्णांक flags, अक्षर **ret_path, खाता **ret_file) {
        वापस fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

प्रकार क्रमागत LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

पूर्णांक link_tmpfile_at(पूर्णांक fd, पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *target, LinkTmpfileFlags flags);
अटल अंतरभूत पूर्णांक link_tmpfile(पूर्णांक fd, स्थिर अक्षर *path, स्थिर अक्षर *target, LinkTmpfileFlags flags) {
        वापस link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
पूर्णांक flink_tmpfile_at(खाता *f, पूर्णांक dir_fd, स्थिर अक्षर *path, स्थिर अक्षर *target, LinkTmpfileFlags flags);
अटल अंतरभूत पूर्णांक flink_tmpfile(खाता *f, स्थिर अक्षर *path, स्थिर अक्षर *target, LinkTmpfileFlags flags) {
        वापस flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

पूर्णांक mkdtemp_malloc(स्थिर अक्षर *ढाँचा, अक्षर **ret);
पूर्णांक mkdtemp_open(स्थिर अक्षर *ढाँचा, पूर्णांक flags, अक्षर **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
काष्ठा cleanup_tmpfile_तथ्य {
        पूर्णांक *dir_fd;
        अक्षर **filename;
};
व्योम cleanup_tmpfile_तथ्य_done(काष्ठा cleanup_tmpfile_तथ्य *d);
#घोषणा _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_तथ्य_done) काष्ठा cleanup_tmpfile_तथ्य UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#घोषणा CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
