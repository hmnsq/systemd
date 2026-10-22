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

පූර්ණ_සංඛ්‍යා mkdirat_safe_internal(
                පූර්ණ_සංඛ්‍යා dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        නම් (r >= 0)
                ආපසු chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        නම් (r != -EEXIST)
                ආපසු r;

        නම් (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ආපසු -errno;

        නම් ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                නම් (r < 0)
                        ආපසු r;
                නම් (r == 0)
                        ආපසු mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                නම් (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ආපසු -errno;
        }

        නම් (flags & MKDIR_IGNORE_EXISTING)
                ආපසු 0;

        නම් (!S_ISDIR(st.st_mode))
                ආපසු log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        නම් ((st.st_mode & ~mode & 0777) != 0)
                ආපසු log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        නම් ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ආපසු log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා mkdirat_errno_wrapper(පූර්ණ_සංඛ්‍යා dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ආපසු RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

පූර්ණ_සංඛ්‍යා mkdirat_safe(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ආපසු mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

පූර්ණ_සංඛ්‍යා mkdirat_parents_internal(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        පූර්ණ_සංඛ්‍යා r;

        assert(path);
        assert(_mkdirat);

        නම් (isempty(path))
                ආපසු 0;

        නම් (!path_is_safe(path))
                ආපසු -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        නම් (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ආපසු r;
        නම් (e == path)
                ආපසු 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        නම් (r > 0)
                ආපසු 0;
        නම් (r == 0)
                ආපසු -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        සඳහා (const var_na *p = path;;) {
                var_na *s;
                පූර්ණ_සංඛ්‍යා n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                නම් (n <= 0)
                        ආපසු n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                නම් (r < 0 && r != -EEXIST)
                        ආපසු r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

පූර්ණ_සංඛ්‍යා mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        නම් (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                නම් (!p)
                        ආපසු -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                නම් (fd < 0)
                        ආපසු -errno;
        } නැතහොත්
                p = path;

        ආපසු mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

පූර්ණ_සංඛ්‍යා mkdirat_parents(පූර්ණ_සංඛ්‍යා dir_fd, const var_na *path, mode_t mode) {
        ආපසු mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

පූර්ණ_සංඛ්‍යා mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ආපසු mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

පූර්ණ_සංඛ්‍යා mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        පූර්ණ_සංඛ්‍යා r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        නම් (r < 0)
                ආපසු r;

        නම් (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                නම් (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ආපසු r;
        } නැතහොත් {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                නම් (r < 0 && r != -EEXIST)
                        ආපසු r;
        }

        ආපසු 0;
}

පූර්ණ_සංඛ්‍යා mkdir_p(const var_na *path, mode_t mode) {
        ආපසු mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

පූර්ණ_සංඛ්‍යා mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ආපසු mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

පූර්ණ_සංඛ්‍යා mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ පූර්ණ_සංඛ්‍යා dfd = -EBADF;
        පූර්ණ_සංඛ්‍යා r;

        assert(p);

        r = path_extract_directory(p, &pp);
        නම් (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                නම් (dfd < 0)
                        ආපසු -errno;
        } නැතහොත් නම් (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ආපසු 0;
        නැතහොත් නම් (r < 0)
                ආපසු r;
        නැතහොත් {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                නම් (r < 0)
                        ආපසු r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                නම් (dfd < 0)
                        ආපසු dfd;
        }

        r = path_extract_filename(p, &bn);
        නම් (r == -EADDRNOTAVAIL) /* Already top-level */
                ආපසු 0;
        නම් (r < 0)
                ආපසු r;

        XOpenFlags flags = 0;
        නම් (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                නම් ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ පූර්ණ_සංඛ්‍යා nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        නම් (nfd == -EEXIST)
                ආපසු 0;
        නම් (nfd < 0)
                ආපසු nfd;

        නම් ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ආපසු -errno;

        නම් (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                නම් (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ආපසු -errno;

                නම් (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ආපසු -errno;
        }

        ආපසු 1;
}
