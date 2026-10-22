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

صحیح mkdirat_محفوظ_اندرونی(
                صحیح dir_fd,
                ثابت حرف *راستہ,
                mode_t منوال,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ساخت stat st;
        صحیح r;

        أكد(راستہ);
        أكد(منوال != منوال_INVALID);
        أكد(_mkdirat);

        r = _mkdirat(dir_fd, راستہ, منوال, label_context);
        اگر (r >= 0)
                واپس chmod_and_chown_at(dir_fd, راستہ, منوال, uid, gid);
        اگر (r != -EEXIST)
                واپس r;

        اگر (fstatat(dir_fd, راستہ, &st, AT_SYMLINK_NOFOLLOW) < 0)
                واپس -errno;

        اگر ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ حرف *p = تہی;

                r = chaseat(XAT_FDROOT, dir_fd, راستہ, CHASE_NONEXISTENT, &p, تہی);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس mkdirat_محفوظ_اندرونی(dir_fd, p, منوال, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                اگر (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        واپس -errno;
        }

        اگر (flags & MKDIR_IGNORE_EXISTING)
                واپس 0;

        اگر (!S_ISDIR(st.st_mode))
                واپس log_بھرا_errno(flags & MKDIR_WARN_منوال ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", راستہ);

        اگر ((st.st_mode & ~منوال & 0777) != 0)
                واپس log_بھرا_errno(flags & MKDIR_WARN_منوال ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      راستہ, st.st_mode & 0777, منوال);

        اگر ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                واپس log_بھرا_errno(flags & MKDIR_WARN_منوال ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      راستہ, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        واپس 0;
}

صحیح mkdirat_errno_wrapper(صحیح dirfd, ثابت حرف *pathname, mode_t منوال, LabelContext *label_context) {
        واپس RET_NERRNO(mkdirat(dirfd, pathname, منوال));
}

صحیح mkdirat_محفوظ(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags) {
        واپس mkdirat_محفوظ_اندرونی(dir_fd, راستہ, منوال, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ تہی);
}

صحیح mkdirat_parents_اندرونی(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ثابت حرف *e = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(_mkdirat);

        اگر (isempty(راستہ))
                واپس 0;

        اگر (!راستہ_is_محفوظ(راستہ))
                واپس -ENOTDIR;

        /* return immediately if directory exists */
        r = راستہ_ڈھونڈو_آخری_component(راستہ, /* accept_dot_dot= */ false, &e, تہی);
        اگر (r <= 0) /* r == 0 means path is equivalent to prefix. */
                واپس r;
        اگر (e == راستہ)
                واپس 0;

        أكد(e > راستہ);
        أكد(*e == '/');

        /* drop the last component */
        راستہ = strndupa_محفوظ(راستہ, e - راستہ);
        r = is_dir_at(dir_fd, راستہ, /* follow= */ true);
        اگر (r > 0)
                واپس 0;
        اگر (r == 0)
                واپس -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        برائے (ثابت حرف *p = راستہ;;) {
                حرف *s;
                صحیح n;

                n = راستہ_ڈھونڈو_پہلا_component(&p, /* accept_dot_dot= */ false, (ثابت حرف **) &s);
                اگر (n <= 0)
                        واپس n;

                أكد(p);
                أكد(s >= راستہ);
                أكد(اندر_سیٹ(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_محفوظ_اندرونی(dir_fd, راستہ, منوال, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                اگر (r < 0 && r != -EEXIST)
                        واپس r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

صحیح mkdir_parents_اندرونی(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ صحیح fd = AT_FDCWD;
        ثابت حرف *p;

        أكد(راستہ);
        أكد(_mkdirat);

        اگر (سابقہ) {
                p = راستہ_startswith_بھرا(راستہ, سابقہ, راستہ_STARTSWITH_REFUSE_DOT_DOT);
                اگر (!p)
                        واپس -EINVAL;

                fd = open(سابقہ, O_راستہ|O_DIRECTORY|O_CLOEXEC);
                اگر (fd < 0)
                        واپس -errno;
        } ورنہ
                p = راستہ;

        واپس mkdirat_parents_اندرونی(fd, p, منوال, uid, gid, flags, _mkdirat, label_context);
}

صحیح mkdirat_parents(صحیح dir_fd, ثابت حرف *راستہ, mode_t منوال) {
        واپس mkdirat_parents_اندرونی(dir_fd, راستہ, منوال, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ تہی);
}

صحیح mkdir_parents_محفوظ(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags) {
        واپس mkdir_parents_اندرونی(سابقہ, راستہ, منوال, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ تہی);
}

صحیح mkdir_p_اندرونی(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        صحیح r;

        /* Like mkdir -p */

        أكد(_mkdirat);

        r = mkdir_parents_اندرونی(سابقہ, راستہ, منوال, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        اگر (r < 0)
                واپس r;

        اگر (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, راستہ, منوال, label_context);
                اگر (r < 0 && (r != -EEXIST || is_dir(راستہ, true) <= 0))
                        واپس r;
        } ورنہ {
                r = mkdir_محفوظ_اندرونی(راستہ, منوال, uid, gid, flags, _mkdirat, label_context);
                اگر (r < 0 && r != -EEXIST)
                        واپس r;
        }

        واپس 0;
}

صحیح mkdir_p(ثابت حرف *راستہ, mode_t منوال) {
        واپس mkdir_p_اندرونی(/* prefix= */ تہی, راستہ, منوال, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ تہی);
}

صحیح mkdir_p_محفوظ(ثابت حرف *سابقہ, ثابت حرف *راستہ, mode_t منوال, uid_t uid, gid_t gid, MkdirFlags flags) {
        واپس mkdir_p_اندرونی(سابقہ, راستہ, منوال, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ تہی);
}

صحیح mkdir_p_جڑ_بھرا(ثابت حرف *جڑ, ثابت حرف *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ حرف *pp = تہی, *bn = تہی;
        _cleanup_close_ صحیح dfd = -EBADF;
        صحیح r;

        أكد(p);

        r = راستہ_نکالو_directory(p, &pp);
        اگر (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                اگر (dfd < 0)
                        واپس -errno;
        } ورنہ اگر (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                واپس 0;
        ورنہ اگر (r < 0)
                واپس r;
        ورنہ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_جڑ_بھرا(جڑ, pp, uid, gid, m, ts, subvolumes);
                اگر (r < 0)
                        واپس r;

                dfd = chase_and_open(pp, جڑ, CHASE_سابقہ_جڑ, O_CLOEXEC|O_DIRECTORY, تہی);
                اگر (dfd < 0)
                        واپس dfd;
        }

        r = راستہ_نکالو_filename(p, &bn);
        اگر (r == -EADDRNOTAVAIL) /* Already top-level */
                واپس 0;
        اگر (r < 0)
                واپس r;

        XOpenFlags flags = 0;
        اگر (hashmap_شامل_ہے(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                اگر ((PTR_مقصد_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ صحیح nfd = xopenat_بھرا(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        اگر (nfd == -EEXIST)
                واپس 0;
        اگر (nfd < 0)
                واپس nfd;

        اگر ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                واپس -errno;

        اگر (ts != USEC_INFINITY) {
                ساخت timespec tspec;
                timespec_store(&tspec, ts);

                اگر (futimens(dfd, (ثابت ساخت timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        واپس -errno;

                اگر (futimens(nfd, (ثابت ساخت timespec[2]) { tspec, tspec }) < 0)
                        واپس -errno;
        }

        واپس 1;
}
