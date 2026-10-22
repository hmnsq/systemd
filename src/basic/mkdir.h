/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన MkdirFlags {
        MKDIR_FOLLOW_SYMLINK  = 1 << 0,
        MKDIR_IGNORE_EXISTING = 1 << 1,  /* Quietly accept a preexisting directory (or file) */
        MKDIR_WARN_బహుళకం       = 1 << 2,  /* Log at LOG_WARNING when mode doesn't match */
} MkdirFlags;

పూర్ణాంకం mkdirat_errno_wrapper(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *pathname, mode_t బహుళకం, LabelContext *label_context);

పూర్ణాంకం mkdirat_సురక్షిత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags);
స్థిర అంతఃవరుస పూర్ణాంకం mkdir_సురక్షిత(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags) {
        తిరిగి mkdirat_సురక్షిత(AT_FDCWD, మార్గం, బహుళకం, uid, gid, flags);
}
పూర్ణాంకం mkdirat_parents(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం);
స్థిర అంతఃవరుస పూర్ణాంకం mkdir_parents(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం) {
        తిరిగి mkdirat_parents(AT_FDCWD, మార్గం, బహుళకం);
}
పూర్ణాంకం mkdir_parents_సురక్షిత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags);
పూర్ణాంకం mkdir_p(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం);
పూర్ణాంకం mkdir_p_సురక్షిత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags);
పూర్ణాంకం mkdir_p_మూలం_నిండిన(స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes);
స్థిర అంతఃవరుస పూర్ణాంకం mkdir_p_మూలం(స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం *p, uid_t uid, gid_t gid, mode_t m) {
        తిరిగి mkdir_p_మూలం_నిండిన(మూలం, p, uid, gid, m, USEC_INFINITY, ఖాళీ);
}

/* The following are used to implement the mkdir_xyz_label() calls, don't use otherwise. */
రకనిర్వచనం పూర్ణాంకం (*mkdirat_func_t)(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *pathname, mode_t బహుళకం, LabelContext *label_context);
పూర్ణాంకం mkdirat_సురక్షిత_అంతర్గత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం mkdir_సురక్షిత_అంతర్గత(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context) {
        తిరిగి mkdirat_సురక్షిత_అంతర్గత(AT_FDCWD, మార్గం, బహుళకం, uid, gid, flags, _mkdir, label_context);
}
పూర్ణాంకం mkdirat_parents_అంతర్గత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context);
పూర్ణాంకం mkdir_parents_అంతర్గత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
పూర్ణాంకం mkdir_p_అంతర్గత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdir, LabelContext *label_context);
