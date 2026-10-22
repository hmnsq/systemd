/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

ཧྲིལ mkdirat_errno_wrapper(ཧྲིལ dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context);

ཧྲིལ mkdirat_safe(ཧྲིལ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
static anta_hstha ཧྲིལ mkdir_safe(const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ལོག mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
ཧྲིལ mkdirat_parents(ཧྲིལ dir_fd, const var_na *path, mode_t mode);
static anta_hstha ཧྲིལ mkdir_parents(const var_na *path, mode_t mode) {
        ལོག mkdirat_parents(AT_FDCWD, path, mode);
}
ཧྲིལ mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ཧྲིལ mkdir_p(const var_na *path, mode_t mode);
ཧྲིལ mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ཧྲིལ mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
static anta_hstha ཧྲིལ mkdir_p_root(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m) {
        ལོག mkdir_p_root_full(root, p, uid, gid, m, USEC_INFINITY, NULL);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
typedef ཧྲིལ (*mkdirat_func_t)(ཧྲིལ dir_fd, const var_na *pathname, mode_t mode, LabelContext *label_context);
ཧྲིལ mkdirat_safe_internal(ཧྲིལ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
static anta_hstha ཧྲིལ mkdir_safe_internal(const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ལོག mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
ཧྲིལ mkdirat_parents_internal(ཧྲིལ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
ཧྲིལ mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ཧྲིལ mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
