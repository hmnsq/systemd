/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/file.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <utmpx.h>

#ଅନ୍ତର୍ଭାଵ "sd-messages.h"

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "lock-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mkdir.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "terminal-util.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        ଅଚଳ ଅକ୍ଷର* strerror_##type(                                    \
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା errnum,                                     \
                        ଅକ୍ଷର *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                ବାଛ (errnum) {                                       \
                ସ୍ଥିତି ESRCH:                                             \
                        ଫେରନ୍ତୁ "Unknown " STRINGIFY(type);              \
                ସ୍ଥିତି ENOEXEC:                                           \
                        ଫେରନ୍ତୁ "Not a system " STRINGIFY(type);         \
                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:                                                \
                        ଫେରନ୍ତୁ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        ଯଦି (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ଫେରନ୍ତୁ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        ଯଦି (uid == (uid_t) UINT32_C(0xFFFF))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_uid(ଅଚଳ ଅକ୍ଷର *s, uid_t *ret) {
        uint32_t uid = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        assert_cc(ଆକାର(uid_t) == ଆକାର(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (!uid_is_valid(uid))
                ଫେରନ୍ତୁ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        ଯଦି (ret)
                *ret = uid;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_uid_range(ଅଚଳ ଅକ୍ଷର *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ ଅକ୍ଷର *word = ଖାଲି;
        uid_t l, u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ret_lower);
        ପ୍ରତିଜ୍ଞା(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EINVAL;

        r = parse_uid(word, &l);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Check for the upper bound and extract it if needed */
        ଯଦି (!s)
                /* Single number with no dash. */
                u = l;
        ଅନ୍ୟଥା ଯଦି (!*s)
                /* Trailing dash is an error. */
                ଫେରନ୍ତୁ -EINVAL;
        ଅନ୍ୟଥା {
                r = parse_uid(s, &u);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (l > u)
                        ଫେରନ୍ତୁ -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* getlogname_malloc(ଶୂନ୍ୟ) {
        uid_t uid;
        ସଂରଚନା stat st;

        ଯଦି (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        ଅନ୍ୟଥା
                uid = getuid();

        ଫେରନ୍ତୁ uid_to_name(uid);
}

ଅକ୍ଷର* getusername_malloc(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *e;

        e = secure_getenv("USER");
        ଯଦି (e)
                ଫେରନ୍ତୁ strdup(e);

        ଫେରନ୍ତୁ uid_to_name(getuid());
}

bool is_nologin_shell(ଅଚଳ ଅକ୍ଷର *shell) {
        ଫେରନ୍ତୁ PATH_IN_SET(shell,
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

bool shell_is_placeholder(ଅଚଳ ଅକ୍ଷର *shell) {
        ଫେରନ୍ତୁ isempty(shell) || is_nologin_shell(shell);
}

ଅଚଳ ଅକ୍ଷର* default_ମୂଳ_shell_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        ପ୍ରତିଜ୍ଞା(rfd >= 0 || rfd == AT_FDCWD);

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ଖାଲି, ଖାଲି);
        ଯଦି (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ DEFAULT_USER_SHELL;

        ଫେରନ୍ତୁ "/bin/sh";
}

ଅଚଳ ଅକ୍ଷର* default_ମୂଳ_shell(ଅଚଳ ଅକ୍ଷର *ମୂଳ) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd = -EBADF;

        rfd = open(empty_to_ମୂଳ(ମୂଳ), O_CLOEXEC | O_DIRECTORY | O_PATH);
        ଯଦି (rfd < 0)
                ଫେରନ୍ତୁ "/bin/sh";

        ଫେରନ୍ତୁ default_ମୂଳ_shell_at(rfd);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା return_user_creds(
                ଅଚଳ ଅକ୍ଷର *username,
                uid_t uid, gid_t gid,
                ଅଚଳ ଅକ୍ଷର *home,
                ଅଚଳ ଅକ୍ଷର *shell,
                ଅକ୍ଷର **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ଅକ୍ଷର **ret_home,
                ଅକ୍ଷର **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ ଅକ୍ଷର *s1 = ଖାଲି, *s2 = ଖାଲି, *s3 = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (ret_username) {
                r = strdup_to(&s1, username);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_home) {
                r = strdup_to(&s2, home);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_shell) {
                r = strdup_to(&s3, shell);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_username)
                *ret_username = TAKE_PTR(s1);
        ଯଦି (ret_uid)
                *ret_uid = uid;
        ଯଦି (ret_gid)
                *ret_gid = gid;
        ଯଦି (ret_home)
                *ret_home = TAKE_PTR(s2);
        ଯଦି (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା synthesize_user_creds(
                ଅଚଳ ଅକ୍ଷର *username,
                UserCredsFlags flags,
                ଅକ୍ଷର **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ଅକ୍ଷର **ret_home,
                ଅକ୍ଷର **ret_shell) {
        ପ୍ରତିଜ୍ଞା(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        ଯଦି (STR_IN_SET(username, "root", "0"))
                ଫେରନ୍ତୁ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_ମୂଳ_shell(ଖାଲି) : ଖାଲି,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ଯଦି (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ଫେରନ୍ତୁ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ଖାଲି : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ଖାଲି : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ଫେରନ୍ତୁ -ENOMEDIUM;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_user_creds(
                ଅଚଳ ଅକ୍ଷର *username,
                UserCredsFlags flags,
                ଅକ୍ଷର **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ଅକ୍ଷର **ret_home,
                ଅକ୍ଷର **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ସଂରଚନା passwd *pw = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(username);
        ପ୍ରତିଜ୍ଞା((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        ଯଦି (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ 0;
                ଯଦି (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                ଯଦି (r >= 0)
                        username = pw->pw_name;

                ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        ଯଦି (ret_username)
                                *ret_username = ଖାଲି;
                        ଯଦି (ret_uid)
                                *ret_uid = u;
                        ଫେରନ୍ତୁ 0;
                }
        } ଅନ୍ୟଥା
                r = getpwnam_malloc(username, &pw);

        ଯଦି (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                ଯଦି (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ଫେରନ୍ତୁ 0;

                ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_uid && !uid_is_valid(pw->pw_uid))
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (ret_gid && !gid_is_valid(pw->pw_gid))
                ଫେରନ୍ତୁ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ଅଚଳ ଅକ୍ଷର *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_ମୂଳ(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? ଖାଲି : pw->pw_dir;

        ଅଚଳ ଅକ୍ଷର *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? ଖାଲି : pw->pw_shell;

        ଫେରନ୍ତୁ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା synthesize_group_creds(ଅଚଳ ଅକ୍ଷର *groupname, ଅକ୍ଷର **ret_name, gid_t *ret_gid) {
        ପ୍ରତିଜ୍ଞା(groupname);

        gid_t id;
        ଅଚଳ ଅକ୍ଷର *n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } ଅନ୍ୟଥା ଯଦି (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } ଅନ୍ୟଥା
                ଫେରନ୍ତୁ -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (ret_gid)
                *ret_gid = id;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_group_creds(ଅଚଳ ଅକ୍ଷର *groupname, UserCredsFlags flags, ଅକ୍ଷର **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ସଂରଚନା group *gr = ଖାଲି;
        gid_t id;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(groupname);

        ଯଦି (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ 0;
                ଯଦି (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                ଯଦି (r >= 0)
                        groupname = gr->gr_name;
                ଅନ୍ୟଥା ଯଦି (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        ଯଦି (ret_gid)
                                *ret_gid = id;
                        ଯଦି (ret_name)
                                *ret_name = ଖାଲି;
                        ଫେରନ୍ତୁ 0;
                }
        } ଅନ୍ୟଥା
                r = getgrnam_malloc(groupname, &gr);

        ଯଦି (r < 0) {
                ଯଦି (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ଫେରନ୍ତୁ 0;
                ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_gid && !gid_is_valid(gr->gr_gid))
                ଫେରନ୍ତୁ -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (ret_gid)
                *ret_gid = gr->gr_gid;
        ଫେରନ୍ତୁ 0;
}

ଅକ୍ଷର* uid_to_name(uid_t uid) {
        ଅକ୍ଷର *ret;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Shortcut things to avoid NSS lookups */
        ଯଦି (uid == 0)
                ଫେରନ୍ତୁ strdup("root");
        ଯଦି (uid == UID_NOBODY && synthesize_nobody())
                ଫେରନ୍ତୁ strdup(NOBODY_USER_NAME);

        ଯଦି (uid_is_valid(uid)) {
                _cleanup_free_ ସଂରଚନା passwd *pw = ଖାଲି;

                r = getpwuid_malloc(uid, &pw);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ strdup(pw->pw_name);
        }

        ଯଦି (asprintf(&ret, UID_FMT, uid) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ ret;
}

ଅକ୍ଷର* gid_to_name(gid_t gid) {
        ଅକ୍ଷର *ret;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (gid == 0)
                ଫେରନ୍ତୁ strdup("root");
        ଯଦି (gid == GID_NOBODY && synthesize_nobody())
                ଫେରନ୍ତୁ strdup(NOBODY_GROUP_NAME);

        ଯଦି (gid_is_valid(gid)) {
                _cleanup_free_ ସଂରଚନା group *gr = ଖାଲି;

                r = getgrgid_malloc(gid, &gr);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ strdup(gr->gr_name);
        }

        ଯଦି (asprintf(&ret, GID_FMT, gid) < 0)
                ଫେରନ୍ତୁ ଖାଲି;

        ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର bool gid_list_has(ଅଚଳ gid_t *list, size_t size, gid_t val) {
        ପ୍ରତିଜ୍ଞା(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                ଯଦି (*i == val)
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngroups;

        ଯଦି (getgid() == gid)
                ଫେରନ୍ତୁ 1;

        ଯଦି (getegid() == gid)
                ଫେରନ୍ତୁ 1;

        ଯଦି (!gid_is_valid(gid))
                ଫେରନ୍ତୁ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        ଯଦି (ngroups < 0)
                ଫେରନ୍ତୁ ngroups;

        ଫେରନ୍ତୁ gid_list_has(gids, ngroups, gid);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_group(ଅଚଳ ଅକ୍ଷର *name) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ ଖାଲି, &gid);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ in_gid(gid);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା merge_gid_lists(ଅଚଳ gid_t *list1, size_t size1, ଅଚଳ gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        ପ୍ରତିଜ୍ଞା(size1 == 0 || list1);
        ପ୍ରତିଜ୍ଞା(size2 == 0 || list2);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (size2 > INT_MAX - size1)
                ଫେରନ୍ତୁ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        ପାଇଁ (size_t i = 0; i < size1; i++)
                ଯଦି (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        ପାଇଁ (size_t i = 0; i < size2; i++)
                ଯଦି (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nresult;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgroups_alloc(gid_t **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngroups = 8;

        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = ଖାଲି;

                p = new(gid_t, ngroups);
                ଯଦି (!p)
                        ଫେରନ୍ତୁ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                ଯଦି (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ଫେରନ୍ତୁ ngroups;
                }
                ଯଦି (ngroups == 0)
                        ଭାଙ୍ଗ;
                ଯଦି (errno != EINVAL)
                        ଫେରନ୍ତୁ -errno;

                /* Give up eventually */
                ଯଦି (attempt++ > 10)
                        ଫେରନ୍ତୁ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ଖାଲି);
                ଯଦି (ngroups < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (ngroups == 0)
                        ଭାଙ୍ଗ;
        }

        *ret = ଖାଲି;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_home_dir(ଅକ୍ଷର **ret) {
        _cleanup_free_ ସଂରଚନା passwd *p = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *e;
        uid_t u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        ଯଦି (e && path_is_valid(e) && path_is_absolute(e))
                ଯାଅ found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        ଯଦି (u == 0) {
                e = "/root";
                ଯାଅ found;
        }
        ଯଦି (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                ଯାଅ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        e = p->pw_dir;
        ଯଦି (!path_is_valid(e) || !path_is_absolute(e))
                ଫେରନ୍ତୁ -EINVAL;

 found:
        ଫେରନ୍ତୁ path_simplify_alloc(e, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_shell(ଅକ୍ଷର **ret) {
        _cleanup_free_ ସଂରଚନା passwd *p = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *e;
        uid_t u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        ଯଦି (e && path_is_valid(e) && path_is_absolute(e))
                ଯାଅ found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        ଯଦି (u == 0) {
                e = default_ମୂଳ_shell(ଖାଲି);
                ଯାଅ found;
        }
        ଯଦି (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                ଯାଅ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        e = p->pw_shell;
        ଯଦି (!path_is_valid(e) || !path_is_absolute(e))
                ଫେରନ୍ତୁ -EINVAL;

 found:
        ଫେରନ୍ତୁ path_simplify_alloc(e, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fully_set_uid_gid(uid_t uid, gid_t gid, ଅଚଳ gid_t supplementary_gids[], size_t n_supplementary_gids) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (gid_is_valid(gid))
                ଯଦି (setresgid(gid, gid, gid) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଯଦି (uid_is_valid(uid))
                ଯଦି (setresuid(uid, uid, uid) < 0)
                        ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା take_etc_passwd_lock(ଅଚଳ ଅକ୍ଷର *ମୂଳ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ ଅକ୍ଷର *path = path_join(ମୂଳ, ETC_PASSWD_LOCK_PATH);
        ଯଦି (!path)
                ଫେରନ୍ତୁ log_oom_debug();

        (ଶୂନ୍ୟ) mkdir_parents(path, 0755);

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Locking %s failed: %m", path);

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

bool valid_user_group_name(ଅଚଳ ଅକ୍ଷର *u, ValidUserFlags flags) {
        ଅଚଳ ଅକ୍ଷର *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        ଯଦି (isempty(u)) /* An empty user name is never valid */
                ଫେରନ୍ତୁ false;

        ଯଦି (parse_uid(u, ଖାଲି) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ଫେରନ୍ତୁ FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        ଯଦି (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                ଯଦି (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        ଫେରନ୍ତୁ false;

                ଯଦି (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ଫେରନ୍ତୁ false;

                ଯଦି (string_has_cc(u, ଖାଲି)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ଫେରନ୍ତୁ false;

                ଯଦି (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ଫେରନ୍ତୁ false;

                ଯଦି (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ଫେରନ୍ତୁ false;

                ଯଦି (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ଫେରନ୍ତୁ false;

                ଯଦି (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ଫେରନ୍ତୁ false;

                /* Compare with strict result and warn if result doesn't match */
                ଯଦି (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } ଅନ୍ୟଥା {
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

                ଯଦି (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        ଫେରନ୍ତୁ false;

                ପାଇଁ (i = u+1; *i; i++)
                        ଯଦି (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ଫେରନ୍ତୁ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                ଯଦି (l > (size_t) sz) /* glibc: 256 */
                        ଫେରନ୍ତୁ false;
                ଯଦି (l > NAME_MAX) /* must fit in a filename: 255 */
                        ଫେରନ୍ତୁ false;
                ଯଦି (l > sizeof_field(ସଂରଚନା utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ true;
}

bool valid_gecos(ଅଚଳ ଅକ୍ଷର *d) {

        ଯଦି (!d)
                ଫେରନ୍ତୁ false;

        ଯଦି (!utf8_is_valid(d))
                ଫେରନ୍ତୁ false;

        ଯଦି (string_has_cc(d, ଖାଲି))
                ଫେରନ୍ତୁ false;

        /* Colons are used as field separators, and hence not OK */
        ଯଦି (strchr(d, ':'))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ଅକ୍ଷର* mangle_gecos(ଅଚଳ ଅକ୍ଷର *d) {
        ଅକ୍ଷର *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        ଯଦି (!mangled)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (ଅକ୍ଷର *i = mangled; *i; i++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;

                ଯଦି ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        ଜାରି;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                ଯଦି (len < 0) {
                        *i = ' ';
                        ଜାରି;
                }

                i += len - 1;
        }

        ଫେରନ୍ତୁ mangled;
}

bool valid_home(ଅଚଳ ଅକ୍ଷର *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (!utf8_is_valid(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (string_has_cc(p, ଖାଲି))
                ଫେରନ୍ତୁ false;

        ଯଦି (!path_is_absolute(p))
                ଫେରନ୍ତୁ false;

        ଯଦି (!path_is_normalized(p))
                ଫେରନ୍ତୁ false;

        /* Colons are used as field separators, and hence not OK */
        ଯଦି (strchr(p, ':'))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool valid_shell(ଅଚଳ ଅକ୍ଷର *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        ଯଦି (!valid_home(p))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା maybe_setgroups(size_t size, ଅଚଳ gid_t *list) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        ଯଦି (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                ଯଦି (!f && errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (f) {
                        r = safe_fgetc(f, /* ret= */ ଖାଲି);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ଫେରନ୍ତୁ 0;
                        }
                }

                _cleanup_free_ ଅକ୍ଷର *setgroups_content = ଖାଲି;
                r = read_one_ଧାଡ଼ି_file("/proc/self/setgroups", &setgroups_content);
                ଯଦି (r < 0 && r != -ENOENT)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ଫେରନ୍ତୁ 0;
                }
        }

        ଫେରନ୍ତୁ RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(ଶୂନ୍ୟ) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cache = -1;

        ଯଦି (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ଫେରନ୍ତୁ cache;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା putpwent_sane(ଅଚଳ ସଂରଚନା passwd *pw, FILE *stream) {
        ପ୍ରତିଜ୍ଞା(pw);
        ପ୍ରତିଜ୍ଞା(stream);

        errno = 0;
        ଯଦି (putpwent(pw, stream) != 0)
                ଫେରନ୍ତୁ errno_or_else(EIO);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା putspent_sane(ଅଚଳ ସଂରଚନା spwd *sp, FILE *stream) {
        ପ୍ରତିଜ୍ଞା(sp);
        ପ୍ରତିଜ୍ଞା(stream);

        errno = 0;
        ଯଦି (putspent(sp, stream) != 0)
                ଫେରନ୍ତୁ errno_or_else(EIO);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା putgrent_sane(ଅଚଳ ସଂରଚନା group *gr, FILE *stream) {
        ପ୍ରତିଜ୍ଞା(gr);
        ପ୍ରତିଜ୍ଞା(stream);

        errno = 0;
        ଯଦି (putgrent(gr, stream) != 0)
                ଫେରନ୍ତୁ errno_or_else(EIO);

        ଫେରନ୍ତୁ 0;
}

#ଯଦି ENABLE_GSHADOW
ପୂର୍ଣ୍ଣସଂଖ୍ୟା putsgent_sane(ଅଚଳ ସଂରଚନା sgrp *sg, FILE *stream) {
        ପ୍ରତିଜ୍ଞା(sg);
        ପ୍ରତିଜ୍ଞା(stream);

        errno = 0;
        ଯଦି (putsgent(sg, stream) != 0)
                ଫେରନ୍ତୁ errno_or_else(EIO);

        ଫେରନ୍ତୁ 0;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetpwent_sane(FILE *stream, ସଂରଚନା passwd **pw) {
        ପ୍ରତିଜ୍ଞା(stream);
        ପ୍ରତିଜ୍ଞା(pw);

        errno = 0;
        ସଂରଚନା passwd *p = fgetpwent(stream);
        ଯଦି (!p && !IN_SET(errno, 0, ENOENT))
                ଫେରନ୍ତୁ -errno;

        *pw = p;
        ଫେରନ୍ତୁ !!p;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetspent_sane(FILE *stream, ସଂରଚନା spwd **sp) {
        ପ୍ରତିଜ୍ଞା(stream);
        ପ୍ରତିଜ୍ଞା(sp);

        errno = 0;
        ସଂରଚନା spwd *s = fgetspent(stream);
        ଯଦି (!s && !IN_SET(errno, 0, ENOENT))
                ଫେରନ୍ତୁ -errno;

        *sp = s;
        ଫେରନ୍ତୁ !!s;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetgrent_sane(FILE *stream, ସଂରଚନା group **gr) {
        ପ୍ରତିଜ୍ଞା(stream);
        ପ୍ରତିଜ୍ଞା(gr);

        errno = 0;
        ସଂରଚନା group *g = fgetgrent(stream);
        ଯଦି (!g && !IN_SET(errno, 0, ENOENT))
                ଫେରନ୍ତୁ -errno;

        *gr = g;
        ଫେରନ୍ତୁ !!g;
}

#ଯଦି ENABLE_GSHADOW
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetsgent_sane(FILE *stream, ସଂରଚନା sgrp **sg) {
        ପ୍ରତିଜ୍ଞା(stream);
        ପ୍ରତିଜ୍ଞା(sg);

        errno = 0;
        ସଂରଚନା sgrp *s = fgetsgent(stream);
        ଯଦି (!s && !IN_SET(errno, 0, ENOENT))
                ଫେରନ୍ତୁ -errno;

        *sg = s;
        ଫେରନ୍ତୁ !!s;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_this_me(ଅଚଳ ଅକ୍ଷର *username) {
        uid_t uid;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ଖାଲି, &uid, ଖାଲି, ଖାଲି, ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ uid == getuid();
}

ଅଚଳ ଅକ୍ଷର* get_home_ମୂଳ(ଶୂନ୍ୟ) {
        /* For debug purposes allow overriding where we look for home dirs */
        ଅଚଳ ଅକ୍ଷର *e = secure_getenv("SYSTEMD_HOME_ROOT");
        ଯଦି (e && path_is_absolute(e) && path_is_normalized(e))
                ଫେରନ୍ତୁ e;

        ଫେରନ୍ତୁ "/home";
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_struct_passwd(ଅଚଳ ସଂରଚନା passwd *pw, ସଂରଚନା passwd **ret) {
        ପ୍ରତିଜ୍ଞା(pw);
        ପ୍ରତିଜ୍ଞା(ret);

        size_t need_bytes = ଆକାର(ସଂରଚନା passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        ଅକ୍ଷର *buf = malloc(need_bytes);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        ସଂରଚନା passwd *newpw = (ସଂରଚନା passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        ଅକ୍ଷର *p = buf + ଆକାର(ସଂରଚନା passwd);

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
        ଫେରନ୍ତୁ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_pwent_in_files(
                ଅକ୍ଷର * ଅଚଳ *files,
                ଅଚଳ ଅକ୍ଷର *name,
                uid_t uid,
                ସଂରଚନା passwd **ret) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(files);
        ପ୍ରତିଜ୍ଞା(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ଖାଲି;
                ସଂରଚନା passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                ଯଦି (r == -ENOENT)
                        ଜାରି;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯେତେବେଳେ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        ଯଦି (name && !streq_ptr(pw->pw_name, name))
                                ଜାରି;
                        ଯଦି (uid_is_valid(uid) && pw->pw_uid != uid)
                                ଜାରି;
                        ଯଦି (ret)
                                ଫେରନ୍ତୁ copy_struct_passwd(pw, ret);
                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ -ESRCH;
}

#ଯଦି !BUILD_STATIC

ସ୍ଥିର size_t getpw_buffer_size(ଶୂନ୍ୟ) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ଫେରନ୍ତୁ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ସ୍ଥିର bool errno_is_user_doesnt_exist(ପୂର୍ଣ୍ଣସଂଖ୍ୟା error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ଫେରନ୍ତୁ IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpwnam_malloc(ଅଚଳ ଅକ୍ଷର *name, ସଂରଚନା passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        ଯଦି (isempty(name))
                ଫେରନ୍ତୁ -EINVAL;

#ଯଦି BUILD_STATIC
        ଫେରନ୍ତୁ lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#ଅନ୍ୟଥା
        size_t bufsize = getpw_buffer_size();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        ପାଇଁ (;;) {
                _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;

                /* Silence static analyzers */
                ପ୍ରତିଜ୍ଞା(bufsize <= SIZE_MAX - ALIGN(ଆକାର(ସଂରଚନା passwd)));
                buf = malloc0(ALIGN(ଆକାର(ସଂରଚନା passwd)) + bufsize);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                ସଂରଚନା passwd *pw = ଖାଲି;
                r = getpwnam_r(name, buf, (ଅକ୍ଷର*) buf + ALIGN(ଆକାର(ସଂରଚନା passwd)), bufsize, &pw);
                ଯଦି (r == 0) {
                        ଯଦି (pw) {
                                ଯଦି (ret)
                                        *ret = TAKE_PTR(buf);
                                ଫେରନ୍ତୁ 0;
                        }

                        ଫେରନ୍ତୁ -ESRCH;
                }

                ପ୍ରତିଜ୍ଞା(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                ଯଦି (errno_is_user_doesnt_exist(r))
                        ଫେରନ୍ତୁ -ESRCH;
                ଯଦି (r != ERANGE)
                        ଫେରନ୍ତୁ -r;

                ଯଦି (bufsize > SIZE_MAX/2 - ALIGN(ଆକାର(ସଂରଚନା passwd)))
                        ଫେରନ୍ତୁ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getpwuid_malloc(uid_t uid, ସଂରଚନା passwd **ret) {
        ଯଦି (!uid_is_valid(uid))
                ଫେରନ୍ତୁ -EINVAL;

#ଯଦି BUILD_STATIC
        ଫେରନ୍ତୁ lookup_pwent_in_files(PASSWD_FILES, /* name= */ ଖାଲି, uid, ret);
#ଅନ୍ୟଥା
        size_t bufsize = getpw_buffer_size();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;

                /* Silence static analyzers */
                ପ୍ରତିଜ୍ଞା(bufsize <= SIZE_MAX - ALIGN(ଆକାର(ସଂରଚନା passwd)));
                buf = malloc0(ALIGN(ଆକାର(ସଂରଚନା passwd)) + bufsize);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                ସଂରଚନା passwd *pw = ଖାଲି;
                r = getpwuid_r(uid, buf, (ଅକ୍ଷର*) buf + ALIGN(ଆକାର(ସଂରଚନା passwd)), bufsize, &pw);
                ଯଦି (r == 0) {
                        ଯଦି (pw) {
                                ଯଦି (ret)
                                        *ret = TAKE_PTR(buf);
                                ଫେରନ୍ତୁ 0;
                        }

                        ଫେରନ୍ତୁ -ESRCH;
                }

                ପ୍ରତିଜ୍ଞା(r > 0);

                ଯଦି (errno_is_user_doesnt_exist(r))
                        ଫେରନ୍ତୁ -ESRCH;
                ଯଦି (r != ERANGE)
                        ଫେରନ୍ତୁ -r;

                ଯଦି (bufsize > SIZE_MAX/2 - ALIGN(ଆକାର(ସଂରଚନା passwd)))
                        ଫେରନ୍ତୁ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା copy_struct_group(ଅଚଳ ସଂରଚନା group *gr, ସଂରଚନା group **ret) {
        ପ୍ରତିଜ୍ଞା(gr);
        ପ୍ରତିଜ୍ଞା(ret);

        size_t need_bytes = ଆକାର(ସଂରଚନା group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += ଆକାର(ଅକ୍ଷର*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += ଆକାର(ଅକ୍ଷର*);  /* NULL terminator for gr_mem */

        ଅକ୍ଷର *buf = malloc(need_bytes);
        ଯଦି (!buf)
                ଫେରନ୍ତୁ -ENOMEM;

        ସଂରଚନା group *newgr = (ସଂରଚନା group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(ଆକାର(ସଂରଚନା group) % alignof(ଅକ୍ଷର*) == 0);

        ଅକ୍ଷର *p = buf + ଆକାର(ସଂରଚନା group) + (n_mem + 1) * ଆକାର(ଅକ୍ଷର*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (ଅକ୍ଷର**) (buf + ଆକାର(ସଂରଚନା group));
        ପାଇଁ (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ଖାଲି;

        *ret = newgr;
        ଫେରନ୍ତୁ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_grent_in_files(
                ଅକ୍ଷର * ଅଚଳ *files,
                ଅଚଳ ଅକ୍ଷର *name,
                gid_t gid,
                ସଂରଚନା group **ret) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(files);
        ପ୍ରତିଜ୍ଞା(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ଖାଲି;
                ସଂରଚନା group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ଯଦି (r == -ENOENT)
                        ଜାରି;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯେତେବେଳେ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ଯଦି (name && !streq_ptr(gr->gr_name, name))
                                ଜାରି;
                        ଯଦି (gid_is_valid(gid) && gr->gr_gid != gid)
                                ଜାରି;
                        ଯଦି (ret)
                                ଫେରନ୍ତୁ copy_struct_group(gr, ret);
                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ -ESRCH;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysconf_ngroups_max(ଶୂନ୍ୟ) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        ଯଦି (ngroups_max <= 0)
                ଫେରନ୍ତୁ errno_or_else(EOPNOTSUPP);
        ଯଦି (ngroups_max > INT_MAX)
                ଫେରନ୍ତୁ -ERANGE;
        ଫେରନ୍ତୁ ngroups_max;
}

ssize_t lookup_groups_in_files(
                ଅକ୍ଷର * ଅଚଳ *files,
                ଅଚଳ ଅକ୍ଷର *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ଖାଲି;
        ssize_t n_arr = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(files);
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(gid_is_valid(gid));

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngroups_max = sysconf_ngroups_max();
        ଯଦି (ngroups_max < 0)
                ଫେରନ୍ତୁ ngroups_max;

        ଯଦି (!GREEDY_REALLOC(arr, n_arr + 1))
                ଫେରନ୍ତୁ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ଖାଲି;
                ସଂରଚନା group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                ଯଦି (r == -ENOENT)
                        ଜାରି;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯେତେବେଳେ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        ଯଦି (gid == gr->gr_gid)
                                ଜାରି;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        ଯଦି (gid_list_has(arr, n_arr, gr->gr_gid))
                                ଜାରି;

                        ଯଦି (!strv_contains(gr->gr_mem, name))
                                ଜାରି;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        ଯଦି (n_arr >= ngroups_max)
                                ଫେରନ୍ତୁ -ENOBUFS;

                        ଯଦି (!GREEDY_REALLOC(arr, n_arr + 1))
                                ଫେରନ୍ତୁ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret)
                *ret = TAKE_PTR(arr);
        ଫେରନ୍ତୁ n_arr;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgrouplist_malloc(ଅଚଳ ଅକ୍ଷର *user, gid_t gid, gid_t **ret) {
#ଯଦି BUILD_STATIC
        ଫେରନ୍ତୁ lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#ଅନ୍ୟଥା
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ngroups_max = sysconf_ngroups_max();
        ଯଦି (ngroups_max < 0)
                ଫେରନ୍ତୁ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        ଯଦି (!gids)
                ଫେରନ୍ତୁ -ENOMEM;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = ngroups_max;
        ଯଦି (getgrouplist(user, gid, gids, &k) < 0)
                ଫେରନ୍ତୁ -EIO;  /* getgrouplist does not set errno */

        ଯଦି (ret)
                *ret = TAKE_PTR(gids);
        ଫେରନ୍ତୁ k;
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା initgroups_wrapper(ଅଚଳ ଅକ୍ଷର *user, gid_t gid) {
#ଯଦି BUILD_STATIC
        _cleanup_free_ gid_t *groups = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        ଯଦି (n_groups <= 0)
                ଫେରନ୍ତୁ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        ପାଇଁ (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                ଯଦି (r != -EINVAL)
                        ଭାଙ୍ଗ;
        }

        ଫେରନ୍ତୁ r;
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ RET_NERRNO(initgroups(user, gid));
#endif
}

#ଯଦି !BUILD_STATIC

ସ୍ଥିର size_t getgr_buffer_size(ଶୂନ୍ୟ) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ଫେରନ୍ତୁ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgrnam_malloc(ଅଚଳ ଅକ୍ଷର *name, ସଂରଚନା group **ret) {
        ଯଦି (isempty(name))
                ଫେରନ୍ତୁ -EINVAL;

#ଯଦି BUILD_STATIC
        ଫେରନ୍ତୁ lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#ଅନ୍ୟଥା
        size_t bufsize = getgr_buffer_size();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;

                /* Silence static analyzers */
                ପ୍ରତିଜ୍ଞା(bufsize <= SIZE_MAX - ALIGN(ଆକାର(ସଂରଚନା group)));
                buf = malloc0(ALIGN(ଆକାର(ସଂରଚନା group)) + bufsize);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                ସଂରଚନା group *gr = ଖାଲି;
                r = getgrnam_r(name, buf, (ଅକ୍ଷର*) buf + ALIGN(ଆକାର(ସଂରଚନା group)), bufsize, &gr);
                ଯଦି (r == 0) {
                        ଯଦି (gr) {
                                ଯଦି (ret)
                                        *ret = TAKE_PTR(buf);
                                ଫେରନ୍ତୁ 0;
                        }

                        ଫେରନ୍ତୁ -ESRCH;
                }

                ପ୍ରତିଜ୍ଞା(r > 0);

                ଯଦି (errno_is_user_doesnt_exist(r))
                        ଫେରନ୍ତୁ -ESRCH;
                ଯଦି (r != ERANGE)
                        ଫେରନ୍ତୁ -r;

                ଯଦି (bufsize > SIZE_MAX/2 - ALIGN(ଆକାର(ସଂରଚନା group)))
                        ଫେରନ୍ତୁ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getgrgid_malloc(gid_t gid, ସଂରଚନା group **ret) {
        ଯଦି (!gid_is_valid(gid))
                ଫେରନ୍ତୁ -EINVAL;

#ଯଦି BUILD_STATIC
        ଫେରନ୍ତୁ lookup_grent_in_files(GROUP_FILES, /* name= */ ଖାଲି, gid, ret);
#ଅନ୍ୟଥା
        size_t bufsize = getgr_buffer_size();
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପାଇଁ (;;) {
                _cleanup_free_ ଶୂନ୍ୟ *buf = ଖାଲି;

                /* Silence static analyzers */
                ପ୍ରତିଜ୍ଞା(bufsize <= SIZE_MAX - ALIGN(ଆକାର(ସଂରଚନା group)));
                buf = malloc0(ALIGN(ଆକାର(ସଂରଚନା group)) + bufsize);
                ଯଦି (!buf)
                        ଫେରନ୍ତୁ -ENOMEM;

                ସଂରଚନା group *gr = ଖାଲି;
                r = getgrgid_r(gid, buf, (ଅକ୍ଷର*) buf + ALIGN(ଆକାର(ସଂରଚନା group)), bufsize, &gr);
                ଯଦି (r == 0) {
                        ଯଦି (gr) {
                                ଯଦି (ret)
                                        *ret = TAKE_PTR(buf);
                                ଫେରନ୍ତୁ 0;
                        }

                        ଫେରନ୍ତୁ -ESRCH;
                }

                ପ୍ରତିଜ୍ଞା(r > 0);

                ଯଦି (errno_is_user_doesnt_exist(r))
                        ଫେରନ୍ତୁ -ESRCH;
                ଯଦି (r != ERANGE)
                        ଫେରନ୍ତୁ -r;

                ଯଦି (bufsize > SIZE_MAX/2 - ALIGN(ଆକାର(ସଂରଚନା group)))
                        ଫେରନ୍ତୁ -ENOMEM;
                bufsize *= 2;
        }
#endif
}
