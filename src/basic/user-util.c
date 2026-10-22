/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/file.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <utmpx.h>

#അന്തര്ഭാവ "sd-messages.h"

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "lock-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mkdir.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "terminal-util.h"
#അന്തര്ഭാവ "user-util.h"
#അന്തര്ഭാവ "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        സ്ഥിരം അക്ഷരം* strerror_##type(                                    \
                        പൂർണ്ണസംഖ്യ errnum,                                     \
                        അക്ഷരം *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                തിരഞ്ഞെടുപ്പ് (errnum) {                                       \
                അവസ്ഥ ESRCH:                                             \
                        തിരികെ "Unknown " STRINGIFY(type);              \
                അവസ്ഥ ENOEXEC:                                           \
                        തിരികെ "Not a system " STRINGIFY(type);         \
                സ്ഥിരസ്ഥിതി:                                                \
                        തിരികെ strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        എങ്കിൽ (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                തിരികെ false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        എങ്കിൽ (uid == (uid_t) UINT32_C(0xFFFF))
                തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ parse_uid(സ്ഥിരം അക്ഷരം *s, uid_t *ret) {
        uint32_t uid = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        assert_cc(വലുപ്പം(uid_t) == വലുപ്പം(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (!uid_is_valid(uid))
                തിരികെ -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        എങ്കിൽ (ret)
                *ret = uid;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ parse_uid_range(സ്ഥിരം അക്ഷരം *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ അക്ഷരം *word = ശൂന്യ;
        uid_t l, u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(ret_lower);
        പ്രതിജ്ഞാ(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EINVAL;

        r = parse_uid(word, &l);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Check for the upper bound and extract it if needed */
        എങ്കിൽ (!s)
                /* Single number with no dash. */
                u = l;
        അല്ലെങ്കിൽ എങ്കിൽ (!*s)
                /* Trailing dash is an error. */
                തിരികെ -EINVAL;
        അല്ലെങ്കിൽ {
                r = parse_uid(s, &u);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (l > u)
                        തിരികെ -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        തിരികെ 0;
}

അക്ഷരം* getlogname_malloc(ശൂന്യം) {
        uid_t uid;
        ഘടന stat st;

        എങ്കിൽ (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        അല്ലെങ്കിൽ
                uid = getuid();

        തിരികെ uid_to_name(uid);
}

അക്ഷരം* getusername_malloc(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *e;

        e = secure_getenv("USER");
        എങ്കിൽ (e)
                തിരികെ strdup(e);

        തിരികെ uid_to_name(getuid());
}

bool is_nologin_shell(സ്ഥിരം അക്ഷരം *shell) {
        തിരികെ PATH_IN_SET(shell,
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

bool shell_is_placeholder(സ്ഥിരം അക്ഷരം *shell) {
        തിരികെ isempty(shell) || is_nologin_shell(shell);
}

സ്ഥിരം അക്ഷരം* default_വേര്_shell_at(പൂർണ്ണസംഖ്യ rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        പ്രതിജ്ഞാ(rfd >= 0 || rfd == AT_FDCWD);

        പൂർണ്ണസംഖ്യ r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ശൂന്യ, ശൂന്യ);
        എങ്കിൽ (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        എങ്കിൽ (r > 0)
                തിരികെ DEFAULT_USER_SHELL;

        തിരികെ "/bin/sh";
}

സ്ഥിരം അക്ഷരം* default_വേര്_shell(സ്ഥിരം അക്ഷരം *വേര്) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = -EBADF;

        rfd = open(empty_to_വേര്(വേര്), O_CLOEXEC | O_DIRECTORY | O_PATH);
        എങ്കിൽ (rfd < 0)
                തിരികെ "/bin/sh";

        തിരികെ default_വേര്_shell_at(rfd);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ return_user_creds(
                സ്ഥിരം അക്ഷരം *username,
                uid_t uid, gid_t gid,
                സ്ഥിരം അക്ഷരം *home,
                സ്ഥിരം അക്ഷരം *shell,
                അക്ഷരം **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                അക്ഷരം **ret_home,
                അക്ഷരം **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ അക്ഷരം *s1 = ശൂന്യ, *s2 = ശൂന്യ, *s3 = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (ret_username) {
                r = strdup_to(&s1, username);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_home) {
                r = strdup_to(&s2, home);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_shell) {
                r = strdup_to(&s3, shell);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_username)
                *ret_username = TAKE_PTR(s1);
        എങ്കിൽ (ret_uid)
                *ret_uid = uid;
        എങ്കിൽ (ret_gid)
                *ret_gid = gid;
        എങ്കിൽ (ret_home)
                *ret_home = TAKE_PTR(s2);
        എങ്കിൽ (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ synthesize_user_creds(
                സ്ഥിരം അക്ഷരം *username,
                UserCredsFlags flags,
                അക്ഷരം **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                അക്ഷരം **ret_home,
                അക്ഷരം **ret_shell) {
        പ്രതിജ്ഞാ(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        എങ്കിൽ (STR_IN_SET(username, "root", "0"))
                തിരികെ return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_വേര്_shell(ശൂന്യ) : ശൂന്യ,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        എങ്കിൽ (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                തിരികെ return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ശൂന്യ : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ശൂന്യ : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        തിരികെ -ENOMEDIUM;
}

പൂർണ്ണസംഖ്യ get_user_creds(
                സ്ഥിരം അക്ഷരം *username,
                UserCredsFlags flags,
                അക്ഷരം **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                അക്ഷരം **ret_home,
                അക്ഷരം **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ഘടന passwd *pw = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(username);
        പ്രതിജ്ഞാ((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        എങ്കിൽ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                എങ്കിൽ (r >= 0)
                        തിരികെ 0;
                എങ്കിൽ (r != -ENOMEDIUM) /* not a username we can synthesize */
                        തിരികെ r;
        }

        എങ്കിൽ (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                എങ്കിൽ (r >= 0)
                        username = pw->pw_name;

                അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        എങ്കിൽ (ret_username)
                                *ret_username = ശൂന്യ;
                        എങ്കിൽ (ret_uid)
                                *ret_uid = u;
                        തിരികെ 0;
                }
        } അല്ലെങ്കിൽ
                r = getpwnam_malloc(username, &pw);

        എങ്കിൽ (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                എങ്കിൽ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        തിരികെ 0;

                തിരികെ r;
        }

        എങ്കിൽ (ret_uid && !uid_is_valid(pw->pw_uid))
                തിരികെ -EBADMSG;

        എങ്കിൽ (ret_gid && !gid_is_valid(pw->pw_gid))
                തിരികെ -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        സ്ഥിരം അക്ഷരം *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_വേര്(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? ശൂന്യ : pw->pw_dir;

        സ്ഥിരം അക്ഷരം *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? ശൂന്യ : pw->pw_shell;

        തിരികെ return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ synthesize_group_creds(സ്ഥിരം അക്ഷരം *groupname, അക്ഷരം **ret_name, gid_t *ret_gid) {
        പ്രതിജ്ഞാ(groupname);

        gid_t id;
        സ്ഥിരം അക്ഷരം *n;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } അല്ലെങ്കിൽ എങ്കിൽ (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } അല്ലെങ്കിൽ
                തിരികെ -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (ret_gid)
                *ret_gid = id;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_group_creds(സ്ഥിരം അക്ഷരം *groupname, UserCredsFlags flags, അക്ഷരം **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ഘടന group *gr = ശൂന്യ;
        gid_t id;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(groupname);

        എങ്കിൽ (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                എങ്കിൽ (r >= 0)
                        തിരികെ 0;
                എങ്കിൽ (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        തിരികെ r;
        }

        എങ്കിൽ (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                എങ്കിൽ (r >= 0)
                        groupname = gr->gr_name;
                അല്ലെങ്കിൽ എങ്കിൽ (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        എങ്കിൽ (ret_gid)
                                *ret_gid = id;
                        എങ്കിൽ (ret_name)
                                *ret_name = ശൂന്യ;
                        തിരികെ 0;
                }
        } അല്ലെങ്കിൽ
                r = getgrnam_malloc(groupname, &gr);

        എങ്കിൽ (r < 0) {
                എങ്കിൽ (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        തിരികെ 0;
                തിരികെ r;
        }

        എങ്കിൽ (ret_gid && !gid_is_valid(gr->gr_gid))
                തിരികെ -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (ret_gid)
                *ret_gid = gr->gr_gid;
        തിരികെ 0;
}

അക്ഷരം* uid_to_name(uid_t uid) {
        അക്ഷരം *ret;
        പൂർണ്ണസംഖ്യ r;

        /* Shortcut things to avoid NSS lookups */
        എങ്കിൽ (uid == 0)
                തിരികെ strdup("root");
        എങ്കിൽ (uid == UID_NOBODY && synthesize_nobody())
                തിരികെ strdup(NOBODY_USER_NAME);

        എങ്കിൽ (uid_is_valid(uid)) {
                _cleanup_free_ ഘടന passwd *pw = ശൂന്യ;

                r = getpwuid_malloc(uid, &pw);
                എങ്കിൽ (r >= 0)
                        തിരികെ strdup(pw->pw_name);
        }

        എങ്കിൽ (asprintf(&ret, UID_FMT, uid) < 0)
                തിരികെ ശൂന്യ;

        തിരികെ ret;
}

അക്ഷരം* gid_to_name(gid_t gid) {
        അക്ഷരം *ret;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (gid == 0)
                തിരികെ strdup("root");
        എങ്കിൽ (gid == GID_NOBODY && synthesize_nobody())
                തിരികെ strdup(NOBODY_GROUP_NAME);

        എങ്കിൽ (gid_is_valid(gid)) {
                _cleanup_free_ ഘടന group *gr = ശൂന്യ;

                r = getgrgid_malloc(gid, &gr);
                എങ്കിൽ (r >= 0)
                        തിരികെ strdup(gr->gr_name);
        }

        എങ്കിൽ (asprintf(&ret, GID_FMT, gid) < 0)
                തിരികെ ശൂന്യ;

        തിരികെ ret;
}

സ്ഥിതം bool gid_list_has(സ്ഥിരം gid_t *list, size_t size, gid_t val) {
        പ്രതിജ്ഞാ(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                എങ്കിൽ (*i == val)
                        തിരികെ true;

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ശൂന്യ;
        പൂർണ്ണസംഖ്യ ngroups;

        എങ്കിൽ (getgid() == gid)
                തിരികെ 1;

        എങ്കിൽ (getegid() == gid)
                തിരികെ 1;

        എങ്കിൽ (!gid_is_valid(gid))
                തിരികെ -EINVAL;

        ngroups = getgroups_alloc(&gids);
        എങ്കിൽ (ngroups < 0)
                തിരികെ ngroups;

        തിരികെ gid_list_has(gids, ngroups, gid);
}

പൂർണ്ണസംഖ്യ in_group(സ്ഥിരം അക്ഷരം *name) {
        പൂർണ്ണസംഖ്യ r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ ശൂന്യ, &gid);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ in_gid(gid);
}

പൂർണ്ണസംഖ്യ merge_gid_lists(സ്ഥിരം gid_t *list1, size_t size1, സ്ഥിരം gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        പ്രതിജ്ഞാ(size1 == 0 || list1);
        പ്രതിജ്ഞാ(size2 == 0 || list2);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (size2 > INT_MAX - size1)
                തിരികെ -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        വേണ്ടി (size_t i = 0; i < size1; i++)
                എങ്കിൽ (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        വേണ്ടി (size_t i = 0; i < size2; i++)
                എങ്കിൽ (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        തിരികെ (പൂർണ്ണസംഖ്യ)nresult;
}

പൂർണ്ണസംഖ്യ getgroups_alloc(gid_t **ret) {
        പൂർണ്ണസംഖ്യ ngroups = 8;

        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = ശൂന്യ;

                p = new(gid_t, ngroups);
                എങ്കിൽ (!p)
                        തിരികെ -ENOMEM;

                ngroups = getgroups(ngroups, p);
                എങ്കിൽ (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        തിരികെ ngroups;
                }
                എങ്കിൽ (ngroups == 0)
                        നിർത്തുക;
                എങ്കിൽ (errno != EINVAL)
                        തിരികെ -errno;

                /* Give up eventually */
                എങ്കിൽ (attempt++ > 10)
                        തിരികെ -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ശൂന്യ);
                എങ്കിൽ (ngroups < 0)
                        തിരികെ -errno;
                എങ്കിൽ (ngroups == 0)
                        നിർത്തുക;
        }

        *ret = ശൂന്യ;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_home_dir(അക്ഷരം **ret) {
        _cleanup_free_ ഘടന passwd *p = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *e;
        uid_t u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        എങ്കിൽ (e && path_is_valid(e) && path_is_absolute(e))
                പോകുക found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        എങ്കിൽ (u == 0) {
                e = "/root";
                പോകുക found;
        }
        എങ്കിൽ (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                പോകുക found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        e = p->pw_dir;
        എങ്കിൽ (!path_is_valid(e) || !path_is_absolute(e))
                തിരികെ -EINVAL;

 found:
        തിരികെ path_simplify_alloc(e, ret);
}

പൂർണ്ണസംഖ്യ get_shell(അക്ഷരം **ret) {
        _cleanup_free_ ഘടന passwd *p = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *e;
        uid_t u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        എങ്കിൽ (e && path_is_valid(e) && path_is_absolute(e))
                പോകുക found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        എങ്കിൽ (u == 0) {
                e = default_വേര്_shell(ശൂന്യ);
                പോകുക found;
        }
        എങ്കിൽ (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                പോകുക found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        e = p->pw_shell;
        എങ്കിൽ (!path_is_valid(e) || !path_is_absolute(e))
                തിരികെ -EINVAL;

 found:
        തിരികെ path_simplify_alloc(e, ret);
}

പൂർണ്ണസംഖ്യ fully_set_uid_gid(uid_t uid, gid_t gid, സ്ഥിരം gid_t supplementary_gids[], size_t n_supplementary_gids) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (gid_is_valid(gid))
                എങ്കിൽ (setresgid(gid, gid, gid) < 0)
                        തിരികെ -errno;

        എങ്കിൽ (uid_is_valid(uid))
                എങ്കിൽ (setresuid(uid, uid, uid) < 0)
                        തിരികെ -errno;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ take_etc_passwd_lock(സ്ഥിരം അക്ഷരം *വേര്) {
        പൂർണ്ണസംഖ്യ r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ അക്ഷരം *path = path_join(വേര്, ETC_PASSWD_LOCK_PATH);
        എങ്കിൽ (!path)
                തിരികെ log_oom_debug();

        (ശൂന്യം) mkdir_parents(path, 0755);

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        എങ്കിൽ (fd < 0)
                തിരികെ log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Locking %s failed: %m", path);

        തിരികെ TAKE_FD(fd);
}

bool valid_user_group_name(സ്ഥിരം അക്ഷരം *u, ValidUserFlags flags) {
        സ്ഥിരം അക്ഷരം *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        എങ്കിൽ (isempty(u)) /* An empty user name is never valid */
                തിരികെ false;

        എങ്കിൽ (parse_uid(u, ശൂന്യ) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                തിരികെ FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        എങ്കിൽ (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                എങ്കിൽ (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        തിരികെ false;

                എങ്കിൽ (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        തിരികെ false;

                എങ്കിൽ (string_has_cc(u, ശൂന്യ)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        തിരികെ false;

                എങ്കിൽ (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        തിരികെ false;

                എങ്കിൽ (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        തിരികെ false;

                എങ്കിൽ (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        തിരികെ false;

                എങ്കിൽ (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        തിരികെ false;

                /* Compare with strict result and warn if result doesn't match */
                എങ്കിൽ (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } അല്ലെങ്കിൽ {
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

                എങ്കിൽ (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        തിരികെ false;

                വേണ്ടി (i = u+1; *i; i++)
                        എങ്കിൽ (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                തിരികെ false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                എങ്കിൽ (l > (size_t) sz) /* glibc: 256 */
                        തിരികെ false;
                എങ്കിൽ (l > NAME_MAX) /* must fit in a filename: 255 */
                        തിരികെ false;
                എങ്കിൽ (l > sizeof_field(ഘടന utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        തിരികെ false;
        }

        തിരികെ true;
}

bool valid_gecos(സ്ഥിരം അക്ഷരം *d) {

        എങ്കിൽ (!d)
                തിരികെ false;

        എങ്കിൽ (!utf8_is_valid(d))
                തിരികെ false;

        എങ്കിൽ (string_has_cc(d, ശൂന്യ))
                തിരികെ false;

        /* Colons are used as field separators, and hence not OK */
        എങ്കിൽ (strchr(d, ':'))
                തിരികെ false;

        തിരികെ true;
}

അക്ഷരം* mangle_gecos(സ്ഥിരം അക്ഷരം *d) {
        അക്ഷരം *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        എങ്കിൽ (!mangled)
                തിരികെ ശൂന്യ;

        വേണ്ടി (അക്ഷരം *i = mangled; *i; i++) {
                പൂർണ്ണസംഖ്യ len;

                എങ്കിൽ ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        തുടരുക;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                എങ്കിൽ (len < 0) {
                        *i = ' ';
                        തുടരുക;
                }

                i += len - 1;
        }

        തിരികെ mangled;
}

bool valid_home(സ്ഥിരം അക്ഷരം *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        എങ്കിൽ (isempty(p))
                തിരികെ false;

        എങ്കിൽ (!utf8_is_valid(p))
                തിരികെ false;

        എങ്കിൽ (string_has_cc(p, ശൂന്യ))
                തിരികെ false;

        എങ്കിൽ (!path_is_absolute(p))
                തിരികെ false;

        എങ്കിൽ (!path_is_normalized(p))
                തിരികെ false;

        /* Colons are used as field separators, and hence not OK */
        എങ്കിൽ (strchr(p, ':'))
                തിരികെ false;

        തിരികെ true;
}

bool valid_shell(സ്ഥിരം അക്ഷരം *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        എങ്കിൽ (!valid_home(p))
                തിരികെ false;

        തിരികെ !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

പൂർണ്ണസംഖ്യ maybe_setgroups(size_t size, സ്ഥിരം gid_t *list) {
        പൂർണ്ണസംഖ്യ r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        എങ്കിൽ (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                എങ്കിൽ (!f && errno != ENOENT)
                        തിരികെ -errno;
                എങ്കിൽ (f) {
                        r = safe_fgetc(f, /* ret= */ ശൂന്യ);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                തിരികെ 0;
                        }
                }

                _cleanup_free_ അക്ഷരം *setgroups_content = ശൂന്യ;
                r = read_one_വരി_file("/proc/self/setgroups", &setgroups_content);
                എങ്കിൽ (r < 0 && r != -ENOENT)
                        തിരികെ r;
                എങ്കിൽ (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        തിരികെ 0;
                }
        }

        തിരികെ RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(ശൂന്യം) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        സ്ഥിതം പൂർണ്ണസംഖ്യ cache = -1;

        എങ്കിൽ (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        തിരികെ cache;
}

പൂർണ്ണസംഖ്യ putpwent_sane(സ്ഥിരം ഘടന passwd *pw, FILE *stream) {
        പ്രതിജ്ഞാ(pw);
        പ്രതിജ്ഞാ(stream);

        errno = 0;
        എങ്കിൽ (putpwent(pw, stream) != 0)
                തിരികെ errno_or_else(EIO);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ putspent_sane(സ്ഥിരം ഘടന spwd *sp, FILE *stream) {
        പ്രതിജ്ഞാ(sp);
        പ്രതിജ്ഞാ(stream);

        errno = 0;
        എങ്കിൽ (putspent(sp, stream) != 0)
                തിരികെ errno_or_else(EIO);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ putgrent_sane(സ്ഥിരം ഘടന group *gr, FILE *stream) {
        പ്രതിജ്ഞാ(gr);
        പ്രതിജ്ഞാ(stream);

        errno = 0;
        എങ്കിൽ (putgrent(gr, stream) != 0)
                തിരികെ errno_or_else(EIO);

        തിരികെ 0;
}

#എങ്കിൽ ENABLE_GSHADOW
പൂർണ്ണസംഖ്യ putsgent_sane(സ്ഥിരം ഘടന sgrp *sg, FILE *stream) {
        പ്രതിജ്ഞാ(sg);
        പ്രതിജ്ഞാ(stream);

        errno = 0;
        എങ്കിൽ (putsgent(sg, stream) != 0)
                തിരികെ errno_or_else(EIO);

        തിരികെ 0;
}
#endif

പൂർണ്ണസംഖ്യ fgetpwent_sane(FILE *stream, ഘടന passwd **pw) {
        പ്രതിജ്ഞാ(stream);
        പ്രതിജ്ഞാ(pw);

        errno = 0;
        ഘടന passwd *p = fgetpwent(stream);
        എങ്കിൽ (!p && !IN_SET(errno, 0, ENOENT))
                തിരികെ -errno;

        *pw = p;
        തിരികെ !!p;
}

പൂർണ്ണസംഖ്യ fgetspent_sane(FILE *stream, ഘടന spwd **sp) {
        പ്രതിജ്ഞാ(stream);
        പ്രതിജ്ഞാ(sp);

        errno = 0;
        ഘടന spwd *s = fgetspent(stream);
        എങ്കിൽ (!s && !IN_SET(errno, 0, ENOENT))
                തിരികെ -errno;

        *sp = s;
        തിരികെ !!s;
}

പൂർണ്ണസംഖ്യ fgetgrent_sane(FILE *stream, ഘടന group **gr) {
        പ്രതിജ്ഞാ(stream);
        പ്രതിജ്ഞാ(gr);

        errno = 0;
        ഘടന group *g = fgetgrent(stream);
        എങ്കിൽ (!g && !IN_SET(errno, 0, ENOENT))
                തിരികെ -errno;

        *gr = g;
        തിരികെ !!g;
}

#എങ്കിൽ ENABLE_GSHADOW
പൂർണ്ണസംഖ്യ fgetsgent_sane(FILE *stream, ഘടന sgrp **sg) {
        പ്രതിജ്ഞാ(stream);
        പ്രതിജ്ഞാ(sg);

        errno = 0;
        ഘടന sgrp *s = fgetsgent(stream);
        എങ്കിൽ (!s && !IN_SET(errno, 0, ENOENT))
                തിരികെ -errno;

        *sg = s;
        തിരികെ !!s;
}
#endif

പൂർണ്ണസംഖ്യ is_this_me(സ്ഥിരം അക്ഷരം *username) {
        uid_t uid;
        പൂർണ്ണസംഖ്യ r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ശൂന്യ, &uid, ശൂന്യ, ശൂന്യ, ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ uid == getuid();
}

സ്ഥിരം അക്ഷരം* get_home_വേര്(ശൂന്യം) {
        /* For debug purposes allow overriding where we look for home dirs */
        സ്ഥിരം അക്ഷരം *e = secure_getenv("SYSTEMD_HOME_ROOT");
        എങ്കിൽ (e && path_is_absolute(e) && path_is_normalized(e))
                തിരികെ e;

        തിരികെ "/home";
}

സ്ഥിതം പൂർണ്ണസംഖ്യ copy_struct_passwd(സ്ഥിരം ഘടന passwd *pw, ഘടന passwd **ret) {
        പ്രതിജ്ഞാ(pw);
        പ്രതിജ്ഞാ(ret);

        size_t need_bytes = വലുപ്പം(ഘടന passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        അക്ഷരം *buf = malloc(need_bytes);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        ഘടന passwd *newpw = (ഘടന passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        അക്ഷരം *p = buf + വലുപ്പം(ഘടന passwd);

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
        തിരികെ 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
പൂർണ്ണസംഖ്യ lookup_pwent_in_files(
                അക്ഷരം * സ്ഥിരം *files,
                സ്ഥിരം അക്ഷരം *name,
                uid_t uid,
                ഘടന passwd **ret) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(files);
        പ്രതിജ്ഞാ(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ശൂന്യ;
                ഘടന passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                എങ്കിൽ (r == -ENOENT)
                        തുടരുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ആകുമ്പോൾ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        എങ്കിൽ (name && !streq_ptr(pw->pw_name, name))
                                തുടരുക;
                        എങ്കിൽ (uid_is_valid(uid) && pw->pw_uid != uid)
                                തുടരുക;
                        എങ്കിൽ (ret)
                                തിരികെ copy_struct_passwd(pw, ret);
                        തിരികെ 0;
                }
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ -ESRCH;
}

#എങ്കിൽ !BUILD_STATIC

സ്ഥിതം size_t getpw_buffer_size(ശൂന്യം) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        തിരികെ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

സ്ഥിതം bool errno_is_user_doesnt_exist(പൂർണ്ണസംഖ്യ error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        തിരികെ IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

പൂർണ്ണസംഖ്യ getpwnam_malloc(സ്ഥിരം അക്ഷരം *name, ഘടന passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        എങ്കിൽ (isempty(name))
                തിരികെ -EINVAL;

#എങ്കിൽ BUILD_STATIC
        തിരികെ lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#അല്ലെങ്കിൽ
        size_t bufsize = getpw_buffer_size();
        പൂർണ്ണസംഖ്യ r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        വേണ്ടി (;;) {
                _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;

                /* Silence static analyzers */
                പ്രതിജ്ഞാ(bufsize <= SIZE_MAX - ALIGN(വലുപ്പം(ഘടന passwd)));
                buf = malloc0(ALIGN(വലുപ്പം(ഘടന passwd)) + bufsize);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                ഘടന passwd *pw = ശൂന്യ;
                r = getpwnam_r(name, buf, (അക്ഷരം*) buf + ALIGN(വലുപ്പം(ഘടന passwd)), bufsize, &pw);
                എങ്കിൽ (r == 0) {
                        എങ്കിൽ (pw) {
                                എങ്കിൽ (ret)
                                        *ret = TAKE_PTR(buf);
                                തിരികെ 0;
                        }

                        തിരികെ -ESRCH;
                }

                പ്രതിജ്ഞാ(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                എങ്കിൽ (errno_is_user_doesnt_exist(r))
                        തിരികെ -ESRCH;
                എങ്കിൽ (r != ERANGE)
                        തിരികെ -r;

                എങ്കിൽ (bufsize > SIZE_MAX/2 - ALIGN(വലുപ്പം(ഘടന passwd)))
                        തിരികെ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

പൂർണ്ണസംഖ്യ getpwuid_malloc(uid_t uid, ഘടന passwd **ret) {
        എങ്കിൽ (!uid_is_valid(uid))
                തിരികെ -EINVAL;

#എങ്കിൽ BUILD_STATIC
        തിരികെ lookup_pwent_in_files(PASSWD_FILES, /* name= */ ശൂന്യ, uid, ret);
#അല്ലെങ്കിൽ
        size_t bufsize = getpw_buffer_size();
        പൂർണ്ണസംഖ്യ r;

        വേണ്ടി (;;) {
                _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;

                /* Silence static analyzers */
                പ്രതിജ്ഞാ(bufsize <= SIZE_MAX - ALIGN(വലുപ്പം(ഘടന passwd)));
                buf = malloc0(ALIGN(വലുപ്പം(ഘടന passwd)) + bufsize);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                ഘടന passwd *pw = ശൂന്യ;
                r = getpwuid_r(uid, buf, (അക്ഷരം*) buf + ALIGN(വലുപ്പം(ഘടന passwd)), bufsize, &pw);
                എങ്കിൽ (r == 0) {
                        എങ്കിൽ (pw) {
                                എങ്കിൽ (ret)
                                        *ret = TAKE_PTR(buf);
                                തിരികെ 0;
                        }

                        തിരികെ -ESRCH;
                }

                പ്രതിജ്ഞാ(r > 0);

                എങ്കിൽ (errno_is_user_doesnt_exist(r))
                        തിരികെ -ESRCH;
                എങ്കിൽ (r != ERANGE)
                        തിരികെ -r;

                എങ്കിൽ (bufsize > SIZE_MAX/2 - ALIGN(വലുപ്പം(ഘടന passwd)))
                        തിരികെ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

സ്ഥിതം പൂർണ്ണസംഖ്യ copy_struct_group(സ്ഥിരം ഘടന group *gr, ഘടന group **ret) {
        പ്രതിജ്ഞാ(gr);
        പ്രതിജ്ഞാ(ret);

        size_t need_bytes = വലുപ്പം(ഘടന group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += വലുപ്പം(അക്ഷരം*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += വലുപ്പം(അക്ഷരം*);  /* NULL terminator for gr_mem */

        അക്ഷരം *buf = malloc(need_bytes);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        ഘടന group *newgr = (ഘടന group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(വലുപ്പം(ഘടന group) % alignof(അക്ഷരം*) == 0);

        അക്ഷരം *p = buf + വലുപ്പം(ഘടന group) + (n_mem + 1) * വലുപ്പം(അക്ഷരം*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (അക്ഷരം**) (buf + വലുപ്പം(ഘടന group));
        വേണ്ടി (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ശൂന്യ;

        *ret = newgr;
        തിരികെ 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
പൂർണ്ണസംഖ്യ lookup_grent_in_files(
                അക്ഷരം * സ്ഥിരം *files,
                സ്ഥിരം അക്ഷരം *name,
                gid_t gid,
                ഘടന group **ret) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(files);
        പ്രതിജ്ഞാ(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ശൂന്യ;
                ഘടന group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                എങ്കിൽ (r == -ENOENT)
                        തുടരുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ആകുമ്പോൾ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        എങ്കിൽ (name && !streq_ptr(gr->gr_name, name))
                                തുടരുക;
                        എങ്കിൽ (gid_is_valid(gid) && gr->gr_gid != gid)
                                തുടരുക;
                        എങ്കിൽ (ret)
                                തിരികെ copy_struct_group(gr, ret);
                        തിരികെ 0;
                }
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ -ESRCH;
}

പൂർണ്ണസംഖ്യ sysconf_ngroups_max(ശൂന്യം) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        എങ്കിൽ (ngroups_max <= 0)
                തിരികെ errno_or_else(EOPNOTSUPP);
        എങ്കിൽ (ngroups_max > INT_MAX)
                തിരികെ -ERANGE;
        തിരികെ ngroups_max;
}

ssize_t lookup_groups_in_files(
                അക്ഷരം * സ്ഥിരം *files,
                സ്ഥിരം അക്ഷരം *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ശൂന്യ;
        ssize_t n_arr = 0;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(files);
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(gid_is_valid(gid));

        പൂർണ്ണസംഖ്യ ngroups_max = sysconf_ngroups_max();
        എങ്കിൽ (ngroups_max < 0)
                തിരികെ ngroups_max;

        എങ്കിൽ (!GREEDY_REALLOC(arr, n_arr + 1))
                തിരികെ -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ശൂന്യ;
                ഘടന group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                എങ്കിൽ (r == -ENOENT)
                        തുടരുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ആകുമ്പോൾ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        എങ്കിൽ (gid == gr->gr_gid)
                                തുടരുക;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        എങ്കിൽ (gid_list_has(arr, n_arr, gr->gr_gid))
                                തുടരുക;

                        എങ്കിൽ (!strv_contains(gr->gr_mem, name))
                                തുടരുക;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        എങ്കിൽ (n_arr >= ngroups_max)
                                തിരികെ -ENOBUFS;

                        എങ്കിൽ (!GREEDY_REALLOC(arr, n_arr + 1))
                                തിരികെ -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(arr);
        തിരികെ n_arr;
}

പൂർണ്ണസംഖ്യ getgrouplist_malloc(സ്ഥിരം അക്ഷരം *user, gid_t gid, gid_t **ret) {
#എങ്കിൽ BUILD_STATIC
        തിരികെ lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#അല്ലെങ്കിൽ
        പൂർണ്ണസംഖ്യ ngroups_max = sysconf_ngroups_max();
        എങ്കിൽ (ngroups_max < 0)
                തിരികെ ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        എങ്കിൽ (!gids)
                തിരികെ -ENOMEM;

        പൂർണ്ണസംഖ്യ k = ngroups_max;
        എങ്കിൽ (getgrouplist(user, gid, gids, &k) < 0)
                തിരികെ -EIO;  /* getgrouplist does not set errno */

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(gids);
        തിരികെ k;
#endif
}

പൂർണ്ണസംഖ്യ initgroups_wrapper(സ്ഥിരം അക്ഷരം *user, gid_t gid) {
#എങ്കിൽ BUILD_STATIC
        _cleanup_free_ gid_t *groups = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        എങ്കിൽ (n_groups <= 0)
                തിരികെ n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        വേണ്ടി (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                എങ്കിൽ (r != -EINVAL)
                        നിർത്തുക;
        }

        തിരികെ r;
#അല്ലെങ്കിൽ
        തിരികെ RET_NERRNO(initgroups(user, gid));
#endif
}

#എങ്കിൽ !BUILD_STATIC

സ്ഥിതം size_t getgr_buffer_size(ശൂന്യം) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        തിരികെ bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

പൂർണ്ണസംഖ്യ getgrnam_malloc(സ്ഥിരം അക്ഷരം *name, ഘടന group **ret) {
        എങ്കിൽ (isempty(name))
                തിരികെ -EINVAL;

#എങ്കിൽ BUILD_STATIC
        തിരികെ lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#അല്ലെങ്കിൽ
        size_t bufsize = getgr_buffer_size();
        പൂർണ്ണസംഖ്യ r;

        വേണ്ടി (;;) {
                _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;

                /* Silence static analyzers */
                പ്രതിജ്ഞാ(bufsize <= SIZE_MAX - ALIGN(വലുപ്പം(ഘടന group)));
                buf = malloc0(ALIGN(വലുപ്പം(ഘടന group)) + bufsize);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                ഘടന group *gr = ശൂന്യ;
                r = getgrnam_r(name, buf, (അക്ഷരം*) buf + ALIGN(വലുപ്പം(ഘടന group)), bufsize, &gr);
                എങ്കിൽ (r == 0) {
                        എങ്കിൽ (gr) {
                                എങ്കിൽ (ret)
                                        *ret = TAKE_PTR(buf);
                                തിരികെ 0;
                        }

                        തിരികെ -ESRCH;
                }

                പ്രതിജ്ഞാ(r > 0);

                എങ്കിൽ (errno_is_user_doesnt_exist(r))
                        തിരികെ -ESRCH;
                എങ്കിൽ (r != ERANGE)
                        തിരികെ -r;

                എങ്കിൽ (bufsize > SIZE_MAX/2 - ALIGN(വലുപ്പം(ഘടന group)))
                        തിരികെ -ENOMEM;
                bufsize *= 2;
        }
#endif
}

പൂർണ്ണസംഖ്യ getgrgid_malloc(gid_t gid, ഘടന group **ret) {
        എങ്കിൽ (!gid_is_valid(gid))
                തിരികെ -EINVAL;

#എങ്കിൽ BUILD_STATIC
        തിരികെ lookup_grent_in_files(GROUP_FILES, /* name= */ ശൂന്യ, gid, ret);
#അല്ലെങ്കിൽ
        size_t bufsize = getgr_buffer_size();
        പൂർണ്ണസംഖ്യ r;

        വേണ്ടി (;;) {
                _cleanup_free_ ശൂന്യം *buf = ശൂന്യ;

                /* Silence static analyzers */
                പ്രതിജ്ഞാ(bufsize <= SIZE_MAX - ALIGN(വലുപ്പം(ഘടന group)));
                buf = malloc0(ALIGN(വലുപ്പം(ഘടന group)) + bufsize);
                എങ്കിൽ (!buf)
                        തിരികെ -ENOMEM;

                ഘടന group *gr = ശൂന്യ;
                r = getgrgid_r(gid, buf, (അക്ഷരം*) buf + ALIGN(വലുപ്പം(ഘടന group)), bufsize, &gr);
                എങ്കിൽ (r == 0) {
                        എങ്കിൽ (gr) {
                                എങ്കിൽ (ret)
                                        *ret = TAKE_PTR(buf);
                                തിരികെ 0;
                        }

                        തിരികെ -ESRCH;
                }

                പ്രതിജ്ഞാ(r > 0);

                എങ്കിൽ (errno_is_user_doesnt_exist(r))
                        തിരികെ -ESRCH;
                എങ്കിൽ (r != ERANGE)
                        തിരികെ -r;

                എങ്കിൽ (bufsize > SIZE_MAX/2 - ALIGN(വലുപ്പം(ഘടന group)))
                        തിരികെ -ENOMEM;
                bufsize *= 2;
        }
#endif
}
