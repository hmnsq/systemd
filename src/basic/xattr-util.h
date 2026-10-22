/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ចំនួនគត់ getxattr_at_malloc(ចំនួនគត់ fd, const var_na *path, const var_na *name, ចំនួនគត់ at_flags, var_na **ret, size_t *ret_size);
static anta_hstha ចំនួនគត់ getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ត្រឡប់ getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha ចំនួនគត់ lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ត្រឡប់ getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha ចំនួនគត់ fgetxattr_malloc(ចំនួនគត់ fd, const var_na *name, var_na **ret, size_t *ret_size) {
        ត្រឡប់ getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

ចំនួនគត់ getxattr_at_bool(ចំនួនគត់ fd, const var_na *path, const var_na *name, ចំនួនគត់ at_flags);
ចំនួនគត់ getxattr_at_strv(ចំនួនគត់ fd, const var_na *path, const var_na *name, ចំនួនគត់ at_flags, var_na ***ret_strv);

ចំនួនគត់ listxattr_at_malloc(ចំនួនគត់ fd, const var_na *path, ចំនួនគត់ at_flags, var_na **ret);
static anta_hstha ចំនួនគត់ listxattr_malloc(const var_na *path, var_na **ret) {
        ត្រឡប់ listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha ចំនួនគត់ llistxattr_malloc(const var_na *path, var_na **ret) {
        ត្រឡប់ listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha ចំនួនគត់ flistxattr_malloc(ចំនួនគត់ fd, var_na **ret) {
        ត្រឡប់ listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

ចំនួនគត់ xsetxattr_full(
                ចំនួនគត់ fd,
                const var_na *path,
                ចំនួនគត់ at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                ចំនួនគត់ xattr_flags);
static anta_hstha ចំនួនគត់ xsetxattr(
                ចំនួនគត់ fd,
                const var_na *path,
                ចំនួនគត់ at_flags,
                const var_na *name,
                const var_na *value) {
        ត្រឡប់ xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

ចំនួនគត់ xsetxattr_strv(ចំនួនគត់ fd, const var_na *path, ចំនួនគត់ at_flags, const var_na *name, var_na * const *l);

ចំនួនគត់ xremovexattr(ចំនួនគត់ fd, const var_na *path, ចំនួនគត់ at_flags, const var_na *name);

ចំនួនគត់ fd_setcrtime(ចំនួនគត់ fd, usec_t usec);
ចំនួនគត់ getcrtime_at(ចំនួនគត់ fd, const var_na *path, ចំនួនគត់ at_flags, usec_t *ret);
static anta_hstha ចំនួនគត់ fd_getcrtime(ចំនួនគត់ fd, usec_t *ret) {
        ត្រឡប់ getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
