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

صحيح mkdirat_محفوظ_اندروني(
                صحيح dir_fd,
                ثابت حرف *رستو,
                mode_t گهڻائي,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        بنية stat st;
        صحيح r;

        أكد(رستو);
        أكد(گهڻائي != گهڻائي_INVALID);
        أكد(_mkdirat);

        r = _mkdirat(dir_fd, رستو, گهڻائي, label_context);
        إذا (r >= 0)
                أرجع chmod_and_chown_at(dir_fd, رستو, گهڻائي, uid, gid);
        إذا (r != -EEXIST)
                أرجع r;

        إذا (fstatat(dir_fd, رستو, &st, AT_SYMLINK_NOFOLLOW) < 0)
                أرجع -errno;

        إذا ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ حرف *p = عدم;

                r = chaseat(XAT_FDROOT, dir_fd, رستو, CHASE_NONEXISTENT, &p, عدم);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع mkdirat_محفوظ_اندروني(dir_fd, p, گهڻائي, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                إذا (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        أرجع -errno;
        }

        إذا (flags & MKDIR_IGNORE_EXISTING)
                أرجع 0;

        إذا (!S_ISDIR(st.st_mode))
                أرجع log_ڀريل_errno(flags & MKDIR_WARN_گهڻائي ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", رستو);

        إذا ((st.st_mode & ~گهڻائي & 0777) != 0)
                أرجع log_ڀريل_errno(flags & MKDIR_WARN_گهڻائي ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      رستو, st.st_mode & 0777, گهڻائي);

        إذا ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                أرجع log_ڀريل_errno(flags & MKDIR_WARN_گهڻائي ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      رستو, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        أرجع 0;
}

صحيح mkdirat_errno_wrapper(صحيح dirfd, ثابت حرف *pathname, mode_t گهڻائي, LabelContext *label_context) {
        أرجع RET_NERRNO(mkdirat(dirfd, pathname, گهڻائي));
}

صحيح mkdirat_محفوظ(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdirat_محفوظ_اندروني(dir_fd, رستو, گهڻائي, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdirat_parents_اندروني(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ثابت حرف *e = عدم;
        صحيح r;

        أكد(رستو);
        أكد(_mkdirat);

        إذا (isempty(رستو))
                أرجع 0;

        إذا (!رستو_is_محفوظ(رستو))
                أرجع -ENOTDIR;

        /* return immediately if directory exists */
        r = رستو_ڳوليو_آخري_component(رستو, /* accept_dot_dot= */ false, &e, عدم);
        إذا (r <= 0) /* r == 0 means path is equivalent to prefix. */
                أرجع r;
        إذا (e == رستو)
                أرجع 0;

        أكد(e > رستو);
        أكد(*e == '/');

        /* drop the last component */
        رستو = strndupa_محفوظ(رستو, e - رستو);
        r = is_dir_at(dir_fd, رستو, /* follow= */ true);
        إذا (r > 0)
                أرجع 0;
        إذا (r == 0)
                أرجع -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        لكل (ثابت حرف *p = رستو;;) {
                حرف *s;
                صحيح n;

                n = رستو_ڳوليو_پهريون_component(&p, /* accept_dot_dot= */ false, (ثابت حرف **) &s);
                إذا (n <= 0)
                        أرجع n;

                أكد(p);
                أكد(s >= رستو);
                أكد(اندريون_سيٽ(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_محفوظ_اندروني(dir_fd, رستو, گهڻائي, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

صحيح mkdir_parents_اندروني(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ صحيح fd = AT_FDCWD;
        ثابت حرف *p;

        أكد(رستو);
        أكد(_mkdirat);

        إذا (اڳياڙي) {
                p = رستو_startswith_ڀريل(رستو, اڳياڙي, رستو_STARTSWITH_REFUSE_DOT_DOT);
                إذا (!p)
                        أرجع -EINVAL;

                fd = open(اڳياڙي, O_رستو|O_DIRECTORY|O_CLOEXEC);
                إذا (fd < 0)
                        أرجع -errno;
        } وإلا
                p = رستو;

        أرجع mkdirat_parents_اندروني(fd, p, گهڻائي, uid, gid, flags, _mkdirat, label_context);
}

صحيح mkdirat_parents(صحيح dir_fd, ثابت حرف *رستو, mode_t گهڻائي) {
        أرجع mkdirat_parents_اندروني(dir_fd, رستو, گهڻائي, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_parents_محفوظ(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdir_parents_اندروني(اڳياڙي, رستو, گهڻائي, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_اندروني(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        صحيح r;

        /* Like mkdir -p */

        أكد(_mkdirat);

        r = mkdir_parents_اندروني(اڳياڙي, رستو, گهڻائي, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        إذا (r < 0)
                أرجع r;

        إذا (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, رستو, گهڻائي, label_context);
                إذا (r < 0 && (r != -EEXIST || is_dir(رستو, true) <= 0))
                        أرجع r;
        } وإلا {
                r = mkdir_محفوظ_اندروني(رستو, گهڻائي, uid, gid, flags, _mkdirat, label_context);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح mkdir_p(ثابت حرف *رستو, mode_t گهڻائي) {
        أرجع mkdir_p_اندروني(/* prefix= */ عدم, رستو, گهڻائي, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_محفوظ(ثابت حرف *اڳياڙي, ثابت حرف *رستو, mode_t گهڻائي, uid_t uid, gid_t gid, MkdirFlags flags) {
        أرجع mkdir_p_اندروني(اڳياڙي, رستو, گهڻائي, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ عدم);
}

صحيح mkdir_p_پاڙ_ڀريل(ثابت حرف *پاڙ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ حرف *pp = عدم, *bn = عدم;
        _cleanup_close_ صحيح dfd = -EBADF;
        صحيح r;

        أكد(p);

        r = رستو_ڪڍو_directory(p, &pp);
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
                r = mkdir_p_پاڙ_ڀريل(پاڙ, pp, uid, gid, m, ts, subvolumes);
                إذا (r < 0)
                        أرجع r;

                dfd = chase_and_open(pp, پاڙ, CHASE_اڳياڙي_پاڙ, O_CLOEXEC|O_DIRECTORY, عدم);
                إذا (dfd < 0)
                        أرجع dfd;
        }

        r = رستو_ڪڍو_filename(p, &bn);
        إذا (r == -EADDRNOTAVAIL) /* Already top-level */
                أرجع 0;
        إذا (r < 0)
                أرجع r;

        XOpenFlags flags = 0;
        إذا (hashmap_شامل_آهي(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                إذا ((PTR_منزل_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ صحيح nfd = xopenat_ڀريل(
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
