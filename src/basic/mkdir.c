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

整数 mkdirat_safe_internal(
                整数 dir_fd,
                const char *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        struct stat st;
        整数 r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        如果 (r >= 0)
                返回 chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        如果 (r != -EEXIST)
                返回 r;

        如果 (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                返回 -errno;

        如果 ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ char *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        返回 mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                如果 (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        返回 -errno;
        }

        如果 (flags & MKDIR_IGNORE_EXISTING)
                返回 0;

        如果 (!S_ISDIR(st.st_mode))
                返回 log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        如果 ((st.st_mode & ~mode & 0777) != 0)
                返回 log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        如果 ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                返回 log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        返回 0;
}

整数 mkdirat_errno_wrapper(整数 dirfd, const char *pathname, mode_t mode, LabelContext *label_context) {
        返回 RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

整数 mkdirat_safe(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        返回 mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

整数 mkdirat_parents_internal(整数 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const char *e = NULL;
        整数 r;

        assert(path);
        assert(_mkdirat);

        如果 (isempty(path))
                返回 0;

        如果 (!path_is_safe(path))
                返回 -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        如果 (r <= 0) /* r == 0 means path is equivalent to prefix. */
                返回 r;
        如果 (e == path)
                返回 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        如果 (r > 0)
                返回 0;
        如果 (r == 0)
                返回 -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        为 (const char *p = path;;) {
                char *s;
                整数 n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const char **) &s);
                如果 (n <= 0)
                        返回 n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                如果 (r < 0 && r != -EEXIST)
                        返回 r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

整数 mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ 整数 fd = AT_FDCWD;
        const char *p;

        assert(path);
        assert(_mkdirat);

        如果 (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                如果 (!p)
                        返回 -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                如果 (fd < 0)
                        返回 -errno;
        } 否则
                p = path;

        返回 mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

整数 mkdirat_parents(整数 dir_fd, const char *path, mode_t mode) {
        返回 mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

整数 mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        返回 mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

整数 mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        整数 r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        如果 (r < 0)
                返回 r;

        如果 (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                如果 (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        返回 r;
        } 否则 {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                如果 (r < 0 && r != -EEXIST)
                        返回 r;
        }

        返回 0;
}

整数 mkdir_p(const char *path, mode_t mode) {
        返回 mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

整数 mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        返回 mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

整数 mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ char *pp = NULL, *bn = NULL;
        _cleanup_close_ 整数 dfd = -EBADF;
        整数 r;

        assert(p);

        r = path_extract_directory(p, &pp);
        如果 (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                如果 (dfd < 0)
                        返回 -errno;
        } 否则 如果 (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                返回 0;
        否则 如果 (r < 0)
                返回 r;
        否则 {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                如果 (r < 0)
                        返回 r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                如果 (dfd < 0)
                        返回 dfd;
        }

        r = path_extract_filename(p, &bn);
        如果 (r == -EADDRNOTAVAIL) /* Already top-level */
                返回 0;
        如果 (r < 0)
                返回 r;

        XOpenFlags flags = 0;
        如果 (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                如果 ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ 整数 nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        如果 (nfd == -EEXIST)
                返回 0;
        如果 (nfd < 0)
                返回 nfd;

        如果 ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                返回 -errno;

        如果 (ts != USEC_INFINITY) {
                struct timespec tspec;
                timespec_store(&tspec, ts);

                如果 (futimens(dfd, (const struct timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        返回 -errno;

                如果 (futimens(nfd, (const struct timespec[2]) { tspec, tspec }) < 0)
                        返回 -errno;
        }

        返回 1;
}
