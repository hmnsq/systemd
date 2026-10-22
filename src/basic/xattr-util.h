/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цео getxattr_at_malloc(цео fd, const char *path, const char *name, цео at_flags, char **ret, size_t *ret_size);
static inline цео getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        врати getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline цео lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        врати getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline цео fgetxattr_malloc(цео fd, const char *name, char **ret, size_t *ret_size) {
        врати getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

цео getxattr_at_bool(цео fd, const char *path, const char *name, цео at_flags);
цео getxattr_at_strv(цео fd, const char *path, const char *name, цео at_flags, char ***ret_strv);

цео listxattr_at_malloc(цео fd, const char *path, цео at_flags, char **ret);
static inline цео listxattr_malloc(const char *path, char **ret) {
        врати listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline цео llistxattr_malloc(const char *path, char **ret) {
        врати listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline цео flistxattr_malloc(цео fd, char **ret) {
        врати listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

цео xsetxattr_full(
                цео fd,
                const char *path,
                цео at_flags,
                const char *name,
                const char *value,
                size_t size,
                цео xattr_flags);
static inline цео xsetxattr(
                цео fd,
                const char *path,
                цео at_flags,
                const char *name,
                const char *value) {
        врати xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

цео xsetxattr_strv(цео fd, const char *path, цео at_flags, const char *name, char * const *l);

цео xremovexattr(цео fd, const char *path, цео at_flags, const char *name);

цео fd_setcrtime(цео fd, usec_t usec);
цео getcrtime_at(цео fd, const char *path, цео at_flags, usec_t *ret);
static inline цео fd_getcrtime(цео fd, usec_t *ret) {
        врати getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
