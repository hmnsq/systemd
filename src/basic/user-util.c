/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/file.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <utmpx.h>

#ಅನ್ತರ್ಭಾವ "sd-messages.h"

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "lock-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mkdir.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "terminal-util.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        ಧ್ರುವ ವರ್ಣ* strerror_##type(                                    \
                        ಪೂರ್ಣಾಙ್ಕ errnum,                                     \
                        ವರ್ಣ *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                ವಿಕಲ್ಪ (errnum) {                                       \
                ಪಕ್ಷ ESRCH:                                             \
                        ಪ್ರತ್ಯಾಗಮ "Unknown " STRINGIFY(type);              \
                ಪಕ್ಷ ENOEXEC:                                           \
                        ಪ್ರತ್ಯಾಗಮ "Not a system " STRINGIFY(type);         \
                ಸ್ವಭಾವ:                                                \
                        ಪ್ರತ್ಯಾಗಮ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ಯದಿ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ಪ್ರತ್ಯಾಗಮ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ಯದಿ (uid == (uid_t) UINT32_C(0xFFFF))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ parse_uid(ಧ್ರುವ ವರ್ಣ *s, uid_t *ret) {
        uint32_t uid = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        assert_cc(ಪರಿಮಾಣ(uid_t) == ಪರಿಮಾಣ(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (!uid_is_valid(uid))
                ಪ್ರತ್ಯಾಗಮ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ಯದಿ (ret)
                *ret = uid;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ parse_uid_range(ಧ್ರುವ ವರ್ಣ *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ ವರ್ಣ *word = ರಿಕ್ತ;
        uid_t l, u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret_lower);
        ಪ್ರತಿಜ್ಞಾ(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        r = parse_uid(word, &l);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Check for the upper bound and extract it if needed */
        ಯದಿ (!s)
                /* Single number with no dash. */
                u = l;
        ಅನ್ಯಥಾ ಯದಿ (!*s)
                /* Trailing dash is an error. */
                ಪ್ರತ್ಯಾಗಮ -EINVAL;
        ಅನ್ಯಥಾ {
                r = parse_uid(s, &u);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (l > u)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* getlogname_malloc(ಶೂನ್ಯ) {
        uid_t uid;
        ಸಂರಚನಾ stat st;

        ಯದಿ (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        ಅನ್ಯಥಾ
                uid = getuid();

        ಪ್ರತ್ಯಾಗಮ uid_to_name(uid);
}

ವರ್ಣ* getusername_malloc(ಶೂನ್ಯ) {
        ಧ್ರುವ ವರ್ಣ *e;

        e = secure_getenv("USER");
        ಯದಿ (e)
                ಪ್ರತ್ಯಾಗಮ strdup(e);

        ಪ್ರತ್ಯಾಗಮ uid_to_name(getuid());
}

bool is_nologin_shell(ಧ್ರುವ ವರ್ಣ *shell) {
        ಪ್ರತ್ಯಾಗಮ PATH_IN_SET(shell,
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

bool shell_is_placeholder(ಧ್ರುವ ವರ್ಣ *shell) {
        ಪ್ರತ್ಯಾಗಮ isempty(shell) || is_nologin_shell(shell);
}

ಧ್ರುವ ವರ್ಣ* default_ಬೇರು_shell_at(ಪೂರ್ಣಾಙ್ಕ rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        ಪ್ರತಿಜ್ಞಾ(rfd >= 0 || rfd == AT_FDCWD);

        ಪೂರ್ಣಾಙ್ಕ r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ರಿಕ್ತ, ರಿಕ್ತ);
        ಯದಿ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ DEFAULT_USER_SHELL;

        ಪ್ರತ್ಯಾಗಮ "/bin/sh";
}

ಧ್ರುವ ವರ್ಣ* default_ಬೇರು_shell(ಧ್ರುವ ವರ್ಣ *ಬೇರು) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ rfd = -EBADF;

        rfd = open(empty_to_ಬೇರು(ಬೇರು), O_CLOEXEC | O_DIRECTORY | O_PATH);
        ಯದಿ (rfd < 0)
                ಪ್ರತ್ಯಾಗಮ "/bin/sh";

        ಪ್ರತ್ಯಾಗಮ default_ಬೇರು_shell_at(rfd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ return_user_creds(
                ಧ್ರುವ ವರ್ಣ *username,
                uid_t uid, gid_t gid,
                ಧ್ರುವ ವರ್ಣ *home,
                ಧ್ರುವ ವರ್ಣ *shell,
                ವರ್ಣ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ವರ್ಣ **ret_home,
                ವರ್ಣ **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ ವರ್ಣ *s1 = ರಿಕ್ತ, *s2 = ರಿಕ್ತ, *s3 = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (ret_username) {
                r = strdup_to(&s1, username);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_home) {
                r = strdup_to(&s2, home);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_shell) {
                r = strdup_to(&s3, shell);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_username)
                *ret_username = TAKE_PTR(s1);
        ಯದಿ (ret_uid)
                *ret_uid = uid;
        ಯದಿ (ret_gid)
                *ret_gid = gid;
        ಯದಿ (ret_home)
                *ret_home = TAKE_PTR(s2);
        ಯದಿ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ synthesize_user_creds(
                ಧ್ರುವ ವರ್ಣ *username,
                UserCredsFlags flags,
                ವರ್ಣ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ವರ್ಣ **ret_home,
                ವರ್ಣ **ret_shell) {
        ಪ್ರತಿಜ್ಞಾ(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ಯದಿ (STR_IN_SET(username, "root", "0"))
                ಪ್ರತ್ಯಾಗಮ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_ಬೇರು_shell(ರಿಕ್ತ) : ರಿಕ್ತ,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ಯದಿ (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ಪ್ರತ್ಯಾಗಮ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ರಿಕ್ತ : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ರಿಕ್ತ : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ಪ್ರತ್ಯಾಗಮ -ENOMEDIUM;
}

ಪೂರ್ಣಾಙ್ಕ get_user_creds(
                ಧ್ರುವ ವರ್ಣ *username,
                UserCredsFlags flags,
                ವರ್ಣ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ವರ್ಣ **ret_home,
                ವರ್ಣ **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ಸಂರಚನಾ passwd *pw = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(username);
        ಪ್ರತಿಜ್ಞಾ((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ಯದಿ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಯದಿ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ಯದಿ (r >= 0)
                        username = pw->pw_name;

                ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ಯದಿ (ret_username)
                                *ret_username = ರಿಕ್ತ;
                        ಯದಿ (ret_uid)
                                *ret_uid = u;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        } ಅನ್ಯಥಾ
                r = getpwnam_malloc(username, &pw);

        ಯದಿ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ಯದಿ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_uid && !uid_is_valid(pw->pw_uid))
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (ret_gid && !gid_is_valid(pw->pw_gid))
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ಧ್ರುವ ವರ್ಣ *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_ಬೇರು(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? ರಿಕ್ತ : pw->pw_dir;

        ಧ್ರುವ ವರ್ಣ *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? ರಿಕ್ತ : pw->pw_shell;

        ಪ್ರತ್ಯಾಗಮ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ synthesize_group_creds(ಧ್ರುವ ವರ್ಣ *groupname, ವರ್ಣ **ret_name, gid_t *ret_gid) {
        ಪ್ರತಿಜ್ಞಾ(groupname);

        gid_t id;
        ಧ್ರುವ ವರ್ಣ *n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } ಅನ್ಯಥಾ ಯದಿ (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } ಅನ್ಯಥಾ
                ಪ್ರತ್ಯಾಗಮ -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (ret_gid)
                *ret_gid = id;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_group_creds(ಧ್ರುವ ವರ್ಣ *groupname, UserCredsFlags flags, ವರ್ಣ **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ಸಂರಚನಾ group *gr = ರಿಕ್ತ;
        gid_t id;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(groupname);

        ಯದಿ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಯದಿ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ಯದಿ (r >= 0)
                        groupname = gr->gr_name;
                ಅನ್ಯಥಾ ಯದಿ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        ಯದಿ (ret_gid)
                                *ret_gid = id;
                        ಯದಿ (ret_name)
                                *ret_name = ರಿಕ್ತ;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        } ಅನ್ಯಥಾ
                r = getgrnam_malloc(groupname, &gr);

        ಯದಿ (r < 0) {
                ಯದಿ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_gid && !gid_is_valid(gr->gr_gid))
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (ret_gid)
                *ret_gid = gr->gr_gid;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ* uid_to_name(uid_t uid) {
        ವರ್ಣ *ret;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Shortcut things to avoid NSS lookups */
        ಯದಿ (uid == 0)
                ಪ್ರತ್ಯಾಗಮ strdup("root");
        ಯದಿ (uid == UID_NOBODY && synthesize_nobody())
                ಪ್ರತ್ಯಾಗಮ strdup(NOBODY_USER_NAME);

        ಯದಿ (uid_is_valid(uid)) {
                _cleanup_free_ ಸಂರಚನಾ passwd *pw = ರಿಕ್ತ;

                r = getpwuid_malloc(uid, &pw);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ strdup(pw->pw_name);
        }

        ಯದಿ (asprintf(&ret, UID_FMT, uid) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ ret;
}

ವರ್ಣ* gid_to_name(gid_t gid) {
        ವರ್ಣ *ret;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (gid == 0)
                ಪ್ರತ್ಯಾಗಮ strdup("root");
        ಯದಿ (gid == GID_NOBODY && synthesize_nobody())
                ಪ್ರತ್ಯಾಗಮ strdup(NOBODY_GROUP_NAME);

        ಯದಿ (gid_is_valid(gid)) {
                _cleanup_free_ ಸಂರಚನಾ group *gr = ರಿಕ್ತ;

                r = getgrgid_malloc(gid, &gr);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ strdup(gr->gr_name);
        }

        ಯದಿ (asprintf(&ret, GID_FMT, gid) < 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ bool gid_list_has(ಧ್ರುವ gid_t *list, size_t size, gid_t val) {
        ಪ್ರತಿಜ್ಞಾ(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                ಯದಿ (*i == val)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ ngroups;

        ಯದಿ (getgid() == gid)
                ಪ್ರತ್ಯಾಗಮ 1;

        ಯದಿ (getegid() == gid)
                ಪ್ರತ್ಯಾಗಮ 1;

        ಯದಿ (!gid_is_valid(gid))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ಯದಿ (ngroups < 0)
                ಪ್ರತ್ಯಾಗಮ ngroups;

        ಪ್ರತ್ಯಾಗಮ gid_list_has(gids, ngroups, gid);
}

ಪೂರ್ಣಾಙ್ಕ in_group(ಧ್ರುವ ವರ್ಣ *name) {
        ಪೂರ್ಣಾಙ್ಕ r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ ರಿಕ್ತ, &gid);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ in_gid(gid);
}

ಪೂರ್ಣಾಙ್ಕ merge_gid_lists(ಧ್ರುವ gid_t *list1, size_t size1, ಧ್ರುವ gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        ಪ್ರತಿಜ್ಞಾ(size1 == 0 || list1);
        ಪ್ರತಿಜ್ಞಾ(size2 == 0 || list2);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (size2 > INT_MAX - size1)
                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        ಪ್ರತಿ (size_t i = 0; i < size1; i++)
                ಯದಿ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        ಪ್ರತಿ (size_t i = 0; i < size2; i++)
                ಯದಿ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)nresult;
}

ಪೂರ್ಣಾಙ್ಕ getgroups_alloc(gid_t **ret) {
        ಪೂರ್ಣಾಙ್ಕ ngroups = 8;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = ರಿಕ್ತ;

                p = new(gid_t, ngroups);
                ಯದಿ (!p)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ಯದಿ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ಪ್ರತ್ಯಾಗಮ ngroups;
                }
                ಯದಿ (ngroups == 0)
                        ಭಙ್ಗ;
                ಯದಿ (errno != EINVAL)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* Give up eventually */
                ಯದಿ (attempt++ > 10)
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ರಿಕ್ತ);
                ಯದಿ (ngroups < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (ngroups == 0)
                        ಭಙ್ಗ;
        }

        *ret = ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_home_dir(ವರ್ಣ **ret) {
        _cleanup_free_ ಸಂರಚನಾ passwd *p = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *e;
        uid_t u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ಯದಿ (e && path_is_valid(e) && path_is_absolute(e))
                ಗಚ್ಛ found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ಯದಿ (u == 0) {
                e = "/root";
                ಗಚ್ಛ found;
        }
        ಯದಿ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                ಗಚ್ಛ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        e = p->pw_dir;
        ಯದಿ (!path_is_valid(e) || !path_is_absolute(e))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

 found:
        ಪ್ರತ್ಯಾಗಮ path_simplify_alloc(e, ret);
}

ಪೂರ್ಣಾಙ್ಕ get_shell(ವರ್ಣ **ret) {
        _cleanup_free_ ಸಂರಚನಾ passwd *p = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *e;
        uid_t u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ಯದಿ (e && path_is_valid(e) && path_is_absolute(e))
                ಗಚ್ಛ found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ಯದಿ (u == 0) {
                e = default_ಬೇರು_shell(ರಿಕ್ತ);
                ಗಚ್ಛ found;
        }
        ಯದಿ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                ಗಚ್ಛ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        e = p->pw_shell;
        ಯದಿ (!path_is_valid(e) || !path_is_absolute(e))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

 found:
        ಪ್ರತ್ಯಾಗಮ path_simplify_alloc(e, ret);
}

ಪೂರ್ಣಾಙ್ಕ fully_set_uid_gid(uid_t uid, gid_t gid, ಧ್ರುವ gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (gid_is_valid(gid))
                ಯದಿ (setresgid(gid, gid, gid) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (uid_is_valid(uid))
                ಯದಿ (setresuid(uid, uid, uid) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ take_etc_passwd_lock(ಧ್ರುವ ವರ್ಣ *ಬೇರು) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ ವರ್ಣ *path = path_join(ಬೇರು, ETC_PASSWD_LOCK_PATH);
        ಯದಿ (!path)
                ಪ್ರತ್ಯಾಗಮ log_oom_debug();

        (ಶೂನ್ಯ) mkdir_parents(path, 0755);

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Locking %s failed: %m", path);

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

bool valid_user_group_name(ಧ್ರುವ ವರ್ಣ *u, ValidUserFlags flags) {
        ಧ್ರುವ ವರ್ಣ *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ಯದಿ (isempty(u)) /* An empty user name is never valid */
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (parse_uid(u, ರಿಕ್ತ) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ಪ್ರತ್ಯಾಗಮ FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        ಯದಿ (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                ಯದಿ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (string_has_cc(u, ರಿಕ್ತ)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ಪ್ರತ್ಯಾಗಮ false;

                ಯದಿ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ಪ್ರತ್ಯಾಗಮ false;

                /* Compare with strict result and warn if result doesn't match */
                ಯದಿ (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } ಅನ್ಯಥಾ {
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

                ಯದಿ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ಪ್ರತ್ಯಾಗಮ false;

                ಪ್ರತಿ (i = u+1; *i; i++)
                        ಯದಿ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ಪ್ರತ್ಯಾಗಮ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ಯದಿ (l > (size_t) sz) /* glibc: 256 */
                        ಪ್ರತ್ಯಾಗಮ false;
                ಯದಿ (l > NAME_MAX) /* must fit in a filename: 255 */
                        ಪ್ರತ್ಯಾಗಮ false;
                ಯದಿ (l > sizeof_field(ಸಂರಚನಾ utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

bool valid_gecos(ಧ್ರುವ ವರ್ಣ *d) {

        ಯದಿ (!d)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!utf8_is_valid(d))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (string_has_cc(d, ರಿಕ್ತ))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Colons are used as field separators, and hence not OK */
        ಯದಿ (strchr(d, ':'))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ವರ್ಣ* mangle_gecos(ಧ್ರುವ ವರ್ಣ *d) {
        ವರ್ಣ *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ಯದಿ (!mangled)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (ವರ್ಣ *i = mangled; *i; i++) {
                ಪೂರ್ಣಾಙ್ಕ len;

                ಯದಿ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        ಅನುವರ್ತ;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ಯದಿ (len < 0) {
                        *i = ' ';
                        ಅನುವರ್ತ;
                }

                i += len - 1;
        }

        ಪ್ರತ್ಯಾಗಮ mangled;
}

bool valid_home(ಧ್ರುವ ವರ್ಣ *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ಯದಿ (isempty(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!utf8_is_valid(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (string_has_cc(p, ರಿಕ್ತ))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!path_is_absolute(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!path_is_normalized(p))
                ಪ್ರತ್ಯಾಗಮ false;

        /* Colons are used as field separators, and hence not OK */
        ಯದಿ (strchr(p, ':'))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

bool valid_shell(ಧ್ರುವ ವರ್ಣ *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ಯದಿ (!valid_home(p))
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ಪೂರ್ಣಾಙ್ಕ maybe_setgroups(size_t size, ಧ್ರುವ gid_t *list) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ಯದಿ (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ಯದಿ (!f && errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (f) {
                        r = safe_fgetc(f, /* ret= */ ರಿಕ್ತ);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }
                }

                _cleanup_free_ ವರ್ಣ *setgroups_content = ರಿಕ್ತ;
                r = read_one_ಸಾಲು_file("/proc/self/setgroups", &setgroups_content);
                ಯದಿ (r < 0 && r != -ENOENT)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(ಶೂನ್ಯ) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cache = -1;

        ಯದಿ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ಪ್ರತ್ಯಾಗಮ cache;
}

ಪೂರ್ಣಾಙ್ಕ putpwent_sane(ಧ್ರುವ ಸಂರಚನಾ passwd *pw, FILE *stream) {
        ಪ್ರತಿಜ್ಞಾ(pw);
        ಪ್ರತಿಜ್ಞಾ(stream);

        errno = 0;
        ಯದಿ (putpwent(pw, stream) != 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ putspent_sane(ಧ್ರುವ ಸಂರಚನಾ spwd *sp, FILE *stream) {
        ಪ್ರತಿಜ್ಞಾ(sp);
        ಪ್ರತಿಜ್ಞಾ(stream);

        errno = 0;
        ಯದಿ (putspent(sp, stream) != 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ putgrent_sane(ಧ್ರುವ ಸಂರಚನಾ group *gr, FILE *stream) {
        ಪ್ರತಿಜ್ಞಾ(gr);
        ಪ್ರತಿಜ್ಞಾ(stream);

        errno = 0;
        ಯದಿ (putgrent(gr, stream) != 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

        ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ ENABLE_GSHADOW
ಪೂರ್ಣಾಙ್ಕ putsgent_sane(ಧ್ರುವ ಸಂರಚನಾ sgrp *sg, FILE *stream) {
        ಪ್ರತಿಜ್ಞಾ(sg);
        ಪ್ರತಿಜ್ಞಾ(stream);

        errno = 0;
        ಯದಿ (putsgent(sg, stream) != 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EIO);

        ಪ್ರತ್ಯಾಗಮ 0;
}
#endif

ಪೂರ್ಣಾಙ್ಕ fgetpwent_sane(FILE *stream, ಸಂರಚನಾ passwd **pw) {
        ಪ್ರತಿಜ್ಞಾ(stream);
        ಪ್ರತಿಜ್ಞಾ(pw);

        errno = 0;
        ಸಂರಚನಾ passwd *p = fgetpwent(stream);
        ಯದಿ (!p && !IN_SET(errno, 0, ENOENT))
                ಪ್ರತ್ಯಾಗಮ -errno;

        *pw = p;
        ಪ್ರತ್ಯಾಗಮ !!p;
}

ಪೂರ್ಣಾಙ್ಕ fgetspent_sane(FILE *stream, ಸಂರಚನಾ spwd **sp) {
        ಪ್ರತಿಜ್ಞಾ(stream);
        ಪ್ರತಿಜ್ಞಾ(sp);

        errno = 0;
        ಸಂರಚನಾ spwd *s = fgetspent(stream);
        ಯದಿ (!s && !IN_SET(errno, 0, ENOENT))
                ಪ್ರತ್ಯಾಗಮ -errno;

        *sp = s;
        ಪ್ರತ್ಯಾಗಮ !!s;
}

ಪೂರ್ಣಾಙ್ಕ fgetgrent_sane(FILE *stream, ಸಂರಚನಾ group **gr) {
        ಪ್ರತಿಜ್ಞಾ(stream);
        ಪ್ರತಿಜ್ಞಾ(gr);

        errno = 0;
        ಸಂರಚನಾ group *g = fgetgrent(stream);
        ಯದಿ (!g && !IN_SET(errno, 0, ENOENT))
                ಪ್ರತ್ಯಾಗಮ -errno;

        *gr = g;
        ಪ್ರತ್ಯಾಗಮ !!g;
}

#ಯದಿ ENABLE_GSHADOW
ಪೂರ್ಣಾಙ್ಕ fgetsgent_sane(FILE *stream, ಸಂರಚನಾ sgrp **sg) {
        ಪ್ರತಿಜ್ಞಾ(stream);
        ಪ್ರತಿಜ್ಞಾ(sg);

        errno = 0;
        ಸಂರಚನಾ sgrp *s = fgetsgent(stream);
        ಯದಿ (!s && !IN_SET(errno, 0, ENOENT))
                ಪ್ರತ್ಯಾಗಮ -errno;

        *sg = s;
        ಪ್ರತ್ಯಾಗಮ !!s;
}
#endif

ಪೂರ್ಣಾಙ್ಕ is_this_me(ಧ್ರುವ ವರ್ಣ *username) {
        uid_t uid;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ರಿಕ್ತ, &uid, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ uid == getuid();
}

ಧ್ರುವ ವರ್ಣ* get_home_ಬೇರು(ಶೂನ್ಯ) {
        /* For debug purposes allow overriding where we look for home dirs */
        ಧ್ರುವ ವರ್ಣ *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ಯದಿ (e && path_is_absolute(e) && path_is_normalized(e))
                ಪ್ರತ್ಯಾಗಮ e;

        ಪ್ರತ್ಯಾಗಮ "/home";
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ copy_struct_passwd(ಧ್ರುವ ಸಂರಚನಾ passwd *pw, ಸಂರಚನಾ passwd **ret) {
        ಪ್ರತಿಜ್ಞಾ(pw);
        ಪ್ರತಿಜ್ಞಾ(ret);

        size_t need_bytes = ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        ವರ್ಣ *buf = malloc(need_bytes);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಸಂರಚನಾ passwd *newpw = (ಸಂರಚನಾ passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        ವರ್ಣ *p = buf + ಪರಿಮಾಣ(ಸಂರಚನಾ passwd);

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
        ಪ್ರತ್ಯಾಗಮ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ಪೂರ್ಣಾಙ್ಕ lookup_pwent_in_files(
                ವರ್ಣ * ಧ್ರುವ *files,
                ಧ್ರುವ ವರ್ಣ *name,
                uid_t uid,
                ಸಂರಚನಾ passwd **ret) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
                ಸಂರಚನಾ passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ಯದಿ (r == -ENOENT)
                        ಅನುವರ್ತ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯಾವತ್ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ಯದಿ (name && !streq_ptr(pw->pw_name, name))
                                ಅನುವರ್ತ;
                        ಯದಿ (uid_is_valid(uid) && pw->pw_uid != uid)
                                ಅನುವರ್ತ;
                        ಯದಿ (ret)
                                ಪ್ರತ್ಯಾಗಮ copy_struct_passwd(pw, ret);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ -ESRCH;
}

#ಯದಿ !BUILD_STATIC

ಸ್ಥಿರ size_t getpw_buffer_size(ಶೂನ್ಯ) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ಪ್ರತ್ಯಾಗಮ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ಸ್ಥಿರ bool errno_is_user_doesnt_exist(ಪೂರ್ಣಾಙ್ಕ error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ಪ್ರತ್ಯಾಗಮ IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

ಪೂರ್ಣಾಙ್ಕ getpwnam_malloc(ಧ್ರುವ ವರ್ಣ *name, ಸಂರಚನಾ passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ಯದಿ (isempty(name))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

#ಯದಿ BUILD_STATIC
        ಪ್ರತ್ಯಾಗಮ lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#ಅನ್ಯಥಾ
        size_t bufsize = getpw_buffer_size();
        ಪೂರ್ಣಾಙ್ಕ r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)));
                buf = malloc0(ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)) + bufsize);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಸಂರಚನಾ passwd *pw = ರಿಕ್ತ;
                r = getpwnam_r(name, buf, (ವರ್ಣ*) buf + ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)), bufsize, &pw);
                ಯದಿ (r == 0) {
                        ಯದಿ (pw) {
                                ಯದಿ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ಯದಿ (errno_is_user_doesnt_exist(r))
                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                ಯದಿ (r != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -r;

                ಯದಿ (bufsize > SIZE_MAX/2 - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ಪೂರ್ಣಾಙ್ಕ getpwuid_malloc(uid_t uid, ಸಂರಚನಾ passwd **ret) {
        ಯದಿ (!uid_is_valid(uid))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

#ಯದಿ BUILD_STATIC
        ಪ್ರತ್ಯಾಗಮ lookup_pwent_in_files(PASSWD_FILES, /* name= */ ರಿಕ್ತ, uid, ret);
#ಅನ್ಯಥಾ
        size_t bufsize = getpw_buffer_size();
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)));
                buf = malloc0(ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)) + bufsize);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಸಂರಚನಾ passwd *pw = ರಿಕ್ತ;
                r = getpwuid_r(uid, buf, (ವರ್ಣ*) buf + ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)), bufsize, &pw);
                ಯದಿ (r == 0) {
                        ಯದಿ (pw) {
                                ಯದಿ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                ಯದಿ (errno_is_user_doesnt_exist(r))
                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                ಯದಿ (r != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -r;

                ಯದಿ (bufsize > SIZE_MAX/2 - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ passwd)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ copy_struct_group(ಧ್ರುವ ಸಂರಚನಾ group *gr, ಸಂರಚನಾ group **ret) {
        ಪ್ರತಿಜ್ಞಾ(gr);
        ಪ್ರತಿಜ್ಞಾ(ret);

        size_t need_bytes = ಪರಿಮಾಣ(ಸಂರಚನಾ group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += ಪರಿಮಾಣ(ವರ್ಣ*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += ಪರಿಮಾಣ(ವರ್ಣ*);  /* NULL terminator for gr_mem */

        ವರ್ಣ *buf = malloc(need_bytes);
        ಯದಿ (!buf)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಸಂರಚನಾ group *newgr = (ಸಂರಚನಾ group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(ಪರಿಮಾಣ(ಸಂರಚನಾ group) % alignof(ವರ್ಣ*) == 0);

        ವರ್ಣ *p = buf + ಪರಿಮಾಣ(ಸಂರಚನಾ group) + (n_mem + 1) * ಪರಿಮಾಣ(ವರ್ಣ*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (ವರ್ಣ**) (buf + ಪರಿಮಾಣ(ಸಂರಚನಾ group));
        ಪ್ರತಿ (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ರಿಕ್ತ;

        *ret = newgr;
        ಪ್ರತ್ಯಾಗಮ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ಪೂರ್ಣಾಙ್ಕ lookup_grent_in_files(
                ವರ್ಣ * ಧ್ರುವ *files,
                ಧ್ರುವ ವರ್ಣ *name,
                gid_t gid,
                ಸಂರಚನಾ group **ret) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
                ಸಂರಚನಾ group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ಯದಿ (r == -ENOENT)
                        ಅನುವರ್ತ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯಾವತ್ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ಯದಿ (name && !streq_ptr(gr->gr_name, name))
                                ಅನುವರ್ತ;
                        ಯದಿ (gid_is_valid(gid) && gr->gr_gid != gid)
                                ಅನುವರ್ತ;
                        ಯದಿ (ret)
                                ಪ್ರತ್ಯಾಗಮ copy_struct_group(gr, ret);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ -ESRCH;
}

ಪೂರ್ಣಾಙ್ಕ sysconf_ngroups_max(ಶೂನ್ಯ) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ಯದಿ (ngroups_max <= 0)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(EOPNOTSUPP);
        ಯದಿ (ngroups_max > INT_MAX)
                ಪ್ರತ್ಯಾಗಮ -ERANGE;
        ಪ್ರತ್ಯಾಗಮ ngroups_max;
}

ssize_t lookup_groups_in_files(
                ವರ್ಣ * ಧ್ರುವ *files,
                ಧ್ರುವ ವರ್ಣ *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ರಿಕ್ತ;
        ssize_t n_arr = 0;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(gid_is_valid(gid));

        ಪೂರ್ಣಾಙ್ಕ ngroups_max = sysconf_ngroups_max();
        ಯದಿ (ngroups_max < 0)
                ಪ್ರತ್ಯಾಗಮ ngroups_max;

        ಯದಿ (!GREEDY_REALLOC(arr, n_arr + 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
                ಸಂರಚನಾ group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ಯದಿ (r == -ENOENT)
                        ಅನುವರ್ತ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯಾವತ್ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ಯದಿ (gid == gr->gr_gid)
                                ಅನುವರ್ತ;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ಯದಿ (gid_list_has(arr, n_arr, gr->gr_gid))
                                ಅನುವರ್ತ;

                        ಯದಿ (!strv_contains(gr->gr_mem, name))
                                ಅನುವರ್ತ;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ಯದಿ (n_arr >= ngroups_max)
                                ಪ್ರತ್ಯಾಗಮ -ENOBUFS;

                        ಯದಿ (!GREEDY_REALLOC(arr, n_arr + 1))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret)
                *ret = TAKE_PTR(arr);
        ಪ್ರತ್ಯಾಗಮ n_arr;
}

ಪೂರ್ಣಾಙ್ಕ getgrouplist_malloc(ಧ್ರುವ ವರ್ಣ *user, gid_t gid, gid_t **ret) {
#ಯದಿ BUILD_STATIC
        ಪ್ರತ್ಯಾಗಮ lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#ಅನ್ಯಥಾ
        ಪೂರ್ಣಾಙ್ಕ ngroups_max = sysconf_ngroups_max();
        ಯದಿ (ngroups_max < 0)
                ಪ್ರತ್ಯಾಗಮ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ಯದಿ (!gids)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪೂರ್ಣಾಙ್ಕ k = ngroups_max;
        ಯದಿ (getgrouplist(user, gid, gids, &k) < 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;  /* getgrouplist does not set errno */

        ಯದಿ (ret)
                *ret = TAKE_PTR(gids);
        ಪ್ರತ್ಯಾಗಮ k;
#endif
}

ಪೂರ್ಣಾಙ್ಕ initgroups_wrapper(ಧ್ರುವ ವರ್ಣ *user, gid_t gid) {
#ಯದಿ BUILD_STATIC
        _cleanup_free_ gid_t *groups = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ಯದಿ (n_groups <= 0)
                ಪ್ರತ್ಯಾಗಮ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        ಪ್ರತಿ (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ಯದಿ (r != -EINVAL)
                        ಭಙ್ಗ;
        }

        ಪ್ರತ್ಯಾಗಮ r;
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(initgroups(user, gid));
#endif
}

#ಯದಿ !BUILD_STATIC

ಸ್ಥಿರ size_t getgr_buffer_size(ಶೂನ್ಯ) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ಪ್ರತ್ಯಾಗಮ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

ಪೂರ್ಣಾಙ್ಕ getgrnam_malloc(ಧ್ರುವ ವರ್ಣ *name, ಸಂರಚನಾ group **ret) {
        ಯದಿ (isempty(name))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

#ಯದಿ BUILD_STATIC
        ಪ್ರತ್ಯಾಗಮ lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#ಅನ್ಯಥಾ
        size_t bufsize = getgr_buffer_size();
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)));
                buf = malloc0(ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)) + bufsize);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಸಂರಚನಾ group *gr = ರಿಕ್ತ;
                r = getgrnam_r(name, buf, (ವರ್ಣ*) buf + ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)), bufsize, &gr);
                ಯದಿ (r == 0) {
                        ಯದಿ (gr) {
                                ಯದಿ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                ಯದಿ (errno_is_user_doesnt_exist(r))
                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                ಯದಿ (r != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -r;

                ಯದಿ (bufsize > SIZE_MAX/2 - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ಪೂರ್ಣಾಙ್ಕ getgrgid_malloc(gid_t gid, ಸಂರಚನಾ group **ret) {
        ಯದಿ (!gid_is_valid(gid))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

#ಯದಿ BUILD_STATIC
        ಪ್ರತ್ಯಾಗಮ lookup_grent_in_files(GROUP_FILES, /* name= */ ರಿಕ್ತ, gid, ret);
#ಅನ್ಯಥಾ
        size_t bufsize = getgr_buffer_size();
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ರಿಕ್ತ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)));
                buf = malloc0(ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)) + bufsize);
                ಯದಿ (!buf)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಸಂರಚನಾ group *gr = ರಿಕ್ತ;
                r = getgrgid_r(gid, buf, (ವರ್ಣ*) buf + ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)), bufsize, &gr);
                ಯದಿ (r == 0) {
                        ಯದಿ (gr) {
                                ಯದಿ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಪ್ರತ್ಯಾಗಮ 0;
                        }

                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                ಯದಿ (errno_is_user_doesnt_exist(r))
                        ಪ್ರತ್ಯಾಗಮ -ESRCH;
                ಯದಿ (r != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -r;

                ಯದಿ (bufsize > SIZE_MAX/2 - ALIGN(ಪರಿಮಾಣ(ಸಂರಚನಾ group)))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                bufsize *= 2;
        }
#endif
}
