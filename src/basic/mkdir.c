/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "btrfs-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "hashmap.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mkdir.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "user-util.h"

పూర్ణాంకం mkdirat_సురక్షిత_అంతర్గత(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                mode_t బహుళకం,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        నిర్మాణం stat st;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(బహుళకం != బహుళకం_INVALID);
        ప్రతిజ్ఞా(_mkdirat);

        r = _mkdirat(dir_fd, మార్గం, బహుళకం, label_context);
        ఒకవేళ (r >= 0)
                తిరిగి chmod_and_chown_at(dir_fd, మార్గం, బహుళకం, uid, gid);
        ఒకవేళ (r != -EEXIST)
                తిరిగి r;

        ఒకవేళ (fstatat(dir_fd, మార్గం, &st, AT_SYMLINK_NOFOLLOW) < 0)
                తిరిగి -errno;

        ఒకవేళ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ అక్షరం *p = ఖాళీ;

                r = chaseat(XAT_FDROOT, dir_fd, మార్గం, CHASE_NONEXISTENT, &p, ఖాళీ);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి mkdirat_సురక్షిత_అంతర్గత(dir_fd, p, బహుళకం, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ఒకవేళ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        తిరిగి -errno;
        }

        ఒకవేళ (flags & MKDIR_IGNORE_EXISTING)
                తిరిగి 0;

        ఒకవేళ (!S_ISDIR(st.st_mode))
                తిరిగి log_నిండిన_errno(flags & MKDIR_WARN_బహుళకం ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", మార్గం);

        ఒకవేళ ((st.st_mode & ~బహుళకం & 0777) != 0)
                తిరిగి log_నిండిన_errno(flags & MKDIR_WARN_బహుళకం ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      మార్గం, st.st_mode & 0777, బహుళకం);

        ఒకవేళ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                తిరిగి log_నిండిన_errno(flags & MKDIR_WARN_బహుళకం ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      మార్గం, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        తిరిగి 0;
}

పూర్ణాంకం mkdirat_errno_wrapper(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *pathname, mode_t బహుళకం, LabelContext *label_context) {
        తిరిగి RET_NERRNO(mkdirat(dirfd, pathname, బహుళకం));
}

పూర్ణాంకం mkdirat_సురక్షిత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags) {
        తిరిగి mkdirat_సురక్షిత_అంతర్గత(dir_fd, మార్గం, బహుళకం, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ఖాళీ);
}

పూర్ణాంకం mkdirat_parents_అంతర్గత(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        స్థిరరాశి అక్షరం *e = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(_mkdirat);

        ఒకవేళ (isempty(మార్గం))
                తిరిగి 0;

        ఒకవేళ (!మార్గం_is_సురక్షిత(మార్గం))
                తిరిగి -ENOTDIR;

        /* return immediately if directory exists */
        r = మార్గం_కనుగొను_చివరి_component(మార్గం, /* accept_dot_dot= */ false, &e, ఖాళీ);
        ఒకవేళ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                తిరిగి r;
        ఒకవేళ (e == మార్గం)
                తిరిగి 0;

        ప్రతిజ్ఞా(e > మార్గం);
        ప్రతిజ్ఞా(*e == '/');

        /* drop the last component */
        మార్గం = strndupa_సురక్షిత(మార్గం, e - మార్గం);
        r = is_dir_at(dir_fd, మార్గం, /* follow= */ true);
        ఒకవేళ (r > 0)
                తిరిగి 0;
        ఒకవేళ (r == 0)
                తిరిగి -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        కొరకు (స్థిరరాశి అక్షరం *p = మార్గం;;) {
                అక్షరం *s;
                పూర్ణాంకం n;

                n = మార్గం_కనుగొను_మొదటి_component(&p, /* accept_dot_dot= */ false, (స్థిరరాశి అక్షరం **) &s);
                ఒకవేళ (n <= 0)
                        తిరిగి n;

                ప్రతిజ్ఞా(p);
                ప్రతిజ్ఞా(s >= మార్గం);
                ప్రతిజ్ఞా(లోపల_సమితి(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_సురక్షిత_అంతర్గత(dir_fd, మార్గం, బహుళకం, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ఒకవేళ (r < 0 && r != -EEXIST)
                        తిరిగి r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

పూర్ణాంకం mkdir_parents_అంతర్గత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ పూర్ణాంకం fd = AT_FDCWD;
        స్థిరరాశి అక్షరం *p;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(_mkdirat);

        ఒకవేళ (ఉపసర్గ) {
                p = మార్గం_startswith_నిండిన(మార్గం, ఉపసర్గ, మార్గం_STARTSWITH_REFUSE_DOT_DOT);
                ఒకవేళ (!p)
                        తిరిగి -EINVAL;

                fd = open(ఉపసర్గ, O_మార్గం|O_DIRECTORY|O_CLOEXEC);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;
        } లేకపోతే
                p = మార్గం;

        తిరిగి mkdirat_parents_అంతర్గత(fd, p, బహుళకం, uid, gid, flags, _mkdirat, label_context);
}

పూర్ణాంకం mkdirat_parents(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం) {
        తిరిగి mkdirat_parents_అంతర్గత(dir_fd, మార్గం, బహుళకం, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ఖాళీ);
}

పూర్ణాంకం mkdir_parents_సురక్షిత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags) {
        తిరిగి mkdir_parents_అంతర్గత(ఉపసర్గ, మార్గం, బహుళకం, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ఖాళీ);
}

పూర్ణాంకం mkdir_p_అంతర్గత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        పూర్ణాంకం r;

        /* Like mkdir -p */

        ప్రతిజ్ఞా(_mkdirat);

        r = mkdir_parents_అంతర్గత(ఉపసర్గ, మార్గం, బహుళకం, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, మార్గం, బహుళకం, label_context);
                ఒకవేళ (r < 0 && (r != -EEXIST || is_dir(మార్గం, true) <= 0))
                        తిరిగి r;
        } లేకపోతే {
                r = mkdir_సురక్షిత_అంతర్గత(మార్గం, బహుళకం, uid, gid, flags, _mkdirat, label_context);
                ఒకవేళ (r < 0 && r != -EEXIST)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం mkdir_p(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం) {
        తిరిగి mkdir_p_అంతర్గత(/* prefix= */ ఖాళీ, మార్గం, బహుళకం, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ఖాళీ);
}

పూర్ణాంకం mkdir_p_సురక్షిత(స్థిరరాశి అక్షరం *ఉపసర్గ, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid, MkdirFlags flags) {
        తిరిగి mkdir_p_అంతర్గత(ఉపసర్గ, మార్గం, బహుళకం, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ఖాళీ);
}

పూర్ణాంకం mkdir_p_మూలం_నిండిన(స్థిరరాశి అక్షరం *మూలం, స్థిరరాశి అక్షరం *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ అక్షరం *pp = ఖాళీ, *bn = ఖాళీ;
        _cleanup_close_ పూర్ణాంకం dfd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);

        r = మార్గం_వెలికితీయి_directory(p, &pp);
        ఒకవేళ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ఒకవేళ (dfd < 0)
                        తిరిగి -errno;
        } లేకపోతే ఒకవేళ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                తిరిగి 0;
        లేకపోతే ఒకవేళ (r < 0)
                తిరిగి r;
        లేకపోతే {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_మూలం_నిండిన(మూలం, pp, uid, gid, m, ts, subvolumes);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                dfd = chase_and_open(pp, మూలం, CHASE_ఉపసర్గ_మూలం, O_CLOEXEC|O_DIRECTORY, ఖాళీ);
                ఒకవేళ (dfd < 0)
                        తిరిగి dfd;
        }

        r = మార్గం_వెలికితీయి_filename(p, &bn);
        ఒకవేళ (r == -EADDRNOTAVAIL) /* Already top-level */
                తిరిగి 0;
        ఒకవేళ (r < 0)
                తిరిగి r;

        XOpenFlags flags = 0;
        ఒకవేళ (hashmap_కలిగియుంది(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ఒకవేళ ((PTR_గమ్యం_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ పూర్ణాంకం nfd = xopenat_నిండిన(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ఒకవేళ (nfd == -EEXIST)
                తిరిగి 0;
        ఒకవేళ (nfd < 0)
                తిరిగి nfd;

        ఒకవేళ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                తిరిగి -errno;

        ఒకవేళ (ts != USEC_INFINITY) {
                నిర్మాణం timespec tspec;
                timespec_store(&tspec, ts);

                ఒకవేళ (futimens(dfd, (స్థిరరాశి నిర్మాణం timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        తిరిగి -errno;

                ఒకవేళ (futimens(nfd, (స్థిరరాశి నిర్మాణం timespec[2]) { tspec, tspec }) < 0)
                        తిరిగి -errno;
        }

        తిరిగి 1;
}
