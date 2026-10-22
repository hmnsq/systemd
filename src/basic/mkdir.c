/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "btrfs-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "hashmap.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mkdir.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "user-util.h"

முழு mkdirat_safe_internal(
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        கட்டமைப்பு stat st;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(mode != MODE_INVALID);
        ப்ரதிஜ்ஞா(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        எனில் (r >= 0)
                திரும்பு chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        எனில் (r != -EEXIST)
                திரும்பு r;

        எனில் (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                திரும்பு -errno;

        எனில் ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ எழுத்து *p = வெற்று;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, வெற்று);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                எனில் (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        திரும்பு -errno;
        }

        எனில் (flags & MKDIR_IGNORE_EXISTING)
                திரும்பு 0;

        எனில் (!S_ISDIR(st.st_mode))
                திரும்பு log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        எனில் ((st.st_mode & ~mode & 0777) != 0)
                திரும்பு log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        எனில் ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                திரும்பு log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        திரும்பு 0;
}

முழு mkdirat_errno_wrapper(முழு dirfd, மாறிலி எழுத்து *pathname, mode_t mode, LabelContext *label_context) {
        திரும்பு RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

முழு mkdirat_safe(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        திரும்பு mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ வெற்று);
}

முழு mkdirat_parents_internal(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        மாறிலி எழுத்து *e = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(_mkdirat);

        எனில் (isempty(path))
                திரும்பு 0;

        எனில் (!path_is_safe(path))
                திரும்பு -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, வெற்று);
        எனில் (r <= 0) /* r == 0 means path is equivalent to prefix. */
                திரும்பு r;
        எனில் (e == path)
                திரும்பு 0;

        ப்ரதிஜ்ஞா(e > path);
        ப்ரதிஜ்ஞா(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        எனில் (r > 0)
                திரும்பு 0;
        எனில் (r == 0)
                திரும்பு -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        க்கு (மாறிலி எழுத்து *p = path;;) {
                எழுத்து *s;
                முழு n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (மாறிலி எழுத்து **) &s);
                எனில் (n <= 0)
                        திரும்பு n;

                ப்ரதிஜ்ஞா(p);
                ப்ரதிஜ்ஞா(s >= path);
                ப்ரதிஜ்ஞா(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                எனில் (r < 0 && r != -EEXIST)
                        திரும்பு r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

முழு mkdir_parents_internal(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ முழு fd = AT_FDCWD;
        மாறிலி எழுத்து *p;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(_mkdirat);

        எனில் (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                எனில் (!p)
                        திரும்பு -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                எனில் (fd < 0)
                        திரும்பு -errno;
        } இல்லையெனில்
                p = path;

        திரும்பு mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

முழு mkdirat_parents(முழு dir_fd, மாறிலி எழுத்து *path, mode_t mode) {
        திரும்பு mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ வெற்று);
}

முழு mkdir_parents_safe(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        திரும்பு mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ வெற்று);
}

முழு mkdir_p_internal(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        முழு r;

        /* Like mkdir -p */

        ப்ரதிஜ்ஞா(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                எனில் (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        திரும்பு r;
        } இல்லையெனில் {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                எனில் (r < 0 && r != -EEXIST)
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு mkdir_p(மாறிலி எழுத்து *path, mode_t mode) {
        திரும்பு mkdir_p_internal(/* prefix= */ வெற்று, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ வெற்று);
}

முழு mkdir_p_safe(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        திரும்பு mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ வெற்று);
}

முழு mkdir_p_வேர்_full(மாறிலி எழுத்து *வேர், மாறிலி எழுத்து *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ எழுத்து *pp = வெற்று, *bn = வெற்று;
        _cleanup_close_ முழு dfd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(p);

        r = path_extract_directory(p, &pp);
        எனில் (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                எனில் (dfd < 0)
                        திரும்பு -errno;
        } இல்லையெனில் எனில் (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                திரும்பு 0;
        இல்லையெனில் எனில் (r < 0)
                திரும்பு r;
        இல்லையெனில் {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_வேர்_full(வேர், pp, uid, gid, m, ts, subvolumes);
                எனில் (r < 0)
                        திரும்பு r;

                dfd = chase_and_open(pp, வேர், CHASE_PREFIX_வேர், O_CLOEXEC|O_DIRECTORY, வெற்று);
                எனில் (dfd < 0)
                        திரும்பு dfd;
        }

        r = path_extract_filename(p, &bn);
        எனில் (r == -EADDRNOTAVAIL) /* Already top-level */
                திரும்பு 0;
        எனில் (r < 0)
                திரும்பு r;

        XOpenFlags flags = 0;
        எனில் (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                எனில் ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ முழு nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        எனில் (nfd == -EEXIST)
                திரும்பு 0;
        எனில் (nfd < 0)
                திரும்பு nfd;

        எனில் ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                திரும்பு -errno;

        எனில் (ts != USEC_INFINITY) {
                கட்டமைப்பு timespec tspec;
                timespec_store(&tspec, ts);

                எனில் (futimens(dfd, (மாறிலி கட்டமைப்பு timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        திரும்பு -errno;

                எனில் (futimens(nfd, (மாறிலி கட்டமைப்பு timespec[2]) { tspec, tspec }) < 0)
                        திரும்பு -errno;
        }

        திரும்பு 1;
}
