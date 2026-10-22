/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

પૂર્ણાંક mkdirat_errno_wrapper(પૂર્ણાંક dirfd, અચલ અક્ષર *pathname, mode_t mode, LabelContext *label_context);

પૂર્ણાંક mkdirat_safe(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
સ્થિર અંતઃરેખ પૂર્ણાંક mkdir_safe(અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        પાછા mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
પૂર્ણાંક mkdirat_parents(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode);
સ્થિર અંતઃરેખ પૂર્ણાંક mkdir_parents(અચલ અક્ષર *path, mode_t mode) {
        પાછા mkdirat_parents(AT_FDCWD, path, mode);
}
પૂર્ણાંક mkdir_parents_safe(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
પૂર્ણાંક mkdir_p(અચલ અક્ષર *path, mode_t mode);
પૂર્ણાંક mkdir_p_safe(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
પૂર્ણાંક mkdir_p_મૂળ_full(અચલ અક્ષર *મૂળ, અચલ અક્ષર *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
સ્થિર અંતઃરેખ પૂર્ણાંક mkdir_p_મૂળ(અચલ અક્ષર *મૂળ, અચલ અક્ષર *p, uid_t uid, gid_t gid, mode_t m) {
        પાછા mkdir_p_મૂળ_full(મૂળ, p, uid, gid, m, USEC_INFINITY, ખાલી);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
પ્રકારવ્યાખ્યા પૂર્ણાંક (*mkdirat_func_t)(પૂર્ણાંક dir_fd, અચલ અક્ષર *pathname, mode_t mode, LabelContext *label_context);
પૂર્ણાંક mkdirat_safe_internal(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
સ્થિર અંતઃરેખ પૂર્ણાંક mkdir_safe_internal(અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        પાછા mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
પૂર્ણાંક mkdirat_parents_internal(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
પૂર્ણાંક mkdir_parents_internal(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
પૂર્ણાંક mkdir_p_internal(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
