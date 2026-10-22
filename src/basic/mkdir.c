/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "btrfs-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "hashmap.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mkdir.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "user-util.h"

पूर्णांक mkdirat_safe_internal(
                पूर्णांक dir_fd,
                स्थिरांक अक्षर *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        संरचना stat st;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode != MODE_INVALID);
        प्रतिज्ञा(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        जर (r >= 0)
                परत chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        जर (r != -EEXIST)
                परत r;

        जर (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                परत -त्रुटि_सं;

        जर ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ अक्षर *p = रिक्त;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, रिक्त);
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        परत mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                जर (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        परत -त्रुटि_सं;
        }

        जर (flags & MKDIR_IGNORE_EXISTING)
                परत 0;

        जर (!S_ISDIR(st.st_mode))
                परत log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        जर ((st.st_mode & ~mode & 0777) != 0)
                परत log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        जर ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                परत log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        परत 0;
}

पूर्णांक mkdirat_errno_wrapper(पूर्णांक dirfd, स्थिरांक अक्षर *pathname, mode_t mode, LabelContext *label_context) {
        परत RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

पूर्णांक mkdirat_safe(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        परत mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णांक mkdirat_parents_internal(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        स्थिरांक अक्षर *e = रिक्त;
        पूर्णांक r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(_mkdirat);

        जर (isempty(path))
                परत 0;

        जर (!path_is_safe(path))
                परत -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, रिक्त);
        जर (r <= 0) /* r == 0 means path is equivalent to prefix. */
                परत r;
        जर (e == path)
                परत 0;

        प्रतिज्ञा(e > path);
        प्रतिज्ञा(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        जर (r > 0)
                परत 0;
        जर (r == 0)
                परत -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        साठी (स्थिरांक अक्षर *p = path;;) {
                अक्षर *s;
                पूर्णांक n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (स्थिरांक अक्षर **) &s);
                जर (n <= 0)
                        परत n;

                प्रतिज्ञा(p);
                प्रतिज्ञा(s >= path);
                प्रतिज्ञा(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                जर (r < 0 && r != -EEXIST)
                        परत r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

पूर्णांक mkdir_parents_internal(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ पूर्णांक fd = AT_FDCWD;
        स्थिरांक अक्षर *p;

        प्रतिज्ञा(path);
        प्रतिज्ञा(_mkdirat);

        जर (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                जर (!p)
                        परत -EINVAL;

                fd = खोलो(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                जर (fd < 0)
                        परत -त्रुटि_सं;
        } अन्यथा
                p = path;

        परत mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

पूर्णांक mkdirat_parents(पूर्णांक dir_fd, स्थिरांक अक्षर *path, mode_t mode) {
        परत mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णांक mkdir_parents_safe(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        परत mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णांक mkdir_p_internal(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        पूर्णांक r;

        /* Like mkdir -p */

        प्रतिज्ञा(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        जर (r < 0)
                परत r;

        जर (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                जर (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        परत r;
        } अन्यथा {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                जर (r < 0 && r != -EEXIST)
                        परत r;
        }

        परत 0;
}

पूर्णांक mkdir_p(स्थिरांक अक्षर *path, mode_t mode) {
        परत mkdir_p_internal(/* prefix= */ रिक्त, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णांक mkdir_p_safe(स्थिरांक अक्षर *prefix, स्थिरांक अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        परत mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णांक mkdir_p_मूळ_full(स्थिरांक अक्षर *मूळ, स्थिरांक अक्षर *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ अक्षर *pp = रिक्त, *bn = रिक्त;
        _cleanup_close_ पूर्णांक dfd = -EBADF;
        पूर्णांक r;

        प्रतिज्ञा(p);

        r = path_extract_directory(p, &pp);
        जर (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = खोलो(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                जर (dfd < 0)
                        परत -त्रुटि_सं;
        } अन्यथा जर (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                परत 0;
        अन्यथा जर (r < 0)
                परत r;
        अन्यथा {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_मूळ_full(मूळ, pp, uid, gid, m, ts, subvolumes);
                जर (r < 0)
                        परत r;

                dfd = chase_and_open(pp, मूळ, CHASE_PREFIX_मूळ, O_CLOEXEC|O_DIRECTORY, रिक्त);
                जर (dfd < 0)
                        परत dfd;
        }

        r = path_extract_filename(p, &bn);
        जर (r == -EADDRNOTAVAIL) /* Already top-level */
                परत 0;
        जर (r < 0)
                परत r;

        XOpenFlags flags = 0;
        जर (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                जर ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ पूर्णांक nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        जर (nfd == -EEXIST)
                परत 0;
        जर (nfd < 0)
                परत nfd;

        जर ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                परत -त्रुटि_सं;

        जर (ts != USEC_INFINITY) {
                संरचना timespec tspec;
                timespec_store(&tspec, ts);

                जर (futimens(dfd, (स्थिरांक संरचना timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        परत -त्रुटि_सं;

                जर (futimens(nfd, (स्थिरांक संरचना timespec[2]) { tspec, tspec }) < 0)
                        परत -त्रुटि_सं;
        }

        परत 1;
}
