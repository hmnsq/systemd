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

ཧྲིལ mkdirat_safe_internal(
                ཧྲིལ dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        ཧྲིལ r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        གལ་སྲིད (r >= 0)
                ལོག chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        གལ་སྲིད (r != -EEXIST)
                ལོག r;

        གལ་སྲིད (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ལོག -errno;

        གལ་སྲིད ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                གལ་སྲིད (r < 0)
                        ལོག r;
                གལ་སྲིད (r == 0)
                        ལོག mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                གལ་སྲིད (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ལོག -errno;
        }

        གལ་སྲིད (flags & MKDIR_IGNORE_EXISTING)
                ལོག 0;

        གལ་སྲིད (!S_ISDIR(st.st_mode))
                ལོག log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        གལ་སྲིད ((st.st_mode & ~mode & 0777) != 0)
                ལོག log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        གལ་སྲིད ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ལོག log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ལོག 0;
}

ཧྲིལ mkdirat_errno_wrapper(ཧྲིལ dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ལོག RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

ཧྲིལ mkdirat_safe(ཧྲིལ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ལོག mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ཧྲིལ mkdirat_parents_internal(ཧྲིལ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        ཧྲིལ r;

        assert(path);
        assert(_mkdirat);

        གལ་སྲིད (isempty(path))
                ལོག 0;

        གལ་སྲིད (!path_is_safe(path))
                ལོག -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        གལ་སྲིད (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ལོག r;
        གལ་སྲིད (e == path)
                ལོག 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        གལ་སྲིད (r > 0)
                ལོག 0;
        གལ་སྲིད (r == 0)
                ལོག -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        དོན་ལུ (const var_na *p = path;;) {
                var_na *s;
                ཧྲིལ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                གལ་སྲིད (n <= 0)
                        ལོག n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                གལ་སྲིད (r < 0 && r != -EEXIST)
                        ལོག r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ཧྲིལ mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ཧྲིལ fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        གལ་སྲིད (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                གལ་སྲིད (!p)
                        ལོག -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                གལ་སྲིད (fd < 0)
                        ལོག -errno;
        } དེ་མིན
                p = path;

        ལོག mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

ཧྲིལ mkdirat_parents(ཧྲིལ dir_fd, const var_na *path, mode_t mode) {
        ལོག mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ཧྲིལ mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ལོག mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ཧྲིལ mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ཧྲིལ r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        གལ་སྲིད (r < 0)
                ལོག r;

        གལ་སྲིད (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                གལ་སྲིད (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ལོག r;
        } དེ་མིན {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                གལ་སྲིད (r < 0 && r != -EEXIST)
                        ལོག r;
        }

        ལོག 0;
}

ཧྲིལ mkdir_p(const var_na *path, mode_t mode) {
        ལོག mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ཧྲིལ mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ལོག mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ཧྲིལ mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ ཧྲིལ dfd = -EBADF;
        ཧྲིལ r;

        assert(p);

        r = path_extract_directory(p, &pp);
        གལ་སྲིད (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                གལ་སྲིད (dfd < 0)
                        ལོག -errno;
        } དེ་མིན གལ་སྲིད (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ལོག 0;
        དེ་མིན གལ་སྲིད (r < 0)
                ལོག r;
        དེ་མིན {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                གལ་སྲིད (r < 0)
                        ལོག r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                གལ་སྲིད (dfd < 0)
                        ལོག dfd;
        }

        r = path_extract_filename(p, &bn);
        གལ་སྲིད (r == -EADDRNOTAVAIL) /* Already top-level */
                ལོག 0;
        གལ་སྲིད (r < 0)
                ལོག r;

        XOpenFlags flags = 0;
        གལ་སྲིད (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                གལ་སྲིད ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ཧྲིལ nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        གལ་སྲིད (nfd == -EEXIST)
                ལོག 0;
        གལ་སྲིད (nfd < 0)
                ལོག nfd;

        གལ་སྲིད ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ལོག -errno;

        གལ་སྲིད (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                གལ་སྲིད (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ལོག -errno;

                གལ་སྲིད (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ལོག -errno;
        }

        ལོག 1;
}
