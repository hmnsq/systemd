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

số_nguyên mkdirat_safe_internal(
                số_nguyên dir_fd,
                const char *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        struct stat st;
        số_nguyên r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        nếu (r >= 0)
                trả_về chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        nếu (r != -EEXIST)
                trả_về r;

        nếu (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                trả_về -errno;

        nếu ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ char *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        trả_về mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                nếu (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        trả_về -errno;
        }

        nếu (flags & MKDIR_IGNORE_EXISTING)
                trả_về 0;

        nếu (!S_ISDIR(st.st_mode))
                trả_về log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        nếu ((st.st_mode & ~mode & 0777) != 0)
                trả_về log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        nếu ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                trả_về log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        trả_về 0;
}

số_nguyên mkdirat_errno_wrapper(số_nguyên dirfd, const char *pathname, mode_t mode, LabelContext *label_context) {
        trả_về RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

số_nguyên mkdirat_safe(số_nguyên dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        trả_về mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

số_nguyên mkdirat_parents_internal(số_nguyên dir_fd, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const char *e = NULL;
        số_nguyên r;

        assert(path);
        assert(_mkdirat);

        nếu (isempty(path))
                trả_về 0;

        nếu (!path_is_safe(path))
                trả_về -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        nếu (r <= 0) /* r == 0 means path is equivalent to prefix. */
                trả_về r;
        nếu (e == path)
                trả_về 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        nếu (r > 0)
                trả_về 0;
        nếu (r == 0)
                trả_về -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        cho (const char *p = path;;) {
                char *s;
                số_nguyên n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const char **) &s);
                nếu (n <= 0)
                        trả_về n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                nếu (r < 0 && r != -EEXIST)
                        trả_về r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

số_nguyên mkdir_parents_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ số_nguyên fd = AT_FDCWD;
        const char *p;

        assert(path);
        assert(_mkdirat);

        nếu (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                nếu (!p)
                        trả_về -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                nếu (fd < 0)
                        trả_về -errno;
        } không_thì
                p = path;

        trả_về mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

số_nguyên mkdirat_parents(số_nguyên dir_fd, const char *path, mode_t mode) {
        trả_về mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

số_nguyên mkdir_parents_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        trả_về mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

số_nguyên mkdir_p_internal(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        số_nguyên r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        nếu (r < 0)
                trả_về r;

        nếu (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                nếu (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        trả_về r;
        } không_thì {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                nếu (r < 0 && r != -EEXIST)
                        trả_về r;
        }

        trả_về 0;
}

số_nguyên mkdir_p(const char *path, mode_t mode) {
        trả_về mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

số_nguyên mkdir_p_safe(const char *prefix, const char *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        trả_về mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

số_nguyên mkdir_p_root_full(const char *root, const char *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ char *pp = NULL, *bn = NULL;
        _cleanup_close_ số_nguyên dfd = -EBADF;
        số_nguyên r;

        assert(p);

        r = path_extract_directory(p, &pp);
        nếu (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                nếu (dfd < 0)
                        trả_về -errno;
        } không_thì nếu (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                trả_về 0;
        không_thì nếu (r < 0)
                trả_về r;
        không_thì {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                nếu (r < 0)
                        trả_về r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                nếu (dfd < 0)
                        trả_về dfd;
        }

        r = path_extract_filename(p, &bn);
        nếu (r == -EADDRNOTAVAIL) /* Already top-level */
                trả_về 0;
        nếu (r < 0)
                trả_về r;

        XOpenFlags flags = 0;
        nếu (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                nếu ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ số_nguyên nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        nếu (nfd == -EEXIST)
                trả_về 0;
        nếu (nfd < 0)
                trả_về nfd;

        nếu ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                trả_về -errno;

        nếu (ts != USEC_INFINITY) {
                struct timespec tspec;
                timespec_store(&tspec, ts);

                nếu (futimens(dfd, (const struct timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        trả_về -errno;

                nếu (futimens(nfd, (const struct timespec[2]) { tspec, tspec }) < 0)
                        trả_về -errno;
        }

        trả_về 1;
}
