/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

จำนวนเต็ม mkdirat_errno_wrapper(จำนวนเต็ม dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context);

จำนวนเต็ม mkdirat_safe(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
static anta_hstha จำนวนเต็ม mkdir_safe(const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        คืน mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
จำนวนเต็ม mkdirat_parents(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode);
static anta_hstha จำนวนเต็ม mkdir_parents(const var_na *path, mode_t mode) {
        คืน mkdirat_parents(AT_FDCWD, path, mode);
}
จำนวนเต็ม mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
จำนวนเต็ม mkdir_p(const var_na *path, mode_t mode);
จำนวนเต็ม mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
จำนวนเต็ม mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
static anta_hstha จำนวนเต็ม mkdir_p_root(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m) {
        คืน mkdir_p_root_full(root, p, uid, gid, m, USEC_INFINITY, NULL);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
typedef จำนวนเต็ม (*mkdirat_func_t)(จำนวนเต็ม dir_fd, const var_na *pathname, mode_t mode, LabelContext *label_context);
จำนวนเต็ม mkdirat_safe_internal(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
static anta_hstha จำนวนเต็ม mkdir_safe_internal(const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        คืน mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
จำนวนเต็ม mkdirat_parents_internal(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
จำนวนเต็ม mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
จำนวนเต็ม mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
