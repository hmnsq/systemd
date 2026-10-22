/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_منوال       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

صحیح mkdirat_errno_wrapper(صحیح dirfd, ثابت حرف *pathname, mode_t منوال, LabelContext *label_context);

صحیح mkdirat_محفوظ(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags);
ساکن اندرونی صحیح mkdir_محفوظ(ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags) {
        واپس mkdirat_محفوظ(AT_FDCWD, راستہ, منوال, uid, gid, flags);
}
صحیح mkdirat_parents(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال);
ساکن اندرونی صحیح mkdir_parents(ثابت حرف *راستہ, mode_t منوال) {
        واپس mkdirat_parents(AT_FDCWD, راستہ, منوال);
}
صحیح mkdir_parents_محفوظ(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags);
صحیح mkdir_p(ثابت حرف *راستہ, mode_t منوال);
صحیح mkdir_p_محفوظ(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags);
صحیح mkdir_p_جڑ_بھرا(ثابت حرف *جڑ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ساکن اندرونی صحیح mkdir_p_جڑ(ثابت حرف *جڑ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m) {
        واپس mkdir_p_جڑ_بھرا(جڑ, p, uid, gid, m, USEC_INFINITY, تہی);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
قسمتعریف صحیح (*mkdirat_func_t)(صحیح dir_fd, ثابت حرف *pathname, mode_t منوال, LabelContext *label_context);
صحیح mkdirat_محفوظ_اندرونی(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ساکن اندرونی صحیح mkdir_محفوظ_اندرونی(ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        واپس mkdirat_محفوظ_اندرونی(AT_FDCWD, راستہ, منوال, uid, gid, flags, _mkdir, label_context);
}
صحیح mkdirat_parents_اندرونی(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
صحیح mkdir_parents_اندرونی(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
صحیح mkdir_p_اندرونی(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
