/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

số_nguyên getxattr_at_malloc(số_nguyên fd, const char *path, const char *name, số_nguyên at_flags, char **ret, size_t *ret_size);
static inline số_nguyên getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        trả_về getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline số_nguyên lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        trả_về getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline số_nguyên fgetxattr_malloc(số_nguyên fd, const char *name, char **ret, size_t *ret_size) {
        trả_về getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

số_nguyên getxattr_at_bool(số_nguyên fd, const char *path, const char *name, số_nguyên at_flags);
số_nguyên getxattr_at_strv(số_nguyên fd, const char *path, const char *name, số_nguyên at_flags, char ***ret_strv);

số_nguyên listxattr_at_malloc(số_nguyên fd, const char *path, số_nguyên at_flags, char **ret);
static inline số_nguyên listxattr_malloc(const char *path, char **ret) {
        trả_về listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline số_nguyên llistxattr_malloc(const char *path, char **ret) {
        trả_về listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline số_nguyên flistxattr_malloc(số_nguyên fd, char **ret) {
        trả_về listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

số_nguyên xsetxattr_full(
                số_nguyên fd,
                const char *path,
                số_nguyên at_flags,
                const char *name,
                const char *value,
                size_t size,
                số_nguyên xattr_flags);
static inline số_nguyên xsetxattr(
                số_nguyên fd,
                const char *path,
                số_nguyên at_flags,
                const char *name,
                const char *value) {
        trả_về xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

số_nguyên xsetxattr_strv(số_nguyên fd, const char *path, số_nguyên at_flags, const char *name, char * const *l);

số_nguyên xremovexattr(số_nguyên fd, const char *path, số_nguyên at_flags, const char *name);

số_nguyên fd_setcrtime(số_nguyên fd, usec_t usec);
số_nguyên getcrtime_at(số_nguyên fd, const char *path, số_nguyên at_flags, usec_t *ret);
static inline số_nguyên fd_getcrtime(số_nguyên fd, usec_t *ret) {
        trả_về getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
