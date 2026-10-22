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

ចំនួនគត់ mkdirat_safe_internal(
                ចំនួនគត់ dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        ចំនួនគត់ r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ប្រសិនបើ (r >= 0)
                ត្រឡប់ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ប្រសិនបើ (r != -EEXIST)
                ត្រឡប់ r;

        ប្រសិនបើ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (r == 0)
                        ត្រឡប់ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ប្រសិនបើ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ត្រឡប់ -errno;
        }

        ប្រសិនបើ (flags & MKDIR_IGNORE_EXISTING)
                ត្រឡប់ 0;

        ប្រសិនបើ (!S_ISDIR(st.st_mode))
                ត្រឡប់ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ប្រសិនបើ ((st.st_mode & ~mode & 0777) != 0)
                ត្រឡប់ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ប្រសិនបើ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ត្រឡប់ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ត្រឡប់ 0;
}

ចំនួនគត់ mkdirat_errno_wrapper(ចំនួនគត់ dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ត្រឡប់ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

ចំនួនគត់ mkdirat_safe(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ត្រឡប់ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ចំនួនគត់ mkdirat_parents_internal(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        ចំនួនគត់ r;

        assert(path);
        assert(_mkdirat);

        ប្រសិនបើ (isempty(path))
                ត្រឡប់ 0;

        ប្រសិនបើ (!path_is_safe(path))
                ត្រឡប់ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        ប្រសិនបើ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ត្រឡប់ r;
        ប្រសិនបើ (e == path)
                ត្រឡប់ 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ប្រសិនបើ (r > 0)
                ត្រឡប់ 0;
        ប្រសិនបើ (r == 0)
                ត្រឡប់ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        សម្រាប់ (const var_na *p = path;;) {
                var_na *s;
                ចំនួនគត់ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                ប្រសិនបើ (n <= 0)
                        ត្រឡប់ n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ប្រសិនបើ (r < 0 && r != -EEXIST)
                        ត្រឡប់ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ចំនួនគត់ mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ចំនួនគត់ fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        ប្រសិនបើ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ប្រសិនបើ (!p)
                        ត្រឡប់ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ប្រសិនបើ (fd < 0)
                        ត្រឡប់ -errno;
        } បើមិនដូច្នេះ
                p = path;

        ត្រឡប់ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

ចំនួនគត់ mkdirat_parents(ចំនួនគត់ dir_fd, const var_na *path, mode_t mode) {
        ត្រឡប់ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ចំនួនគត់ mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ត្រឡប់ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ចំនួនគត់ mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ចំនួនគត់ r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        ប្រសិនបើ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ប្រសិនបើ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ត្រឡប់ r;
        } បើមិនដូច្នេះ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ប្រសិនបើ (r < 0 && r != -EEXIST)
                        ត្រឡប់ r;
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ mkdir_p(const var_na *path, mode_t mode) {
        ត្រឡប់ mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ចំនួនគត់ mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ត្រឡប់ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ចំនួនគត់ mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ ចំនួនគត់ dfd = -EBADF;
        ចំនួនគត់ r;

        assert(p);

        r = path_extract_directory(p, &pp);
        ប្រសិនបើ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ប្រសិនបើ (dfd < 0)
                        ត្រឡប់ -errno;
        } បើមិនដូច្នេះ ប្រសិនបើ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ត្រឡប់ 0;
        បើមិនដូច្នេះ ប្រសិនបើ (r < 0)
                ត្រឡប់ r;
        បើមិនដូច្នេះ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                ប្រសិនបើ (dfd < 0)
                        ត្រឡប់ dfd;
        }

        r = path_extract_filename(p, &bn);
        ប្រសិនបើ (r == -EADDRNOTAVAIL) /* Already top-level */
                ត្រឡប់ 0;
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        XOpenFlags flags = 0;
        ប្រសិនបើ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ប្រសិនបើ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ចំនួនគត់ nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ប្រសិនបើ (nfd == -EEXIST)
                ត្រឡប់ 0;
        ប្រសិនបើ (nfd < 0)
                ត្រឡប់ nfd;

        ប្រសិនបើ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                ប្រសិនបើ (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ត្រឡប់ -errno;

                ប្រសិនបើ (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ត្រឡប់ -errno;
        }

        ត្រឡប់ 1;
}
