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

ಪೂರ್ಣಾಙ್ಕ mkdirat_safe_internal(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ಸಂರಚನಾ stat st;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(mode != MODE_INVALID);
        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ಯದಿ (r >= 0)
                ಪ್ರತ್ಯಾಗಮ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ಯದಿ (r != -EEXIST)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, ರಿಕ್ತ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ಯದಿ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಯದಿ (flags & MKDIR_IGNORE_EXISTING)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!S_ISDIR(st.st_mode))
                ಪ್ರತ್ಯಾಗಮ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ಯದಿ ((st.st_mode & ~mode & 0777) != 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ಯದಿ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ಪ್ರತ್ಯಾಗಮ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ mkdirat_errno_wrapper(ಪೂರ್ಣಾಙ್ಕ dirfd, ಧ್ರುವ ವರ್ಣ *pathname, mode_t mode, LabelContext *label_context) {
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

ಪೂರ್ಣಾಙ್ಕ mkdirat_safe(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಪ್ರತ್ಯಾಗಮ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ mkdirat_parents_internal(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ಧ್ರುವ ವರ್ಣ *e = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        ಯದಿ (isempty(path))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (!path_is_safe(path))
                ಪ್ರತ್ಯಾಗಮ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, ರಿಕ್ತ);
        ಯದಿ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (e == path)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತಿಜ್ಞಾ(e > path);
        ಪ್ರತಿಜ್ಞಾ(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = path;;) {
                ವರ್ಣ *s;
                ಪೂರ್ಣಾಙ್ಕ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ಧ್ರುವ ವರ್ಣ **) &s);
                ಯದಿ (n <= 0)
                        ಪ್ರತ್ಯಾಗಮ n;

                ಪ್ರತಿಜ್ಞಾ(p);
                ಪ್ರತಿಜ್ಞಾ(s >= path);
                ಪ್ರತಿಜ್ಞಾ(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ಯದಿ (r < 0 && r != -EEXIST)
                        ಪ್ರತ್ಯಾಗಮ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ಪೂರ್ಣಾಙ್ಕ mkdir_parents_internal(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = AT_FDCWD;
        ಧ್ರುವ ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(path);
        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        ಯದಿ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        } ಅನ್ಯಥಾ
                p = path;

        ಪ್ರತ್ಯಾಗಮ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

ಪೂರ್ಣಾಙ್ಕ mkdirat_parents(ಪೂರ್ಣಾಙ್ಕ dir_fd, ಧ್ರುವ ವರ್ಣ *path, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ mkdir_parents_safe(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಪ್ರತ್ಯಾಗಮ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ mkdir_p_internal(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Like mkdir -p */

        ಪ್ರತಿಜ್ಞಾ(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ಯದಿ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ಪ್ರತ್ಯಾಗಮ r;
        } ಅನ್ಯಥಾ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ಯದಿ (r < 0 && r != -EEXIST)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ mkdir_p(ಧ್ರುವ ವರ್ಣ *path, mode_t mode) {
        ಪ್ರತ್ಯಾಗಮ mkdir_p_internal(/* prefix= */ ರಿಕ್ತ, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ mkdir_p_safe(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ಪ್ರತ್ಯಾಗಮ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ mkdir_p_ಬೇರು_full(ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಧ್ರುವ ವರ್ಣ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ ವರ್ಣ *pp = ರಿಕ್ತ, *bn = ರಿಕ್ತ;
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ dfd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(p);

        r = path_extract_directory(p, &pp);
        ಯದಿ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ಯದಿ (dfd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        } ಅನ್ಯಥಾ ಯದಿ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಅನ್ಯಥಾ ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಅನ್ಯಥಾ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_ಬೇರು_full(ಬೇರು, pp, uid, gid, m, ts, subvolumes);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                dfd = chase_and_open(pp, ಬೇರು, CHASE_PREFIX_ಬೇರು, O_CLOEXEC|O_DIRECTORY, ರಿಕ್ತ);
                ಯದಿ (dfd < 0)
                        ಪ್ರತ್ಯಾಗಮ dfd;
        }

        r = path_extract_filename(p, &bn);
        ಯದಿ (r == -EADDRNOTAVAIL) /* Already top-level */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        XOpenFlags flags = 0;
        ಯದಿ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ಯದಿ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ಯದಿ (nfd == -EEXIST)
                ಪ್ರತ್ಯಾಗಮ 0;
        ಯದಿ (nfd < 0)
                ಪ್ರತ್ಯಾಗಮ nfd;

        ಯದಿ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ts != USEC_INFINITY) {
                ಸಂರಚನಾ timespec tspec;
                timespec_store(&tspec, ts);

                ಯದಿ (futimens(dfd, (ಧ್ರುವ ಸಂರಚನಾ timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (futimens(nfd, (ಧ್ರುವ ಸಂರಚನಾ timespec[2]) { tspec, tspec }) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ 1;
}
