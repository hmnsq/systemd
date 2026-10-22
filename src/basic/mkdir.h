/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_MODE       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

പൂർണ്ണസംഖ്യ mkdirat_errno_wrapper(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *pathname, mode_t mode, LabelContext *label_context);

പൂർണ്ണസംഖ്യ mkdirat_safe(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ mkdir_safe(സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        തിരികെ mkdirat_safe(AT_FDCWD, path, mode, uid, gid, flags);
}
പൂർണ്ണസംഖ്യ mkdirat_parents(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ mkdir_parents(സ്ഥിരം അക്ഷരം *path, mode_t mode) {
        തിരികെ mkdirat_parents(AT_FDCWD, path, mode);
}
പൂർണ്ണസംഖ്യ mkdir_parents_safe(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
പൂർണ്ണസംഖ്യ mkdir_p(സ്ഥിരം അക്ഷരം *path, mode_t mode);
പൂർണ്ണസംഖ്യ mkdir_p_safe(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags);
പൂർണ്ണസംഖ്യ mkdir_p_വേര്_full(സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ mkdir_p_വേര്(സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം *p, uid_t uid, gid_t gid, mode_t m) {
        തിരികെ mkdir_p_വേര്_full(വേര്, p, uid, gid, m, USEC_INFINITY, ശൂന്യ);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
തരനിർവചനം പൂർണ്ണസംഖ്യ (*mkdirat_func_t)(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *pathname, mode_t mode, LabelContext *label_context);
പൂർണ്ണസംഖ്യ mkdirat_safe_internal(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ mkdir_safe_internal(സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        തിരികെ mkdirat_safe_internal(AT_FDCWD, path, mode, uid, gid, flags, _mkdir, label_context);
}
പൂർണ്ണസംഖ്യ mkdirat_parents_internal(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
പൂർണ്ണസംഖ്യ mkdir_parents_internal(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
പൂർണ്ണസംഖ്യ mkdir_p_internal(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
