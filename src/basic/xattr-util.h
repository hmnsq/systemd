/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

ကိန်းပြည့် getxattr_at_malloc(ကိန်းပြည့် fd, const var_na *path, const var_na *name, ကိန်းပြည့် at_flags, var_na **ret, size_t *ret_size);
static anta_hstha ကိန်းပြည့် getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ပြန်ပေး getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha ကိန်းပြည့် lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ပြန်ပေး getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha ကိန်းပြည့် fgetxattr_malloc(ကိန်းပြည့် fd, const var_na *name, var_na **ret, size_t *ret_size) {
        ပြန်ပေး getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

ကိန်းပြည့် getxattr_at_bool(ကိန်းပြည့် fd, const var_na *path, const var_na *name, ကိန်းပြည့် at_flags);
ကိန်းပြည့် getxattr_at_strv(ကိန်းပြည့် fd, const var_na *path, const var_na *name, ကိန်းပြည့် at_flags, var_na ***ret_strv);

ကိန်းပြည့် listxattr_at_malloc(ကိန်းပြည့် fd, const var_na *path, ကိန်းပြည့် at_flags, var_na **ret);
static anta_hstha ကိန်းပြည့် listxattr_malloc(const var_na *path, var_na **ret) {
        ပြန်ပေး listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha ကိန်းပြည့် llistxattr_malloc(const var_na *path, var_na **ret) {
        ပြန်ပေး listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha ကိန်းပြည့် flistxattr_malloc(ကိန်းပြည့် fd, var_na **ret) {
        ပြန်ပေး listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

ကိန်းပြည့် xsetxattr_full(
                ကိန်းပြည့် fd,
                const var_na *path,
                ကိန်းပြည့် at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                ကိန်းပြည့် xattr_flags);
static anta_hstha ကိန်းပြည့် xsetxattr(
                ကိန်းပြည့် fd,
                const var_na *path,
                ကိန်းပြည့် at_flags,
                const var_na *name,
                const var_na *value) {
        ပြန်ပေး xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

ကိန်းပြည့် xsetxattr_strv(ကိန်းပြည့် fd, const var_na *path, ကိန်းပြည့် at_flags, const var_na *name, var_na * const *l);

ကိန်းပြည့် xremovexattr(ကိန်းပြည့် fd, const var_na *path, ကိန်းပြည့် at_flags, const var_na *name);

ကိန်းပြည့် fd_setcrtime(ကိန်းပြည့် fd, usec_t usec);
ကိန်းပြည့် getcrtime_at(ကိန်းပြည့် fd, const var_na *path, ကိန်းပြည့် at_flags, usec_t *ret);
static anta_hstha ကိန်းပြည့် fd_getcrtime(ကိန်းပြည့် fd, usec_t *ret) {
        ပြန်ပေး getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
