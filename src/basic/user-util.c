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

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_STRERROR_ACCOUNT(type)                                   \
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* strerror_##type(                                    \
                        ಪೂರ್ಣಾಂಕ errnum,                                     \
                        ಅಕ್ಷರ *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                ಆಯ್ಕೆ (errnum) {                                       \
                ಸ್ಥಿತಿ ESRCH:                                             \
                        ಹಿಂತಿರುಗಿ "Unknown " STRINGIFY(type);              \
                ಸ್ಥಿತಿ ENOEXEC:                                           \
                        ಹಿಂತಿರುಗಿ "Not a system " STRINGIFY(type);         \
                ಪೂರ್ವನಿಯೋಜಿತ:                                                \
                        ಹಿಂತಿರುಗಿ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ಆದರೆ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ಹಿಂತಿರುಗಿ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ಆದರೆ (uid == (uid_t) UINT32_C(0xFFFF))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ parse_uid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uid_t *ret) {
        uint32_t uid = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        assert_cc(ಗಾತ್ರ(uid_t) == ಗಾತ್ರ(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = ಸುರಕ್ಷಿತ_atou32_ತುಂಬಿದ(s, 10
                             | ಸುರಕ್ಷಿತ_ATO_REFUSE_PLUS_MINUS
                             | ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_ZERO
                             | ಸುರಕ್ಷಿತ_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (!uid_is_valid(uid))
                ಹಿಂತಿರುಗಿ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ಆದರೆ (ret)
                *ret = uid;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ parse_uid_range(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, uid_t *ret_ಅಧೋ, uid_t *ret_ಊರ್ಧ್ವ) {
        _cleanup_free_ ಅಕ್ಷರ *ಶಬ್ದ = ಖಾಲಿ;
        uid_t l, u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ret_ಅಧೋ);
        ಪ್ರತಿಜ್ಞಾ(ret_ಊರ್ಧ್ವ);

        r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&s, &ಶಬ್ದ, "-", ಹೊರತೆಗೆ_DONT_COALESCE_SEPARATORS);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EINVAL;

        r = parse_uid(ಶಬ್ದ, &l);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Check for the upper bound and extract it if needed */
        ಆದರೆ (!s)
                /* Single number with no dash. */
                u = l;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!*s)
                /* Trailing dash is an error. */
                ಹಿಂತಿರುಗಿ -EINVAL;
        ಇಲ್ಲದಿದ್ದರೆ {
                r = parse_uid(s, &u);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (l > u)
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        *ret_ಅಧೋ = l;
        *ret_ಊರ್ಧ್ವ = u;
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* getlogname_malloc(ಶೂನ್ಯ) {
        uid_t uid;
        ರಚನೆ stat st;

        ಆದರೆ (isatty_ಸುರಕ್ಷಿತ(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        ಇಲ್ಲದಿದ್ದರೆ
                uid = getuid();

        ಹಿಂತಿರುಗಿ uid_ಗಮ್ಯ_name(uid);
}

ಅಕ್ಷರ* getusername_malloc(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        e = secure_getenv("USER");
        ಆದರೆ (e)
                ಹಿಂತಿರುಗಿ strdup(e);

        ಹಿಂತಿರುಗಿ uid_ಗಮ್ಯ_name(getuid());
}

bool is_nologin_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell) {
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_ಒಳ_ಗಣ(shell,
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

bool shell_is_placeholder(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell) {
        ಹಿಂತಿರುಗಿ isempty(shell) || is_nologin_shell(shell);
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_ಬೇರು_shell_at(ಪೂರ್ಣಾಂಕ rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        ಪ್ರತಿಜ್ಞಾ(rfd >= 0 || rfd == AT_FDCWD);

        ಪೂರ್ಣಾಂಕ r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ಖಾಲಿ, ಖಾಲಿ);
        ಆದರೆ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ DEFAULT_USER_SHELL;

        ಹಿಂತಿರುಗಿ "/bin/sh";
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* default_ಬೇರು_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ rfd = -EBADF;

        rfd = open(ಖಾಲಿಗಳು_ಗಮ್ಯ_ಬೇರು(ಬೇರು), O_CLOEXEC | O_DIRECTORY | O_ಮಾರ್ಗ);
        ಆದರೆ (rfd < 0)
                ಹಿಂತಿರುಗಿ "/bin/sh";

        ಹಿಂತಿರುಗಿ default_ಬೇರು_shell_at(rfd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ return_user_creds(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username,
                uid_t uid, gid_t gid,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *home,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *shell,
                ಅಕ್ಷರ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ಅಕ್ಷರ **ret_home,
                ಅಕ್ಷರ **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ ಅಕ್ಷರ *s1 = ಖಾಲಿ, *s2 = ಖಾಲಿ, *s3 = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (ret_username) {
                r = strdup_ಗಮ್ಯ(&s1, username);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_home) {
                r = strdup_ಗಮ್ಯ(&s2, home);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_shell) {
                r = strdup_ಗಮ್ಯ(&s3, shell);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_username)
                *ret_username = TAKE_PTR(s1);
        ಆದರೆ (ret_uid)
                *ret_uid = uid;
        ಆದರೆ (ret_gid)
                *ret_gid = gid;
        ಆದರೆ (ret_home)
                *ret_home = TAKE_PTR(s2);
        ಆದರೆ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ synthesize_user_creds(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username,
                UserCredsFlags flags,
                ಅಕ್ಷರ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ಅಕ್ಷರ **ret_home,
                ಅಕ್ಷರ **ret_shell) {
        ಪ್ರತಿಜ್ಞಾ(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ಆದರೆ (STR_ಒಳ_ಗಣ(username, "root", "0"))
                ಹಿಂತಿರುಗಿ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_ಬೇರು_shell(ಖಾಲಿ) : ಖಾಲಿ,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ಆದರೆ (STR_ಒಳ_ಗಣ(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ಹಿಂತಿರುಗಿ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_ಗಣ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ಖಾಲಿ : "/",
                                         FLAGS_ಗಣ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ಖಾಲಿ : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ಹಿಂತಿರುಗಿ -ENOMEDIUM;
}

ಪೂರ್ಣಾಂಕ get_user_creds(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username,
                UserCredsFlags flags,
                ಅಕ್ಷರ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ಅಕ್ಷರ **ret_home,
                ಅಕ್ಷರ **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ರಚನೆ passwd *pw = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(username);
        ಪ್ರತಿಜ್ಞಾ((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ಆದರೆ (!FLAGS_ಗಣ(flags, USER_CREDS_PREFER_NSS) ||
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
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ 0;
                ಆದರೆ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ಆದರೆ (r >= 0)
                        username = pw->pw_name;

                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ಆದರೆ (ret_username)
                                *ret_username = ಖಾಲಿ;
                        ಆದರೆ (ret_uid)
                                *ret_uid = u;
                        ಹಿಂತಿರುಗಿ 0;
                }
        } ಇಲ್ಲದಿದ್ದರೆ
                r = getpwnam_malloc(username, &pw);

        ಆದರೆ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ಆದರೆ (FLAGS_ಗಣ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ಹಿಂತಿರುಗಿ 0;

                ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_uid && !uid_is_valid(pw->pw_uid))
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (ret_gid && !gid_is_valid(pw->pw_gid))
                ಹಿಂತಿರುಗಿ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *h =
                (FLAGS_ಗಣ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && ಖಾಲಿಗಳು_or_ಬೇರು(pw->pw_dir)) ||
                (FLAGS_ಗಣ(flags, USER_CREDS_CLEAN) && (!ಮಾರ್ಗ_is_valid(pw->pw_dir) || !ಮಾರ್ಗ_is_absolute(pw->pw_dir)))
                ? ಖಾಲಿ : pw->pw_dir;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s =
                (FLAGS_ಗಣ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_ಗಣ(flags, USER_CREDS_CLEAN) && (!ಮಾರ್ಗ_is_valid(pw->pw_shell) || !ಮಾರ್ಗ_is_absolute(pw->pw_shell)))
                ? ಖಾಲಿ : pw->pw_shell;

        ಹಿಂತಿರುಗಿ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ synthesize_ಗುಂಪು_creds(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *groupname, ಅಕ್ಷರ **ret_name, gid_t *ret_gid) {
        ಪ್ರತಿಜ್ಞಾ(groupname);

        gid_t id;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (STR_ಒಳ_ಗಣ(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STR_ಒಳ_ಗಣ(groupname, NOBODY_ಗುಂಪು_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_ಗುಂಪು_NAME;
        } ಇಲ್ಲದಿದ್ದರೆ
                ಹಿಂತಿರುಗಿ -ENOMEDIUM;

        r = strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret_name, n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (ret_gid)
                *ret_gid = id;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_ಗುಂಪು_creds(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *groupname, UserCredsFlags flags, ಅಕ್ಷರ **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ರಚನೆ group *gr = ಖಾಲಿ;
        gid_t id;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(groupname);

        ಆದರೆ (!FLAGS_ಗಣ(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_ಗುಂಪು_creds(groupname, ret_name, ret_gid);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ 0;
                ಆದರೆ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ಆದರೆ (r >= 0)
                        groupname = gr->gr_name;
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FLAGS_ಗಣ(flags, USER_CREDS_ALLOW_MISSING)) {
                        ಆದರೆ (ret_gid)
                                *ret_gid = id;
                        ಆದರೆ (ret_name)
                                *ret_name = ಖಾಲಿ;
                        ಹಿಂತಿರುಗಿ 0;
                }
        } ಇಲ್ಲದಿದ್ದರೆ
                r = getgrnam_malloc(groupname, &gr);

        ಆದರೆ (r < 0) {
                ಆದರೆ (FLAGS_ಗಣ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_ಗುಂಪು_creds(groupname, ret_name, ret_gid) >= 0)
                        ಹಿಂತಿರುಗಿ 0;
                ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_gid && !gid_is_valid(gr->gr_gid))
                ಹಿಂತಿರುಗಿ -EBADMSG;

        r = strdup_ಗಮ್ಯ_ತುಂಬಿದ(ret_name, groupname);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (ret_gid)
                *ret_gid = gr->gr_gid;
        ಹಿಂತಿರುಗಿ 0;
}

ಅಕ್ಷರ* uid_ಗಮ್ಯ_name(uid_t uid) {
        ಅಕ್ಷರ *ret;
        ಪೂರ್ಣಾಂಕ r;

        /* Shortcut things to avoid NSS lookups */
        ಆದರೆ (uid == 0)
                ಹಿಂತಿರುಗಿ strdup("root");
        ಆದರೆ (uid == UID_NOBODY && synthesize_nobody())
                ಹಿಂತಿರುಗಿ strdup(NOBODY_USER_NAME);

        ಆದರೆ (uid_is_valid(uid)) {
                _cleanup_free_ ರಚನೆ passwd *pw = ಖಾಲಿ;

                r = getpwuid_malloc(uid, &pw);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ strdup(pw->pw_name);
        }

        ಆದರೆ (asprintf(&ret, UID_FMT, uid) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ret;
}

ಅಕ್ಷರ* gid_ಗಮ್ಯ_name(gid_t gid) {
        ಅಕ್ಷರ *ret;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (gid == 0)
                ಹಿಂತಿರುಗಿ strdup("root");
        ಆದರೆ (gid == GID_NOBODY && synthesize_nobody())
                ಹಿಂತಿರುಗಿ strdup(NOBODY_ಗುಂಪು_NAME);

        ಆದರೆ (gid_is_valid(gid)) {
                _cleanup_free_ ರಚನೆ group *gr = ಖಾಲಿ;

                r = getgrgid_malloc(gid, &gr);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ strdup(gr->gr_name);
        }

        ಆದರೆ (asprintf(&ret, GID_FMT, gid) < 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ bool gid_list_has(ಸ್ಥಿರಾಂಕ gid_t *list, size_t ಗಾತ್ರ_2, gid_t val) {
        ಪ್ರತಿಜ್ಞಾ(list || ಗಾತ್ರ_2 == 0);

        FOREACH_ARRAY(i, list, ಗಾತ್ರ_2)
                ಆದರೆ (*i == val)
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ ಒಳ_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ ngroups;

        ಆದರೆ (getgid() == gid)
                ಹಿಂತಿರುಗಿ 1;

        ಆದರೆ (getegid() == gid)
                ಹಿಂತಿರುಗಿ 1;

        ಆದರೆ (!gid_is_valid(gid))
                ಹಿಂತಿರುಗಿ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ಆದರೆ (ngroups < 0)
                ಹಿಂತಿರುಗಿ ngroups;

        ಹಿಂತಿರುಗಿ gid_list_has(gids, ngroups, gid);
}

ಪೂರ್ಣಾಂಕ ಒಳ_ಗುಂಪು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        ಪೂರ್ಣಾಂಕ r;
        gid_t gid;

        r = get_ಗುಂಪು_creds(name, /* flags= */ 0, /* ret_name= */ ಖಾಲಿ, &gid);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಒಳ_gid(gid);
}

ಪೂರ್ಣಾಂಕ ವಿಲೀನ_gid_lists(ಸ್ಥಿರಾಂಕ gid_t *list1, size_t size1, ಸ್ಥಿರಾಂಕ gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        ಪ್ರತಿಜ್ಞಾ(size1 == 0 || list1);
        ಪ್ರತಿಜ್ಞಾ(size2 == 0 || list2);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (size2 > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ - size1)
                ಹಿಂತಿರುಗಿ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        ಗಾಗಿ (size_t i = 0; i < size1; i++)
                ಆದರೆ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        ಗಾಗಿ (size_t i = 0; i < size2; i++)
                ಆದರೆ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)nresult;
}

ಪೂರ್ಣಾಂಕ getgroups_alloc(gid_t **ret) {
        ಪೂರ್ಣಾಂಕ ngroups = 8;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ attempt = 0;;) {
                _cleanup_free_ gid_t *p = ಖಾಲಿ;

                p = new(gid_t, ngroups);
                ಆದರೆ (!p)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ಆದರೆ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ಹಿಂತಿರುಗಿ ngroups;
                }
                ಆದರೆ (ngroups == 0)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (errno != EINVAL)
                        ಹಿಂತಿರುಗಿ -errno;

                /* Give up eventually */
                ಆದರೆ (attempt++ > 10)
                        ಹಿಂತಿರುಗಿ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ಖಾಲಿ);
                ಆದರೆ (ngroups < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (ngroups == 0)
                        ನಿಲ್ಲಿಸು;
        }

        *ret = ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_home_dir(ಅಕ್ಷರ **ret) {
        _cleanup_free_ ರಚನೆ passwd *p = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        uid_t u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ಆದರೆ (e && ಮಾರ್ಗ_is_valid(e) && ಮಾರ್ಗ_is_absolute(e))
                ಹೋಗು ಸಿಕ್ಕಿತು;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ಆದರೆ (u == 0) {
                e = "/root";
                ಹೋಗು ಸಿಕ್ಕಿತು;
        }
        ಆದರೆ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                ಹೋಗು ಸಿಕ್ಕಿತು;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        e = p->pw_dir;
        ಆದರೆ (!ಮಾರ್ಗ_is_valid(e) || !ಮಾರ್ಗ_is_absolute(e))
                ಹಿಂತಿರುಗಿ -EINVAL;

 ಸಿಕ್ಕಿತು:
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_simplify_alloc(e, ret);
}

ಪೂರ್ಣಾಂಕ get_shell(ಅಕ್ಷರ **ret) {
        _cleanup_free_ ರಚನೆ passwd *p = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        uid_t u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ಆದರೆ (e && ಮಾರ್ಗ_is_valid(e) && ಮಾರ್ಗ_is_absolute(e))
                ಹೋಗು ಸಿಕ್ಕಿತು;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ಆದರೆ (u == 0) {
                e = default_ಬೇರು_shell(ಖಾಲಿ);
                ಹೋಗು ಸಿಕ್ಕಿತು;
        }
        ಆದರೆ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                ಹೋಗು ಸಿಕ್ಕಿತು;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        e = p->pw_shell;
        ಆದರೆ (!ಮಾರ್ಗ_is_valid(e) || !ಮಾರ್ಗ_is_absolute(e))
                ಹಿಂತಿರುಗಿ -EINVAL;

 ಸಿಕ್ಕಿತು:
        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_simplify_alloc(e, ret);
}

ಪೂರ್ಣಾಂಕ fully_ಗಣ_uid_gid(uid_t uid, gid_t gid, ಸ್ಥಿರಾಂಕ gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (gid_is_valid(gid))
                ಆದರೆ (setresgid(gid, gid, gid) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (uid_is_valid(uid))
                ಆದರೆ (setresuid(uid, uid, uid) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ take_etc_passwd_ಬೀಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು) {
        ಪೂರ್ಣಾಂಕ r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಮಾರ್ಗ_join(ಬೇರು, ETC_PASSWD_ಬೀಗ_ಮಾರ್ಗ);
        ಆದರೆ (!ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ log_oom_debug();

        (ಶೂನ್ಯ) mkdir_parents(ಮಾರ್ಗ, 0755);

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = open(ಮಾರ್ಗ, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Cannot open %s: %m", ಮಾರ್ಗ);

        r = unposix_ಬೀಗ(fd, LOCK_EX);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Locking %s failed: %m", ಮಾರ್ಗ);

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

bool valid_user_ಗುಂಪು_name(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *u, ValidUserFlags flags) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ಆದರೆ (isempty(u)) /* An empty user name is never valid */
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (parse_uid(u, ಖಾಲಿ) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(flags, VALID_USER_ALLOW_NUMERIC);

        ಆದರೆ (FLAGS_ಗಣ(flags, VALID_USER_RELAX)) {

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

                ಆದರೆ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (string_has_cc(u, ಖಾಲಿ)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (ಒಳ_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (u[0] == '-' && ಒಳ_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ಹಿಂತಿರುಗಿ false;

                ಆದರೆ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ಹಿಂತಿರುಗಿ false;

                /* Compare with strict result and warn if result doesn't match */
                ಆದರೆ (FLAGS_ಗಣ(flags, VALID_USER_WARN) && !valid_user_ಗುಂಪು_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_ಸಂದೇಶ("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ಅಂಶ("USER_GROUP_NAME=%s", u),
                                   LOG_ಸಂದೇಶ_ID(SD_ಸಂದೇಶ_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } ಇಲ್ಲದಿದ್ದರೆ {
                ದೀರ್ಘ sz;
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

                ಆದರೆ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ಹಿಂತಿರುಗಿ false;

                ಗಾಗಿ (i = u+1; *i; i++)
                        ಆದರೆ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !ಒಳ_ಗಣ(*i, '_', '-'))
                                ಹಿಂತಿರುಗಿ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ಆದರೆ (l > (size_t) sz) /* glibc: 256 */
                        ಹಿಂತಿರುಗಿ false;
                ಆದರೆ (l > NAME_MAX) /* must fit in a filename: 255 */
                        ಹಿಂತಿರುಗಿ false;
                ಆದರೆ (l > sizeof_ಕ್ಷೇತ್ರ(ರಚನೆ utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ true;
}

bool valid_gecos(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *d) {

        ಆದರೆ (!d)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!utf8_is_valid(d))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (string_has_cc(d, ಖಾಲಿ))
                ಹಿಂತಿರುಗಿ false;

        /* Colons are used as field separators, and hence not OK */
        ಆದರೆ (strchr(d, ':'))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಅಕ್ಷರ* mangle_gecos(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *d) {
        ಅಕ್ಷರ *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ಆದರೆ (!mangled)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (ಅಕ್ಷರ *i = mangled; *i; i++) {
                ಪೂರ್ಣಾಂಕ len;

                ಆದರೆ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        ಮುಂದುವರಿ;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ಆದರೆ (len < 0) {
                        *i = ' ';
                        ಮುಂದುವರಿ;
                }

                i += len - 1;
        }

        ಹಿಂತಿರುಗಿ mangled;
}

bool valid_home(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ಆದರೆ (isempty(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!utf8_is_valid(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (string_has_cc(p, ಖಾಲಿ))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!ಮಾರ್ಗ_is_absolute(p))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!ಮಾರ್ಗ_is_normalized(p))
                ಹಿಂತಿರುಗಿ false;

        /* Colons are used as field separators, and hence not OK */
        ಆದರೆ (strchr(p, ':'))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

bool valid_shell(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ಆದರೆ (!valid_home(p))
                ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ಪೂರ್ಣಾಂಕ maybe_setgroups(size_t ಗಾತ್ರ_2, ಸ್ಥಿರಾಂಕ gid_t *list) {
        ಪೂರ್ಣಾಂಕ r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ಆದರೆ (ಗಾತ್ರ_2 == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ಆದರೆ (!f && errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (f) {
                        r = ಸುರಕ್ಷಿತ_fgetc(f, /* ret= */ ಖಾಲಿ);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ಹಿಂತಿರುಗಿ 0;
                        }
                }

                _cleanup_free_ ಅಕ್ಷರ *setgroups_content = ಖಾಲಿ;
                r = read_one_ಸಾಲು_file("/proc/self/setgroups", &setgroups_content);
                ಆದರೆ (r < 0 && r != -ENOENT)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ಹಿಂತಿರುಗಿ 0;
                }
        }

        ಹಿಂತಿರುಗಿ RET_NERRNO(setgroups(ಗಾತ್ರ_2, list));
}

bool synthesize_nobody(ಶೂನ್ಯ) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cache = -1;

        ಆದರೆ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ಹಿಂತಿರುಗಿ cache;
}

ಪೂರ್ಣಾಂಕ putpwent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ passwd *pw, FILE *ಹರಿವು) {
        ಪ್ರತಿಜ್ಞಾ(pw);
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);

        errno = 0;
        ಆದರೆ (putpwent(pw, ಹರಿವು) != 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ putspent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ spwd *sp, FILE *ಹರಿವು) {
        ಪ್ರತಿಜ್ಞಾ(sp);
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);

        errno = 0;
        ಆದರೆ (putspent(sp, ಹರಿವು) != 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ putgrent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ group *gr, FILE *ಹರಿವು) {
        ಪ್ರತಿಜ್ಞಾ(gr);
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);

        errno = 0;
        ಆದರೆ (putgrent(gr, ಹರಿವು) != 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

        ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ ENABLE_GSHADOW
ಪೂರ್ಣಾಂಕ putsgent_sane(ಸ್ಥಿರಾಂಕ ರಚನೆ sgrp *sg, FILE *ಹರಿವು) {
        ಪ್ರತಿಜ್ಞಾ(sg);
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);

        errno = 0;
        ಆದರೆ (putsgent(sg, ಹರಿವು) != 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EIO);

        ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ fgetpwent_sane(FILE *ಹರಿವು, ರಚನೆ passwd **pw) {
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);
        ಪ್ರತಿಜ್ಞಾ(pw);

        errno = 0;
        ರಚನೆ passwd *p = fgetpwent(ಹರಿವು);
        ಆದರೆ (!p && !ಒಳ_ಗಣ(errno, 0, ENOENT))
                ಹಿಂತಿರುಗಿ -errno;

        *pw = p;
        ಹಿಂತಿರುಗಿ !!p;
}

ಪೂರ್ಣಾಂಕ fgetspent_sane(FILE *ಹರಿವು, ರಚನೆ spwd **sp) {
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);
        ಪ್ರತಿಜ್ಞಾ(sp);

        errno = 0;
        ರಚನೆ spwd *s = fgetspent(ಹರಿವು);
        ಆದರೆ (!s && !ಒಳ_ಗಣ(errno, 0, ENOENT))
                ಹಿಂತಿರುಗಿ -errno;

        *sp = s;
        ಹಿಂತಿರುಗಿ !!s;
}

ಪೂರ್ಣಾಂಕ fgetgrent_sane(FILE *ಹರಿವು, ರಚನೆ group **gr) {
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);
        ಪ್ರತಿಜ್ಞಾ(gr);

        errno = 0;
        ರಚನೆ group *g = fgetgrent(ಹರಿವು);
        ಆದರೆ (!g && !ಒಳ_ಗಣ(errno, 0, ENOENT))
                ಹಿಂತಿರುಗಿ -errno;

        *gr = g;
        ಹಿಂತಿರುಗಿ !!g;
}

#ಆದರೆ ENABLE_GSHADOW
ಪೂರ್ಣಾಂಕ fgetsgent_sane(FILE *ಹರಿವು, ರಚನೆ sgrp **sg) {
        ಪ್ರತಿಜ್ಞಾ(ಹರಿವು);
        ಪ್ರತಿಜ್ಞಾ(sg);

        errno = 0;
        ರಚನೆ sgrp *s = fgetsgent(ಹರಿವು);
        ಆದರೆ (!s && !ಒಳ_ಗಣ(errno, 0, ENOENT))
                ಹಿಂತಿರುಗಿ -errno;

        *sg = s;
        ಹಿಂತಿರುಗಿ !!s;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ is_this_me(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *username) {
        uid_t uid;
        ಪೂರ್ಣಾಂಕ r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ಖಾಲಿ, &uid, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ uid == getuid();
}

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* get_home_ಬೇರು(ಶೂನ್ಯ) {
        /* For debug purposes allow overriding where we look for home dirs */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ಆದರೆ (e && ಮಾರ್ಗ_is_absolute(e) && ಮಾರ್ಗ_is_normalized(e))
                ಹಿಂತಿರುಗಿ e;

        ಹಿಂತಿರುಗಿ "/home";
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ copy_struct_passwd(ಸ್ಥಿರಾಂಕ ರಚನೆ passwd *pw, ರಚನೆ passwd **ret) {
        ಪ್ರತಿಜ್ಞಾ(pw);
        ಪ್ರತಿಜ್ಞಾ(ret);

        size_t need_ಬೈಟುಗಳು = ಗಾತ್ರ(ರಚನೆ passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        ಅಕ್ಷರ *buf = ಸ್ಮರಣೆ_ಕೊಡು(need_ಬೈಟುಗಳು);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ರಚನೆ passwd *newpw = (ರಚನೆ passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        ಅಕ್ಷರ *p = buf + ಗಾತ್ರ(ರಚನೆ passwd);

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
        ಹಿಂತಿರುಗಿ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ಪೂರ್ಣಾಂಕ lookup_pwent_ಒಳ_files(
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *files,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                uid_t uid,
                ರಚನೆ passwd **ret) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ಖಾಲಿ;
                ರಚನೆ passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ಆದರೆ (r == -ENOENT)
                        ಮುಂದುವರಿ;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆಗ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ಆದರೆ (name && !streq_ptr(pw->pw_name, name))
                                ಮುಂದುವರಿ;
                        ಆದರೆ (uid_is_valid(uid) && pw->pw_uid != uid)
                                ಮುಂದುವರಿ;
                        ಆದರೆ (ret)
                                ಹಿಂತಿರುಗಿ copy_struct_passwd(pw, ret);
                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ -ESRCH;
}

#ಆದರೆ !BUILD_STATIC

ಸ್ಥಿರ size_t getpw_ಬಫರ್_ಗಾತ್ರ(ಶೂನ್ಯ) {
        ದೀರ್ಘ bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ಹಿಂತಿರುಗಿ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ಸ್ಥಿರ bool errno_is_user_doesnt_exist(ಪೂರ್ಣಾಂಕ error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ಹಿಂತಿರುಗಿ ಒಳ_ಗಣ(ನಿರಪೇಕ್ಷ(error), ENOENT, ESRCH, EBADF, EPERM);
}

#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ getpwnam_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ರಚನೆ passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ಆದರೆ (isempty(name))
                ಹಿಂತಿರುಗಿ -EINVAL;

#ಆದರೆ BUILD_STATIC
        ಹಿಂತಿರುಗಿ lookup_pwent_ಒಳ_files(PASSWD_FILES, name, UID_INVALID, ret);
#ಇಲ್ಲದಿದ್ದರೆ
        size_t bufsize = getpw_ಬಫರ್_ಗಾತ್ರ();
        ಪೂರ್ಣಾಂಕ r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)));
                buf = malloc0(ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)) + bufsize);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ರಚನೆ passwd *pw = ಖಾಲಿ;
                r = getpwnam_r(name, buf, (ಅಕ್ಷರ*) buf + ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)), bufsize, &pw);
                ಆದರೆ (r == 0) {
                        ಆದರೆ (pw) {
                                ಆದರೆ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಹಿಂತಿರುಗಿ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ಆದರೆ (errno_is_user_doesnt_exist(r))
                        ಹಿಂತಿರುಗಿ -ESRCH;
                ಆದರೆ (r != ERANGE)
                        ಹಿಂತಿರುಗಿ -r;

                ಆದರೆ (bufsize > SIZE_MAX/2 - ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                bufsize *= 2;
        }
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ getpwuid_malloc(uid_t uid, ರಚನೆ passwd **ret) {
        ಆದರೆ (!uid_is_valid(uid))
                ಹಿಂತಿರುಗಿ -EINVAL;

#ಆದರೆ BUILD_STATIC
        ಹಿಂತಿರುಗಿ lookup_pwent_ಒಳ_files(PASSWD_FILES, /* name= */ ಖಾಲಿ, uid, ret);
#ಇಲ್ಲದಿದ್ದರೆ
        size_t bufsize = getpw_ಬಫರ್_ಗಾತ್ರ();
        ಪೂರ್ಣಾಂಕ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)));
                buf = malloc0(ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)) + bufsize);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ರಚನೆ passwd *pw = ಖಾಲಿ;
                r = getpwuid_r(uid, buf, (ಅಕ್ಷರ*) buf + ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)), bufsize, &pw);
                ಆದರೆ (r == 0) {
                        ಆದರೆ (pw) {
                                ಆದರೆ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಹಿಂತಿರುಗಿ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                ಆದರೆ (errno_is_user_doesnt_exist(r))
                        ಹಿಂತಿರುಗಿ -ESRCH;
                ಆದರೆ (r != ERANGE)
                        ಹಿಂತಿರುಗಿ -r;

                ಆದರೆ (bufsize > SIZE_MAX/2 - ALIGN(ಗಾತ್ರ(ರಚನೆ passwd)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                bufsize *= 2;
        }
#ಆದರೆ_ಅಂತ್ಯ
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ copy_struct_ಗುಂಪು(ಸ್ಥಿರಾಂಕ ರಚನೆ group *gr, ರಚನೆ group **ret) {
        ಪ್ರತಿಜ್ಞಾ(gr);
        ಪ್ರತಿಜ್ಞಾ(ret);

        size_t need_ಬೈಟುಗಳು = ಗಾತ್ರ(ರಚನೆ group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_ಬೈಟುಗಳು += ಗಾತ್ರ(ಅಕ್ಷರ*) + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(*s) + 1;
                n_mem++;
        }
        need_ಬೈಟುಗಳು += ಗಾತ್ರ(ಅಕ್ಷರ*);  /* NULL terminator for gr_mem */

        ಅಕ್ಷರ *buf = ಸ್ಮರಣೆ_ಕೊಡು(need_ಬೈಟುಗಳು);
        ಆದರೆ (!buf)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ರಚನೆ group *newgr = (ರಚನೆ group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(ಗಾತ್ರ(ರಚನೆ group) % alignof(ಅಕ್ಷರ*) == 0);

        ಅಕ್ಷರ *p = buf + ಗಾತ್ರ(ರಚನೆ group) + (n_mem + 1) * ಗಾತ್ರ(ಅಕ್ಷರ*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (ಅಕ್ಷರ**) (buf + ಗಾತ್ರ(ರಚನೆ group));
        ಗಾಗಿ (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ಖಾಲಿ;

        *ret = newgr;
        ಹಿಂತಿರುಗಿ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ಪೂರ್ಣಾಂಕ lookup_grent_ಒಳ_files(
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *files,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                gid_t gid,
                ರಚನೆ group **ret) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ಖಾಲಿ;
                ರಚನೆ group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ಆದರೆ (r == -ENOENT)
                        ಮುಂದುವರಿ;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆಗ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ಆದರೆ (name && !streq_ptr(gr->gr_name, name))
                                ಮುಂದುವರಿ;
                        ಆದರೆ (gid_is_valid(gid) && gr->gr_gid != gid)
                                ಮುಂದುವರಿ;
                        ಆದರೆ (ret)
                                ಹಿಂತಿರುಗಿ copy_struct_ಗುಂಪು(gr, ret);
                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ -ESRCH;
}

ಪೂರ್ಣಾಂಕ sysconf_ngroups_max(ಶೂನ್ಯ) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        ದೀರ್ಘ ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ಆದರೆ (ngroups_max <= 0)
                ಹಿಂತಿರುಗಿ errno_or_else(EOPNOTSUPP);
        ಆದರೆ (ngroups_max > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ)
                ಹಿಂತಿರುಗಿ -ERANGE;
        ಹಿಂತಿರುಗಿ ngroups_max;
}

ssize_t lookup_groups_ಒಳ_files(
                ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *files,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ಖಾಲಿ;
        ssize_t n_arr = 0;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(files);
        ಪ್ರತಿಜ್ಞಾ(name);
        ಪ್ರತಿಜ್ಞಾ(gid_is_valid(gid));

        ಪೂರ್ಣಾಂಕ ngroups_max = sysconf_ngroups_max();
        ಆದರೆ (ngroups_max < 0)
                ಹಿಂತಿರುಗಿ ngroups_max;

        ಆದರೆ (!GREEDY_REALLOC(arr, n_arr + 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ಖಾಲಿ;
                ರಚನೆ group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ಆದರೆ (r == -ENOENT)
                        ಮುಂದುವರಿ;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆಗ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ಆದರೆ (gid == gr->gr_gid)
                                ಮುಂದುವರಿ;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ಆದರೆ (gid_list_has(arr, n_arr, gr->gr_gid))
                                ಮುಂದುವರಿ;

                        ಆದರೆ (!strv_ಒಳಗೊಂಡಿದೆ(gr->gr_mem, name))
                                ಮುಂದುವರಿ;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ಆದರೆ (n_arr >= ngroups_max)
                                ಹಿಂತಿರುಗಿ -ENOBUFS;

                        ಆದರೆ (!GREEDY_REALLOC(arr, n_arr + 1))
                                ಹಿಂತಿರುಗಿ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret)
                *ret = TAKE_PTR(arr);
        ಹಿಂತಿರುಗಿ n_arr;
}

ಪೂರ್ಣಾಂಕ getgrouplist_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, gid_t gid, gid_t **ret) {
#ಆದರೆ BUILD_STATIC
        ಹಿಂತಿರುಗಿ lookup_groups_ಒಳ_files(ಗುಂಪು_FILES, user, gid, ret);
#ಇಲ್ಲದಿದ್ದರೆ
        ಪೂರ್ಣಾಂಕ ngroups_max = sysconf_ngroups_max();
        ಆದರೆ (ngroups_max < 0)
                ಹಿಂತಿರುಗಿ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ಆದರೆ (!gids)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಪೂರ್ಣಾಂಕ k = ngroups_max;
        ಆದರೆ (getgrouplist(user, gid, gids, &k) < 0)
                ಹಿಂತಿರುಗಿ -EIO;  /* getgrouplist does not set errno */

        ಆದರೆ (ret)
                *ret = TAKE_PTR(gids);
        ಹಿಂತಿರುಗಿ k;
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ initgroups_wrapper(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *user, gid_t gid) {
#ಆದರೆ BUILD_STATIC
        _cleanup_free_ gid_t *groups = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ಆದರೆ (n_groups <= 0)
                ಹಿಂತಿರುಗಿ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        ಗಾಗಿ (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ಆದರೆ (r != -EINVAL)
                        ನಿಲ್ಲಿಸು;
        }

        ಹಿಂತಿರುಗಿ r;
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ RET_NERRNO(initgroups(user, gid));
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ !BUILD_STATIC

ಸ್ಥಿರ size_t getgr_ಬಫರ್_ಗಾತ್ರ(ಶೂನ್ಯ) {
        ದೀರ್ಘ bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ಹಿಂತಿರುಗಿ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ getgrnam_malloc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ರಚನೆ group **ret) {
        ಆದರೆ (isempty(name))
                ಹಿಂತಿರುಗಿ -EINVAL;

#ಆದರೆ BUILD_STATIC
        ಹಿಂತಿರುಗಿ lookup_grent_ಒಳ_files(ಗುಂಪು_FILES, name, GID_INVALID, ret);
#ಇಲ್ಲದಿದ್ದರೆ
        size_t bufsize = getgr_ಬಫರ್_ಗಾತ್ರ();
        ಪೂರ್ಣಾಂಕ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಗಾತ್ರ(ರಚನೆ group)));
                buf = malloc0(ALIGN(ಗಾತ್ರ(ರಚನೆ group)) + bufsize);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ರಚನೆ group *gr = ಖಾಲಿ;
                r = getgrnam_r(name, buf, (ಅಕ್ಷರ*) buf + ALIGN(ಗಾತ್ರ(ರಚನೆ group)), bufsize, &gr);
                ಆದರೆ (r == 0) {
                        ಆದರೆ (gr) {
                                ಆದರೆ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಹಿಂತಿರುಗಿ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                ಆದರೆ (errno_is_user_doesnt_exist(r))
                        ಹಿಂತಿರುಗಿ -ESRCH;
                ಆದರೆ (r != ERANGE)
                        ಹಿಂತಿರುಗಿ -r;

                ಆದರೆ (bufsize > SIZE_MAX/2 - ALIGN(ಗಾತ್ರ(ರಚನೆ group)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                bufsize *= 2;
        }
#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ getgrgid_malloc(gid_t gid, ರಚನೆ group **ret) {
        ಆದರೆ (!gid_is_valid(gid))
                ಹಿಂತಿರುಗಿ -EINVAL;

#ಆದರೆ BUILD_STATIC
        ಹಿಂತಿರುಗಿ lookup_grent_ಒಳ_files(ಗುಂಪು_FILES, /* name= */ ಖಾಲಿ, gid, ret);
#ಇಲ್ಲದಿದ್ದರೆ
        size_t bufsize = getgr_ಬಫರ್_ಗಾತ್ರ();
        ಪೂರ್ಣಾಂಕ r;

        ಗಾಗಿ (;;) {
                _cleanup_free_ ಶೂನ್ಯ *buf = ಖಾಲಿ;

                /* Silence static analyzers */
                ಪ್ರತಿಜ್ಞಾ(bufsize <= SIZE_MAX - ALIGN(ಗಾತ್ರ(ರಚನೆ group)));
                buf = malloc0(ALIGN(ಗಾತ್ರ(ರಚನೆ group)) + bufsize);
                ಆದರೆ (!buf)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ರಚನೆ group *gr = ಖಾಲಿ;
                r = getgrgid_r(gid, buf, (ಅಕ್ಷರ*) buf + ALIGN(ಗಾತ್ರ(ರಚನೆ group)), bufsize, &gr);
                ಆದರೆ (r == 0) {
                        ಆದರೆ (gr) {
                                ಆದರೆ (ret)
                                        *ret = TAKE_PTR(buf);
                                ಹಿಂತಿರುಗಿ 0;
                        }

                        ಹಿಂತಿರುಗಿ -ESRCH;
                }

                ಪ್ರತಿಜ್ಞಾ(r > 0);

                ಆದರೆ (errno_is_user_doesnt_exist(r))
                        ಹಿಂತಿರುಗಿ -ESRCH;
                ಆದರೆ (r != ERANGE)
                        ಹಿಂತಿರುಗಿ -r;

                ಆದರೆ (bufsize > SIZE_MAX/2 - ALIGN(ಗಾತ್ರ(ರಚನೆ group)))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                bufsize *= 2;
        }
#ಆದರೆ_ಅಂತ್ಯ
}
