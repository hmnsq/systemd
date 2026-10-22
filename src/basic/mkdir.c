/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "btrfs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mkdir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

ਪੂਰਨ mkdirat_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                mode_t ਬਹੁਲਕ,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ਬਣਤਰ stat st;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ != ਬਹੁਲਕ_INVALID);
        ਪ੍ਰਤਿਜ੍ਞਾ(_mkdirat);

        r = _mkdirat(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, label_context);
        ਜੇ (r >= 0)
                ਵਾਪਸ chmod_and_chown_at(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid);
        ਜੇ (r != -EEXIST)
                ਵਾਪਸ r;

        ਜੇ (fstatat(dir_fd, ਰਸਤਾ, &st, AT_SYMLINK_NOFOLLOW) < 0)
                ਵਾਪਸ -errno;

        ਜੇ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;

                r = chaseat(XAT_FDROOT, dir_fd, ਰਸਤਾ, CHASE_NONEXISTENT, &p, ਨਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ mkdirat_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(dir_fd, p, ਬਹੁਲਕ, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                ਜੇ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        ਵਾਪਸ -errno;
        }

        ਜੇ (flags & MKDIR_IGNORE_EXISTING)
                ਵਾਪਸ 0;

        ਜੇ (!S_ISDIR(st.st_mode))
                ਵਾਪਸ log_ਭਰਿਆ_errno(flags & MKDIR_WARN_ਬਹੁਲਕ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", ਰਸਤਾ);

        ਜੇ ((st.st_mode & ~ਬਹੁਲਕ & 0777) != 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(flags & MKDIR_WARN_ਬਹੁਲਕ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      ਰਸਤਾ, st.st_mode & 0777, ਬਹੁਲਕ);

        ਜੇ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                ਵਾਪਸ log_ਭਰਿਆ_errno(flags & MKDIR_WARN_ਬਹੁਲਕ ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      ਰਸਤਾ, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        ਵਾਪਸ 0;
}

ਪੂਰਨ mkdirat_errno_wrapper(ਪੂਰਨ dirfd, ਅਚਲ ਅੱਖਰ *pathname, mode_t ਬਹੁਲਕ, LabelContext *label_context) {
        ਵਾਪਸ RET_NERRNO(mkdirat(dirfd, pathname, ਬਹੁਲਕ));
}

ਪੂਰਨ mkdirat_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ਵਾਪਸ mkdirat_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ਨਿਰਾ);
}

ਪੂਰਨ mkdirat_parents_ਅੰਦਰੂਨੀ(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ਅਚਲ ਅੱਖਰ *e = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(_mkdirat);

        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ 0;

        ਜੇ (!ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(ਰਸਤਾ))
                ਵਾਪਸ -ENOTDIR;

        /* return immediately if directory exists */
        r = ਰਸਤਾ_ਲੱਭੋ_ਆਖਰੀ_component(ਰਸਤਾ, /* accept_dot_dot= */ false, &e, ਨਿਰਾ);
        ਜੇ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                ਵਾਪਸ r;
        ਜੇ (e == ਰਸਤਾ)
                ਵਾਪਸ 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(e > ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(*e == '/');

        /* drop the last component */
        ਰਸਤਾ = strndupa_ਸੁਰੱਖਿਅਤ(ਰਸਤਾ, e - ਰਸਤਾ);
        r = is_dir_at(dir_fd, ਰਸਤਾ, /* follow= */ true);
        ਜੇ (r > 0)
                ਵਾਪਸ 0;
        ਜੇ (r == 0)
                ਵਾਪਸ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        ਲਈ (ਅਚਲ ਅੱਖਰ *p = ਰਸਤਾ;;) {
                ਅੱਖਰ *s;
                ਪੂਰਨ n;

                n = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&p, /* accept_dot_dot= */ false, (ਅਚਲ ਅੱਖਰ **) &s);
                ਜੇ (n <= 0)
                        ਵਾਪਸ n;

                ਪ੍ਰਤਿਜ੍ਞਾ(p);
                ਪ੍ਰਤਿਜ੍ਞਾ(s >= ਰਸਤਾ);
                ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                ਜੇ (r < 0 && r != -EEXIST)
                        ਵਾਪਸ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

ਪੂਰਨ mkdir_parents_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ ਪੂਰਨ fd = AT_FDCWD;
        ਅਚਲ ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(_mkdirat);

        ਜੇ (ਅਗੇਤਰ) {
                p = ਰਸਤਾ_startswith_ਭਰਿਆ(ਰਸਤਾ, ਅਗੇਤਰ, ਰਸਤਾ_STARTSWITH_REFUSE_DOT_DOT);
                ਜੇ (!p)
                        ਵਾਪਸ -EINVAL;

                fd = open(ਅਗੇਤਰ, O_ਰਸਤਾ|O_DIRECTORY|O_CLOEXEC);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;
        } ਨਹੀਂਤਾਂ
                p = ਰਸਤਾ;

        ਵਾਪਸ mkdirat_parents_ਅੰਦਰੂਨੀ(fd, p, ਬਹੁਲਕ, uid, gid, flags, _mkdirat, label_context);
}

ਪੂਰਨ mkdirat_parents(ਪੂਰਨ dir_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ mkdirat_parents_ਅੰਦਰੂਨੀ(dir_fd, ਰਸਤਾ, ਬਹੁਲਕ, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ਨਿਰਾ);
}

ਪੂਰਨ mkdir_parents_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ਵਾਪਸ mkdir_parents_ਅੰਦਰੂਨੀ(ਅਗੇਤਰ, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ਨਿਰਾ);
}

ਪੂਰਨ mkdir_p_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        ਪੂਰਨ r;

        /* Like mkdir -p */

        ਪ੍ਰਤਿਜ੍ਞਾ(_mkdirat);

        r = mkdir_parents_ਅੰਦਰੂਨੀ(ਅਗੇਤਰ, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, ਰਸਤਾ, ਬਹੁਲਕ, label_context);
                ਜੇ (r < 0 && (r != -EEXIST || is_dir(ਰਸਤਾ, true) <= 0))
                        ਵਾਪਸ r;
        } ਨਹੀਂਤਾਂ {
                r = mkdir_ਸੁਰੱਖਿਅਤ_ਅੰਦਰੂਨੀ(ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags, _mkdirat, label_context);
                ਜੇ (r < 0 && r != -EEXIST)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ mkdir_p(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ) {
        ਵਾਪਸ mkdir_p_ਅੰਦਰੂਨੀ(/* prefix= */ ਨਿਰਾ, ਰਸਤਾ, ਬਹੁਲਕ, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ਨਿਰਾ);
}

ਪੂਰਨ mkdir_p_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, mode_t ਬਹੁਲਕ, uid_t uid, gid_t gid, MkdirFlags flags) {
        ਵਾਪਸ mkdir_p_ਅੰਦਰੂਨੀ(ਅਗੇਤਰ, ਰਸਤਾ, ਬਹੁਲਕ, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ਨਿਰਾ);
}

ਪੂਰਨ mkdir_p_ਜੜ੍ਹ_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅਚਲ ਅੱਖਰ *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ ਅੱਖਰ *pp = ਨਿਰਾ, *bn = ਨਿਰਾ;
        _cleanup_close_ ਪੂਰਨ dfd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        r = ਰਸਤਾ_ਕੱਢੋ_directory(p, &pp);
        ਜੇ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                ਜੇ (dfd < 0)
                        ਵਾਪਸ -errno;
        } ਨਹੀਂਤਾਂ ਜੇ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                ਵਾਪਸ 0;
        ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਨਹੀਂਤਾਂ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_ਜੜ੍ਹ_ਭਰਿਆ(ਜੜ੍ਹ, pp, uid, gid, m, ts, subvolumes);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                dfd = chase_and_open(pp, ਜੜ੍ਹ, CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ, O_CLOEXEC|O_DIRECTORY, ਨਿਰਾ);
                ਜੇ (dfd < 0)
                        ਵਾਪਸ dfd;
        }

        r = ਰਸਤਾ_ਕੱਢੋ_filename(p, &bn);
        ਜੇ (r == -EADDRNOTAVAIL) /* Already top-level */
                ਵਾਪਸ 0;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        XOpenFlags flags = 0;
        ਜੇ (hashmap_ਸ਼ਾਮਲ_ਹੈ(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                ਜੇ ((PTR_ਮੰਜ਼ਿਲ_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ ਪੂਰਨ nfd = xopenat_ਭਰਿਆ(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        ਜੇ (nfd == -EEXIST)
                ਵਾਪਸ 0;
        ਜੇ (nfd < 0)
                ਵਾਪਸ nfd;

        ਜੇ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ts != USEC_INFINITY) {
                ਬਣਤਰ timespec tspec;
                timespec_store(&tspec, ts);

                ਜੇ (futimens(dfd, (ਅਚਲ ਬਣਤਰ timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (futimens(nfd, (ਅਚਲ ਬਣਤਰ timespec[2]) { tspec, tspec }) < 0)
                        ਵਾਪਸ -errno;
        }

        ਵਾਪਸ 1;
}
