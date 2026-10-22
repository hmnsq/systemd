/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

정수 getxattr_at_malloc(정수 fd, const char *path, const char *name, 정수 at_flags, char **ret, size_t *ret_size);
static inline 정수 getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        반환 getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline 정수 lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        반환 getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline 정수 fgetxattr_malloc(정수 fd, const char *name, char **ret, size_t *ret_size) {
        반환 getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

정수 getxattr_at_bool(정수 fd, const char *path, const char *name, 정수 at_flags);
정수 getxattr_at_strv(정수 fd, const char *path, const char *name, 정수 at_flags, char ***ret_strv);

정수 listxattr_at_malloc(정수 fd, const char *path, 정수 at_flags, char **ret);
static inline 정수 listxattr_malloc(const char *path, char **ret) {
        반환 listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline 정수 llistxattr_malloc(const char *path, char **ret) {
        반환 listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline 정수 flistxattr_malloc(정수 fd, char **ret) {
        반환 listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

정수 xsetxattr_full(
                정수 fd,
                const char *path,
                정수 at_flags,
                const char *name,
                const char *value,
                size_t size,
                정수 xattr_flags);
static inline 정수 xsetxattr(
                정수 fd,
                const char *path,
                정수 at_flags,
                const char *name,
                const char *value) {
        반환 xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

정수 xsetxattr_strv(정수 fd, const char *path, 정수 at_flags, const char *name, char * const *l);

정수 xremovexattr(정수 fd, const char *path, 정수 at_flags, const char *name);

정수 fd_setcrtime(정수 fd, usec_t usec);
정수 getcrtime_at(정수 fd, const char *path, 정수 at_flags, usec_t *ret);
static inline 정수 fd_getcrtime(정수 fd, usec_t *ret) {
        반환 getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
