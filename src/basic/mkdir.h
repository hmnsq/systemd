/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع شمارش MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحیح mkdirat_errno_wrapper(صحیح dirfd, ثابت نویسه *pathname, mode_t mode, LabelContext *label_context);

صحیح mkdirat_safe(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ایستا درونخطی صحیح mkdir_safe(ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        بازگشت mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
صحیح mkdirat_parents(صحیح dir_fd, ثابت نویسه *path, mode_t mode);
ایستا درونخطی صحیح mkdir_parents(ثابت نویسه *path, mode_t mode) {
        بازگشت mkdirat_parents(AT_FDCWD, path, mode);
}
صحیح mkdir_parents_safe(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
صحیح mkdir_p(ثابت نویسه *path, mode_t mode);
صحیح mkdir_p_safe(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
صحیح mkdir_p_ریشه_full(ثابت نویسه *ریشه, ثابت نویسه *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ایستا درونخطی صحیح mkdir_p_ریشه(ثابت نویسه *ریشه, ثابت نویسه *p, uid_t uid, gid_t gid, mode_t m) {
        بازگشت mkdir_p_ریشه_full(ریشه, p, uid, gid, m, USEC_INFINITY, پوچ);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
تعریفنوع صحیح (*mkdirat_func_t)(صحیح dir_fd, ثابت نویسه *pathname, mode_t mode, LabelContext *label_context);
صحیح mkdirat_safe_internal(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ایستا درونخطی صحیح mkdir_safe_internal(ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        بازگشت mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
صحیح mkdirat_parents_internal(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحیح mkdir_parents_internal(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحیح mkdir_p_internal(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
