/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

पूर्णांक getxattr_at_malloc(पूर्णांक fd, स्थिर अक्षर *path, स्थिर अक्षर *name, पूर्णांक at_flags, अक्षर **ret, माप_प्रकार *ret_आकार);
अटल अंतरभूत पूर्णांक getxattr_malloc(स्थिर अक्षर *path, स्थिर अक्षर *name, अक्षर **ret, माप_प्रकार *ret_आकार) {
        वापस getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_आकार);
}
अटल अंतरभूत पूर्णांक lgetxattr_malloc(स्थिर अक्षर *path, स्थिर अक्षर *name, अक्षर **ret, माप_प्रकार *ret_आकार) {
        वापस getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_आकार);
}
अटल अंतरभूत पूर्णांक fgetxattr_malloc(पूर्णांक fd, स्थिर अक्षर *name, अक्षर **ret, माप_प्रकार *ret_आकार) {
        वापस getxattr_at_malloc(fd, शून्य, name, AT_EMPTY_PATH, ret, ret_आकार);
}

पूर्णांक getxattr_at_bool(पूर्णांक fd, स्थिर अक्षर *path, स्थिर अक्षर *name, पूर्णांक at_flags);
पूर्णांक getxattr_at_strv(पूर्णांक fd, स्थिर अक्षर *path, स्थिर अक्षर *name, पूर्णांक at_flags, अक्षर ***ret_strv);

पूर्णांक listxattr_at_malloc(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, अक्षर **ret);
अटल अंतरभूत पूर्णांक listxattr_malloc(स्थिर अक्षर *path, अक्षर **ret) {
        वापस listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
अटल अंतरभूत पूर्णांक llistxattr_malloc(स्थिर अक्षर *path, अक्षर **ret) {
        वापस listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
अटल अंतरभूत पूर्णांक flistxattr_malloc(पूर्णांक fd, अक्षर **ret) {
        वापस listxattr_at_malloc(fd, शून्य, AT_EMPTY_PATH, ret);
}

पूर्णांक xsetxattr_full(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक at_flags,
                स्थिर अक्षर *name,
                स्थिर अक्षर *मूल्य,
                माप_प्रकार आकार,
                पूर्णांक xattr_flags);
अटल अंतरभूत पूर्णांक xsetxattr(
                पूर्णांक fd,
                स्थिर अक्षर *path,
                पूर्णांक at_flags,
                स्थिर अक्षर *name,
                स्थिर अक्षर *मूल्य) {
        वापस xsetxattr_full(fd, path, at_flags, name, मूल्य, SIZE_MAX, 0);
}

पूर्णांक xsetxattr_strv(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, स्थिर अक्षर *name, अक्षर * स्थिर *l);

पूर्णांक xremovexattr(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, स्थिर अक्षर *name);

पूर्णांक fd_setcrtime(पूर्णांक fd, usec_t usec);
पूर्णांक getcrtime_at(पूर्णांक fd, स्थिर अक्षर *path, पूर्णांक at_flags, usec_t *ret);
अटल अंतरभूत पूर्णांक fd_getcrtime(पूर्णांक fd, usec_t *ret) {
        वापस getcrtime_at(fd, शून्य, 0, ret);
}

bool xattr_is_acl(स्थिर अक्षर *name);
bool xattr_is_selinux(स्थिर अक्षर *name);
