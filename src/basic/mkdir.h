/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع تعداد MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_نما       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t نما, LabelContext *label_context);

صحيح mkdirat_امن(صحيح dir_fd, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags);
ساكن مضمن صحيح mkdir_امن(ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_امن(AT_FDCWD, مسیر, نما, uid, gid, flags);
}
صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *مسیر, mode_t نما);
ساكن مضمن صحيح mkdir_parents(ثابت حرف *مسیر, mode_t نما) {
        أرجع mkdirat_parents(AT_FDCWD, مسیر, نما);
}
صحيح mkdir_parents_امن(ثابت حرف *پیشوند, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p(ثابت حرف *مسیر, mode_t نما);
صحيح mkdir_p_امن(ثابت حرف *پیشوند, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p_ریشه_پر(ثابت حرف *ریشه, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ساكن مضمن صحيح mkdir_p_ریشه(ثابت حرف *ریشه, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m) {
        أرجع mkdir_p_ریشه_پر(ریشه, p, uid, gid, m, USEC_INFINITY, عدم);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
تعريفنوع صحيح (*mkdirat_func_t)(صحيح dir_fd, ثابت حرف *pathname, mode_t نما, LabelContext *label_context);
صحيح mkdirat_امن_داخلی(صحيح dir_fd, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ساكن مضمن صحيح mkdir_امن_داخلی(ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        أرجع mkdirat_امن_داخلی(AT_FDCWD, مسیر, نما, uid, gid, flags, _mkdir, label_context);
}
صحيح mkdirat_parents_داخلی(صحيح dir_fd, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحيح mkdir_parents_داخلی(ثابت حرف *پیشوند, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحيح mkdir_p_داخلی(ثابت حرف *پیشوند, ثابت حرف *مسیر, mode_t نما, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
