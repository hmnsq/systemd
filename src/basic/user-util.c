/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <sys/file.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>
#ضمن <utmpx.h>

#ضمن "sd-messages.h"

#ضمن "alloc-util.h"
#ضمن "chase.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "format-util.h"
#ضمن "lock-util.h"
#ضمن "log.h"
#ضمن "mkdir.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "terminal-util.h"
#ضمن "user-util.h"
#ضمن "utf8.h"

#وصف_ڪريو DEFINE_STRERROR_ACCOUNT(type)                                   \
        ثابت حرف* strerror_##type(                                    \
                        صحيح errnum,                                     \
                        حرف *buf,                                      \
                        size_t buflen) {                                \
                                                                        \
                errnum = ABS(errnum);                                   \
                بدل (errnum) {                                       \
                حالة ESRCH:                                             \
                        أرجع "Unknown " STRINGIFY(type);              \
                حالة ENOEXEC:                                           \
                        أرجع "Not a system " STRINGIFY(type);         \
                افتراضي:                                                \
                        أرجع strerror_r(errnum, buf, buflen);         \
                }                                                       \
        }

DEFINE_STRERROR_ACCOUNT(user);
DEFINE_STRERROR_ACCOUNT(group);

bool uid_is_valid(uid_t uid) {

        /* Also see POSIX IEEE Std 1003.1-2008, 2016 Edition, 3.436. */

        /* Some libc APIs use UID_INVALID as special placeholder */
        إذا (uid == (uid_t) UINT32_C(0xFFFFFFFF))
                أرجع false;

        /* A long time ago UIDs where 16 bit, hence explicitly avoid the 16-bit -1 too */
        إذا (uid == (uid_t) UINT32_C(0xFFFF))
                أرجع false;

        أرجع true;
}

صحيح parse_uid(ثابت حرف *s, uid_t *ret) {
        uint32_t uid = 0;
        صحيح r;

        أكد(s);

        assert_cc(حجم(uid_t) == حجم(uint32_t));

        /* We are very strict when parsing UIDs, and prohibit +/- as prefix, leading zero as prefix, and
         * whitespace. We do this, since this call is often used in a context where we parse things as UID
         * first, and if that doesn't work we fall back to NSS. Thus we really want to make sure that UIDs
         * are parsed as UIDs only if they really really look like UIDs. */
        r = محفوظ_atou32_ڀريل(s, 10
                             | محفوظ_ATO_REFUSE_PLUS_MINUS
                             | محفوظ_ATO_REFUSE_LEADING_ZERO
                             | محفوظ_ATO_REFUSE_LEADING_WHITESPACE, &uid);
        إذا (r < 0)
                أرجع r;

        إذا (!uid_is_valid(uid))
                أرجع -ENXIO; /* we return ENXIO instead of EINVAL
                                * here, to make it easy to distinguish
                                * invalid numeric uids from invalid
                                * strings. */

        إذا (ret)
                *ret = uid;

        أرجع 0;
}

صحيح parse_uid_range(ثابت حرف *s, uid_t *ret_هيٺاهون, uid_t *ret_مٿانهون) {
        _cleanup_free_ حرف *لفظ = عدم;
        uid_t l, u;
        صحيح r;

        أكد(s);
        أكد(ret_هيٺاهون);
        أكد(ret_مٿانهون);

        r = ڪڍو_پهريون_لفظ(&s, &لفظ, "-", ڪڍو_DONT_COALESCE_SEPARATORS);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EINVAL;

        r = parse_uid(لفظ, &l);
        إذا (r < 0)
                أرجع r;

        /* Check for the upper bound and extract it if needed */
        إذا (!s)
                /* Single number with no dash. */
                u = l;
        وإلا إذا (!*s)
                /* Trailing dash is an error. */
                أرجع -EINVAL;
        وإلا {
                r = parse_uid(s, &u);
                إذا (r < 0)
                        أرجع r;

                إذا (l > u)
                        أرجع -EINVAL;
        }

        *ret_هيٺاهون = l;
        *ret_مٿانهون = u;
        أرجع 0;
}

حرف* getlogname_malloc(فراغ) {
        uid_t uid;
        بنية stat st;

        إذا (isatty_محفوظ(STDIN_FILENO) && fstat(STDIN_FILENO, &st) >= 0)
                uid = st.st_uid;
        وإلا
                uid = getuid();

        أرجع uid_منزل_name(uid);
}

حرف* getusername_malloc(فراغ) {
        ثابت حرف *e;

        e = secure_getenv("USER");
        إذا (e)
                أرجع strdup(e);

        أرجع uid_منزل_name(getuid());
}

bool is_nologin_shell(ثابت حرف *shell) {
        أرجع رستو_اندريون_سيٽ(shell,
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
        أرجع isempty(shell) || is_nologin_shell(shell);
}

ثابت حرف* default_پاڙ_shell_at(صحيح rfd) {
        /* We want to use the preferred shell, i.e. DEFAULT_USER_SHELL, which usually
         * will be /bin/bash. Fall back to /bin/sh if DEFAULT_USER_SHELL is not found,
         * or any access errors. */

        أكد(rfd >= 0 || rfd == AT_FDCWD);

        صحيح r = chaseat(rfd, rfd, DEFAULT_USER_SHELL, /* flags= */ 0, عدم, عدم);
        إذا (r < 0 && r != -ENOENT)
                log_debug_errno(r, "Failed to look up shell '%s': %m", DEFAULT_USER_SHELL);
        إذا (r > 0)
                أرجع DEFAULT_USER_SHELL;

        أرجع "/bin/sh";
}

ثابت حرف* default_پاڙ_shell(ثابت حرف *پاڙ) {
        _cleanup_close_ صحيح rfd = -EBADF;

        rfd = open(خالي_جايون_منزل_پاڙ(پاڙ), O_CLOEXEC | O_DIRECTORY | O_رستو);
        إذا (rfd < 0)
                أرجع "/bin/sh";

        أرجع default_پاڙ_shell_at(rfd);
}

ساكن صحيح return_user_creds(
                ثابت حرف *username,
                uid_t uid, gid_t gid,
                ثابت حرف *home,
                ثابت حرف *shell,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {
        /* Helper function to help with the strdups and atomic setting of return params. */

        _cleanup_free_ حرف *s1 = عدم, *s2 = عدم, *s3 = عدم;
        صحيح r;

        إذا (ret_username) {
                r = strdup_منزل(&s1, username);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_home) {
                r = strdup_منزل(&s2, home);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_shell) {
                r = strdup_منزل(&s3, shell);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_username)
                *ret_username = TAKE_PTR(s1);
        إذا (ret_uid)
                *ret_uid = uid;
        إذا (ret_gid)
                *ret_gid = gid;
        إذا (ret_home)
                *ret_home = TAKE_PTR(s2);
        إذا (ret_shell)
                *ret_shell = TAKE_PTR(s3);
        أرجع 0;
}

ساكن صحيح synthesize_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {
        أكد(username);

        /* We enforce some special rules for uid=0 and uid=65534: in order to avoid nss lookups for root we
         * hardcode their user record data. */
        إذا (STR_اندريون_سيٽ(username, "root", "0"))
                أرجع return_user_creds("root", 0, 0,
                                         "/root",
                                         ret_shell ? default_پاڙ_shell(عدم) : عدم,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        إذا (STR_اندريون_سيٽ(username, NOBODY_USER_NAME, "65534") &&
            synthesize_nobody())
                أرجع return_user_creds(NOBODY_USER_NAME, UID_NOBODY, GID_NOBODY,
                                         FLAGS_سيٽ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? عدم : "/",
                                         FLAGS_سيٽ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) ? عدم : NOLOGIN,
                                         ret_username,
                                         ret_uid, ret_gid,
                                         ret_home,
                                         ret_shell);

        أرجع -ENOMEDIUM;
}

صحيح get_user_creds(
                ثابت حرف *username,
                UserCredsFlags flags,
                حرف **ret_username,
                uid_t *ret_uid, gid_t *ret_gid,
                حرف **ret_home,
                حرف **ret_shell) {

        uid_t u = UID_INVALID;
        _cleanup_free_ بنية passwd *pw = عدم;
        صحيح r;

        أكد(username);
        أكد((ret_home || ret_shell) || !(flags & (USER_CREDS_SUPPRESS_PLACEHOLDER|USER_CREDS_CLEAN)));

        إذا (!FLAGS_سيٽ(flags, USER_CREDS_PREFER_NSS) ||
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
                إذا (r >= 0)
                        أرجع 0;
                إذا (r != -ENOMEDIUM) /* not a username we can synthesize */
                        أرجع r;
        }

        إذا (parse_uid(username, &u) >= 0) {
                r = getpwuid_malloc(u, &pw);

                /* If there are multiple users with the same id, make sure to leave $USER to the configured value
                 * instead of the first occurrence in the database. However if the uid was configured by a numeric uid,
                 * then let's pick the real username from /etc/passwd. */
                إذا (r >= 0)
                        username = pw->pw_name;

                وإلا إذا (FLAGS_سيٽ(flags, USER_CREDS_ALLOW_MISSING) && !ret_gid && !ret_home && !ret_shell) {
                        /* If the specified user is a numeric UID and it isn't in the user database, and the caller
                         * passed USER_CREDS_ALLOW_MISSING and was only interested in the UID, then just return that
                         * and don't complain. */
                        إذا (ret_username)
                                *ret_username = عدم;
                        إذا (ret_uid)
                                *ret_uid = u;
                        أرجع 0;
                }
        } وإلا
                r = getpwnam_malloc(username, &pw);

        إذا (r < 0) {
                /* If the user requested that we only synthesize as fallback, do so now */
                إذا (FLAGS_سيٽ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_user_creds(username, flags, ret_username, ret_uid, ret_gid, ret_home, ret_shell) >= 0)
                        أرجع 0;

                أرجع r;
        }

        إذا (ret_uid && !uid_is_valid(pw->pw_uid))
                أرجع -EBADMSG;

        إذا (ret_gid && !gid_is_valid(pw->pw_gid))
                أرجع -EBADMSG;

        /* Note: we don't insist on normalized paths, since there are setups that have /./ in the path */
        ثابت حرف *h =
                (FLAGS_سيٽ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && خالي_جايون_or_پاڙ(pw->pw_dir)) ||
                (FLAGS_سيٽ(flags, USER_CREDS_CLEAN) && (!رستو_is_valid(pw->pw_dir) || !رستو_is_absolute(pw->pw_dir)))
                ? عدم : pw->pw_dir;

        ثابت حرف *s =
                (FLAGS_سيٽ(flags, USER_CREDS_SUPPRESS_PLACEHOLDER) && shell_is_placeholder(pw->pw_shell)) ||
                (FLAGS_سيٽ(flags, USER_CREDS_CLEAN) && (!رستو_is_valid(pw->pw_shell) || !رستو_is_absolute(pw->pw_shell)))
                ? عدم : pw->pw_shell;

        أرجع return_user_creds(username, pw->pw_uid, pw->pw_gid, h, s,
                                 ret_username, ret_uid, ret_gid, ret_home, ret_shell);
}

ساكن صحيح synthesize_گروهه_creds(ثابت حرف *groupname, حرف **ret_name, gid_t *ret_gid) {
        أكد(groupname);

        gid_t id;
        ثابت حرف *n;
        صحيح r;

        إذا (STR_اندريون_سيٽ(groupname, "root", "0")) {
                id = 0;
                n = "root";
        } وإلا إذا (STR_اندريون_سيٽ(groupname, NOBODY_گروهه_NAME, "65534") &&
                   synthesize_nobody()) {
                id = GID_NOBODY;
                n = NOBODY_گروهه_NAME;
        } وإلا
                أرجع -ENOMEDIUM;

        r = strdup_منزل_ڀريل(ret_name, n);
        إذا (r < 0)
                أرجع r;
        إذا (ret_gid)
                *ret_gid = id;
        أرجع 0;
}

صحيح get_گروهه_creds(ثابت حرف *groupname, UserCredsFlags flags, حرف **ret_name, gid_t *ret_gid) {
        _cleanup_free_ بنية group *gr = عدم;
        gid_t id;
        صحيح r;

        أكد(groupname);

        إذا (!FLAGS_سيٽ(flags, USER_CREDS_PREFER_NSS)) {
                r = synthesize_گروهه_creds(groupname, ret_name, ret_gid);
                إذا (r >= 0)
                        أرجع 0;
                إذا (r != -ENOMEDIUM) /* not a groupname we can synthesize */
                        أرجع r;
        }

        إذا (parse_gid(groupname, &id) >= 0) {
                r = getgrgid_malloc(id, &gr);
                إذا (r >= 0)
                        groupname = gr->gr_name;
                وإلا إذا (FLAGS_سيٽ(flags, USER_CREDS_ALLOW_MISSING)) {
                        إذا (ret_gid)
                                *ret_gid = id;
                        إذا (ret_name)
                                *ret_name = عدم;
                        أرجع 0;
                }
        } وإلا
                r = getgrnam_malloc(groupname, &gr);

        إذا (r < 0) {
                إذا (FLAGS_سيٽ(flags, USER_CREDS_PREFER_NSS) &&
                    synthesize_گروهه_creds(groupname, ret_name, ret_gid) >= 0)
                        أرجع 0;
                أرجع r;
        }

        إذا (ret_gid && !gid_is_valid(gr->gr_gid))
                أرجع -EBADMSG;

        r = strdup_منزل_ڀريل(ret_name, groupname);
        إذا (r < 0)
                أرجع r;
        إذا (ret_gid)
                *ret_gid = gr->gr_gid;
        أرجع 0;
}

حرف* uid_منزل_name(uid_t uid) {
        حرف *ret;
        صحيح r;

        /* Shortcut things to avoid NSS lookups */
        إذا (uid == 0)
                أرجع strdup("root");
        إذا (uid == UID_NOBODY && synthesize_nobody())
                أرجع strdup(NOBODY_USER_NAME);

        إذا (uid_is_valid(uid)) {
                _cleanup_free_ بنية passwd *pw = عدم;

                r = getpwuid_malloc(uid, &pw);
                إذا (r >= 0)
                        أرجع strdup(pw->pw_name);
        }

        إذا (asprintf(&ret, UID_FMT, uid) < 0)
                أرجع عدم;

        أرجع ret;
}

حرف* gid_منزل_name(gid_t gid) {
        حرف *ret;
        صحيح r;

        إذا (gid == 0)
                أرجع strdup("root");
        إذا (gid == GID_NOBODY && synthesize_nobody())
                أرجع strdup(NOBODY_گروهه_NAME);

        إذا (gid_is_valid(gid)) {
                _cleanup_free_ بنية group *gr = عدم;

                r = getgrgid_malloc(gid, &gr);
                إذا (r >= 0)
                        أرجع strdup(gr->gr_name);
        }

        إذا (asprintf(&ret, GID_FMT, gid) < 0)
                أرجع عدم;

        أرجع ret;
}

ساكن bool gid_list_has(ثابت gid_t *list, size_t سائيز, gid_t val) {
        أكد(list || سائيز == 0);

        FOREACH_ARRAY(i, list, سائيز)
                إذا (*i == val)
                        أرجع true;

        أرجع false;
}

صحيح اندريون_gid(gid_t gid) {
        _cleanup_free_ gid_t *gids = عدم;
        صحيح ngroups;

        إذا (getgid() == gid)
                أرجع 1;

        إذا (getegid() == gid)
                أرجع 1;

        إذا (!gid_is_valid(gid))
                أرجع -EINVAL;

        ngroups = getgroups_alloc(&gids);
        إذا (ngroups < 0)
                أرجع ngroups;

        أرجع gid_list_has(gids, ngroups, gid);
}

صحيح اندريون_گروهه(ثابت حرف *name) {
        صحيح r;
        gid_t gid;

        r = get_گروهه_creds(name, /* flags= */ 0, /* ret_name= */ عدم, &gid);
        إذا (r < 0)
                أرجع r;

        أرجع اندريون_gid(gid);
}

صحيح ملايو_gid_lists(ثابت gid_t *list1, size_t size1, ثابت gid_t *list2, size_t size2, gid_t **ret) {
        size_t nresult = 0;

        أكد(size1 == 0 || list1);
        أكد(size2 == 0 || list2);
        أكد(ret);

        إذا (size2 > وڌ_کان_وڌ_عدد - size1)
                أرجع -ENOBUFS;

        gid_t *buf = new(gid_t, size1 + size2);
        إذا (!buf)
                أرجع -ENOMEM;

        /* Duplicates need to be skipped on merging, otherwise they'll be passed on and stored in the kernel. */
        لكل (size_t i = 0; i < size1; i++)
                إذا (!gid_list_has(buf, nresult, list1[i]))
                        buf[nresult++] = list1[i];
        لكل (size_t i = 0; i < size2; i++)
                إذا (!gid_list_has(buf, nresult, list2[i]))
                        buf[nresult++] = list2[i];
        *ret = buf;
        أرجع (صحيح)nresult;
}

صحيح getgroups_alloc(gid_t **ret) {
        صحيح ngroups = 8;

        أكد(ret);

        لكل (بنانشان attempt = 0;;) {
                _cleanup_free_ gid_t *p = عدم;

                p = new(gid_t, ngroups);
                إذا (!p)
                        أرجع -ENOMEM;

                ngroups = getgroups(ngroups, p);
                إذا (ngroups > 0) {
                        *ret = TAKE_PTR(p);
                        أرجع ngroups;
                }
                إذا (ngroups == 0)
                        اكسر;
                إذا (errno != EINVAL)
                        أرجع -errno;

                /* Give up eventually */
                إذا (attempt++ > 10)
                        أرجع -EINVAL;

                /* Get actual size needed, and size the array explicitly. Note that this is potentially racy
                 * to use (in multi-threaded programs), hence let's call this in a loop. */
                ngroups = getgroups(0, عدم);
                إذا (ngroups < 0)
                        أرجع -errno;
                إذا (ngroups == 0)
                        اكسر;
        }

        *ret = عدم;
        أرجع 0;
}

صحيح get_home_dir(حرف **ret) {
        _cleanup_free_ بنية passwd *p = عدم;
        ثابت حرف *e;
        uid_t u;
        صحيح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("HOME");
        إذا (e && رستو_is_valid(e) && رستو_is_absolute(e))
                اذهب مليو;

        /* Hardcode home directory for root and nobody to avoid NSS */
        u = getuid();
        إذا (u == 0) {
                e = "/root";
                اذهب مليو;
        }
        إذا (u == UID_NOBODY && synthesize_nobody()) {
                e = "/";
                اذهب مليو;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        إذا (r < 0)
                أرجع r;

        e = p->pw_dir;
        إذا (!رستو_is_valid(e) || !رستو_is_absolute(e))
                أرجع -EINVAL;

 مليو:
        أرجع رستو_simplify_alloc(e, ret);
}

صحيح get_shell(حرف **ret) {
        _cleanup_free_ بنية passwd *p = عدم;
        ثابت حرف *e;
        uid_t u;
        صحيح r;

        أكد(ret);

        /* Take the user specified one */
        e = secure_getenv("SHELL");
        إذا (e && رستو_is_valid(e) && رستو_is_absolute(e))
                اذهب مليو;

        /* Hardcode shell for root and nobody to avoid NSS */
        u = getuid();
        إذا (u == 0) {
                e = default_پاڙ_shell(عدم);
                اذهب مليو;
        }
        إذا (u == UID_NOBODY && synthesize_nobody()) {
                e = NOLOGIN;
                اذهب مليو;
        }

        /* Check the database... */
        r = getpwuid_malloc(u, &p);
        إذا (r < 0)
                أرجع r;

        e = p->pw_shell;
        إذا (!رستو_is_valid(e) || !رستو_is_absolute(e))
                أرجع -EINVAL;

 مليو:
        أرجع رستو_simplify_alloc(e, ret);
}

صحيح fully_سيٽ_uid_gid(uid_t uid, gid_t gid, ثابت gid_t supplementary_gids[], size_t n_supplementary_gids) {
        صحيح r;

        أكد(supplementary_gids || n_supplementary_gids == 0);

        /* Sets all UIDs and all GIDs to the specified ones. Drops all auxiliary GIDs */

        r = maybe_setgroups(n_supplementary_gids, supplementary_gids);
        إذا (r < 0)
                أرجع r;

        إذا (gid_is_valid(gid))
                إذا (setresgid(gid, gid, gid) < 0)
                        أرجع -errno;

        إذا (uid_is_valid(uid))
                إذا (setresuid(uid, uid, uid) < 0)
                        أرجع -errno;

        أرجع 0;
}

صحيح take_etc_passwd_تالو(ثابت حرف *پاڙ) {
        صحيح r;

        /* This is roughly the same as lckpwdf(), but not as awful. We don't want to use alarm() and signals,
         * hence we implement our own trivial version of this.
         *
         * Note that shadow-utils also takes per-database locks in addition to lckpwdf(). However, we don't,
         * given that they are redundant: they invoke lckpwdf() first and keep it during everything they do.
         * The per-database locks are awfully racy, and thus we just won't do them. */

        _cleanup_free_ حرف *رستو = رستو_join(پاڙ, ETC_PASSWD_تالو_رستو);
        إذا (!رستو)
                أرجع log_oom_debug();

        (فراغ) mkdir_parents(رستو, 0755);

        _cleanup_close_ صحيح fd = open(رستو, O_WRONLY|O_CREAT|O_CLOEXEC|O_NOCTTY|O_NOFOLLOW, 0600);
        إذا (fd < 0)
                أرجع log_debug_errno(errno, "Cannot open %s: %m", رستو);

        r = unposix_تالو(fd, LOCK_EX);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Locking %s failed: %m", رستو);

        أرجع TAKE_FD(fd);
}

bool valid_user_گروهه_name(ثابت حرف *u, ValidUserFlags flags) {
        ثابت حرف *i;

        /* Checks if the specified name is a valid user/group name. There are two flavours of this call:
         * strict mode is the default which is POSIX plus some extra rules; and relaxed mode where we accept
         * pretty much everything except the really worst offending names.
         *
         * Whenever we synthesize users ourselves we should use the strict mode. But when we process users
         * created by other stuff, let's be more liberal. */

        إذا (isempty(u)) /* An empty user name is never valid */
                أرجع false;

        إذا (parse_uid(u, عدم) >= 0) /* Something that parses as numeric UID string is valid exactly when the
                                      * flag for it is set */
                أرجع FLAGS_سيٽ(flags, VALID_USER_ALLOW_NUMERIC);

        إذا (FLAGS_سيٽ(flags, VALID_USER_RELAX)) {

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

                إذا (startswith(u, " ") || endswith(u, " ")) /* At least expect whitespace padding is removed
                                                             * at front and back (accept in the middle, since
                                                             * that's apparently a thing on Windows). Note
                                                             * that this also blocks usernames consisting of
                                                             * whitespace only. */
                        أرجع false;

                إذا (!utf8_is_valid(u)) /* We want to synthesize JSON from this, hence insist on UTF-8 */
                        أرجع false;

                إذا (string_has_cc(u, عدم)) /* CC characters are just dangerous (and \n in particular is the
                                             * record separator in /etc/passwd), so we can't allow that. */
                        أرجع false;

                إذا (strpbrk(u, ":/")) /* Colons are the field separator in /etc/passwd, we can't allow
                                       * that. Slashes are special to file systems paths and user names
                                       * typically show up in the file system as home directories, hence
                                       * don't allow slashes. */
                        أرجع false;

                إذا (اندريون_charset(u, DIGITS)) /* Don't allow fully numeric strings, they might be confused with
                                            * UIDs (note that this test is more broad than the parse_uid()
                                            * test above, as it will cover more than the 32-bit range, and it
                                            * will detect 65535 (which is in invalid UID, even though in the
                                            * unsigned 32 bit range) */
                        أرجع false;

                إذا (u[0] == '-' && اندريون_charset(u + 1, DIGITS)) /* Don't allow negative fully numeric strings
                                                               * either. After all some people write 65535 as
                                                               * -1 (even though that's not even true on
                                                               * 32-bit uid_t anyway) */
                        أرجع false;

                إذا (dot_or_dot_dot(u)) /* User names typically become home directory names, and these two are
                                        * special in that context, don't allow that. */
                        أرجع false;

                /* Compare with strict result and warn if result doesn't match */
                إذا (FLAGS_سيٽ(flags, VALID_USER_WARN) && !valid_user_گروهه_name(u, 0))
                        log_struct(LOG_NOTICE,
                                   LOG_پيغام("Accepting user/group name '%s', which does not match strict user/group name rules.", u),
                                   LOG_شي("USER_GROUP_NAME=%s", u),
                                   LOG_پيغام_ID(SD_پيغام_UNSAFE_USER_NAME_STR));

                /* Note that we make no restrictions on the length in relaxed mode! */
        } وإلا {
                ڊگهو sz;
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

                إذا (!ascii_isalpha(u[0]) &&
                    u[0] != '_')
                        أرجع false;

                لكل (i = u+1; *i; i++)
                        إذا (!ascii_isalpha(*i) &&
                            !ascii_isdigit(*i) &&
                            !اندريون_سيٽ(*i, '_', '-'))
                                أرجع false;

                l = i - u;

                sz = sysconf(_SC_LOGIN_NAME_MAX);
                assert_se(sz > 0);

                إذا (l > (size_t) sz) /* glibc: 256 */
                        أرجع false;
                إذا (l > NAME_MAX) /* must fit in a filename: 255 */
                        أرجع false;
                إذا (l > sizeof_ميدان(بنية utmpx, ut_user) - 1) /* must fit in utmp: 31 */
                        أرجع false;
        }

        أرجع true;
}

bool valid_gecos(ثابت حرف *d) {

        إذا (!d)
                أرجع false;

        إذا (!utf8_is_valid(d))
                أرجع false;

        إذا (string_has_cc(d, عدم))
                أرجع false;

        /* Colons are used as field separators, and hence not OK */
        إذا (strchr(d, ':'))
                أرجع false;

        أرجع true;
}

حرف* mangle_gecos(ثابت حرف *d) {
        حرف *mangled;

        /* Makes sure the provided string becomes valid as a GEGOS field, by dropping bad chars. glibc's
         * putpwent() only changes \n and : to spaces. We do more: replace all CC too, and remove invalid
         * UTF-8 */

        mangled = strdup(d);
        إذا (!mangled)
                أرجع عدم;

        لكل (حرف *i = mangled; *i; i++) {
                صحيح len;

                إذا ((uint8_t) *i < (uint8_t) ' ' || *i == ':') {
                        *i = ' ';
                        استمر;
                }

                len = utf8_encoded_valid_unichar(i, SIZE_MAX);
                إذا (len < 0) {
                        *i = ' ';
                        استمر;
                }

                i += len - 1;
        }

        أرجع mangled;
}

bool valid_home(ثابت حرف *p) {
        /* Note that this function is also called by valid_shell(), any
         * changes must account for that. */

        إذا (isempty(p))
                أرجع false;

        إذا (!utf8_is_valid(p))
                أرجع false;

        إذا (string_has_cc(p, عدم))
                أرجع false;

        إذا (!رستو_is_absolute(p))
                أرجع false;

        إذا (!رستو_is_normalized(p))
                أرجع false;

        /* Colons are used as field separators, and hence not OK */
        إذا (strchr(p, ':'))
                أرجع false;

        أرجع true;
}

bool valid_shell(ثابت حرف *p) {
        /* We have the same requirements, so just piggy-back on the home check.
         *
         * Let's ignore /etc/shells because this is only applicable to real and not system users. It is also
         * incompatible with the idea of empty /etc/. */
        إذا (!valid_home(p))
                أرجع false;

        أرجع !endswith(p, "/"); /* one additional restriction: shells may not be dirs */
}

صحيح maybe_setgroups(size_t سائيز, ثابت gid_t *list) {
        صحيح r;

        /* Check if setgroups is allowed before we try to drop all the auxiliary groups */
        إذا (سائيز == 0) { /* Dropping all aux groups? */

                /* The kernel refuses setgroups() if there are no GID mappings in the current
                 * user namespace, so check that beforehand and don't try to setgroups() if
                 * there are no GID mappings. */
                _cleanup_fclose_ FILE *f = fopen("/proc/self/gid_map", "re");
                إذا (!f && errno != ENOENT)
                        أرجع -errno;
                إذا (f) {
                        r = محفوظ_fgetc(f, /* ret= */ عدم);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0) {
                                log_debug("Skipping setgroups(), /proc/self/gid_map is empty");
                                أرجع 0;
                        }
                }

                _cleanup_free_ حرف *setgroups_content = عدم;
                r = read_one_سٽ_file("/proc/self/setgroups", &setgroups_content);
                إذا (r < 0 && r != -ENOENT)
                        أرجع r;
                إذا (r > 0 && streq(setgroups_content, "deny")) {
                        log_debug("Skipping setgroups(), /proc/self/setgroups is set to 'deny'");
                        أرجع 0;
                }
        }

        أرجع RET_NERRNO(setgroups(سائيز, list));
}

bool synthesize_nobody(فراغ) {
        /* Returns true when we shall synthesize the "nobody" user (which we do by default). This can be turned off by
         * touching /etc/systemd/dont-synthesize-nobody in order to provide upgrade compatibility with legacy systems
         * that used the "nobody" user name and group name for other UIDs/GIDs than 65534.
         *
         * Note that we do not employ any kind of synchronization on the following caching variable. If the variable is
         * accessed in multi-threaded programs in the worst case it might happen that we initialize twice, but that
         * shouldn't matter as each initialization should come to the same result. */
        ساكن صحيح cache = -1;

        إذا (cache < 0)
                cache = access("/etc/systemd/dont-synthesize-nobody", F_OK) < 0;

        أرجع cache;
}

صحيح putpwent_sane(ثابت بنية passwd *pw, FILE *وهڪرو) {
        أكد(pw);
        أكد(وهڪرو);

        errno = 0;
        إذا (putpwent(pw, وهڪرو) != 0)
                أرجع errno_or_else(EIO);

        أرجع 0;
}

صحيح putspent_sane(ثابت بنية spwd *sp, FILE *وهڪرو) {
        أكد(sp);
        أكد(وهڪرو);

        errno = 0;
        إذا (putspent(sp, وهڪرو) != 0)
                أرجع errno_or_else(EIO);

        أرجع 0;
}

صحيح putgrent_sane(ثابت بنية group *gr, FILE *وهڪرو) {
        أكد(gr);
        أكد(وهڪرو);

        errno = 0;
        إذا (putgrent(gr, وهڪرو) != 0)
                أرجع errno_or_else(EIO);

        أرجع 0;
}

#إذا ENABLE_GSHADOW
صحيح putsgent_sane(ثابت بنية sgrp *sg, FILE *وهڪرو) {
        أكد(sg);
        أكد(وهڪرو);

        errno = 0;
        إذا (putsgent(sg, وهڪرو) != 0)
                أرجع errno_or_else(EIO);

        أرجع 0;
}
#جيڪڏهن_پورو

صحيح fgetpwent_sane(FILE *وهڪرو, بنية passwd **pw) {
        أكد(وهڪرو);
        أكد(pw);

        errno = 0;
        بنية passwd *p = fgetpwent(وهڪرو);
        إذا (!p && !اندريون_سيٽ(errno, 0, ENOENT))
                أرجع -errno;

        *pw = p;
        أرجع !!p;
}

صحيح fgetspent_sane(FILE *وهڪرو, بنية spwd **sp) {
        أكد(وهڪرو);
        أكد(sp);

        errno = 0;
        بنية spwd *s = fgetspent(وهڪرو);
        إذا (!s && !اندريون_سيٽ(errno, 0, ENOENT))
                أرجع -errno;

        *sp = s;
        أرجع !!s;
}

صحيح fgetgrent_sane(FILE *وهڪرو, بنية group **gr) {
        أكد(وهڪرو);
        أكد(gr);

        errno = 0;
        بنية group *g = fgetgrent(وهڪرو);
        إذا (!g && !اندريون_سيٽ(errno, 0, ENOENT))
                أرجع -errno;

        *gr = g;
        أرجع !!g;
}

#إذا ENABLE_GSHADOW
صحيح fgetsgent_sane(FILE *وهڪرو, بنية sgrp **sg) {
        أكد(وهڪرو);
        أكد(sg);

        errno = 0;
        بنية sgrp *s = fgetsgent(وهڪرو);
        إذا (!s && !اندريون_سيٽ(errno, 0, ENOENT))
                أرجع -errno;

        *sg = s;
        أرجع !!s;
}
#جيڪڏهن_پورو

صحيح is_this_me(ثابت حرف *username) {
        uid_t uid;
        صحيح r;

        /* Checks if the specified username is our current one. Passed string might be a UID or a user name. */

        r = get_user_creds(username, /* flags= */ USER_CREDS_ALLOW_MISSING, عدم, &uid, عدم, عدم, عدم);
        إذا (r < 0)
                أرجع r;

        أرجع uid == getuid();
}

ثابت حرف* get_home_پاڙ(فراغ) {
        /* For debug purposes allow overriding where we look for home dirs */
        ثابت حرف *e = secure_getenv("SYSTEMD_HOME_ROOT");
        إذا (e && رستو_is_absolute(e) && رستو_is_normalized(e))
                أرجع e;

        أرجع "/home";
}

ساكن صحيح copy_struct_passwd(ثابت بنية passwd *pw, بنية passwd **ret) {
        أكد(pw);
        أكد(ret);

        size_t need_بائيٽون = حجم(بنية passwd)
                + strlen_ptr(pw->pw_name) + 1
                + strlen_ptr(pw->pw_passwd) + 1
                + strlen_ptr(pw->pw_gecos) + 1
                + strlen_ptr(pw->pw_dir) + 1
                + strlen_ptr(pw->pw_shell) + 1;

        حرف *buf = يادگيري_ڏيو(need_بائيٽون);
        إذا (!buf)
                أرجع -ENOMEM;

        بنية passwd *newpw = (بنية passwd *) buf;

        /* The layout in our buffer:
         * struct passwd, and then individual strings. */
        حرف *p = buf + حجم(بنية passwd);

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
        أرجع 0;
}

/* Iterate the given list of passwd-format files looking for an entry matching the predicate (by
 * name if 'name' is non-NULL and by 'uid' if valid). Returns -ESRCH if no entry is found. */
صحيح lookup_pwent_اندريون_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                uid_t uid,
                بنية passwd **ret) {

        صحيح r;

        أكد(files);
        أكد(name || uid_is_valid(uid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = عدم;
                بنية passwd *pw;

                r = fopen_unlocked(*fname, "re", &f);
                إذا (r == -ENOENT)
                        استمر;
                إذا (r < 0)
                        أرجع r;

                طالما ((r = fgetpwent_sane(f, &pw)) > 0) {
                        إذا (name && !streq_ptr(pw->pw_name, name))
                                استمر;
                        إذا (uid_is_valid(uid) && pw->pw_uid != uid)
                                استمر;
                        إذا (ret)
                                أرجع copy_struct_passwd(pw, ret);
                        أرجع 0;
                }
                إذا (r < 0)
                        أرجع r;
        }

        أرجع -ESRCH;
}

#إذا !BUILD_STATIC

ساكن size_t getpw_بفر_سائيز(فراغ) {
        ڊگهو bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        أرجع bufsize <= 0 ? 4096U : (size_t) bufsize;
}

ساكن bool errno_is_user_doesnt_exist(صحيح error) {
        /* See getpwnam(3) and getgrnam(3): those codes and others can be returned if the user or group are
         * not found. */
        أرجع اندريون_سيٽ(مطلق_قدر(error), ENOENT, ESRCH, EBADF, EPERM);
}

#جيڪڏهن_پورو

صحيح getpwnam_malloc(ثابت حرف *name, بنية passwd **ret) {
        /* For static builds use a very simplistic implementation that looks at 'passwd' in /etc/
         * and /usr/lib/, and for non-static builds use NSS. The first matching entry wins. The
         * same logic is implemented in getpwnam_malloc, getpwuid_malloc, getgrnam_malloc, and
         * getgrgid_malloc. */

        إذا (isempty(name))
                أرجع -EINVAL;

#إذا BUILD_STATIC
        أرجع lookup_pwent_اندريون_files(PASSWD_FILES, name, UID_INVALID, ret);
#وإلا
        size_t bufsize = getpw_بفر_سائيز();
        صحيح r;

        /* A wrapper around getpwnam_r() that allocates the necessary buffer on the heap. The caller must
         * free() the returned structure! */

        لكل (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية passwd)));
                buf = malloc0(ALIGN(حجم(بنية passwd)) + bufsize);
                إذا (!buf)
                        أرجع -ENOMEM;

                بنية passwd *pw = عدم;
                r = getpwnam_r(name, buf, (حرف*) buf + ALIGN(حجم(بنية passwd)), bufsize, &pw);
                إذا (r == 0) {
                        إذا (pw) {
                                إذا (ret)
                                        *ret = TAKE_PTR(buf);
                                أرجع 0;
                        }

                        أرجع -ESRCH;
                }

                أكد(r > 0);

                /* getpwnam() may fail with ENOENT if /etc/passwd is missing.  For us that is equivalent to
                 * the name not being defined. */
                إذا (errno_is_user_doesnt_exist(r))
                        أرجع -ESRCH;
                إذا (r != ERANGE)
                        أرجع -r;

                إذا (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية passwd)))
                        أرجع -ENOMEM;
                bufsize *= 2;
        }
#جيڪڏهن_پورو
}

صحيح getpwuid_malloc(uid_t uid, بنية passwd **ret) {
        إذا (!uid_is_valid(uid))
                أرجع -EINVAL;

#إذا BUILD_STATIC
        أرجع lookup_pwent_اندريون_files(PASSWD_FILES, /* name= */ عدم, uid, ret);
#وإلا
        size_t bufsize = getpw_بفر_سائيز();
        صحيح r;

        لكل (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية passwd)));
                buf = malloc0(ALIGN(حجم(بنية passwd)) + bufsize);
                إذا (!buf)
                        أرجع -ENOMEM;

                بنية passwd *pw = عدم;
                r = getpwuid_r(uid, buf, (حرف*) buf + ALIGN(حجم(بنية passwd)), bufsize, &pw);
                إذا (r == 0) {
                        إذا (pw) {
                                إذا (ret)
                                        *ret = TAKE_PTR(buf);
                                أرجع 0;
                        }

                        أرجع -ESRCH;
                }

                أكد(r > 0);

                إذا (errno_is_user_doesnt_exist(r))
                        أرجع -ESRCH;
                إذا (r != ERANGE)
                        أرجع -r;

                إذا (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية passwd)))
                        أرجع -ENOMEM;
                bufsize *= 2;
        }
#جيڪڏهن_پورو
}

ساكن صحيح copy_struct_گروهه(ثابت بنية group *gr, بنية group **ret) {
        أكد(gr);
        أكد(ret);

        size_t need_بائيٽون = حجم(بنية group)
                + strlen_ptr(gr->gr_name) + 1
                + strlen_ptr(gr->gr_passwd) + 1,
                n_mem = 0;
        STRV_FOREACH(s, gr->gr_mem) {
                need_بائيٽون += حجم(حرف*) + لڙي_ڊيگهه(*s) + 1;
                n_mem++;
        }
        need_بائيٽون += حجم(حرف*);  /* NULL terminator for gr_mem */

        حرف *buf = يادگيري_ڏيو(need_بائيٽون);
        إذا (!buf)
                أرجع -ENOMEM;

        بنية group *newgr = (بنية group *) buf;

        /* The layout in our buffer:
         * struct group, ->gr_mem pointers terminated by NULL, ->gr_name, ->gr_passwd, ->gr_mem items */
        /* The ->gr_mem array is first, because it needs alignment. */
        assert_cc(حجم(بنية group) % alignof(حرف*) == 0);

        حرف *p = buf + حجم(بنية group) + (n_mem + 1) * حجم(حرف*);

        newgr->gr_name = p;
        p = stpcpy(p, strempty(gr->gr_name)) + 1;

        newgr->gr_passwd = p;
        p = stpcpy(p, strempty(gr->gr_passwd)) + 1;

        newgr->gr_gid = gr->gr_gid;

        newgr->gr_mem = (حرف**) (buf + حجم(بنية group));
        لكل (size_t i = 0; i < n_mem; i++) {
                newgr->gr_mem[i] = p;
                p = stpcpy(p, gr->gr_mem[i]) + 1;
        }
        newgr->gr_mem[n_mem] = عدم;

        *ret = newgr;
        أرجع 0;
}

/* See lookup_pwent_in_files() for the analogous passwd-file version. */
صحيح lookup_grent_اندريون_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                بنية group **ret) {

        صحيح r;

        أكد(files);
        أكد(name || gid_is_valid(gid));

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = عدم;
                بنية group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                إذا (r == -ENOENT)
                        استمر;
                إذا (r < 0)
                        أرجع r;

                طالما ((r = fgetgrent_sane(f, &gr)) > 0) {
                        إذا (name && !streq_ptr(gr->gr_name, name))
                                استمر;
                        إذا (gid_is_valid(gid) && gr->gr_gid != gid)
                                استمر;
                        إذا (ret)
                                أرجع copy_struct_گروهه(gr, ret);
                        أرجع 0;
                }
                إذا (r < 0)
                        أرجع r;
        }

        أرجع -ESRCH;
}

صحيح sysconf_ngroups_max(فراغ) {
        /* Query sysconf _SC_NGROUPS_MAX. Returns an int because the expected value is 64k
         * and later on this is used as an int with various glibc consumers. */

        errno = 0;
        ڊگهو ngroups_max = sysconf(_SC_NGROUPS_MAX);
        إذا (ngroups_max <= 0)
                أرجع errno_or_else(EOPNOTSUPP);
        إذا (ngroups_max > وڌ_کان_وڌ_عدد)
                أرجع -ERANGE;
        أرجع ngroups_max;
}

ssize_t lookup_groups_اندريون_files(
                حرف * ثابت *files,
                ثابت حرف *name,
                gid_t gid,
                gid_t **ret) {

        _cleanup_free_ gid_t *arr = عدم;
        ssize_t n_arr = 0;
        صحيح r;

        أكد(files);
        أكد(name);
        أكد(gid_is_valid(gid));

        صحيح ngroups_max = sysconf_ngroups_max();
        إذا (ngroups_max < 0)
                أرجع ngroups_max;

        إذا (!GREEDY_REALLOC(arr, n_arr + 1))
                أرجع -ENOMEM;
        arr[n_arr++] = gid;

        STRV_FOREACH(fname, files) {
                _cleanup_fclose_ FILE *f = عدم;
                بنية group *gr;

                r = fopen_unlocked(*fname, "re", &f);
                إذا (r == -ENOENT)
                        استمر;
                إذا (r < 0)
                        أرجع r;

                طالما ((r = fgetgrent_sane(f, &gr)) > 0) {
                        إذا (gid == gr->gr_gid)
                                استمر;

                        /* Deduplicate groups, in case the group is doubly defined in the same or
                         * different files. */
                        إذا (gid_list_has(arr, n_arr, gr->gr_gid))
                                استمر;

                        إذا (!strv_شامل_آهي(gr->gr_mem, name))
                                استمر;

                        /* Do not allow more than _SC_NGROUPS_MAX. */
                        إذا (n_arr >= ngroups_max)
                                أرجع -ENOBUFS;

                        إذا (!GREEDY_REALLOC(arr, n_arr + 1))
                                أرجع -ENOMEM;
                        arr[n_arr++] = gr->gr_gid;
                }
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret)
                *ret = TAKE_PTR(arr);
        أرجع n_arr;
}

صحيح getgrouplist_malloc(ثابت حرف *user, gid_t gid, gid_t **ret) {
#إذا BUILD_STATIC
        أرجع lookup_groups_اندريون_files(گروهه_FILES, user, gid, ret);
#وإلا
        صحيح ngroups_max = sysconf_ngroups_max();
        إذا (ngroups_max < 0)
                أرجع ngroups_max;

        _cleanup_free_ gid_t *gids = new(gid_t, ngroups_max);
        إذا (!gids)
                أرجع -ENOMEM;

        صحيح k = ngroups_max;
        إذا (getgrouplist(user, gid, gids, &k) < 0)
                أرجع -EIO;  /* getgrouplist does not set errno */

        إذا (ret)
                *ret = TAKE_PTR(gids);
        أرجع k;
#جيڪڏهن_پورو
}

صحيح initgroups_wrapper(ثابت حرف *user, gid_t gid) {
#إذا BUILD_STATIC
        _cleanup_free_ gid_t *groups = عدم;
        صحيح r, n_groups;

        n_groups = getgrouplist_malloc(user, gid, &groups);
        إذا (n_groups <= 0)
                أرجع n_groups;

        /* Try to set the maximum number of groups the kernel can handle. This is what glibc does. */
        لكل (; n_groups > 0; n_groups--) {
                r = RET_NERRNO(setgroups(n_groups, groups));
                إذا (r != -EINVAL)
                        اكسر;
        }

        أرجع r;
#وإلا
        أرجع RET_NERRNO(initgroups(user, gid));
#جيڪڏهن_پورو
}

#إذا !BUILD_STATIC

ساكن size_t getgr_بفر_سائيز(فراغ) {
        ڊگهو bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
        أرجع bufsize <= 0 ? 4096U : (size_t) bufsize;
}

#جيڪڏهن_پورو

صحيح getgrnam_malloc(ثابت حرف *name, بنية group **ret) {
        إذا (isempty(name))
                أرجع -EINVAL;

#إذا BUILD_STATIC
        أرجع lookup_grent_اندريون_files(گروهه_FILES, name, GID_INVALID, ret);
#وإلا
        size_t bufsize = getgr_بفر_سائيز();
        صحيح r;

        لكل (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية group)));
                buf = malloc0(ALIGN(حجم(بنية group)) + bufsize);
                إذا (!buf)
                        أرجع -ENOMEM;

                بنية group *gr = عدم;
                r = getgrnam_r(name, buf, (حرف*) buf + ALIGN(حجم(بنية group)), bufsize, &gr);
                إذا (r == 0) {
                        إذا (gr) {
                                إذا (ret)
                                        *ret = TAKE_PTR(buf);
                                أرجع 0;
                        }

                        أرجع -ESRCH;
                }

                أكد(r > 0);

                إذا (errno_is_user_doesnt_exist(r))
                        أرجع -ESRCH;
                إذا (r != ERANGE)
                        أرجع -r;

                إذا (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية group)))
                        أرجع -ENOMEM;
                bufsize *= 2;
        }
#جيڪڏهن_پورو
}

صحيح getgrgid_malloc(gid_t gid, بنية group **ret) {
        إذا (!gid_is_valid(gid))
                أرجع -EINVAL;

#إذا BUILD_STATIC
        أرجع lookup_grent_اندريون_files(گروهه_FILES, /* name= */ عدم, gid, ret);
#وإلا
        size_t bufsize = getgr_بفر_سائيز();
        صحيح r;

        لكل (;;) {
                _cleanup_free_ فراغ *buf = عدم;

                /* Silence static analyzers */
                أكد(bufsize <= SIZE_MAX - ALIGN(حجم(بنية group)));
                buf = malloc0(ALIGN(حجم(بنية group)) + bufsize);
                إذا (!buf)
                        أرجع -ENOMEM;

                بنية group *gr = عدم;
                r = getgrgid_r(gid, buf, (حرف*) buf + ALIGN(حجم(بنية group)), bufsize, &gr);
                إذا (r == 0) {
                        إذا (gr) {
                                إذا (ret)
                                        *ret = TAKE_PTR(buf);
                                أرجع 0;
                        }

                        أرجع -ESRCH;
                }

                أكد(r > 0);

                إذا (errno_is_user_doesnt_exist(r))
                        أرجع -ESRCH;
                إذا (r != ERANGE)
                        أرجع -r;

                إذا (bufsize > SIZE_MAX/2 - ALIGN(حجم(بنية group)))
                        أرجع -ENOMEM;
                bufsize *= 2;
        }
#جيڪڏهن_پورو
}
