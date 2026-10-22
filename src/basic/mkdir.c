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
        ܐܢ (r >= 0)
                ܦܢܐ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ܐܢ (r != -EEXIST)
                ܦܢܐ r;

        ܐܢ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ܦܢܐ -errno;

        ܐܢ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ حرف *p = عدم;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, عدم);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        ܦܢܐ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ܐܢ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ܦܢܐ -errno;
        }

        ܐܢ (flags & MKDIR_IGNORE_EXISTING)
                ܦܢܐ 0;

        ܐܢ (!S_ISDIR(st.st_mode))
                ܦܢܐ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ܐܢ ((st.st_mode & ~mode & 0777) != 0)
                ܦܢܐ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ܐܢ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ܦܢܐ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ܦܢܐ 0;
}

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t mode, LabelContext *label_context) {
        ܦܢܐ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

صحيح mkdirat_safe(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ܦܢܐ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdirat_parents_internal(صحيح dir_fd, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ثابت حرف *e = عدم;
        صحيح r;

        أكد(path);
        أكد(_mkdirat);

        ܐܢ (isempty(path))
                ܦܢܐ 0;

        ܐܢ (!path_is_safe(path))
                ܦܢܐ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, عدم);
        ܐܢ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ܦܢܐ r;
        ܐܢ (e == path)
                ܦܢܐ 0;

        أكد(e > path);
        أكد(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ܐܢ (r > 0)
                ܦܢܐ 0;
        ܐܢ (r == 0)
                ܦܢܐ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        لكل (ثابت حرف *p = path;;) {
                حرف *s;
                صحيح n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ثابت حرف **) &s);
                ܐܢ (n <= 0)
                        ܦܢܐ n;

                أكد(p);
                أكد(s >= path);
                أكد(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ܐܢ (r < 0 && r != -EEXIST)
                        ܦܢܐ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

صحيح mkdir_parents_internal(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ صحيح fd = AT_FDCWD;
        ثابت حرف *p;

        أكد(path);
        أكد(_mkdirat);

        ܐܢ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ܐܢ (!p)
                        ܦܢܐ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ܐܢ (fd < 0)
                        ܦܢܐ -errno;
        } ܐܚܪܢܐ
                p = path;

        ܦܢܐ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *path, mode_t mode) {
        ܦܢܐ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_parents_safe(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ܦܢܐ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_internal(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        صحيح r;

        /* Like mkdir -p */

        أكد(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ܐܢ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ܦܢܐ r;
        } ܐܚܪܢܐ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ܐܢ (r < 0 && r != -EEXIST)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
}

صحيح mkdir_p(ثابت حرف *path, mode_t mode) {
        ܦܢܐ mkdir_p_internal(/* prefix= */ عدم, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_safe(ثابت حرف *prefix, ثابت حرف *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ܦܢܐ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_root_full(ثابت حرف *root, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ حرف *pp = عدم, *bn = عدم;
        _cleanup_close_ صحيح dfd = -EBADF;
        صحيح r;

        أكد(p);

        r = path_extract_directory(p, &pp);
        ܐܢ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ܐܢ (dfd < 0)
                        ܦܢܐ -errno;
        } ܐܚܪܢܐ ܐܢ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ܦܢܐ 0;
        ܐܚܪܢܐ ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܚܪܢܐ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, عدم);
                ܐܢ (dfd < 0)
                        ܦܢܐ dfd;
        }

        r = path_extract_filename(p, &bn);
        ܐܢ (r == -EADDRNOTAVAIL) /* Already top-level */
                ܦܢܐ 0;
        ܐܢ (r < 0)
                ܦܢܐ r;

        XOpenFlags flags = 0;
        ܐܢ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ܐܢ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ صحيح nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ܐܢ (nfd == -EEXIST)
                ܦܢܐ 0;
        ܐܢ (nfd < 0)
                ܦܢܐ nfd;

        ܐܢ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ܦܢܐ -errno;

        ܐܢ (ts != USEC_INFINITY) {
                بنية timespec tspec;
                timespec_store(&tspec, ts);

                ܐܢ (futimens(dfd, (ثابت بنية timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ܦܢܐ -errno;

                ܐܢ (futimens(nfd, (ثابت بنية timespec[2]) { tspec, tspec }) < 0)
                        ܦܢܐ -errno;
        }

        ܦܢܐ 1;
}
