/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన ChaseFlags {
        CHASE_ఉపసర్గ_మూలం        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
        CHASE_NONEXISTENT        = 1 << 1,  /* It's OK if the path doesn't actually exist. */
        CHASE_NO_AUTOFS          = 1 << 2,  /* Return -EREMOTE if autofs mount point found */
        CHASE_TRIGGER_AUTOFS     = 1 << 3,  /* Use open_tree() rather than openat() to trigger autofs mount */
        CHASE_సురక్షిత               = 1 << 4,  /* Return -EPERM if we ever traverse from unprivileged to privileged files or directories */
        CHASE_TRAIL_SLASH        = 1 << 5,  /* Any trailing slash will be preserved */
        CHASE_అడుగు               = 1 << 6,  /* Just execute a single step of the normalization */
        CHASE_NOFOLLOW           = 1 << 7,  /* Do not follow the path's right-most component. With ret_fd, when the path's
                                             * right-most component refers to symlink, return O_PATH fd of the symlink. */
        CHASE_WARN               = 1 << 8,  /* Emit an appropriate warning when an error is encountered.
                                             * Note: this may do an NSS lookup, hence this flag cannot be used in PID 1. */
        CHASE_PROHIBIT_SYMLINKS  = 1 << 9,  /* Refuse all symlinks */
        CHASE_మాతృ             = 1 << 10, /* Chase the parent directory of the given path. Note that the
                                             * full path is still stored in ret_path and only the returned
                                             * file descriptor will point to the parent directory. Note that
                                             * the result path is the root or '.', then the file descriptor
                                             * also points to the result path even if this flag is set.
                                             * When this specified, chase() will succeed with 1 even if the
                                             * file points to the last path component does not exist. */
        CHASE_MKDIR_0755         = 1 << 11, /* Create any missing directories in the given path. */
        CHASE_వెలికితీయి_FILENAME   = 1 << 12, /* Only return the last component of the resolved path */
        CHASE_MUST_BE_DIRECTORY  = 1 << 13, /* Fail if returned inode fd is not a dir */
        CHASE_MUST_BE_REGULAR    = 1 << 14, /* Fail if returned inode fd is not a regular file */
        CHASE_MUST_BE_SOCKET     = 1 << 15, /* Fail if returned inode fd is not a socket */
} ChaseFlags;

పూర్ణాంకం statx_unsafe_transition(స్థిరరాశి నిర్మాణం statx *a, స్థిరరాశి నిర్మాణం statx *b);
bool stat_unsafe_transition(స్థిరరాశి నిర్మాణం stat *a, స్థిరరాశి నిర్మాణం stat *b);

/* How many iterations to execute before returning -ELOOP */
#నిర్వచించు CHASE_MAX 128U

పూర్ణాంకం chase(స్థిరరాశి అక్షరం *మార్గం_with_ఉపసర్గ, స్థిరరాశి అక్షరం *మూలం, ChaseFlags flags, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);

పూర్ణాంకం chaseat_ఉపసర్గ_మూలం(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret);
పూర్ణాంకం chase_వెలికితీయి_filename(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret);

పూర్ణాంకం chase_and_open(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, పూర్ణాంకం open_flags, అక్షరం **ret_మార్గం);
పూర్ణాంకం chase_and_opendir(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, DIR **ret_dir);
పూర్ణాంకం chase_and_stat(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, నిర్మాణం stat *ret_stat);
పూర్ణాంకం chase_and_access(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, పూర్ణాంకం access_బహుళకం, అక్షరం **ret_మార్గం);
పూర్ణాంకం chase_and_fopen_unlocked(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, స్థిరరాశి అక్షరం *open_flags, అక్షరం **ret_మార్గం, FILE **ret_file);
పూర్ణాంకం chase_and_unlink(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, పూర్ణాంకం unlink_flags, అక్షరం **ret_మార్గం);
పూర్ణాంకం chase_and_open_మాతృ(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ChaseFlags chase_flags, అక్షరం **ret_filename);

పూర్ణాంకం chaseat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags flags, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);

పూర్ణాంకం chase_and_openat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, పూర్ణాంకం open_flags, అక్షరం **ret_మార్గం);
పూర్ణాంకం chase_and_opendirat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, DIR **ret_dir);
పూర్ణాంకం chase_and_statat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, అక్షరం **ret_మార్గం, నిర్మాణం stat *ret_stat);
పూర్ణాంకం chase_and_accessat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, పూర్ణాంకం access_బహుళకం, అక్షరం **ret_మార్గం);
పూర్ణాంకం chase_and_fopenat_unlocked(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, స్థిరరాశి అక్షరం *open_flags, అక్షరం **ret_మార్గం, FILE **ret_file);
పూర్ణాంకం chase_and_unlinkat(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, పూర్ణాంకం unlink_flags, అక్షరం **ret_మార్గం);
పూర్ణాంకం chase_and_open_మాతృ_at(పూర్ణాంకం మూలం_fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, ChaseFlags chase_flags, అక్షరం **ret_filename);
