/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef enum MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

số_nguyên mkdirat_errno_wrapper(số_nguyên dirfd, const char *pathname, mode_t mode, LabelContext *label_context);

số_nguyên mkdirat_safe(số_nguyên dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
static inline số_nguyên mkdir_safe(const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        trả_về mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
số_nguyên mkdirat_parents(số_nguyên dir_fd, const char *path, mode_t mode);
static inline số_nguyên mkdir_parents(const char *path, mode_t mode) {
        trả_về mkdirat_parents(AT_FDCWD, path, mode);
}
số_nguyên mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
số_nguyên mkdir_p(const char *path, mode_t mode);
số_nguyên mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
số_nguyên mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
static inline số_nguyên mkdir_p_root(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m) {
        trả_về mkdir_p_root_full(root, p, uid, gid, m, USEC_INFINITY, NULL);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
typedef số_nguyên (*mkdirat_func_t)(số_nguyên dir_fd, const char *pathname, mode_t mode, LabelContext *label_context);
số_nguyên mkdirat_safe_internal(số_nguyên dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
static inline số_nguyên mkdir_safe_internal(const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        trả_về mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
số_nguyên mkdirat_parents_internal(số_nguyên dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
số_nguyên mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
số_nguyên mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
