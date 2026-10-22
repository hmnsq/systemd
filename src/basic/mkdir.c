/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "btrfs-util.h"
#شامل "chase.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "format-util.h"
#شامل "fs-util.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "mkdir.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-util.h"
#شامل "time-util.h"
#شامل "user-util.h"

صحیح mkdirat_safe_internal(
                صحیح dir_fd,
                ثابت نویسه *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ساختار stat st;
        صحیح r;

        أكد(path);
        أكد(mode != MODE_INVALID);
        أكد(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        اگر (r >= 0)
                بازگشت chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        اگر (r != -EEXIST)
                بازگشت r;

        اگر (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                بازگشت -errno;

        اگر ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ نویسه *p = پوچ;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, پوچ);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                اگر (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        بازگشت -errno;
        }

        اگر (flags & MKDIR_IGNORE_EXISTING)
                بازگشت 0;

        اگر (!S_ISDIR(st.st_mode))
                بازگشت log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        اگر ((st.st_mode & ~mode & 0777) != 0)
                بازگشت log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        اگر ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                بازگشت log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        بازگشت 0;
}

صحیح mkdirat_errno_wrapper(صحیح dirfd, ثابت نویسه *pathname, mode_t mode, LabelContext *label_context) {
        بازگشت RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

صحیح mkdirat_safe(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        بازگشت mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ پوچ);
}

صحیح mkdirat_parents_internal(صحیح dir_fd, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ثابت نویسه *e = پوچ;
        صحیح r;

        أكد(path);
        أكد(_mkdirat);

        اگر (isempty(path))
                بازگشت 0;

        اگر (!path_is_safe(path))
                بازگشت -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, پوچ);
        اگر (r <= 0) /* r == 0 means path is equivalent to prefix. */
                بازگشت r;
        اگر (e == path)
                بازگشت 0;

        أكد(e > path);
        أكد(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        اگر (r > 0)
                بازگشت 0;
        اگر (r == 0)
                بازگشت -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        برای (ثابت نویسه *p = path;;) {
                نویسه *s;
                صحیح n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ثابت نویسه **) &s);
                اگر (n <= 0)
                        بازگشت n;

                أكد(p);
                أكد(s >= path);
                أكد(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                اگر (r < 0 && r != -EEXIST)
                        بازگشت r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

صحیح mkdir_parents_internal(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ صحیح fd = AT_FDCWD;
        ثابت نویسه *p;

        أكد(path);
        أكد(_mkdirat);

        اگر (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                اگر (!p)
                        بازگشت -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                اگر (fd < 0)
                        بازگشت -errno;
        } وگرنه
                p = path;

        بازگشت mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

صحیح mkdirat_parents(صحیح dir_fd, ثابت نویسه *path, mode_t mode) {
        بازگشت mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ پوچ);
}

صحیح mkdir_parents_safe(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        بازگشت mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ پوچ);
}

صحیح mkdir_p_internal(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        صحیح r;

        /* Like mkdir -p */

        أكد(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        اگر (r < 0)
                بازگشت r;

        اگر (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                اگر (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        بازگشت r;
        } وگرنه {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                اگر (r < 0 && r != -EEXIST)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح mkdir_p(ثابت نویسه *path, mode_t mode) {
        بازگشت mkdir_p_internal(/* prefix= */ پوچ, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ پوچ);
}

صحیح mkdir_p_safe(ثابت نویسه *prefix, ثابت نویسه *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        بازگشت mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ پوچ);
}

صحیح mkdir_p_ریشه_full(ثابت نویسه *ریشه, ثابت نویسه *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ نویسه *pp = پوچ, *bn = پوچ;
        _cleanup_close_ صحیح dfd = -EBADF;
        صحیح r;

        أكد(p);

        r = path_extract_directory(p, &pp);
        اگر (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                اگر (dfd < 0)
                        بازگشت -errno;
        } وگرنه اگر (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                بازگشت 0;
        وگرنه اگر (r < 0)
                بازگشت r;
        وگرنه {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_ریشه_full(ریشه, pp, uid, gid, m, ts, subvolumes);
                اگر (r < 0)
                        بازگشت r;

                dfd = chase_and_open(pp, ریشه, CHASE_PREFIX_ریشه, O_CLOEXEC|O_DIRECTORY, پوچ);
                اگر (dfd < 0)
                        بازگشت dfd;
        }

        r = path_extract_filename(p, &bn);
        اگر (r == -EADDRNOTAVAIL) /* Already top-level */
                بازگشت 0;
        اگر (r < 0)
                بازگشت r;

        XOpenFlags flags = 0;
        اگر (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                اگر ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ صحیح nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        اگر (nfd == -EEXIST)
                بازگشت 0;
        اگر (nfd < 0)
                بازگشت nfd;

        اگر ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                بازگشت -errno;

        اگر (ts != USEC_INFINITY) {
                ساختار timespec tspec;
                timespec_store(&tspec, ts);

                اگر (futimens(dfd, (ثابت ساختار timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        بازگشت -errno;

                اگر (futimens(nfd, (ثابت ساختار timespec[2]) { tspec, tspec }) < 0)
                        بازگشت -errno;
        }

        بازگشت 1;
}
