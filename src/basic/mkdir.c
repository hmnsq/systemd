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

целое mkdirat_safe_internal(
                целое dir_fd,
                const char *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        struct stat st;
        целое r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        если (r >= 0)
                вернуть chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        если (r != -EEXIST)
                вернуть r;

        если (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                вернуть -errno;

        если ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ char *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        вернуть mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                если (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        вернуть -errno;
        }

        если (flags & MKDIR_IGNORE_EXISTING)
                вернуть 0;

        если (!S_ISDIR(st.st_mode))
                вернуть log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        если ((st.st_mode & ~mode & 0777) != 0)
                вернуть log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        если ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                вернуть log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        вернуть 0;
}

целое mkdirat_errno_wrapper(целое dirfd, const char *pathname, mode_t mode, LabelContext *label_context) {
        вернуть RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

целое mkdirat_safe(целое dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        вернуть mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

целое mkdirat_parents_internal(целое dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const char *e = NULL;
        целое r;

        assert(path);
        assert(_mkdirat);

        если (isempty(path))
                вернуть 0;

        если (!path_is_safe(path))
                вернуть -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        если (r <= 0) /* r == 0 means path is equivalent to prefix. */
                вернуть r;
        если (e == path)
                вернуть 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        если (r > 0)
                вернуть 0;
        если (r == 0)
                вернуть -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        для (const char *p = path;;) {
                char *s;
                целое n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const char **) &s);
                если (n <= 0)
                        вернуть n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                если (r < 0 && r != -EEXIST)
                        вернуть r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

целое mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ целое fd = AT_FDCWD;
        const char *p;

        assert(path);
        assert(_mkdirat);

        если (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                если (!p)
                        вернуть -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                если (fd < 0)
                        вернуть -errno;
        } иначе
                p = path;

        вернуть mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

целое mkdirat_parents(целое dir_fd, const char *path, mode_t mode) {
        вернуть mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

целое mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        вернуть mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

целое mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        целое r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        если (r < 0)
                вернуть r;

        если (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                если (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        вернуть r;
        } иначе {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                если (r < 0 && r != -EEXIST)
                        вернуть r;
        }

        вернуть 0;
}

целое mkdir_p(const char *path, mode_t mode) {
        вернуть mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

целое mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        вернуть mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

целое mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ char *pp = NULL, *bn = NULL;
        _cleanup_close_ целое dfd = -EBADF;
        целое r;

        assert(p);

        r = path_extract_directory(p, &pp);
        если (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                если (dfd < 0)
                        вернуть -errno;
        } иначе если (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                вернуть 0;
        иначе если (r < 0)
                вернуть r;
        иначе {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                если (r < 0)
                        вернуть r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                если (dfd < 0)
                        вернуть dfd;
        }

        r = path_extract_filename(p, &bn);
        если (r == -EADDRNOTAVAIL) /* Already top-level */
                вернуть 0;
        если (r < 0)
                вернуть r;

        XOpenFlags flags = 0;
        если (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                если ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ целое nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        если (nfd == -EEXIST)
                вернуть 0;
        если (nfd < 0)
                вернуть nfd;

        если ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                вернуть -errno;

        если (ts != USEC_INFINITY) {
                struct timespec tspec;
                timespec_store(&tspec, ts);

                если (futimens(dfd, (const struct timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        вернуть -errno;

                если (futimens(nfd, (const struct timespec[2]) { tspec, tspec }) < 0)
                        вернуть -errno;
        }

        вернуть 1;
}
