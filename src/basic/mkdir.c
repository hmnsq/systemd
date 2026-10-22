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

puur_naa_nka mkdirat_safe_internal(
                puur_naa_nka dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        puur_naa_nka r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ᤀᤡ᤺ (r >= 0)
                ᤀᤡᤴ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ᤀᤡ᤺ (r != -EEXIST)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡ᤺ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;
                ᤀᤡ᤺ (r == 0)
                        ᤀᤡᤴ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ᤀᤡ᤺ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ᤀᤡᤴ -errno;
        }

        ᤀᤡ᤺ (flags & MKDIR_IGNORE_EXISTING)
                ᤀᤡᤴ 0;

        ᤀᤡ᤺ (!S_ISDIR(st.st_mode))
                ᤀᤡᤴ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ᤀᤡ᤺ ((st.st_mode & ~mode & 0777) != 0)
                ᤀᤡᤴ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ᤀᤡ᤺ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ᤀᤡᤴ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ᤀᤡᤴ 0;
}

puur_naa_nka mkdirat_errno_wrapper(puur_naa_nka dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ᤀᤡᤴ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

puur_naa_nka mkdirat_safe(puur_naa_nka dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ᤀᤡᤴ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdirat_parents_internal(puur_naa_nka dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        puur_naa_nka r;

        assert(path);
        assert(_mkdirat);

        ᤀᤡ᤺ (isempty(path))
                ᤀᤡᤴ 0;

        ᤀᤡ᤺ (!path_is_safe(path))
                ᤀᤡᤴ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        ᤀᤡ᤺ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ᤀᤡᤴ r;
        ᤀᤡ᤺ (e == path)
                ᤀᤡᤴ 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ᤀᤡ᤺ (r > 0)
                ᤀᤡᤴ 0;
        ᤀᤡ᤺ (r == 0)
                ᤀᤡᤴ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        for (const var_na *p = path;;) {
                var_na *s;
                puur_naa_nka n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                ᤀᤡ᤺ (n <= 0)
                        ᤀᤡᤴ n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ᤀᤡ᤺ (r < 0 && r != -EEXIST)
                        ᤀᤡᤴ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

puur_naa_nka mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ puur_naa_nka fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        ᤀᤡ᤺ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ᤀᤡ᤺ (!p)
                        ᤀᤡᤴ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ᤀᤡ᤺ (fd < 0)
                        ᤀᤡᤴ -errno;
        } ᤀᤡ᤺ᤆᤡ
                p = path;

        ᤀᤡᤴ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

puur_naa_nka mkdirat_parents(puur_naa_nka dir_fd, const var_na *path, mode_t mode) {
        ᤀᤡᤴ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ᤀᤡᤴ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        puur_naa_nka r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        ᤀᤡ᤺ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ᤀᤡ᤺ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ᤀᤡᤴ r;
        } ᤀᤡ᤺ᤆᤡ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ᤀᤡ᤺ (r < 0 && r != -EEXIST)
                        ᤀᤡᤴ r;
        }

        ᤀᤡᤴ 0;
}

puur_naa_nka mkdir_p(const var_na *path, mode_t mode) {
        ᤀᤡᤴ mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ᤀᤡᤴ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ puur_naa_nka dfd = -EBADF;
        puur_naa_nka r;

        assert(p);

        r = path_extract_directory(p, &pp);
        ᤀᤡ᤺ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ᤀᤡ᤺ (dfd < 0)
                        ᤀᤡᤴ -errno;
        } ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ᤀᤡᤴ 0;
        ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;
        ᤀᤡ᤺ᤆᤡ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ᤀᤡ᤺ (r < 0)
                        ᤀᤡᤴ r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                ᤀᤡ᤺ (dfd < 0)
                        ᤀᤡᤴ dfd;
        }

        r = path_extract_filename(p, &bn);
        ᤀᤡ᤺ (r == -EADDRNOTAVAIL) /* Already top-level */
                ᤀᤡᤴ 0;
        ᤀᤡ᤺ (r < 0)
                ᤀᤡᤴ r;

        XOpenFlags flags = 0;
        ᤀᤡ᤺ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ᤀᤡ᤺ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ puur_naa_nka nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ᤀᤡ᤺ (nfd == -EEXIST)
                ᤀᤡᤴ 0;
        ᤀᤡ᤺ (nfd < 0)
                ᤀᤡᤴ nfd;

        ᤀᤡ᤺ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ᤀᤡᤴ -errno;

        ᤀᤡ᤺ (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                ᤀᤡ᤺ (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ᤀᤡᤴ -errno;

                ᤀᤡ᤺ (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ᤀᤡᤴ -errno;
        }

        ᤀᤡᤴ 1;
}
