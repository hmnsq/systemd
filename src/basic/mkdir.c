/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "btrfs-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "hashmap.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mkdir.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "user-util.h"

પૂર્ણાંક mkdirat_safe_internal(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        સંરચના stat st;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(mode != MODE_INVALID);
        પ્રતિજ્ઞા(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        જો (r >= 0)
                પાછા chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        જો (r != -EEXIST)
                પાછા r;

        જો (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                પાછા -errno;

        જો ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ અક્ષર *p = ખાલી;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, ખાલી);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                જો (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        પાછા -errno;
        }

        જો (flags & MKDIR_IGNORE_EXISTING)
                પાછા 0;

        જો (!S_ISDIR(st.st_mode))
                પાછા log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        જો ((st.st_mode & ~mode & 0777) != 0)
                પાછા log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        જો ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                પાછા log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        પાછા 0;
}

પૂર્ણાંક mkdirat_errno_wrapper(પૂર્ણાંક dirfd, અચલ અક્ષર *pathname, mode_t mode, LabelContext *label_context) {
        પાછા RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

પૂર્ણાંક mkdirat_safe(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        પાછા mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ખાલી);
}

પૂર્ણાંક mkdirat_parents_internal(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        અચલ અક્ષર *e = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(_mkdirat);

        જો (isempty(path))
                પાછા 0;

        જો (!path_is_safe(path))
                પાછા -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, ખાલી);
        જો (r <= 0) /* r == 0 means path is equivalent to prefix. */
                પાછા r;
        જો (e == path)
                પાછા 0;

        પ્રતિજ્ઞા(e > path);
        પ્રતિજ્ઞા(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        જો (r > 0)
                પાછા 0;
        જો (r == 0)
                પાછા -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        માટે (અચલ અક્ષર *p = path;;) {
                અક્ષર *s;
                પૂર્ણાંક n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (અચલ અક્ષર **) &s);
                જો (n <= 0)
                        પાછા n;

                પ્રતિજ્ઞા(p);
                પ્રતિજ્ઞા(s >= path);
                પ્રતિજ્ઞા(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                જો (r < 0 && r != -EEXIST)
                        પાછા r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

પૂર્ણાંક mkdir_parents_internal(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ પૂર્ણાંક fd = AT_FDCWD;
        અચલ અક્ષર *p;

        પ્રતિજ્ઞા(path);
        પ્રતિજ્ઞા(_mkdirat);

        જો (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                જો (!p)
                        પાછા -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                જો (fd < 0)
                        પાછા -errno;
        } નહીંતર
                p = path;

        પાછા mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

પૂર્ણાંક mkdirat_parents(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, mode_t mode) {
        પાછા mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ખાલી);
}

પૂર્ણાંક mkdir_parents_safe(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        પાછા mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ખાલી);
}

પૂર્ણાંક mkdir_p_internal(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        પૂર્ણાંક r;

        /* Like mkdir -p */

        પ્રતિજ્ઞા(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        જો (r < 0)
                પાછા r;

        જો (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                જો (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        પાછા r;
        } નહીંતર {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                જો (r < 0 && r != -EEXIST)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક mkdir_p(અચલ અક્ષર *path, mode_t mode) {
        પાછા mkdir_p_internal(/* prefix= */ ખાલી, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ખાલી);
}

પૂર્ણાંક mkdir_p_safe(અચલ અક્ષર *prefix, અચલ અક્ષર *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        પાછા mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ખાલી);
}

પૂર્ણાંક mkdir_p_મૂળ_full(અચલ અક્ષર *મૂળ, અચલ અક્ષર *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ અક્ષર *pp = ખાલી, *bn = ખાલી;
        _cleanup_close_ પૂર્ણાંક dfd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(p);

        r = path_extract_directory(p, &pp);
        જો (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                જો (dfd < 0)
                        પાછા -errno;
        } નહીંતર જો (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                પાછા 0;
        નહીંતર જો (r < 0)
                પાછા r;
        નહીંતર {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_મૂળ_full(મૂળ, pp, uid, gid, m, ts, subvolumes);
                જો (r < 0)
                        પાછા r;

                dfd = chase_and_open(pp, મૂળ, CHASE_PREFIX_મૂળ, O_CLOEXEC|O_DIRECTORY, ખાલી);
                જો (dfd < 0)
                        પાછા dfd;
        }

        r = path_extract_filename(p, &bn);
        જો (r == -EADDRNOTAVAIL) /* Already top-level */
                પાછા 0;
        જો (r < 0)
                પાછા r;

        XOpenFlags flags = 0;
        જો (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                જો ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ પૂર્ણાંક nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        જો (nfd == -EEXIST)
                પાછા 0;
        જો (nfd < 0)
                પાછા nfd;

        જો ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                પાછા -errno;

        જો (ts != USEC_INFINITY) {
                સંરચના timespec tspec;
                timespec_store(&tspec, ts);

                જો (futimens(dfd, (અચલ સંરચના timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        પાછા -errno;

                જો (futimens(nfd, (અચલ સંરચના timespec[2]) { tspec, tspec }) < 0)
                        પાછા -errno;
        }

        પાછા 1;
}
