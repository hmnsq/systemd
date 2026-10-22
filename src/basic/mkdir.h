/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع تعداد MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_گهڻائي       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t گهڻائي, LabelContext *label_context);

صحيح mkdirat_محفوظ(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags);
ساكن مضمن صحيح mkdir_محفوظ(ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_محفوظ(AT_FDCWD, رستو, گهڻائي, uid, gid, flags);
}
صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي);
ساكن مضمن صحيح mkdir_parents(ثابت حرف *رستو, mode_t گهڻائي) {
        أرجع mkdirat_parents(AT_FDCWD, رستو, گهڻائي);
}
صحيح mkdir_parents_محفوظ(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p(ثابت حرف *رستو, mode_t گهڻائي);
صحيح mkdir_p_محفوظ(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags);
صحيح mkdir_p_پاڙ_ڀريل(ثابت حرف *پاڙ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ساكن مضمن صحيح mkdir_p_پاڙ(ثابت حرف *پاڙ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m) {
        أرجع mkdir_p_پاڙ_ڀريل(پاڙ, p, uid, gid, m, USEC_INFINITY, عدم);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
تعريفنوع صحيح (*mkdirat_func_t)(صحيح dir_fd, ثابت حرف *pathname, mode_t گهڻائي, LabelContext *label_context);
صحيح mkdirat_محفوظ_اندروني(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ساكن مضمن صحيح mkdir_محفوظ_اندروني(ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        أرجع mkdirat_محفوظ_اندروني(AT_FDCWD, رستو, گهڻائي, uid, gid, flags, _mkdir, label_context);
}
صحيح mkdirat_parents_اندروني(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحيح mkdir_parents_اندروني(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحيح mkdir_p_اندروني(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
