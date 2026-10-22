/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ গণনা MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

পূর্ণাঙ্ক mkdirat_errno_wrapper(পূর্ণাঙ্ক dirfd, ধ্রুৱ ৱর্ণ *pathname, mode_t mode, LabelContext *label_context);

পূর্ণাঙ্ক mkdirat_safe(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক mkdir_safe(ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ঘূৰাই mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
পূর্ণাঙ্ক mkdirat_parents(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode);
স্থির অন্তঃস্থ পূর্ণাঙ্ক mkdir_parents(ধ্রুৱ ৱর্ণ *path, mode_t mode) {
        ঘূৰাই mkdirat_parents(AT_FDCWD, path, mode);
}
পূর্ণাঙ্ক mkdir_parents_safe(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
পূর্ণাঙ্ক mkdir_p(ধ্রুৱ ৱর্ণ *path, mode_t mode);
পূর্ণাঙ্ক mkdir_p_safe(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
পূর্ণাঙ্ক mkdir_p_মূল_full(ধ্রুৱ ৱর্ণ *মূল, ধ্রুৱ ৱর্ণ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
স্থির অন্তঃস্থ পূর্ণাঙ্ক mkdir_p_মূল(ধ্রুৱ ৱর্ণ *মূল, ধ্রুৱ ৱর্ণ *p, uid_t uid, gid_t gid, mode_t m) {
        ঘূৰাই mkdir_p_মূল_full(মূল, p, uid, gid, m, USEC_INFINITY, রিক্ত);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
প্রকারনির্দেশ পূর্ণাঙ্ক (*mkdirat_func_t)(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *pathname, mode_t mode, LabelContext *label_context);
পূর্ণাঙ্ক mkdirat_safe_internal(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
স্থির অন্তঃস্থ পূর্ণাঙ্ক mkdir_safe_internal(ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ঘূৰাই mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
পূর্ণাঙ্ক mkdirat_parents_internal(পূর্ণাঙ্ক dir_fd, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
পূর্ণাঙ্ক mkdir_parents_internal(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
পূর্ণাঙ্ক mkdir_p_internal(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
