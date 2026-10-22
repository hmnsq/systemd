/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع تعداد MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t mode, LabelContext *label_context);

صحيح mkdirat_safe(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ساكن مضمن صحيح mkdir_safe(ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *path, mode_t mode);
ساكن مضمن صحيح mkdir_parents(ثابت حرف *path, mode_t mode) {
        أرجع mkdirat_parents(AT_FDCWD, path, mode);
}
صحيح mkdir_parents_safe(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p(ثابت حرف *path, mode_t mode);
صحيح mkdir_p_safe(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p_يىلتىز_full(ثابت حرف *يىلتىز, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ساكن مضمن صحيح mkdir_p_يىلتىز(ثابت حرف *يىلتىز, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m) {
        أرجع mkdir_p_يىلتىز_full(يىلتىز, p, uid, gid, m, USEC_INFINITY, عدم);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
تعريفنوع صحيح (*mkdirat_func_t)(صحيح dir_fd, ثابت حرف *pathname, mode_t mode, LabelContext *label_context);
صحيح mkdirat_safe_internal(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ساكن مضمن صحيح mkdir_safe_internal(ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        أرجع mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
صحيح mkdirat_parents_internal(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحيح mkdir_parents_internal(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحيح mkdir_p_internal(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
