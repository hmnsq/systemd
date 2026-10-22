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

ຈຳນວນເຕັມ mkdirat_safe_internal(
                ຈຳນວນເຕັມ dir_fd,
                const var_na *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        sa_nracanaa stat st;
        ຈຳນວນເຕັມ r;

        assert(path);
        assert(mode != MODE_INVALID);
        assert(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ຖ້າ (r >= 0)
                ສົ່ງຄືນ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ຖ້າ (r != -EEXIST)
                ສົ່ງຄືນ r;

        ຖ້າ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ສົ່ງຄືນ -errno;

        ຖ້າ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (r == 0)
                        ສົ່ງຄືນ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ຖ້າ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ສົ່ງຄືນ -errno;
        }

        ຖ້າ (flags & MKDIR_IGNORE_EXISTING)
                ສົ່ງຄືນ 0;

        ຖ້າ (!S_ISDIR(st.st_mode))
                ສົ່ງຄືນ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ຖ້າ ((st.st_mode & ~mode & 0777) != 0)
                ສົ່ງຄືນ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ຖ້າ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ສົ່ງຄືນ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ mkdirat_errno_wrapper(ຈຳນວນເຕັມ dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ສົ່ງຄືນ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

ຈຳນວນເຕັມ mkdirat_safe(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ສົ່ງຄືນ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ຈຳນວນເຕັມ mkdirat_parents_internal(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        ຈຳນວນເຕັມ r;

        assert(path);
        assert(_mkdirat);

        ຖ້າ (isempty(path))
                ສົ່ງຄືນ 0;

        ຖ້າ (!path_is_safe(path))
                ສົ່ງຄືນ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        ຖ້າ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ສົ່ງຄືນ r;
        ຖ້າ (e == path)
                ສົ່ງຄືນ 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ຖ້າ (r > 0)
                ສົ່ງຄືນ 0;
        ຖ້າ (r == 0)
                ສົ່ງຄືນ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        ສຳລັບ (const var_na *p = path;;) {
                var_na *s;
                ຈຳນວນເຕັມ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                ຖ້າ (n <= 0)
                        ສົ່ງຄືນ n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ຖ້າ (r < 0 && r != -EEXIST)
                        ສົ່ງຄືນ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ຈຳນວນເຕັມ mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ຈຳນວນເຕັມ fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        ຖ້າ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ຖ້າ (!p)
                        ສົ່ງຄືນ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ຖ້າ (fd < 0)
                        ສົ່ງຄືນ -errno;
        } ບໍ່ດັ່ງນັ້ນ
                p = path;

        ສົ່ງຄືນ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

ຈຳນວນເຕັມ mkdirat_parents(ຈຳນວນເຕັມ dir_fd, const var_na *path, mode_t mode) {
        ສົ່ງຄືນ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ຈຳນວນເຕັມ mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ສົ່ງຄືນ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ຈຳນວນເຕັມ mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ຈຳນວນເຕັມ r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ຖ້າ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ສົ່ງຄືນ r;
        } ບໍ່ດັ່ງນັ້ນ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ຖ້າ (r < 0 && r != -EEXIST)
                        ສົ່ງຄືນ r;
        }

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ mkdir_p(const var_na *path, mode_t mode) {
        ສົ່ງຄືນ mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ຈຳນວນເຕັມ mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ສົ່ງຄືນ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

ຈຳນວນເຕັມ mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ ຈຳນວນເຕັມ dfd = -EBADF;
        ຈຳນວນເຕັມ r;

        assert(p);

        r = path_extract_directory(p, &pp);
        ຖ້າ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ຖ້າ (dfd < 0)
                        ສົ່ງຄືນ -errno;
        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ສົ່ງຄືນ 0;
        ບໍ່ດັ່ງນັ້ນ ຖ້າ (r < 0)
                ສົ່ງຄືນ r;
        ບໍ່ດັ່ງນັ້ນ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                ຖ້າ (dfd < 0)
                        ສົ່ງຄືນ dfd;
        }

        r = path_extract_filename(p, &bn);
        ຖ້າ (r == -EADDRNOTAVAIL) /* Already top-level */
                ສົ່ງຄືນ 0;
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        XOpenFlags flags = 0;
        ຖ້າ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ຖ້າ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ຈຳນວນເຕັມ nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ຖ້າ (nfd == -EEXIST)
                ສົ່ງຄືນ 0;
        ຖ້າ (nfd < 0)
                ສົ່ງຄືນ nfd;

        ຖ້າ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ສົ່ງຄືນ -errno;

        ຖ້າ (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                ຖ້າ (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ສົ່ງຄືນ -errno;

                ຖ້າ (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ສົ່ງຄືນ -errno;
        }

        ສົ່ງຄືນ 1;
}
