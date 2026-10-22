/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "btrfs-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "format-util.h"
#ضمن "fs-util.h"
#ضمن "hashmap.h"
#ضمن "log.h"
#ضمن "mkdir.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-util.h"
#ضمن "time-util.h"
#ضمن "user-util.h"

صحيح mkdirat_safe_internal(
                صحيح dir_fd,
                ثابت حرف *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        بنية stat st;
        صحيح r;

        أكد(path);
        أكد(mode != MODE_INVALID);
        أكد(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        إذا (r >= 0)
                إرجاع chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        إذا (r != -EEXIST)
                إرجاع r;

        إذا (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                إرجاع -errno;

        إذا ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ حرف *p = عدم;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, عدم);
                إذا (r < 0)
                        إرجاع r;
                إذا (r == 0)
                        إرجاع mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                إذا (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        إرجاع -errno;
        }

        إذا (flags & MKDIR_IGNORE_EXISTING)
                إرجاع 0;

        إذا (!S_ISDIR(st.st_mode))
                إرجاع log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        إذا ((st.st_mode & ~mode & 0777) != 0)
                إرجاع log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        إذا ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                إرجاع log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        إرجاع 0;
}

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t mode, LabelContext *label_context) {
        إرجاع RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

صحيح mkdirat_safe(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        إرجاع mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdirat_parents_internal(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ثابت حرف *e = عدم;
        صحيح r;

        أكد(path);
        أكد(_mkdirat);

        إذا (isempty(path))
                إرجاع 0;

        إذا (!path_is_safe(path))
                إرجاع -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, عدم);
        إذا (r <= 0) /* r == 0 means path is equivalent to prefix. */
                إرجاع r;
        إذا (e == path)
                إرجاع 0;

        أكد(e > path);
        أكد(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        إذا (r > 0)
                إرجاع 0;
        إذا (r == 0)
                إرجاع -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        لكل (ثابت حرف *p = path;;) {
                حرف *s;
                صحيح n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ثابت حرف **) &s);
                إذا (n <= 0)
                        إرجاع n;

                أكد(p);
                أكد(s >= path);
                أكد(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                إذا (r < 0 && r != -EEXIST)
                        إرجاع r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

صحيح mkdir_parents_internal(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ صحيح fd = AT_FDCWD;
        ثابت حرف *p;

        أكد(path);
        أكد(_mkdirat);

        إذا (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                إذا (!p)
                        إرجاع -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                إذا (fd < 0)
                        إرجاع -errno;
        } وإلا
                p = path;

        إرجاع mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *path, mode_t mode) {
        إرجاع mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_parents_safe(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        إرجاع mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_internal(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        صحيح r;

        /* Like mkdir -p */

        أكد(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        إذا (r < 0)
                إرجاع r;

        إذا (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                إذا (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        إرجاع r;
        } وإلا {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                إذا (r < 0 && r != -EEXIST)
                        إرجاع r;
        }

        إرجاع 0;
}

صحيح mkdir_p(ثابت حرف *path, mode_t mode) {
        إرجاع mkdir_p_internal(/* prefix= */ عدم, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_safe(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        إرجاع mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_جذر_full(ثابت حرف *جذر, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ حرف *pp = عدم, *bn = عدم;
        _cleanup_close_ صحيح dfd = -EBADF;
        صحيح r;

        أكد(p);

        r = path_extract_directory(p, &pp);
        إذا (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                إذا (dfd < 0)
                        إرجاع -errno;
        } وإلا إذا (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                إرجاع 0;
        وإلا إذا (r < 0)
                إرجاع r;
        وإلا {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_جذر_full(جذر, pp, uid, gid, m, ts, subvolumes);
                إذا (r < 0)
                        إرجاع r;

                dfd = chase_and_open(pp, جذر, CHASE_PREFIX_جذر, O_CLOEXEC|O_DIRECTORY, عدم);
                إذا (dfd < 0)
                        إرجاع dfd;
        }

        r = path_extract_filename(p, &bn);
        إذا (r == -EADDRNOTAVAIL) /* Already top-level */
                إرجاع 0;
        إذا (r < 0)
                إرجاع r;

        XOpenFlags flags = 0;
        إذا (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                إذا ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ صحيح nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        إذا (nfd == -EEXIST)
                إرجاع 0;
        إذا (nfd < 0)
                إرجاع nfd;

        إذا ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                إرجاع -errno;

        إذا (ts != USEC_INFINITY) {
                بنية timespec tspec;
                timespec_store(&tspec, ts);

                إذا (futimens(dfd, (ثابت بنية timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        إرجاع -errno;

                إذا (futimens(nfd, (ثابت بنية timespec[2]) { tspec, tspec }) < 0)
                        إرجاع -errno;
        }

        إرجاع 1;
}
