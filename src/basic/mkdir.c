/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "btrfs-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mkdir.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_safe_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ସଂରଚନା stat st;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(mode != MODE_INVALID);
        ପ୍ରତିଜ୍ଞା(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        ଯଦି (r >= 0)
                ଫେରନ୍ତୁ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        ଯଦି (r != -EEXIST)
                ଫେରନ୍ତୁ r;

        ଯଦି (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, ଖାଲି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ଯଦି (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଯଦି (flags & MKDIR_IGNORE_EXISTING)
                ଫେରନ୍ତୁ 0;

        ଯଦି (!S_ISDIR(st.st_mode))
                ଫେରନ୍ତୁ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        ଯଦି ((st.st_mode & ~mode & 0777) != 0)
                ଫେରନ୍ତୁ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        ଯଦି ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ଫେରନ୍ତୁ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_errno_wrapper(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dirfd, ଅଚଳ ଅକ୍ଷର *pathname, mode_t mode, LabelContext *label_context) {
        ଫେରନ୍ତୁ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ଫେରନ୍ତୁ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_parents_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ଅଚଳ ଅକ୍ଷର *e = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(_mkdirat);

        ଯଦି (isempty(path))
                ଫେରନ୍ତୁ 0;

        ଯଦି (!path_is_safe(path))
                ଫେରନ୍ତୁ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, ଖାଲି);
        ଯଦି (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ଫେରନ୍ତୁ r;
        ଯଦି (e == path)
                ଫେରନ୍ତୁ 0;

        ପ୍ରତିଜ୍ଞା(e > path);
        ପ୍ରତିଜ୍ଞା(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ 0;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = path;;) {
                ଅକ୍ଷର *s;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (ଅଚଳ ଅକ୍ଷର **) &s);
                ଯଦି (n <= 0)
                        ଫେରନ୍ତୁ n;

                ପ୍ରତିଜ୍ଞା(p);
                ପ୍ରତିଜ୍ଞା(s >= path);
                ପ୍ରତିଜ୍ଞା(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ଯଦି (r < 0 && r != -EEXIST)
                        ଫେରନ୍ତୁ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_parents_internal(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = AT_FDCWD;
        ଅଚଳ ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(_mkdirat);

        ଯଦି (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ -errno;
        } ଅନ୍ୟଥା
                p = path;

        ଫେରନ୍ତୁ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdirat_parents(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, ଅଚଳ ଅକ୍ଷର *path, mode_t mode) {
        ଫେରନ୍ତୁ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_parents_safe(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ଫେରନ୍ତୁ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_internal(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Like mkdir -p */

        ପ୍ରତିଜ୍ଞା(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                ଯଦି (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        ଫେରନ୍ତୁ r;
        } ଅନ୍ୟଥା {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                ଯଦି (r < 0 && r != -EEXIST)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p(ଅଚଳ ଅକ୍ଷର *path, mode_t mode) {
        ଫେରନ୍ତୁ mkdir_p_internal(/* prefix= */ ଖାଲି, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_safe(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        ଫେରନ୍ତୁ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା mkdir_p_ମୂଳ_full(ଅଚଳ ଅକ୍ଷର *ମୂଳ, ଅଚଳ ଅକ୍ଷର *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ ଅକ୍ଷର *pp = ଖାଲି, *bn = ଖାଲି;
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dfd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(p);

        r = path_extract_directory(p, &pp);
        ଯଦି (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ଯଦି (dfd < 0)
                        ଫେରନ୍ତୁ -errno;
        } ଅନ୍ୟଥା ଯଦି (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ଫେରନ୍ତୁ 0;
        ଅନ୍ୟଥା ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଅନ୍ୟଥା {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_ମୂଳ_full(ମୂଳ, pp, uid, gid, m, ts, subvolumes);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                dfd = chase_and_open(pp, ମୂଳ, CHASE_PREFIX_ମୂଳ, O_CLOEXEC|O_DIRECTORY, ଖାଲି);
                ଯଦି (dfd < 0)
                        ଫେରନ୍ତୁ dfd;
        }

        r = path_extract_filename(p, &bn);
        ଯଦି (r == -EADDRNOTAVAIL) /* Already top-level */
                ଫେରନ୍ତୁ 0;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        XOpenFlags flags = 0;
        ଯଦି (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ଯଦି ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ଯଦି (nfd == -EEXIST)
                ଫେରନ୍ତୁ 0;
        ଯଦି (nfd < 0)
                ଫେରନ୍ତୁ nfd;

        ଯଦି ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ts != USEC_INFINITY) {
                ସଂରଚନା timespec tspec;
                timespec_store(&tspec, ts);

                ଯଦି (futimens(dfd, (ଅଚଳ ସଂରଚନା timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (futimens(nfd, (ଅଚଳ ସଂରଚନା timespec[2]) { tspec, tspec }) < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ 1;
}
