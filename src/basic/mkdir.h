/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_ਬਹੁਲਕ       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

ਪੂਰਨ mkdirat_errno_wrapper(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *pathname, mode_t ਬਹੁਲਕ, LabelContext *label_context);

ਪੂਰਨ mkdirat_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ mkdir_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ਵਾਪਸ mkdirat_ਸੁਰੱਖਿਅਤ(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags);
}
ਪੂਰਨ mkdirat_parents(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ mkdir_parents(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ mkdirat_parents(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ);
}
ਪੂਰਨ mkdir_parents_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags);
ਪੂਰਨ mkdir_p(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ);
ਪੂਰਨ mkdir_p_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags);
ਪੂਰਨ mkdir_p_ਜੜ੍ਹ_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ mkdir_p_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ *p, uid_t uid, gid_t gid, mode_t m) {
        ਵਾਪਸ mkdir_p_ਜੜ੍ਹ_ਭਰਿਆ(ਜੜ੍ਹ, p, uid, gid, m, USEC_INFINITY, ਨਿਰਾ);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*mkdirat_func_t)(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *pathname, mode_t ਬਹੁਲਕ, LabelContext *label_context);
ਪੂਰਨ mkdirat_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ mkdir_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ਵਾਪਸ mkdirat_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags, _mkdir, label_context);
}
ਪੂਰਨ mkdirat_parents_ਅੰਦਰੂਨੀ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
ਪੂਰਨ mkdir_parents_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ਪੂਰਨ mkdir_p_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
