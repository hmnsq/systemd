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

정수 mkdirat_safe_internal(
                정수 dir_fd,
                const char *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        struct stat st;
        정수 r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        만약 (r >= 0)
                반환 chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        만약 (r != -EEXIST)
                반환 r;

        만약 (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                반환 -errno;

        만약 ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ char *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                만약 (r < 0)
                        반환 r;
                만약 (r == 0)
                        반환 mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                만약 (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        반환 -errno;
        }

        만약 (flags & MKDIR_IGNORE_EXISTING)
                반환 0;

        만약 (!S_ISDIR(st.st_mode))
                반환 log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        만약 ((st.st_mode & ~mode & 0777) != 0)
                반환 log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        만약 ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                반환 log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        반환 0;
}

정수 mkdirat_errno_wrapper(정수 dirfd, const char *pathname, mode_t mode, LabelContext *label_context) {
        반환 RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

정수 mkdirat_safe(정수 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        반환 mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

정수 mkdirat_parents_internal(정수 dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const char *e = NULL;
        정수 r;

        assert(path);
        assert(_mkdirat);

        만약 (isempty(path))
                반환 0;

        만약 (!path_is_safe(path))
                반환 -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        만약 (r <= 0) /* r == 0 means path is equivalent to prefix. */
                반환 r;
        만약 (e == path)
                반환 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        만약 (r > 0)
                반환 0;
        만약 (r == 0)
                반환 -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        위해 (const char *p = path;;) {
                char *s;
                정수 n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const char **) &s);
                만약 (n <= 0)
                        반환 n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                만약 (r < 0 && r != -EEXIST)
                        반환 r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

정수 mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ 정수 fd = AT_FDCWD;
        const char *p;

        assert(path);
        assert(_mkdirat);

        만약 (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                만약 (!p)
                        반환 -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                만약 (fd < 0)
                        반환 -errno;
        } 아니면
                p = path;

        반환 mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

정수 mkdirat_parents(정수 dir_fd, const char *path, mode_t mode) {
        반환 mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

정수 mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        반환 mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

정수 mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        정수 r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        만약 (r < 0)
                반환 r;

        만약 (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                만약 (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        반환 r;
        } 아니면 {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                만약 (r < 0 && r != -EEXIST)
                        반환 r;
        }

        반환 0;
}

정수 mkdir_p(const char *path, mode_t mode) {
        반환 mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

정수 mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        반환 mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

정수 mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ char *pp = NULL, *bn = NULL;
        _cleanup_close_ 정수 dfd = -EBADF;
        정수 r;

        assert(p);

        r = path_extract_directory(p, &pp);
        만약 (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                만약 (dfd < 0)
                        반환 -errno;
        } 아니면 만약 (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                반환 0;
        아니면 만약 (r < 0)
                반환 r;
        아니면 {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                만약 (r < 0)
                        반환 r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                만약 (dfd < 0)
                        반환 dfd;
        }

        r = path_extract_filename(p, &bn);
        만약 (r == -EADDRNOTAVAIL) /* Already top-level */
                반환 0;
        만약 (r < 0)
                반환 r;

        XOpenFlags flags = 0;
        만약 (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                만약 ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ 정수 nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        만약 (nfd == -EEXIST)
                반환 0;
        만약 (nfd < 0)
                반환 nfd;

        만약 ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                반환 -errno;

        만약 (ts != USEC_INFINITY) {
                struct timespec tspec;
                timespec_store(&tspec, ts);

                만약 (futimens(dfd, (const struct timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        반환 -errno;

                만약 (futimens(nfd, (const struct timespec[2]) { tspec, tspec }) < 0)
                        반환 -errno;
        }

        반환 1;
}
