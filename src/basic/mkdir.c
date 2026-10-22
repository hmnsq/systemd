/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "btrfs-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "hashmap.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mkdir.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "user-util.h"

পূর্ণাঙ্ক mkdirat_safe_internal(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুব বর্ণ *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        সংরচনা stat st;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(mode != MODE_INVALID);
        প্রতিজ্ঞা(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        যদি (r >= 0)
                প্রত্যাগম chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        যদি (r != -EEXIST)
                প্রত্যাগম r;

        যদি (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                প্রত্যাগম -errno;

        যদি ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ বর্ণ *p = রিক্ত;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, রিক্ত);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        প্রত্যাগম mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                যদি (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        প্রত্যাগম -errno;
        }

        যদি (flags & MKDIR_IGNORE_EXISTING)
                প্রত্যাগম 0;

        যদি (!S_ISDIR(st.st_mode))
                প্রত্যাগম log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        যদি ((st.st_mode & ~mode & 0777) != 0)
                প্রত্যাগম log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        যদি ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                প্রত্যাগম log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক mkdirat_errno_wrapper(পূর্ণাঙ্ক dirfd, ধ্রুব বর্ণ *pathname, mode_t mode, LabelContext *label_context) {
        প্রত্যাগম RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

পূর্ণাঙ্ক mkdirat_safe(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        প্রত্যাগম mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ রিক্ত);
}

পূর্ণাঙ্ক mkdirat_parents_internal(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ধ্রুব বর্ণ *e = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(_mkdirat);

        যদি (isempty(path))
                প্রত্যাগম 0;

        যদি (!path_is_safe(path))
                প্রত্যাগম -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, রিক্ত);
        যদি (r <= 0) /* r == 0 means path is equivalent to prefix. */
                প্রত্যাগম r;
        যদি (e == path)
                প্রত্যাগম 0;

        প্রতিজ্ঞা(e > path);
        প্রতিজ্ঞা(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        যদি (r > 0)
                প্রত্যাগম 0;
        যদি (r == 0)
                প্রত্যাগম -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        প্রতি (ধ্রুব বর্ণ *p = path;;) {
                বর্ণ *s;
                পূর্ণাঙ্ক n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ধ্রুব বর্ণ **) &s);
                যদি (n <= 0)
                        প্রত্যাগম n;

                প্রতিজ্ঞা(p);
                প্রতিজ্ঞা(s >= path);
                প্রতিজ্ঞা(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                যদি (r < 0 && r != -EEXIST)
                        প্রত্যাগম r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

পূর্ণাঙ্ক mkdir_parents_internal(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ পূর্ণাঙ্ক fd = AT_FDCWD;
        ধ্রুব বর্ণ *p;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(_mkdirat);

        যদি (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                যদি (!p)
                        প্রত্যাগম -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                যদি (fd < 0)
                        প্রত্যাগম -errno;
        } অন্যথা
                p = path;

        প্রত্যাগম mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

পূর্ণাঙ্ক mkdirat_parents(পূর্ণাঙ্ক dir_fd, ধ্রুব বর্ণ *path, mode_t mode) {
        প্রত্যাগম mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ রিক্ত);
}

পূর্ণাঙ্ক mkdir_parents_safe(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        প্রত্যাগম mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ রিক্ত);
}

পূর্ণাঙ্ক mkdir_p_internal(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        পূর্ণাঙ্ক r;

        /* Like mkdir -p */

        প্রতিজ্ঞা(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                যদি (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        প্রত্যাগম r;
        } অন্যথা {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                যদি (r < 0 && r != -EEXIST)
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক mkdir_p(ধ্রুব বর্ণ *path, mode_t mode) {
        প্রত্যাগম mkdir_p_internal(/* prefix= */ রিক্ত, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ রিক্ত);
}

পূর্ণাঙ্ক mkdir_p_safe(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        প্রত্যাগম mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ রিক্ত);
}

পূর্ণাঙ্ক mkdir_p_মূল_full(ধ্রুব বর্ণ *মূল, ধ্রুব বর্ণ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ বর্ণ *pp = রিক্ত, *bn = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক dfd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);

        r = path_extract_directory(p, &pp);
        যদি (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                যদি (dfd < 0)
                        প্রত্যাগম -errno;
        } অন্যথা যদি (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                প্রত্যাগম 0;
        অন্যথা যদি (r < 0)
                প্রত্যাগম r;
        অন্যথা {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_মূল_full(মূল, pp, uid, gid, m, ts, subvolumes);
                যদি (r < 0)
                        প্রত্যাগম r;

                dfd = chase_and_open(pp, মূল, CHASE_PREFIX_মূল, O_CLOEXEC|O_DIRECTORY, রিক্ত);
                যদি (dfd < 0)
                        প্রত্যাগম dfd;
        }

        r = path_extract_filename(p, &bn);
        যদি (r == -EADDRNOTAVAIL) /* Already top-level */
                প্রত্যাগম 0;
        যদি (r < 0)
                প্রত্যাগম r;

        XOpenFlags flags = 0;
        যদি (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                যদি ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ পূর্ণাঙ্ক nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        যদি (nfd == -EEXIST)
                প্রত্যাগম 0;
        যদি (nfd < 0)
                প্রত্যাগম nfd;

        যদি ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                প্রত্যাগম -errno;

        যদি (ts != USEC_INFINITY) {
                সংরচনা timespec tspec;
                timespec_store(&tspec, ts);

                যদি (futimens(dfd, (ধ্রুব সংরচনা timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        প্রত্যাগম -errno;

                যদি (futimens(nfd, (ধ্রুব সংরচনা timespec[2]) { tspec, tspec }) < 0)
                        প্রত্যাগম -errno;
        }

        প্রত্যাগম 1;
}
