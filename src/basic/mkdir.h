/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع تعداد MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_ډېرپېښ       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t ډېرپېښ, LabelContext *label_context);

صحيح mkdirat_خوندي(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags);
ساكن مضمن صحيح mkdir_خوندي(ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_خوندي(AT_FDCWD, لار, ډېرپېښ, uid, gid, flags);
}
صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ);
ساكن مضمن صحيح mkdir_parents(ثابت حرف *لار, mode_t ډېرپېښ) {
        أرجع mkdirat_parents(AT_FDCWD, لار, ډېرپېښ);
}
صحيح mkdir_parents_خوندي(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p(ثابت حرف *لار, mode_t ډېرپېښ);
صحيح mkdir_p_خوندي(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p_ریښه_ډک(ثابت حرف *ریښه, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ساكن مضمن صحيح mkdir_p_ریښه(ثابت حرف *ریښه, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m) {
        أرجع mkdir_p_ریښه_ډک(ریښه, p, uid, gid, m, USEC_INFINITY, عدم);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
تعريفنوع صحيح (*mkdirat_func_t)(صحيح dir_fd, ثابت حرف *pathname, mode_t ډېرپېښ, LabelContext *label_context);
صحيح mkdirat_خوندي_دننی(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ساكن مضمن صحيح mkdir_خوندي_دننی(ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        أرجع mkdirat_خوندي_دننی(AT_FDCWD, لار, ډېرپېښ, uid, gid, flags, _mkdir, label_context);
}
صحيح mkdirat_parents_دننی(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحيح mkdir_parents_دننی(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحيح mkdir_p_دننی(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
