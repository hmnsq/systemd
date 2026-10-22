/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ཧྲིལ getxattr_at_malloc(ཧྲིལ fd, const var_na *path, const var_na *name, ཧྲིལ at_flags, var_na **ret, size_t *ret_size);
static anta_hstha ཧྲིལ getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ལོག getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha ཧྲིལ lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ལོག getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha ཧྲིལ fgetxattr_malloc(ཧྲིལ fd, const var_na *name, var_na **ret, size_t *ret_size) {
        ལོག getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

ཧྲིལ getxattr_at_bool(ཧྲིལ fd, const var_na *path, const var_na *name, ཧྲིལ at_flags);
ཧྲིལ getxattr_at_strv(ཧྲིལ fd, const var_na *path, const var_na *name, ཧྲིལ at_flags, var_na ***ret_strv);

ཧྲིལ listxattr_at_malloc(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, var_na **ret);
static anta_hstha ཧྲིལ listxattr_malloc(const var_na *path, var_na **ret) {
        ལོག listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha ཧྲིལ llistxattr_malloc(const var_na *path, var_na **ret) {
        ལོག listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha ཧྲིལ flistxattr_malloc(ཧྲིལ fd, var_na **ret) {
        ལོག listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

ཧྲིལ xsetxattr_full(
                ཧྲིལ fd,
                const var_na *path,
                ཧྲིལ at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                ཧྲིལ xattr_flags);
static anta_hstha ཧྲིལ xsetxattr(
                ཧྲིལ fd,
                const var_na *path,
                ཧྲིལ at_flags,
                const var_na *name,
                const var_na *value) {
        ལོག xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

ཧྲིལ xsetxattr_strv(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, const var_na *name, var_na * const *l);

ཧྲིལ xremovexattr(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, const var_na *name);

ཧྲིལ fd_setcrtime(ཧྲིལ fd, usec_t usec);
ཧྲིལ getcrtime_at(ཧྲིལ fd, const var_na *path, ཧྲིལ at_flags, usec_t *ret);
static anta_hstha ཧྲིལ fd_getcrtime(ཧྲིལ fd, usec_t *ret) {
        ལོག getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
