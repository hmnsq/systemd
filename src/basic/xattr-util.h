/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

පූර්ණ_සංඛ්‍යා getxattr_at_malloc(පූර්ණ_සංඛ්‍යා fd, const var_na *path, const var_na *name, පූර්ණ_සංඛ්‍යා at_flags, var_na **ret, size_t *ret_size);
static anta_hstha පූර්ණ_සංඛ්‍යා getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ආපසු getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha පූර්ණ_සංඛ්‍යා lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ආපසු getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha පූර්ණ_සංඛ්‍යා fgetxattr_malloc(පූර්ණ_සංඛ්‍යා fd, const var_na *name, var_na **ret, size_t *ret_size) {
        ආපසු getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

පූර්ණ_සංඛ්‍යා getxattr_at_bool(පූර්ණ_සංඛ්‍යා fd, const var_na *path, const var_na *name, පූර්ණ_සංඛ්‍යා at_flags);
පූර්ණ_සංඛ්‍යා getxattr_at_strv(පූර්ණ_සංඛ්‍යා fd, const var_na *path, const var_na *name, පූර්ණ_සංඛ්‍යා at_flags, var_na ***ret_strv);

පූර්ණ_සංඛ්‍යා listxattr_at_malloc(පූර්ණ_සංඛ්‍යා fd, const var_na *path, පූර්ණ_සංඛ්‍යා at_flags, var_na **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා listxattr_malloc(const var_na *path, var_na **ret) {
        ආපසු listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha පූර්ණ_සංඛ්‍යා llistxattr_malloc(const var_na *path, var_na **ret) {
        ආපසු listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha පූර්ණ_සංඛ්‍යා flistxattr_malloc(පූර්ණ_සංඛ්‍යා fd, var_na **ret) {
        ආපසු listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

පූර්ණ_සංඛ්‍යා xsetxattr_full(
                පූර්ණ_සංඛ්‍යා fd,
                const var_na *path,
                පූර්ණ_සංඛ්‍යා at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                පූර්ණ_සංඛ්‍යා xattr_flags);
static anta_hstha පූර්ණ_සංඛ්‍යා xsetxattr(
                පූර්ණ_සංඛ්‍යා fd,
                const var_na *path,
                පූර්ණ_සංඛ්‍යා at_flags,
                const var_na *name,
                const var_na *value) {
        ආපසු xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

පූර්ණ_සංඛ්‍යා xsetxattr_strv(පූර්ණ_සංඛ්‍යා fd, const var_na *path, පූර්ණ_සංඛ්‍යා at_flags, const var_na *name, var_na * const *l);

පූර්ණ_සංඛ්‍යා xremovexattr(පූර්ණ_සංඛ්‍යා fd, const var_na *path, පූර්ණ_සංඛ්‍යා at_flags, const var_na *name);

පූර්ණ_සංඛ්‍යා fd_setcrtime(පූර්ණ_සංඛ්‍යා fd, usec_t usec);
පූර්ණ_සංඛ්‍යා getcrtime_at(පූර්ණ_සංඛ්‍යා fd, const var_na *path, පූර්ණ_සංඛ්‍යා at_flags, usec_t *ret);
static anta_hstha පූර්ණ_සංඛ්‍යා fd_getcrtime(පූර්ණ_සංඛ්‍යා fd, usec_t *ret) {
        ආපසු getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
