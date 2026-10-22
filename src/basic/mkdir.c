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

पूर्णाङ्क mkdirat_safe_internal(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        संरचना stat st;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(mode != MODE_INVALID);
        प्रतिज्ञा(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        यदि (r >= 0)
                प्रत्यागम chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        यदि (r != -EEXIST)
                प्रत्यागम r;

        यदि (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                प्रत्यागम -errno;

        यदि ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ वर्ण *p = रिक्त;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, रिक्त);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                यदि (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        प्रत्यागम -errno;
        }

        यदि (flags & MKDIR_IGNORE_EXISTING)
                प्रत्यागम 0;

        यदि (!S_ISDIR(st.st_mode))
                प्रत्यागम log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        यदि ((st.st_mode & ~mode & 0777) != 0)
                प्रत्यागम log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        यदि ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                प्रत्यागम log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        प्रत्यागम 0;
}

पूर्णाङ्क mkdirat_errno_wrapper(पूर्णाङ्क dirfd, ध्रुव वर्ण *pathname, mode_t mode, LabelContext *label_context) {
        प्रत्यागम RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

पूर्णाङ्क mkdirat_safe(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        प्रत्यागम mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णाङ्क mkdirat_parents_internal(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ध्रुव वर्ण *e = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(_mkdirat);

        यदि (isempty(path))
                प्रत्यागम 0;

        यदि (!path_is_safe(path))
                प्रत्यागम -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, रिक्त);
        यदि (r <= 0) /* r == 0 means path is equivalent to prefix. */
                प्रत्यागम r;
        यदि (e == path)
                प्रत्यागम 0;

        प्रतिज्ञा(e > path);
        प्रतिज्ञा(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        यदि (r > 0)
                प्रत्यागम 0;
        यदि (r == 0)
                प्रत्यागम -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        प्रति (ध्रुव वर्ण *p = path;;) {
                वर्ण *s;
                पूर्णाङ्क n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ध्रुव वर्ण **) &s);
                यदि (n <= 0)
                        प्रत्यागम n;

                प्रतिज्ञा(p);
                प्रतिज्ञा(s >= path);
                प्रतिज्ञा(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                यदि (r < 0 && r != -EEXIST)
                        प्रत्यागम r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

पूर्णाङ्क mkdir_parents_internal(ध्रुव वर्ण *prefix, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ पूर्णाङ्क fd = AT_FDCWD;
        ध्रुव वर्ण *p;

        प्रतिज्ञा(path);
        प्रतिज्ञा(_mkdirat);

        यदि (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                यदि (!p)
                        प्रत्यागम -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                यदि (fd < 0)
                        प्रत्यागम -errno;
        } अन्यथा
                p = path;

        प्रत्यागम mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

पूर्णाङ्क mkdirat_parents(पूर्णाङ्क dir_fd, ध्रुव वर्ण *path, mode_t mode) {
        प्रत्यागम mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णाङ्क mkdir_parents_safe(ध्रुव वर्ण *prefix, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        प्रत्यागम mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णाङ्क mkdir_p_internal(ध्रुव वर्ण *prefix, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        पूर्णाङ्क r;

        /* Like mkdir -p */

        प्रतिज्ञा(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                यदि (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        प्रत्यागम r;
        } अन्यथा {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                यदि (r < 0 && r != -EEXIST)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क mkdir_p(ध्रुव वर्ण *path, mode_t mode) {
        प्रत्यागम mkdir_p_internal(/* prefix= */ रिक्त, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णाङ्क mkdir_p_safe(ध्रुव वर्ण *prefix, ध्रुव वर्ण *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        प्रत्यागम mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ रिक्त);
}

पूर्णाङ्क mkdir_p_मूल_full(ध्रुव वर्ण *मूल, ध्रुव वर्ण *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ वर्ण *pp = रिक्त, *bn = रिक्त;
        _cleanup_close_ पूर्णाङ्क dfd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);

        r = path_extract_directory(p, &pp);
        यदि (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                यदि (dfd < 0)
                        प्रत्यागम -errno;
        } अन्यथा यदि (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                प्रत्यागम 0;
        अन्यथा यदि (r < 0)
                प्रत्यागम r;
        अन्यथा {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_मूल_full(मूल, pp, uid, gid, m, ts, subvolumes);
                यदि (r < 0)
                        प्रत्यागम r;

                dfd = chase_and_open(pp, मूल, CHASE_PREFIX_मूल, O_CLOEXEC|O_DIRECTORY, रिक्त);
                यदि (dfd < 0)
                        प्रत्यागम dfd;
        }

        r = path_extract_filename(p, &bn);
        यदि (r == -EADDRNOTAVAIL) /* Already top-level */
                प्रत्यागम 0;
        यदि (r < 0)
                प्रत्यागम r;

        XOpenFlags flags = 0;
        यदि (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                यदि ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ पूर्णाङ्क nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        यदि (nfd == -EEXIST)
                प्रत्यागम 0;
        यदि (nfd < 0)
                प्रत्यागम nfd;

        यदि ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                प्रत्यागम -errno;

        यदि (ts != USEC_INFINITY) {
                संरचना timespec tspec;
                timespec_store(&tspec, ts);

                यदि (futimens(dfd, (ध्रुव संरचना timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        प्रत्यागम -errno;

                यदि (futimens(nfd, (ध्रुव संरचना timespec[2]) { tspec, tspec }) < 0)
                        प्रत्यागम -errno;
        }

        प्रत्यागम 1;
}
