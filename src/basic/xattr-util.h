/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

पूर्णाङ्क getxattr_at_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags, वर्ण **ret, size_t *ret_size);
स्थिर अन्तःस्थ पूर्णाङ्क getxattr_malloc(ध्रुव वर्ण *path, ध्रुव वर्ण *name, वर्ण **ret, size_t *ret_size) {
        प्रत्यागम getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क lgetxattr_malloc(ध्रुव वर्ण *path, ध्रुव वर्ण *name, वर्ण **ret, size_t *ret_size) {
        प्रत्यागम getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
स्थिर अन्तःस्थ पूर्णाङ्क fgetxattr_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *name, वर्ण **ret, size_t *ret_size) {
        प्रत्यागम getxattr_at_malloc(fd, रिक्त, name, AT_EMPTY_PATH, ret, ret_size);
}

पूर्णाङ्क getxattr_at_bool(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags);
पूर्णाङ्क getxattr_at_strv(पूर्णाङ्क fd, ध्रुव वर्ण *path, ध्रुव वर्ण *name, पूर्णाङ्क at_flags, वर्ण ***ret_strv);

पूर्णाङ्क listxattr_at_malloc(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क listxattr_malloc(ध्रुव वर्ण *path, वर्ण **ret) {
        प्रत्यागम listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क llistxattr_malloc(ध्रुव वर्ण *path, वर्ण **ret) {
        प्रत्यागम listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
स्थिर अन्तःस्थ पूर्णाङ्क flistxattr_malloc(पूर्णाङ्क fd, वर्ण **ret) {
        प्रत्यागम listxattr_at_malloc(fd, रिक्त, AT_EMPTY_PATH, ret);
}

पूर्णाङ्क xsetxattr_full(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मानम्,
                size_t size,
                पूर्णाङ्क xattr_flags);
स्थिर अन्तःस्थ पूर्णाङ्क xsetxattr(
                पूर्णाङ्क fd,
                ध्रुव वर्ण *path,
                पूर्णाङ्क at_flags,
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मानम्) {
        प्रत्यागम xsetxattr_full(fd, path, at_flags, name, मानम्, SIZE_MAX, 0);
}

पूर्णाङ्क xsetxattr_strv(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, ध्रुव वर्ण *name, वर्ण * ध्रुव *l);

पूर्णाङ्क xremovexattr(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, ध्रुव वर्ण *name);

पूर्णाङ्क fd_setcrtime(पूर्णाङ्क fd, usec_t usec);
पूर्णाङ्क getcrtime_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, पूर्णाङ्क at_flags, usec_t *ret);
स्थिर अन्तःस्थ पूर्णाङ्क fd_getcrtime(पूर्णाङ्क fd, usec_t *ret) {
        प्रत्यागम getcrtime_at(fd, रिक्त, 0, ret);
}

bool xattr_is_acl(ध्रुव वर्ण *name);
bool xattr_is_selinux(ध्रुव वर्ण *name);
