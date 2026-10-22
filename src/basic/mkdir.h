/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef enum MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

整数 mkdirat_errno_wrapper(整数 dirfd, const char *pathname, mode_t mode, LabelContext *label_context);

整数 mkdirat_safe(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
static inline 整数 mkdir_safe(const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        戻る mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
整数 mkdirat_parents(整数 dir_fd, const char *path, mode_t mode);
static inline 整数 mkdir_parents(const char *path, mode_t mode) {
        戻る mkdirat_parents(AT_FDCWD, path, mode);
}
整数 mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
整数 mkdir_p(const char *path, mode_t mode);
整数 mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
整数 mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
static inline 整数 mkdir_p_root(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m) {
        戻る mkdir_p_root_full(root, p, uid, gid, m, USEC_INFINITY, NULL);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
typedef 整数 (*mkdirat_func_t)(整数 dir_fd, const char *pathname, mode_t mode, LabelContext *label_context);
整数 mkdirat_safe_internal(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
static inline 整数 mkdir_safe_internal(const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        戻る mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
整数 mkdirat_parents_internal(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
整数 mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
整数 mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
