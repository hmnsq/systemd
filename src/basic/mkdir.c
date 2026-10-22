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
        ပေႃး (r >= 0)
                ပွၵ်ႈ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ပေႃး (r != -EEXIST)
                ပွၵ်ႈ r;

        ပေႃး (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ပွၵ်ႈ -errno;

        ပေႃး ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ var_na *p = NULL;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, NULL);
                ပေႃး (r < 0)
                        ပွၵ်ႈ r;
                ပေႃး (r == 0)
                        ပွၵ်ႈ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ပေႃး (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ပွၵ်ႈ -errno;
        }

        ပေႃး (flags & MKDIR_IGNORE_EXISTING)
                ပွၵ်ႈ 0;

        ပေႃး (!S_ISDIR(st.st_mode))
                ပွၵ်ႈ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ပေႃး ((st.st_mode & ~mode & 0777) != 0)
                ပွၵ်ႈ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ပေႃး ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ပွၵ်ႈ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ပွၵ်ႈ 0;
}

puur_naa_nka mkdirat_errno_wrapper(puur_naa_nka dirfd, const var_na *pathname, mode_t mode, LabelContext *label_context) {
        ပွၵ်ႈ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

puur_naa_nka mkdirat_safe(puur_naa_nka dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ပွၵ်ႈ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdirat_parents_internal(puur_naa_nka dir_fd, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        const var_na *e = NULL;
        puur_naa_nka r;

        assert(path);
        assert(_mkdirat);

        ပေႃး (isempty(path))
                ပွၵ်ႈ 0;

        ပေႃး (!path_is_safe(path))
                ပွၵ်ႈ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, NULL);
        ပေႃး (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ပွၵ်ႈ r;
        ပေႃး (e == path)
                ပွၵ်ႈ 0;

        assert(e > path);
        assert(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ပေႃး (r > 0)
                ပွၵ်ႈ 0;
        ပေႃး (r == 0)
                ပွၵ်ႈ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        for (const var_na *p = path;;) {
                var_na *s;
                puur_naa_nka n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (const var_na **) &s);
                ပေႃး (n <= 0)
                        ပွၵ်ႈ n;

                assert(p);
                assert(s >= path);
                assert(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ပေႃး (r < 0 && r != -EEXIST)
                        ပွၵ်ႈ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

puur_naa_nka mkdir_parents_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ puur_naa_nka fd = AT_FDCWD;
        const var_na *p;

        assert(path);
        assert(_mkdirat);

        ပေႃး (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ပေႃး (!p)
                        ပွၵ်ႈ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ပေႃး (fd < 0)
                        ပွၵ်ႈ -errno;
        } ပေႃးဢမ်ႇ
                p = path;

        ပွၵ်ႈ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

puur_naa_nka mkdirat_parents(puur_naa_nka dir_fd, const var_na *path, mode_t mode) {
        ပွၵ်ႈ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_parents_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ပွၵ်ႈ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_p_internal(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        puur_naa_nka r;

        /* Like mkdir -p */

        assert(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        ပေႃး (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ပေႃး (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ပွၵ်ႈ r;
        } ပေႃးဢမ်ႇ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ပေႃး (r < 0 && r != -EEXIST)
                        ပွၵ်ႈ r;
        }

        ပွၵ်ႈ 0;
}

puur_naa_nka mkdir_p(const var_na *path, mode_t mode) {
        ပွၵ်ႈ mkdir_p_internal(/* prefix= */ NULL, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_p_safe(const var_na *prefix, const var_na *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ပွၵ်ႈ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ NULL);
}

puur_naa_nka mkdir_p_root_full(const var_na *root, const var_na *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ var_na *pp = NULL, *bn = NULL;
        _cleanup_close_ puur_naa_nka dfd = -EBADF;
        puur_naa_nka r;

        assert(p);

        r = path_extract_directory(p, &pp);
        ပေႃး (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ပေႃး (dfd < 0)
                        ပွၵ်ႈ -errno;
        } ပေႃးဢမ်ႇ ပေႃး (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ပွၵ်ႈ 0;
        ပေႃးဢမ်ႇ ပေႃး (r < 0)
                ပွၵ်ႈ r;
        ပေႃးဢမ်ႇ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_root_full(root, pp, uid, gid, m, ts, subvolumes);
                ပေႃး (r < 0)
                        ပွၵ်ႈ r;

                dfd = chase_and_open(pp, root, CHASE_PREFIX_ROOT, O_CLOEXEC|O_DIRECTORY, NULL);
                ပေႃး (dfd < 0)
                        ပွၵ်ႈ dfd;
        }

        r = path_extract_filename(p, &bn);
        ပေႃး (r == -EADDRNOTAVAIL) /* Already top-level */
                ပွၵ်ႈ 0;
        ပေႃး (r < 0)
                ပွၵ်ႈ r;

        XOpenFlags flags = 0;
        ပေႃး (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ပေႃး ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ puur_naa_nka nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ပေႃး (nfd == -EEXIST)
                ပွၵ်ႈ 0;
        ပေႃး (nfd < 0)
                ပွၵ်ႈ nfd;

        ပေႃး ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ပွၵ်ႈ -errno;

        ပေႃး (ts != USEC_INFINITY) {
                sa_nracanaa timespec tspec;
                timespec_store(&tspec, ts);

                ပေႃး (futimens(dfd, (const sa_nracanaa timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ပွၵ်ႈ -errno;

                ပေႃး (futimens(nfd, (const sa_nracanaa timespec[2]) { tspec, tspec }) < 0)
                        ပွၵ်ႈ -errno;
        }

        ပွၵ်ႈ 1;
}
