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

#تعریف_کرو DEFINE_STRERROR_ACCOUNT(type)                                   \
        ثابت حرف* strerror_##type(                                    \
                        صحیح errnum,                                     \
                        حرف *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                انتخاب (errnum) {                                       \
                حالت ESRCH:                                             \
                        واپس "Unknown " STRINGIFY(type);              \
                حالت ENOEXEC:                                           \
                        واپس "Not a system " STRINGIFY(type);         \
                طےشدہ:                                                \
                        واپس strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        اگر (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                واپس false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        اگر (uid == (uid_t) UINT32_C(0xFFFF))
                واپس false;

        واپس true;
}

صحیح parse_uid(ثابت حرف *s, uid_t *ret) {
        uint32_t uid = 0;
        صحیح r;

        أكد(s);

        assert_cc(حجم(uid_t) == حجم(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = محفوظ_atou32_بھرا(s, 10
                             | محفوظ_ATO_REFUSE_PLUS_MINUS
                             | محفوظ_ATO_REFUSE_LEADING_ZERO
                             | محفوظ_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        اگر (r < 0)
                واپس r;

        اگر (!uid_is_valid(uid))
                واپس -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        اگر (ret)
                *ret = uid;

        واپس 0;
}

صحیح parse_uid_range(ثابت حرف *s, uid_t *ret_زیریں, uid_t *ret_بالائی) {
        _cleanup_free_ حرف *لفظ = تہی;
        uid_t l, u;
        صحیح r;

        أكد(s);
        أكد(ret_زیریں);
        أكد(ret_بالائی);

        r = نکالو_پہلا_لفظ(&s, &لفظ, "-", نکالو_DONT_COALESCE_SEPARATORS);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EINVAL;

        r = parse_uid(لفظ, &l);
        اگر (r < 0)
                واپس r;

        /* Check for the upper bound and extract it if needed */
        اگر (!s)
                /* Single number with no dash. */
                u = l;
        ورنہ اگر (!*s)
                /* Trailing dash is an error. */
                واپس -EINVAL;
        ورنہ {
                r = parse_uid(s, &u);
                اگر (r < 0)
                        واپس r;

                اگر (l > u)
                        واپس -EINVAL;
        }

        *ret_زیریں = l;
        *ret_بالائی = u;
        واپس 0;
}

حرف* getlogname_malloc(خالی) {
        uid_t uid;
        ساخت stat st;

        اگر (isatty_محفوظ(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        ورنہ
                uid = getuid();

        واپس uid_مقصد_name(uid);
}

حرف* getusername_malloc(خالی) {
        ثابت حرف *e;

        e = secure_getenv("USER");
        اگر (e)
                واپس strdup(e);

        واپس uid_مقصد_name(getuid());
}

bool is_nologin_shell(ثابت حرف *shell) {
        واپس راستہ_اندر_سیٹ(shell,
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

bool shell_is_placeholder(ثابت حرف *shell) {
        واپس isempty(shell) || is_nologin_shell(shell);
}

ثابت حرف* default_جڑ_shell_at(صحیح rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        أكد(rfd >= 0 || rfd == AT_FDCWD);

        صحیح r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, تہی, تہی);
        اگر (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        اگر (r > 0)
                واپس DEFAULT_USER_SHELL;

        واپس "/bin/sh";
}

ثابت حرف* default_جڑ_shell(ثابت حرف *جڑ) {
        _cleanup_close_ صحیح rfd = -EBADF;

        rfd = open(خالی_خانے_مقصد_جڑ(جڑ), O_CLOEXEC | O_DIRECTORY | O_راستہ);
        اگر (rfd < 0)
                واپس "/bin/sh";

        واپس default_جڑ_shell_at(rfd);
}

ساکن صحیح return_user_creds(
                ثابت حرف *username,
                uid_t uid, gid_t gid,
                ثابت حرف *home,
                ثابت حرف *shell,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ حرف *s1 = تہی, *s2 = تہی, *s3 = تہی;
        صحیح r;

        اگر (ret_username) {
                r = strdup_مقصد(&s1, username);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_home) {
                r = strdup_مقصد(&s2, home);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_shell) {
                r = strdup_مقصد(&s3, shell);
                اگر (r < 0)
                        واپس r;
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
        واپس 0;
}

ساکن صحیح synthesize_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {
        أكد(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        اگر (STR_اندر_سیٹ(username, "root", "0"))
                واپس return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_جڑ_shell(تہی) : تہی,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        اگر (STR_اندر_سیٹ(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                واپس return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_سیٹ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? تہی : "/",
                                         FLAGS_سیٹ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? تہی : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        واپس -ENOMEDIUM;
}

صحیح get_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ ساخت passwd *pw = تہی;
        صحیح r;

        أكد(username);
        أكد((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        اگر (!FLAGS_سیٹ(flags, USER_CREDS_PREFER_NSS) ||
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
                        واپس 0;
                اگر (r != -ENOMEDIUM) /* not a username we can synthesize */
                        واپس r;
        }

        اگر (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                اگر (r >= 0)
                        username = pw->pw_name;

                ورنہ اگر (FLAGS_سیٹ(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        اگر (ret_username)
                                *ret_username = تہی;
                        اگر (ret_uid)
                                *ret_uid = u;
                        واپس 0;
                }
        } ورنہ
                r = getpwnam_malloc(username, &pw);

        اگر (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                اگر (FLAGS_سیٹ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        واپس 0;

                واپس r;
        }

        اگر (ret_uid && !uid_is_valid(pw->pw_uid))
                واپس -EBADMSG;

        اگر (ret_gid && !gid_is_valid(pw->pw_gid))
                واپس -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ثابت حرف *h =
                (FLAGS_سیٹ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && خالی_خانے_or_جڑ(pw->pw_dir)) ||
                (FLAGS_سیٹ(flags, USER_CREDS_CLEAN) && (!راستہ_is_valid(pw->pw_dir) || !راستہ_is_absolute(pw->pw_dir)))
                ? تہی : pw->pw_dir;

        ثابت حرف *s =
                (FLAGS_سیٹ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_سیٹ(flags, USER_CREDS_CLEAN) && (!راستہ_is_valid(pw->pw_shell) || !راستہ_is_absolute(pw->pw_shell)))
                ? تہی : pw->pw_shell;

        واپس return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ساکن صحیح synthesize_گروہ_creds(ثابت حرف *groupname, حرف **ret_name, gid_t *ret_gid) {
        أكد(groupname);

        gid_t id;
        ثابت حرف *n;
        صحیح r;

        اگر (STR_اندر_سیٹ(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } ورنہ اگر (STR_اندر_سیٹ(groupname, NOBODY_گروہ_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_گروہ_NAME;
        } ورنہ
                واپس -ENOMEDIUM;

        r = strdup_مقصد_بھرا(ret_name, n);
        اگر (r < 0)
                واپس r;
        اگر (ret_gid)
                *ret_gid = id;
        واپس 0;
}

صحیح get_گروہ_creds(ثابت حرف *groupname, UserCredsFlags flags, حرف **ret_name, gid_t *ret_gid) {
        _cleanup_free_ ساخت group *gr = تہی;
        gid_t id;
        صحیح r;

        أكد(groupname);

        اگر (!FLAGS_سیٹ(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_گروہ_creds(groupname, ret_name, ret_gid);
                اگر (r >= 0)
                        واپس 0;
                اگر (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        واپس r;
        }

        اگر (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                اگر (r >= 0)
                        groupname = gr->gr_name;
                ورنہ اگر (FLAGS_سیٹ(flags, USER_CREDS_ALLOW_MISSING)) {
                        اگر (ret_gid)
                                *ret_gid = id;
                        اگر (ret_name)
                                *ret_name = تہی;
                        واپس 0;
                }
        } ورنہ
                r = getgrnam_malloc(groupname, &gr);

        اگر (r < 0) {
                اگر (FLAGS_سیٹ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_گروہ_creds(groupname, ret_name, ret_gid) >= 0)
                        واپس 0;
                واپس r;
        }

        اگر (ret_gid && !gid_is_valid(gr->gr_gid))
                واپس -EBADMSG;

        r = strdup_مقصد_بھرا(ret_name, groupname);
        اگر (r < 0)
                واپس r;
        اگر (ret_gid)
                *ret_gid = gr->gr_gid;
        واپس 0;
}

حرف* uid_مقصد_name(uid_t uid) {
        حرف *ret;
        صحیح r;

        /* Shortcut things to avoid NSS lookups */
        اگر (uid == 0)
                واپس strdup("root");
        اگر (uid == UID_NOBODY && synthesize_nobody())
                واپس strdup(NOBODY_USER_NAME);

        اگر (uid_is_valid(uid)) {
                _cleanup_free_ ساخت passwd *pw = تہی;

                r = getpwuid_malloc(uid, &pw);
                اگر (r >= 0)
                        واپس strdup(pw->pw_name);
        }

        اگر (asprintf(&ret, UID_FMT, uid) < 0)
                واپس تہی;

        واپس ret;
}

حرف* gid_مقصد_name(gid_t gid) {
        حرف *ret;
        صحیح r;

        اگر (gid == 0)
                واپس strdup("root");
        اگر (gid == GID_NOBODY && synthesize_nobody())
                واپس strdup(NOBODY_گروہ_NAME);

        اگر (gid_is_valid(gid)) {
                _cleanup_free_ ساخت group *gr = تہی;

                r = getgrgid_malloc(gid, &gr);
                اگر (r >= 0)
                        واپس strdup(gr->gr_name);
        }

        اگر (asprintf(&ret, GID_FMT, gid) < 0)
                واپس تہی;

        واپس ret;
}

ساکن bool gid_list_has(ثابت gid_t *list, size_t سائز, gid_t val) {
        أكد(list || سائز == 0);

        FOREACH_ARRAY(i, list, سائز)
                اگر (*i == val)
                        واپس true;

        واپس false;
}

صحیح اندر_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = تہی;
        صحیح ngroups;

        اگر (getgid() == gid)
                واپس 1;

        اگر (getegid() == gid)
                واپس 1;

        اگر (!gid_is_valid(gid))
                واپس -EINVAL;

        ngroups = getgroups_alloc(&gids);
        اگر (ngroups < 0)
                واپس ngroups;

        واپس gid_list_has(gids, ngroups, gid);
}

صحیح اندر_گروہ(ثابت حرف *name) {
        صحیح r;
        gid_t gid;

        r = get_گروہ_creds(name, /* flags= */ 0, /* ret_name= */ تہی, &gid);
        اگر (r < 0)
                واپس r;

        واپس اندر_gid(gid);
}

صحیح ملاؤ_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        أكد(size1 == 0 || list1);
        أكد(size2 == 0 || list2);
        أكد(ret);

        اگر (size2 > زیادہ_سے_زیادہ_عدد - size1)
                واپس -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        اگر (!buf)
                واپس -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        برائے (size_t i = 0; i < size1; i++)
                اگر (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        برائے (size_t i = 0; i < size2; i++)
                اگر (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        واپس (صحیح)nresult;
}

صحیح getgroups_alloc(gid_t **ret) {
        صحیح ngroups = 8;

        أكد(ret);

        برائے (غیرعلامتی attempt = 0;;) {
                _cleanup_free_ gid_t *p = تہی;

                p = new(gid_t, ngroups);
                اگر (!p)
                        واپس -ENOMEM;

                ngroups = getgroups(ngroups, p);
                اگر (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        واپس ngroups;
                }
                اگر (ngroups == 0)
                        توڑو;
                اگر (errno != EINVAL)
                        واپس -errno;

                /* Give up eventually */
                اگر (attempt++ > 10)
                        واپس -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, تہی);
                اگر (ngroups < 0)
                        واپس -errno;
                اگر (ngroups == 0)
                        توڑو;
        }

        *ret = تہی;
        واپس 0;
}

صحیح get_home_dir(حرف **ret) {
        _cleanup_free_ ساخت passwd *p = تہی;
        ثابت حرف *e;
        uid_t u;
        صحیح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        اگر (e && راستہ_is_valid(e) && راستہ_is_absolute(e))
                جاؤ ملا;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        اگر (u == 0) {
                e = "/root";
                جاؤ ملا;
        }
        اگر (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                جاؤ ملا;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        اگر (r < 0)
                واپس r;

        e = p->pw_dir;
        اگر (!راستہ_is_valid(e) || !راستہ_is_absolute(e))
                واپس -EINVAL;

 ملا:
        واپس راستہ_simplify_alloc(e, ret);
}

صحیح get_shell(حرف **ret) {
        _cleanup_free_ ساخت passwd *p = تہی;
        ثابت حرف *e;
        uid_t u;
        صحیح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        اگر (e && راستہ_is_valid(e) && راستہ_is_absolute(e))
                جاؤ ملا;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        اگر (u == 0) {
                e = default_جڑ_shell(تہی);
                جاؤ ملا;
        }
        اگر (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                جاؤ ملا;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        اگر (r < 0)
                واپس r;

        e = p->pw_shell;
        اگر (!راستہ_is_valid(e) || !راستہ_is_absolute(e))
                واپس -EINVAL;

 ملا:
        واپس راستہ_simplify_alloc(e, ret);
}

صحیح fully_سیٹ_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids) {
        صحیح r;

        أكد(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        اگر (r < 0)
                واپس r;

        اگر (gid_is_valid(gid))
                اگر (setresgid(gid, gid, gid) < 0)
                        واپس -errno;

        اگر (uid_is_valid(uid))
                اگر (setresuid(uid, uid, uid) < 0)
                        واپس -errno;

        واپس 0;
}

صحیح take_etc_passwd_تالا(ثابت حرف *جڑ) {
        صحیح r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ حرف *راستہ = راستہ_join(جڑ, ETC_PASSWD_تالا_راستہ);
        اگر (!راستہ)
                واپس log_oom_debug();

        (خالی) mkdir_parents(راستہ, 0755);

        _cleanup_close_ صحیح fd = open(راستہ, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        اگر (fd < 0)
                واپس log_debug_errno(errno, "Cannot open %s: %m", راستہ);

        r = unposix_تالا(fd, LOCK_EX);
        اگر (r < 0)
                واپس log_debug_errno(r, "Locking %s failed: %m", راستہ);

        واپس TAKE_FD(fd);
}

bool valid_user_گروہ_name(ثابت حرف *u, ValidUserFlags flags) {
        ثابت حرف *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        اگر (isempty(u)) /* An empty user name is never valid */
                واپس false;

        اگر (parse_uid(u, تہی) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                واپس FLAGS_سیٹ(flags, VALID_USER_ALLOW_NUMERIC);

        اگر (FLAGS_سیٹ(flags, VALID_USER_RELAX)) {

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
                        واپس false;

                اگر (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        واپس false;

                اگر (string_has_cc(u, تہی)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        واپس false;

                اگر (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        واپس false;

                اگر (اندر_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        واپس false;

                اگر (u[0] == '-' && اندر_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        واپس false;

                اگر (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        واپس false;

                /* Compare with strict result and warn if result doesn't match */
                اگر (FLAGS_سیٹ(flags, VALID_USER_WARN) && !valid_user_گروہ_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_پیغام("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_شے("USER_GROUP_NAME=%s", u),
                                   LOG_پیغام_ID(SD_پیغام_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } ورنہ {
                طویل sz;
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
                        واپس false;

                برائے (i = u+1; *i; i++)
                        اگر (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !اندر_سیٹ(*i, '_', '-'))
                                واپس false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                اگر (l > (size_t) sz) /* glibc: 256 */
                        واپس false;
                اگر (l > NAME_MAX) /* must fit in a filename: 255 */
                        واپس false;
                اگر (l > sizeof_میدان(ساخت utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        واپس false;
        }

        واپس true;
}

bool valid_gecos(ثابت حرف *d) {

        اگر (!d)
                واپس false;

        اگر (!utf8_is_valid(d))
                واپس false;

        اگر (string_has_cc(d, تہی))
                واپس false;

        /* Colons are used as field separators, and hence not OK */
        اگر (strchr(d, ':'))
                واپس false;

        واپس true;
}

حرف* mangle_gecos(ثابت حرف *d) {
        حرف *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        اگر (!mangled)
                واپس تہی;

        برائے (حرف *i = mangled; *i; i++) {
                صحیح len;

                اگر ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        جاری;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                اگر (len < 0) {
                        *i = ' ';
                        جاری;
                }

                i += len - 1;
        }

        واپس mangled;
}

bool valid_home(ثابت حرف *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        اگر (isempty(p))
                واپس false;

        اگر (!utf8_is_valid(p))
                واپس false;

        اگر (string_has_cc(p, تہی))
                واپس false;

        اگر (!راستہ_is_absolute(p))
                واپس false;

        اگر (!راستہ_is_normalized(p))
                واپس false;

        /* Colons are used as field separators, and hence not OK */
        اگر (strchr(p, ':'))
                واپس false;

        واپس true;
}

bool valid_shell(ثابت حرف *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        اگر (!valid_home(p))
                واپس false;

        واپس !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

صحیح maybe_setgroups(size_t سائز, ثابت gid_t *list) {
        صحیح r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        اگر (سائز == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                اگر (!f && errno != ENOENT)
                        واپس -errno;
                اگر (f) {
                        r = محفوظ_fgetc(f, /* ret= */ تہی);
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                واپس 0;
                        }
                }

                _cleanup_free_ حرف *setgroups_content = تہی;
                r = read_one_سطر_file("/proc/self/setgroups", &setgroups_content);
                اگر (r < 0 && r != -ENOENT)
                        واپس r;
                اگر (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        واپس 0;
                }
        }

        واپس RET_NERRNO(setgroups(سائز, list));
}

bool synthesize_nobody(خالی) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ساکن صحیح cache = -1;

        اگر (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        واپس cache;
}

صحیح putpwent_sane(ثابت ساخت passwd *pw, FILE *دھارا) {
        أكد(pw);
        أكد(دھارا);

        errno = 0;
        اگر (putpwent(pw, دھارا) != 0)
                واپس errno_or_else(EIO);

        واپس 0;
}

صحیح putspent_sane(ثابت ساخت spwd *sp, FILE *دھارا) {
        أكد(sp);
        أكد(دھارا);

        errno = 0;
        اگر (putspent(sp, دھارا) != 0)
                واپس errno_or_else(EIO);

        واپس 0;
}

صحیح putgrent_sane(ثابت ساخت group *gr, FILE *دھارا) {
        أكد(gr);
        أكد(دھارا);

        errno = 0;
        اگر (putgrent(gr, دھارا) != 0)
                واپس errno_or_else(EIO);

        واپس 0;
}

#اگر ENABLE_GSHADOW
صحیح putsgent_sane(ثابت ساخت sgrp *sg, FILE *دھارا) {
        أكد(sg);
        أكد(دھارا);

        errno = 0;
        اگر (putsgent(sg, دھارا) != 0)
                واپس errno_or_else(EIO);

        واپس 0;
}
#اگر_ختم

صحیح fgetpwent_sane(FILE *دھارا, ساخت passwd **pw) {
        أكد(دھارا);
        أكد(pw);

        errno = 0;
        ساخت passwd *p = fgetpwent(دھارا);
        اگر (!p && !اندر_سیٹ(errno, 0, ENOENT))
                واپس -errno;

        *pw = p;
        واپس !!p;
}

صحیح fgetspent_sane(FILE *دھارا, ساخت spwd **sp) {
        أكد(دھارا);
        أكد(sp);

        errno = 0;
        ساخت spwd *s = fgetspent(دھارا);
        اگر (!s && !اندر_سیٹ(errno, 0, ENOENT))
                واپس -errno;

        *sp = s;
        واپس !!s;
}

صحیح fgetgrent_sane(FILE *دھارا, ساخت group **gr) {
        أكد(دھارا);
        أكد(gr);

        errno = 0;
        ساخت group *g = fgetgrent(دھارا);
        اگر (!g && !اندر_سیٹ(errno, 0, ENOENT))
                واپس -errno;

        *gr = g;
        واپس !!g;
}

#اگر ENABLE_GSHADOW
صحیح fgetsgent_sane(FILE *دھارا, ساخت sgrp **sg) {
        أكد(دھارا);
        أكد(sg);

        errno = 0;
        ساخت sgrp *s = fgetsgent(دھارا);
        اگر (!s && !اندر_سیٹ(errno, 0, ENOENT))
                واپس -errno;

        *sg = s;
        واپس !!s;
}
#اگر_ختم

صحیح is_this_me(ثابت حرف *username) {
        uid_t uid;
        صحیح r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, تہی, &uid, تہی, تہی, تہی);
        اگر (r < 0)
                واپس r;

        واپس uid == getuid();
}

ثابت حرف* get_home_جڑ(خالی) {
        /* For debug purposes allow overriding where we look for home dirs */
        ثابت حرف *e = secure_getenv("SYSTEMD_HOME_ROOT");
        اگر (e && راستہ_is_absolute(e) && راستہ_is_normalized(e))
                واپس e;

        واپس "/home";
}

ساکن صحیح copy_struct_passwd(ثابت ساخت passwd *pw, ساخت passwd **ret) {
        أكد(pw);
        أكد(ret);

        size_t need_بائٹیں = حجم(ساخت passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        حرف *buf = حافظہ_دو(need_بائٹیں);
        اگر (!buf)
                واپس -ENOMEM;

        ساخت passwd *newpw = (ساخت passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        حرف *p = buf + حجم(ساخت passwd);

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
        واپس 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
صحیح lookup_pwent_اندر_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                uid_t uid,
                ساخت passwd **ret) {

        صحیح r;

        أكد(files);
        أكد(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = تہی;
                ساخت passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                اگر (r == -ENOENT)
                        جاری;
                اگر (r < 0)
                        واپس r;

                جب ((r = fgetpwent_sane(f, &pw)) > 0) {
                        اگر (name && !streq_ptr(pw->pw_name, name))
                                جاری;
                        اگر (uid_is_valid(uid) && pw->pw_uid != uid)
                                جاری;
                        اگر (ret)
                                واپس copy_struct_passwd(pw, ret);
                        واپس 0;
                }
                اگر (r < 0)
                        واپس r;
        }

        واپس -ESRCH;
}

#اگر !BUILD_STATIC

ساکن size_t getpw_بفر_سائز(خالی) {
        طویل bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        واپس bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ساکن bool errno_is_user_doesnt_exist(صحیح error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        واپس اندر_سیٹ(مطلق_قدر(error), ENOENT, ESRCH, EBADF, EPERM);
}

#اگر_ختم

صحیح getpwnam_malloc(ثابت حرف *name, ساخت passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        اگر (isempty(name))
                واپس -EINVAL;

#اگر BUILD_STATIC
        واپس lookup_pwent_اندر_files(PASSWD_FILES, name, UID_INVALID, ret);
#ورنہ
        size_t bufsize = getpw_بفر_سائز();
        صحیح r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        برائے (;;) {
                _cleanup_free_ خالی *buf = تہی;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(ساخت passwd)));
                buf = malloc0(ALIGN(حجم(ساخت passwd)) + bufsize);
                اگر (!buf)
                        واپس -ENOMEM;

                ساخت passwd *pw = تہی;
                r = getpwnam_r(name, buf, (حرف*) buf + ALIGN(حجم(ساخت passwd)), bufsize, &pw);
                اگر (r == 0) {
                        اگر (pw) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                واپس 0;
                        }

                        واپس -ESRCH;
                }

                أكد(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                اگر (errno_is_user_doesnt_exist(r))
                        واپس -ESRCH;
                اگر (r != ERANGE)
                        واپس -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(حجم(ساخت passwd)))
                        واپس -ENOMEM;
                bufsize *= 2;
        }
#اگر_ختم
}

صحیح getpwuid_malloc(uid_t uid, ساخت passwd **ret) {
        اگر (!uid_is_valid(uid))
                واپس -EINVAL;

#اگر BUILD_STATIC
        واپس lookup_pwent_اندر_files(PASSWD_FILES, /* name= */ تہی, uid, ret);
#ورنہ
        size_t bufsize = getpw_بفر_سائز();
        صحیح r;

        برائے (;;) {
                _cleanup_free_ خالی *buf = تہی;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(ساخت passwd)));
                buf = malloc0(ALIGN(حجم(ساخت passwd)) + bufsize);
                اگر (!buf)
                        واپس -ENOMEM;

                ساخت passwd *pw = تہی;
                r = getpwuid_r(uid, buf, (حرف*) buf + ALIGN(حجم(ساخت passwd)), bufsize, &pw);
                اگر (r == 0) {
                        اگر (pw) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                واپس 0;
                        }

                        واپس -ESRCH;
                }

                أكد(r > 0);

                اگر (errno_is_user_doesnt_exist(r))
                        واپس -ESRCH;
                اگر (r != ERANGE)
                        واپس -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(حجم(ساخت passwd)))
                        واپس -ENOMEM;
                bufsize *= 2;
        }
#اگر_ختم
}

ساکن صحیح copy_struct_گروہ(ثابت ساخت group *gr, ساخت group **ret) {
        أكد(gr);
        أكد(ret);

        size_t need_بائٹیں = حجم(ساخت group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_بائٹیں += حجم(حرف*) + لڑی_لمبائی(*s) + 1;
                n_mem++;
        }
        need_بائٹیں += حجم(حرف*);  /* NULL terminator for gr_mem */

        حرف *buf = حافظہ_دو(need_بائٹیں);
        اگر (!buf)
                واپس -ENOMEM;

        ساخت group *newgr = (ساخت group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(حجم(ساخت group) % alignof(حرف*) == 0);

        حرف *p = buf + حجم(ساخت group) + (n_mem + 1) * حجم(حرف*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (حرف**) (buf + حجم(ساخت group));
        برائے (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = تہی;

        *ret = newgr;
        واپس 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
صحیح lookup_grent_اندر_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                ساخت group **ret) {

        صحیح r;

        أكد(files);
        أكد(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = تہی;
                ساخت group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                اگر (r == -ENOENT)
                        جاری;
                اگر (r < 0)
                        واپس r;

                جب ((r = fgetgrent_sane(f, &gr)) > 0) {
                        اگر (name && !streq_ptr(gr->gr_name, name))
                                جاری;
                        اگر (gid_is_valid(gid) && gr->gr_gid != gid)
                                جاری;
                        اگر (ret)
                                واپس copy_struct_گروہ(gr, ret);
                        واپس 0;
                }
                اگر (r < 0)
                        واپس r;
        }

        واپس -ESRCH;
}

صحیح sysconf_ngroups_max(خالی) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        طویل ngroups_max = sysconf(_SC_NGROUPS_MAX);
        اگر (ngroups_max <= 0)
                واپس errno_or_else(EOPNOTSUPP);
        اگر (ngroups_max > زیادہ_سے_زیادہ_عدد)
                واپس -ERANGE;
        واپس ngroups_max;
}

ssize_t lookup_groups_اندر_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = تہی;
        ssize_t n_arr = 0;
        صحیح r;

        أكد(files);
        أكد(name);
        أكد(gid_is_valid(gid));

        صحیح ngroups_max = sysconf_ngroups_max();
        اگر (ngroups_max < 0)
                واپس ngroups_max;

        اگر (!GREEDY_REALLOC(arr, n_arr + 1))
                واپس -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = تہی;
                ساخت group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                اگر (r == -ENOENT)
                        جاری;
                اگر (r < 0)
                        واپس r;

                جب ((r = fgetgrent_sane(f, &gr)) > 0) {
                        اگر (gid == gr->gr_gid)
                                جاری;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        اگر (gid_list_has(arr, n_arr, gr->gr_gid))
                                جاری;

                        اگر (!strv_شامل_ہے(gr->gr_mem, name))
                                جاری;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        اگر (n_arr >= ngroups_max)
                                واپس -ENOBUFS;

                        اگر (!GREEDY_REALLOC(arr, n_arr + 1))
                                واپس -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret)
                *ret = TAKE_PTR(arr);
        واپس n_arr;
}

صحیح getgrouplist_malloc(ثابت حرف *user, gid_t gid, gid_t **ret) {
#اگر BUILD_STATIC
        واپس lookup_groups_اندر_files(گروہ_FILES, user, gid, ret);
#ورنہ
        صحیح ngroups_max = sysconf_ngroups_max();
        اگر (ngroups_max < 0)
                واپس ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        اگر (!gids)
                واپس -ENOMEM;

        صحیح k = ngroups_max;
        اگر (getgrouplist(user, gid, gids, &k) < 0)
                واپس -EIO;  /* getgrouplist does not set errno */

        اگر (ret)
                *ret = TAKE_PTR(gids);
        واپس k;
#اگر_ختم
}

صحیح initgroups_wrapper(ثابت حرف *user, gid_t gid) {
#اگر BUILD_STATIC
        _cleanup_free_ gid_t *groups = تہی;
        صحیح r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        اگر (n_groups <= 0)
                واپس n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        برائے (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                اگر (r != -EINVAL)
                        توڑو;
        }

        واپس r;
#ورنہ
        واپس RET_NERRNO(initgroups(user, gid));
#اگر_ختم
}

#اگر !BUILD_STATIC

ساکن size_t getgr_بفر_سائز(خالی) {
        طویل bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        واپس bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#اگر_ختم

صحیح getgrnam_malloc(ثابت حرف *name, ساخت group **ret) {
        اگر (isempty(name))
                واپس -EINVAL;

#اگر BUILD_STATIC
        واپس lookup_grent_اندر_files(گروہ_FILES, name, GID_INVALID, ret);
#ورنہ
        size_t bufsize = getgr_بفر_سائز();
        صحیح r;

        برائے (;;) {
                _cleanup_free_ خالی *buf = تہی;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(ساخت group)));
                buf = malloc0(ALIGN(حجم(ساخت group)) + bufsize);
                اگر (!buf)
                        واپس -ENOMEM;

                ساخت group *gr = تہی;
                r = getgrnam_r(name, buf, (حرف*) buf + ALIGN(حجم(ساخت group)), bufsize, &gr);
                اگر (r == 0) {
                        اگر (gr) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                واپس 0;
                        }

                        واپس -ESRCH;
                }

                أكد(r > 0);

                اگر (errno_is_user_doesnt_exist(r))
                        واپس -ESRCH;
                اگر (r != ERANGE)
                        واپس -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(حجم(ساخت group)))
                        واپس -ENOMEM;
                bufsize *= 2;
        }
#اگر_ختم
}

صحیح getgrgid_malloc(gid_t gid, ساخت group **ret) {
        اگر (!gid_is_valid(gid))
                واپس -EINVAL;

#اگر BUILD_STATIC
        واپس lookup_grent_اندر_files(گروہ_FILES, /* name= */ تہی, gid, ret);
#ورنہ
        size_t bufsize = getgr_بفر_سائز();
        صحیح r;

        برائے (;;) {
                _cleanup_free_ خالی *buf = تہی;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(ساخت group)));
                buf = malloc0(ALIGN(حجم(ساخت group)) + bufsize);
                اگر (!buf)
                        واپس -ENOMEM;

                ساخت group *gr = تہی;
                r = getgrgid_r(gid, buf, (حرف*) buf + ALIGN(حجم(ساخت group)), bufsize, &gr);
                اگر (r == 0) {
                        اگر (gr) {
                                اگر (ret)
                                        *ret = TAKE_PTR(buf);
                                واپس 0;
                        }

                        واپس -ESRCH;
                }

                أكد(r > 0);

                اگر (errno_is_user_doesnt_exist(r))
                        واپس -ESRCH;
                اگر (r != ERANGE)
                        واپس -r;

                اگر (bufsize > SIZE_MAX/2 - ALIGN(حجم(ساخت group)))
                        واپس -ENOMEM;
                bufsize *= 2;
        }
#اگر_ختم
}
