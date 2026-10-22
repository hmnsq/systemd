/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

整数 getxattr_at_malloc(整数 fd, const char *path, const char *name, 整数 at_flags, char **ret, size_t *ret_size);
static inline 整数 getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        返回 getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline 整数 lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        返回 getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline 整数 fgetxattr_malloc(整数 fd, const char *name, char **ret, size_t *ret_size) {
        返回 getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

整数 getxattr_at_bool(整数 fd, const char *path, const char *name, 整数 at_flags);
整数 getxattr_at_strv(整数 fd, const char *path, const char *name, 整数 at_flags, char ***ret_strv);

整数 listxattr_at_malloc(整数 fd, const char *path, 整数 at_flags, char **ret);
static inline 整数 listxattr_malloc(const char *path, char **ret) {
        返回 listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline 整数 llistxattr_malloc(const char *path, char **ret) {
        返回 listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline 整数 flistxattr_malloc(整数 fd, char **ret) {
        返回 listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

整数 xsetxattr_full(
                整数 fd,
                const char *path,
                整数 at_flags,
                const char *name,
                const char *value,
                size_t size,
                整数 xattr_flags);
static inline 整数 xsetxattr(
                整数 fd,
                const char *path,
                整数 at_flags,
                const char *name,
                const char *value) {
        返回 xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

整数 xsetxattr_strv(整数 fd, const char *path, 整数 at_flags, const char *name, char * const *l);

整数 xremovexattr(整数 fd, const char *path, 整数 at_flags, const char *name);

整数 fd_setcrtime(整数 fd, usec_t usec);
整数 getcrtime_at(整数 fd, const char *path, 整数 at_flags, usec_t *ret);
static inline 整数 fd_getcrtime(整数 fd, usec_t *ret) {
        返回 getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
