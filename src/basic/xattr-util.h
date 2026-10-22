/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ціле getxattr_at_malloc(ціле fd, const char *path, const char *name, ціле at_flags, char **ret, size_t *ret_size);
static inline ціле getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        повернути getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline ціле lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        повернути getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline ціле fgetxattr_malloc(ціле fd, const char *name, char **ret, size_t *ret_size) {
        повернути getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

ціле getxattr_at_bool(ціле fd, const char *path, const char *name, ціле at_flags);
ціле getxattr_at_strv(ціле fd, const char *path, const char *name, ціле at_flags, char ***ret_strv);

ціле listxattr_at_malloc(ціле fd, const char *path, ціле at_flags, char **ret);
static inline ціле listxattr_malloc(const char *path, char **ret) {
        повернути listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline ціле llistxattr_malloc(const char *path, char **ret) {
        повернути listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline ціле flistxattr_malloc(ціле fd, char **ret) {
        повернути listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

ціле xsetxattr_full(
                ціле fd,
                const char *path,
                ціле at_flags,
                const char *name,
                const char *value,
                size_t size,
                ціле xattr_flags);
static inline ціле xsetxattr(
                ціле fd,
                const char *path,
                ціле at_flags,
                const char *name,
                const char *value) {
        повернути xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

ціле xsetxattr_strv(ціле fd, const char *path, ціле at_flags, const char *name, char * const *l);

ціле xremovexattr(ціле fd, const char *path, ціле at_flags, const char *name);

ціле fd_setcrtime(ціле fd, usec_t usec);
ціле getcrtime_at(ціле fd, const char *path, ціле at_flags, usec_t *ret);
static inline ціле fd_getcrtime(ціле fd, usec_t *ret) {
        повернути getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
