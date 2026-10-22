/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <sys/file.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>
#ضمن <utmpx.h>

#ضمن "sd-messages.h"

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "format-util.h"
#ضمن "lock-util.h"
#ضمن "log.h"
#ضمن "mkdir.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "terminal-util.h"
#ضمن "user-util.h"
#ضمن "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        ثابت حرف* strerror_##type(                                    \
                        صحيح errnum,                                     \
                        حرف *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                بدل (errnum) {                                       \
                حالة ESRCH:                                             \
                        החזר "Unknown " STRINGIFY(type);              \
                حالة ENOEXEC:                                           \
                        החזר "Not a system " STRINGIFY(type);         \
                افتراضي:                                                \
                        החזר strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        אם (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                החזר false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        אם (uid == (uid_t) UINT32_C(0xFFFF))
                החזר false;

        החזר true;
}

صحيح parse_uid(ثابت حرف *s, uid_t *ret) {
        uint32_t uid = 0;
        صحيح r;

        أكد(s);

        assert_cc(حجم(uid_t) == حجم(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        אם (r < 0)
                החזר r;

        אם (!uid_is_valid(uid))
                החזר -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        אם (ret)
                *ret = uid;

        החזר 0;
}

صحيح parse_uid_range(ثابت حرف *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ حرف *word = عدم;
        uid_t l, u;
        صحيح r;

        أكد(s);
        أكد(ret_lower);
        أكد(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        אם (r < 0)
                החזר r;
        אם (r == 0)
                החזר -EINVAL;

        r = parse_uid(word, &l);
        אם (r < 0)
                החזר r;

        /* Check for the upper bound and extract it if needed */
        אם (!s)
                /* Single number with no dash. */
                u = l;
        אחרת אם (!*s)
                /* Trailing dash is an error. */
                החזר -EINVAL;
        אחרת {
                r = parse_uid(s, &u);
                אם (r < 0)
                        החזר r;

                אם (l > u)
                        החזר -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        החזר 0;
}

حرف* getlogname_malloc(فراغ) {
        uid_t uid;
        بنية stat st;

        אם (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        אחרת
                uid = getuid();

        החזר uid_to_name(uid);
}

حرف* getusername_malloc(فراغ) {
        ثابت حرف *e;

        e = secure_getenv("USER");
        אם (e)
                החזר strdup(e);

        החזר uid_to_name(getuid());
}

bool is_nologin_shell(ثابت حرف *shell) {
        החזר PATH_IN_SET(shell,
                           /* 'nologin' is the friendliest way to disable logins for a user account. It prints a nice
                            * message and exits. Different distributions place the binary at different places though,
                            * hence let's list them all. */
                           "/bin/nologin",
                           "/sbin/nologin",
                           "/usr/bin/nologin",
                           "/usr/sbin/nologin",
                           /* 'true' and 'false' work too for the same purpose, but are less friendly as they don't do
                            * any message printing. Different distributions place the binary at various places but at
                            * least not in the 'sbin' directory. */
                           "/bin/false",
                           "/usr/bin/false",
                           "/bin/true",
                           "/usr/bin/true");
}

bool shell_is_placeholder(ثابت حرف *shell) {
        החזר isempty(shell) || is_nologin_shell(shell);
}

ثابت حرف* default_root_shell_at(صحيح rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        أكد(rfd >= 0 || rfd == AT_FDCWD);

        صحيح r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, عدم, عدم);
        אם (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        אם (r > 0)
                החזר DEFAULT_USER_SHELL;

        החזר "/bin/sh";
}

ثابت حرف* default_root_shell(ثابت حرف *root) {
        _cleanup_close_ صحيح rfd = -EBADF;

        rfd = open(empty_to_root(root), O_CLOEXEC | O_DIRECTORY | O_PATH);
        אם (rfd < 0)
                החזר "/bin/sh";

        החזר default_root_shell_at(rfd);
}

ساكن صحيح return_user_creds(
                ثابت حرف *username,
                uid_t uid, gid_t gid,
                ثابت حرف *home,
                ثابت حرف *shell,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ حرف *s1 = عدم, *s2 = عدم, *s3 = عدم;
        صحيح r;

        אם (ret_username) {
                r = strdup_to(&s1, username);
                אם (r < 0)
                        החזר r;
        }

        אם (ret_home) {
                r = strdup_to(&s2, home);
                אם (r < 0)
                        החזר r;
        }

        אם (ret_shell) {
                r = strdup_to(&s3, shell);
                אם (r < 0)
                        החזר r;
        }

        אם (ret_username)
                *ret_username = TAKE_PTR(s1);
        אם (ret_uid)
                *ret_uid = uid;
        אם (ret_gid)
                *ret_gid = gid;
        אם (ret_home)
                *ret_home = TAKE_PTR(s2);
        אם (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        החזר 0;
}

ساكن صحيح synthesize_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {
        أكد(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        אם (STR_IN_SET(username, "root", "0"))
                החזר return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_root_shell(عدم) : عدم,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        אם (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                החזר return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? عدم : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? عدم : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        החזר -ENOMEDIUM;
}

صحيح get_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ بنية passwd *pw = عدم;
        صحيح r;

        أكد(username);
        أكد((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        אם (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
            (!ret_home && !ret_shell)) {

                /* So here's the deal: normally, we'll try to synthesize all records we can synthesize, and override
                 * the user database with that. However, if the user specifies USER_CREDS_PREFER_NSS then the
                 * user database will override the synthetic records instead — except if the user is only interested in
                 * the UID and/or GID (but not the home directory, or the shell), in which case we'll always override
                 * the user database (i.e. the USER_CREDS_PREFER_NSS flag has no effect in this case). Why?
                 * Simply because there are valid usecase where the user might change the home directory or the shell
                 * of the relevant users, but changing the UID/GID mappings for them is something we explicitly don't
                 * support. */

                r = synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell);
                אם (r >= 0)
                        החזר 0;
                אם (r != -ENOMEDIUM) /* not a username we can synthesize */
                        החזר r;
        }

        אם (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                אם (r >= 0)
                        username = pw->pw_name;

                אחרת אם (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        אם (ret_username)
                                *ret_username = عدم;
                        אם (ret_uid)
                                *ret_uid = u;
                        החזר 0;
                }
        } אחרת
                r = getpwnam_malloc(username, &pw);

        אם (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                אם (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        החזר 0;

                החזר r;
        }

        אם (ret_uid && !uid_is_valid(pw->pw_uid))
                החזר -EBADMSG;

        אם (ret_gid && !gid_is_valid(pw->pw_gid))
                החזר -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ثابت حرف *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_root(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? عدم : pw->pw_dir;

        ثابت حرف *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? عدم : pw->pw_shell;

        החזר return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ساكن صحيح synthesize_group_creds(ثابت حرف *groupname, حرف **ret_name, gid_t *ret_gid) {
        أكد(groupname);

        gid_t id;
        ثابت حرف *n;
        صحيح r;

        אם (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } אחרת אם (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } אחרת
                החזר -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        אם (r < 0)
                החזר r;
        אם (ret_gid)
                *ret_gid = id;
        החזר 0;
}

صحيح get_group_creds(ثابت حرف *groupname, UserCredsFlags flags, حرف **ret_name, gid_t *ret_gid) {
        _cleanup_free_ بنية group *gr = عدم;
        gid_t id;
        صحيح r;

        أكد(groupname);

        אם (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                אם (r >= 0)
                        החזר 0;
                אם (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        החזר r;
        }

        אם (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                אם (r >= 0)
                        groupname = gr->gr_name;
                אחרת אם (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        אם (ret_gid)
                                *ret_gid = id;
                        אם (ret_name)
                                *ret_name = عدم;
                        החזר 0;
                }
        } אחרת
                r = getgrnam_malloc(groupname, &gr);

        אם (r < 0) {
                אם (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        החזר 0;
                החזר r;
        }

        אם (ret_gid && !gid_is_valid(gr->gr_gid))
                החזר -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        אם (r < 0)
                החזר r;
        אם (ret_gid)
                *ret_gid = gr->gr_gid;
        החזר 0;
}

حرف* uid_to_name(uid_t uid) {
        حرف *ret;
        صحيح r;

        /* Shortcut things to avoid NSS lookups */
        אם (uid == 0)
                החזר strdup("root");
        אם (uid == UID_NOBODY && synthesize_nobody())
                החזר strdup(NOBODY_USER_NAME);

        אם (uid_is_valid(uid)) {
                _cleanup_free_ بنية passwd *pw = عدم;

                r = getpwuid_malloc(uid, &pw);
                אם (r >= 0)
                        החזר strdup(pw->pw_name);
        }

        אם (asprintf(&ret, UID_FMT, uid) < 0)
                החזר عدم;

        החזר ret;
}

حرف* gid_to_name(gid_t gid) {
        حرف *ret;
        صحيح r;

        אם (gid == 0)
                החזר strdup("root");
        אם (gid == GID_NOBODY && synthesize_nobody())
                החזר strdup(NOBODY_GROUP_NAME);

        אם (gid_is_valid(gid)) {
                _cleanup_free_ بنية group *gr = عدم;

                r = getgrgid_malloc(gid, &gr);
                אם (r >= 0)
                        החזר strdup(gr->gr_name);
        }

        אם (asprintf(&ret, GID_FMT, gid) < 0)
                החזר عدم;

        החזר ret;
}

ساكن bool gid_list_has(ثابت gid_t *list, size_t size, gid_t val) {
        أكد(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                אם (*i == val)
                        החזר true;

        החזר false;
}

صحيح in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = عدم;
        صحيح ngroups;

        אם (getgid() == gid)
                החזר 1;

        אם (getegid() == gid)
                החזר 1;

        אם (!gid_is_valid(gid))
                החזר -EINVAL;

        ngroups = getgroups_alloc(&gids);
        אם (ngroups < 0)
                החזר ngroups;

        החזר gid_list_has(gids, ngroups, gid);
}

صحيح in_group(ثابت حرف *name) {
        صحيح r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ عدم, &gid);
        אם (r < 0)
                החזר r;

        החזר in_gid(gid);
}

صحيح merge_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        أكد(size1 == 0 || list1);
        أكد(size2 == 0 || list2);
        أكد(ret);

        אם (size2 > INT_MAX - size1)
                החזר -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        אם (!buf)
                החזר -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        עבור (size_t i = 0; i < size1; i++)
                אם (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        עבור (size_t i = 0; i < size2; i++)
                אם (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        החזר (صحيح)nresult;
}

صحيح getgroups_alloc(gid_t **ret) {
        صحيح ngroups = 8;

        أكد(ret);

        עבור (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = عدم;

                p = new(gid_t, ngroups);
                אם (!p)
                        החזר -ENOMEM;

                ngroups = getgroups(ngroups, p);
                אם (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        החזר ngroups;
                }
                אם (ngroups == 0)
                        اكسر;
                אם (errno != EINVAL)
                        החזר -errno;

                /* Give up eventually */
                אם (attempt++ > 10)
                        החזר -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, عدم);
                אם (ngroups < 0)
                        החזר -errno;
                אם (ngroups == 0)
                        اكسر;
        }

        *ret = عدم;
        החזר 0;
}

صحيح get_home_dir(حرف **ret) {
        _cleanup_free_ بنية passwd *p = عدم;
        ثابت حرف *e;
        uid_t u;
        صحيح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        אם (e && path_is_valid(e) && path_is_absolute(e))
                اذهب found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        אם (u == 0) {
                e = "/root";
                اذهب found;
        }
        אם (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                اذهب found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        אם (r < 0)
                החזר r;

        e = p->pw_dir;
        אם (!path_is_valid(e) || !path_is_absolute(e))
                החזר -EINVAL;

 found:
        החזר path_simplify_alloc(e, ret);
}

صحيح get_shell(حرف **ret) {
        _cleanup_free_ بنية passwd *p = عدم;
        ثابت حرف *e;
        uid_t u;
        صحيح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        אם (e && path_is_valid(e) && path_is_absolute(e))
                اذهب found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        אם (u == 0) {
                e = default_root_shell(عدم);
                اذهب found;
        }
        אם (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                اذهب found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        אם (r < 0)
                החזר r;

        e = p->pw_shell;
        אם (!path_is_valid(e) || !path_is_absolute(e))
                החזר -EINVAL;

 found:
        החזר path_simplify_alloc(e, ret);
}

صحيح fully_set_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids) {
        صحيح r;

        أكد(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        אם (r < 0)
                החזר r;

        אם (gid_is_valid(gid))
                אם (setresgid(gid, gid, gid) < 0)
                        החזר -errno;

        אם (uid_is_valid(uid))
                אם (setresuid(uid, uid, uid) < 0)
                        החזר -errno;

        החזר 0;
}

صحيح take_etc_passwd_lock(ثابت حرف *root) {
        صحيح r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ حرف *path = path_join(root, ETC_PASSWD_LOCK_PATH);
        אם (!path)
                החזר log_oom_debug();

        (فراغ) mkdir_parents(path, 0755);

        _cleanup_close_ صحيح fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        אם (fd < 0)
                החזר log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        אם (r < 0)
                החזר log_debug_errno(r, "Locking %s failed: %m", path);

        החזר TAKE_FD(fd);
}

bool valid_user_group_name(ثابت حرف *u, ValidUserFlags flags) {
        ثابت حرف *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        אם (isempty(u)) /* An empty user name is never valid */
                החזר false;

        אם (parse_uid(u, عدم) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                החזר FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        אם (FLAGS_SET(flags, VALID_USER_RELAX)) {

                /* In relaxed mode we just check very superficially. Apparently SSSD and other stuff is
                 * extremely liberal (way too liberal if you ask me, even inserting "@" in user names, which
                 * is bound to cause problems for example when used with an MTA), hence only filter the most
                 * obvious cases, or where things would result in an invalid entry if such a user name would
                 * show up in /etc/passwd (or equivalent getent output).
                 *
                 * Note that we stepped far out of POSIX territory here. It's not our fault though, but
                 * SSSD's, Samba's and everybody else who ignored POSIX on this. (I mean, I am happy to step
                 * outside of POSIX' bounds any day, but I must say in this case I probably wouldn't
                 * have...) */

                אם (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        החזר false;

                אם (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        החזר false;

                אם (string_has_cc(u, عدم)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        החזר false;

                אם (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        החזר false;

                אם (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        החזר false;

                אם (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        החזר false;

                אם (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        החזר false;

                /* Compare with strict result and warn if result doesn't match */
                אם (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } אחרת {
                long sz;
                size_t l;

                /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.437. We are a bit stricter here
                 * however. Specifically we deviate from POSIX rules:
                 *
                 * - We don't allow empty user names (see above)
                 * - We require that names fit into the appropriate utmp field
                 * - We don't allow any dots (this conflicts with chown syntax which permits dots as user/group name separator)
                 * - We don't allow dashes or digit as the first character
                 *
                 * Note that other systems are even more restrictive, and don't permit underscores or uppercase characters.
                 */

                אם (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        החזר false;

                עבור (i = u+1; *i; i++)
                        אם (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                החזר false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                אם (l > (size_t) sz) /* glibc: 256 */
                        החזר false;
                אם (l > NAME_MAX) /* must fit in a filename: 255 */
                        החזר false;
                אם (l > sizeof_field(بنية utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        החזר false;
        }

        החזר true;
}

bool valid_gecos(ثابت حرف *d) {

        אם (!d)
                החזר false;

        אם (!utf8_is_valid(d))
                החזר false;

        אם (string_has_cc(d, عدم))
                החזר false;

        /* Colons are used as field separators, and hence not OK */
        אם (strchr(d, ':'))
                החזר false;

        החזר true;
}

حرف* mangle_gecos(ثابت حرف *d) {
        حرف *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        אם (!mangled)
                החזר عدم;

        עבור (حرف *i = mangled; *i; i++) {
                صحيح len;

                אם ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        استمر;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                אם (len < 0) {
                        *i = ' ';
                        استمر;
                }

                i += len - 1;
        }

        החזר mangled;
}

bool valid_home(ثابت حرف *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        אם (isempty(p))
                החזר false;

        אם (!utf8_is_valid(p))
                החזר false;

        אם (string_has_cc(p, عدم))
                החזר false;

        אם (!path_is_absolute(p))
                החזר false;

        אם (!path_is_normalized(p))
                החזר false;

        /* Colons are used as field separators, and hence not OK */
        אם (strchr(p, ':'))
                החזר false;

        החזר true;
}

bool valid_shell(ثابت حرف *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        אם (!valid_home(p))
                החזר false;

        החזר !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

صحيح maybe_setgroups(size_t size, ثابت gid_t *list) {
        صحيح r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        אם (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                אם (!f && errno != ENOENT)
                        החזר -errno;
                אם (f) {
                        r = safe_fgetc(f, /* ret= */ عدم);
                        אם (r < 0)
                                החזר r;
                        אם (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                החזר 0;
                        }
                }

                _cleanup_free_ حرف *setgroups_content = عدم;
                r = read_one_line_file("/proc/self/setgroups", &setgroups_content);
                אם (r < 0 && r != -ENOENT)
                        החזר r;
                אם (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        החזר 0;
                }
        }

        החזר RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(فراغ) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ساكن صحيح cache = -1;

        אם (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        החזר cache;
}

صحيح putpwent_sane(ثابت بنية passwd *pw, FILE *stream) {
        أكد(pw);
        أكد(stream);

        errno = 0;
        אם (putpwent(pw, stream) != 0)
                החזר errno_or_else(EIO);

        החזר 0;
}

صحيح putspent_sane(ثابت بنية spwd *sp, FILE *stream) {
        أكد(sp);
        أكد(stream);

        errno = 0;
        אם (putspent(sp, stream) != 0)
                החזר errno_or_else(EIO);

        החזר 0;
}

صحيح putgrent_sane(ثابت بنية group *gr, FILE *stream) {
        أكد(gr);
        أكد(stream);

        errno = 0;
        אם (putgrent(gr, stream) != 0)
                החזר errno_or_else(EIO);

        החזר 0;
}

#אם ENABLE_GSHADOW
صحيح putsgent_sane(ثابت بنية sgrp *sg, FILE *stream) {
        أكد(sg);
        أكد(stream);

        errno = 0;
        אם (putsgent(sg, stream) != 0)
                החזר errno_or_else(EIO);

        החזר 0;
}
#endif

صحيح fgetpwent_sane(FILE *stream, بنية passwd **pw) {
        أكد(stream);
        أكد(pw);

        errno = 0;
        بنية passwd *p = fgetpwent(stream);
        אם (!p && !IN_SET(errno, 0, ENOENT))
                החזר -errno;

        *pw = p;
        החזר !!p;
}

صحيح fgetspent_sane(FILE *stream, بنية spwd **sp) {
        أكد(stream);
        أكد(sp);

        errno = 0;
        بنية spwd *s = fgetspent(stream);
        אם (!s && !IN_SET(errno, 0, ENOENT))
                החזר -errno;

        *sp = s;
        החזר !!s;
}

صحيح fgetgrent_sane(FILE *stream, بنية group **gr) {
        أكد(stream);
        أكد(gr);

        errno = 0;
        بنية group *g = fgetgrent(stream);
        אם (!g && !IN_SET(errno, 0, ENOENT))
                החזר -errno;

        *gr = g;
        החזר !!g;
}

#אם ENABLE_GSHADOW
صحيح fgetsgent_sane(FILE *stream, بنية sgrp **sg) {
        أكد(stream);
        أكد(sg);

        errno = 0;
        بنية sgrp *s = fgetsgent(stream);
        אם (!s && !IN_SET(errno, 0, ENOENT))
                החזר -errno;

        *sg = s;
        החזר !!s;
}
#endif

صحيح is_this_me(ثابت حرف *username) {
        uid_t uid;
        صحيح r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, عدم, &uid, عدم, عدم, عدم);
        אם (r < 0)
                החזר r;

        החזר uid == getuid();
}

ثابت حرف* get_home_root(فراغ) {
        /* For debug purposes allow overriding where we look for home dirs */
        ثابت حرف *e = secure_getenv("SYSTEMD_HOME_ROOT");
        אם (e && path_is_absolute(e) && path_is_normalized(e))
                החזר e;

        החזר "/home";
}

ساكن صحيح copy_struct_passwd(ثابت بنية passwd *pw, بنية passwd **ret) {
        أكد(pw);
        أكد(ret);

        size_t need_bytes = حجم(بنية passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        حرف *buf = malloc(need_bytes);
        אם (!buf)
                החזר -ENOMEM;

        بنية passwd *newpw = (بنية passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        حرف *p = buf + حجم(بنية passwd);

        newpw->pw_name = p;
        p = stpcpy(p, strempty(pw->pw_name)) + 1;

        newpw->pw_passwd = p;
        p = stpcpy(p, strempty(pw->pw_passwd)) + 1;

        newpw->pw_uid = pw->pw_uid;
        newpw->pw_gid = pw->pw_gid;

        newpw->pw_gecos = p;
        p = stpcpy(p, strempty(pw->pw_gecos)) + 1;

        newpw->pw_dir = p;
        p = stpcpy(p, strempty(pw->pw_dir)) + 1;

        newpw->pw_shell = p;
        p = stpcpy(p, strempty(pw->pw_shell)) + 1;

        *ret = newpw;
        החזר 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
صحيح lookup_pwent_in_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                uid_t uid,
                بنية passwd **ret) {

        صحيح r;

        أكد(files);
        أكد(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = عدم;
                بنية passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                אם (r == -ENOENT)
                        استمر;
                אם (r < 0)
                        החזר r;

                כלעוד ((r = fgetpwent_sane(f, &pw)) > 0) {
                        אם (name && !streq_ptr(pw->pw_name, name))
                                استمر;
                        אם (uid_is_valid(uid) && pw->pw_uid != uid)
                                استمر;
                        אם (ret)
                                החזר copy_struct_passwd(pw, ret);
                        החזר 0;
                }
                אם (r < 0)
                        החזר r;
        }

        החזר -ESRCH;
}

#אם !BUILD_STATIC

ساكن size_t getpw_buffer_size(فراغ) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        החזר bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ساكن bool errno_is_user_doesnt_exist(صحيح error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        החזר IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

صحيح getpwnam_malloc(ثابت حرف *name, بنية passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        אם (isempty(name))
                החזר -EINVAL;

#אם BUILD_STATIC
        החזר lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#אחרת
        size_t bufsize = getpw_buffer_size();
        صحيح r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        עבור (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية passwd)));
                buf = malloc0(ALIGN(حجم(بنية passwd)) + bufsize);
                אם (!buf)
                        החזר -ENOMEM;

                بنية passwd *pw = عدم;
                r = getpwnam_r(name, buf, (حرف*) buf + ALIGN(حجم(بنية passwd)), bufsize, &pw);
                אם (r == 0) {
                        אם (pw) {
                                אם (ret)
                                        *ret = TAKE_PTR(buf);
                                החזר 0;
                        }

                        החזר -ESRCH;
                }

                أكد(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                אם (errno_is_user_doesnt_exist(r))
                        החזר -ESRCH;
                אם (r != ERANGE)
                        החזר -r;

                אם (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية passwd)))
                        החזר -ENOMEM;
                bufsize *= 2;
        }
#endif
}

صحيح getpwuid_malloc(uid_t uid, بنية passwd **ret) {
        אם (!uid_is_valid(uid))
                החזר -EINVAL;

#אם BUILD_STATIC
        החזר lookup_pwent_in_files(PASSWD_FILES, /* name= */ عدم, uid, ret);
#אחרת
        size_t bufsize = getpw_buffer_size();
        صحيح r;

        עבור (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية passwd)));
                buf = malloc0(ALIGN(حجم(بنية passwd)) + bufsize);
                אם (!buf)
                        החזר -ENOMEM;

                بنية passwd *pw = عدم;
                r = getpwuid_r(uid, buf, (حرف*) buf + ALIGN(حجم(بنية passwd)), bufsize, &pw);
                אם (r == 0) {
                        אם (pw) {
                                אם (ret)
                                        *ret = TAKE_PTR(buf);
                                החזר 0;
                        }

                        החזר -ESRCH;
                }

                أكد(r > 0);

                אם (errno_is_user_doesnt_exist(r))
                        החזר -ESRCH;
                אם (r != ERANGE)
                        החזר -r;

                אם (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية passwd)))
                        החזר -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ساكن صحيح copy_struct_group(ثابت بنية group *gr, بنية group **ret) {
        أكد(gr);
        أكد(ret);

        size_t need_bytes = حجم(بنية group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += حجم(حرف*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += حجم(حرف*);  /* NULL terminator for gr_mem */

        حرف *buf = malloc(need_bytes);
        אם (!buf)
                החזר -ENOMEM;

        بنية group *newgr = (بنية group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(حجم(بنية group) % alignof(حرف*) == 0);

        حرف *p = buf + حجم(بنية group) + (n_mem + 1) * حجم(حرف*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (حرف**) (buf + حجم(بنية group));
        עבור (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = عدم;

        *ret = newgr;
        החזר 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
صحيح lookup_grent_in_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                بنية group **ret) {

        صحيح r;

        أكد(files);
        أكد(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = عدم;
                بنية group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                אם (r == -ENOENT)
                        استمر;
                אם (r < 0)
                        החזר r;

                כלעוד ((r = fgetgrent_sane(f, &gr)) > 0) {
                        אם (name && !streq_ptr(gr->gr_name, name))
                                استمر;
                        אם (gid_is_valid(gid) && gr->gr_gid != gid)
                                استمر;
                        אם (ret)
                                החזר copy_struct_group(gr, ret);
                        החזר 0;
                }
                אם (r < 0)
                        החזר r;
        }

        החזר -ESRCH;
}

صحيح sysconf_ngroups_max(فراغ) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        אם (ngroups_max <= 0)
                החזר errno_or_else(EOPNOTSUPP);
        אם (ngroups_max > INT_MAX)
                החזר -ERANGE;
        החזר ngroups_max;
}

ssize_t lookup_groups_in_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = عدم;
        ssize_t n_arr = 0;
        صحيح r;

        أكد(files);
        أكد(name);
        أكد(gid_is_valid(gid));

        صحيح ngroups_max = sysconf_ngroups_max();
        אם (ngroups_max < 0)
                החזר ngroups_max;

        אם (!GREEDY_REALLOC(arr, n_arr + 1))
                החזר -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = عدم;
                بنية group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                אם (r == -ENOENT)
                        استمر;
                אם (r < 0)
                        החזר r;

                כלעוד ((r = fgetgrent_sane(f, &gr)) > 0) {
                        אם (gid == gr->gr_gid)
                                استمر;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        אם (gid_list_has(arr, n_arr, gr->gr_gid))
                                استمر;

                        אם (!strv_contains(gr->gr_mem, name))
                                استمر;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        אם (n_arr >= ngroups_max)
                                החזר -ENOBUFS;

                        אם (!GREEDY_REALLOC(arr, n_arr + 1))
                                החזר -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                אם (r < 0)
                        החזר r;
        }

        אם (ret)
                *ret = TAKE_PTR(arr);
        החזר n_arr;
}

صحيح getgrouplist_malloc(ثابت حرف *user, gid_t gid, gid_t **ret) {
#אם BUILD_STATIC
        החזר lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#אחרת
        صحيح ngroups_max = sysconf_ngroups_max();
        אם (ngroups_max < 0)
                החזר ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        אם (!gids)
                החזר -ENOMEM;

        صحيح k = ngroups_max;
        אם (getgrouplist(user, gid, gids, &k) < 0)
                החזר -EIO;  /* getgrouplist does not set errno */

        אם (ret)
                *ret = TAKE_PTR(gids);
        החזר k;
#endif
}

صحيح initgroups_wrapper(ثابت حرف *user, gid_t gid) {
#אם BUILD_STATIC
        _cleanup_free_ gid_t *groups = عدم;
        صحيح r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        אם (n_groups <= 0)
                החזר n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        עבור (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                אם (r != -EINVAL)
                        اكسر;
        }

        החזר r;
#אחרת
        החזר RET_NERRNO(initgroups(user, gid));
#endif
}

#אם !BUILD_STATIC

ساكن size_t getgr_buffer_size(فراغ) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        החזר bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

صحيح getgrnam_malloc(ثابت حرف *name, بنية group **ret) {
        אם (isempty(name))
                החזר -EINVAL;

#אם BUILD_STATIC
        החזר lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#אחרת
        size_t bufsize = getgr_buffer_size();
        صحيح r;

        עבור (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية group)));
                buf = malloc0(ALIGN(حجم(بنية group)) + bufsize);
                אם (!buf)
                        החזר -ENOMEM;

                بنية group *gr = عدم;
                r = getgrnam_r(name, buf, (حرف*) buf + ALIGN(حجم(بنية group)), bufsize, &gr);
                אם (r == 0) {
                        אם (gr) {
                                אם (ret)
                                        *ret = TAKE_PTR(buf);
                                החזר 0;
                        }

                        החזר -ESRCH;
                }

                أكد(r > 0);

                אם (errno_is_user_doesnt_exist(r))
                        החזר -ESRCH;
                אם (r != ERANGE)
                        החזר -r;

                אם (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية group)))
                        החזר -ENOMEM;
                bufsize *= 2;
        }
#endif
}

صحيح getgrgid_malloc(gid_t gid, بنية group **ret) {
        אם (!gid_is_valid(gid))
                החזר -EINVAL;

#אם BUILD_STATIC
        החזר lookup_grent_in_files(GROUP_FILES, /* name= */ عدم, gid, ret);
#אחרת
        size_t bufsize = getgr_buffer_size();
        صحيح r;

        עבור (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية group)));
                buf = malloc0(ALIGN(حجم(بنية group)) + bufsize);
                אם (!buf)
                        החזר -ENOMEM;

                بنية group *gr = عدم;
                r = getgrgid_r(gid, buf, (حرف*) buf + ALIGN(حجم(بنية group)), bufsize, &gr);
                אם (r == 0) {
                        אם (gr) {
                                אם (ret)
                                        *ret = TAKE_PTR(buf);
                                החזר 0;
                        }

                        החזר -ESRCH;
                }

                أكد(r > 0);

                אם (errno_is_user_doesnt_exist(r))
                        החזר -ESRCH;
                אם (r != ERANGE)
                        החזר -r;

                אם (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية group)))
                        החזר -ENOMEM;
                bufsize *= 2;
        }
#endif
}
