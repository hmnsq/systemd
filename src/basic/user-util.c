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
        ধ্রুবক অক্ষর* strerror_##type(                                    \
                        পূর্ণসংখ্যা errnum,                                     \
                        অক্ষর *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                নির্বাচন (errnum) {                                       \
                ক্ষেত্র ESRCH:                                             \
                        ফেরত "Unknown " STRINGIFY(type);              \
                ক্ষেত্র ENOEXEC:                                           \
                        ফেরত "Not a system " STRINGIFY(type);         \
                পূর্বনির্ধারিত:                                                \
                        ফেরত strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        যদি (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                ফেরত false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        যদি (uid == (uid_t) UINT32_C(0xFFFF))
                ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা parse_uid(ধ্রুবক অক্ষর *s, uid_t *ret) {
        uint32_t uid = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        assert_cc(আকার(uid_t) == আকার(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        যদি (r < 0)
                ফেরত r;

        যদি (!uid_is_valid(uid))
                ফেরত -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        যদি (ret)
                *ret = uid;

        ফেরত 0;
}

পূর্ণসংখ্যা parse_uid_range(ধ্রুবক অক্ষর *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ অক্ষর *word = ফাঁকা;
        uid_t l, u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(ret_lower);
        প্রতিজ্ঞা(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EINVAL;

        r = parse_uid(word, &l);
        যদি (r < 0)
                ফেরত r;

        /* Check for the upper bound and extract it if needed */
        যদি (!s)
                /* Single number with no dash. */
                u = l;
        অন্যথা যদি (!*s)
                /* Trailing dash is an error. */
                ফেরত -EINVAL;
        অন্যথা {
                r = parse_uid(s, &u);
                যদি (r < 0)
                        ফেরত r;

                যদি (l > u)
                        ফেরত -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        ফেরত 0;
}

অক্ষর* getlogname_malloc(শূন্য) {
        uid_t uid;
        কাঠামো stat st;

        যদি (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        অন্যথা
                uid = getuid();

        ফেরত uid_to_name(uid);
}

অক্ষর* getusername_malloc(শূন্য) {
        ধ্রুবক অক্ষর *e;

        e = secure_getenv("USER");
        যদি (e)
                ফেরত strdup(e);

        ফেরত uid_to_name(getuid());
}

bool is_nologin_shell(ধ্রুবক অক্ষর *shell) {
        ফেরত PATH_IN_SET(shell,
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

bool shell_is_placeholder(ধ্রুবক অক্ষর *shell) {
        ফেরত isempty(shell) || is_nologin_shell(shell);
}

ধ্রুবক অক্ষর* default_মূল_shell_at(পূর্ণসংখ্যা rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        প্রতিজ্ঞা(rfd >= 0 || rfd == AT_FDCWD);

        পূর্ণসংখ্যা r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, ফাঁকা, ফাঁকা);
        যদি (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        যদি (r > 0)
                ফেরত DEFAULT_USER_SHELL;

        ফেরত "/bin/sh";
}

ধ্রুবক অক্ষর* default_মূল_shell(ধ্রুবক অক্ষর *মূল) {
        _cleanup_close_ পূর্ণসংখ্যা rfd = -EBADF;

        rfd = open(empty_to_মূল(মূল), O_CLOEXEC | O_DIRECTORY | O_PATH);
        যদি (rfd < 0)
                ফেরত "/bin/sh";

        ফেরত default_মূল_shell_at(rfd);
}

স্থির পূর্ণসংখ্যা return_user_creds(
                ধ্রুবক অক্ষর *username,
                uid_t uid, gid_t gid,
                ধ্রুবক অক্ষর *home,
                ধ্রুবক অক্ষর *shell,
                অক্ষর **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                অক্ষর **ret_home,
                অক্ষর **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ অক্ষর *s1 = ফাঁকা, *s2 = ফাঁকা, *s3 = ফাঁকা;
        পূর্ণসংখ্যা r;

        যদি (ret_username) {
                r = strdup_to(&s1, username);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_home) {
                r = strdup_to(&s2, home);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_shell) {
                r = strdup_to(&s3, shell);
                যদি (r < 0)
                        ফেরত r;
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
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা synthesize_user_creds(
                ধ্রুবক অক্ষর *username,
                UserCredsFlags flags,
                অক্ষর **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                অক্ষর **ret_home,
                অক্ষর **ret_shell) {
        প্রতিজ্ঞা(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        যদি (STR_IN_SET(username, "root", "0"))
                ফেরত return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_মূল_shell(ফাঁকা) : ফাঁকা,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        যদি (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                ফেরত return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ফাঁকা : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? ফাঁকা : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        ফেরত -ENOMEDIUM;
}

পূর্ণসংখ্যা get_user_creds(
                ধ্রুবক অক্ষর *username,
                UserCredsFlags flags,
                অক্ষর **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                অক্ষর **ret_home,
                অক্ষর **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ কাঠামো passwd *pw = ফাঁকা;
        পূর্ণসংখ্যা r;

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
                        ফেরত 0;
                যদি (r != -ENOMEDIUM) /* not a username we can synthesize */
                        ফেরত r;
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
                                *ret_username = ফাঁকা;
                        যদি (ret_uid)
                                *ret_uid = u;
                        ফেরত 0;
                }
        } অন্যথা
                r = getpwnam_malloc(username, &pw);

        যদি (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                যদি (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        ফেরত 0;

                ফেরত r;
        }

        যদি (ret_uid && !uid_is_valid(pw->pw_uid))
                ফেরত -EBADMSG;

        যদি (ret_gid && !gid_is_valid(pw->pw_gid))
                ফেরত -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ধ্রুবক অক্ষর *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_মূল(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? ফাঁকা : pw->pw_dir;

        ধ্রুবক অক্ষর *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? ফাঁকা : pw->pw_shell;

        ফেরত return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

স্থির পূর্ণসংখ্যা synthesize_group_creds(ধ্রুবক অক্ষর *groupname, অক্ষর **ret_name, gid_t *ret_gid) {
        প্রতিজ্ঞা(groupname);

        gid_t id;
        ধ্রুবক অক্ষর *n;
        পূর্ণসংখ্যা r;

        যদি (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } অন্যথা যদি (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } অন্যথা
                ফেরত -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        যদি (r < 0)
                ফেরত r;
        যদি (ret_gid)
                *ret_gid = id;
        ফেরত 0;
}

পূর্ণসংখ্যা get_group_creds(ধ্রুবক অক্ষর *groupname, UserCredsFlags flags, অক্ষর **ret_name, gid_t *ret_gid) {
        _cleanup_free_ কাঠামো group *gr = ফাঁকা;
        gid_t id;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(groupname);

        যদি (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                যদি (r >= 0)
                        ফেরত 0;
                যদি (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        ফেরত r;
        }

        যদি (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                যদি (r >= 0)
                        groupname = gr->gr_name;
                অন্যথা যদি (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        যদি (ret_gid)
                                *ret_gid = id;
                        যদি (ret_name)
                                *ret_name = ফাঁকা;
                        ফেরত 0;
                }
        } অন্যথা
                r = getgrnam_malloc(groupname, &gr);

        যদি (r < 0) {
                যদি (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        ফেরত 0;
                ফেরত r;
        }

        যদি (ret_gid && !gid_is_valid(gr->gr_gid))
                ফেরত -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        যদি (r < 0)
                ফেরত r;
        যদি (ret_gid)
                *ret_gid = gr->gr_gid;
        ফেরত 0;
}

অক্ষর* uid_to_name(uid_t uid) {
        অক্ষর *ret;
        পূর্ণসংখ্যা r;

        /* Shortcut things to avoid NSS lookups */
        যদি (uid == 0)
                ফেরত strdup("root");
        যদি (uid == UID_NOBODY && synthesize_nobody())
                ফেরত strdup(NOBODY_USER_NAME);

        যদি (uid_is_valid(uid)) {
                _cleanup_free_ কাঠামো passwd *pw = ফাঁকা;

                r = getpwuid_malloc(uid, &pw);
                যদি (r >= 0)
                        ফেরত strdup(pw->pw_name);
        }

        যদি (asprintf(&ret, UID_FMT, uid) < 0)
                ফেরত ফাঁকা;

        ফেরত ret;
}

অক্ষর* gid_to_name(gid_t gid) {
        অক্ষর *ret;
        পূর্ণসংখ্যা r;

        যদি (gid == 0)
                ফেরত strdup("root");
        যদি (gid == GID_NOBODY && synthesize_nobody())
                ফেরত strdup(NOBODY_GROUP_NAME);

        যদি (gid_is_valid(gid)) {
                _cleanup_free_ কাঠামো group *gr = ফাঁকা;

                r = getgrgid_malloc(gid, &gr);
                যদি (r >= 0)
                        ফেরত strdup(gr->gr_name);
        }

        যদি (asprintf(&ret, GID_FMT, gid) < 0)
                ফেরত ফাঁকা;

        ফেরত ret;
}

স্থির bool gid_list_has(ধ্রুবক gid_t *list, size_t আকার_2, gid_t val) {
        প্রতিজ্ঞা(list || আকার_2 == 0);

        FOREACH_ARRAY(i, list, আকার_2)
                যদি (*i == val)
                        ফেরত true;

        ফেরত false;
}

পূর্ণসংখ্যা in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = ফাঁকা;
        পূর্ণসংখ্যা ngroups;

        যদি (getgid() == gid)
                ফেরত 1;

        যদি (getegid() == gid)
                ফেরত 1;

        যদি (!gid_is_valid(gid))
                ফেরত -EINVAL;

        ngroups = getgroups_alloc(&gids);
        যদি (ngroups < 0)
                ফেরত ngroups;

        ফেরত gid_list_has(gids, ngroups, gid);
}

পূর্ণসংখ্যা in_group(ধ্রুবক অক্ষর *name) {
        পূর্ণসংখ্যা r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ ফাঁকা, &gid);
        যদি (r < 0)
                ফেরত r;

        ফেরত in_gid(gid);
}

পূর্ণসংখ্যা merge_gid_lists(ধ্রুবক gid_t *list1, size_t size1, ধ্রুবক gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        প্রতিজ্ঞা(size1 == 0 || list1);
        প্রতিজ্ঞা(size2 == 0 || list2);
        প্রতিজ্ঞা(ret);

        যদি (size2 > INT_MAX - size1)
                ফেরত -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        যদি (!buf)
                ফেরত -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        জন্য (size_t i = 0; i < size1; i++)
                যদি (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        জন্য (size_t i = 0; i < size2; i++)
                যদি (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        ফেরত (পূর্ণসংখ্যা)nresult;
}

পূর্ণসংখ্যা getgroups_alloc(gid_t **ret) {
        পূর্ণসংখ্যা ngroups = 8;

        প্রতিজ্ঞা(ret);

        জন্য (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = ফাঁকা;

                p = new(gid_t, ngroups);
                যদি (!p)
                        ফেরত -ENOMEM;

                ngroups = getgroups(ngroups, p);
                যদি (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        ফেরত ngroups;
                }
                যদি (ngroups == 0)
                        বিরতি;
                যদি (errno != EINVAL)
                        ফেরত -errno;

                /* Give up eventually */
                যদি (attempt++ > 10)
                        ফেরত -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, ফাঁকা);
                যদি (ngroups < 0)
                        ফেরত -errno;
                যদি (ngroups == 0)
                        বিরতি;
        }

        *ret = ফাঁকা;
        ফেরত 0;
}

পূর্ণসংখ্যা get_home_dir(অক্ষর **ret) {
        _cleanup_free_ কাঠামো passwd *p = ফাঁকা;
        ধ্রুবক অক্ষর *e;
        uid_t u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        যদি (e && path_is_valid(e) && path_is_absolute(e))
                যাও found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        যদি (u == 0) {
                e = "/root";
                যাও found;
        }
        যদি (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                যাও found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        যদি (r < 0)
                ফেরত r;

        e = p->pw_dir;
        যদি (!path_is_valid(e) || !path_is_absolute(e))
                ফেরত -EINVAL;

 found:
        ফেরত path_simplify_alloc(e, ret);
}

পূর্ণসংখ্যা get_shell(অক্ষর **ret) {
        _cleanup_free_ কাঠামো passwd *p = ফাঁকা;
        ধ্রুবক অক্ষর *e;
        uid_t u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        যদি (e && path_is_valid(e) && path_is_absolute(e))
                যাও found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        যদি (u == 0) {
                e = default_মূল_shell(ফাঁকা);
                যাও found;
        }
        যদি (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                যাও found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        যদি (r < 0)
                ফেরত r;

        e = p->pw_shell;
        যদি (!path_is_valid(e) || !path_is_absolute(e))
                ফেরত -EINVAL;

 found:
        ফেরত path_simplify_alloc(e, ret);
}

পূর্ণসংখ্যা fully_set_uid_gid(uid_t uid, gid_t gid, ধ্রুবক gid_t supplementary_gids[], size_t n_supplementary_gids) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        যদি (r < 0)
                ফেরত r;

        যদি (gid_is_valid(gid))
                যদি (setresgid(gid, gid, gid) < 0)
                        ফেরত -errno;

        যদি (uid_is_valid(uid))
                যদি (setresuid(uid, uid, uid) < 0)
                        ফেরত -errno;

        ফেরত 0;
}

পূর্ণসংখ্যা take_etc_passwd_lock(ধ্রুবক অক্ষর *মূল) {
        পূর্ণসংখ্যা r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ অক্ষর *path = path_join(মূল, ETC_PASSWD_LOCK_PATH);
        যদি (!path)
                ফেরত log_oom_debug();

        (শূন্য) mkdir_parents(path, 0755);

        _cleanup_close_ পূর্ণসংখ্যা fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        যদি (fd < 0)
                ফেরত log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Locking %s failed: %m", path);

        ফেরত TAKE_FD(fd);
}

bool valid_user_group_name(ধ্রুবক অক্ষর *u, ValidUserFlags flags) {
        ধ্রুবক অক্ষর *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        যদি (isempty(u)) /* An empty user name is never valid */
                ফেরত false;

        যদি (parse_uid(u, ফাঁকা) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                ফেরত FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

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
                        ফেরত false;

                যদি (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        ফেরত false;

                যদি (string_has_cc(u, ফাঁকা)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        ফেরত false;

                যদি (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        ফেরত false;

                যদি (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        ফেরত false;

                যদি (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        ফেরত false;

                যদি (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        ফেরত false;

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
                        ফেরত false;

                জন্য (i = u+1; *i; i++)
                        যদি (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                ফেরত false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                যদি (l > (size_t) sz) /* glibc: 256 */
                        ফেরত false;
                যদি (l > NAME_MAX) /* must fit in a filename: 255 */
                        ফেরত false;
                যদি (l > sizeof_field(কাঠামো utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        ফেরত false;
        }

        ফেরত true;
}

bool valid_gecos(ধ্রুবক অক্ষর *d) {

        যদি (!d)
                ফেরত false;

        যদি (!utf8_is_valid(d))
                ফেরত false;

        যদি (string_has_cc(d, ফাঁকা))
                ফেরত false;

        /* Colons are used as field separators, and hence not OK */
        যদি (strchr(d, ':'))
                ফেরত false;

        ফেরত true;
}

অক্ষর* mangle_gecos(ধ্রুবক অক্ষর *d) {
        অক্ষর *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        যদি (!mangled)
                ফেরত ফাঁকা;

        জন্য (অক্ষর *i = mangled; *i; i++) {
                পূর্ণসংখ্যা len;

                যদি ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        চালু;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                যদি (len < 0) {
                        *i = ' ';
                        চালু;
                }

                i += len - 1;
        }

        ফেরত mangled;
}

bool valid_home(ধ্রুবক অক্ষর *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        যদি (isempty(p))
                ফেরত false;

        যদি (!utf8_is_valid(p))
                ফেরত false;

        যদি (string_has_cc(p, ফাঁকা))
                ফেরত false;

        যদি (!path_is_absolute(p))
                ফেরত false;

        যদি (!path_is_normalized(p))
                ফেরত false;

        /* Colons are used as field separators, and hence not OK */
        যদি (strchr(p, ':'))
                ফেরত false;

        ফেরত true;
}

bool valid_shell(ধ্রুবক অক্ষর *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        যদি (!valid_home(p))
                ফেরত false;

        ফেরত !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

পূর্ণসংখ্যা maybe_setgroups(size_t আকার_2, ধ্রুবক gid_t *list) {
        পূর্ণসংখ্যা r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        যদি (আকার_2 == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                যদি (!f && errno != ENOENT)
                        ফেরত -errno;
                যদি (f) {
                        r = safe_fgetc(f, /* ret= */ ফাঁকা);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                ফেরত 0;
                        }
                }

                _cleanup_free_ অক্ষর *setgroups_content = ফাঁকা;
                r = read_one_সারি_file("/proc/self/setgroups", &setgroups_content);
                যদি (r < 0 && r != -ENOENT)
                        ফেরত r;
                যদি (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        ফেরত 0;
                }
        }

        ফেরত RET_NERRNO(setgroups(আকার_2, list));
}

bool synthesize_nobody(শূন্য) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        স্থির পূর্ণসংখ্যা cache = -1;

        যদি (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        ফেরত cache;
}

পূর্ণসংখ্যা putpwent_sane(ধ্রুবক কাঠামো passwd *pw, FILE *stream) {
        প্রতিজ্ঞা(pw);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putpwent(pw, stream) != 0)
                ফেরত errno_or_else(EIO);

        ফেরত 0;
}

পূর্ণসংখ্যা putspent_sane(ধ্রুবক কাঠামো spwd *sp, FILE *stream) {
        প্রতিজ্ঞা(sp);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putspent(sp, stream) != 0)
                ফেরত errno_or_else(EIO);

        ফেরত 0;
}

পূর্ণসংখ্যা putgrent_sane(ধ্রুবক কাঠামো group *gr, FILE *stream) {
        প্রতিজ্ঞা(gr);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putgrent(gr, stream) != 0)
                ফেরত errno_or_else(EIO);

        ফেরত 0;
}

#যদি ENABLE_GSHADOW
পূর্ণসংখ্যা putsgent_sane(ধ্রুবক কাঠামো sgrp *sg, FILE *stream) {
        প্রতিজ্ঞা(sg);
        প্রতিজ্ঞা(stream);

        errno = 0;
        যদি (putsgent(sg, stream) != 0)
                ফেরত errno_or_else(EIO);

        ফেরত 0;
}
#endif

পূর্ণসংখ্যা fgetpwent_sane(FILE *stream, কাঠামো passwd **pw) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(pw);

        errno = 0;
        কাঠামো passwd *p = fgetpwent(stream);
        যদি (!p && !IN_SET(errno, 0, ENOENT))
                ফেরত -errno;

        *pw = p;
        ফেরত !!p;
}

পূর্ণসংখ্যা fgetspent_sane(FILE *stream, কাঠামো spwd **sp) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(sp);

        errno = 0;
        কাঠামো spwd *s = fgetspent(stream);
        যদি (!s && !IN_SET(errno, 0, ENOENT))
                ফেরত -errno;

        *sp = s;
        ফেরত !!s;
}

পূর্ণসংখ্যা fgetgrent_sane(FILE *stream, কাঠামো group **gr) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(gr);

        errno = 0;
        কাঠামো group *g = fgetgrent(stream);
        যদি (!g && !IN_SET(errno, 0, ENOENT))
                ফেরত -errno;

        *gr = g;
        ফেরত !!g;
}

#যদি ENABLE_GSHADOW
পূর্ণসংখ্যা fgetsgent_sane(FILE *stream, কাঠামো sgrp **sg) {
        প্রতিজ্ঞা(stream);
        প্রতিজ্ঞা(sg);

        errno = 0;
        কাঠামো sgrp *s = fgetsgent(stream);
        যদি (!s && !IN_SET(errno, 0, ENOENT))
                ফেরত -errno;

        *sg = s;
        ফেরত !!s;
}
#endif

পূর্ণসংখ্যা is_this_me(ধ্রুবক অক্ষর *username) {
        uid_t uid;
        পূর্ণসংখ্যা r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, ফাঁকা, &uid, ফাঁকা, ফাঁকা, ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        ফেরত uid == getuid();
}

ধ্রুবক অক্ষর* get_home_মূল(শূন্য) {
        /* For debug purposes allow overriding where we look for home dirs */
        ধ্রুবক অক্ষর *e = secure_getenv("SYSTEMD_HOME_ROOT");
        যদি (e && path_is_absolute(e) && path_is_normalized(e))
                ফেরত e;

        ফেরত "/home";
}

স্থির পূর্ণসংখ্যা copy_struct_passwd(ধ্রুবক কাঠামো passwd *pw, কাঠামো passwd **ret) {
        প্রতিজ্ঞা(pw);
        প্রতিজ্ঞা(ret);

        size_t need_bytes = আকার(কাঠামো passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        অক্ষর *buf = malloc(need_bytes);
        যদি (!buf)
                ফেরত -ENOMEM;

        কাঠামো passwd *newpw = (কাঠামো passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        অক্ষর *p = buf + আকার(কাঠামো passwd);

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
        ফেরত 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
পূর্ণসংখ্যা lookup_pwent_in_files(
                অক্ষর * ধ্রুবক *files,
                ধ্রুবক অক্ষর *name,
                uid_t uid,
                কাঠামো passwd **ret) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ফাঁকা;
                কাঠামো passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        চালু;
                যদি (r < 0)
                        ফেরত r;

                যখন ((r = fgetpwent_sane(f, &pw)) > 0) {
                        যদি (name && !streq_ptr(pw->pw_name, name))
                                চালু;
                        যদি (uid_is_valid(uid) && pw->pw_uid != uid)
                                চালু;
                        যদি (ret)
                                ফেরত copy_struct_passwd(pw, ret);
                        ফেরত 0;
                }
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত -ESRCH;
}

#যদি !BUILD_STATIC

স্থির size_t getpw_বাফার_আকার(শূন্য) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        ফেরত bufsize <= 0 ? 4096U : (size_t) bufsize;
}

স্থির bool errno_is_user_doesnt_exist(পূর্ণসংখ্যা error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        ফেরত IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

পূর্ণসংখ্যা getpwnam_malloc(ধ্রুবক অক্ষর *name, কাঠামো passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        যদি (isempty(name))
                ফেরত -EINVAL;

#যদি BUILD_STATIC
        ফেরত lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#অন্যথা
        size_t bufsize = getpw_বাফার_আকার();
        পূর্ণসংখ্যা r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        জন্য (;;) {
                _cleanup_free_ শূন্য *buf = ফাঁকা;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(আকার(কাঠামো passwd)));
                buf = malloc0(ALIGN(আকার(কাঠামো passwd)) + bufsize);
                যদি (!buf)
                        ফেরত -ENOMEM;

                কাঠামো passwd *pw = ফাঁকা;
                r = getpwnam_r(name, buf, (অক্ষর*) buf + ALIGN(আকার(কাঠামো passwd)), bufsize, &pw);
                যদি (r == 0) {
                        যদি (pw) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ফেরত 0;
                        }

                        ফেরত -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                যদি (errno_is_user_doesnt_exist(r))
                        ফেরত -ESRCH;
                যদি (r != ERANGE)
                        ফেরত -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(আকার(কাঠামো passwd)))
                        ফেরত -ENOMEM;
                bufsize *= 2;
        }
#endif
}

পূর্ণসংখ্যা getpwuid_malloc(uid_t uid, কাঠামো passwd **ret) {
        যদি (!uid_is_valid(uid))
                ফেরত -EINVAL;

#যদি BUILD_STATIC
        ফেরত lookup_pwent_in_files(PASSWD_FILES, /* name= */ ফাঁকা, uid, ret);
#অন্যথা
        size_t bufsize = getpw_বাফার_আকার();
        পূর্ণসংখ্যা r;

        জন্য (;;) {
                _cleanup_free_ শূন্য *buf = ফাঁকা;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(আকার(কাঠামো passwd)));
                buf = malloc0(ALIGN(আকার(কাঠামো passwd)) + bufsize);
                যদি (!buf)
                        ফেরত -ENOMEM;

                কাঠামো passwd *pw = ফাঁকা;
                r = getpwuid_r(uid, buf, (অক্ষর*) buf + ALIGN(আকার(কাঠামো passwd)), bufsize, &pw);
                যদি (r == 0) {
                        যদি (pw) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ফেরত 0;
                        }

                        ফেরত -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        ফেরত -ESRCH;
                যদি (r != ERANGE)
                        ফেরত -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(আকার(কাঠামো passwd)))
                        ফেরত -ENOMEM;
                bufsize *= 2;
        }
#endif
}

স্থির পূর্ণসংখ্যা copy_struct_group(ধ্রুবক কাঠামো group *gr, কাঠামো group **ret) {
        প্রতিজ্ঞা(gr);
        প্রতিজ্ঞা(ret);

        size_t need_bytes = আকার(কাঠামো group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += আকার(অক্ষর*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += আকার(অক্ষর*);  /* NULL terminator for gr_mem */

        অক্ষর *buf = malloc(need_bytes);
        যদি (!buf)
                ফেরত -ENOMEM;

        কাঠামো group *newgr = (কাঠামো group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(আকার(কাঠামো group) % alignof(অক্ষর*) == 0);

        অক্ষর *p = buf + আকার(কাঠামো group) + (n_mem + 1) * আকার(অক্ষর*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (অক্ষর**) (buf + আকার(কাঠামো group));
        জন্য (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = ফাঁকা;

        *ret = newgr;
        ফেরত 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
পূর্ণসংখ্যা lookup_grent_in_files(
                অক্ষর * ধ্রুবক *files,
                ধ্রুবক অক্ষর *name,
                gid_t gid,
                কাঠামো group **ret) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ফাঁকা;
                কাঠামো group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        চালু;
                যদি (r < 0)
                        ফেরত r;

                যখন ((r = fgetgrent_sane(f, &gr)) > 0) {
                        যদি (name && !streq_ptr(gr->gr_name, name))
                                চালু;
                        যদি (gid_is_valid(gid) && gr->gr_gid != gid)
                                চালু;
                        যদি (ret)
                                ফেরত copy_struct_group(gr, ret);
                        ফেরত 0;
                }
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত -ESRCH;
}

পূর্ণসংখ্যা sysconf_ngroups_max(শূন্য) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        যদি (ngroups_max <= 0)
                ফেরত errno_or_else(EOPNOTSUPP);
        যদি (ngroups_max > INT_MAX)
                ফেরত -ERANGE;
        ফেরত ngroups_max;
}

ssize_t lookup_groups_in_files(
                অক্ষর * ধ্রুবক *files,
                ধ্রুবক অক্ষর *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = ফাঁকা;
        ssize_t n_arr = 0;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(files);
        প্রতিজ্ঞা(name);
        প্রতিজ্ঞা(gid_is_valid(gid));

        পূর্ণসংখ্যা ngroups_max = sysconf_ngroups_max();
        যদি (ngroups_max < 0)
                ফেরত ngroups_max;

        যদি (!GREEDY_REALLOC(arr, n_arr + 1))
                ফেরত -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = ফাঁকা;
                কাঠামো group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                যদি (r == -ENOENT)
                        চালু;
                যদি (r < 0)
                        ফেরত r;

                যখন ((r = fgetgrent_sane(f, &gr)) > 0) {
                        যদি (gid == gr->gr_gid)
                                চালু;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        যদি (gid_list_has(arr, n_arr, gr->gr_gid))
                                চালু;

                        যদি (!strv_contains(gr->gr_mem, name))
                                চালু;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        যদি (n_arr >= ngroups_max)
                                ফেরত -ENOBUFS;

                        যদি (!GREEDY_REALLOC(arr, n_arr + 1))
                                ফেরত -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret)
                *ret = TAKE_PTR(arr);
        ফেরত n_arr;
}

পূর্ণসংখ্যা getgrouplist_malloc(ধ্রুবক অক্ষর *user, gid_t gid, gid_t **ret) {
#যদি BUILD_STATIC
        ফেরত lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#অন্যথা
        পূর্ণসংখ্যা ngroups_max = sysconf_ngroups_max();
        যদি (ngroups_max < 0)
                ফেরত ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        যদি (!gids)
                ফেরত -ENOMEM;

        পূর্ণসংখ্যা k = ngroups_max;
        যদি (getgrouplist(user, gid, gids, &k) < 0)
                ফেরত -EIO;  /* getgrouplist does not set errno */

        যদি (ret)
                *ret = TAKE_PTR(gids);
        ফেরত k;
#endif
}

পূর্ণসংখ্যা initgroups_wrapper(ধ্রুবক অক্ষর *user, gid_t gid) {
#যদি BUILD_STATIC
        _cleanup_free_ gid_t *groups = ফাঁকা;
        পূর্ণসংখ্যা r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        যদি (n_groups <= 0)
                ফেরত n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        জন্য (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                যদি (r != -EINVAL)
                        বিরতি;
        }

        ফেরত r;
#অন্যথা
        ফেরত RET_NERRNO(initgroups(user, gid));
#endif
}

#যদি !BUILD_STATIC

স্থির size_t getgr_বাফার_আকার(শূন্য) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        ফেরত bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

পূর্ণসংখ্যা getgrnam_malloc(ধ্রুবক অক্ষর *name, কাঠামো group **ret) {
        যদি (isempty(name))
                ফেরত -EINVAL;

#যদি BUILD_STATIC
        ফেরত lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#অন্যথা
        size_t bufsize = getgr_বাফার_আকার();
        পূর্ণসংখ্যা r;

        জন্য (;;) {
                _cleanup_free_ শূন্য *buf = ফাঁকা;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(আকার(কাঠামো group)));
                buf = malloc0(ALIGN(আকার(কাঠামো group)) + bufsize);
                যদি (!buf)
                        ফেরত -ENOMEM;

                কাঠামো group *gr = ফাঁকা;
                r = getgrnam_r(name, buf, (অক্ষর*) buf + ALIGN(আকার(কাঠামো group)), bufsize, &gr);
                যদি (r == 0) {
                        যদি (gr) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ফেরত 0;
                        }

                        ফেরত -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        ফেরত -ESRCH;
                যদি (r != ERANGE)
                        ফেরত -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(আকার(কাঠামো group)))
                        ফেরত -ENOMEM;
                bufsize *= 2;
        }
#endif
}

পূর্ণসংখ্যা getgrgid_malloc(gid_t gid, কাঠামো group **ret) {
        যদি (!gid_is_valid(gid))
                ফেরত -EINVAL;

#যদি BUILD_STATIC
        ফেরত lookup_grent_in_files(GROUP_FILES, /* name= */ ফাঁকা, gid, ret);
#অন্যথা
        size_t bufsize = getgr_বাফার_আকার();
        পূর্ণসংখ্যা r;

        জন্য (;;) {
                _cleanup_free_ শূন্য *buf = ফাঁকা;

                /* Silence static analyzers */
                প্রতিজ্ঞা(bufsize <= SIZE_MAX - ALIGN(আকার(কাঠামো group)));
                buf = malloc0(ALIGN(আকার(কাঠামো group)) + bufsize);
                যদি (!buf)
                        ফেরত -ENOMEM;

                কাঠামো group *gr = ফাঁকা;
                r = getgrgid_r(gid, buf, (অক্ষর*) buf + ALIGN(আকার(কাঠামো group)), bufsize, &gr);
                যদি (r == 0) {
                        যদি (gr) {
                                যদি (ret)
                                        *ret = TAKE_PTR(buf);
                                ফেরত 0;
                        }

                        ফেরত -ESRCH;
                }

                প্রতিজ্ঞা(r > 0);

                যদি (errno_is_user_doesnt_exist(r))
                        ফেরত -ESRCH;
                যদি (r != ERANGE)
                        ফেরত -r;

                যদি (bufsize > SIZE_MAX/2 - ALIGN(আকার(কাঠামো group)))
                        ফেরত -ENOMEM;
                bufsize *= 2;
        }
#endif
}
