/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या गणना MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

पूर्णांक mkdirat_errno_wrapper(पूर्णांक dirfd, स्थिरांक अक्षर *pathname, mode_t mode, LabelContext *label_context);

पूर्णांक mkdirat_safe(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
स्थिर ओळीत पूर्णांक mkdir_safe(स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        परत mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
पूर्णांक mkdirat_parents(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode);
स्थिर ओळीत पूर्णांक mkdir_parents(स्थिरांक अक्षर *path, mode_t mode) {
        परत mkdirat_parents(AT_FDCWD, path, mode);
}
पूर्णांक mkdir_parents_safe(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
पूर्णांक mkdir_p(स्थिरांक अक्षर *path, mode_t mode);
पूर्णांक mkdir_p_safe(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
पूर्णांक mkdir_p_मूळ_full(स्थिरांक अक्षर *मूळ, स्थिरांक अक्षर *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
स्थिर ओळीत पूर्णांक mkdir_p_मूळ(स्थिरांक अक्षर *मूळ, स्थिरांक अक्षर *p, uid_t uid, gid_t gid, mode_t m) {
        परत mkdir_p_मूळ_full(मूळ, p, uid, gid, m, USEC_INFINITY, रिक्त);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
प्रकारव्याख्या पूर्णांक (*mkdirat_func_t)(पूर्णांक dir_fd, स्थिरांक अक्षर *pathname, mode_t mode, LabelContext *label_context);
पूर्णांक mkdirat_safe_internal(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
स्थिर ओळीत पूर्णांक mkdir_safe_internal(स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        परत mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
पूर्णांक mkdirat_parents_internal(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
पूर्णांक mkdir_parents_internal(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
पूर्णांक mkdir_p_internal(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
