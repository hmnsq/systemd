/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <unistd.h>

#include "alloc-util.h"
#include "btrfs-util.h"
#include "chase.h"
#include "errno-util.h"
#include "fd-util.h"
#include "format-util.h"
#include "fs-util.h"
#include "hashmap.h"
#include "log.h"
#include "mkdir.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "time-util.h"
#include "user-util.h"

цяло mkdirat_safe_internal(
                цяло dir_fd,
                const char *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        struct stat st;
        цяло r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ако (r >= 0)
                върни chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ако (r != -EEXIST)
                върни r;

        ако (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                върни -errno;

        ако ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ char *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                ако (r < 0)
                        върни r;
                ако (r == 0)
                        върни mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ако (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        върни -errno;
        }

        ако (flags & MKDIR_IGNORE_EXISTING)
                върни 0;

        ако (!S_ISDIR(st.st_mode))
                върни log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ако ((st.st_mode & ~mode & 0777) != 0)
                върни log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ако ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                върни log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        върни 0;
}

цяло mkdirat_errno_wrapper(цяло dirfd, const char *pathname, mode_t mode, LabelContext *label_context) {
        върни RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

цяло mkdirat_safe(цяло dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        върни mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

цяло mkdirat_parents_internal(цяло dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const char *e = NULL;
        цяло r;

        assert(path);
        assert(_mkdirat);

        ако (isempty(path))
                върни 0;

        ако (!path_is_safe(path))
                върни -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        ако (r <= 0) /* r == 0 means path is equivalent to prefix. */
                върни r;
        ако (e == path)
                върни 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ако (r > 0)
                върни 0;
        ако (r == 0)
                върни -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        за (const char *p = path;;) {
                char *s;
                цяло n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const char **) &s);
                ако (n <= 0)
                        върни n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ако (r < 0 && r != -EEXIST)
                        върни r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

цяло mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ цяло fd = AT_FDCWD;
        const char *p;

        assert(path);
        assert(_mkdirat);

        ако (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ако (!p)
                        върни -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ако (fd < 0)
                        върни -errno;
        } иначе
                p = path;

        върни mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

цяло mkdirat_parents(цяло dir_fd, const char *path, mode_t mode) {
        върни mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

цяло mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        върни mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

цяло mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        цяло r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ако (r < 0)
                върни r;

        ако (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ако (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        върни r;
        } иначе {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ако (r < 0 && r != -EEXIST)
                        върни r;
        }

        върни 0;
}

цяло mkdir_p(const char *path, mode_t mode) {
        върни mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

цяло mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        върни mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

цяло mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ char *pp = NULL, *bn = NULL;
        _cleanup_close_ цяло dfd = -EBADF;
        цяло r;

        assert(p);

        r = path_extract_directory(p, &pp);
        ако (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ако (dfd < 0)
                        върни -errno;
        } иначе ако (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                върни 0;
        иначе ако (r < 0)
                върни r;
        иначе {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ако (r < 0)
                        върни r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                ако (dfd < 0)
                        върни dfd;
        }

        r = path_extract_filename(p, &bn);
        ако (r == -EADDRNOTAVAIL) /* Already top-level */
                върни 0;
        ако (r < 0)
                върни r;

        XOpenFlags flags = 0;
        ако (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ако ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ цяло nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ако (nfd == -EEXIST)
                върни 0;
        ако (nfd < 0)
                върни nfd;

        ако ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                върни -errno;

        ако (ts != USEC_INFINITY) {
                struct timespec tspec;
                timespec_store(&tspec, ts);

                ако (futimens(dfd, (const struct timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        върни -errno;

                ако (futimens(nfd, (const struct timespec[2]) { tspec, tspec }) < 0)
                        върни -errno;
        }

        върни 1;
}
