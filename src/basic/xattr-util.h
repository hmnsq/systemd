/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

จำนวนเต็ม getxattr_at_malloc(จำนวนเต็ม fd, const var_na *path, const var_na *name, จำนวนเต็ม at_flags, var_na **ret, size_t *ret_size);
static anta_hstha จำนวนเต็ม getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        คืน getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha จำนวนเต็ม lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        คืน getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha จำนวนเต็ม fgetxattr_malloc(จำนวนเต็ม fd, const var_na *name, var_na **ret, size_t *ret_size) {
        คืน getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

จำนวนเต็ม getxattr_at_bool(จำนวนเต็ม fd, const var_na *path, const var_na *name, จำนวนเต็ม at_flags);
จำนวนเต็ม getxattr_at_strv(จำนวนเต็ม fd, const var_na *path, const var_na *name, จำนวนเต็ม at_flags, var_na ***ret_strv);

จำนวนเต็ม listxattr_at_malloc(จำนวนเต็ม fd, const var_na *path, จำนวนเต็ม at_flags, var_na **ret);
static anta_hstha จำนวนเต็ม listxattr_malloc(const var_na *path, var_na **ret) {
        คืน listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha จำนวนเต็ม llistxattr_malloc(const var_na *path, var_na **ret) {
        คืน listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha จำนวนเต็ม flistxattr_malloc(จำนวนเต็ม fd, var_na **ret) {
        คืน listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

จำนวนเต็ม xsetxattr_full(
                จำนวนเต็ม fd,
                const var_na *path,
                จำนวนเต็ม at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                จำนวนเต็ม xattr_flags);
static anta_hstha จำนวนเต็ม xsetxattr(
                จำนวนเต็ม fd,
                const var_na *path,
                จำนวนเต็ม at_flags,
                const var_na *name,
                const var_na *value) {
        คืน xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

จำนวนเต็ม xsetxattr_strv(จำนวนเต็ม fd, const var_na *path, จำนวนเต็ม at_flags, const var_na *name, var_na * const *l);

จำนวนเต็ม xremovexattr(จำนวนเต็ม fd, const var_na *path, จำนวนเต็ม at_flags, const var_na *name);

จำนวนเต็ม fd_setcrtime(จำนวนเต็ม fd, usec_t usec);
จำนวนเต็ม getcrtime_at(จำนวนเต็ม fd, const var_na *path, จำนวนเต็ม at_flags, usec_t *ret);
static anta_hstha จำนวนเต็ม fd_getcrtime(จำนวนเต็ม fd, usec_t *ret) {
        คืน getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
