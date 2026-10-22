/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "btrfs-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mkdir.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

ಪೂರ್ಣಾಂಕ mkdirat_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                mode_t ಬಹುಲಕ,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ರಚನೆ stat st;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ != ಬಹುಲಕ_INVALID);
        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        r = _mkdirat(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, label_context);
        ಆದರೆ (r >= 0)
                ಹಿಂತಿರುಗಿ chmod_and_chown_at(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid);
        ಆದರೆ (r != -EEXIST)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (fstatat(dir_fd, ಮಾರ್ಗ, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;

                r = chaseat(XAT_FDROOT, dir_fd, ಮಾರ್ಗ, CHASE_NONEXISTENT, &p, ಖಾಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ mkdirat_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(dir_fd, p, ಬಹುಲಕ, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ಆದರೆ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಆದರೆ (flags & MKDIR_IGNORE_EXISTING)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!S_ISDIR(st.st_mode))
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(flags & MKDIR_WARN_ಬಹುಲಕ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", ಮಾರ್ಗ);

        ಆದರೆ ((st.st_mode & ~ಬಹುಲಕ & 0777) != 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(flags & MKDIR_WARN_ಬಹುಲಕ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      ಮಾರ್ಗ, st.st_mode & 0777, ಬಹುಲಕ);

        ಆದರೆ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(flags & MKDIR_WARN_ಬಹುಲಕ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      ಮಾರ್ಗ, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ mkdirat_errno_wrapper(ಪೂರ್ಣಾಂಕ dirfd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, mode_t ಬಹುಲಕ, LabelContext *label_context) {
        ಹಿಂತಿರುಗಿ RET_NERRNO(mkdirat(dirfd, pathname, ಬಹುಲಕ));
}

ಪೂರ್ಣಾಂಕ mkdirat_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಹಿಂತಿರುಗಿ mkdirat_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ mkdirat_parents_ಆಂತರಿಕ(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        ಆದರೆ (isempty(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (!ಮಾರ್ಗ_is_ಸುರಕ್ಷಿತ(ಮಾರ್ಗ))
                ಹಿಂತಿರುಗಿ -ENOTDIR;

        /* return immediately if directory exists */
        r = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಕೊನೆಯ_component(ಮಾರ್ಗ, /* accept_dot_dot= */ false, &e, ಖಾಲಿ);
        ಆದರೆ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (e == ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ 0;

        ಪ್ರತಿಜ್ಞಾ(e > ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(*e == '/');

        /* drop the last component */
        ಮಾರ್ಗ = strndupa_ಸುರಕ್ಷಿತ(ಮಾರ್ಗ, e - ಮಾರ್ಗ);
        r = is_dir_at(dir_fd, ಮಾರ್ಗ, /* follow= */ true);
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಮಾರ್ಗ;;) {
                ಅಕ್ಷರ *s;
                ಪೂರ್ಣಾಂಕ n;

                n = ಮಾರ್ಗ_ಕಂಡುಹಿಡಿ_ಮೊದಲ_component(&p, /* accept_dot_dot= */ false, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **) &s);
                ಆದರೆ (n <= 0)
                        ಹಿಂತಿರುಗಿ n;

                ಪ್ರತಿಜ್ಞಾ(p);
                ಪ್ರತಿಜ್ಞಾ(s >= ಮಾರ್ಗ);
                ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ಆದರೆ (r < 0 && r != -EEXIST)
                        ಹಿಂತಿರುಗಿ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ಪೂರ್ಣಾಂಕ mkdir_parents_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = AT_FDCWD;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);
        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        ಆದರೆ (ಪೂರ್ವಪ್ರತ್ಯಯ) {
                p = ಮಾರ್ಗ_startswith_ತುಂಬಿದ(ಮಾರ್ಗ, ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ_STARTSWITH_REFUSE_DOT_DOT);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                fd = open(ಪೂರ್ವಪ್ರತ್ಯಯ, O_ಮಾರ್ಗ|O_DIRECTORY|O_CLOEXEC);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        } ಇಲ್ಲದಿದ್ದರೆ
                p = ಮಾರ್ಗ;

        ಹಿಂತಿರುಗಿ mkdirat_parents_ಆಂತರಿಕ(fd, p, ಬಹುಲಕ, uid, gid, flags, _mkdirat, label_context);
}

ಪೂರ್ಣಾಂಕ mkdirat_parents(ಪೂರ್ಣಾಂಕ dir_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ mkdirat_parents_ಆಂತರಿಕ(dir_fd, ಮಾರ್ಗ, ಬಹುಲಕ, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ mkdir_parents_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಹಿಂತಿರುಗಿ mkdir_parents_ಆಂತರಿಕ(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ mkdir_p_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ಪೂರ್ಣಾಂಕ r;

        /* Like mkdir -p */

        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        r = mkdir_parents_ಆಂತರಿಕ(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, ಮಾರ್ಗ, ಬಹುಲಕ, label_context);
                ಆದರೆ (r < 0 && (r != -EEXIST || is_dir(ಮಾರ್ಗ, true) <= 0))
                        ಹಿಂತಿರುಗಿ r;
        } ಇಲ್ಲದಿದ್ದರೆ {
                r = mkdir_ಸುರಕ್ಷಿತ_ಆಂತರಿಕ(ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags, _mkdirat, label_context);
                ಆದರೆ (r < 0 && r != -EEXIST)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ mkdir_p(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ) {
        ಹಿಂತಿರುಗಿ mkdir_p_ಆಂತರಿಕ(/* prefix= */ ಖಾಲಿ, ಮಾರ್ಗ, ಬಹುಲಕ, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ mkdir_p_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, mode_t ಬಹುಲಕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಹಿಂತಿರುಗಿ mkdir_p_ಆಂತರಿಕ(ಪೂರ್ವಪ್ರತ್ಯಯ, ಮಾರ್ಗ, ಬಹುಲಕ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ mkdir_p_ಬೇರು_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ ಅಕ್ಷರ *pp = ಖಾಲಿ, *bn = ಖಾಲಿ;
        _cleanup_close_ ಪೂರ್ಣಾಂಕ dfd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(p, &pp);
        ಆದರೆ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ಆದರೆ (dfd < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ಹಿಂತಿರುಗಿ 0;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಇಲ್ಲದಿದ್ದರೆ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_ಬೇರು_ತುಂಬಿದ(ಬೇರು, pp, uid, gid, m, ts, subvolumes);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                dfd = chase_and_open(pp, ಬೇರು, CHASE_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು, O_CLOEXEC|O_DIRECTORY, ಖಾಲಿ);
                ಆದರೆ (dfd < 0)
                        ಹಿಂತಿರುಗಿ dfd;
        }

        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(p, &bn);
        ಆದರೆ (r == -EADDRNOTAVAIL) /* Already top-level */
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        XOpenFlags flags = 0;
        ಆದರೆ (hashmap_ಒಳಗೊಂಡಿದೆ(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ಆದರೆ ((PTR_ಗಮ್ಯ_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ಪೂರ್ಣಾಂಕ nfd = xopenat_ತುಂಬಿದ(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ಆದರೆ (nfd == -EEXIST)
                ಹಿಂತಿರುಗಿ 0;
        ಆದರೆ (nfd < 0)
                ಹಿಂತಿರುಗಿ nfd;

        ಆದರೆ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ts != USEC_INFINITY) {
                ರಚನೆ timespec tspec;
                timespec_store(&tspec, ts);

                ಆದರೆ (futimens(dfd, (ಸ್ಥಿರಾಂಕ ರಚನೆ timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (futimens(nfd, (ಸ್ಥಿರಾಂಕ ರಚನೆ timespec[2]) { tspec, tspec }) < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ 1;
}
