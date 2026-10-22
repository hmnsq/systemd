/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

puur_naa_nka getxattr_at_malloc(puur_naa_nka fd, const var_na *path, const var_na *name, puur_naa_nka at_flags, var_na **ret, size_t *ret_size);
static anta_hstha puur_naa_nka getxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ᰀᰧᰌ getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
static anta_hstha puur_naa_nka lgetxattr_malloc(const var_na *path, const var_na *name, var_na **ret, size_t *ret_size) {
        ᰀᰧᰌ getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
static anta_hstha puur_naa_nka fgetxattr_malloc(puur_naa_nka fd, const var_na *name, var_na **ret, size_t *ret_size) {
        ᰀᰧᰌ getxattr_at_malloc(fd, NULL, name, AT_EMPTY_PATH, ret, ret_size);
}

puur_naa_nka getxattr_at_bool(puur_naa_nka fd, const var_na *path, const var_na *name, puur_naa_nka at_flags);
puur_naa_nka getxattr_at_strv(puur_naa_nka fd, const var_na *path, const var_na *name, puur_naa_nka at_flags, var_na ***ret_strv);

puur_naa_nka listxattr_at_malloc(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, var_na **ret);
static anta_hstha puur_naa_nka listxattr_malloc(const var_na *path, var_na **ret) {
        ᰀᰧᰌ listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
static anta_hstha puur_naa_nka llistxattr_malloc(const var_na *path, var_na **ret) {
        ᰀᰧᰌ listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
static anta_hstha puur_naa_nka flistxattr_malloc(puur_naa_nka fd, var_na **ret) {
        ᰀᰧᰌ listxattr_at_malloc(fd, NULL, AT_EMPTY_PATH, ret);
}

puur_naa_nka xsetxattr_full(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka at_flags,
                const var_na *name,
                const var_na *value,
                size_t size,
                puur_naa_nka xattr_flags);
static anta_hstha puur_naa_nka xsetxattr(
                puur_naa_nka fd,
                const var_na *path,
                puur_naa_nka at_flags,
                const var_na *name,
                const var_na *value) {
        ᰀᰧᰌ xsetxattr_full(fd, path, at_flags, name, value, SIZE_MAX, 0);
}

puur_naa_nka xsetxattr_strv(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, const var_na *name, var_na * const *l);

puur_naa_nka xremovexattr(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, const var_na *name);

puur_naa_nka fd_setcrtime(puur_naa_nka fd, usec_t usec);
puur_naa_nka getcrtime_at(puur_naa_nka fd, const var_na *path, puur_naa_nka at_flags, usec_t *ret);
static anta_hstha puur_naa_nka fd_getcrtime(puur_naa_nka fd, usec_t *ret) {
        ᰀᰧᰌ getcrtime_at(fd, NULL, 0, ret);
}

bool xattr_is_acl(const var_na *name);
bool xattr_is_selinux(const var_na *name);
