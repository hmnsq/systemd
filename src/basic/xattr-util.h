/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णांक getxattr_at_malloc(पूर्णांक fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, पूर्णांक at_flags, अक्षर **ret, माप_प्रकार *ret_size);
स्थिर ओळीत पूर्णांक getxattr_malloc(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, अक्षर **ret, माप_प्रकार *ret_size) {
        परत getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
स्थिर ओळीत पूर्णांक lgetxattr_malloc(स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, अक्षर **ret, माप_प्रकार *ret_size) {
        परत getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
स्थिर ओळीत पूर्णांक fgetxattr_malloc(पूर्णांक fd, स्थिरांक अक्षर *name, अक्षर **ret, माप_प्रकार *ret_size) {
        परत getxattr_at_malloc(fd, रिक्त, name, AT_EMPTY_PATH, ret, ret_size);
}

पूर्णांक getxattr_at_bool(पूर्णांक fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, पूर्णांक at_flags);
पूर्णांक getxattr_at_strv(पूर्णांक fd, स्थिरांक अक्षर *path, स्थिरांक अक्षर *name, पूर्णांक at_flags, अक्षर ***ret_strv);

पूर्णांक listxattr_at_malloc(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, अक्षर **ret);
स्थिर ओळीत पूर्णांक listxattr_malloc(स्थिरांक अक्षर *path, अक्षर **ret) {
        परत listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
स्थिर ओळीत पूर्णांक llistxattr_malloc(स्थिरांक अक्षर *path, अक्षर **ret) {
        परत listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
स्थिर ओळीत पूर्णांक flistxattr_malloc(पूर्णांक fd, अक्षर **ret) {
        परत listxattr_at_malloc(fd, रिक्त, AT_EMPTY_PATH, ret);
}

पूर्णांक xsetxattr_full(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक at_flags,
                स्थिरांक अक्षर *name,
                स्थिरांक अक्षर *मूल्य,
                माप_प्रकार size,
                पूर्णांक xattr_flags);
स्थिर ओळीत पूर्णांक xsetxattr(
                पूर्णांक fd,
                स्थिरांक अक्षर *path,
                पूर्णांक at_flags,
                स्थिरांक अक्षर *name,
                स्थिरांक अक्षर *मूल्य) {
        परत xsetxattr_full(fd, path, at_flags, name, मूल्य, SIZE_MAX, 0);
}

पूर्णांक xsetxattr_strv(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, स्थिरांक अक्षर *name, अक्षर * स्थिरांक *l);

पूर्णांक xremovexattr(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, स्थिरांक अक्षर *name);

पूर्णांक fd_setcrtime(पूर्णांक fd, usec_t usec);
पूर्णांक getcrtime_at(पूर्णांक fd, स्थिरांक अक्षर *path, पूर्णांक at_flags, usec_t *ret);
स्थिर ओळीत पूर्णांक fd_getcrtime(पूर्णांक fd, usec_t *ret) {
        परत getcrtime_at(fd, रिक्त, 0, ret);
}

bool xattr_is_acl(स्थिरांक अक्षर *name);
bool xattr_is_selinux(स्थिरांक अक्षर *name);
