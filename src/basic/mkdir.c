/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "btrfs-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "hashmap.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mkdir.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "user-util.h"

പൂർണ്ണസംഖ്യ mkdirat_safe_internal(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                mode_t mode,
                uid_t uid,
                gid_t gid,
                MkdirFlags flags,
                mkdirat_func_t _mkdirat,
                LabelContext *label_context) {

        ഘടന stat st;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(mode != MODE_INVALID);
        പ്രതിജ്ഞാ(_mkdirat);

        r = _mkdirat(dir_fd, path, mode, label_context);
        എങ്കിൽ (r >= 0)
                തിരികെ chmod_and_chown_at(dir_fd, path, mode, uid, gid);
        എങ്കിൽ (r != -EEXIST)
                തിരികെ r;

        എങ്കിൽ (fstatat(dir_fd, path, &st, AT_SYMLINK_NOFOLLOW) < 0)
                തിരികെ -errno;

        എങ്കിൽ ((flags & MKDIR_FOLLOW_SYMLINK) && S_ISLNK(st.st_mode)) {
                _cleanup_free_ അക്ഷരം *p = ശൂന്യ;

                r = chaseat(XAT_FDROOT, dir_fd, path, CHASE_NONEXISTENT, &p, ശൂന്യ);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ mkdirat_safe_internal(dir_fd, p, mode, uid, gid,
                                                     flags & ~MKDIR_FOLLOW_SYMLINK,
                                                     _mkdirat, label_context);

                എങ്കിൽ (fstatat(dir_fd, p, &st, AT_SYMLINK_NOFOLLOW) < 0)
                        തിരികെ -errno;
        }

        എങ്കിൽ (flags & MKDIR_IGNORE_EXISTING)
                തിരികെ 0;

        എങ്കിൽ (!S_ISDIR(st.st_mode))
                തിരികെ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(ENOTDIR),
                                      "Path \"%s\" already exists and is not a directory, refusing.", path);

        എങ്കിൽ ((st.st_mode & ~mode & 0777) != 0)
                തിരികെ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but has mode %04o that is too permissive (%04o was requested), refusing.",
                                      path, st.st_mode & 0777, mode);

        എങ്കിൽ ((uid != UID_INVALID && st.st_uid != uid) ||
            (gid != GID_INVALID && st.st_gid != gid))
                തിരികെ log_full_errno(flags & MKDIR_WARN_MODE ? LOG_WARNING : LOG_DEBUG, SYNTHETIC_ERRNO(EEXIST),
                                      "Directory \"%s\" already exists, but is owned by "UID_FMT":"GID_FMT" (%s:%s was requested), refusing.",
                                      path, st.st_uid, st.st_gid, uid != UID_INVALID ? FORMAT_UID(uid) : "-",
                                      gid != UID_INVALID ? FORMAT_GID(gid) : "-");

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ mkdirat_errno_wrapper(പൂർണ്ണസംഖ്യ dirfd, സ്ഥിരം അക്ഷരം *pathname, mode_t mode, LabelContext *label_context) {
        തിരികെ RET_NERRNO(mkdirat(dirfd, pathname, mode));
}

പൂർണ്ണസംഖ്യ mkdirat_safe(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        തിരികെ mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ mkdirat_parents_internal(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        സ്ഥിരം അക്ഷരം *e = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(_mkdirat);

        എങ്കിൽ (isempty(path))
                തിരികെ 0;

        എങ്കിൽ (!path_is_safe(path))
                തിരികെ -ENOTDIR;

        /* return immediately if directory exists */
        r = path_find_last_component(path, /* accept_dot_dot= */ false, &e, ശൂന്യ);
        എങ്കിൽ (r <= 0) /* r == 0 means path is equivalent to prefix. */
                തിരികെ r;
        എങ്കിൽ (e == path)
                തിരികെ 0;

        പ്രതിജ്ഞാ(e > path);
        പ്രതിജ്ഞാ(*e == '/');

        /* drop the last component */
        path = strndupa_safe(path, e - path);
        r = is_dir_at(dir_fd, path, /* follow= */ true);
        എങ്കിൽ (r > 0)
                തിരികെ 0;
        എങ്കിൽ (r == 0)
                തിരികെ -ENOTDIR;

        /* create every parent directory in the path, except the last component */
        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = path;;) {
                അക്ഷരം *s;
                പൂർണ്ണസംഖ്യ n;

                n = path_find_first_component(&p, /* accept_dot_dot= */ false, (സ്ഥിരം അക്ഷരം **) &s);
                എങ്കിൽ (n <= 0)
                        തിരികെ n;

                പ്രതിജ്ഞാ(p);
                പ്രതിജ്ഞാ(s >= path);
                പ്രതിജ്ഞാ(IN_SET(s[n], '/', '\0'));

                s[n] = '\0';

                r = mkdirat_safe_internal(dir_fd, path, mode, uid, gid, flags | MKDIR_IGNORE_EXISTING, _mkdirat, label_context);
                എങ്കിൽ (r < 0 && r != -EEXIST)
                        തിരികെ r;

                s[n] = *p == '\0' ? '\0' : '/';
        }
}

പൂർണ്ണസംഖ്യ mkdir_parents_internal(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = AT_FDCWD;
        സ്ഥിരം അക്ഷരം *p;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(_mkdirat);

        എങ്കിൽ (prefix) {
                p = path_startswith_full(path, prefix, PATH_STARTSWITH_REFUSE_DOT_DOT);
                എങ്കിൽ (!p)
                        തിരികെ -EINVAL;

                fd = open(prefix, O_PATH|O_DIRECTORY|O_CLOEXEC);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;
        } അല്ലെങ്കിൽ
                p = path;

        തിരികെ mkdirat_parents_internal(fd, p, mode, uid, gid, flags, _mkdirat, label_context);
}

പൂർണ്ണസംഖ്യ mkdirat_parents(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, mode_t mode) {
        തിരികെ mkdirat_parents_internal(dir_fd, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ mkdir_parents_safe(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        തിരികെ mkdir_parents_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ mkdir_p_internal(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags, mkdirat_func_t _mkdirat, LabelContext *label_context) {
        പൂർണ്ണസംഖ്യ r;

        /* Like mkdir -p */

        പ്രതിജ്ഞാ(_mkdirat);

        r = mkdir_parents_internal(prefix, path, mode, uid, gid, flags | MKDIR_FOLLOW_SYMLINK, _mkdirat, label_context);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!uid_is_valid(uid) && !gid_is_valid(gid) && flags == 0) {
                r = _mkdirat(AT_FDCWD, path, mode, label_context);
                എങ്കിൽ (r < 0 && (r != -EEXIST || is_dir(path, true) <= 0))
                        തിരികെ r;
        } അല്ലെങ്കിൽ {
                r = mkdir_safe_internal(path, mode, uid, gid, flags, _mkdirat, label_context);
                എങ്കിൽ (r < 0 && r != -EEXIST)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ mkdir_p(സ്ഥിരം അക്ഷരം *path, mode_t mode) {
        തിരികെ mkdir_p_internal(/* prefix= */ ശൂന്യ, path, mode, UID_INVALID, UID_INVALID, 0, mkdirat_errno_wrapper, /* label_context= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ mkdir_p_safe(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, mode_t mode, uid_t uid, gid_t gid, MkdirFlags flags) {
        തിരികെ mkdir_p_internal(prefix, path, mode, uid, gid, flags, mkdirat_errno_wrapper, /* label_context= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ mkdir_p_വേര്_full(സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം *p, uid_t uid, gid_t gid, mode_t m, usec_t ts, Hashmap *subvolumes) {
        _cleanup_free_ അക്ഷരം *pp = ശൂന്യ, *bn = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ dfd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);

        r = path_extract_directory(p, &pp);
        എങ്കിൽ (r == -EDESTADDRREQ) {
                /* only fname is passed, no prefix to operate on */
                dfd = open(".", O_RDONLY|O_CLOEXEC|O_DIRECTORY);
                എങ്കിൽ (dfd < 0)
                        തിരികെ -errno;
        } അല്ലെങ്കിൽ എങ്കിൽ (r == -EADDRNOTAVAIL)
                /* only root dir or "." was passed, i.e. there is no parent to extract, in that case there's nothing to do. */
                തിരികെ 0;
        അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                തിരികെ r;
        അല്ലെങ്കിൽ {
                /* Extracting the parent dir worked, hence we aren't top-level? Recurse up first. */
                r = mkdir_p_വേര്_full(വേര്, pp, uid, gid, m, ts, subvolumes);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                dfd = chase_and_open(pp, വേര്, CHASE_PREFIX_വേര്, O_CLOEXEC|O_DIRECTORY, ശൂന്യ);
                എങ്കിൽ (dfd < 0)
                        തിരികെ dfd;
        }

        r = path_extract_filename(p, &bn);
        എങ്കിൽ (r == -EADDRNOTAVAIL) /* Already top-level */
                തിരികെ 0;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        XOpenFlags flags = 0;
        എങ്കിൽ (hashmap_contains(subvolumes, p)) {
                flags = XO_SUBVOLUME;
                എങ്കിൽ ((PTR_TO_INT(hashmap_get(subvolumes, p)) & BTRFS_SUBVOL_NODATACOW))
                        flags |= XO_NOCOW;
        }

        _cleanup_close_ പൂർണ്ണസംഖ്യ nfd = xopenat_full(
                                dfd, bn,
                                O_DIRECTORY|O_CREAT|O_EXCL|O_NOFOLLOW|O_CLOEXEC,
                                flags,
                                m);
        എങ്കിൽ (nfd == -EEXIST)
                തിരികെ 0;
        എങ്കിൽ (nfd < 0)
                തിരികെ nfd;

        എങ്കിൽ ((uid_is_valid(uid) || gid_is_valid(gid)) && fchown(nfd, uid, gid) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ts != USEC_INFINITY) {
                ഘടന timespec tspec;
                timespec_store(&tspec, ts);

                എങ്കിൽ (futimens(dfd, (സ്ഥിരം ഘടന timespec[2]) { TIMESPEC_OMIT, tspec }) < 0)
                        തിരികെ -errno;

                എങ്കിൽ (futimens(nfd, (സ്ഥിരം ഘടന timespec[2]) { tspec, tspec }) < 0)
                        തിരികെ -errno;
        }

        തിരികെ 1;
}
