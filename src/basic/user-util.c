/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/file.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <utmpx.h>

#অন্তর্ভাৱ "sd-messages.h"

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "chase.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "extract-word.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fileio.h"
#অন্তর্ভাৱ "format-util.h"
#অন্তর্ভাৱ "lock-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "mkdir.h"
#অন্তর্ভাৱ "parse-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"
#অন্তর্ভাৱ "terminal-util.h"
#অন্তর্ভাৱ "user-util.h"
#অন্তর্ভাৱ "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        ধ্রুৱ ৱর্ণ* strerror_##type(                                    \
                        পূর্ণাঙ্ক errnum,                                     \
                        ৱর্ণ *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                ৱিকল্প (errnum) {                                       \
                পক্ষ ESRCH:                                             \
                        ঘূৰাই "Unknown " STRINGIFY(type);              \
                পক্ষ ENOEXEC:                                           \
                        ঘূৰাই "Not a system " STRINGIFY(type);         \
                স্ৱভাৱ:                                                \
                        ঘূৰাই strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        যদি (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ঘূৰাই false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        যদি (uid == (uid_t) UINT32_C(0xFFFF))
                ঘূৰাই false;

        ঘূৰাই true;
}

পূর্ণাঙ্ক parse_uid(ধ্রুৱ ৱর্ণ *s, uid_t *ret) {
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
                ঘূৰাই r;

        যদি (!uid_is_valid(uid))
                ঘূৰাই -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        যদি (ret)
                *ret = uid;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক parse_uid_range(ধ্রুৱ ৱর্ণ *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ ৱর্ণ *word = রিক্ত;
        uid_t l, u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret_lower);
        প্রতিজ্ঞা(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -EINVAL;

        r = parse_uid(word, &l);
        যদি (r < 0)
                ঘূৰাই r;

        /* Check for the upper bound and extract it if needed */
        যদি (!s)
                /* Single number with no dash. */
                u = l;
        অন্যথা যদি (!*s)
                /* Trailing dash is an error. */
                ঘূৰাই -EINVAL;
        অন্যথা {
                r = parse_uid(s, &u);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (l > u)
                        ঘূৰাই -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ঘূৰাই 0;
}

ৱর্ণ* getlogname_malloc(শূন্য) {
        uid_t uid;
        সংরচনা stat st;

        যদি (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        অন্যথা
                uid = getuid();

        ঘূৰাই uid_to_name(uid);
}

ৱর্ণ* getusername_malloc(শূন্য) {
        ধ্রুৱ ৱর্ণ *e;

        e = secure_getenv("USER");
        যদি (e)
                ঘূৰাই strdup(e);

        ঘূৰাই uid_to_name(getuid());
}

bool is_nologin_shell(ধ্রুৱ ৱর্ণ *shell) {
        ঘূৰাই PATH_IN_SET(shell,
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

bool shell_is_placeholder(ধ্রুৱ ৱর্ণ *shell) {
        ঘূৰাই isempty(shell) || is_nologin_shell(shell);
}

ধ্রুৱ ৱর্ণ* default_মূল_shell_at(পূর্ণাঙ্ক rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        প্রতিজ্ঞা(rfd >= 0 || rfd == AT_FDCWD);

        পূর্ণাঙ্ক r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, রিক্ত, রিক্ত);
        যদি (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        যদি (r > 0)
                ঘূৰাই DEFAULT_USER_SHELL;

        ঘূৰাই "/bin/sh";
}

ধ্রুৱ ৱর্ণ* default_মূল_shell(ধ্রুৱ ৱর্ণ *মূল) {
        _cleanup_close_ পূর্ণাঙ্ক rfd = -EBADF;

        rfd = open(empty_to_মূল(মূল), O_CLOEXEC | O_DIRECTORY | O_PATH);
        যদি (rfd < 0)
                ঘূৰাই "/bin/sh";

        ঘূৰাই default_মূল_shell_at(rfd);
}

স্থির পূর্ণাঙ্ক return_user_creds(
                ধ্রুৱ ৱর্ণ *username,
                uid_t uid, gid_t gid,
                ধ্রুৱ ৱর্ণ *home,
                ধ্রুৱ ৱর্ণ *shell,
                ৱর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ৱর্ণ **ret_home,
                ৱর্ণ **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ ৱর্ণ *s1 = রিক্ত, *s2 = রিক্ত, *s3 = রিক্ত;
        পূর্ণাঙ্ক r;

        যদি (ret_username) {
                r = strdup_to(&s1, username);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (ret_home) {
                r = strdup_to(&s2, home);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (ret_shell) {
                r = strdup_to(&s3, shell);
                যদি (r < 0)
                        ঘূৰাই r;
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
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক synthesize_user_creds(
                ধ্রুৱ ৱর্ণ *username,
                UserCredsFlags flags,
                ৱর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ৱর্ণ **ret_home,
                ৱর্ণ **ret_shell) {
        প্রতিজ্ঞা(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        যদি (STR_IN_SET(username, "root", "0"))
                ঘূৰাই return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_মূল_shell(রিক্ত) : রিক্ত,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        যদি (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ঘূৰাই return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? রিক্ত : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? রিক্ত : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ঘূৰাই -ENOMEDIUM;
}

পূর্ণাঙ্ক get_user_creds(
                ধ্রুৱ ৱর্ণ *username,
                UserCredsFlags flags,
                ৱর্ণ **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                ৱর্ণ **ret_home,
                ৱর্ণ **ret_shell) {

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
                        ঘূৰাই 0;
                যদি (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ঘূৰাই r;
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
                        ঘূৰাই 0;
                }
        } অন্যথা
                r = getpwnam_malloc(username, &pw);

        যদি (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                যদি (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ঘূৰাই 0;

                ঘূৰাই r;
        }

        যদি (ret_uid && !uid_is_valid(pw->pw_uid))
                ঘূৰাই -EBADMSG;

        যদি (ret_gid && !gid_is_valid(pw->pw_gid))
                ঘূৰাই -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ধ্রুৱ ৱর্ণ *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_মূল(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? রিক্ত : pw->pw_dir;

        ধ্রুৱ ৱর্ণ *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? রিক্ত : pw->pw_shell;

        ঘূৰাই return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

স্থির পূর্ণাঙ্ক synthesize_group_creds(ধ্রুৱ ৱর্ণ *groupname, ৱর্ণ **ret_name, gid_t *ret_gid) {
        প্রতিজ্ঞা(groupname);

        gid_t id;
        ধ্রুৱ ৱর্ণ *n;
        পূর্ণাঙ্ক r;

        যদি (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } অন্যথা যদি (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } অন্যথা
                ঘূৰাই -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (ret_gid)
                *ret_gid = id;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক get_group_creds(ধ্রুৱ ৱর্ণ *groupname, UserCredsFlags flags, ৱর্ণ **ret_name, gid_t *ret_gid) {
        _cleanup_free_ সংরচনা group *gr = রিক্ত;
        gid_t id;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(groupname);

        যদি (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                যদি (r >= 0)
                        ঘূৰাই 0;
                যদি (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ঘূৰাই r;
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
                        ঘূৰাই 0;
                }
        } অন্যথা
                r = getgrnam_malloc(groupname, &gr);

        যদি (r < 0) {
                যদি (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ঘূৰাই 0;
                ঘূৰাই r;
        }

        যদি (ret_gid && !gid_is_valid(gr->gr_gid))
                ঘূৰাই -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (ret_gid)
                *ret_gid = gr->gr_gid;
        ঘূৰাই 0;
}

ৱর্ণ* uid_to_name(uid_t uid) {
        ৱর্ণ *ret;
        পূর্ণাঙ্ক r;

        /* Shortcut things to avoid NSS lookups */
        যদি (uid == 0)
                ঘূৰাই strdup("root");
        যদি (uid == UID_NOBODY && synthesize_nobody())
                ঘূৰাই strdup(NOBODY_USER_NAME);

        যদি (uid_is_valid(uid)) {
                _cleanup_free_ সংরচনা passwd *pw = রিক্ত;

                r = getpwuid_malloc(uid, &pw);
                যদি (r >= 0)
                        ঘূৰাই strdup(pw->pw_name);
        }

        যদি (asprintf(&ret, UID_FMT, uid) < 0)
                ঘূৰাই রিক্ত;

        ঘূৰাই ret;
}

ৱর্ণ* gid_to_name(gid_t gid) {
        ৱর্ণ *ret;
        পূর্ণাঙ্ক r;

        যদি (gid == 0)
                ঘূৰাই strdup("root");
        যদি (gid == GID_NOBODY && synthesize_nobody())
                ঘূৰাই strdup(NOBODY_GROUP_NAME);

        যদি (gid_is_valid(gid)) {
                _cleanup_free_ সংরচনা group *gr = রিক্ত;

                r = getgrgid_malloc(gid, &gr);
                যদি (r >= 0)
                        ঘূৰাই strdup(gr->gr_name);
        }

        যদি (asprintf(&ret, GID_FMT, gid) < 0)
                ঘূৰাই রিক্ত;

        ঘূৰাই ret;
}

স্থির bool gid_list_has(ধ্রুৱ gid_t *list, size_t size, gid_t val) {
        প্রতিজ্ঞা(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                যদি (*i == val)
                        ঘূৰাই true;

        ঘূৰাই false;
}

পূর্ণাঙ্ক in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = রিক্ত;
        পূর্ণাঙ্ক ngroups;

        যদি (getgid() == gid)
                ঘূৰাই 1;

        যদি (getegid() == gid)
                ঘূৰাই 1;

        যদি (!gid_is_valid(gid))
                ঘূৰাই -EINVAL;

        ngroups = getgroups_alloc(&gids);
        যদি (ngroups < 0)
                ঘূৰাই ngroups;

        ঘূৰাই gid_list_has(gids, ngroups, gid);
}

পূর্ণাঙ্ক in_group(ধ্রুৱ ৱর্ণ *name) {
        পূর্ণাঙ্ক r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ রিক্ত, &gid);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই in_gid(gid);
}

পূর্ণাঙ্ক merge_gid_lists(ধ্রুৱ gid_t *list1, size_t size1, ধ্রুৱ gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        প্রতিজ্ঞা(size1 == 0 || list1);
        প্রতিজ্ঞা(size2 == 0 || list2);
        প্রতিজ্ঞা(ret);

        যদি (size2 > INT_MAX - size1)
                ঘূৰাই -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        যদি (!buf)
                ঘূৰাই -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        বাবে (size_t i = 0; i < size1; i++)
                যদি (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        বাবে (size_t i = 0; i < size2; i++)
                যদি (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ঘূৰাই (পূর্ণাঙ্ক)nresult;
}

পূর্ণাঙ্ক getgroups_alloc(gid_t **ret) {
        পূর্ণাঙ্ক ngroups = 8;

        প্রতিজ্ঞা(ret);

        বাবে (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = রিক্ত;

                p = new(gid_t, ngroups);
                যদি (!p)
                        ঘূৰাই -ENOMEM;

                ngroups = getgroups(ngroups, p);
                যদি (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ঘূৰাই ngroups;
                }
                যদি (ngroups == 0)
                        ভঙ্গ;
                যদি (errno != EINVAL)
                        ঘূৰাই -errno;

                /* Give up eventually */
                যদি (attempt++ > 10)
                        ঘূৰাই -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, রিক্ত);
                যদি (ngroups < 0)
                        ঘূৰাই -errno;
                যদি (ngroups == 0)
                        ভঙ্গ;
        }

        *ret = রিক্ত;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক get_home_dir(ৱর্ণ **ret) {
        _cleanup_free_ সংরচনা passwd *p = রিক্ত;
        ধ্রুৱ ৱর্ণ *e;
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
                ঘূৰাই r;

        e = p->pw_dir;
        যদি (!path_is_valid(e) || !path_is_absolute(e))
                ঘূৰাই -EINVAL;

 found:
        ঘূৰাই path_simplify_alloc(e, ret);
}

পূর্ণাঙ্ক get_shell(ৱর্ণ **ret) {
        _cleanup_free_ সংরচনা passwd *p = রিক্ত;
        ধ্রুৱ ৱর্ণ *e;
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
                ঘূৰাই r;

        e = p->pw_shell;
        যদি (!path_is_valid(e) || !path_is_absolute(e))
                ঘূৰাই -EINVAL;

 found:
        ঘূৰাই path_simplify_alloc(e, ret);
}

পূর্ণাঙ্ক fully_set_uid_gid(uid_t uid, gid_t gid, ধ্রুৱ gid_t supplementary_gids[], size_t n_supplementary_gids) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        যদি (r < 0)
                ঘূৰাই r;

        যদি (gid_is_valid(gid))
                যদি (setresgid(gid, gid, gid) < 0)
                        ঘূৰাই -errno;

        যদি (uid_is_valid(uid))
                যদি (setresuid(uid, uid, uid) < 0)
                        ঘূৰাই -errno;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক take_etc_passwd_lock(ধ্রুৱ ৱর্ণ *মূল) {
        পূর্ণাঙ্ক r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ ৱর্ণ *path = path_join(মূল, ETC_PASSWD_LOCK_PATH);
        যদি (!path)
                ঘূৰাই log_oom_debug();

        (শূন্য) mkdir_parents(path, 0755);

        _cleanup_close_ পূর্ণাঙ্ক fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        যদি (fd < 0)
                ঘূৰাই log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        যদি (r < 0)
                ঘূৰাই log_debug_errno(r, "Locking %s failed: %m", path);

        ঘূৰাই TAKE_FD(fd);
}

bool valid_user_group_name(ধ্রুৱ ৱর্ণ *u, ValidUserFlags flags) {
        ধ্রুৱ ৱর্ণ *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        যদি (isempty(u)) /* An empty user name is never valid */
                ঘূৰাই false;

        যদি (parse_uid(u, রিক্ত) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ঘূৰাই FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

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
                        ঘূৰাই false;

                যদি (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ঘূৰাই false;

                যদি (string_has_cc(u, রিক্ত)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ঘূৰাই false;

                যদি (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ঘূৰাই false;

                যদি (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ঘূৰাই false;

                যদি (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ঘূৰাই false;

                যদি (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ঘূৰাই false;

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
                        ঘূৰাই false;

                বাবে (i = u+1; *i; i++)
                        যদি (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ঘূৰাই false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                যদি (l > (size_t) sz) /* glibc: 256 */
                        ঘূৰাই false;
                যদি (l > NAME_MAX) /* must fit in a filename: 255 */
                        ঘূৰাই false;
                যদি (l > sizeof_field(সংরচনা utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ঘূৰাই false;
        }

        ঘূৰাই true;
}

bool valid_gecos(ধ্রুৱ ৱর্ণ *d) {

        যদি (!d)
                ঘূৰাই false;

        যদি (!utf8_is_valid(d))
                ঘূৰাই false;

        যদি (string_has_cc(d, রিক্ত))
                ঘূৰাই false;

        /* Colons are used as field separators, and hence not OK */
        যদি (strchr(d, ':'))
                ঘূৰাই false;

        ঘূৰাই true;
}

ৱর্ণ* mangle_gecos(ধ্রুৱ ৱর্ণ *d) {
        ৱর্ণ *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        যদি (!mangled)
                ঘূৰাই রিক্ত;

        বাবে (ৱর্ণ *i = mangled; *i; i++) {
                পূর্ণাঙ্ক len;

                যদি ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        অনুৱর্ত;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                যদি (len < 0) {
                        *i = ' ';
                        অনুৱর্ত;
                }

                i += len - 1;
        }

        ঘূৰাই mangled;
}

bool valid_home(ধ্রুৱ ৱর্ণ *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        যদি (isempty(p))
                ঘূৰাই false;

        যদি (!utf8_is_valid(p))
                ঘূৰাই false;

        যদি (string_has_cc(p, রিক্ত))
                ঘূৰাই false;

        যদি (!path_is_absolute(p))
                ঘূৰাই false;

        যদি (!path_is_normalized(p))
                ঘূৰাই false;

        /* Colons are used as field separators, and hence not OK */
        যদি (strchr(p, ':'))
                ঘূৰাই false;

        ঘূৰাই true;
}

bool valid_shell(ধ্রুৱ ৱর্ণ *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        যদি (!valid_home(p))
                ঘূৰাই false;

        ঘূৰাই !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

পূর্ণাঙ্ক maybe_setgroups(size_t size, ধ্রুৱ gid_t *list) {
        পূর্ণাঙ্ক r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        যদি (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                যদি (!f && errno != ENOENT)
                        ঘূৰাই -errno;
                যদি (f) {
                        r = safe_fgetc(f, /* ret= */ রিক্ত);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ঘূৰাই 0;
                        }
                }

                _cleanup_free_ ৱর্ণ *setgroups_content = রিক্ত;
                r = read_one_সাৰি_file("/proc/self/setgroups", &setgroups_content);
                যদি (r < 0 && r != -ENOENT)
                        ঘূৰাই r;
                যদি (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ঘূৰাই 0;
                }
        }

        ঘূৰাই RET_NERRNO(setgroups(size, list));
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

        ঘূৰাই cache;
}

পূর্ণাঙ্ক putpwent_sane(ধ্রুৱ সংরচনা passwd *pw, FILE *stream) {
        প্রতিজ্ঞা(pw);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putpwent(pw, stream) != 0)
                ঘূৰাই errno_or_else(EIO);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক putspent_sane(ধ্রুৱ সংরচনা spwd *sp, FILE *stream) {
        প্রতিজ্ঞা(sp);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putspent(sp, stream) != 0)
                ঘূৰাই errno_or_else(EIO);

        ঘূৰাই 0;
}

পূর্ণাঙ্ক putgrent_sane(ধ্রুৱ সংরচনা group *gr, FILE *stream) {
        প্রতিজ্ঞা(gr);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putgrent(gr, stream) != 0)
                ঘূৰাই errno_or_else(EIO);

        ঘূৰাই 0;
}

#যদি ENABLE_GSHADOW
পূর্ণাঙ্ক putsgent_sane(ধ্রুৱ সংরচনা sgrp *sg, FILE *stream) {
        প্রতিজ্ঞা(sg);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putsgent(sg, stream) != 0)
                ঘূৰাই errno_or_else(EIO);

        ঘূৰাই 0;
}
#endif

পূর্ণাঙ্ক fgetpwent_sane(FILE *stream, সংরচনা passwd **pw) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(pw);

        errno = 0;
        সংরচনা passwd *p = fgetpwent(stream);
        যদি (!p && !IN_SET(errno, 0, ENOENT))
                ঘূৰাই -errno;

        *pw = p;
        ঘূৰাই !!p;
}

পূর্ণাঙ্ক fgetspent_sane(FILE *stream, সংরচনা spwd **sp) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(sp);

        errno = 0;
        সংরচনা spwd *s = fgetspent(stream);
        যদি (!s && !IN_SET(errno, 0, ENOENT))
                ঘূৰাই -errno;

        *sp = s;
        ঘূৰাই !!s;
}

পূর্ণাঙ্ক fgetgrent_sane(FILE *stream, সংরচনা group **gr) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(gr);

        errno = 0;
        সংরচনা group *g = fgetgrent(stream);
        যদি (!g && !IN_SET(errno, 0, ENOENT))
                ঘূৰাই -errno;

        *gr = g;
        ঘূৰাই !!g;
}

#যদি ENABLE_GSHADOW
পূর্ণাঙ্ক fgetsgent_sane(FILE *stream, সংরচনা sgrp **sg) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(sg);

        errno = 0;
        সংরচনা sgrp *s = fgetsgent(stream);
        যদি (!s && !IN_SET(errno, 0, ENOENT))
                ঘূৰাই -errno;

        *sg = s;
        ঘূৰাই !!s;
}
#endif

পূর্ণাঙ্ক is_this_me(ধ্রুৱ ৱর্ণ *username) {
        uid_t uid;
        পূর্ণাঙ্ক r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, রিক্ত, &uid, রিক্ত, রিক্ত, রিক্ত);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই uid == getuid();
}

ধ্রুৱ ৱর্ণ* get_home_মূল(শূন্য) {
        /* For debug purposes allow overriding where we look for home dirs */
        ধ্রুৱ ৱর্ণ *e = secure_getenv("SYSTEMD_HOME_ROOT");
        যদি (e && path_is_absolute(e) && path_is_normalized(e))
                ঘূৰাই e;

        ঘূৰাই "/home";
}

স্থির পূর্ণাঙ্ক copy_struct_passwd(ধ্রুৱ সংরচনা passwd *pw, সংরচনা passwd **ret) {
        প্রতিজ্ঞা(pw);
        প্রতিজ্ঞা(ret);

        size_t need_bytes = পরিমাণ(সংরচনা passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        ৱর্ণ *buf = malloc(need_bytes);
        যদি (!buf)
                ঘূৰাই -ENOMEM;

        সংরচনা passwd *newpw = (সংরচনা passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        ৱর্ণ *p = buf + পরিমাণ(সংরচনা passwd);

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
        ঘূৰাই 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
পূর্ণাঙ্ক lookup_pwent_in_files(
                ৱর্ণ * ধ্রুৱ *files,
                ধ্রুৱ ৱর্ণ *name,
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
                        অনুৱর্ত;
                যদি (r < 0)
                        ঘূৰাই r;

                যেতিয়ালৈ ((r = fgetpwent_sane(f, &pw)) > 0) {
                        যদি (name && !streq_ptr(pw->pw_name, name))
                                অনুৱর্ত;
                        যদি (uid_is_valid(uid) && pw->pw_uid != uid)
                                অনুৱর্ত;
                        যদি (ret)
                                ঘূৰাই copy_struct_passwd(pw, ret);
                        ঘূৰাই 0;
                }
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই -ESRCH;
}

#যদি !BUILD_STATIC

স্থির size_t getpw_buffer_size(শূন্য) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ঘূৰাই bufsize <= 0 ? 4096U : (size_t) bufsize;
}

স্থির bool errno_is_user_doesnt_exist(পূর্ণাঙ্ক error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ঘূৰাই IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

পূর্ণাঙ্ক getpwnam_malloc(ধ্রুৱ ৱর্ণ *name, সংরচনা passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        যদি (isempty(name))
                ঘূৰাই -EINVAL;

#যদি BUILD_STATIC
        ঘূৰাই lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#অন্যথা
        size_t bufsize = getpw_buffer_size();
        পূর্ণাঙ্ক r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        বাবে (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা passwd)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা passwd)) + bufsize);
                যদি (!buf)
                        ঘূৰাই -ENOMEM;

                সংরচনা passwd *pw = রিক্ত;
                r = getpwnam_r(name, buf, (ৱর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা passwd)), bufsize, &pw);
                যদি (r == 0) {
                        যদি (pw) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ঘূৰাই 0;
                        }

                        ঘূৰাই -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                যদি (errno_is_user_doesnt_exist(r))
                        ঘূৰাই -ESRCH;
                যদি (r != ERANGE)
                        ঘূৰাই -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা passwd)))
                        ঘূৰাই -ENOMEM;
                bufsize *= 2;
        }
#endif
}

পূর্ণাঙ্ক getpwuid_malloc(uid_t uid, সংরচনা passwd **ret) {
        যদি (!uid_is_valid(uid))
                ঘূৰাই -EINVAL;

#যদি BUILD_STATIC
        ঘূৰাই lookup_pwent_in_files(PASSWD_FILES, /* name= */ রিক্ত, uid, ret);
#অন্যথা
        size_t bufsize = getpw_buffer_size();
        পূর্ণাঙ্ক r;

        বাবে (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা passwd)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা passwd)) + bufsize);
                যদি (!buf)
                        ঘূৰাই -ENOMEM;

                সংরচনা passwd *pw = রিক্ত;
                r = getpwuid_r(uid, buf, (ৱর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা passwd)), bufsize, &pw);
                যদি (r == 0) {
                        যদি (pw) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ঘূৰাই 0;
                        }

                        ঘূৰাই -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        ঘূৰাই -ESRCH;
                যদি (r != ERANGE)
                        ঘূৰাই -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা passwd)))
                        ঘূৰাই -ENOMEM;
                bufsize *= 2;
        }
#endif
}

স্থির পূর্ণাঙ্ক copy_struct_group(ধ্রুৱ সংরচনা group *gr, সংরচনা group **ret) {
        প্রতিজ্ঞা(gr);
        প্রতিজ্ঞা(ret);

        size_t need_bytes = পরিমাণ(সংরচনা group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += পরিমাণ(ৱর্ণ*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += পরিমাণ(ৱর্ণ*);  /* NULL terminator for gr_mem */

        ৱর্ণ *buf = malloc(need_bytes);
        যদি (!buf)
                ঘূৰাই -ENOMEM;

        সংরচনা group *newgr = (সংরচনা group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(পরিমাণ(সংরচনা group) % alignof(ৱর্ণ*) == 0);

        ৱর্ণ *p = buf + পরিমাণ(সংরচনা group) + (n_mem + 1) * পরিমাণ(ৱর্ণ*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (ৱর্ণ**) (buf + পরিমাণ(সংরচনা group));
        বাবে (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = রিক্ত;

        *ret = newgr;
        ঘূৰাই 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
পূর্ণাঙ্ক lookup_grent_in_files(
                ৱর্ণ * ধ্রুৱ *files,
                ধ্রুৱ ৱর্ণ *name,
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
                        অনুৱর্ত;
                যদি (r < 0)
                        ঘূৰাই r;

                যেতিয়ালৈ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        যদি (name && !streq_ptr(gr->gr_name, name))
                                অনুৱর্ত;
                        যদি (gid_is_valid(gid) && gr->gr_gid != gid)
                                অনুৱর্ত;
                        যদি (ret)
                                ঘূৰাই copy_struct_group(gr, ret);
                        ঘূৰাই 0;
                }
                যদি (r < 0)
                        ঘূৰাই r;
        }

        ঘূৰাই -ESRCH;
}

পূর্ণাঙ্ক sysconf_ngroups_max(শূন্য) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        যদি (ngroups_max <= 0)
                ঘূৰাই errno_or_else(EOPNOTSUPP);
        যদি (ngroups_max > INT_MAX)
                ঘূৰাই -ERANGE;
        ঘূৰাই ngroups_max;
}

ssize_t lookup_groups_in_files(
                ৱর্ণ * ধ্রুৱ *files,
                ধ্রুৱ ৱর্ণ *name,
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
                ঘূৰাই ngroups_max;

        যদি (!GREEDY_REALLOC(arr, n_arr + 1))
                ঘূৰাই -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = রিক্ত;
                সংরচনা group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        অনুৱর্ত;
                যদি (r < 0)
                        ঘূৰাই r;

                যেতিয়ালৈ ((r = fgetgrent_sane(f, &gr)) > 0) {
                        যদি (gid == gr->gr_gid)
                                অনুৱর্ত;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        যদি (gid_list_has(arr, n_arr, gr->gr_gid))
                                অনুৱর্ত;

                        যদি (!strv_contains(gr->gr_mem, name))
                                অনুৱর্ত;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        যদি (n_arr >= ngroups_max)
                                ঘূৰাই -ENOBUFS;

                        যদি (!GREEDY_REALLOC(arr, n_arr + 1))
                                ঘূৰাই -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (ret)
                *ret = TAKE_PTR(arr);
        ঘূৰাই n_arr;
}

পূর্ণাঙ্ক getgrouplist_malloc(ধ্রুৱ ৱর্ণ *user, gid_t gid, gid_t **ret) {
#যদি BUILD_STATIC
        ঘূৰাই lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#অন্যথা
        পূর্ণাঙ্ক ngroups_max = sysconf_ngroups_max();
        যদি (ngroups_max < 0)
                ঘূৰাই ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        যদি (!gids)
                ঘূৰাই -ENOMEM;

        পূর্ণাঙ্ক k = ngroups_max;
        যদি (getgrouplist(user, gid, gids, &k) < 0)
                ঘূৰাই -EIO;  /* getgrouplist does not set errno */

        যদি (ret)
                *ret = TAKE_PTR(gids);
        ঘূৰাই k;
#endif
}

পূর্ণাঙ্ক initgroups_wrapper(ধ্রুৱ ৱর্ণ *user, gid_t gid) {
#যদি BUILD_STATIC
        _cleanup_free_ gid_t *groups = রিক্ত;
        পূর্ণাঙ্ক r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        যদি (n_groups <= 0)
                ঘূৰাই n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        বাবে (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                যদি (r != -EINVAL)
                        ভঙ্গ;
        }

        ঘূৰাই r;
#অন্যথা
        ঘূৰাই RET_NERRNO(initgroups(user, gid));
#endif
}

#যদি !BUILD_STATIC

স্থির size_t getgr_buffer_size(শূন্য) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ঘূৰাই bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

পূর্ণাঙ্ক getgrnam_malloc(ধ্রুৱ ৱর্ণ *name, সংরচনা group **ret) {
        যদি (isempty(name))
                ঘূৰাই -EINVAL;

#যদি BUILD_STATIC
        ঘূৰাই lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#অন্যথা
        size_t bufsize = getgr_buffer_size();
        পূর্ণাঙ্ক r;

        বাবে (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা group)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা group)) + bufsize);
                যদি (!buf)
                        ঘূৰাই -ENOMEM;

                সংরচনা group *gr = রিক্ত;
                r = getgrnam_r(name, buf, (ৱর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা group)), bufsize, &gr);
                যদি (r == 0) {
                        যদি (gr) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ঘূৰাই 0;
                        }

                        ঘূৰাই -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        ঘূৰাই -ESRCH;
                যদি (r != ERANGE)
                        ঘূৰাই -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা group)))
                        ঘূৰাই -ENOMEM;
                bufsize *= 2;
        }
#endif
}

পূর্ণাঙ্ক getgrgid_malloc(gid_t gid, সংরচনা group **ret) {
        যদি (!gid_is_valid(gid))
                ঘূৰাই -EINVAL;

#যদি BUILD_STATIC
        ঘূৰাই lookup_grent_in_files(GROUP_FILES, /* name= */ রিক্ত, gid, ret);
#অন্যথা
        size_t bufsize = getgr_buffer_size();
        পূর্ণাঙ্ক r;

        বাবে (;;) {
                _cleanup_free_ শূন্য *buf = রিক্ত;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(পরিমাণ(সংরচনা group)));
                buf = malloc0(ALIGN(পরিমাণ(সংরচনা group)) + bufsize);
                যদি (!buf)
                        ঘূৰাই -ENOMEM;

                সংরচনা group *gr = রিক্ত;
                r = getgrgid_r(gid, buf, (ৱর্ণ*) buf + ALIGN(পরিমাণ(সংরচনা group)), bufsize, &gr);
                যদি (r == 0) {
                        যদি (gr) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ঘূৰাই 0;
                        }

                        ঘূৰাই -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        ঘূৰাই -ESRCH;
                যদি (r != ERANGE)
                        ঘূৰাই -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(পরিমাণ(সংরচনা group)))
                        ঘূৰাই -ENOMEM;
                bufsize *= 2;
        }
#endif
}
