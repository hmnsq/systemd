/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

целое getxattr_at_malloc(целое fd, const char *path, const char *name, целое at_flags, char **ret, size_t *ret_size);
static inline целое getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        вернуть getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline целое lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        вернуть getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline целое fgetxattr_malloc(целое fd, const char *name, char **ret, size_t *ret_size) {
        вернуть getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

целое getxattr_at_bool(целое fd, const char *path, const char *name, целое at_flags);
целое getxattr_at_strv(целое fd, const char *path, const char *name, целое at_flags, char ***ret_strv);

целое listxattr_at_malloc(целое fd, const char *path, целое at_flags, char **ret);
static inline целое listxattr_malloc(const char *path, char **ret) {
        вернуть listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline целое llistxattr_malloc(const char *path, char **ret) {
        вернуть listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline целое flistxattr_malloc(целое fd, char **ret) {
        вернуть listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

целое xsetxattr_full(
                целое fd,
                const char *path,
                целое at_flags,
                const char *name,
                const char *value,
                size_t size,
                целое xattr_flags);
static inline целое xsetxattr(
                целое fd,
                const char *path,
                целое at_flags,
                const char *name,
                const char *value) {
        вернуть xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

целое xsetxattr_strv(целое fd, const char *path, целое at_flags, const char *name, char * const *l);

целое xremovexattr(целое fd, const char *path, целое at_flags, const char *name);

целое fd_setcrtime(целое fd, usec_t usec);
целое getcrtime_at(целое fd, const char *path, целое at_flags, usec_t *ret);
static inline целое fd_getcrtime(целое fd, usec_t *ret) {
        вернуть getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
