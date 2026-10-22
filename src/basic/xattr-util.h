/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

цяло getxattr_at_malloc(цяло fd, const char *path, const char *name, цяло at_flags, char **ret, size_t *ret_size);
static inline цяло getxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        върни getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static inline цяло lgetxattr_malloc(const char *path, const char *name, char **ret, size_t *ret_size) {
        върни getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static inline цяло fgetxattr_malloc(цяло fd, const char *name, char **ret, size_t *ret_size) {
        върни getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

цяло getxattr_at_bool(цяло fd, const char *path, const char *name, цяло at_flags);
цяло getxattr_at_strv(цяло fd, const char *path, const char *name, цяло at_flags, char ***ret_strv);

цяло listxattr_at_malloc(цяло fd, const char *path, цяло at_flags, char **ret);
static inline цяло listxattr_malloc(const char *path, char **ret) {
        върни listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static inline цяло llistxattr_malloc(const char *path, char **ret) {
        върни listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static inline цяло flistxattr_malloc(цяло fd, char **ret) {
        върни listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

цяло xsetxattr_full(
                цяло fd,
                const char *path,
                цяло at_flags,
                const char *name,
                const char *value,
                size_t size,
                цяло xattr_flags);
static inline цяло xsetxattr(
                цяло fd,
                const char *path,
                цяло at_flags,
                const char *name,
                const char *value) {
        върни xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

цяло xsetxattr_strv(цяло fd, const char *path, цяло at_flags, const char *name, char * const *l);

цяло xremovexattr(цяло fd, const char *path, цяло at_flags, const char *name);

цяло fd_setcrtime(цяло fd, usec_t usec);
цяло getcrtime_at(цяло fd, const char *path, цяло at_flags, usec_t *ret);
static inline цяло fd_getcrtime(цяло fd, usec_t *ret) {
        върни getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const char *name);
bool xattr_is_selinux(const char *name);
