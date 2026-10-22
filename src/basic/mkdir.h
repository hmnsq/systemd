/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

पूर्णाङ्क mkdirat_errno_wrapper(पूर्णाङ्क dirfd, अचर वर्ण *pathname, mode_t mode, LabelContext *label_context);

पूर्णाङ्क mkdirat_safe(पूर्णाङ्क dir_fd, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
स्थिर अन्तःस्थ पूर्णाङ्क mkdir_safe(अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        फर्काऊ mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
पूर्णाङ्क mkdirat_parents(पूर्णाङ्क dir_fd, अचर वर्ण *path, mode_t mode);
स्थिर अन्तःस्थ पूर्णाङ्क mkdir_parents(अचर वर्ण *path, mode_t mode) {
        फर्काऊ mkdirat_parents(AT_FDCWD, path, mode);
}
पूर्णाङ्क mkdir_parents_safe(अचर वर्ण *prefix, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
पूर्णाङ्क mkdir_p(अचर वर्ण *path, mode_t mode);
पूर्णाङ्क mkdir_p_safe(अचर वर्ण *prefix, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
पूर्णाङ्क mkdir_p_जरा_full(अचर वर्ण *जरा, अचर वर्ण *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
स्थिर अन्तःस्थ पूर्णाङ्क mkdir_p_जरा(अचर वर्ण *जरा, अचर वर्ण *p, uid_t uid, gid_t gid, mode_t m) {
        फर्काऊ mkdir_p_जरा_full(जरा, p, uid, gid, m, USEC_INFINITY, रिक्त);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
प्रकारनिर्देश पूर्णाङ्क (*mkdirat_func_t)(पूर्णाङ्क dir_fd, अचर वर्ण *pathname, mode_t mode, LabelContext *label_context);
पूर्णाङ्क mkdirat_safe_internal(पूर्णाङ्क dir_fd, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
स्थिर अन्तःस्थ पूर्णाङ्क mkdir_safe_internal(अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        फर्काऊ mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
पूर्णाङ्क mkdirat_parents_internal(पूर्णाङ्क dir_fd, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
पूर्णाङ्क mkdir_parents_internal(अचर वर्ण *prefix, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
पूर्णाङ्क mkdir_p_internal(अचर वर्ण *prefix, अचर वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
