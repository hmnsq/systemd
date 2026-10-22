/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_ಬಹುಲಕ       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

ಪೂರ್ಣಾಂಕ mkdirat_errno_wrapper(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, mode_t ಬಹುಲಕ, LabelContext *label_context);

ಪೂರ್ಣಾಂಕ mkdirat_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ mkdir_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಹಿಂತಿರುಗಿ mkdirat_ಸುರಕ್ಷಿತ(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags);
}
ಪೂರ್ಣಾಂಕ mkdirat_parents(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ mkdir_parents(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ mkdirat_parents(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ);
}
ಪೂರ್ಣಾಂಕ mkdir_parents_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags);
ಪೂರ್ಣಾಂಕ mkdir_p(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ mkdir_p_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags);
ಪೂರ್ಣಾಂಕ mkdir_p_ಬೇರು_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ mkdir_p_ಬೇರು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, uid_t uid, gid_t gid, mode_t m) {
        ಹಿಂತಿರುಗಿ mkdir_p_ಬೇರು_ತುಂಬಿದ(ಬೇರು, p, uid, gid, m, USEC_INFINITY, ಖಾಲಿ);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*mkdirat_func_t)(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, mode_t ಬಹುಲಕ, LabelContext *label_context);
ಪೂರ್ಣಾಂಕ mkdirat_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ mkdir_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        ಹಿಂತಿರುಗಿ mkdirat_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags, _mkdir, label_context);
}
ಪೂರ್ಣಾಂಕ mkdirat_parents_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
ಪೂರ್ಣಾಂಕ mkdir_parents_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
ಪೂರ್ಣಾಂಕ mkdir_p_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
