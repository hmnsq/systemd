/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "btrfs-util.h"
#समावेश "chase.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "format-util.h"
#समावेश "fs-util.h"
#समावेश "hashmap.h"
#समावेश "log.h"
#समावेश "mkdir.h"
#समावेश "path-util.h"
#समावेश "stat-util.h"
#समावेश "string-util.h"
#समावेश "time-util.h"
#समावेश "user-util.h"

पूर्णांक mkdirat_safe_internal(
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        काष्ठा stat st;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(mode != MODE_INVALID);
        निश्चित(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        अगर (r >= 0)
                वापस chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        अगर (r != -EEXIST)
                वापस r;

        अगर (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                वापस -त्रुटि_सं;

        अगर ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ अक्षर *p = शून्य;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, शून्य);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                अगर (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        वापस -त्रुटि_सं;
        }

        अगर (flags & MKDIR_IGNORE_EXISTING)
                वापस 0;

        अगर (!S_ISDIR(st.st_mode))
                वापस log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        अगर ((st.st_mode & ~mode & 0777) != 0)
                वापस log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        अगर ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                वापस log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        वापस 0;
}

पूर्णांक mkdirat_errno_wrapper(पूर्णांक dirfd, स्थिर अक्षर *pathname, mode_t mode, LabelContext *label_context) {
        वापस RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

पूर्णांक mkdirat_safe(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        वापस mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ शून्य);
}

पूर्णांक mkdirat_parents_internal(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        स्थिर अक्षर *e = शून्य;
        पूर्णांक r;

        निश्चित(path);
        निश्चित(_mkdirat);

        अगर (isempty(path))
                वापस 0;

        अगर (!path_is_safe(path))
                वापस -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, शून्य);
        अगर (r <= 0) /* r == 0 means path is equivalent to prefix. */
                वापस r;
        अगर (e == path)
                वापस 0;

        निश्चित(e > path);
        निश्चित(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        अगर (r > 0)
                वापस 0;
        अगर (r == 0)
                वापस -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        क्रम (स्थिर अक्षर *p = path;;) {
                अक्षर *s;
                पूर्णांक n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (स्थिर अक्षर **) &s);
                अगर (n <= 0)
                        वापस n;

                निश्चित(p);
                निश्चित(s >= path);
                निश्चित(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                अगर (r < 0 && r != -EEXIST)
                        वापस r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

पूर्णांक mkdir_parents_internal(स्थिर अक्षर *prefix, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ पूर्णांक fd = AT_FDCWD;
        स्थिर अक्षर *p;

        निश्चित(path);
        निश्चित(_mkdirat);

        अगर (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                अगर (!p)
                        वापस -EINVAL;

                fd = खोलो(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                अगर (fd < 0)
                        वापस -त्रुटि_सं;
        } अन्यथा
                p = path;

        वापस mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

पूर्णांक mkdirat_parents(पूर्णांक dir_fd, स्थिर अक्षर *path, mode_t mode) {
        वापस mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ शून्य);
}

पूर्णांक mkdir_parents_safe(स्थिर अक्षर *prefix, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        वापस mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ शून्य);
}

पूर्णांक mkdir_p_internal(स्थिर अक्षर *prefix, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        पूर्णांक r;

        /* Like mkdir -p */

        निश्चित(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        अगर (r < 0)
                वापस r;

        अगर (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                अगर (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        वापस r;
        } अन्यथा {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                अगर (r < 0 && r != -EEXIST)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक mkdir_p(स्थिर अक्षर *path, mode_t mode) {
        वापस mkdir_p_internal(/* prefix= */ शून्य, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ शून्य);
}

पूर्णांक mkdir_p_safe(स्थिर अक्षर *prefix, स्थिर अक्षर *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        वापस mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ शून्य);
}

पूर्णांक mkdir_p_मूल_full(स्थिर अक्षर *मूल, स्थिर अक्षर *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ अक्षर *pp = शून्य, *bn = शून्य;
        _cleanup_close_ पूर्णांक dfd = -EBADF;
        पूर्णांक r;

        निश्चित(p);

        r = path_extract_directory(p, &pp);
        अगर (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = खोलो(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                अगर (dfd < 0)
                        वापस -त्रुटि_सं;
        } अन्यथा अगर (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                वापस 0;
        अन्यथा अगर (r < 0)
                वापस r;
        अन्यथा {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_मूल_full(मूल, pp, uid, gid, m, ts, subvolumes);
                अगर (r < 0)
                        वापस r;

                dfd = chase_and_open(pp, मूल, CHASE_PREFIX_मूल, O_CLOEXEC|O_DIRECTORY, शून्य);
                अगर (dfd < 0)
                        वापस dfd;
        }

        r = path_extract_filename(p, &bn);
        अगर (r == -EADDRNOTAVAIL) /* Already top-level */
                वापस 0;
        अगर (r < 0)
                वापस r;

        XOpenFlags flags = 0;
        अगर (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                अगर ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ पूर्णांक nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        अगर (nfd == -EEXIST)
                वापस 0;
        अगर (nfd < 0)
                वापस nfd;

        अगर ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                वापस -त्रुटि_सं;

        अगर (ts != USEC_INFINITY) {
                काष्ठा timespec tspec;
                timespec_store(&tspec, ts);

                अगर (futimens(dfd, (स्थिर काष्ठा timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        वापस -त्रुटि_सं;

                अगर (futimens(nfd, (स्थिर काष्ठा timespec[2]) { tspec, tspec }) < 0)
                        वापस -त्रुटि_सं;
        }

        वापस 1;
}
