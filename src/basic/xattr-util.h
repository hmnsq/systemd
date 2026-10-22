/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ຈຳນວນເຕັມ getxattr_at_malloc(ຈຳນວນເຕັມ fd, const var_na *path, const var_na *name, ຈຳນວນເຕັມ at_flags, var_na **ret, size_t *ret_size);
static anta_hstha ຈຳນວນເຕັມ getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ສົ່ງຄືນ getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha ຈຳນວນເຕັມ lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ສົ່ງຄືນ getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha ຈຳນວນເຕັມ fgetxattr_malloc(ຈຳນວນເຕັມ fd, const var_na *name, var_na **ret, size_t *ret_size) {
        ສົ່ງຄືນ getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

ຈຳນວນເຕັມ getxattr_at_bool(ຈຳນວນເຕັມ fd, const var_na *path, const var_na *name, ຈຳນວນເຕັມ at_flags);
ຈຳນວນເຕັມ getxattr_at_strv(ຈຳນວນເຕັມ fd, const var_na *path, const var_na *name, ຈຳນວນເຕັມ at_flags, var_na ***ret_strv);

ຈຳນວນເຕັມ listxattr_at_malloc(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, var_na **ret);
static anta_hstha ຈຳນວນເຕັມ listxattr_malloc(const var_na *path, var_na **ret) {
        ສົ່ງຄືນ listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha ຈຳນວນເຕັມ llistxattr_malloc(const var_na *path, var_na **ret) {
        ສົ່ງຄືນ listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha ຈຳນວນເຕັມ flistxattr_malloc(ຈຳນວນເຕັມ fd, var_na **ret) {
        ສົ່ງຄືນ listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

ຈຳນວນເຕັມ xsetxattr_full(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                ຈຳນວນເຕັມ at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                ຈຳນວນເຕັມ xattr_flags);
static anta_hstha ຈຳນວນເຕັມ xsetxattr(
                ຈຳນວນເຕັມ fd,
                const var_na *path,
                ຈຳນວນເຕັມ at_flags,
                const var_na *name,
                const var_na *value) {
        ສົ່ງຄືນ xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

ຈຳນວນເຕັມ xsetxattr_strv(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, const var_na *name, var_na * const *l);

ຈຳນວນເຕັມ xremovexattr(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, const var_na *name);

ຈຳນວນເຕັມ fd_setcrtime(ຈຳນວນເຕັມ fd, usec_t usec);
ຈຳນວນເຕັມ getcrtime_at(ຈຳນວນເຕັມ fd, const var_na *path, ຈຳນວນເຕັມ at_flags, usec_t *ret);
static anta_hstha ຈຳນວນເຕັມ fd_getcrtime(ຈຳນວນເຕັມ fd, usec_t *ret) {
        ສົ່ງຄືນ getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
