/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/file.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <utmpx.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "sd-messages.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "lock-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mkdir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "terminal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_STRERROR_ACCOUNT(type)                                   \
        ਅਚਲ ਅੱਖਰ* strerror_##type(                                    \
                        ਪੂਰਨ errnum,                                     \
                        ਅੱਖਰ *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                ਚੋਣ (errnum) {                                       \
                ਹਾਲਤ ESRCH:                                             \
                        ਵਾਪਸ "Unknown " STRINGIFY(type);              \
                ਹਾਲਤ ENOEXEC:                                           \
                        ਵਾਪਸ "Not a system " STRINGIFY(type);         \
                ਮੂਲ:                                                \
                        ਵਾਪਸ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ਜੇ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ਵਾਪਸ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ਜੇ (uid == (uid_t) UINT32_C(0xFFFF))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ parse_uid(ਅਚਲ ਅੱਖਰ *s, uid_t *ret) {
        uint32_t uid = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        assert_cc(ਆਕਾਰ(uid_t) == ਆਕਾਰ(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = ਸੁਰੱਖਿਅਤ_atou32_ਭਰਿਆ(s, 10
                             | ਸੁਰੱਖਿਅਤ_ATO_REFUSE_PLUS_MINUS
                             | ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_ZERO
                             | ਸੁਰੱਖਿਅਤ_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (!uid_is_valid(uid))
                ਵਾਪਸ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ਜੇ (ret)
                *ret = uid;

        ਵਾਪਸ 0;
}

ਪੂਰਨ parse_uid_range(ਅਚਲ ਅੱਖਰ *s, uid_t *ret_ਹੇਠਲਾ, uid_t *ret_ਉੱਤਲਾ) {
        _cleanup_free_ ਅੱਖਰ *ਸ਼ਬਦ = ਨਿਰਾ;
        uid_t l, u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਹੇਠਲਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਉੱਤਲਾ);

        r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&s, &ਸ਼ਬਦ, "-", ਕੱਢੋ_DONT_COALESCE_SEPARATORS);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EINVAL;

        r = parse_uid(ਸ਼ਬਦ, &l);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Check for the upper bound and extract it if needed */
        ਜੇ (!s)
                /* Single number with no dash. */
                u = l;
        ਨਹੀਂਤਾਂ ਜੇ (!*s)
                /* Trailing dash is an error. */
                ਵਾਪਸ -EINVAL;
        ਨਹੀਂਤਾਂ {
                r = parse_uid(s, &u);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (l > u)
                        ਵਾਪਸ -EINVAL;
        }

        *ret_ਹੇਠਲਾ = l;
        *ret_ਉੱਤਲਾ = u;
        ਵਾਪਸ 0;
}

ਅੱਖਰ* getlogname_malloc(ਖਾਲੀ) {
        uid_t uid;
        ਬਣਤਰ stat st;

        ਜੇ (isatty_ਸੁਰੱਖਿਅਤ(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        ਨਹੀਂਤਾਂ
                uid = getuid();

        ਵਾਪਸ uid_ਮੰਜ਼ਿਲ_name(uid);
}

ਅੱਖਰ* getusername_malloc(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *e;

        e = secure_getenv("USER");
        ਜੇ (e)
                ਵਾਪਸ strdup(e);

        ਵਾਪਸ uid_ਮੰਜ਼ਿਲ_name(getuid());
}

bool is_nologin_shell(ਅਚਲ ਅੱਖਰ *shell) {
        ਵਾਪਸ ਰਸਤਾ_ਵਿੱਚ_ਸੈੱਟ(shell,
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

bool shell_is_placeholder(ਅਚਲ ਅੱਖਰ *shell) {
        ਵਾਪਸ isempty(shell) || is_nologin_shell(shell);
}

ਅਚਲ ਅੱਖਰ* default_ਜੜ੍ਹ_shell_at(ਪੂਰਨ rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        ਪ੍ਰਤਿਜ੍ਞਾ(rfd >= 0 || rfd == AT_FDCWD);

        ਪੂਰਨ r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ਨਿਰਾ, ਨਿਰਾ);
        ਜੇ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ਜੇ (r > 0)
                ਵਾਪਸ DEFAULT_USER_SHELL;

        ਵਾਪਸ "/bin/sh";
}

ਅਚਲ ਅੱਖਰ* default_ਜੜ੍ਹ_shell(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ) {
        _cleanup_close_ ਪੂਰਨ rfd = -EBADF;

        rfd = open(ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਜੜ੍ਹ), O_CLOEXEC | O_DIRECTORY | O_ਰਸਤਾ);
        ਜੇ (rfd < 0)
                ਵਾਪਸ "/bin/sh";

        ਵਾਪਸ default_ਜੜ੍ਹ_shell_at(rfd);
}

ਸਥਿਰ ਪੂਰਨ return_user_creds(
                ਅਚਲ ਅੱਖਰ *username,
                uid_t uid, gid_t gid,
                ਅਚਲ ਅੱਖਰ *home,
                ਅਚਲ ਅੱਖਰ *shell,
                ਅੱਖਰ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ਅੱਖਰ **ret_home,
                ਅੱਖਰ **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ ਅੱਖਰ *s1 = ਨਿਰਾ, *s2 = ਨਿਰਾ, *s3 = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਜੇ (ret_username) {
                r = strdup_ਮੰਜ਼ਿਲ(&s1, username);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_home) {
                r = strdup_ਮੰਜ਼ਿਲ(&s2, home);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_shell) {
                r = strdup_ਮੰਜ਼ਿਲ(&s3, shell);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_username)
                *ret_username = TAKE_PTR(s1);
        ਜੇ (ret_uid)
                *ret_uid = uid;
        ਜੇ (ret_gid)
                *ret_gid = gid;
        ਜੇ (ret_home)
                *ret_home = TAKE_PTR(s2);
        ਜੇ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ synthesize_user_creds(
                ਅਚਲ ਅੱਖਰ *username,
                UserCredsFlags flags,
                ਅੱਖਰ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ਅੱਖਰ **ret_home,
                ਅੱਖਰ **ret_shell) {
        ਪ੍ਰਤਿਜ੍ਞਾ(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(username, "root", "0"))
                ਵਾਪਸ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_ਜੜ੍ਹ_shell(ਨਿਰਾ) : ਨਿਰਾ,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ਵਾਪਸ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_ਸੈੱਟ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ਨਿਰਾ : "/",
                                         FLAGS_ਸੈੱਟ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ਨਿਰਾ : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ਵਾਪਸ -ENOMEDIUM;
}

ਪੂਰਨ get_user_creds(
                ਅਚਲ ਅੱਖਰ *username,
                UserCredsFlags flags,
                ਅੱਖਰ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ਅੱਖਰ **ret_home,
                ਅੱਖਰ **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ਬਣਤਰ passwd *pw = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(username);
        ਪ੍ਰਤਿਜ੍ਞਾ((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, USER_CREDS_PREFER_NSS) ||
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
                ਜੇ (r >= 0)
                        ਵਾਪਸ 0;
                ਜੇ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ਵਾਪਸ r;
        }

        ਜੇ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ਜੇ (r >= 0)
                        username = pw->pw_name;

                ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ਜੇ (ret_username)
                                *ret_username = ਨਿਰਾ;
                        ਜੇ (ret_uid)
                                *ret_uid = u;
                        ਵਾਪਸ 0;
                }
        } ਨਹੀਂਤਾਂ
                r = getpwnam_malloc(username, &pw);

        ਜੇ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ਜੇ (FLAGS_ਸੈੱਟ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ਵਾਪਸ 0;

                ਵਾਪਸ r;
        }

        ਜੇ (ret_uid && !uid_is_valid(pw->pw_uid))
                ਵਾਪਸ -EBADMSG;

        ਜੇ (ret_gid && !gid_is_valid(pw->pw_gid))
                ਵਾਪਸ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ਅਚਲ ਅੱਖਰ *h =
                (FLAGS_ਸੈੱਟ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(pw->pw_dir)) ||
                (FLAGS_ਸੈੱਟ(flags, USER_CREDS_CLEAN) && (!ਰਸਤਾ_is_valid(pw->pw_dir) || !ਰਸਤਾ_is_absolute(pw->pw_dir)))
                ? ਨਿਰਾ : pw->pw_dir;

        ਅਚਲ ਅੱਖਰ *s =
                (FLAGS_ਸੈੱਟ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_ਸੈੱਟ(flags, USER_CREDS_CLEAN) && (!ਰਸਤਾ_is_valid(pw->pw_shell) || !ਰਸਤਾ_is_absolute(pw->pw_shell)))
                ? ਨਿਰਾ : pw->pw_shell;

        ਵਾਪਸ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ਸਥਿਰ ਪੂਰਨ synthesize_ਸਮੂਹ_creds(ਅਚਲ ਅੱਖਰ *groupname, ਅੱਖਰ **ret_name, gid_t *ret_gid) {
        ਪ੍ਰਤਿਜ੍ਞਾ(groupname);

        gid_t id;
        ਅਚਲ ਅੱਖਰ *n;
        ਪੂਰਨ r;

        ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } ਨਹੀਂਤਾਂ ਜੇ (STR_ਵਿੱਚ_ਸੈੱਟ(groupname, NOBODY_ਸਮੂਹ_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_ਸਮੂਹ_NAME;
        } ਨਹੀਂਤਾਂ
                ਵਾਪਸ -ENOMEDIUM;

        r = strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret_name, n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (ret_gid)
                *ret_gid = id;
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_ਸਮੂਹ_creds(ਅਚਲ ਅੱਖਰ *groupname, UserCredsFlags flags, ਅੱਖਰ **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ਬਣਤਰ group *gr = ਨਿਰਾ;
        gid_t id;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(groupname);

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_ਸਮੂਹ_creds(groupname, ret_name, ret_gid);
                ਜੇ (r >= 0)
                        ਵਾਪਸ 0;
                ਜੇ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ਵਾਪਸ r;
        }

        ਜੇ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ਜੇ (r >= 0)
                        groupname = gr->gr_name;
                ਨਹੀਂਤਾਂ ਜੇ (FLAGS_ਸੈੱਟ(flags, USER_CREDS_ALLOW_MISSING)) {
                        ਜੇ (ret_gid)
                                *ret_gid = id;
                        ਜੇ (ret_name)
                                *ret_name = ਨਿਰਾ;
                        ਵਾਪਸ 0;
                }
        } ਨਹੀਂਤਾਂ
                r = getgrnam_malloc(groupname, &gr);

        ਜੇ (r < 0) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_ਸਮੂਹ_creds(groupname, ret_name, ret_gid) >= 0)
                        ਵਾਪਸ 0;
                ਵਾਪਸ r;
        }

        ਜੇ (ret_gid && !gid_is_valid(gr->gr_gid))
                ਵਾਪਸ -EBADMSG;

        r = strdup_ਮੰਜ਼ਿਲ_ਭਰਿਆ(ret_name, groupname);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (ret_gid)
                *ret_gid = gr->gr_gid;
        ਵਾਪਸ 0;
}

ਅੱਖਰ* uid_ਮੰਜ਼ਿਲ_name(uid_t uid) {
        ਅੱਖਰ *ret;
        ਪੂਰਨ r;

        /* Shortcut things to avoid NSS lookups */
        ਜੇ (uid == 0)
                ਵਾਪਸ strdup("root");
        ਜੇ (uid == UID_NOBODY && synthesize_nobody())
                ਵਾਪਸ strdup(NOBODY_USER_NAME);

        ਜੇ (uid_is_valid(uid)) {
                _cleanup_free_ ਬਣਤਰ passwd *pw = ਨਿਰਾ;

                r = getpwuid_malloc(uid, &pw);
                ਜੇ (r >= 0)
                        ਵਾਪਸ strdup(pw->pw_name);
        }

        ਜੇ (asprintf(&ret, UID_FMT, uid) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ret;
}

ਅੱਖਰ* gid_ਮੰਜ਼ਿਲ_name(gid_t gid) {
        ਅੱਖਰ *ret;
        ਪੂਰਨ r;

        ਜੇ (gid == 0)
                ਵਾਪਸ strdup("root");
        ਜੇ (gid == GID_NOBODY && synthesize_nobody())
                ਵਾਪਸ strdup(NOBODY_ਸਮੂਹ_NAME);

        ਜੇ (gid_is_valid(gid)) {
                _cleanup_free_ ਬਣਤਰ group *gr = ਨਿਰਾ;

                r = getgrgid_malloc(gid, &gr);
                ਜੇ (r >= 0)
                        ਵਾਪਸ strdup(gr->gr_name);
        }

        ਜੇ (asprintf(&ret, GID_FMT, gid) < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ ret;
}

ਸਥਿਰ bool gid_list_has(ਅਚਲ gid_t *list, size_t ਮਾਪ, gid_t val) {
        ਪ੍ਰਤਿਜ੍ਞਾ(list || ਮਾਪ == 0);

        FOREACH_ARRAY(i, list, ਮਾਪ)
                ਜੇ (*i == val)
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

ਪੂਰਨ ਵਿੱਚ_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ਨਿਰਾ;
        ਪੂਰਨ ngroups;

        ਜੇ (getgid() == gid)
                ਵਾਪਸ 1;

        ਜੇ (getegid() == gid)
                ਵਾਪਸ 1;

        ਜੇ (!gid_is_valid(gid))
                ਵਾਪਸ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ਜੇ (ngroups < 0)
                ਵਾਪਸ ngroups;

        ਵਾਪਸ gid_list_has(gids, ngroups, gid);
}

ਪੂਰਨ ਵਿੱਚ_ਸਮੂਹ(ਅਚਲ ਅੱਖਰ *name) {
        ਪੂਰਨ r;
        gid_t gid;

        r = get_ਸਮੂਹ_creds(name, /* flags= */ 0, /* ret_name= */ ਨਿਰਾ, &gid);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਵਿੱਚ_gid(gid);
}

ਪੂਰਨ ਮਿਲਾਓ_gid_lists(ਅਚਲ gid_t *list1, size_t size1, ਅਚਲ gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(size1 == 0 || list1);
        ਪ੍ਰਤਿਜ੍ਞਾ(size2 == 0 || list2);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (size2 > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - size1)
                ਵਾਪਸ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        ਲਈ (size_t i = 0; i < size1; i++)
                ਜੇ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        ਲਈ (size_t i = 0; i < size2; i++)
                ਜੇ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ਵਾਪਸ (ਪੂਰਨ)nresult;
}

ਪੂਰਨ getgroups_alloc(gid_t **ret) {
        ਪੂਰਨ ngroups = 8;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ attempt = 0;;) {
                _cleanup_free_ gid_t *p = ਨਿਰਾ;

                p = new(gid_t, ngroups);
                ਜੇ (!p)
                        ਵਾਪਸ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ਜੇ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ਵਾਪਸ ngroups;
                }
                ਜੇ (ngroups == 0)
                        ਰੋਕੋ;
                ਜੇ (errno != EINVAL)
                        ਵਾਪਸ -errno;

                /* Give up eventually */
                ਜੇ (attempt++ > 10)
                        ਵਾਪਸ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ਨਿਰਾ);
                ਜੇ (ngroups < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (ngroups == 0)
                        ਰੋਕੋ;
        }

        *ret = ਨਿਰਾ;
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_home_dir(ਅੱਖਰ **ret) {
        _cleanup_free_ ਬਣਤਰ passwd *p = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e;
        uid_t u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ਜੇ (e && ਰਸਤਾ_is_valid(e) && ਰਸਤਾ_is_absolute(e))
                ਜਾਓ ਮਿਲਿਆ;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ਜੇ (u == 0) {
                e = "/root";
                ਜਾਓ ਮਿਲਿਆ;
        }
        ਜੇ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                ਜਾਓ ਮਿਲਿਆ;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        e = p->pw_dir;
        ਜੇ (!ਰਸਤਾ_is_valid(e) || !ਰਸਤਾ_is_absolute(e))
                ਵਾਪਸ -EINVAL;

 ਮਿਲਿਆ:
        ਵਾਪਸ ਰਸਤਾ_simplify_alloc(e, ret);
}

ਪੂਰਨ get_shell(ਅੱਖਰ **ret) {
        _cleanup_free_ ਬਣਤਰ passwd *p = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *e;
        uid_t u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ਜੇ (e && ਰਸਤਾ_is_valid(e) && ਰਸਤਾ_is_absolute(e))
                ਜਾਓ ਮਿਲਿਆ;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ਜੇ (u == 0) {
                e = default_ਜੜ੍ਹ_shell(ਨਿਰਾ);
                ਜਾਓ ਮਿਲਿਆ;
        }
        ਜੇ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                ਜਾਓ ਮਿਲਿਆ;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        e = p->pw_shell;
        ਜੇ (!ਰਸਤਾ_is_valid(e) || !ਰਸਤਾ_is_absolute(e))
                ਵਾਪਸ -EINVAL;

 ਮਿਲਿਆ:
        ਵਾਪਸ ਰਸਤਾ_simplify_alloc(e, ret);
}

ਪੂਰਨ fully_ਸੈੱਟ_uid_gid(uid_t uid, gid_t gid, ਅਚਲ gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (gid_is_valid(gid))
                ਜੇ (setresgid(gid, gid, gid) < 0)
                        ਵਾਪਸ -errno;

        ਜੇ (uid_is_valid(uid))
                ਜੇ (setresuid(uid, uid, uid) < 0)
                        ਵਾਪਸ -errno;

        ਵਾਪਸ 0;
}

ਪੂਰਨ take_etc_passwd_ਤਾਲਾ(ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ) {
        ਪੂਰਨ r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਰਸਤਾ_join(ਜੜ੍ਹ, ETC_PASSWD_ਤਾਲਾ_ਰਸਤਾ);
        ਜੇ (!ਰਸਤਾ)
                ਵਾਪਸ log_oom_debug();

        (ਖਾਲੀ) mkdir_parents(ਰਸਤਾ, 0755);

        _cleanup_close_ ਪੂਰਨ fd = open(ਰਸਤਾ, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ਜੇ (fd < 0)
                ਵਾਪਸ log_debug_errno(errno, "Cannot open %s: %m", ਰਸਤਾ);

        r = unposix_ਤਾਲਾ(fd, LOCK_EX);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Locking %s failed: %m", ਰਸਤਾ);

        ਵਾਪਸ TAKE_FD(fd);
}

bool valid_user_ਸਮੂਹ_name(ਅਚਲ ਅੱਖਰ *u, ValidUserFlags flags) {
        ਅਚਲ ਅੱਖਰ *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ਜੇ (isempty(u)) /* An empty user name is never valid */
                ਵਾਪਸ false;

        ਜੇ (parse_uid(u, ਨਿਰਾ) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ਵਾਪਸ FLAGS_ਸੈੱਟ(flags, VALID_USER_ALLOW_NUMERIC);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, VALID_USER_RELAX)) {

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

                ਜੇ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ਵਾਪਸ false;

                ਜੇ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ਵਾਪਸ false;

                ਜੇ (string_has_cc(u, ਨਿਰਾ)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ਵਾਪਸ false;

                ਜੇ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ਵਾਪਸ false;

                ਜੇ (ਵਿੱਚ_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ਵਾਪਸ false;

                ਜੇ (u[0] == '-' && ਵਿੱਚ_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ਵਾਪਸ false;

                ਜੇ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ਵਾਪਸ false;

                /* Compare with strict result and warn if result doesn't match */
                ਜੇ (FLAGS_ਸੈੱਟ(flags, VALID_USER_WARN) && !valid_user_ਸਮੂਹ_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_ਸੁਨੇਹਾ("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ਵਸਤੂ("USER_GROUP_NAME=%s", u),
                                   LOG_ਸੁਨੇਹਾ_ID(SD_ਸੁਨੇਹਾ_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } ਨਹੀਂਤਾਂ {
                ਲੰਮਾ sz;
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

                ਜੇ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ਵਾਪਸ false;

                ਲਈ (i = u+1; *i; i++)
                        ਜੇ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !ਵਿੱਚ_ਸੈੱਟ(*i, '_', '-'))
                                ਵਾਪਸ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ਜੇ (l > (size_t) sz) /* glibc: 256 */
                        ਵਾਪਸ false;
                ਜੇ (l > NAME_MAX) /* must fit in a filename: 255 */
                        ਵਾਪਸ false;
                ਜੇ (l > sizeof_ਖੇਤਰ(ਬਣਤਰ utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ true;
}

bool valid_gecos(ਅਚਲ ਅੱਖਰ *d) {

        ਜੇ (!d)
                ਵਾਪਸ false;

        ਜੇ (!utf8_is_valid(d))
                ਵਾਪਸ false;

        ਜੇ (string_has_cc(d, ਨਿਰਾ))
                ਵਾਪਸ false;

        /* Colons are used as field separators, and hence not OK */
        ਜੇ (strchr(d, ':'))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਅੱਖਰ* mangle_gecos(ਅਚਲ ਅੱਖਰ *d) {
        ਅੱਖਰ *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ਜੇ (!mangled)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (ਅੱਖਰ *i = mangled; *i; i++) {
                ਪੂਰਨ len;

                ਜੇ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        ਜਾਰੀ;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ਜੇ (len < 0) {
                        *i = ' ';
                        ਜਾਰੀ;
                }

                i += len - 1;
        }

        ਵਾਪਸ mangled;
}

bool valid_home(ਅਚਲ ਅੱਖਰ *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਜੇ (!utf8_is_valid(p))
                ਵਾਪਸ false;

        ਜੇ (string_has_cc(p, ਨਿਰਾ))
                ਵਾਪਸ false;

        ਜੇ (!ਰਸਤਾ_is_absolute(p))
                ਵਾਪਸ false;

        ਜੇ (!ਰਸਤਾ_is_normalized(p))
                ਵਾਪਸ false;

        /* Colons are used as field separators, and hence not OK */
        ਜੇ (strchr(p, ':'))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool valid_shell(ਅਚਲ ਅੱਖਰ *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ਜੇ (!valid_home(p))
                ਵਾਪਸ false;

        ਵਾਪਸ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ਪੂਰਨ maybe_setgroups(size_t ਮਾਪ, ਅਚਲ gid_t *list) {
        ਪੂਰਨ r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ਜੇ (ਮਾਪ == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ਜੇ (!f && errno != ENOENT)
                        ਵਾਪਸ -errno;
                ਜੇ (f) {
                        r = ਸੁਰੱਖਿਅਤ_fgetc(f, /* ret= */ ਨਿਰਾ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ਵਾਪਸ 0;
                        }
                }

                _cleanup_free_ ਅੱਖਰ *setgroups_content = ਨਿਰਾ;
                r = read_one_ਸਤਰ_file("/proc/self/setgroups", &setgroups_content);
                ਜੇ (r < 0 && r != -ENOENT)
                        ਵਾਪਸ r;
                ਜੇ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ਵਾਪਸ 0;
                }
        }

        ਵਾਪਸ RET_NERRNO(setgroups(ਮਾਪ, list));
}

bool synthesize_nobody(ਖਾਲੀ) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ਸਥਿਰ ਪੂਰਨ cache = -1;

        ਜੇ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ਵਾਪਸ cache;
}

ਪੂਰਨ putpwent_sane(ਅਚਲ ਬਣਤਰ passwd *pw, FILE *ਧਾਰਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pw);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);

        errno = 0;
        ਜੇ (putpwent(pw, ਧਾਰਾ) != 0)
                ਵਾਪਸ errno_or_else(EIO);

        ਵਾਪਸ 0;
}

ਪੂਰਨ putspent_sane(ਅਚਲ ਬਣਤਰ spwd *sp, FILE *ਧਾਰਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(sp);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);

        errno = 0;
        ਜੇ (putspent(sp, ਧਾਰਾ) != 0)
                ਵਾਪਸ errno_or_else(EIO);

        ਵਾਪਸ 0;
}

ਪੂਰਨ putgrent_sane(ਅਚਲ ਬਣਤਰ group *gr, FILE *ਧਾਰਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(gr);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);

        errno = 0;
        ਜੇ (putgrent(gr, ਧਾਰਾ) != 0)
                ਵਾਪਸ errno_or_else(EIO);

        ਵਾਪਸ 0;
}

#ਜੇ ENABLE_GSHADOW
ਪੂਰਨ putsgent_sane(ਅਚਲ ਬਣਤਰ sgrp *sg, FILE *ਧਾਰਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(sg);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);

        errno = 0;
        ਜੇ (putsgent(sg, ਧਾਰਾ) != 0)
                ਵਾਪਸ errno_or_else(EIO);

        ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ fgetpwent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ passwd **pw) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(pw);

        errno = 0;
        ਬਣਤਰ passwd *p = fgetpwent(ਧਾਰਾ);
        ਜੇ (!p && !ਵਿੱਚ_ਸੈੱਟ(errno, 0, ENOENT))
                ਵਾਪਸ -errno;

        *pw = p;
        ਵਾਪਸ !!p;
}

ਪੂਰਨ fgetspent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ spwd **sp) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(sp);

        errno = 0;
        ਬਣਤਰ spwd *s = fgetspent(ਧਾਰਾ);
        ਜੇ (!s && !ਵਿੱਚ_ਸੈੱਟ(errno, 0, ENOENT))
                ਵਾਪਸ -errno;

        *sp = s;
        ਵਾਪਸ !!s;
}

ਪੂਰਨ fgetgrent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ group **gr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(gr);

        errno = 0;
        ਬਣਤਰ group *g = fgetgrent(ਧਾਰਾ);
        ਜੇ (!g && !ਵਿੱਚ_ਸੈੱਟ(errno, 0, ENOENT))
                ਵਾਪਸ -errno;

        *gr = g;
        ਵਾਪਸ !!g;
}

#ਜੇ ENABLE_GSHADOW
ਪੂਰਨ fgetsgent_sane(FILE *ਧਾਰਾ, ਬਣਤਰ sgrp **sg) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਧਾਰਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(sg);

        errno = 0;
        ਬਣਤਰ sgrp *s = fgetsgent(ਧਾਰਾ);
        ਜੇ (!s && !ਵਿੱਚ_ਸੈੱਟ(errno, 0, ENOENT))
                ਵਾਪਸ -errno;

        *sg = s;
        ਵਾਪਸ !!s;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ is_this_me(ਅਚਲ ਅੱਖਰ *username) {
        uid_t uid;
        ਪੂਰਨ r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ਨਿਰਾ, &uid, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ uid == getuid();
}

ਅਚਲ ਅੱਖਰ* get_home_ਜੜ੍ਹ(ਖਾਲੀ) {
        /* For debug purposes allow overriding where we look for home dirs */
        ਅਚਲ ਅੱਖਰ *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ਜੇ (e && ਰਸਤਾ_is_absolute(e) && ਰਸਤਾ_is_normalized(e))
                ਵਾਪਸ e;

        ਵਾਪਸ "/home";
}

ਸਥਿਰ ਪੂਰਨ copy_struct_passwd(ਅਚਲ ਬਣਤਰ passwd *pw, ਬਣਤਰ passwd **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(pw);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        size_t need_ਬਾਈਟਾਂ = ਆਕਾਰ(ਬਣਤਰ passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        ਅੱਖਰ *buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(need_ਬਾਈਟਾਂ);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        ਬਣਤਰ passwd *newpw = (ਬਣਤਰ passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        ਅੱਖਰ *p = buf + ਆਕਾਰ(ਬਣਤਰ passwd);

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
        ਵਾਪਸ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ਪੂਰਨ lookup_pwent_ਵਿੱਚ_files(
                ਅੱਖਰ * ਅਚਲ *files,
                ਅਚਲ ਅੱਖਰ *name,
                uid_t uid,
                ਬਣਤਰ passwd **ret) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(files);
        ਪ੍ਰਤਿਜ੍ਞਾ(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ਨਿਰਾ;
                ਬਣਤਰ passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ਜੇ (r == -ENOENT)
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜਦੋਂ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ਜੇ (name && !streq_ptr(pw->pw_name, name))
                                ਜਾਰੀ;
                        ਜੇ (uid_is_valid(uid) && pw->pw_uid != uid)
                                ਜਾਰੀ;
                        ਜੇ (ret)
                                ਵਾਪਸ copy_struct_passwd(pw, ret);
                        ਵਾਪਸ 0;
                }
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ -ESRCH;
}

#ਜੇ !BUILD_STATIC

ਸਥਿਰ size_t getpw_ਬਫਰ_ਮਾਪ(ਖਾਲੀ) {
        ਲੰਮਾ bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ਵਾਪਸ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ਸਥਿਰ bool errno_is_user_doesnt_exist(ਪੂਰਨ error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ਵਾਪਸ ਵਿੱਚ_ਸੈੱਟ(ਨਿਰਪੇਖ(error), ENOENT, ESRCH, EBADF, EPERM);
}

#ਜੇ_ਅੰਤ

ਪੂਰਨ getpwnam_malloc(ਅਚਲ ਅੱਖਰ *name, ਬਣਤਰ passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ਜੇ (isempty(name))
                ਵਾਪਸ -EINVAL;

#ਜੇ BUILD_STATIC
        ਵਾਪਸ lookup_pwent_ਵਿੱਚ_files(PASSWD_FILES, name, UID_INVALID, ret);
#ਨਹੀਂਤਾਂ
        size_t bufsize = getpw_ਬਫਰ_ਮਾਪ();
        ਪੂਰਨ r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        ਲਈ (;;) {
                _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;

                /* Silence static analyzers */
                ਪ੍ਰਤਿਜ੍ਞਾ(bufsize <= SIZE_MAX - ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)));
                buf = malloc0(ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)) + bufsize);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                ਬਣਤਰ passwd *pw = ਨਿਰਾ;
                r = getpwnam_r(name, buf, (ਅੱਖਰ*) buf + ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)), bufsize, &pw);
                ਜੇ (r == 0) {
                        ਜੇ (pw) {
                                ਜੇ (ret)
                                        *ret = TAKE_PTR(buf);
                                ਵਾਪਸ 0;
                        }

                        ਵਾਪਸ -ESRCH;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ਜੇ (errno_is_user_doesnt_exist(r))
                        ਵਾਪਸ -ESRCH;
                ਜੇ (r != ERANGE)
                        ਵਾਪਸ -r;

                ਜੇ (bufsize > SIZE_MAX/2 - ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)))
                        ਵਾਪਸ -ENOMEM;
                bufsize *= 2;
        }
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ getpwuid_malloc(uid_t uid, ਬਣਤਰ passwd **ret) {
        ਜੇ (!uid_is_valid(uid))
                ਵਾਪਸ -EINVAL;

#ਜੇ BUILD_STATIC
        ਵਾਪਸ lookup_pwent_ਵਿੱਚ_files(PASSWD_FILES, /* name= */ ਨਿਰਾ, uid, ret);
#ਨਹੀਂਤਾਂ
        size_t bufsize = getpw_ਬਫਰ_ਮਾਪ();
        ਪੂਰਨ r;

        ਲਈ (;;) {
                _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;

                /* Silence static analyzers */
                ਪ੍ਰਤਿਜ੍ਞਾ(bufsize <= SIZE_MAX - ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)));
                buf = malloc0(ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)) + bufsize);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                ਬਣਤਰ passwd *pw = ਨਿਰਾ;
                r = getpwuid_r(uid, buf, (ਅੱਖਰ*) buf + ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)), bufsize, &pw);
                ਜੇ (r == 0) {
                        ਜੇ (pw) {
                                ਜੇ (ret)
                                        *ret = TAKE_PTR(buf);
                                ਵਾਪਸ 0;
                        }

                        ਵਾਪਸ -ESRCH;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

                ਜੇ (errno_is_user_doesnt_exist(r))
                        ਵਾਪਸ -ESRCH;
                ਜੇ (r != ERANGE)
                        ਵਾਪਸ -r;

                ਜੇ (bufsize > SIZE_MAX/2 - ALIGN(ਆਕਾਰ(ਬਣਤਰ passwd)))
                        ਵਾਪਸ -ENOMEM;
                bufsize *= 2;
        }
#ਜੇ_ਅੰਤ
}

ਸਥਿਰ ਪੂਰਨ copy_struct_ਸਮੂਹ(ਅਚਲ ਬਣਤਰ group *gr, ਬਣਤਰ group **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(gr);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        size_t need_ਬਾਈਟਾਂ = ਆਕਾਰ(ਬਣਤਰ group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_ਬਾਈਟਾਂ += ਆਕਾਰ(ਅੱਖਰ*) + ਲੜੀ_ਲੰਬਾਈ(*s) + 1;
                n_mem++;
        }
        need_ਬਾਈਟਾਂ += ਆਕਾਰ(ਅੱਖਰ*);  /* NULL terminator for gr_mem */

        ਅੱਖਰ *buf = ਸਮ੍ਰਿਤੀ_ਦਿਓ(need_ਬਾਈਟਾਂ);
        ਜੇ (!buf)
                ਵਾਪਸ -ENOMEM;

        ਬਣਤਰ group *newgr = (ਬਣਤਰ group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(ਆਕਾਰ(ਬਣਤਰ group) % alignof(ਅੱਖਰ*) == 0);

        ਅੱਖਰ *p = buf + ਆਕਾਰ(ਬਣਤਰ group) + (n_mem + 1) * ਆਕਾਰ(ਅੱਖਰ*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (ਅੱਖਰ**) (buf + ਆਕਾਰ(ਬਣਤਰ group));
        ਲਈ (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ਨਿਰਾ;

        *ret = newgr;
        ਵਾਪਸ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ਪੂਰਨ lookup_grent_ਵਿੱਚ_files(
                ਅੱਖਰ * ਅਚਲ *files,
                ਅਚਲ ਅੱਖਰ *name,
                gid_t gid,
                ਬਣਤਰ group **ret) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(files);
        ਪ੍ਰਤਿਜ੍ਞਾ(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ਨਿਰਾ;
                ਬਣਤਰ group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ਜੇ (r == -ENOENT)
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜਦੋਂ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ਜੇ (name && !streq_ptr(gr->gr_name, name))
                                ਜਾਰੀ;
                        ਜੇ (gid_is_valid(gid) && gr->gr_gid != gid)
                                ਜਾਰੀ;
                        ਜੇ (ret)
                                ਵਾਪਸ copy_struct_ਸਮੂਹ(gr, ret);
                        ਵਾਪਸ 0;
                }
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ -ESRCH;
}

ਪੂਰਨ sysconf_ngroups_max(ਖਾਲੀ) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        ਲੰਮਾ ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ਜੇ (ngroups_max <= 0)
                ਵਾਪਸ errno_or_else(EOPNOTSUPP);
        ਜੇ (ngroups_max > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
                ਵਾਪਸ -ERANGE;
        ਵਾਪਸ ngroups_max;
}

ssize_t lookup_groups_ਵਿੱਚ_files(
                ਅੱਖਰ * ਅਚਲ *files,
                ਅਚਲ ਅੱਖਰ *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ਨਿਰਾ;
        ssize_t n_arr = 0;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(files);
        ਪ੍ਰਤਿਜ੍ਞਾ(name);
        ਪ੍ਰਤਿਜ੍ਞਾ(gid_is_valid(gid));

        ਪੂਰਨ ngroups_max = sysconf_ngroups_max();
        ਜੇ (ngroups_max < 0)
                ਵਾਪਸ ngroups_max;

        ਜੇ (!GREEDY_REALLOC(arr, n_arr + 1))
                ਵਾਪਸ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ਨਿਰਾ;
                ਬਣਤਰ group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ਜੇ (r == -ENOENT)
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜਦੋਂ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ਜੇ (gid == gr->gr_gid)
                                ਜਾਰੀ;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ਜੇ (gid_list_has(arr, n_arr, gr->gr_gid))
                                ਜਾਰੀ;

                        ਜੇ (!strv_ਸ਼ਾਮਲ_ਹੈ(gr->gr_mem, name))
                                ਜਾਰੀ;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ਜੇ (n_arr >= ngroups_max)
                                ਵਾਪਸ -ENOBUFS;

                        ਜੇ (!GREEDY_REALLOC(arr, n_arr + 1))
                                ਵਾਪਸ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret)
                *ret = TAKE_PTR(arr);
        ਵਾਪਸ n_arr;
}

ਪੂਰਨ getgrouplist_malloc(ਅਚਲ ਅੱਖਰ *user, gid_t gid, gid_t **ret) {
#ਜੇ BUILD_STATIC
        ਵਾਪਸ lookup_groups_ਵਿੱਚ_files(ਸਮੂਹ_FILES, user, gid, ret);
#ਨਹੀਂਤਾਂ
        ਪੂਰਨ ngroups_max = sysconf_ngroups_max();
        ਜੇ (ngroups_max < 0)
                ਵਾਪਸ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ਜੇ (!gids)
                ਵਾਪਸ -ENOMEM;

        ਪੂਰਨ k = ngroups_max;
        ਜੇ (getgrouplist(user, gid, gids, &k) < 0)
                ਵਾਪਸ -EIO;  /* getgrouplist does not set errno */

        ਜੇ (ret)
                *ret = TAKE_PTR(gids);
        ਵਾਪਸ k;
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ initgroups_wrapper(ਅਚਲ ਅੱਖਰ *user, gid_t gid) {
#ਜੇ BUILD_STATIC
        _cleanup_free_ gid_t *groups = ਨਿਰਾ;
        ਪੂਰਨ r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ਜੇ (n_groups <= 0)
                ਵਾਪਸ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        ਲਈ (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ਜੇ (r != -EINVAL)
                        ਰੋਕੋ;
        }

        ਵਾਪਸ r;
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ RET_NERRNO(initgroups(user, gid));
#ਜੇ_ਅੰਤ
}

#ਜੇ !BUILD_STATIC

ਸਥਿਰ size_t getgr_ਬਫਰ_ਮਾਪ(ਖਾਲੀ) {
        ਲੰਮਾ bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ਵਾਪਸ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#ਜੇ_ਅੰਤ

ਪੂਰਨ getgrnam_malloc(ਅਚਲ ਅੱਖਰ *name, ਬਣਤਰ group **ret) {
        ਜੇ (isempty(name))
                ਵਾਪਸ -EINVAL;

#ਜੇ BUILD_STATIC
        ਵਾਪਸ lookup_grent_ਵਿੱਚ_files(ਸਮੂਹ_FILES, name, GID_INVALID, ret);
#ਨਹੀਂਤਾਂ
        size_t bufsize = getgr_ਬਫਰ_ਮਾਪ();
        ਪੂਰਨ r;

        ਲਈ (;;) {
                _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;

                /* Silence static analyzers */
                ਪ੍ਰਤਿਜ੍ਞਾ(bufsize <= SIZE_MAX - ALIGN(ਆਕਾਰ(ਬਣਤਰ group)));
                buf = malloc0(ALIGN(ਆਕਾਰ(ਬਣਤਰ group)) + bufsize);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                ਬਣਤਰ group *gr = ਨਿਰਾ;
                r = getgrnam_r(name, buf, (ਅੱਖਰ*) buf + ALIGN(ਆਕਾਰ(ਬਣਤਰ group)), bufsize, &gr);
                ਜੇ (r == 0) {
                        ਜੇ (gr) {
                                ਜੇ (ret)
                                        *ret = TAKE_PTR(buf);
                                ਵਾਪਸ 0;
                        }

                        ਵਾਪਸ -ESRCH;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

                ਜੇ (errno_is_user_doesnt_exist(r))
                        ਵਾਪਸ -ESRCH;
                ਜੇ (r != ERANGE)
                        ਵਾਪਸ -r;

                ਜੇ (bufsize > SIZE_MAX/2 - ALIGN(ਆਕਾਰ(ਬਣਤਰ group)))
                        ਵਾਪਸ -ENOMEM;
                bufsize *= 2;
        }
#ਜੇ_ਅੰਤ
}

ਪੂਰਨ getgrgid_malloc(gid_t gid, ਬਣਤਰ group **ret) {
        ਜੇ (!gid_is_valid(gid))
                ਵਾਪਸ -EINVAL;

#ਜੇ BUILD_STATIC
        ਵਾਪਸ lookup_grent_ਵਿੱਚ_files(ਸਮੂਹ_FILES, /* name= */ ਨਿਰਾ, gid, ret);
#ਨਹੀਂਤਾਂ
        size_t bufsize = getgr_ਬਫਰ_ਮਾਪ();
        ਪੂਰਨ r;

        ਲਈ (;;) {
                _cleanup_free_ ਖਾਲੀ *buf = ਨਿਰਾ;

                /* Silence static analyzers */
                ਪ੍ਰਤਿਜ੍ਞਾ(bufsize <= SIZE_MAX - ALIGN(ਆਕਾਰ(ਬਣਤਰ group)));
                buf = malloc0(ALIGN(ਆਕਾਰ(ਬਣਤਰ group)) + bufsize);
                ਜੇ (!buf)
                        ਵਾਪਸ -ENOMEM;

                ਬਣਤਰ group *gr = ਨਿਰਾ;
                r = getgrgid_r(gid, buf, (ਅੱਖਰ*) buf + ALIGN(ਆਕਾਰ(ਬਣਤਰ group)), bufsize, &gr);
                ਜੇ (r == 0) {
                        ਜੇ (gr) {
                                ਜੇ (ret)
                                        *ret = TAKE_PTR(buf);
                                ਵਾਪਸ 0;
                        }

                        ਵਾਪਸ -ESRCH;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ(r > 0);

                ਜੇ (errno_is_user_doesnt_exist(r))
                        ਵਾਪਸ -ESRCH;
                ਜੇ (r != ERANGE)
                        ਵਾਪਸ -r;

                ਜੇ (bufsize > SIZE_MAX/2 - ALIGN(ਆਕਾਰ(ਬਣਤਰ group)))
                        ਵਾਪਸ -ENOMEM;
                bufsize *= 2;
        }
#ਜੇ_ਅੰਤ
}
