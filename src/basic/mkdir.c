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

จำนวนเต็ม mkdirat_safe_internal(
                จำนวนเต็ม dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        จำนวนเต็ม r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ถ้า (r >= 0)
                คืน chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ถ้า (r != -EEXIST)
                คืน r;

        ถ้า (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                คืน -errno;

        ถ้า ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        คืน mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ถ้า (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        คืน -errno;
        }

        ถ้า (flags & MKDIR_IGNORE_EXISTING)
                คืน 0;

        ถ้า (!S_ISDIR(st.st_mode))
                คืน log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ถ้า ((st.st_mode & ~mode & 0777) != 0)
                คืน log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ถ้า ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                คืน log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        คืน 0;
}

จำนวนเต็ม mkdirat_errno_wrapper(จำนวนเต็ม dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        คืน RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

จำนวนเต็ม mkdirat_safe(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        คืน mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

จำนวนเต็ม mkdirat_parents_internal(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(_mkdirat);

        ถ้า (isempty(path))
                คืน 0;

        ถ้า (!path_is_safe(path))
                คืน -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        ถ้า (r <= 0) /* r == 0 means path is equivalent to prefix. */
                คืน r;
        ถ้า (e == path)
                คืน 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ถ้า (r > 0)
                คืน 0;
        ถ้า (r == 0)
                คืน -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        สำหรับ (const var_na *p = path;;) {
                var_na *s;
                จำนวนเต็ม n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                ถ้า (n <= 0)
                        คืน n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ถ้า (r < 0 && r != -EEXIST)
                        คืน r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

จำนวนเต็ม mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ จำนวนเต็ม fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        ถ้า (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ถ้า (!p)
                        คืน -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ถ้า (fd < 0)
                        คืน -errno;
        } มิฉะนั้น
                p = path;

        คืน mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

จำนวนเต็ม mkdirat_parents(จำนวนเต็ม dir_fd, const var_na *path, mode_t mode) {
        คืน mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

จำนวนเต็ม mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        คืน mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

จำนวนเต็ม mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        จำนวนเต็ม r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ถ้า (r < 0)
                คืน r;

        ถ้า (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ถ้า (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        คืน r;
        } มิฉะนั้น {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ถ้า (r < 0 && r != -EEXIST)
                        คืน r;
        }

        คืน 0;
}

จำนวนเต็ม mkdir_p(const var_na *path, mode_t mode) {
        คืน mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

จำนวนเต็ม mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        คืน mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

จำนวนเต็ม mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ จำนวนเต็ม dfd = -EBADF;
        จำนวนเต็ม r;

        assert(p);

        r = path_extract_directory(p, &pp);
        ถ้า (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ถ้า (dfd < 0)
                        คืน -errno;
        } มิฉะนั้น ถ้า (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                คืน 0;
        มิฉะนั้น ถ้า (r < 0)
                คืน r;
        มิฉะนั้น {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ถ้า (r < 0)
                        คืน r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                ถ้า (dfd < 0)
                        คืน dfd;
        }

        r = path_extract_filename(p, &bn);
        ถ้า (r == -EADDRNOTAVAIL) /* Already top-level */
                คืน 0;
        ถ้า (r < 0)
                คืน r;

        XOpenFlags flags = 0;
        ถ้า (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ถ้า ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ จำนวนเต็ม nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ถ้า (nfd == -EEXIST)
                คืน 0;
        ถ้า (nfd < 0)
                คืน nfd;

        ถ้า ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                คืน -errno;

        ถ้า (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                ถ้า (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        คืน -errno;

                ถ้า (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        คืน -errno;
        }

        คืน 1;
}
