/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/file.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <utmpx.h>

#અન્તર્ભાવ "sd-messages.h"

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "lock-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mkdir.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "terminal-util.h"
#અન્તર્ભાવ "user-util.h"
#અન્તર્ભાવ "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        અચલ અક્ષર* strerror_##type(                                    \
                        પૂર્ણાંક errnum,                                     \
                        અક્ષર *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                પસંદગી (errnum) {                                       \
                સ્થિતિ ESRCH:                                             \
                        પાછા "Unknown " STRINGIFY(type);              \
                સ્થિતિ ENOEXEC:                                           \
                        પાછા "Not a system " STRINGIFY(type);         \
                મૂળભૂત:                                                \
                        પાછા strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        જો (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                પાછા false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        જો (uid == (uid_t) UINT32_C(0xFFFF))
                પાછા false;

        પાછા true;
}

પૂર્ણાંક parse_uid(અચલ અક્ષર *s, uid_t *ret) {
        uint32_t uid = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        assert_cc(કદ(uid_t) == કદ(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        જો (r < 0)
                પાછા r;

        જો (!uid_is_valid(uid))
                પાછા -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        જો (ret)
                *ret = uid;

        પાછા 0;
}

પૂર્ણાંક parse_uid_range(અચલ અક્ષર *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ અક્ષર *word = ખાલી;
        uid_t l, u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(ret_lower);
        પ્રતિજ્ઞા(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EINVAL;

        r = parse_uid(word, &l);
        જો (r < 0)
                પાછા r;

        /* Check for the upper bound and extract it if needed */
        જો (!s)
                /* Single number with no dash. */
                u = l;
        નહીંતર જો (!*s)
                /* Trailing dash is an error. */
                પાછા -EINVAL;
        નહીંતર {
                r = parse_uid(s, &u);
                જો (r < 0)
                        પાછા r;

                જો (l > u)
                        પાછા -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        પાછા 0;
}

અક્ષર* getlogname_malloc(શૂન્ય) {
        uid_t uid;
        સંરચના stat st;

        જો (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        નહીંતર
                uid = getuid();

        પાછા uid_to_name(uid);
}

અક્ષર* getusername_malloc(શૂન્ય) {
        અચલ અક્ષર *e;

        e = secure_getenv("USER");
        જો (e)
                પાછા strdup(e);

        પાછા uid_to_name(getuid());
}

bool is_nologin_shell(અચલ અક્ષર *shell) {
        પાછા PATH_IN_SET(shell,
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

bool shell_is_placeholder(અચલ અક્ષર *shell) {
        પાછા isempty(shell) || is_nologin_shell(shell);
}

અચલ અક્ષર* default_મૂળ_shell_at(પૂર્ણાંક rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        પ્રતિજ્ઞા(rfd >= 0 || rfd == AT_FDCWD);

        પૂર્ણાંક r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ખાલી, ખાલી);
        જો (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        જો (r > 0)
                પાછા DEFAULT_USER_SHELL;

        પાછા "/bin/sh";
}

અચલ અક્ષર* default_મૂળ_shell(અચલ અક્ષર *મૂળ) {
        _cleanup_close_ પૂર્ણાંક rfd = -EBADF;

        rfd = open(empty_to_મૂળ(મૂળ), O_CLOEXEC | O_DIRECTORY | O_PATH);
        જો (rfd < 0)
                પાછા "/bin/sh";

        પાછા default_મૂળ_shell_at(rfd);
}

સ્થિર પૂર્ણાંક return_user_creds(
                અચલ અક્ષર *username,
                uid_t uid, gid_t gid,
                અચલ અક્ષર *home,
                અચલ અક્ષર *shell,
                અક્ષર **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                અક્ષર **ret_home,
                અક્ષર **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ અક્ષર *s1 = ખાલી, *s2 = ખાલી, *s3 = ખાલી;
        પૂર્ણાંક r;

        જો (ret_username) {
                r = strdup_to(&s1, username);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_home) {
                r = strdup_to(&s2, home);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_shell) {
                r = strdup_to(&s3, shell);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_username)
                *ret_username = TAKE_PTR(s1);
        જો (ret_uid)
                *ret_uid = uid;
        જો (ret_gid)
                *ret_gid = gid;
        જો (ret_home)
                *ret_home = TAKE_PTR(s2);
        જો (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        પાછા 0;
}

સ્થિર પૂર્ણાંક synthesize_user_creds(
                અચલ અક્ષર *username,
                UserCredsFlags flags,
                અક્ષર **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                અક્ષર **ret_home,
                અક્ષર **ret_shell) {
        પ્રતિજ્ઞા(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        જો (STR_IN_SET(username, "root", "0"))
                પાછા return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_મૂળ_shell(ખાલી) : ખાલી,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        જો (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                પાછા return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ખાલી : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ખાલી : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        પાછા -ENOMEDIUM;
}

પૂર્ણાંક get_user_creds(
                અચલ અક્ષર *username,
                UserCredsFlags flags,
                અક્ષર **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                અક્ષર **ret_home,
                અક્ષર **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ સંરચના passwd *pw = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(username);
        પ્રતિજ્ઞા((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        જો (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                જો (r >= 0)
                        પાછા 0;
                જો (r != -ENOMEDIUM) /* not a username we can synthesize */
                        પાછા r;
        }

        જો (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                જો (r >= 0)
                        username = pw->pw_name;

                નહીંતર જો (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        જો (ret_username)
                                *ret_username = ખાલી;
                        જો (ret_uid)
                                *ret_uid = u;
                        પાછા 0;
                }
        } નહીંતર
                r = getpwnam_malloc(username, &pw);

        જો (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                જો (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        પાછા 0;

                પાછા r;
        }

        જો (ret_uid && !uid_is_valid(pw->pw_uid))
                પાછા -EBADMSG;

        જો (ret_gid && !gid_is_valid(pw->pw_gid))
                પાછા -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        અચલ અક્ષર *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_મૂળ(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? ખાલી : pw->pw_dir;

        અચલ અક્ષર *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? ખાલી : pw->pw_shell;

        પાછા return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

સ્થિર પૂર્ણાંક synthesize_group_creds(અચલ અક્ષર *groupname, અક્ષર **ret_name, gid_t *ret_gid) {
        પ્રતિજ્ઞા(groupname);

        gid_t id;
        અચલ અક્ષર *n;
        પૂર્ણાંક r;

        જો (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } નહીંતર જો (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } નહીંતર
                પાછા -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        જો (r < 0)
                પાછા r;
        જો (ret_gid)
                *ret_gid = id;
        પાછા 0;
}

પૂર્ણાંક get_group_creds(અચલ અક્ષર *groupname, UserCredsFlags flags, અક્ષર **ret_name, gid_t *ret_gid) {
        _cleanup_free_ સંરચના group *gr = ખાલી;
        gid_t id;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(groupname);

        જો (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                જો (r >= 0)
                        પાછા 0;
                જો (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        પાછા r;
        }

        જો (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                જો (r >= 0)
                        groupname = gr->gr_name;
                નહીંતર જો (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        જો (ret_gid)
                                *ret_gid = id;
                        જો (ret_name)
                                *ret_name = ખાલી;
                        પાછા 0;
                }
        } નહીંતર
                r = getgrnam_malloc(groupname, &gr);

        જો (r < 0) {
                જો (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        પાછા 0;
                પાછા r;
        }

        જો (ret_gid && !gid_is_valid(gr->gr_gid))
                પાછા -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        જો (r < 0)
                પાછા r;
        જો (ret_gid)
                *ret_gid = gr->gr_gid;
        પાછા 0;
}

અક્ષર* uid_to_name(uid_t uid) {
        અક્ષર *ret;
        પૂર્ણાંક r;

        /* Shortcut things to avoid NSS lookups */
        જો (uid == 0)
                પાછા strdup("root");
        જો (uid == UID_NOBODY && synthesize_nobody())
                પાછા strdup(NOBODY_USER_NAME);

        જો (uid_is_valid(uid)) {
                _cleanup_free_ સંરચના passwd *pw = ખાલી;

                r = getpwuid_malloc(uid, &pw);
                જો (r >= 0)
                        પાછા strdup(pw->pw_name);
        }

        જો (asprintf(&ret, UID_FMT, uid) < 0)
                પાછા ખાલી;

        પાછા ret;
}

અક્ષર* gid_to_name(gid_t gid) {
        અક્ષર *ret;
        પૂર્ણાંક r;

        જો (gid == 0)
                પાછા strdup("root");
        જો (gid == GID_NOBODY && synthesize_nobody())
                પાછા strdup(NOBODY_GROUP_NAME);

        જો (gid_is_valid(gid)) {
                _cleanup_free_ સંરચના group *gr = ખાલી;

                r = getgrgid_malloc(gid, &gr);
                જો (r >= 0)
                        પાછા strdup(gr->gr_name);
        }

        જો (asprintf(&ret, GID_FMT, gid) < 0)
                પાછા ખાલી;

        પાછા ret;
}

સ્થિર bool gid_list_has(અચલ gid_t *list, size_t size, gid_t val) {
        પ્રતિજ્ઞા(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                જો (*i == val)
                        પાછા true;

        પાછા false;
}

પૂર્ણાંક in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ખાલી;
        પૂર્ણાંક ngroups;

        જો (getgid() == gid)
                પાછા 1;

        જો (getegid() == gid)
                પાછા 1;

        જો (!gid_is_valid(gid))
                પાછા -EINVAL;

        ngroups = getgroups_alloc(&gids);
        જો (ngroups < 0)
                પાછા ngroups;

        પાછા gid_list_has(gids, ngroups, gid);
}

પૂર્ણાંક in_group(અચલ અક્ષર *name) {
        પૂર્ણાંક r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ ખાલી, &gid);
        જો (r < 0)
                પાછા r;

        પાછા in_gid(gid);
}

પૂર્ણાંક merge_gid_lists(અચલ gid_t *list1, size_t size1, અચલ gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        પ્રતિજ્ઞા(size1 == 0 || list1);
        પ્રતિજ્ઞા(size2 == 0 || list2);
        પ્રતિજ્ઞા(ret);

        જો (size2 > INT_MAX - size1)
                પાછા -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        જો (!buf)
                પાછા -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        માટે (size_t i = 0; i < size1; i++)
                જો (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        માટે (size_t i = 0; i < size2; i++)
                જો (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        પાછા (પૂર્ણાંક)nresult;
}

પૂર્ણાંક getgroups_alloc(gid_t **ret) {
        પૂર્ણાંક ngroups = 8;

        પ્રતિજ્ઞા(ret);

        માટે (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = ખાલી;

                p = new(gid_t, ngroups);
                જો (!p)
                        પાછા -ENOMEM;

                ngroups = getgroups(ngroups, p);
                જો (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        પાછા ngroups;
                }
                જો (ngroups == 0)
                        તોડો;
                જો (errno != EINVAL)
                        પાછા -errno;

                /* Give up eventually */
                જો (attempt++ > 10)
                        પાછા -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ખાલી);
                જો (ngroups < 0)
                        પાછા -errno;
                જો (ngroups == 0)
                        તોડો;
        }

        *ret = ખાલી;
        પાછા 0;
}

પૂર્ણાંક get_home_dir(અક્ષર **ret) {
        _cleanup_free_ સંરચના passwd *p = ખાલી;
        અચલ અક્ષર *e;
        uid_t u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        જો (e && path_is_valid(e) && path_is_absolute(e))
                જાઓ found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        જો (u == 0) {
                e = "/root";
                જાઓ found;
        }
        જો (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                જાઓ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        જો (r < 0)
                પાછા r;

        e = p->pw_dir;
        જો (!path_is_valid(e) || !path_is_absolute(e))
                પાછા -EINVAL;

 found:
        પાછા path_simplify_alloc(e, ret);
}

પૂર્ણાંક get_shell(અક્ષર **ret) {
        _cleanup_free_ સંરચના passwd *p = ખાલી;
        અચલ અક્ષર *e;
        uid_t u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        જો (e && path_is_valid(e) && path_is_absolute(e))
                જાઓ found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        જો (u == 0) {
                e = default_મૂળ_shell(ખાલી);
                જાઓ found;
        }
        જો (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                જાઓ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        જો (r < 0)
                પાછા r;

        e = p->pw_shell;
        જો (!path_is_valid(e) || !path_is_absolute(e))
                પાછા -EINVAL;

 found:
        પાછા path_simplify_alloc(e, ret);
}

પૂર્ણાંક fully_set_uid_gid(uid_t uid, gid_t gid, અચલ gid_t supplementary_gids[], size_t n_supplementary_gids) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        જો (r < 0)
                પાછા r;

        જો (gid_is_valid(gid))
                જો (setresgid(gid, gid, gid) < 0)
                        પાછા -errno;

        જો (uid_is_valid(uid))
                જો (setresuid(uid, uid, uid) < 0)
                        પાછા -errno;

        પાછા 0;
}

પૂર્ણાંક take_etc_passwd_lock(અચલ અક્ષર *મૂળ) {
        પૂર્ણાંક r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ અક્ષર *path = path_join(મૂળ, ETC_PASSWD_LOCK_PATH);
        જો (!path)
                પાછા log_oom_debug();

        (શૂન્ય) mkdir_parents(path, 0755);

        _cleanup_close_ પૂર્ણાંક fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        જો (fd < 0)
                પાછા log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        જો (r < 0)
                પાછા log_debug_errno(r, "Locking %s failed: %m", path);

        પાછા TAKE_FD(fd);
}

bool valid_user_group_name(અચલ અક્ષર *u, ValidUserFlags flags) {
        અચલ અક્ષર *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        જો (isempty(u)) /* An empty user name is never valid */
                પાછા false;

        જો (parse_uid(u, ખાલી) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                પાછા FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        જો (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                જો (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        પાછા false;

                જો (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        પાછા false;

                જો (string_has_cc(u, ખાલી)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        પાછા false;

                જો (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        પાછા false;

                જો (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        પાછા false;

                જો (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        પાછા false;

                જો (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        પાછા false;

                /* Compare with strict result and warn if result doesn't match */
                જો (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } નહીંતર {
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

                જો (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        પાછા false;

                માટે (i = u+1; *i; i++)
                        જો (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                પાછા false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                જો (l > (size_t) sz) /* glibc: 256 */
                        પાછા false;
                જો (l > NAME_MAX) /* must fit in a filename: 255 */
                        પાછા false;
                જો (l > sizeof_field(સંરચના utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        પાછા false;
        }

        પાછા true;
}

bool valid_gecos(અચલ અક્ષર *d) {

        જો (!d)
                પાછા false;

        જો (!utf8_is_valid(d))
                પાછા false;

        જો (string_has_cc(d, ખાલી))
                પાછા false;

        /* Colons are used as field separators, and hence not OK */
        જો (strchr(d, ':'))
                પાછા false;

        પાછા true;
}

અક્ષર* mangle_gecos(અચલ અક્ષર *d) {
        અક્ષર *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        જો (!mangled)
                પાછા ખાલી;

        માટે (અક્ષર *i = mangled; *i; i++) {
                પૂર્ણાંક len;

                જો ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        ચાલુ;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                જો (len < 0) {
                        *i = ' ';
                        ચાલુ;
                }

                i += len - 1;
        }

        પાછા mangled;
}

bool valid_home(અચલ અક્ષર *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        જો (isempty(p))
                પાછા false;

        જો (!utf8_is_valid(p))
                પાછા false;

        જો (string_has_cc(p, ખાલી))
                પાછા false;

        જો (!path_is_absolute(p))
                પાછા false;

        જો (!path_is_normalized(p))
                પાછા false;

        /* Colons are used as field separators, and hence not OK */
        જો (strchr(p, ':'))
                પાછા false;

        પાછા true;
}

bool valid_shell(અચલ અક્ષર *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        જો (!valid_home(p))
                પાછા false;

        પાછા !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

પૂર્ણાંક maybe_setgroups(size_t size, અચલ gid_t *list) {
        પૂર્ણાંક r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        જો (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                જો (!f && errno != ENOENT)
                        પાછા -errno;
                જો (f) {
                        r = safe_fgetc(f, /* ret= */ ખાલી);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                પાછા 0;
                        }
                }

                _cleanup_free_ અક્ષર *setgroups_content = ખાલી;
                r = read_one_લીટી_file("/proc/self/setgroups", &setgroups_content);
                જો (r < 0 && r != -ENOENT)
                        પાછા r;
                જો (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        પાછા 0;
                }
        }

        પાછા RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(શૂન્ય) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        સ્થિર પૂર્ણાંક cache = -1;

        જો (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        પાછા cache;
}

પૂર્ણાંક putpwent_sane(અચલ સંરચના passwd *pw, FILE *stream) {
        પ્રતિજ્ઞા(pw);
        પ્રતિજ્ઞા(stream);

        errno = 0;
        જો (putpwent(pw, stream) != 0)
                પાછા errno_or_else(EIO);

        પાછા 0;
}

પૂર્ણાંક putspent_sane(અચલ સંરચના spwd *sp, FILE *stream) {
        પ્રતિજ્ઞા(sp);
        પ્રતિજ્ઞા(stream);

        errno = 0;
        જો (putspent(sp, stream) != 0)
                પાછા errno_or_else(EIO);

        પાછા 0;
}

પૂર્ણાંક putgrent_sane(અચલ સંરચના group *gr, FILE *stream) {
        પ્રતિજ્ઞા(gr);
        પ્રતિજ્ઞા(stream);

        errno = 0;
        જો (putgrent(gr, stream) != 0)
                પાછા errno_or_else(EIO);

        પાછા 0;
}

#જો ENABLE_GSHADOW
પૂર્ણાંક putsgent_sane(અચલ સંરચના sgrp *sg, FILE *stream) {
        પ્રતિજ્ઞા(sg);
        પ્રતિજ્ઞા(stream);

        errno = 0;
        જો (putsgent(sg, stream) != 0)
                પાછા errno_or_else(EIO);

        પાછા 0;
}
#endif

પૂર્ણાંક fgetpwent_sane(FILE *stream, સંરચના passwd **pw) {
        પ્રતિજ્ઞા(stream);
        પ્રતિજ્ઞા(pw);

        errno = 0;
        સંરચના passwd *p = fgetpwent(stream);
        જો (!p && !IN_SET(errno, 0, ENOENT))
                પાછા -errno;

        *pw = p;
        પાછા !!p;
}

પૂર્ણાંક fgetspent_sane(FILE *stream, સંરચના spwd **sp) {
        પ્રતિજ્ઞા(stream);
        પ્રતિજ્ઞા(sp);

        errno = 0;
        સંરચના spwd *s = fgetspent(stream);
        જો (!s && !IN_SET(errno, 0, ENOENT))
                પાછા -errno;

        *sp = s;
        પાછા !!s;
}

પૂર્ણાંક fgetgrent_sane(FILE *stream, સંરચના group **gr) {
        પ્રતિજ્ઞા(stream);
        પ્રતિજ્ઞા(gr);

        errno = 0;
        સંરચના group *g = fgetgrent(stream);
        જો (!g && !IN_SET(errno, 0, ENOENT))
                પાછા -errno;

        *gr = g;
        પાછા !!g;
}

#જો ENABLE_GSHADOW
પૂર્ણાંક fgetsgent_sane(FILE *stream, સંરચના sgrp **sg) {
        પ્રતિજ્ઞા(stream);
        પ્રતિજ્ઞા(sg);

        errno = 0;
        સંરચના sgrp *s = fgetsgent(stream);
        જો (!s && !IN_SET(errno, 0, ENOENT))
                પાછા -errno;

        *sg = s;
        પાછા !!s;
}
#endif

પૂર્ણાંક is_this_me(અચલ અક્ષર *username) {
        uid_t uid;
        પૂર્ણાંક r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ખાલી, &uid, ખાલી, ખાલી, ખાલી);
        જો (r < 0)
                પાછા r;

        પાછા uid == getuid();
}

અચલ અક્ષર* get_home_મૂળ(શૂન્ય) {
        /* For debug purposes allow overriding where we look for home dirs */
        અચલ અક્ષર *e = secure_getenv("SYSTEMD_HOME_ROOT");
        જો (e && path_is_absolute(e) && path_is_normalized(e))
                પાછા e;

        પાછા "/home";
}

સ્થિર પૂર્ણાંક copy_struct_passwd(અચલ સંરચના passwd *pw, સંરચના passwd **ret) {
        પ્રતિજ્ઞા(pw);
        પ્રતિજ્ઞા(ret);

        size_t need_bytes = કદ(સંરચના passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        અક્ષર *buf = malloc(need_bytes);
        જો (!buf)
                પાછા -ENOMEM;

        સંરચના passwd *newpw = (સંરચના passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        અક્ષર *p = buf + કદ(સંરચના passwd);

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
        પાછા 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
પૂર્ણાંક lookup_pwent_in_files(
                અક્ષર * અચલ *files,
                અચલ અક્ષર *name,
                uid_t uid,
                સંરચના passwd **ret) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(files);
        પ્રતિજ્ઞા(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ખાલી;
                સંરચના passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                જો (r == -ENOENT)
                        ચાલુ;
                જો (r < 0)
                        પાછા r;

                જ્યારે ((r = fgetpwent_sane(f, &pw)) > 0) {
                        જો (name && !streq_ptr(pw->pw_name, name))
                                ચાલુ;
                        જો (uid_is_valid(uid) && pw->pw_uid != uid)
                                ચાલુ;
                        જો (ret)
                                પાછા copy_struct_passwd(pw, ret);
                        પાછા 0;
                }
                જો (r < 0)
                        પાછા r;
        }

        પાછા -ESRCH;
}

#જો !BUILD_STATIC

સ્થિર size_t getpw_buffer_size(શૂન્ય) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        પાછા bufsize <= 0 ? 4096U : (size_t) bufsize;
}

સ્થિર bool errno_is_user_doesnt_exist(પૂર્ણાંક error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        પાછા IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

પૂર્ણાંક getpwnam_malloc(અચલ અક્ષર *name, સંરચના passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        જો (isempty(name))
                પાછા -EINVAL;

#જો BUILD_STATIC
        પાછા lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#નહીંતર
        size_t bufsize = getpw_buffer_size();
        પૂર્ણાંક r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        માટે (;;) {
                _cleanup_free_ શૂન્ય *buf = ખાલી;

                /* Silence static analyzers */
                પ્રતિજ્ઞા(bufsize <= SIZE_MAX - ALIGN(કદ(સંરચના passwd)));
                buf = malloc0(ALIGN(કદ(સંરચના passwd)) + bufsize);
                જો (!buf)
                        પાછા -ENOMEM;

                સંરચના passwd *pw = ખાલી;
                r = getpwnam_r(name, buf, (અક્ષર*) buf + ALIGN(કદ(સંરચના passwd)), bufsize, &pw);
                જો (r == 0) {
                        જો (pw) {
                                જો (ret)
                                        *ret = TAKE_PTR(buf);
                                પાછા 0;
                        }

                        પાછા -ESRCH;
                }

                પ્રતિજ્ઞા(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                જો (errno_is_user_doesnt_exist(r))
                        પાછા -ESRCH;
                જો (r != ERANGE)
                        પાછા -r;

                જો (bufsize > SIZE_MAX/2 - ALIGN(કદ(સંરચના passwd)))
                        પાછા -ENOMEM;
                bufsize *= 2;
        }
#endif
}

પૂર્ણાંક getpwuid_malloc(uid_t uid, સંરચના passwd **ret) {
        જો (!uid_is_valid(uid))
                પાછા -EINVAL;

#જો BUILD_STATIC
        પાછા lookup_pwent_in_files(PASSWD_FILES, /* name= */ ખાલી, uid, ret);
#નહીંતર
        size_t bufsize = getpw_buffer_size();
        પૂર્ણાંક r;

        માટે (;;) {
                _cleanup_free_ શૂન્ય *buf = ખાલી;

                /* Silence static analyzers */
                પ્રતિજ્ઞા(bufsize <= SIZE_MAX - ALIGN(કદ(સંરચના passwd)));
                buf = malloc0(ALIGN(કદ(સંરચના passwd)) + bufsize);
                જો (!buf)
                        પાછા -ENOMEM;

                સંરચના passwd *pw = ખાલી;
                r = getpwuid_r(uid, buf, (અક્ષર*) buf + ALIGN(કદ(સંરચના passwd)), bufsize, &pw);
                જો (r == 0) {
                        જો (pw) {
                                જો (ret)
                                        *ret = TAKE_PTR(buf);
                                પાછા 0;
                        }

                        પાછા -ESRCH;
                }

                પ્રતિજ્ઞા(r > 0);

                જો (errno_is_user_doesnt_exist(r))
                        પાછા -ESRCH;
                જો (r != ERANGE)
                        પાછા -r;

                જો (bufsize > SIZE_MAX/2 - ALIGN(કદ(સંરચના passwd)))
                        પાછા -ENOMEM;
                bufsize *= 2;
        }
#endif
}

સ્થિર પૂર્ણાંક copy_struct_group(અચલ સંરચના group *gr, સંરચના group **ret) {
        પ્રતિજ્ઞા(gr);
        પ્રતિજ્ઞા(ret);

        size_t need_bytes = કદ(સંરચના group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += કદ(અક્ષર*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += કદ(અક્ષર*);  /* NULL terminator for gr_mem */

        અક્ષર *buf = malloc(need_bytes);
        જો (!buf)
                પાછા -ENOMEM;

        સંરચના group *newgr = (સંરચના group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(કદ(સંરચના group) % alignof(અક્ષર*) == 0);

        અક્ષર *p = buf + કદ(સંરચના group) + (n_mem + 1) * કદ(અક્ષર*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (અક્ષર**) (buf + કદ(સંરચના group));
        માટે (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ખાલી;

        *ret = newgr;
        પાછા 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
પૂર્ણાંક lookup_grent_in_files(
                અક્ષર * અચલ *files,
                અચલ અક્ષર *name,
                gid_t gid,
                સંરચના group **ret) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(files);
        પ્રતિજ્ઞા(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ખાલી;
                સંરચના group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                જો (r == -ENOENT)
                        ચાલુ;
                જો (r < 0)
                        પાછા r;

                જ્યારે ((r = fgetgrent_sane(f, &gr)) > 0) {
                        જો (name && !streq_ptr(gr->gr_name, name))
                                ચાલુ;
                        જો (gid_is_valid(gid) && gr->gr_gid != gid)
                                ચાલુ;
                        જો (ret)
                                પાછા copy_struct_group(gr, ret);
                        પાછા 0;
                }
                જો (r < 0)
                        પાછા r;
        }

        પાછા -ESRCH;
}

પૂર્ણાંક sysconf_ngroups_max(શૂન્ય) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        જો (ngroups_max <= 0)
                પાછા errno_or_else(EOPNOTSUPP);
        જો (ngroups_max > INT_MAX)
                પાછા -ERANGE;
        પાછા ngroups_max;
}

ssize_t lookup_groups_in_files(
                અક્ષર * અચલ *files,
                અચલ અક્ષર *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ખાલી;
        ssize_t n_arr = 0;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(files);
        પ્રતિજ્ઞા(name);
        પ્રતિજ્ઞા(gid_is_valid(gid));

        પૂર્ણાંક ngroups_max = sysconf_ngroups_max();
        જો (ngroups_max < 0)
                પાછા ngroups_max;

        જો (!GREEDY_REALLOC(arr, n_arr + 1))
                પાછા -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ખાલી;
                સંરચના group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                જો (r == -ENOENT)
                        ચાલુ;
                જો (r < 0)
                        પાછા r;

                જ્યારે ((r = fgetgrent_sane(f, &gr)) > 0) {
                        જો (gid == gr->gr_gid)
                                ચાલુ;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        જો (gid_list_has(arr, n_arr, gr->gr_gid))
                                ચાલુ;

                        જો (!strv_contains(gr->gr_mem, name))
                                ચાલુ;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        જો (n_arr >= ngroups_max)
                                પાછા -ENOBUFS;

                        જો (!GREEDY_REALLOC(arr, n_arr + 1))
                                પાછા -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                જો (r < 0)
                        પાછા r;
        }

        જો (ret)
                *ret = TAKE_PTR(arr);
        પાછા n_arr;
}

પૂર્ણાંક getgrouplist_malloc(અચલ અક્ષર *user, gid_t gid, gid_t **ret) {
#જો BUILD_STATIC
        પાછા lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#નહીંતર
        પૂર્ણાંક ngroups_max = sysconf_ngroups_max();
        જો (ngroups_max < 0)
                પાછા ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        જો (!gids)
                પાછા -ENOMEM;

        પૂર્ણાંક k = ngroups_max;
        જો (getgrouplist(user, gid, gids, &k) < 0)
                પાછા -EIO;  /* getgrouplist does not set errno */

        જો (ret)
                *ret = TAKE_PTR(gids);
        પાછા k;
#endif
}

પૂર્ણાંક initgroups_wrapper(અચલ અક્ષર *user, gid_t gid) {
#જો BUILD_STATIC
        _cleanup_free_ gid_t *groups = ખાલી;
        પૂર્ણાંક r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        જો (n_groups <= 0)
                પાછા n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        માટે (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                જો (r != -EINVAL)
                        તોડો;
        }

        પાછા r;
#નહીંતર
        પાછા RET_NERRNO(initgroups(user, gid));
#endif
}

#જો !BUILD_STATIC

સ્થિર size_t getgr_buffer_size(શૂન્ય) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        પાછા bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

પૂર્ણાંક getgrnam_malloc(અચલ અક્ષર *name, સંરચના group **ret) {
        જો (isempty(name))
                પાછા -EINVAL;

#જો BUILD_STATIC
        પાછા lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#નહીંતર
        size_t bufsize = getgr_buffer_size();
        પૂર્ણાંક r;

        માટે (;;) {
                _cleanup_free_ શૂન્ય *buf = ખાલી;

                /* Silence static analyzers */
                પ્રતિજ્ઞા(bufsize <= SIZE_MAX - ALIGN(કદ(સંરચના group)));
                buf = malloc0(ALIGN(કદ(સંરચના group)) + bufsize);
                જો (!buf)
                        પાછા -ENOMEM;

                સંરચના group *gr = ખાલી;
                r = getgrnam_r(name, buf, (અક્ષર*) buf + ALIGN(કદ(સંરચના group)), bufsize, &gr);
                જો (r == 0) {
                        જો (gr) {
                                જો (ret)
                                        *ret = TAKE_PTR(buf);
                                પાછા 0;
                        }

                        પાછા -ESRCH;
                }

                પ્રતિજ્ઞા(r > 0);

                જો (errno_is_user_doesnt_exist(r))
                        પાછા -ESRCH;
                જો (r != ERANGE)
                        પાછા -r;

                જો (bufsize > SIZE_MAX/2 - ALIGN(કદ(સંરચના group)))
                        પાછા -ENOMEM;
                bufsize *= 2;
        }
#endif
}

પૂર્ણાંક getgrgid_malloc(gid_t gid, સંરચના group **ret) {
        જો (!gid_is_valid(gid))
                પાછા -EINVAL;

#જો BUILD_STATIC
        પાછા lookup_grent_in_files(GROUP_FILES, /* name= */ ખાલી, gid, ret);
#નહીંતર
        size_t bufsize = getgr_buffer_size();
        પૂર્ણાંક r;

        માટે (;;) {
                _cleanup_free_ શૂન્ય *buf = ખાલી;

                /* Silence static analyzers */
                પ્રતિજ્ઞા(bufsize <= SIZE_MAX - ALIGN(કદ(સંરચના group)));
                buf = malloc0(ALIGN(કદ(સંરચના group)) + bufsize);
                જો (!buf)
                        પાછા -ENOMEM;

                સંરચના group *gr = ખાલી;
                r = getgrgid_r(gid, buf, (અક્ષર*) buf + ALIGN(કદ(સંરચના group)), bufsize, &gr);
                જો (r == 0) {
                        જો (gr) {
                                જો (ret)
                                        *ret = TAKE_PTR(buf);
                                પાછા 0;
                        }

                        પાછા -ESRCH;
                }

                પ્રતિજ્ઞા(r > 0);

                જો (errno_is_user_doesnt_exist(r))
                        પાછા -ESRCH;
                જો (r != ERANGE)
                        પાછા -r;

                જો (bufsize > SIZE_MAX/2 - ALIGN(કદ(સંરચના group)))
                        પાછા -ENOMEM;
                bufsize *= 2;
        }
#endif
}
