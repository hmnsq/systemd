/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

முழு mkdirat_errno_wrapper(முழு dirfd, மாறிலி எழுத்து *pathname, mode_t mode, LabelContext *label_context);

முழு mkdirat_safe(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
நிலையான உள்வரி முழு mkdir_safe(மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        திரும்பு mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
முழு mkdirat_parents(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode);
நிலையான உள்வரி முழு mkdir_parents(மாறிலி எழுத்து *path, mode_t mode) {
        திரும்பு mkdirat_parents(AT_FDCWD, path, mode);
}
முழு mkdir_parents_safe(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
முழு mkdir_p(மாறிலி எழுத்து *path, mode_t mode);
முழு mkdir_p_safe(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
முழு mkdir_p_வேர்_full(மாறிலி எழுத்து *வேர், மாறிலி எழுத்து *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
நிலையான உள்வரி முழு mkdir_p_வேர்(மாறிலி எழுத்து *வேர், மாறிலி எழுத்து *p, uid_t uid, gid_t gid, mode_t m) {
        திரும்பு mkdir_p_வேர்_full(வேர், p, uid, gid, m, USEC_INFINITY, வெற்று);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
வகைவரை முழு (*mkdirat_func_t)(முழு dir_fd, மாறிலி எழுத்து *pathname, mode_t mode, LabelContext *label_context);
முழு mkdirat_safe_internal(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
நிலையான உள்வரி முழு mkdir_safe_internal(மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        திரும்பு mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
முழு mkdirat_parents_internal(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
முழு mkdir_parents_internal(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
முழு mkdir_p_internal(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
