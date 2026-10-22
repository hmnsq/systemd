/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

পূর্ণসংখ্যা mkdirat_errno_wrapper(পূর্ণসংখ্যা dirfd, ধ্রুবক অক্ষর *pathname, mode_t mode, LabelContext *label_context);

পূর্ণসংখ্যা mkdirat_safe(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা mkdir_safe(ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ফেরত mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
পূর্ণসংখ্যা mkdirat_parents(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode);
স্থির অন্তঃরেখ পূর্ণসংখ্যা mkdir_parents(ধ্রুবক অক্ষর *path, mode_t mode) {
        ফেরত mkdirat_parents(AT_FDCWD, path, mode);
}
পূর্ণসংখ্যা mkdir_parents_safe(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
পূর্ণসংখ্যা mkdir_p(ধ্রুবক অক্ষর *path, mode_t mode);
পূর্ণসংখ্যা mkdir_p_safe(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
পূর্ণসংখ্যা mkdir_p_মূল_full(ধ্রুবক অক্ষর *মূল, ধ্রুবক অক্ষর *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
স্থির অন্তঃরেখ পূর্ণসংখ্যা mkdir_p_মূল(ধ্রুবক অক্ষর *মূল, ধ্রুবক অক্ষর *p, uid_t uid, gid_t gid, mode_t m) {
        ফেরত mkdir_p_মূল_full(মূল, p, uid, gid, m, USEC_INFINITY, ফাঁকা);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
প্রকারলেখ পূর্ণসংখ্যা (*mkdirat_func_t)(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *pathname, mode_t mode, LabelContext *label_context);
পূর্ণসংখ্যা mkdirat_safe_internal(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
স্থির অন্তঃরেখ পূর্ণসংখ্যা mkdir_safe_internal(ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ফেরত mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
পূর্ণসংখ্যা mkdirat_parents_internal(পূর্ণসংখ্যা dir_fd, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
পূর্ণসংখ্যা mkdir_parents_internal(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
পূর্ণসংখ্যা mkdir_p_internal(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
