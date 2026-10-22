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

صحيح mkdirat_خوندي_دننی(
                صحيح dir_fd,
                ثابت حرف *لار,
                mode_t ډېرپېښ,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        بنية stat st;
        صحيح r;

        أكد(لار);
        أكد(ډېرپېښ != ډېرپېښ_INVALID);
        أكد(_mkdirat);

        r = _mkdirat(dir_fd, لار, ډېرپېښ, label_context);
        إذا (r >= 0)
                أرجع chmod_and_chown_at(dir_fd, لار, ډېرپېښ, uid, gid);
        إذا (r != -EEXIST)
                أرجع r;

        إذا (fstatat(dir_fd, لار, &st, AT_SYMLINK_NOFOLLOW) < 0)
                أرجع -errno;

        إذا ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ حرف *p = عدم;

                r = chaseat(XAT_FDROOT, dir_fd, لار, CHASE_NONEXISTENT, &p, عدم);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع mkdirat_خوندي_دننی(dir_fd, p, ډېرپېښ, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                إذا (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        أرجع -errno;
        }

        إذا (flags & MKDIR_IGNORE_EXISTING)
                أرجع 0;

        إذا (!S_ISDIR(st.st_mode))
                أرجع log_ډک_errno(flags & MKDIR_WARN_ډېرپېښ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", لار);

        إذا ((st.st_mode & ~ډېرپېښ & 0777) != 0)
                أرجع log_ډک_errno(flags & MKDIR_WARN_ډېرپېښ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      لار, st.st_mode & 0777, ډېرپېښ);

        إذا ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                أرجع log_ډک_errno(flags & MKDIR_WARN_ډېرپېښ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      لار, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        أرجع 0;
}

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t ډېرپېښ, LabelContext *label_context) {
        أرجع RET_NERRNO(mkdirat(dirfd, pathname, ډېرپېښ));
}

صحيح mkdirat_خوندي(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_خوندي_دننی(dir_fd, لار, ډېرپېښ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdirat_parents_دننی(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ثابت حرف *e = عدم;
        صحيح r;

        أكد(لار);
        أكد(_mkdirat);

        إذا (isempty(لار))
                أرجع 0;

        إذا (!لار_is_خوندي(لار))
                أرجع -ENOTDIR;

        /* return immediately if directory exists */
        r = لار_پیدا_کړه_وروستی_component(لار, /* accept_dot_dot= */ false, &e, عدم);
        إذا (r <= 0) /* r == 0 means path is equivalent to prefix. */
                أرجع r;
        إذا (e == لار)
                أرجع 0;

        أكد(e > لار);
        أكد(*e == '/');

        /* drop the last component */
        لار = strndupa_خوندي(لار, e - لار);
        r = is_dir_at(dir_fd, لار, /* follow= */ true);
        إذا (r > 0)
                أرجع 0;
        إذا (r == 0)
                أرجع -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        لكل (ثابت حرف *p = لار;;) {
                حرف *s;
                صحيح n;

                n = لار_پیدا_کړه_لومړی_component(&p, /* accept_dot_dot= */ false, (ثابت حرف **) &s);
                إذا (n <= 0)
                        أرجع n;

                أكد(p);
                أكد(s >= لار);
                أكد(دننه_سېټ(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_خوندي_دننی(dir_fd, لار, ډېرپېښ, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

صحيح mkdir_parents_دننی(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ صحيح fd = AT_FDCWD;
        ثابت حرف *p;

        أكد(لار);
        أكد(_mkdirat);

        إذا (مختاړی) {
                p = لار_startswith_ډک(لار, مختاړی, لار_STARTSWITH_REFUSE_DOT_DOT);
                إذا (!p)
                        أرجع -EINVAL;

                fd = open(مختاړی, O_لار|O_DIRECTORY|O_CLOEXEC);
                إذا (fd < 0)
                        أرجع -errno;
        } وإلا
                p = لار;

        أرجع mkdirat_parents_دننی(fd, p, ډېرپېښ, uid, gid, flags, _mkdirat, label_context);
}

صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *لار, mode_t ډېرپېښ) {
        أرجع mkdirat_parents_دننی(dir_fd, لار, ډېرپېښ, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_parents_خوندي(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdir_parents_دننی(مختاړی, لار, ډېرپېښ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_دننی(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        صحيح r;

        /* Like mkdir -p */

        أكد(_mkdirat);

        r = mkdir_parents_دننی(مختاړی, لار, ډېرپېښ, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        إذا (r < 0)
                أرجع r;

        إذا (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, لار, ډېرپېښ, label_context);
                إذا (r < 0 && (r != -EEXIST || is_dir(لار, true) <= 0))
                        أرجع r;
        } وإلا {
                r = mkdir_خوندي_دننی(لار, ډېرپېښ, uid, gid, flags, _mkdirat, label_context);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح mkdir_p(ثابت حرف *لار, mode_t ډېرپېښ) {
        أرجع mkdir_p_دننی(/* prefix= */ عدم, لار, ډېرپېښ, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_خوندي(ثابت حرف *مختاړی, ثابت حرف *لار, mode_t ډېرپېښ, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdir_p_دننی(مختاړی, لار, ډېرپېښ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_ریښه_ډک(ثابت حرف *ریښه, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ حرف *pp = عدم, *bn = عدم;
        _cleanup_close_ صحيح dfd = -EBADF;
        صحيح r;

        أكد(p);

        r = لار_ایستل_directory(p, &pp);
        إذا (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                إذا (dfd < 0)
                        أرجع -errno;
        } وإلا إذا (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                أرجع 0;
        وإلا إذا (r < 0)
                أرجع r;
        وإلا {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_ریښه_ډک(ریښه, pp, uid, gid, m, ts, subvolumes);
                إذا (r < 0)
                        أرجع r;

                dfd = chase_and_open(pp, ریښه, CHASE_مختاړی_ریښه, O_CLOEXEC|O_DIRECTORY, عدم);
                إذا (dfd < 0)
                        أرجع dfd;
        }

        r = لار_ایستل_filename(p, &bn);
        إذا (r == -EADDRNOTAVAIL) /* Already top-level */
                أرجع 0;
        إذا (r < 0)
                أرجع r;

        XOpenFlags flags = 0;
        إذا (hashmap_لري(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                إذا ((PTR_منزل_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ صحيح nfd = xopenat_ډک(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        إذا (nfd == -EEXIST)
                أرجع 0;
        إذا (nfd < 0)
                أرجع nfd;

        إذا ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                أرجع -errno;

        إذا (ts != USEC_INFINITY) {
                بنية timespec tspec;
                timespec_store(&tspec, ts);

                إذا (futimens(dfd, (ثابت بنية timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        أرجع -errno;

                إذا (futimens(nfd, (ثابت بنية timespec[2]) { tspec, tspec }) < 0)
                        أرجع -errno;
        }

        أرجع 1;
}
