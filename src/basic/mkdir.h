/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

ಪೂರ್ಣಾಙ್ಕ mkdirat_errno_wrapper(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *pathname, mode_t mode, LabelContext *label_context);

ಪೂರ್ಣಾಙ್ಕ mkdirat_safe(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ mkdir_safe(ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಪ್ರತ್ಯಾಗಮ mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
ಪೂರ್ಣಾಙ್ಕ mkdirat_parents(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ mkdir_parents(ಧ್ರುವ ವರ್ಣ *path, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ mkdirat_parents(AT_FDCWD, path, mode);
}
ಪೂರ್ಣಾಙ್ಕ mkdir_parents_safe(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ಪೂರ್ಣಾಙ್ಕ mkdir_p(ಧ್ರುವ ವರ್ಣ *path, mode_t mode);
ಪೂರ್ಣಾಙ್ಕ mkdir_p_safe(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ಪೂರ್ಣಾಙ್ಕ mkdir_p_ಬೇರು_full(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ mkdir_p_ಬೇರು(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ *p, uid_t uid, gid_t gid, mode_t m) {
        ಪ್ರತ್ಯಾಗಮ mkdir_p_ಬೇರು_full(ಬೇರು, p, uid, gid, m, USEC_INFINITY, ರಿಕ್ತ);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*mkdirat_func_t)(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *pathname, mode_t mode, LabelContext *label_context);
ಪೂರ್ಣಾಙ್ಕ mkdirat_safe_internal(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ mkdir_safe_internal(ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ಪ್ರತ್ಯಾಗಮ mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
ಪೂರ್ಣಾಙ್ಕ mkdirat_parents_internal(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
ಪೂರ್ಣಾಙ್ಕ mkdir_parents_internal(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ಪೂರ್ಣಾಙ್ಕ mkdir_p_internal(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
