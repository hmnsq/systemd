/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع تعداد MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_منوال       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t منوال, LabelContext *label_context);

صحيح mkdirat_محفوظ(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags);
ساكن مضمن صحيح mkdir_محفوظ(ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_محفوظ(AT_FDCWD, رستہ, منوال, uid, gid, flags);
}
صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال);
ساكن مضمن صحيح mkdir_parents(ثابت حرف *رستہ, mode_t منوال) {
        أرجع mkdirat_parents(AT_FDCWD, رستہ, منوال);
}
صحيح mkdir_parents_محفوظ(ثابت حرف *اگیتر, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p(ثابت حرف *رستہ, mode_t منوال);
صحيح mkdir_p_محفوظ(ثابت حرف *اگیتر, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p_جڑ_بھریا(ثابت حرف *جڑ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ساكن مضمن صحيح mkdir_p_جڑ(ثابت حرف *جڑ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m) {
        أرجع mkdir_p_جڑ_بھریا(جڑ, p, uid, gid, m, USEC_INFINITY, عدم);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
تعريفنوع صحيح (*mkdirat_func_t)(صحيح dir_fd, ثابت حرف *pathname, mode_t منوال, LabelContext *label_context);
صحيح mkdirat_محفوظ_اندرونی(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ساكن مضمن صحيح mkdir_محفوظ_اندرونی(ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        أرجع mkdirat_محفوظ_اندرونی(AT_FDCWD, رستہ, منوال, uid, gid, flags, _mkdir, label_context);
}
صحيح mkdirat_parents_اندرونی(صحيح dir_fd, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحيح mkdir_parents_اندرونی(ثابت حرف *اگیتر, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحيح mkdir_p_اندرونی(ثابت حرف *اگیتر, ثابت حرف *رستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
