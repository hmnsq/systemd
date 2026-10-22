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

ကိန်းပြည့် mkdirat_safe_internal(
                ကိန်းပြည့် dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        ကိန်းပြည့် r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        အကယ်၍ (r >= 0)
                ပြန်ပေး chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        အကယ်၍ (r != -EEXIST)
                ပြန်ပေး r;

        အကယ်၍ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r == 0)
                        ပြန်ပေး mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                အကယ်၍ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ပြန်ပေး -errno;
        }

        အကယ်၍ (flags & MKDIR_IGNORE_EXISTING)
                ပြန်ပေး 0;

        အကယ်၍ (!S_ISDIR(st.st_mode))
                ပြန်ပေး log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        အကယ်၍ ((st.st_mode & ~mode & 0777) != 0)
                ပြန်ပေး log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        အကယ်၍ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ပြန်ပေး log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ပြန်ပေး 0;
}

ကိန်းပြည့် mkdirat_errno_wrapper(ကိန်းပြည့် dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ပြန်ပေး RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

ကိန်းပြည့် mkdirat_safe(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ပြန်ပေး mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ကိန်းပြည့် mkdirat_parents_internal(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        ကိန်းပြည့် r;

        assert(path);
        assert(_mkdirat);

        အကယ်၍ (isempty(path))
                ပြန်ပေး 0;

        အကယ်၍ (!path_is_safe(path))
                ပြန်ပေး -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        အကယ်၍ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ပြန်ပေး r;
        အကယ်၍ (e == path)
                ပြန်ပေး 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        အကယ်၍ (r > 0)
                ပြန်ပေး 0;
        အကယ်၍ (r == 0)
                ပြန်ပေး -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        အတွက် (const var_na *p = path;;) {
                var_na *s;
                ကိန်းပြည့် n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                အကယ်၍ (n <= 0)
                        ပြန်ပေး n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                အကယ်၍ (r < 0 && r != -EEXIST)
                        ပြန်ပေး r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ကိန်းပြည့် mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ကိန်းပြည့် fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        အကယ်၍ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                အကယ်၍ (!p)
                        ပြန်ပေး -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                အကယ်၍ (fd < 0)
                        ပြန်ပေး -errno;
        } မဟုတ်ပါက
                p = path;

        ပြန်ပေး mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

ကိန်းပြည့် mkdirat_parents(ကိန်းပြည့် dir_fd, const var_na *path, mode_t mode) {
        ပြန်ပေး mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ကိန်းပြည့် mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ပြန်ပေး mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ကိန်းပြည့် mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ကိန်းပြည့် r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                အကယ်၍ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ပြန်ပေး r;
        } မဟုတ်ပါက {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                အကယ်၍ (r < 0 && r != -EEXIST)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် mkdir_p(const var_na *path, mode_t mode) {
        ပြန်ပေး mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ကိန်းပြည့် mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ပြန်ပေး mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ကိန်းပြည့် mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ ကိန်းပြည့် dfd = -EBADF;
        ကိန်းပြည့် r;

        assert(p);

        r = path_extract_directory(p, &pp);
        အကယ်၍ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                အကယ်၍ (dfd < 0)
                        ပြန်ပေး -errno;
        } မဟုတ်ပါက အကယ်၍ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ပြန်ပေး 0;
        မဟုတ်ပါက အကယ်၍ (r < 0)
                ပြန်ပေး r;
        မဟုတ်ပါက {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                အကယ်၍ (dfd < 0)
                        ပြန်ပေး dfd;
        }

        r = path_extract_filename(p, &bn);
        အကယ်၍ (r == -EADDRNOTAVAIL) /* Already top-level */
                ပြန်ပေး 0;
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        XOpenFlags flags = 0;
        အကယ်၍ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                အကယ်၍ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ကိန်းပြည့် nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        အကယ်၍ (nfd == -EEXIST)
                ပြန်ပေး 0;
        အကယ်၍ (nfd < 0)
                ပြန်ပေး nfd;

        အကယ်၍ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                အကယ်၍ (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ပြန်ပေး -errno;

                အကယ်၍ (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ပြန်ပေး -errno;
        }

        ပြန်ပေး 1;
}
