/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/file.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <utmpx.h>

#অন্তর্ভাব "sd-messages.h"

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "lock-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mkdir.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "terminal-util.h"
#অন্তর্ভাব "user-util.h"
#অন্তর্ভাব "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        ধ্রুব বর্ণ* strerror_##type(                                    \
                        পূর্ণাঙ্ক errnum,                                     \
                        বর্ণ *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                বিকল্প (errnum) {                                       \
                পক্ষ ESRCH:                                             \
                        প্রত্যাগম "Unknown " STRINGIFY(type);              \
                পক্ষ ENOEXEC:                                           \
                        প্রত্যাগম "Not a system " STRINGIFY(type);         \
                স্বভাব:                                                \
                        প্রত্যাগম strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        যদি (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                প্রত্যাগম false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        যদি (uid == (uid_t) UINT32_C(0xFFFF))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

পূর্ণাঙ্ক parse_uid(ধ্রুব বর্ণ *s, uid_t *ret) {
        uint32_t uid = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);

        assert_cc(পরিমাণ(uid_t) == পরিমাণ(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (!uid_is_valid(uid))
                প্রত্যাগম -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        যদি (ret)
                *ret = uid;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক parse_uid_range(ধ্রুব বর্ণ *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ বর্ণ *word = রিক্ত;
        uid_t l, u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret_lower);
        প্রতিজ্ঞা(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0)
                প্রত্যাগম -EINVAL;

        r = parse_uid(word, &l);
        যদি (r < 0)
                প্রত্যাগম r;

        /* Check for the upper bound and extract it if needed */
        যদি (!s)
                /* Single number with no dash. */
                u = l;
        অন্যথা যদি (!*s)
                /* Trailing dash is an error. */
                প্রত্যাগম -EINVAL;
        অন্যথা {
                r = parse_uid(s, &u);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (l > u)
                        প্রত্যাগম -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        প্রত্যাগম 0;
}

বর্ণ* getlogname_malloc(শূন্য) {
        uid_t uid;
        সংরচনা stat st;

        যদি (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        অন্যথা
                uid = getuid();

        প্রত্যাগম uid_to_name(uid);
}

বর্ণ* getusername_malloc(শূন্য) {
        ধ্রুব বর্ণ *e;

        e = secure_getenv("USER");
        যদি (e)
                প্রত্যাগম strdup(e);

        প্রত্যাগম uid_to_name(getuid());
}

bool is_nologin_shell(ধ্রুব বর্ণ *shell) {
        প্রত্যাগম PATH_IN_SET(shell,
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

bool shell_is_placeholder(ধ্রুব বর্ণ *shell) {
        প্রত্যাগম isempty(shell) || is_nologin_shell(shell);
}

ধ্রুব বর্ণ* default_মূল_shell_at(পূর্ণাঙ্ক rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        প্রতিজ্ঞা(rfd >= 0 || rfd == AT_FDCWD);

        পূর্ণাঙ্ক r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, রিক্ত, রিক্ত);
        যদি (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        যদি (r > 0)
                প্রত্যাগম DEFAULT_USER_SHELL;

        প্রত্যাগম "/bin/sh";
}

ধ্রুব বর্ণ* default_মূল_shell(ধ্রুব বর্ণ *মূল) {
        _cleanup_close_ পূর্ণাঙ্ক rfd = -EBADF;

        rfd = open(empty_to_মূল(মূল), O_CLOEXEC | O_DIRECTORY | O_PATH);
        যদি (rfd < 0)
                প্রত্যাগম "/bin/sh";

        প্রত্যাগম default_মূল_shell_at(rfd);
}

স্থির পূর্ণাঙ্ক return_user_creds(
                ধ্রুব বর্ণ *username,
                uid_t uid, gid_t gid,
                ধ্রুব বর্ণ *home,
                ধ্রুব বর্ণ *shell,
                বর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                বর্ণ **ret_home,
                বর্ণ **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ বর্ণ *s1 = রিক্ত, *s2 = রিক্ত, *s3 = রিক্ত;
        পূর্ণাঙ্ক r;

        যদি (ret_username) {
                r = strdup_to(&s1, username);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_home) {
                r = strdup_to(&s2, home);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_shell) {
                r = strdup_to(&s3, shell);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_username)
                *ret_username = TAKE_PTR(s1);
        যদি (ret_uid)
                *ret_uid = uid;
        যদি (ret_gid)
                *ret_gid = gid;
        যদি (ret_home)
                *ret_home = TAKE_PTR(s2);
        যদি (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক synthesize_user_creds(
                ধ্রুব বর্ণ *username,
                UserCredsFlags flags,
                বর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                বর্ণ **ret_home,
                বর্ণ **ret_shell) {
        প্রতিজ্ঞা(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        যদি (STR_IN_SET(username, "root", "0"))
                প্রত্যাগম return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_মূল_shell(রিক্ত) : রিক্ত,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        যদি (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                প্রত্যাগম return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? রিক্ত : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? রিক্ত : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        প্রত্যাগম -ENOMEDIUM;
}

পূর্ণাঙ্ক get_user_creds(
                ধ্রুব বর্ণ *username,
                UserCredsFlags flags,
                বর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                বর্ণ **ret_home,
                বর্ণ **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ সংরচনা passwd *pw = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(username);
        প্রতিজ্ঞা((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        যদি (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                যদি (r >= 0)
                        প্রত্যাগম 0;
                যদি (r != -ENOMEDIUM) /* not a username we can synthesize */
                        প্রত্যাগম r;
        }

        যদি (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                যদি (r >= 0)
                        username = pw->pw_name;

                অন্যথা যদি (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        যদি (ret_username)
                                *ret_username = রিক্ত;
                        যদি (ret_uid)
                                *ret_uid = u;
                        প্রত্যাগম 0;
                }
        } অন্যথা
                r = getpwnam_malloc(username, &pw);

        যদি (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                যদি (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        প্রত্যাগম 0;

                প্রত্যাগম r;
        }

        যদি (ret_uid && !uid_is_valid(pw->pw_uid))
                প্রত্যাগম -EBADMSG;

        যদি (ret_gid && !gid_is_valid(pw->pw_gid))
                প্রত্যাগম -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ধ্রুব বর্ণ *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_মূল(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? রিক্ত : pw->pw_dir;

        ধ্রুব বর্ণ *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? রিক্ত : pw->pw_shell;

        প্রত্যাগম return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

স্থির পূর্ণাঙ্ক synthesize_group_creds(ধ্রুব বর্ণ *groupname, বর্ণ **ret_name, gid_t *ret_gid) {
        প্রতিজ্ঞা(groupname);

        gid_t id;
        ধ্রুব বর্ণ *n;
        পূর্ণাঙ্ক r;

        যদি (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } অন্যথা যদি (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } অন্যথা
                প্রত্যাগম -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (ret_gid)
                *ret_gid = id;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক get_group_creds(ধ্রুব বর্ণ *groupname, UserCredsFlags flags, বর্ণ **ret_name, gid_t *ret_gid) {
        _cleanup_free_ সংরচনা group *gr = রিক্ত;
        gid_t id;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(groupname);

        যদি (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                যদি (r >= 0)
                        প্রত্যাগম 0;
                যদি (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        প্রত্যাগম r;
        }

        যদি (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                যদি (r >= 0)
                        groupname = gr->gr_name;
                অন্যথা যদি (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        যদি (ret_gid)
                                *ret_gid = id;
                        যদি (ret_name)
                                *ret_name = রিক্ত;
                        প্রত্যাগম 0;
                }
        } অন্যথা
                r = getgrnam_malloc(groupname, &gr);

        যদি (r < 0) {
                যদি (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        প্রত্যাগম 0;
                প্রত্যাগম r;
        }

        যদি (ret_gid && !gid_is_valid(gr->gr_gid))
                প্রত্যাগম -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (ret_gid)
                *ret_gid = gr->gr_gid;
        প্রত্যাগম 0;
}

বর্ণ* uid_to_name(uid_t uid) {
        বর্ণ *ret;
        পূর্ণাঙ্ক r;

        /* Shortcut things to avoid NSS lookups */
        যদি (uid == 0)
                প্রত্যাগম strdup("root");
        যদি (uid == UID_NOBODY && synthesize_nobody())
                প্রত্যাগম strdup(NOBODY_USER_NAME);

        যদি (uid_is_valid(uid)) {
                _cleanup_free_ সংরচনা passwd *pw = রিক্ত;

                r = getpwuid_malloc(uid, &pw);
                যদি (r >= 0)
                        প্রত্যাগম strdup(pw->pw_name);
        }

        যদি (asprintf(&ret, UID_FMT, uid) < 0)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম ret;
}

বর্ণ* gid_to_name(gid_t gid) {
        বর্ণ *ret;
        পূর্ণাঙ্ক r;

        যদি (gid == 0)
                প্রত্যাগম strdup("root");
        যদি (gid == GID_NOBODY && synthesize_nobody())
                প্রত্যাগম strdup(NOBODY_GROUP_NAME);

        যদি (gid_is_valid(gid)) {
                _cleanup_free_ সংরচনা group *gr = রিক্ত;

                r = getgrgid_malloc(gid, &gr);
                যদি (r >= 0)
                        প্রত্যাগম strdup(gr->gr_name);
        }

        যদি (asprintf(&ret, GID_FMT, gid) < 0)
                প্রত্যাগম রিক্ত;

        প্রত্যাগম ret;
}

স্থির bool gid_list_has(ধ্রুব gid_t *list, size_t size, gid_t val) {
        প্রতিজ্ঞা(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                যদি (*i == val)
                        প্রত্যাগম true;

        প্রত্যাগম false;
}

পূর্ণাঙ্ক in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = রিক্ত;
        পূর্ণাঙ্ক ngroups;

        যদি (getgid() == gid)
                প্রত্যাগম 1;

        যদি (getegid() == gid)
                প্রত্যাগম 1;

        যদি (!gid_is_valid(gid))
                প্রত্যাগম -EINVAL;

        ngroups = getgroups_alloc(&gids);
        যদি (ngroups < 0)
                প্রত্যাগম ngroups;

        প্রত্যাগম gid_list_has(gids, ngroups, gid);
}

পূর্ণাঙ্ক in_group(ধ্রুব বর্ণ *name) {
        পূর্ণাঙ্ক r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ রিক্ত, &gid);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম in_gid(gid);
}

পূর্ণাঙ্ক merge_gid_lists(ধ্রুব gid_t *list1, size_t size1, ধ্রুব gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        প্রতিজ্ঞা(size1 == 0 || list1);
        প্রতিজ্ঞা(size2 == 0 || list2);
        প্রতিজ্ঞা(ret);

        যদি (size2 > INT_MAX - size1)
                প্রত্যাগম -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        প্রতি (size_t i = 0; i < size1; i++)
                যদি (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        প্রতি (size_t i = 0; i < size2; i++)
                যদি (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        প্রত্যাগম (পূর্ণাঙ্ক)nresult;
}

পূর্ণাঙ্ক getgroups_alloc(gid_t **ret) {
        পূর্ণাঙ্ক ngroups = 8;

        প্রতিজ্ঞা(ret);

        প্রতি (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = রিক্ত;

                p = new(gid_t, ngroups);
                যদি (!p)
                        প্রত্যাগম -ENOMEM;

                ngroups = getgroups(ngroups, p);
                যদি (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        প্রত্যাগম ngroups;
                }
                যদি (ngroups == 0)
                        ভঙ্গ;
                যদি (errno != EINVAL)
                        প্রত্যাগম -errno;

                /* Give up eventually */
                যদি (attempt++ > 10)
                        প্রত্যাগম -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, রিক্ত);
                যদি (ngroups < 0)
                        প্রত্যাগম -errno;
                যদি (ngroups == 0)
                        ভঙ্গ;
        }

        *ret = রিক্ত;
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক get_home_dir(বর্ণ **ret) {
        _cleanup_free_ সংরচনা passwd *p = রিক্ত;
        ধ্রুব বর্ণ *e;
        uid_t u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        যদি (e && path_is_valid(e) && path_is_absolute(e))
                গচ্ছ found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        যদি (u == 0) {
                e = "/root";
                গচ্ছ found;
        }
        যদি (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                গচ্ছ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        e = p->pw_dir;
        যদি (!path_is_valid(e) || !path_is_absolute(e))
                প্রত্যাগম -EINVAL;

 found:
        প্রত্যাগম path_simplify_alloc(e, ret);
}

পূর্ণাঙ্ক get_shell(বর্ণ **ret) {
        _cleanup_free_ সংরচনা passwd *p = রিক্ত;
        ধ্রুব বর্ণ *e;
        uid_t u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        যদি (e && path_is_valid(e) && path_is_absolute(e))
                গচ্ছ found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        যদি (u == 0) {
                e = default_মূল_shell(রিক্ত);
                গচ্ছ found;
        }
        যদি (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                গচ্ছ found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        যদি (r < 0)
                প্রত্যাগম r;

        e = p->pw_shell;
        যদি (!path_is_valid(e) || !path_is_absolute(e))
                প্রত্যাগম -EINVAL;

 found:
        প্রত্যাগম path_simplify_alloc(e, ret);
}

পূর্ণাঙ্ক fully_set_uid_gid(uid_t uid, gid_t gid, ধ্রুব gid_t supplementary_gids[], size_t n_supplementary_gids) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (gid_is_valid(gid))
                যদি (setresgid(gid, gid, gid) < 0)
                        প্রত্যাগম -errno;

        যদি (uid_is_valid(uid))
                যদি (setresuid(uid, uid, uid) < 0)
                        প্রত্যাগম -errno;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক take_etc_passwd_lock(ধ্রুব বর্ণ *মূল) {
        পূর্ণাঙ্ক r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ বর্ণ *path = path_join(মূল, ETC_PASSWD_LOCK_PATH);
        যদি (!path)
                প্রত্যাগম log_oom_debug();

        (শূন্য) mkdir_parents(path, 0755);

        _cleanup_close_ পূর্ণাঙ্ক fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        যদি (fd < 0)
                প্রত্যাগম log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Locking %s failed: %m", path);

        প্রত্যাগম TAKE_FD(fd);
}

bool valid_user_group_name(ধ্রুব বর্ণ *u, ValidUserFlags flags) {
        ধ্রুব বর্ণ *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        যদি (isempty(u)) /* An empty user name is never valid */
                প্রত্যাগম false;

        যদি (parse_uid(u, রিক্ত) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                প্রত্যাগম FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        যদি (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                যদি (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        প্রত্যাগম false;

                যদি (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        প্রত্যাগম false;

                যদি (string_has_cc(u, রিক্ত)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        প্রত্যাগম false;

                যদি (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        প্রত্যাগম false;

                যদি (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        প্রত্যাগম false;

                যদি (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        প্রত্যাগম false;

                যদি (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        প্রত্যাগম false;

                /* Compare with strict result and warn if result doesn't match */
                যদি (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } অন্যথা {
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

                যদি (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        প্রত্যাগম false;

                প্রতি (i = u+1; *i; i++)
                        যদি (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                প্রত্যাগম false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                যদি (l > (size_t) sz) /* glibc: 256 */
                        প্রত্যাগম false;
                যদি (l > NAME_MAX) /* must fit in a filename: 255 */
                        প্রত্যাগম false;
                যদি (l > sizeof_field(সংরচনা utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        প্রত্যাগম false;
        }

        প্রত্যাগম true;
}

bool valid_gecos(ধ্রুব বর্ণ *d) {

        যদি (!d)
                প্রত্যাগম false;

        যদি (!utf8_is_valid(d))
                প্রত্যাগম false;

        যদি (string_has_cc(d, রিক্ত))
                প্রত্যাগম false;

        /* Colons are used as field separators, and hence not OK */
        যদি (strchr(d, ':'))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

বর্ণ* mangle_gecos(ধ্রুব বর্ণ *d) {
        বর্ণ *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        যদি (!mangled)
                প্রত্যাগম রিক্ত;

        প্রতি (বর্ণ *i = mangled; *i; i++) {
                পূর্ণাঙ্ক len;

                যদি ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        অনুবর্ত;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                যদি (len < 0) {
                        *i = ' ';
                        অনুবর্ত;
                }

                i += len - 1;
        }

        প্রত্যাগম mangled;
}

bool valid_home(ধ্রুব বর্ণ *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        যদি (isempty(p))
                প্রত্যাগম false;

        যদি (!utf8_is_valid(p))
                প্রত্যাগম false;

        যদি (string_has_cc(p, রিক্ত))
                প্রত্যাগম false;

        যদি (!path_is_absolute(p))
                প্রত্যাগম false;

        যদি (!path_is_normalized(p))
                প্রত্যাগম false;

        /* Colons are used as field separators, and hence not OK */
        যদি (strchr(p, ':'))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

bool valid_shell(ধ্রুব বর্ণ *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        যদি (!valid_home(p))
                প্রত্যাগম false;

        প্রত্যাগম !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

পূর্ণাঙ্ক maybe_setgroups(size_t size, ধ্রুব gid_t *list) {
        পূর্ণাঙ্ক r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        যদি (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                যদি (!f && errno != ENOENT)
                        প্রত্যাগম -errno;
                যদি (f) {
                        r = safe_fgetc(f, /* ret= */ রিক্ত);
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                প্রত্যাগম 0;
                        }
                }

                _cleanup_free_ বর্ণ *setgroups_content = রিক্ত;
                r = read_one_সারি_file("/proc/self/setgroups", &setgroups_content);
                যদি (r < 0 && r != -ENOENT)
                        প্রত্যাগম r;
                যদি (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        প্রত্যাগম 0;
                }
        }

        প্রত্যাগম RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(শূন্য) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        স্থির পূর্ণাঙ্ক cache = -1;

        যদি (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        প্রত্যাগম cache;
}

পূর্ণাঙ্ক putpwent_sane(ধ্রুব সংরচনা passwd *pw, FILE *stream) {
        প্রতিজ্ঞা(pw);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putpwent(pw, stream) != 0)
                প্রত্যাগম errno_or_else(EIO);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক putspent_sane(ধ্রুব সংরচনা spwd *sp, FILE *stream) {
        প্রতিজ্ঞা(sp);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putspent(sp, stream) != 0)
                প্রত্যাগম errno_or_else(EIO);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক putgrent_sane(ধ্রুব সংরচনা group *gr, FILE *stream) {
        প্রতিজ্ঞা(gr);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putgrent(gr, stream) != 0)
                প্রত্যাগম errno_or_else(EIO);

        প্রত্যাগম 0;
}

#যদি ENABLE_GSHADOW
পূর্ণাঙ্ক putsgent_sane(ধ্রুব সংরচনা sgrp *sg, FILE *stream) {
        প্রতিজ্ঞা(sg);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putsgent(sg, stream) != 0)
                প্রত্যাগম errno_or_else(EIO);

        প্রত্যাগম 0;
}
#endif

পূর্ণাঙ্ক fgetpwent_sane(FILE *stream, সংরচনা passwd **pw) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(pw);

        errno = 0;
        সংরচনা passwd *p = fgetpwent(stream);
        যদি (!p && !IN_SET(errno, 0, ENOENT))
                প্রত্যাগম -errno;

        *pw = p;
        প্রত্যাগম !!p;
}

পূর্ণাঙ্ক fgetspent_sane(FILE *stream, সংরচনা spwd **sp) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(sp);

        errno = 0;
        সংরচনা spwd *s = fgetspent(stream);
        যদি (!s && !IN_SET(errno, 0, ENOENT))
                প্রত্যাগম -errno;

        *sp = s;
        প্রত্যাগম !!s;
}

পূর্ণাঙ্ক fgetgrent_sane(FILE *stream, সংরচনা group **gr) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(gr);

        errno = 0;
        সংরচনা group *g = fgetgrent(stream);
        যদি (!g && !IN_SET(errno, 0, ENOENT))
                প্রত্যাগম -errno;

        *gr = g;
        প্রত্যাগম !!g;
}

#যদি ENABLE_GSHADOW
পূর্ণাঙ্ক fgetsgent_sane(FILE *stream, সংরচনা sgrp **sg) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(sg);

        errno = 0;
        সংরচনা sgrp *s = fgetsgent(stream);
        যদি (!s && !IN_SET(errno, 0, ENOENT))
                প্রত্যাগম -errno;

        *sg = s;
        প্রত্যাগম !!s;
}
#endif

পূর্ণাঙ্ক is_this_me(ধ্রুব বর্ণ *username) {
        uid_t uid;
        পূর্ণাঙ্ক r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, রিক্ত, &uid, রিক্ত, রিক্ত, রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম uid == getuid();
}

ধ্রুব বর্ণ* get_home_মূল(শূন্য) {
        /* For debug purposes allow overriding where we look for home dirs */
        ধ্রুব বর্ণ *e = secure_getenv("SYSTEMD_HOME_ROOT");
        যদি (e && path_is_absolute(e) && path_is_normalized(e))
                প্রত্যাগম e;

        প্রত্যাগম "/home";
}

স্থির পূর্ণাঙ্ক copy_struct_passwd(ধ্রুব সংরচনা passwd *pw, সংরচনা passwd **ret) {
        প্রতিজ্ঞা(pw);
        প্রতিজ্ঞা(ret);

        size_t need_bytes = পরিমাণ(সংরচনা passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        বর্ণ *buf = malloc(need_bytes);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        সংরচনা passwd *newpw = (সংরচনা passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        বর্ণ *p = buf + পরিমাণ(সংরচনা passwd);

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
        প্রত্যাগম 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
পূর্ণাঙ্ক lookup_pwent_in_files(
                বর্ণ * ধ্রুব *files,
                ধ্রুব বর্ণ *name,
                uid_t uid,
                সংরচনা passwd **ret) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = রিক্ত;
                সংরচনা passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        অনুবর্ত;
                যদি (r < 0)
                        প্রত্যাগম r;

                যাবত্ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        যদি (name && !streq_ptr(pw->pw_name, name))
                                অনুবর্ত;
                        যদি (uid_is_valid(uid) && pw->pw_uid != uid)
                                অনুবর্ত;
                        যদি (ret)
                                প্রত্যাগম copy_struct_passwd(pw, ret);
                        প্রত্যাগম 0;
                }
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম -ESRCH;
}

#যদি !BUILD_STATIC

স্থির size_t getpw_buffer_size(শূন্য) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        প্রত্যাগম bufsize <= 0 ? 4096U : (size_t) bufsize;
}

স্থির bool errno_is_user_doesnt_exist(পূর্ণাঙ্ক error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        প্রত্যাগম IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

পূর্ণাঙ্ক getpwnam_malloc(ধ্রুব বর্ণ *name, সংরচনা passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        যদি (isempty(name))
                প্রত্যাগম -EINVAL;

#যদি BUILD_STATIC
        প্রত্যাগম lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#অন্যথা
        size_t bufsize = getpw_buffer_size();
        পূর্ণাঙ্ক r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        প্রতি (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা passwd)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা passwd)) + bufsize);
                যদি (!buf)
                        প্রত্যাগম -ENOMEM;

                সংরচনা passwd *pw = রিক্ত;
                r = getpwnam_r(name, buf, (বর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা passwd)), bufsize, &pw);
                যদি (r == 0) {
                        যদি (pw) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                প্রত্যাগম 0;
                        }

                        প্রত্যাগম -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                যদি (errno_is_user_doesnt_exist(r))
                        প্রত্যাগম -ESRCH;
                যদি (r != ERANGE)
                        প্রত্যাগম -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা passwd)))
                        প্রত্যাগম -ENOMEM;
                bufsize *= 2;
        }
#endif
}

পূর্ণাঙ্ক getpwuid_malloc(uid_t uid, সংরচনা passwd **ret) {
        যদি (!uid_is_valid(uid))
                প্রত্যাগম -EINVAL;

#যদি BUILD_STATIC
        প্রত্যাগম lookup_pwent_in_files(PASSWD_FILES, /* name= */ রিক্ত, uid, ret);
#অন্যথা
        size_t bufsize = getpw_buffer_size();
        পূর্ণাঙ্ক r;

        প্রতি (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা passwd)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা passwd)) + bufsize);
                যদি (!buf)
                        প্রত্যাগম -ENOMEM;

                সংরচনা passwd *pw = রিক্ত;
                r = getpwuid_r(uid, buf, (বর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা passwd)), bufsize, &pw);
                যদি (r == 0) {
                        যদি (pw) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                প্রত্যাগম 0;
                        }

                        প্রত্যাগম -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        প্রত্যাগম -ESRCH;
                যদি (r != ERANGE)
                        প্রত্যাগম -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা passwd)))
                        প্রত্যাগম -ENOMEM;
                bufsize *= 2;
        }
#endif
}

স্থির পূর্ণাঙ্ক copy_struct_group(ধ্রুব সংরচনা group *gr, সংরচনা group **ret) {
        প্রতিজ্ঞা(gr);
        প্রতিজ্ঞা(ret);

        size_t need_bytes = পরিমাণ(সংরচনা group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += পরিমাণ(বর্ণ*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += পরিমাণ(বর্ণ*);  /* NULL terminator for gr_mem */

        বর্ণ *buf = malloc(need_bytes);
        যদি (!buf)
                প্রত্যাগম -ENOMEM;

        সংরচনা group *newgr = (সংরচনা group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(পরিমাণ(সংরচনা group) % alignof(বর্ণ*) == 0);

        বর্ণ *p = buf + পরিমাণ(সংরচনা group) + (n_mem + 1) * পরিমাণ(বর্ণ*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (বর্ণ**) (buf + পরিমাণ(সংরচনা group));
        প্রতি (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = রিক্ত;

        *ret = newgr;
        প্রত্যাগম 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
পূর্ণাঙ্ক lookup_grent_in_files(
                বর্ণ * ধ্রুব *files,
                ধ্রুব বর্ণ *name,
                gid_t gid,
                সংরচনা group **ret) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = রিক্ত;
                সংরচনা group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        অনুবর্ত;
                যদি (r < 0)
                        প্রত্যাগম r;

                যাবত্ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        যদি (name && !streq_ptr(gr->gr_name, name))
                                অনুবর্ত;
                        যদি (gid_is_valid(gid) && gr->gr_gid != gid)
                                অনুবর্ত;
                        যদি (ret)
                                প্রত্যাগম copy_struct_group(gr, ret);
                        প্রত্যাগম 0;
                }
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম -ESRCH;
}

পূর্ণাঙ্ক sysconf_ngroups_max(শূন্য) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        যদি (ngroups_max <= 0)
                প্রত্যাগম errno_or_else(EOPNOTSUPP);
        যদি (ngroups_max > INT_MAX)
                প্রত্যাগম -ERANGE;
        প্রত্যাগম ngroups_max;
}

ssize_t lookup_groups_in_files(
                বর্ণ * ধ্রুব *files,
                ধ্রুব বর্ণ *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = রিক্ত;
        ssize_t n_arr = 0;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(gid_is_valid(gid));

        পূর্ণাঙ্ক ngroups_max = sysconf_ngroups_max();
        যদি (ngroups_max < 0)
                প্রত্যাগম ngroups_max;

        যদি (!GREEDY_REALLOC(arr, n_arr + 1))
                প্রত্যাগম -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = রিক্ত;
                সংরচনা group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        অনুবর্ত;
                যদি (r < 0)
                        প্রত্যাগম r;

                যাবত্ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        যদি (gid == gr->gr_gid)
                                অনুবর্ত;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        যদি (gid_list_has(arr, n_arr, gr->gr_gid))
                                অনুবর্ত;

                        যদি (!strv_contains(gr->gr_mem, name))
                                অনুবর্ত;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        যদি (n_arr >= ngroups_max)
                                প্রত্যাগম -ENOBUFS;

                        যদি (!GREEDY_REALLOC(arr, n_arr + 1))
                                প্রত্যাগম -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret)
                *ret = TAKE_PTR(arr);
        প্রত্যাগম n_arr;
}

পূর্ণাঙ্ক getgrouplist_malloc(ধ্রুব বর্ণ *user, gid_t gid, gid_t **ret) {
#যদি BUILD_STATIC
        প্রত্যাগম lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#অন্যথা
        পূর্ণাঙ্ক ngroups_max = sysconf_ngroups_max();
        যদি (ngroups_max < 0)
                প্রত্যাগম ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        যদি (!gids)
                প্রত্যাগম -ENOMEM;

        পূর্ণাঙ্ক k = ngroups_max;
        যদি (getgrouplist(user, gid, gids, &k) < 0)
                প্রত্যাগম -EIO;  /* getgrouplist does not set errno */

        যদি (ret)
                *ret = TAKE_PTR(gids);
        প্রত্যাগম k;
#endif
}

পূর্ণাঙ্ক initgroups_wrapper(ধ্রুব বর্ণ *user, gid_t gid) {
#যদি BUILD_STATIC
        _cleanup_free_ gid_t *groups = রিক্ত;
        পূর্ণাঙ্ক r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        যদি (n_groups <= 0)
                প্রত্যাগম n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        প্রতি (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                যদি (r != -EINVAL)
                        ভঙ্গ;
        }

        প্রত্যাগম r;
#অন্যথা
        প্রত্যাগম RET_NERRNO(initgroups(user, gid));
#endif
}

#যদি !BUILD_STATIC

স্থির size_t getgr_buffer_size(শূন্য) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        প্রত্যাগম bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

পূর্ণাঙ্ক getgrnam_malloc(ধ্রুব বর্ণ *name, সংরচনা group **ret) {
        যদি (isempty(name))
                প্রত্যাগম -EINVAL;

#যদি BUILD_STATIC
        প্রত্যাগম lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#অন্যথা
        size_t bufsize = getgr_buffer_size();
        পূর্ণাঙ্ক r;

        প্রতি (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা group)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা group)) + bufsize);
                যদি (!buf)
                        প্রত্যাগম -ENOMEM;

                সংরচনা group *gr = রিক্ত;
                r = getgrnam_r(name, buf, (বর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা group)), bufsize, &gr);
                যদি (r == 0) {
                        যদি (gr) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                প্রত্যাগম 0;
                        }

                        প্রত্যাগম -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        প্রত্যাগম -ESRCH;
                যদি (r != ERANGE)
                        প্রত্যাগম -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা group)))
                        প্রত্যাগম -ENOMEM;
                bufsize *= 2;
        }
#endif
}

পূর্ণাঙ্ক getgrgid_malloc(gid_t gid, সংরচনা group **ret) {
        যদি (!gid_is_valid(gid))
                প্রত্যাগম -EINVAL;

#যদি BUILD_STATIC
        প্রত্যাগম lookup_grent_in_files(GROUP_FILES, /* name= */ রিক্ত, gid, ret);
#অন্যথা
        size_t bufsize = getgr_buffer_size();
        পূর্ণাঙ্ক r;

        প্রতি (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা group)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা group)) + bufsize);
                যদি (!buf)
                        প্রত্যাগম -ENOMEM;

                সংরচনা group *gr = রিক্ত;
                r = getgrgid_r(gid, buf, (বর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা group)), bufsize, &gr);
                যদি (r == 0) {
                        যদি (gr) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                প্রত্যাগম 0;
                        }

                        প্রত্যাগম -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        প্রত্যাগম -ESRCH;
                যদি (r != ERANGE)
                        প্রত্যাগম -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা group)))
                        প্রত্যাগম -ENOMEM;
                bufsize *= 2;
        }
#endif
}
