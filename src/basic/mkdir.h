/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_errno_wrapper(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *pathname, mode_t mode, LabelContext *label_context);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_safe(ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ଫେରନ୍ତୁ mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_parents(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_parents(ଅଚଳ ଅକ୍ଷର *path, mode_t mode) {
        ଫେରନ୍ତୁ mkdirat_parents(AT_FDCWD, path, mode);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_parents_safe(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p(ଅଚଳ ଅକ୍ଷର *path, mode_t mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_safe(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_ମୂଳ_full(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅଚଳ ଅକ୍ଷର *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_ମୂଳ(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅଚଳ ଅକ୍ଷର *p, uid_t uid, gid_t gid, mode_t m) {
        ଫେରନ୍ତୁ mkdir_p_ମୂଳ_full(ମୂଳ, p, uid, gid, m, USEC_INFINITY, ଖାଲି);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*mkdirat_func_t)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *pathname, mode_t mode, LabelContext *label_context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_safe_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_safe_internal(ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ଫେରନ୍ତୁ mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_parents_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_parents_internal(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_internal(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
