/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <sys/file.h>
#شامل <sys/stat.h>
#شامل <unistd.h>
#شامل <utmpx.h>

#شامل "sd-messages.h"

#شامل "alloc-util.h"
#شامل "chase.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "format-util.h"
#شامل "lock-util.h"
#شامل "log.h"
#شامل "mkdir.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "terminal-util.h"
#شامل "user-util.h"
#شامل "utf8.h"

#define DEFINE_STRERROR_ACCOUNT(type)                                   \
        ثابت نویسه* strerror_##type(                                    \
                        صحیح errnum,                                     \
                        نویسه *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                انتخاب (errnum) {                                       \
                حالت ESRCH:                                             \
                        بازگشت "Unknown " STRINGIFY(type);              \
                حالت ENOEXEC:                                           \
                        بازگشت "Not a system " STRINGIFY(type);         \
                پیشفرض:                                                \
                        بازگشت strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        اگر (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                بازگشت false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        اگر (uid == (uid_t) UINT32_C(0xFFFF))
                بازگشت false;

        بازگشت true;
}

صحیح parse_uid(ثابت نویسه *s, uid_t *ret) {
        uint32_t uid = 0;
        صحیح r;

        أكد(s);

        assert_cc(اندازه(uid_t) == اندازه(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = safe_atou32_full(s, 10
                             | SAFE_ATO_REFUSE_PLUS_MINUS
                             | SAFE_ATO_REFUSE_LEADING_ZERO
                             | SAFE_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        اگر (r < 0)
                بازگشت r;

        اگر (!uid_is_valid(uid))
                بازگشت -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        اگر (ret)
                *ret = uid;

        بازگشت 0;
}

صحیح parse_uid_range(ثابت نویسه *s, uid_t *ret_lower, uid_t *ret_upper) {
        _cleanup_free_ نویسه *word = پوچ;
        uid_t l, u;
        صحیح r;

        أكد(s);
        أكد(ret_lower);
        أكد(ret_upper);

        r = extract_first_word(&s, &word, "-", EXTRACT_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EINVAL;

        r = parse_uid(word, &l);
        اگر (r < 0)
                بازگشت r;

        /* Check for the upper bound and extract it if needed */
        اگر (!s)
                /* Single number with no dash. */
                u = l;
        وگرنه اگر (!*s)
                /* Trailing dash is an error. */
                بازگشت -EINVAL;
        وگرنه {
                r = parse_uid(s, &u);
                اگر (r < 0)
                        بازگشت r;

                اگر (l > u)
                        بازگشت -EINVAL;
        }

        *ret_lower = l;
        *ret_upper = u;
        بازگشت 0;
}

نویسه* getlogname_malloc(تهی) {
        uid_t uid;
        ساختار stat st;

        اگر (isatty_safe(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        وگرنه
                uid = getuid();

        بازگشت uid_to_name(uid);
}

نویسه* getusername_malloc(تهی) {
        ثابت نویسه *e;

        e = secure_getenv("USER");
        اگر (e)
                بازگشت strdup(e);

        بازگشت uid_to_name(getuid());
}

bool is_nologin_shell(ثابت نویسه *shell) {
        بازگشت PATH_IN_SET(shell,
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

bool shell_is_placeholder(ثابت نویسه *shell) {
        بازگشت isempty(shell) || is_nologin_shell(shell);
}

ثابت نویسه* default_ریشه_shell_at(صحیح rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        أكد(rfd >= 0 || rfd == AT_FDCWD);

        صحیح r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, پوچ, پوچ);
        اگر (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        اگر (r > 0)
                بازگشت DEFAULT_USER_SHELL;

        بازگشت "/bin/sh";
}

ثابت نویسه* default_ریشه_shell(ثابت نویسه *ریشه) {
        _cleanup_close_ صحیح rfd = -EBADF;

        rfd = open(empty_to_ریشه(ریشه), O_CLOEXEC | O_DIRECTORY | O_PATH);
        اگر (rfd < 0)
                بازگشت "/bin/sh";

        بازگشت default_ریشه_shell_at(rfd);
}

ایستا صحیح return_user_creds(
                ثابت نویسه *username,
                uid_t uid, gid_t gid,
                ثابت نویسه *home,
                ثابت نویسه *shell,
                نویسه **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                نویسه **ret_home,
                نویسه **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ نویسه *s1 = پوچ, *s2 = پوچ, *s3 = پوچ;
        صحیح r;

        اگر (ret_username) {
                r = strdup_to(&s1, username);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_home) {
                r = strdup_to(&s2, home);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_shell) {
                r = strdup_to(&s3, shell);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_username)
                *ret_username = TAKE_PTR(s1);
        اگر (ret_uid)
                *ret_uid = uid;
        اگر (ret_gid)
                *ret_gid = gid;
        اگر (ret_home)
                *ret_home = TAKE_PTR(s2);
        اگر (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        بازگشت 0;
}

ایستا صحیح synthesize_user_creds(
                ثابت نویسه *username,
                UserCredsFlags flags,
                نویسه **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                نویسه **ret_home,
                نویسه **ret_shell) {
        أكد(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        اگر (STR_IN_SET(username, "root", "0"))
                بازگشت return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_ریشه_shell(پوچ) : پوچ,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        اگر (STR_IN_SET(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                بازگشت return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? پوچ : "/",
                                         FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? پوچ : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        بازگشت -ENOMEDIUM;
}

صحیح get_user_creds(
                ثابت نویسه *username,
                UserCredsFlags flags,
                نویسه **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                نویسه **ret_home,
                نویسه **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ساختار passwd *pw = پوچ;
        صحیح r;

        أكد(username);
        أكد((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        اگر (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS) ||
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
                اگر (r >= 0)
                        بازگشت 0;
                اگر (r != -ENOMEDIUM) /* not a username we can synthesize */
                        بازگشت r;
        }

        اگر (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                اگر (r >= 0)
                        username = pw->pw_name;

                وگرنه اگر (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        اگر (ret_username)
                                *ret_username = پوچ;
                        اگر (ret_uid)
                                *ret_uid = u;
                        بازگشت 0;
                }
        } وگرنه
                r = getpwnam_malloc(username, &pw);

        اگر (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                اگر (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        بازگشت 0;

                بازگشت r;
        }

        اگر (ret_uid && !uid_is_valid(pw->pw_uid))
                بازگشت -EBADMSG;

        اگر (ret_gid && !gid_is_valid(pw->pw_gid))
                بازگشت -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ثابت نویسه *h =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && empty_or_ریشه(pw->pw_dir)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_dir) || !path_is_absolute(pw->pw_dir)))
                ? پوچ : pw->pw_dir;

        ثابت نویسه *s =
                (FLAGS_SET(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_SET(flags, USER_CREDS_CLEAN) && (!path_is_valid(pw->pw_shell) || !path_is_absolute(pw->pw_shell)))
                ? پوچ : pw->pw_shell;

        بازگشت return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ایستا صحیح synthesize_group_creds(ثابت نویسه *groupname, نویسه **ret_name, gid_t *ret_gid) {
        أكد(groupname);

        gid_t id;
        ثابت نویسه *n;
        صحیح r;

        اگر (STR_IN_SET(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } وگرنه اگر (STR_IN_SET(groupname, NOBODY_GROUP_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_GROUP_NAME;
        } وگرنه
                بازگشت -ENOMEDIUM;

        r = strdup_to_full(ret_name, n);
        اگر (r < 0)
                بازگشت r;
        اگر (ret_gid)
                *ret_gid = id;
        بازگشت 0;
}

صحیح get_group_creds(ثابت نویسه *groupname, UserCredsFlags flags, نویسه **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ساختار group *gr = پوچ;
        gid_t id;
        صحیح r;

        أكد(groupname);

        اگر (!FLAGS_SET(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_group_creds(groupname, ret_name, ret_gid);
                اگر (r >= 0)
                        بازگشت 0;
                اگر (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        بازگشت r;
        }

        اگر (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                اگر (r >= 0)
                        groupname = gr->gr_name;
                وگرنه اگر (FLAGS_SET(flags, USER_CREDS_ALLOW_MISSING)) {
                        اگر (ret_gid)
                                *ret_gid = id;
                        اگر (ret_name)
                                *ret_name = پوچ;
                        بازگشت 0;
                }
        } وگرنه
                r = getgrnam_malloc(groupname, &gr);

        اگر (r < 0) {
                اگر (FLAGS_SET(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_group_creds(groupname, ret_name, ret_gid) >= 0)
                        بازگشت 0;
                بازگشت r;
        }

        اگر (ret_gid && !gid_is_valid(gr->gr_gid))
                بازگشت -EBADMSG;

        r = strdup_to_full(ret_name, groupname);
        اگر (r < 0)
                بازگشت r;
        اگر (ret_gid)
                *ret_gid = gr->gr_gid;
        بازگشت 0;
}

نویسه* uid_to_name(uid_t uid) {
        نویسه *ret;
        صحیح r;

        /* Shortcut things to avoid NSS lookups */
        اگر (uid == 0)
                بازگشت strdup("root");
        اگر (uid == UID_NOBODY && synthesize_nobody())
                بازگشت strdup(NOBODY_USER_NAME);

        اگر (uid_is_valid(uid)) {
                _cleanup_free_ ساختار passwd *pw = پوچ;

                r = getpwuid_malloc(uid, &pw);
                اگر (r >= 0)
                        بازگشت strdup(pw->pw_name);
        }

        اگر (asprintf(&ret, UID_FMT, uid) < 0)
                بازگشت پوچ;

        بازگشت ret;
}

نویسه* gid_to_name(gid_t gid) {
        نویسه *ret;
        صحیح r;

        اگر (gid == 0)
                بازگشت strdup("root");
        اگر (gid == GID_NOBODY && synthesize_nobody())
                بازگشت strdup(NOBODY_GROUP_NAME);

        اگر (gid_is_valid(gid)) {
                _cleanup_free_ ساختار group *gr = پوچ;

                r = getgrgid_malloc(gid, &gr);
                اگر (r >= 0)
                        بازگشت strdup(gr->gr_name);
        }

        اگر (asprintf(&ret, GID_FMT, gid) < 0)
                بازگشت پوچ;

        بازگشت ret;
}

ایستا bool gid_list_has(ثابت gid_t *list, size_t size, gid_t val) {
        أكد(list || size == 0);

        FOREACH_ARRAY(i, list, size)
                اگر (*i == val)
                        بازگشت true;

        بازگشت false;
}

صحیح in_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = پوچ;
        صحیح ngroups;

        اگر (getgid() == gid)
                بازگشت 1;

        اگر (getegid() == gid)
                بازگشت 1;

        اگر (!gid_is_valid(gid))
                بازگشت -EINVAL;

        ngroups = getgroups_alloc(&gids);
        اگر (ngroups < 0)
                بازگشت ngroups;

        بازگشت gid_list_has(gids, ngroups, gid);
}

صحیح in_group(ثابت نویسه *name) {
        صحیح r;
        gid_t gid;

        r = get_group_creds(name, /* flags= */ 0, /* ret_name= */ پوچ, &gid);
        اگر (r < 0)
                بازگشت r;

        بازگشت in_gid(gid);
}

صحیح merge_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        أكد(size1 == 0 || list1);
        أكد(size2 == 0 || list2);
        أكد(ret);

        اگر (size2 > INT_MAX - size1)
                بازگشت -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        اگر (!buf)
                بازگشت -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        برای (size_t i = 0; i < size1; i++)
                اگر (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        برای (size_t i = 0; i < size2; i++)
                اگر (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        بازگشت (صحیح)nresult;
}

صحیح getgroups_alloc(gid_t **ret) {
        صحیح ngroups = 8;

        أكد(ret);

        برای (unsigned attempt = 0;;) {
                _cleanup_free_ gid_t *p = پوچ;

                p = new(gid_t, ngroups);
                اگر (!p)
                        بازگشت -ENOMEM;

                ngroups = getgroups(ngroups, p);
                اگر (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        بازگشت ngroups;
                }
                اگر (ngroups == 0)
                        بشکن;
                اگر (errno != EINVAL)
                        بازگشت -errno;

                /* Give up eventually */
                اگر (attempt++ > 10)
                        بازگشت -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, پوچ);
                اگر (ngroups < 0)
                        بازگشت -errno;
                اگر (ngroups == 0)
                        بشکن;
        }

        *ret = پوچ;
        بازگشت 0;
}

صحیح get_home_dir(نویسه **ret) {
        _cleanup_free_ ساختار passwd *p = پوچ;
        ثابت نویسه *e;
        uid_t u;
        صحیح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        اگر (e && path_is_valid(e) && path_is_absolute(e))
                برو found;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        اگر (u == 0) {
                e = "/root";
                برو found;
        }
        اگر (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                برو found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        اگر (r < 0)
                بازگشت r;

        e = p->pw_dir;
        اگر (!path_is_valid(e) || !path_is_absolute(e))
                بازگشت -EINVAL;

 found:
        بازگشت path_simplify_alloc(e, ret);
}

صحیح get_shell(نویسه **ret) {
        _cleanup_free_ ساختار passwd *p = پوچ;
        ثابت نویسه *e;
        uid_t u;
        صحیح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        اگر (e && path_is_valid(e) && path_is_absolute(e))
                برو found;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        اگر (u == 0) {
                e = default_ریشه_shell(پوچ);
                برو found;
        }
        اگر (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                برو found;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        اگر (r < 0)
                بازگشت r;

        e = p->pw_shell;
        اگر (!path_is_valid(e) || !path_is_absolute(e))
                بازگشت -EINVAL;

 found:
        بازگشت path_simplify_alloc(e, ret);
}

صحیح fully_set_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids) {
        صحیح r;

        أكد(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        اگر (r < 0)
                بازگشت r;

        اگر (gid_is_valid(gid))
                اگر (setresgid(gid, gid, gid) < 0)
                        بازگشت -errno;

        اگر (uid_is_valid(uid))
                اگر (setresuid(uid, uid, uid) < 0)
                        بازگشت -errno;

        بازگشت 0;
}

صحیح take_etc_passwd_lock(ثابت نویسه *ریشه) {
        صحیح r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ نویسه *path = path_join(ریشه, ETC_PASSWD_LOCK_PATH);
        اگر (!path)
                بازگشت log_oom_debug();

        (تهی) mkdir_parents(path, 0755);

        _cleanup_close_ صحیح fd = open(path, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        اگر (fd < 0)
                بازگشت log_debug_errno(errno, "Cannot open %s: %m", path);

        r = unposix_lock(fd, LOCK_EX);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Locking %s failed: %m", path);

        بازگشت TAKE_FD(fd);
}

bool valid_user_group_name(ثابت نویسه *u, ValidUserFlags flags) {
        ثابت نویسه *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        اگر (isempty(u)) /* An empty user name is never valid */
                بازگشت false;

        اگر (parse_uid(u, پوچ) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                بازگشت FLAGS_SET(flags, VALID_USER_ALLOW_NUMERIC);

        اگر (FLAGS_SET(flags, VALID_USER_RELAX)) {

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

                اگر (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        بازگشت false;

                اگر (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        بازگشت false;

                اگر (string_has_cc(u, پوچ)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        بازگشت false;

                اگر (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        بازگشت false;

                اگر (in_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        بازگشت false;

                اگر (u[0] == '-' && in_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        بازگشت false;

                اگر (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        بازگشت false;

                /* Compare with strict result and warn if result doesn't match */
                اگر (FLAGS_SET(flags, VALID_USER_WARN) && !valid_user_group_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_MESSAGE("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_ITEM("USER_GROUP_NAME=%s", u),
                                   LOG_MESSAGE_ID(SD_MESSAGE_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } وگرنه {
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

                اگر (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        بازگشت false;

                برای (i = u+1; *i; i++)
                        اگر (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !IN_SET(*i, '_', '-'))
                                بازگشت false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                اگر (l > (size_t) sz) /* glibc: 256 */
                        بازگشت false;
                اگر (l > NAME_MAX) /* must fit in a filename: 255 */
                        بازگشت false;
                اگر (l > sizeof_field(ساختار utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        بازگشت false;
        }

        بازگشت true;
}

bool valid_gecos(ثابت نویسه *d) {

        اگر (!d)
                بازگشت false;

        اگر (!utf8_is_valid(d))
                بازگشت false;

        اگر (string_has_cc(d, پوچ))
                بازگشت false;

        /* Colons are used as field separators, and hence not OK */
        اگر (strchr(d, ':'))
                بازگشت false;

        بازگشت true;
}

نویسه* mangle_gecos(ثابت نویسه *d) {
        نویسه *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        اگر (!mangled)
                بازگشت پوچ;

        برای (نویسه *i = mangled; *i; i++) {
                صحیح len;

                اگر ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        ادامه;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                اگر (len < 0) {
                        *i = ' ';
                        ادامه;
                }

                i += len - 1;
        }

        بازگشت mangled;
}

bool valid_home(ثابت نویسه *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        اگر (isempty(p))
                بازگشت false;

        اگر (!utf8_is_valid(p))
                بازگشت false;

        اگر (string_has_cc(p, پوچ))
                بازگشت false;

        اگر (!path_is_absolute(p))
                بازگشت false;

        اگر (!path_is_normalized(p))
                بازگشت false;

        /* Colons are used as field separators, and hence not OK */
        اگر (strchr(p, ':'))
                بازگشت false;

        بازگشت true;
}

bool valid_shell(ثابت نویسه *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        اگر (!valid_home(p))
                بازگشت false;

        بازگشت !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

صحیح maybe_setgroups(size_t size, ثابت gid_t *list) {
        صحیح r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        اگر (size == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                اگر (!f && errno != ENOENT)
                        بازگشت -errno;
                اگر (f) {
                        r = safe_fgetc(f, /* ret= */ پوچ);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                بازگشت 0;
                        }
                }

                _cleanup_free_ نویسه *setgroups_content = پوچ;
                r = read_one_سطر_file("/proc/self/setgroups", &setgroups_content);
                اگر (r < 0 && r != -ENOENT)
                        بازگشت r;
                اگر (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        بازگشت 0;
                }
        }

        بازگشت RET_NERRNO(setgroups(size, list));
}

bool synthesize_nobody(تهی) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ایستا صحیح cache = -1;

        اگر (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        بازگشت cache;
}

صحیح putpwent_sane(ثابت ساختار passwd *pw, FILE *stream) {
        أكد(pw);
        أكد(stream);

        errno = 0;
        اگر (putpwent(pw, stream) != 0)
                بازگشت errno_or_else(EIO);

        بازگشت 0;
}

صحیح putspent_sane(ثابت ساختار spwd *sp, FILE *stream) {
        أكد(sp);
        أكد(stream);

        errno = 0;
        اگر (putspent(sp, stream) != 0)
                بازگشت errno_or_else(EIO);

        بازگشت 0;
}

صحیح putgrent_sane(ثابت ساختار group *gr, FILE *stream) {
        أكد(gr);
        أكد(stream);

        errno = 0;
        اگر (putgrent(gr, stream) != 0)
                بازگشت errno_or_else(EIO);

        بازگشت 0;
}

#اگر ENABLE_GSHADOW
صحیح putsgent_sane(ثابت ساختار sgrp *sg, FILE *stream) {
        أكد(sg);
        أكد(stream);

        errno = 0;
        اگر (putsgent(sg, stream) != 0)
                بازگشت errno_or_else(EIO);

        بازگشت 0;
}
#endif

صحیح fgetpwent_sane(FILE *stream, ساختار passwd **pw) {
        أكد(stream);
        أكد(pw);

        errno = 0;
        ساختار passwd *p = fgetpwent(stream);
        اگر (!p && !IN_SET(errno, 0, ENOENT))
                بازگشت -errno;

        *pw = p;
        بازگشت !!p;
}

صحیح fgetspent_sane(FILE *stream, ساختار spwd **sp) {
        أكد(stream);
        أكد(sp);

        errno = 0;
        ساختار spwd *s = fgetspent(stream);
        اگر (!s && !IN_SET(errno, 0, ENOENT))
                بازگشت -errno;

        *sp = s;
        بازگشت !!s;
}

صحیح fgetgrent_sane(FILE *stream, ساختار group **gr) {
        أكد(stream);
        أكد(gr);

        errno = 0;
        ساختار group *g = fgetgrent(stream);
        اگر (!g && !IN_SET(errno, 0, ENOENT))
                بازگشت -errno;

        *gr = g;
        بازگشت !!g;
}

#اگر ENABLE_GSHADOW
صحیح fgetsgent_sane(FILE *stream, ساختار sgrp **sg) {
        أكد(stream);
        أكد(sg);

        errno = 0;
        ساختار sgrp *s = fgetsgent(stream);
        اگر (!s && !IN_SET(errno, 0, ENOENT))
                بازگشت -errno;

        *sg = s;
        بازگشت !!s;
}
#endif

صحیح is_this_me(ثابت نویسه *username) {
        uid_t uid;
        صحیح r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, پوچ, &uid, پوچ, پوچ, پوچ);
        اگر (r < 0)
                بازگشت r;

        بازگشت uid == getuid();
}

ثابت نویسه* get_home_ریشه(تهی) {
        /* For debug purposes allow overriding where we look for home dirs */
        ثابت نویسه *e = secure_getenv("SYSTEMD_HOME_ROOT");
        اگر (e && path_is_absolute(e) && path_is_normalized(e))
                بازگشت e;

        بازگشت "/home";
}

ایستا صحیح copy_struct_passwd(ثابت ساختار passwd *pw, ساختار passwd **ret) {
        أكد(pw);
        أكد(ret);

        size_t need_bytes = اندازه(ساختار passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        نویسه *buf = malloc(need_bytes);
        اگر (!buf)
                بازگشت -ENOMEM;

        ساختار passwd *newpw = (ساختار passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        نویسه *p = buf + اندازه(ساختار passwd);

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
        بازگشت 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
صحیح lookup_pwent_in_files(
                نویسه * ثابت *files,
                ثابت نویسه *name,
                uid_t uid,
                ساختار passwd **ret) {

        صحیح r;

        أكد(files);
        أكد(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = پوچ;
                ساختار passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                اگر (r == -ENOENT)
                        ادامه;
                اگر (r < 0)
                        بازگشت r;

                وقتی ((r = fgetpwent_sane(f, &pw)) > 0) {
                        اگر (name && !streq_ptr(pw->pw_name, name))
                                ادامه;
                        اگر (uid_is_valid(uid) && pw->pw_uid != uid)
                                ادامه;
                        اگر (ret)
                                بازگشت copy_struct_passwd(pw, ret);
                        بازگشت 0;
                }
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت -ESRCH;
}

#اگر !BUILD_STATIC

ایستا size_t getpw_buffer_size(تهی) {
        long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        بازگشت bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ایستا bool errno_is_user_doesnt_exist(صحیح error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        بازگشت IN_SET(abs(error), ENOENT, ESRCH, EBADF, EPERM);
}

#endif

صحیح getpwnam_malloc(ثابت نویسه *name, ساختار passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        اگر (isempty(name))
                بازگشت -EINVAL;

#اگر BUILD_STATIC
        بازگشت lookup_pwent_in_files(PASSWD_FILES, name, UID_INVALID, ret);
#وگرنه
        size_t bufsize = getpw_buffer_size();
        صحیح r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        برای (;;) {
                _cleanup_free_ تهی *buf = پوچ;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(اندازه(ساختار passwd)));
                buf = malloc0(ALIGN(اندازه(ساختار passwd)) + bufsize);
                اگر (!buf)
                        بازگشت -ENOMEM;

                ساختار passwd *pw = پوچ;
                r = getpwnam_r(name, buf, (نویسه*) buf + ALIGN(اندازه(ساختار passwd)), bufsize, &pw);
                اگر (r == 0) {
                        اگر (pw) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                بازگشت 0;
                        }

                        بازگشت -ESRCH;
                }

                أكد(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                اگر (errno_is_user_doesnt_exist(r))
                        بازگشت -ESRCH;
                اگر (r != ERANGE)
                        بازگشت -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(اندازه(ساختار passwd)))
                        بازگشت -ENOMEM;
                bufsize *= 2;
        }
#endif
}

صحیح getpwuid_malloc(uid_t uid, ساختار passwd **ret) {
        اگر (!uid_is_valid(uid))
                بازگشت -EINVAL;

#اگر BUILD_STATIC
        بازگشت lookup_pwent_in_files(PASSWD_FILES, /* name= */ پوچ, uid, ret);
#وگرنه
        size_t bufsize = getpw_buffer_size();
        صحیح r;

        برای (;;) {
                _cleanup_free_ تهی *buf = پوچ;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(اندازه(ساختار passwd)));
                buf = malloc0(ALIGN(اندازه(ساختار passwd)) + bufsize);
                اگر (!buf)
                        بازگشت -ENOMEM;

                ساختار passwd *pw = پوچ;
                r = getpwuid_r(uid, buf, (نویسه*) buf + ALIGN(اندازه(ساختار passwd)), bufsize, &pw);
                اگر (r == 0) {
                        اگر (pw) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                بازگشت 0;
                        }

                        بازگشت -ESRCH;
                }

                أكد(r > 0);

                اگر (errno_is_user_doesnt_exist(r))
                        بازگشت -ESRCH;
                اگر (r != ERANGE)
                        بازگشت -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(اندازه(ساختار passwd)))
                        بازگشت -ENOMEM;
                bufsize *= 2;
        }
#endif
}

ایستا صحیح copy_struct_group(ثابت ساختار group *gr, ساختار group **ret) {
        أكد(gr);
        أكد(ret);

        size_t need_bytes = اندازه(ساختار group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_bytes += اندازه(نویسه*) + strlen(*s) + 1;
                n_mem++;
        }
        need_bytes += اندازه(نویسه*);  /* NULL terminator for gr_mem */

        نویسه *buf = malloc(need_bytes);
        اگر (!buf)
                بازگشت -ENOMEM;

        ساختار group *newgr = (ساختار group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(اندازه(ساختار group) % alignof(نویسه*) == 0);

        نویسه *p = buf + اندازه(ساختار group) + (n_mem + 1) * اندازه(نویسه*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (نویسه**) (buf + اندازه(ساختار group));
        برای (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = پوچ;

        *ret = newgr;
        بازگشت 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
صحیح lookup_grent_in_files(
                نویسه * ثابت *files,
                ثابت نویسه *name,
                gid_t gid,
                ساختار group **ret) {

        صحیح r;

        أكد(files);
        أكد(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = پوچ;
                ساختار group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                اگر (r == -ENOENT)
                        ادامه;
                اگر (r < 0)
                        بازگشت r;

                وقتی ((r = fgetgrent_sane(f, &gr)) > 0) {
                        اگر (name && !streq_ptr(gr->gr_name, name))
                                ادامه;
                        اگر (gid_is_valid(gid) && gr->gr_gid != gid)
                                ادامه;
                        اگر (ret)
                                بازگشت copy_struct_group(gr, ret);
                        بازگشت 0;
                }
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت -ESRCH;
}

صحیح sysconf_ngroups_max(تهی) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        long ngroups_max = sysconf(_SC_NGROUPS_MAX);
        اگر (ngroups_max <= 0)
                بازگشت errno_or_else(EOPNOTSUPP);
        اگر (ngroups_max > INT_MAX)
                بازگشت -ERANGE;
        بازگشت ngroups_max;
}

ssize_t lookup_groups_in_files(
                نویسه * ثابت *files,
                ثابت نویسه *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = پوچ;
        ssize_t n_arr = 0;
        صحیح r;

        أكد(files);
        أكد(name);
        أكد(gid_is_valid(gid));

        صحیح ngroups_max = sysconf_ngroups_max();
        اگر (ngroups_max < 0)
                بازگشت ngroups_max;

        اگر (!GREEDY_REALLOC(arr, n_arr + 1))
                بازگشت -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = پوچ;
                ساختار group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                اگر (r == -ENOENT)
                        ادامه;
                اگر (r < 0)
                        بازگشت r;

                وقتی ((r = fgetgrent_sane(f, &gr)) > 0) {
                        اگر (gid == gr->gr_gid)
                                ادامه;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        اگر (gid_list_has(arr, n_arr, gr->gr_gid))
                                ادامه;

                        اگر (!strv_contains(gr->gr_mem, name))
                                ادامه;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        اگر (n_arr >= ngroups_max)
                                بازگشت -ENOBUFS;

                        اگر (!GREEDY_REALLOC(arr, n_arr + 1))
                                بازگشت -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret)
                *ret = TAKE_PTR(arr);
        بازگشت n_arr;
}

صحیح getgrouplist_malloc(ثابت نویسه *user, gid_t gid, gid_t **ret) {
#اگر BUILD_STATIC
        بازگشت lookup_groups_in_files(GROUP_FILES, user, gid, ret);
#وگرنه
        صحیح ngroups_max = sysconf_ngroups_max();
        اگر (ngroups_max < 0)
                بازگشت ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        اگر (!gids)
                بازگشت -ENOMEM;

        صحیح k = ngroups_max;
        اگر (getgrouplist(user, gid, gids, &k) < 0)
                بازگشت -EIO;  /* getgrouplist does not set errno */

        اگر (ret)
                *ret = TAKE_PTR(gids);
        بازگشت k;
#endif
}

صحیح initgroups_wrapper(ثابت نویسه *user, gid_t gid) {
#اگر BUILD_STATIC
        _cleanup_free_ gid_t *groups = پوچ;
        صحیح r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        اگر (n_groups <= 0)
                بازگشت n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        برای (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                اگر (r != -EINVAL)
                        بشکن;
        }

        بازگشت r;
#وگرنه
        بازگشت RET_NERRNO(initgroups(user, gid));
#endif
}

#اگر !BUILD_STATIC

ایستا size_t getgr_buffer_size(تهی) {
        long bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        بازگشت bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#endif

صحیح getgrnam_malloc(ثابت نویسه *name, ساختار group **ret) {
        اگر (isempty(name))
                بازگشت -EINVAL;

#اگر BUILD_STATIC
        بازگشت lookup_grent_in_files(GROUP_FILES, name, GID_INVALID, ret);
#وگرنه
        size_t bufsize = getgr_buffer_size();
        صحیح r;

        برای (;;) {
                _cleanup_free_ تهی *buf = پوچ;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(اندازه(ساختار group)));
                buf = malloc0(ALIGN(اندازه(ساختار group)) + bufsize);
                اگر (!buf)
                        بازگشت -ENOMEM;

                ساختار group *gr = پوچ;
                r = getgrnam_r(name, buf, (نویسه*) buf + ALIGN(اندازه(ساختار group)), bufsize, &gr);
                اگر (r == 0) {
                        اگر (gr) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                بازگشت 0;
                        }

                        بازگشت -ESRCH;
                }

                أكد(r > 0);

                اگر (errno_is_user_doesnt_exist(r))
                        بازگشت -ESRCH;
                اگر (r != ERANGE)
                        بازگشت -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(اندازه(ساختار group)))
                        بازگشت -ENOMEM;
                bufsize *= 2;
        }
#endif
}

صحیح getgrgid_malloc(gid_t gid, ساختار group **ret) {
        اگر (!gid_is_valid(gid))
                بازگشت -EINVAL;

#اگر BUILD_STATIC
        بازگشت lookup_grent_in_files(GROUP_FILES, /* name= */ پوچ, gid, ret);
#وگرنه
        size_t bufsize = getgr_buffer_size();
        صحیح r;

        برای (;;) {
                _cleanup_free_ تهی *buf = پوچ;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(اندازه(ساختار group)));
                buf = malloc0(ALIGN(اندازه(ساختار group)) + bufsize);
                اگر (!buf)
                        بازگشت -ENOMEM;

                ساختار group *gr = پوچ;
                r = getgrgid_r(gid, buf, (نویسه*) buf + ALIGN(اندازه(ساختار group)), bufsize, &gr);
                اگر (r == 0) {
                        اگر (gr) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                بازگشت 0;
                        }

                        بازگشت -ESRCH;
                }

                أكد(r > 0);

                اگر (errno_is_user_doesnt_exist(r))
                        بازگشت -ESRCH;
                اگر (r != ERANGE)
                        بازگشت -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(اندازه(ساختار group)))
                        بازگشت -ENOMEM;
                bufsize *= 2;
        }
#endif
}
